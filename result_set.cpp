#include "result_set.h"
#include <string>

namespace zdb{
    result_set::result_set()
    {
        m_query_res = 0;
        m_cur_row = 0;
        m_field_count = 0;
    }

    result_set::~result_set()
    {
        close();
    }

    void result_set::create_filed_idx_list()
    {
        if(0 == m_query_res){
            return;
        }

        for(int i = 0;i < m_field_count; ++i){
            MYSQL_FIELD* ptr_field = mysql_fetch_field_direct(m_query_res, i);
            if(NULL == ptr_field){
                continue;
            }

            m_field_idx_list.insert(std::make_pair(ptr_field->name, i));
        }
    }

    bool result_set::bind(MYSQL_RES* res, std::string& error)
    {
        close();

        m_query_res = res;

        if(0 == m_query_res){
            error = "mysql_res is null";
            m_field_count = 0;
            return false;
        }

        m_field_count = mysql_num_fields(res);

        create_filed_idx_list();

        return true;
    }

    void result_set::close()
    {
        if(0 == m_query_res){
            return;
        }

        mysql_free_result(m_query_res);
        m_query_res = 0;
        m_cur_row = 0;
        m_field_count = 0;
        m_field_idx_list.erase(m_field_idx_list.begin(), m_field_idx_list.end());
        m_field_idx_list.clear();
    }

    bool result_set::seek(my_ulonglong offset, std::string& error)
    {
        if(0 == m_query_res){
            error = "result_set::m_query_res is not initialized.";
            return false;
        }

        mysql_data_seek(m_query_res, offset);

        return true;
    }

    bool result_set::get_next_record(std::string& error)
    {
        if(0 == m_query_res){
            error = "m_query_res is not initialized.";
            return false;
        }

        if((m_cur_row = mysql_fetch_row(m_query_res)) != NULL){
            return true;
        }else{
            error = "failed to call mysql_fetch_row.";
            return false;
        }

        return true;
    }

    my_ulonglong result_set::get_record_count(std::string& error)
    {
        if(0 == m_query_res){
            error = "m_query_res is not initialized.";
            return 2;
        }

        return mysql_num_rows(m_query_res);
    }

    bool result_set::get_field(int idx, char*& val, bool& is_null, std::string& error)
    {
        is_null = false;

        if(0 == m_cur_row){
            error = "m_cur_row is not initialized.";
            return false;
        }

        if(idx < 0 || idx >= m_field_count){
            error = "idx is invalid.";
            return false;
        }

        val = m_cur_row[idx];
        if(val == NULL || val[0] == 0){
            is_null = true;
        }

        return true;
    }

    bool result_set::get_field(int idx, int& val, std::string& error)
    {
        val = 0;
        char* ptr_field = 0;
        bool is_null = false;

        if(!get_field(idx, ptr_field, is_null, error)){
            return false;
        }

        if(!is_null){
            val = atoi(ptr_field);
        }

        return true;
    }

    bool result_set::get_field(int idx, unsigned int& val, std::string& error)
    {
        val = 0;
        char* ptr_field = 0;
        bool is_null = false;

        if(!get_field(idx, ptr_field, is_null, error)){
            return false;
        }

        if(!is_null){
            val = atoi(ptr_field);
        }

        return true;
    }
        
    bool result_set::get_field(int idx, long long& val, std::string& error)
    {
        val = 0;
        char* ptr_field = 0;
        bool is_null = false;

        if(!get_field(idx, ptr_field, is_null, error)){
            return false;
        }

        if(!is_null){
            val = _atoi64(ptr_field);
        }

        return true;
    }
        
    bool result_set::get_field(int idx, std::string& val, std::string& error)
    {
        val = "";
        char* ptr_field = 0;
        bool is_null = false;

        if(!get_field(idx, ptr_field, is_null, error)){
            return false;
        }

        if(!is_null){
            val = ptr_field;
        }

        return true;
    }

    bool result_set::get_field(int idx, char* val, int len, std::string& error)
    {
        memset(val, 0, len);

        char* ptr_field = 0;
        bool is_null = false;

        if(!get_field(idx, ptr_field, is_null, error)){
            return false;
        }

        if(!is_null){
            strncpy_s(val, sizeof(val), ptr_field, len);
        }

        return true;
    }

