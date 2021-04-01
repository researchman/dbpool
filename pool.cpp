#include "pool.h"
#include <thread>
#include <chrono>
#include <algorithm>

namespace zdb{
    std::mutex g_mtx;
    std::vector<async_sql*> g_async_list;

    db_pool::db_pool()
    {
        g_async_list.reserve(1<<15);
        m_async_conn = nullptr;
    }

    db_pool::~db_pool()
    {
        close();
    }

    bool db_pool::create(const db_pool_setting& cfg, std::string& error)
    {
        return create(cfg, true, error);
    }

    bool db_pool::create(const db_pool_setting& cfg, bool async, std::string& error)
    {
        std::unique_lock<std::mutex> lock(m_mtx);

        if(cfg.m_size < db_pool_min_size){
            error = "db pool's size is too small";
            return false;
        }

        if(cfg.m_size > db_pool_max_size){
            error = "db pool's size is too big";
            return false;
        }

        m_pool_setting = cfg;

        if((int)m_idle_list.size() < m_pool_setting.m_size){
            for(int i = 0; i < m_pool_setting.m_size; ++i){
                ptr_connection conn = create_connection(error);
                if(!conn){
                    return false;
                }
            }
        }

        if(async || !m_running){
            create_async_connection();
            start_async_thread();
        }

        return true;
    }

    ptr_connection db_pool::create_connection(std::string& error, bool is_temp = false)
    {
        db_setting conn_setting = static_cast<db_setting>(m_pool_setting);
        ptr_connection conn = std::make_shared<zdb::connection>(is_temp);

        if(!conn->connect(conn_setting, error)){
            conn.reset();
            conn = nullptr;
            return 0;
        }

        if(!conn_setting.m_stmt_sql.empty()){
            if(!conn->prepare_stmt(conn_setting.m_stmt_sql.c_str(), error)){
                conn.reset();
                conn = nullptr;
                return 0;
            }
        }

        m_idle_list.push_back(conn);

        return conn;
    }

    void db_pool::close()
    {
        std::lock_guard<std::mutex> lock(m_mtx);

        stop_async_thread();

        destroy_async_connection();

        ptr_connection conn = nullptr;
        for(auto it : m_work_list){
            conn = it;
            conn->close();
            conn.reset();
            conn = nullptr;
        }
        m_work_list.erase(m_work_list.begin(), m_work_list.end());
        m_work_list.clear();

        for(auto it : m_idle_list){
            conn = it;
            conn->close();
            conn.reset();
            conn = nullptr;
        }
        m_idle_list.erase(m_idle_list.begin(), m_idle_list.end());
        m_idle_list.clear();
    }

    ptr_connection db_pool::get_connect(std::string& error)
    {
        std::lock_guard<std::mutex> lock(m_mtx);

        ptr_connection ptr_conn = nullptr;

        if(m_idle_list.size() > 0){
            ptr_conn = *(m_idle_list.begin());
            m_idle_list.pop_front();
            m_work_list.push_back(ptr_conn);
        }else{
            if(m_idle_list.size() < db_pool_max_size){
                ptr_conn = create_connection(error, true);
                if(!ptr_conn){
                    error = "failed connect to database";
                    return 0;
                }

                m_idle_list.pop_front();
                m_work_list.push_back(ptr_conn);
            }else{
                error = "the count of db connection is beyond the max capacity";
                return 0;
            }
        }

        if(ptr_conn){
            if(ptr_conn->ping(error) != 0){
                db_setting cfg = static_cast<db_setting>(m_pool_setting);
                ptr_conn->close();
                int ret = ptr_conn->connect(cfg, error);
                if(ret < 0){
                    back(ptr_conn);
                    error = "failed connect to database";
                    return 0;
                }
            }
        }

        return ptr_conn;
    }

    void db_pool::back(ptr_connection ptr_conn)
    {
        std::lock_guard<std::mutex> lock(m_mtx);

        if(0 == ptr_conn){
            return;
        }

        m_work_list.remove(ptr_conn);

        if(ptr_conn->is_temp()){
            ptr_conn.reset();
            ptr_conn = nullptr;
        }else{
            m_idle_list.push_back(ptr_conn);
        }
    }

