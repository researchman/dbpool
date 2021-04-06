/*
* @file         
    helper.h

* @brief 
    数据库工具助手类

* @version      
    V1.0 

* @author       
    zhuyunfei

* @date         
    2021/03/31

* @note
    2021-04-01 添加注释

* @warning
* @bug
* @copyright
*/
#ifndef db_helper_h
#define db_helper_h
#include <mysql.h>

namespace zdb{
    struct db_helper{
        public:
            static db_helper& instance()
            {
                static db_helper m_helper;
                return m_helper;
            }
        private:
            db_helper(){};
            db_helper(const db_helper&){};
            //~db_helper();
            db_helper& operator=(const db_helper&);

        private: 
            bool m_flag;
        public:
        /*
		* @brief    把MYSQL_TIME初始化函数。
		* @param    [out] MYSQL_TIME & val  要初始化的MYSQL_TIME值\n
		* @return   无\n
		* @note
		* @warning
		* @bug
		*/
        void init_mysql_time(MYSQL_TIME& val);
        /*
	    * @brief    把MYSQL_TIME转成时间字符串转成函数。
	    * @param    [in]  char* str         时间字符串
	    * @param    [out] MYSQL_TIME & val  要转换的MYSQL_TIME类型的值
	    * @return   无\n
	    * @note
	    * @warning
	    * @bug
	    */
        void to_datetime(const char* str, MYSQL_TIME& val);
        /*
	    * @brief    把时间字符串转成MYSQL_TIME的函数。
	    * @param    [in]  MYSQL_TIME & val    转换好的MYSQL_TIME类型的值\n
	    * @param    [out] char* str           时间字符串\n
	    * @return   无\n
	    * @note
	    * @warning
	    * @bug
	    */
        void to_string(const MYSQL_TIME& val, char* str, int len);
    };
}

#endif