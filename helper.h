#ifndef db_helper_h
#define db_helper_h
#include <mysql.h>

namespace zdb{
    struct db_helper{
        void init_mysql_time(MYSQL_TIME& val);
        void to_datetime(const char* str, MYSQL_TIME& val);
        void to_string(const MYSQL_TIME& val, char* str, int len);
    };
};

#endif