    bool db_pool::query(const char* sql, result_set& res, std::string& error)
    {
        ptr_connection conn = get_connect(error);
        if(conn == nullptr){
            return false;
        }

        MYSQL_RES* raw_res = conn->query(sql, error);
        back(conn);

        return res.bind(raw_res, error);
    }

    MYSQL_RES* db_pool::query(const char* sql, std::string& error)
    {
        ptr_connection conn = get_connect(error);
        if(0 == conn){
            return nullptr;
        }

        MYSQL_RES* res = conn->query(sql, error);
        back(conn);
        
        return res;
    }

    my_ulonglong db_pool::execute_affect_rows(const char* sql, std::string& error)
    {
        ptr_connection conn = get_connect(error);
        if(0 == conn){
            return 0;
        }

        my_ulonglong ret = conn->execute_affect_rows(sql, error);
        back(conn);

        return ret;
    }
    my_ulonglong db_pool::execute_real_affect_rows( const char *sql, std::string& error)
    {
        ptr_connection conn = get_connect(error);
        if(0 == conn){
            return 0;
        }

        my_ulonglong ret = conn->execute_real_affect_rows(sql, error);
        back(conn);

        return ret;
    }

    void db_pool::create_async_connection()
    {
        std::string error = "";

        destroy_async_connection();

        db_setting cfg = static_cast<db_setting>(m_pool_setting);
        ptr_connection conn = std::make_shared<connection>();
        if(!conn->connect(cfg, error)){
            conn.reset();
            conn = nullptr;
            return;
        }

        m_async_conn = conn;
    }

    void db_pool::destroy_async_connection()
    {
        if(m_async_conn){
            m_async_conn->close();
            m_async_conn.reset();
            m_async_conn = nullptr;
        }
    }

    void db_pool::start_async_thread()
    {
        m_running = true;
        std::thread thr(std::bind(&db_pool::async_thread_func, this, this));
        thr.detach();
    }

    void db_pool::stop_async_thread()
    {
        if(!m_running){
            return;
        }

        m_running = false;

        while(!m_is_exited){
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        {
            std::unique_lock<std::mutex> lock(g_mtx);
            std::for_each(g_async_list.begin(), g_async_list.end(),[](async_sql* sql){
                delete sql;
            });

            g_async_list.clear();
        }
    }

    void db_pool::async_thread_func(db_pool* owner)
    {
        while(owner->m_running){
            if(!g_async_list.empty()){
                std::vector<async_sql*> async_list;
                async_list.reserve(1<<15);

                {
                    std::unique_lock<std::mutex> lock(g_mtx);
                    async_list.swap(g_async_list);
                }

                for(auto it : async_list){
                    execute_async_sql(it);
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        m_is_exited = true;
    }

    void db_pool::execute_async_sql(async_sql* ptr_data)
    {
        std::string error = "";
        if(!ptr_data){
            return;
        }

        if(m_async_conn){
            my_ulonglong res = 0;
            res = m_async_conn->execute_real_affect_rows(ptr_data->m_sql.c_str(), error);

            if((my_ulonglong)-1 == res){
                if(m_async_conn->ping(error) != 0){
                    db_setting setting = static_cast<db_setting>(m_pool_setting);
                    m_async_conn->close();
                    if(m_async_conn->connect(setting, error) < 0){
                        error = "failed connect to database.";
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    }
                }

                ptr_data->m_failed_count++;
                if(ptr_data->m_failed_count < MAX_ASYNC_EXEC_FAILED_COUNT){
                    std::unique_lock<std::mutex> lock(g_mtx);
                    g_async_list.push_back(ptr_data);
                    return;
                }
            }
        }

        delete ptr_data;
        ptr_data = nullptr;
    }

    bool db_pool::push_async(const std::string& sql)
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        g_async_list.push_back(new async_sql(std::move(sql)));

        return true;
    }
}