#ifndef zdb_connection_h
#define zdb_connection_h
#include <memory>
#include <mysql.h>
#include "db_common.h"
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


        const char* get_last_error();

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