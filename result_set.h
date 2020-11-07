#ifndef zdb_result_set_h
#define zdb_result_set_h
#include <mysql.h>
#include <string>
#include <map>
#include "common.h"
#include "helper.h"

namespace zdb{
    class result_set{
        private:
        MYSQL_RES* m_query_res;     // the result set of query
        MYSQL_ROW m_cur_row;    
        db_helper m_helper;    // the current rows
        int m_field_counts;
        std::map<std::string, int> m_field_idx_list;    //key is filed name, value is filed index

        private:
        void create_filed_idx_list();

        bool get_field(int idx, char*& val, bool& is_null, std::string& error);

        public:
        result_set();
        ~result_set();

        bool bind(MYSQL_RES* res, std::string& error);
        bool seek(my_ulonglong offset, std::string& error);
        bool get_next_record(std::string& error);
        my_ulonglong get_record_count(std::string& error);

        bool get_field(int idx, int& val, std::string& error);
        bool get_field(int idx, unsigned int& val, std::string& error);
        bool get_field(int idx, long long& val, std::string& error);
        bool get_field(int idx, std::string& val, std::string& error);
        bool get_field(int idx, char* val, int len, std::string& error);
        bool get_field(int idx, bool& val, std::string& error);
        bool get_field(int idx, float& val, std::string& error);
        bool get_field(int idx, double& val, std::string& error);
        bool get_field(int idx, MYSQL_TIME& val, std::string& error);
 
        int get_field_idx_by_name(const char* name, std::string& error);
        bool get_field(const char* name, int& val, std::string& error);
        bool get_field(const char* name, long long& val, std::string& error);
        bool get_field(const char* name, unsigned int& val, std::string& error);
        bool get_field(const char* name, std::string& val, std::string& error);
        bool get_field(const char* name, char* val, int len, std::string& error);
        bool get_field(const char* name, bool& val, std::string& error);
        bool get_field(const char* name, float& val, std::string& error);
        bool get_field(const char* name, double& val, std::string& error);
        bool get_field(const char* name, MYSQL_TIME& val, std::string& error);

        int is_null(int idx, std::string& error);

        void close();
    };

};

#endif