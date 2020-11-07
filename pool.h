#ifndef zdb_pool_h
#define zdb_pool_h
#include <boost/atomic.hpp>
#include <boost/thread/thread.hpp>
#include <boost/serialization/singleton.hpp>
#include <list>
#include <string>
#include <mutex>
#include "common.h"
#include "connection.h"

namespace zdb{
    class db_pool{
        private:
        std::list<connection*> m_work_conn_list;
        std::list<connection*> m_idle_conn_list;

        db_pool_setting m_pool_setting;
        std::mutex m_mtx;
        std::atomic<bool> m_running;
        std::atomic<bool> m_is_exited;
        connection* m_async_conn;

        private:
        connection* create_connection(std::string& error, bool is_temp = false);
        void create_async_connection();
        void destroy_async_connection();
        void start_async_connection();
        void stop_async_connection();

        protected:
        void async_thread_func(db_pool* owner);

        public:
        db_pool();
        ~db_pool();

        bool create(const db_pool_setting& cfg, std::string& error);
        void close();
        connection* get_connect(std::string& error);
        void back(connection* pconn);

        bool push_async(const std::string& sql);
        bool query(const char* sql, result_set& res, std::string& error);
        MYSQL_RES* query(const char* sql, std::string& error);
        my_ulonglong execute_sql(const char* sql, std::string& error);
    };
};

typedef boost::serialization::singleton<zdb::db_pool> singleton_db_pool;
#define sdb_pool singleton_db_pool::get_mutable_instance()
#define sdb_pool_const singleton_db_pool::get_const_instance()

#endif