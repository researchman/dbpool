#ifndef zdb_common_h
#define zdb_common_h
#include <string>

namespace zdb{
    struct db_setting{
        std::string m_host;     // db server's ip
        std::string m_user;     // user name    
        std::string m_pwd;      // password
        std::string m_dbname;   // db name
        std::string m_charset;  // db's charset
        std::string m_stmt_sql; // stmt sql
        
        size_t m_port;  // port,default is 3306

        int m_timeout;

        db_setting(): m_host(""), m_user(""), m_pwd(""), m_dbname(""), m_charset(""), m_stmt_sql(""), m_port(3306), m_timeout(0)
        {}

        db_setting(const std::string host, const std::string user, const std::string pwd, const std::string dbname, const std::string charset, const size_t port)
            : m_host(host)
            , m_user(user)
            , m_pwd(pwd)
            , m_dbname(dbname)
            , m_charset(charset)
            , m_port(port)
            , m_timeout(0)
            {
                m_stmt_sql = "";
            }

        void set_timeout(const int& val)
        {
            m_timeout = val;
        }

        void set_stmtsql(const std::string& val)
        {
            m_stmt_sql = val;
        }
    };

    struct db_pool_setting: public db_setting{
        int m_size;     // 连接池大小
        int m_min_size; // 最小连接数
        int m_max_size; // 最大连接数

        db_pool_setting(): m_size(10), m_min_size(1), m_max_size(60)
        {}

        db_pool_setting(const int size, const int min_size, const int max_size)
            : m_size(size)
            , m_min_size(min_size)
            , m_max_size(max_size)
            {}
    }
}

#endif