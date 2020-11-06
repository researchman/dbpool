#include "connection.h"

namespace zdb{
    connection::connection(bool temp)
    {
        m_conn = mysql_init(NULL);
        m_stmt = 0;
        m_temp_flag = temp;
    }

    connection::~connection()
    {
        close();
    }

    bool connection::connect(const zdb::db_setting& cfg, std::string& error)
    {
        // set timeout
        if(mysql_options(m_conn, MYSQL_OPT_CONNECT_TIMEOUT, &cfg.m_timeout) != 0){
            error = "failed to call mysql_options, last_error=";
            error += get_last_error();
            return false;
        }
        
        //set charset
        if(mysql_set_charset_set(m_conn, cfg.m_charset.c_str()) != 0){
            error = "failed to call mysql_set_charset_set, last_error=";
            error += get_last_error();
            return false;
        }

        if(!mysql_real_connect(m_conn, cfg.m_host.c_str(), cfg.m_user.c_str(), cfg.m_pwd.c_str(), cfg.m_dbname.c_str(), cfg.m_pwd, NULL, 0)){
            error = "failed to call mysql_real_connect, last_error=";
            error += get_last_error();
            return false;
        }

        return true;
    }

    void connection::close()
    {
        stmt_close();
        if(m_conn){
            mysql_close(m_conn);
            m_conn = NULL;
        }
    }

    result_set* execute_query(const char* sz_sql, std::string& error)
    {
        if(!is_open()){
            error = "not connected to database";
            return nullptr;
        }

        if(mysql_query(m_conn, sz_sql)){
            error = "failed to call mysql_query! last_error=";
            error += get_last_error();
            return nullptr;
        }

        if(!m_res.bind()){
            
        }
    }

    MYSQL_RES* query(const char* sz_sql, std::string& error)
    {

    }

    const char* connection::get_last_error()
    {
        if(NULL == m_conn)
        {
            return "";
        }

        return mysql_error(m_conn);
    }
}