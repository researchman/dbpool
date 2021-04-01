#include "connection.h"

namespace zdb{
    connection::connection(bool temp)
    {
        m_conn = mysql_init(NULL);
        m_stmt = 0;
        m_tmp_flag = temp;
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
        if(mysql_set_character_set(m_conn, cfg.m_charset.c_str()) != 0){
            error = "failed to call mysql_set_charset_set, last_error=";
            error += get_last_error();
            return false;
        }

        if(!mysql_real_connect(m_conn, cfg.m_host.c_str(), cfg.m_user.c_str(), cfg.m_pwd.c_str(), cfg.m_dbname.c_str(), cfg.m_port, NULL, 0)){
            error = "failed to call mysql_real_connect, last_error=";
            error += get_last_error();
            return false;
        }
        mysql_query(m_conn, "set names utf8");
        // 重连
        char value = 1;
        mysql_options(m_conn, MYSQL_OPT_RECONNECT, &value);

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

    result_set* connection::execute_query(const char* sql, std::string& error)
    {
        if(!is_open()){
            error = "not connected to database";
            return nullptr;
        }

        if(mysql_query(m_conn, sql)){
            error = "failed to call mysql_query! last_error=";
            error += get_last_error();
            return nullptr;
        }

        if(!m_res.bind(mysql_store_result(m_conn), error)){
            return 0;
        }

        return &m_res;
    }

    MYSQL_RES* connection::query(const char* sql, std::string& error)
    {
        if(!is_open()){
            error = "not connected to database.";
            return 0;
        }

        if(mysql_query(m_conn, sql)){
            error = "failed to call mysql_query, last_error=";
            error += get_last_error();
            return 0;
        }

        return mysql_store_result(m_conn);
    }

    my_ulonglong connection::execute_affect_rows(const char* sql, std::string& error)
    {
        if(!is_open()){
            error = "not connected to database.";
            return 1;
        }

        if(mysql_query(m_conn, sql)){
            error = "failed to call mysql_query, last_error=";
            error += get_last_error();
            return 0;
        }

        return mysql_affected_rows(m_conn);
    }

    my_ulonglong connection::execute_real_affect_rows(const char* sql, std::string& error)
    {
        if(!is_open()){
            error = "not connected to database.";
            return 1;
        }

        unsigned long len = strlen(sql);

        if(mysql_real_query(m_conn, sql, len)){
            error = "failed to call mysql_real_query, last_error=";
            error += get_last_error();
            return 2;
        }
        return mysql_affected_rows(m_conn);
    }

    int connection::ping(std::string& error)
    {
        if(0 == m_conn){
            error = "not connected to database.";
            return 1;
        }

        int ret = -1;
        ret = mysql_ping(m_conn);
        if(ret){
            error = "failed to call mysql_ping, last_error=";
            error += get_last_error();
        }

        return ret;
    }

    my_ulonglong connection::get_last_inserted_id(std::string& error)
    {
        if(!is_open()){
            error = "not connected to database.";
            return 1;
        }

        return mysql_insert_id(m_conn);
    }
    
    int connection::auto_commit(bool mode, std::string& error)
    {
        if(0 == m_conn){
            error = "not connected to database.";
            return 1;
        }

        return mysql_autocommit(m_conn, mode);
    }

    int connection::commit(std::string& error)
    {
        if(0 == m_conn){
            error = "not connected to database.";
            return 1;
        }

        return mysql_commit(m_conn);
    }

    int connection::roll_back(std::string& error)
    {
        if(0 == m_conn){
            error = "not connected to database.";
            return 1;
        }

        return mysql_rollback(m_conn);
    }

    bool connection::prepare_stmt(const char* sql, std::string& error)
    {
        if(!is_open()){
            error = "not connected to database.";
            return 1;
        }

        stmt_close();

        m_stmt = mysql_stmt_init(m_conn);

        if(mysql_stmt_prepare(m_stmt, sql, strlen(sql)) != 0){
            error = "failed to call mysql_stmt_prepare, last_error=";
            error += get_last_error();
            return false;
        }

        return true;
    }

    bool connection::stmt_execute(MYSQL_BIND* binds, int64_t* pid, std::string& error)
    {
        if(NULL == m_stmt){
            error = "bind error, m_stmt = null";
            return false;
        }

        if(mysql_stmt_bind_param(m_stmt, binds) != 0){
            error = "failed to call mysql_stmt_bind_param, last_error=";
            error += get_last_error();
            return false;
        }

        if(mysql_stmt_execute(m_stmt) != 0){
            error = "failed to call mysql_stmt_excute, last_error=";
            error += get_last_error();
            return false;
        }

        if(pid){
            *pid = mysql_stmt_insert_id(m_stmt);
        }

        return true;
    }

    void connection::stmt_close()
    {
        if(m_stmt){
            mysql_stmt_close(m_stmt);
            m_stmt = 0;
        }
    }

    const char* connection::get_last_error()
    {
        if(NULL == m_conn)
        {
            return nullptr;
        }

        return mysql_error(m_conn);
    }
}