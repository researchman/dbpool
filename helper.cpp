/*
* @file         helper.cpp
* @brief        
* @author       zhuyunfei
* @date         2021/03/31
* @version      V1.0 
* @copyright
*/
#include "helper.h"
#include <stdio.h>

namespace zdb{
    db_helper::db_helper(){}
    db_helper::~db_helper(){}

    void db_helper::init_mysql_time(MYSQL_TIME& val)
    {
        val.year          = 0;
        val.month         = 0;
        val.day           = 0;
        val.hour          = 0;
        val.minute        = 0;
        val.second        = 0;
        val.neg           = false;
        val.second_part   = 0;
        val.time_type     = MYSQL_TIMESTAMP_DATETIME;
    }

    void db_helper::to_datetime(const char* str, MYSQL_TIME& val)
    {
        sscanf_s(str, "%d-%d-%d %d:%d:%d" &val.year, &val.month, &val.day, &val.hour, &val.minute, &val.second);
        val.second_part = 0;
        val.neg = false;
        val.time_type = MYSQL_TIMESTAMP_DATE;
    }

    void db_helper::to_string(const MYSQL_TIME& val, char* str, int len)
    {
        char buf[20] = {0};
        sprintf_s(buf, "%04d-%02d-%02d %02d:%02d:%02d", val.year, val.month, val.day, val.hour, val.minute, val.second);
        strncpy(str, buf, len);
    }
};