    bool result_set::get_field(int idx, bool& val, std::string& error)
    {
        val = false;
        char* ptr_field = 0;
        bool is_null = false;

        if(!get_field(idx, ptr_field, is_null, error)){
            return false;
        }

        if(!is_null){
            val = (atoi(ptr_field)==1?true:false);
        }

        return true;
    }

    bool result_set::get_field(int idx, float& val, std::string& error)
    {
        val = 0;
        char* ptr_field = 0;
        bool is_null = false;

        if(!get_field(idx, ptr_field, is_null, error)){
            return false;
        }

        if(!is_null){
            val = (float)atof(ptr_field);
        }

        return true;
    }
    bool result_set::get_field(int idx, double& val, std::string& error)
    {
        val = 0;
        char* ptr_field = 0;
        bool is_null = false;

        if(!get_field(idx, ptr_field, is_null, error)){
            return false;
        }

        if(!is_null){
            val = std::stod(ptr_field);
        }

        return true;
    }

    bool result_set::get_field(int idx, MYSQL_TIME& val, std::string& error)
    {
        db_helper::instance().init_mysql_time(val);
        char* ptr_field = 0;
        bool is_null = false;

        if(!get_field(idx, ptr_field, is_null, error)){
            return false;
        }

        if(!is_null){
            db_helper::instance().to_datetime(ptr_field, val);
        }

        return true;
    }
 
    int result_set::get_field_idx_by_name(const char* name, std::string& error)
    {
        auto fi = m_field_idx_list.find(name);
        if(fi == m_field_idx_list.end()){
            error = "failed to find field: ";
            error += name;
            return -1;
        }

        return fi->second;
    }

    bool result_set::get_field(const char* name, int& val, std::string& error)
    {
        int idx = get_field_idx_by_name(name, error);

        return (idx >= 0)?get_field(idx, val, error):false;
    }

    bool result_set::get_field(const char* name, long long& val, std::string& error)
    {
        int idx = get_field_idx_by_name(name, error);

        return (idx >= 0)?get_field(idx, val, error):false;
    }

    bool result_set::get_field(const char* name, unsigned int& val, std::string& error)
    {
        int idx = get_field_idx_by_name(name, error);

        return (idx >= 0)?get_field(idx, val, error):false;
    }

    bool result_set::get_field(const char* name, std::string& val, std::string& error)
    {
        int idx = get_field_idx_by_name(name, error);

        return (idx >= 0)?get_field(idx, val, error):false;
    }

    bool result_set::get_field(const char* name, char* val, int len, std::string& error)
    {
        int idx = get_field_idx_by_name(name, error);

        return (idx >= 0)?get_field(idx, val, len, error):false;
    }

    bool result_set::get_field(const char* name, bool& val, std::string& error)
    {
        int idx = get_field_idx_by_name(name, error);

        return (idx >= 0)?get_field(idx, val, error):false;
    }

    bool result_set::get_field(const char* name, float& val, std::string& error)
    {
        int idx = get_field_idx_by_name(name, error);

        return (idx >= 0)?get_field(idx, val, error):false;
    }

    bool result_set::get_field(const char* name, double& val, std::string& error)
    {
        int idx = get_field_idx_by_name(name, error);

        return (idx >= 0)?get_field(idx, val, error):false;
    }

    bool result_set::get_field(const char* name, MYSQL_TIME& val, std::string& error)
    {
        int idx = get_field_idx_by_name(name, error);

        return (idx >= 0)?get_field(idx, val, error):false;
    }

    int result_set::is_null(int idx, std::string& error)
    {
        if(0 == m_cur_row){
            error = "m_cur_row is not initialized.";
            return -1;
        }

        if(idx < 0 || idx >= m_field_count){
            error = "idx is invalid";
            return -1;
        }

        char* ptr_field = m_cur_row[idx];
        if(ptr_field == NULL || ptr_field[0] == 0){
            return 1;
        }

        return 0;
    }
}