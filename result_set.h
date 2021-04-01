/*
* @file         
    result_set.h

* @brief 
    数据库结果集类

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
        friend class connection;

        MYSQL_RES* m_query_res;     // 结果集
        MYSQL_ROW m_cur_row;        // 当前记录行
        int m_field_count;         // 字段个数
        std::map<std::string, int> m_field_idx_list;    //字段名-字段下标

        private:
        /*
		* @brief
		    构建字段名字段下标对应列表函数。
		* @note
		* @warning
		* @bug
		*/
        void create_filed_idx_list();
        /*
		* @brief
		获得原始字符串值函数。
		* @param  [in]  int idx             字段下标\n
		* @param  [out] char* val           获得的原始字符串值\n
		* @param  [out] bool& is_null       是否为空\n
		* @param  [out] std::string& error  错误信息\n

		* @return 返回获得字段值是否成功
		* @return  true  成功\n
		* @return  false  失败\n
		* @note
		* @warning
    	* @bug
		*/
        bool get_field(int idx, char*& val, bool& is_null, std::string& error);

        public:
        result_set();
        ~result_set();

        /* 
        * @brief
	        绑定到一个MYSQL结果集函数。
	    * @param  [in] MYSQL_RES *res        MYSQL结果集\n
	    * @param  [out] std::string& error   错误信息\n
	    * @return 返回绑定到一个MYSQL结果集是否成功
	    * @return  true  成功\n
	    * @return  false  失败\n
	    * @note
	    * @warning
	    * @bug
	    */
        bool bind(MYSQL_RES* res, std::string& error);
        /*
	    * @brief
	        查找数据函数。
	    * @param  [in] my_ulonglong offset  偏移量\n
	    * @param  [out] std::string& error  错误信息\n
	    * @return 返回查找数据是否成功
	    * @return  true  成功\n
	    * @return  false  失败\n
	    * @note
	    * @warning
	    * @bug
	    */
        bool seek(my_ulonglong offset, std::string& error);
        /*
	    * @brief
	        获得下一条记录函数。
	    * @param  [out] std::string& error  错误信息\n
	    * @return 返回获得下一条记录是否成功
	    * @return  true  成功\n
	    * @return  false  失败\n
	    * @note
	    * @warning
	    * @bug
	    */
        bool get_next_record(std::string& error);
        /*
	    * @brief
	        获得记录数函数。
	    * @param  [out] std::string& error  错误信息\n
	    * @return 返回获得的记录数
	    * @return  >=0  成功\n
	    * @return  -1  失败\n
	    * @note
	    * @warning
	    * @bug
	    */
        my_ulonglong get_record_count(std::string& error);

        /*
	    * @brief
	        获得int字段值函数。
	    * @param  [in] int idx  字段下标\n
	    * @param  [out] int& val  获得的int字段值\n
	    * @param  [out] std::string& error  错误信息\n
	    * @return 返回获得字段值是否成功
	    * @return  true  成功\n
	    * @return  false  失败\n
	    * @note
	    * @warning
	    * @bug
	    */
        bool get_field(int idx, int& val, std::string& error);
        /*
	    * @brief
	        获得unsigned int字段值函数。
	    * @param  [in] int idx  字段下标\n
	    * @param  [out] unsigned int& val   获得的int字段值\n
	    * @param  [out] std::string& error  错误信息\n
	    * @return 返回获得字段值是否成功
	    * @return  true  成功\n
	    * @return  false  失败\n
	    * @note
	    * @warning
	    * @bug
	    */
        bool get_field(int idx, unsigned int& val, std::string& error);
        /*
	    * @brief
	        获得long long字段值函数。
	    * @param  [in] int idx  字段下标\n
	    * @param  [out] long long& val  获得的int字段值\n
	    * @param  [out] std::string& error  错误信息\n
	    * @return 返回获得字段值是否成功
	    * @return  true  成功\n
	    * @return  false  失败\n
	    * @note
	    * @warning
	    * @bug
	    */
        bool get_field(int idx, long long& val, std::string& error);
        /*
	    * @brief
	        获得string字段值函数。
	    * @param  [in] int idx  字段下标\n
	    * @param  [out] string& val  获得的int字段值\n
	    * @param  [out] std::string& error  错误信息\n
	    * @return 返回获得字段值是否成功
	    * @return  true  成功\n
	    * @return  false  失败\n
	    * @note
	    * @warning
	    * @bug
	    */
        bool get_field(int idx, std::string& val, std::string& error);
        /*
	    * @brief
	        获得char*字段值函数。
	    * @param  [in] int idx              字段下标\n
	    * @param  [out] char* val           获得的int字段值\n
        * @param  [in] int len              value的最大长度\n
	    * @param  [out] std::string& error  错误信息\n
	    * @return 返回获得字段值是否成功
	    * @return  true  成功\n
	    * @return  false  失败\n
	    * @note
	    * @warning
	    * @bug
	    */
        bool get_field(int idx, char* val, int len, std::string& error);
        /*
	    * @brief
	        获得bool字段值函数。
	    * @param  [in] int idx  字段下标\n
	    * @param  [out] bool& val  获得的int字段值\n
	    * @param  [out] std::string& error  错误信息\n
	    * @return 返回获得字段值是否成功
	    * @return  true  成功\n
	    * @return  false  失败\n
	    * @note
	    * @warning
	    * @bug
	    */
        bool get_field(int idx, bool& val, std::string& error);
        /*
	    * @brief
	        获得float字段值函数。
	    * @param  [in] int idx  字段下标\n
	    * @param  [out] float& val  获得的int字段值\n
	    * @param  [out] std::string& error  错误信息\n
	    * @return 返回获得字段值是否成功
	    * @return  true  成功\n
	    * @return  false  失败\n
	    * @note
	    * @warning
	    * @bug
	    */
        bool get_field(int idx, float& val, std::string& error);
        /*
	    * @brief
	        获得double字段值函数。
	    * @param  [in] int idx  字段下标\n
	    * @param  [out] double& val  获得的int字段值\n
	    * @param  [out] std::string& error  错误信息\n
	    * @return 返回获得字段值是否成功
	    * @return  true  成功\n
	    * @return  false  失败\n
	    * @note
	    * @warning
	    * @bug
	    */
        bool get_field(int idx, double& val, std::string& error);
        /*
	    * @brief
	        获得lMYSQL_TIME字段值函数。
	    * @param  [in] int idx  字段下标\n
	    * @param  [out] MYSQL_TIME& val  获得的int字段值\n
	    * @param  [out] std::string& error  错误信息\n
	    * @return 返回获得字段值是否成功
	    * @return  true  成功\n
	    * @return  false  失败\n
	    * @note
	    * @warning
	    * @bug
	    */
        bool get_field(int idx, MYSQL_TIME& val, std::string& error);
 
        /*
	    * @brief
	        根据字段名获得字段下标函数。
	    * @param  [in] const char* name  字段名\n
	    * @param  [out] std::string& error  错误信息\n
	    * @return 返回获得的字段下标
	    * @return  >=0  成功\n
	    * @return  -1  失败\n
	    * @note
	    * @warning
	    * @bug
	    */
        int get_field_idx_by_name(const char* name, std::string& error);
        /*
	    * @brief
	        根据字段名获得int字段值函数。
	    * @param  [in]  const char* name    字段名\n
	    * @param  [out] int& val            获得的字段值\n
	    * @param  [out] std::string& error  错误信息\n
	    * @return 返回获得字段值是否成功
	    * @return  true  成功\n
	    * @return  false  失败\n
	    * @note
	    * @warning
	    * @bug
	    */
        bool get_field(const char* name, int& val, std::string& error);
        /*
	    * @brief
	        根据字段名获得long long字段值函数。
	    * @param  [in]  const char* name    字段名\n
	    * @param  [out] long long& val      获得的字段值\n
	    * @param  [out] std::string& error  错误信息\n
	    * @return 返回获得字段值是否成功
	    * @return  true  成功\n
	    * @return  false  失败\n
	    * @note
	    * @warning
	    * @bug
	    */
        bool get_field(const char* name, long long& val, std::string& error);
        /*
	    * @brief
	        根据字段名获得unsigned int字段值函数。
	    * @param  [in]  const char* name    字段名\n
	    * @param  [out] unsigned int& val   获得的字段值\n
	    * @param  [out] std::string& error  错误信息\n
	    * @return 返回获得字段值是否成功
	    * @return  true  成功\n
	    * @return  false  失败\n
	    * @note
	    * @warning
	    * @bug
	    */
        bool get_field(const char* name, unsigned int& val, std::string& error);
        /*
	    * @brief
	        根据字段名获得string字段值函数。
	    * @param  [in]  const char* name    字段名\n
	    * @param  [out] string& val         获得的字段值\n
	    * @param  [out] std::string& error  错误信息\n
	    * @return 返回获得字段值是否成功
	    * @return  true  成功\n
	    * @return  false  失败\n
	    * @note
	    * @warning
	    * @bug
	    */
        bool get_field(const char* name, std::string& val, std::string& error);
        /*
	    * @brief
	        根据字段名获得char*字段值函数。
	    * @param  [in]  const char* name    字段名\n
	    * @param  [out] char* val           获得的字段值\n
	    * @param  [in]  int len             value的长度\n
	    * @param  [out] std::string& error  错误信息\n
	    * @return 返回获得字段值是否成功
	    * @return  true  成功\n
	    * @return  false  失败\n
	    * @note
	    * @warning
	    * @bug
	    */
        bool get_field(const char* name, char* val, int len, std::string& error);
        /*
	    * @brief
	        根据字段名获得bool字段值函数。
	    * @param  [in]  const char* name    字段名\n
	    * @param  [out] bool& val           获得的字段值\n
	    * @param  [out] std::string& error  错误信息\n
	    * @return 返回获得字段值是否成功
	    * @return  true  成功\n
	    * @return  false  失败\n
	    * @note
	    * @warning
	    * @bug
	    */
        bool get_field(const char* name, bool& val, std::string& error);
        /*
	    * @brief
	        根据字段名获得float字段值函数。
	    * @param  [in]  const char* name    字段名\n
	    * @param  [out] float& val          获得的字段值\n
	    * @param  [out] std::string& error  错误信息\n
	    * @return 返回获得字段值是否成功
	    * @return  true  成功\n
	    * @return  false  失败\n
	    * @note
	    * @warning
	    * @bug
	    */
        bool get_field(const char* name, float& val, std::string& error);
        /*
	    * @brief
	        根据字段名获得double字段值函数。
	    * @param  [in]  const char* name    字段名\n
	    * @param  [out] double& val         获得的字段值\n
	    * @param  [out] std::string& error  错误信息\n
	    * @return 返回获得字段值是否成功
	    * @return  true  成功\n
	    * @return  false  失败\n
	    * @note
	    * @warning
	    * @bug
	    */
        bool get_field(const char* name, double& val, std::string& error);
        /*
	    * @brief
	        根据字段名获得MYSQL_TIME字段值函数。
	    * @param  [in]  const char* name    字段名\n
	    * @param  [out] MYSQL_TIME& val     获得的字段值\n
	    * @param  [out] std::string& error  错误信息\n
	    * @return 返回获得字段值是否成功
	    * @return  true  成功\n
	    * @return  false  失败\n
	    * @note
	    * @warning
	    * @bug
	    */
        bool get_field(const char* name, MYSQL_TIME& val, std::string& error);
        /*
		* @brief
		    判断字段是否为空函数。
		* @param  [in]  int idx             字段下标\n
		* @param  [out] std::string& error  错误信息\n
		* @return 返回字段是否为空
		* @return  1  为空\n
		* @return  0  不为空\n
		* @return  -1  有错误\n
		* @note
		* @warning
		* @bug
		*/
        int is_null(int idx, std::string& error);

        /*
	    * @brief
	        关闭函数。
	    * @param  无\n
	    * @return 无\n
	    * @note
	    * @warning
	    * @bug
	    */
        void close();
    };

};

#endif