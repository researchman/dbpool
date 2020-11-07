#ifndef zdb_connection_h
#define zdb_connection_h
#include <memory>
#include <mysql.h>
#include "common.h"
#include "result_set.h"

namespace zdb{
    class connection: public std::enabled_from_this<connection>{
        private:
        MYSQL* m_conn;
        MYSQL_STMT* m_stmt;     //
        result_set m_res;
        bool m_temp_flag;

        public:
        connection(bool temp = false);

        ~connection();

        bool connect(const zdb::db_setting& cfg, std::string& error);

        void close();

        result_set* execute_query(const char* sz_sql, std::string& error);
        MYSQL_RES* query(const char* sz_sql, std::string& error);
        my_ulonglong excute_affect_rows(const char* sz_sql, std::string& error);
        my_ulonglong excute_real_affect_rows(const char* sz_sql, std::string& error);
        my_ulonglong get_last_inserted_id(std::string& error);

        int ping(std::string& error);

        int auto_commit(bool mode, std::string& error);
        int commit(std::string& error);
        int roll_back(std::string& error);

        bool prepare_stmt(const char* sz_sql, std::string& error);
        bool stmt_execute(MYSQL_BIND* binds, int64_t* pid, std::string& error);
        void stmt_close();

        const char* get_last_error();

        bool is_temp()
        {
            return m_temp_flag;
        }

        std::shared_ptr<connection> get()
        {
            return shared_from_this();
        }

        bool is_open()
        {
            return (m_conn == NULL)?false:true;
        }
    };
};

#endif