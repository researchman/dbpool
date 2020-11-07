#include "pool.h"

namespace zdb{
    std::mutex g_mtx;
    std::vector<async_sql*> g_async_list;

    db_pool::db_pool()
    {
        g_async_list.reserve(1<<15);
        m_async_conn = 0;
    }

    db_pool::~db_pool()
    {
        close();
    }

    bool create(const db_pool_setting& cfg, std::string& error)
    {
        return create(cfg, true, error);
    }

    bool create(const db_pool_setting& cfg, bool async, std::string& error)
    {
        std::unique_ptr<std::mutex> lock(m_mtx);

        if(cfg.m_size < db_pool_min_size){
            error = "db pool's size is too small";
            return false;
        }

        if(cfg.m_size > db_pool_max_size){
            error = "db pool's size is too big";
            return false;
        }

        m_pool_setting = cfg;

        if((int)m_idle_conn_list.size() < m_pool_setting.m_size){
            for(int i = 0; i < m_pool_setting.m_size; ++i){
                connection* conn = create_connection(error);
                if(!conn){
                    return false;
                }
            }
        }

        if(async || !m_running){
            create_async_connection();
            start_async_connection();
        }

        return true;
    }

    void close()
    {
        std::unique_ptr<std::mutex> lock(m_mtx);

        stop_async_connection();

        destroy_async_connection();

        connection* conn = 0;
        for(auto it : m_work_conn_list){
            conn = *it;
            conn->close();
            delete conn;
            conn = 0;
        }
        m_work_conn_list.erase(m_work_conn_list.begin(), m_work_conn_list.end());
        m_work_conn_list.clear();

        for(auto it : m_idle_conn_list){
            conn = *it;
            conn->close();
            delete conn;
            conn = 0;
        }
        m_idle_conn_list.erase(m_idle_conn_list.begin(), m_idle_conn_list.end());
        m_idle_conn_list.clear();
    }

    connection* get_connect(std::string& error)
    {
        std::unique_ptr<std::mutex> lock(m_mtx);

        connection* conn = 0;

        if(m_idle_conn_list.size() > 0){
            conn = *(m_idle_conn_list.begin());
            m_idle_conn_list.pop_front();
            m_work_conn_list.push_back(conn);
        }else{
            if(m_idle_conn_list.size() < db_pool_max_size){
                conn = create_connection(error, true);
                if(!conn){
                    error = "failed connect to database";
                    return 0;
                }

                m_idle_conn_list.pop_front();
                m_work_conn_list.push_back(conn);
            }else{
                error = "the count of db connection is beyond the max capacity";
                return 0;
            }
        }

        if(conn){
            if(conn->ping(error) != 0){
                db_setting cfg = static_cast<db_setting>(m_pool_setting);
                conn->close();
                int ret = conn->connect(cfg, error);
                if(ret < 0){
                    back(conn);
                    error = "failed connect to database";
                    return 0;
                }
            }
        }

        return conn;
    }

    void back(connection* pconn)
    {
        std::unique_ptr<std::mutex> lock(m_mtx);

        if(0 == pconn){
            return;
        }

        m_work_conn_list.remove(pconn);

        if(pconn->is_temp()){
            delete pconn;
            pconn = 0;
        }else{
            m_idle_conn_list.push_back(pconn);
        }
    }

    bool query(const char* sql, result_set& res, std::string& error)
    {
        MYSQL_RES* raw_res = query(sql, error);
        return res.bind(raw_res);
    }

    MYSQL_RES* query(const char* sql, std::string& error)
    {
        connection* conn = get_connect(error);
        if(0 == conn){
            return nullptr;
        }

        MYSQL_RES* res = conn->query(sql, error);
        back(conn);
        
        return res;
    }

    my_ulonglong execute_sql(const char* sql, std::string& error)
    {
        connection* conn = get_connect(error);
        if(0 == conn){
            return nullptr;
        }

        my_ulonglong ret = conn->execute_sql(sql, error);
        back(conn);

        return ret;
    }

    connection* create_connection(std::string& error, bool is_temp = false)
    {
        db_setting conn_setting = static_cast<db_setting>(m_pool_setting);
        connection* conn = new connection(is_temp);

        if(!conn->connect(conn_setting, error)){
            delete conn;
            conn = 0;
            return 0;
        }

        if(!conn_setting.m_stmt_sql.empty()){
            if(!conn->prepare_stmt(conn_setting.m_stmt_sql.c_str(), error)){
                delete conn;
                conn = 0;
                return 0;
            }
        }

        m_idle_conn_list.push_back(conn);

        return conn;
    }

    void create_async_connection()
    {
        std::string error = "";

        destroy_async_connection();

        db_setting cfg = static_cast<db_setting>(m_pool_setting);
        connection* conn = new connection();
        if(!conn->connect(cfg, error)){
            delete conn;
            conn = 0;
            return;
        }

        m_async_conn = conn;
    }

    void destroy_async_connection()
    {
        if(m_async_conn){
            m_async_conn->close();
            delete m_async_conn;
            m_async_conn = 0;
        }
    }

    void start_async_connection()
    {
        m_running = true;
        boost::thread conn_thread(boost::bind(&db_pool::async_thread_func, this, this));
        conn_thread.detach();
    }

    void stop_async_connection()
    {
        if(!m_running){
            return;
        }

        m_running = false;

        while(!m_is_exited){
            boost::this_thread::sleep(boost::posix_time::millisec(1));
        }

        {
            std::unique_lock<std::mutex> lock(g_mtx);
            std::for_each(g_async_list.begin(), g_async_list.end(),[](async_sql* sql){
                delete sql;
            });

            g_async_list.clear();
        }
    }

    void async_thread_func(db_pool* owner)
    {
        while(owner->m_running){
            if(!g_async_list.empty()){
                std::vector<async_sql*> async_list;
                async_sql.reserve(1<<15);

                {
                    std::unique_lock<std::mutex> lock(g_mtx);
                    async_sql.swap(g_async_list);
                }

                for(auto it : async_sql){
                    execute_async_sql(it);
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        m_is_exited = true;
    }

    bool push_async(const std::string& sql)
    {
        std::unique_ptr<std::mutex> lock(m_mtx);
        g_async_list.push_back(new async_sql(std::move(sql)));

        return true;
    }
}