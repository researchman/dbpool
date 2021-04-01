/*
* @file         
    connection.h

* @brief 
    数据库连接类

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
#ifndef zdb_connection_h
#define zdb_connection_h
#include <memory>
#include <mysql.h>
#include "common.h"
#include "result_set.h"

namespace zdb{
    class connection: public std::enable_shared_from_this<connection>{
        private:
        MYSQL* m_conn;          // 数据库连接
        MYSQL_STMT* m_stmt;     //
        result_set m_res;       // 结果集
        bool m_tmp_flag;        // 是否为临时连接

        public:
        connection(bool temp = false);
        ~connection();

        /*
		* @brief	连接数据库函数。
		* @param  	[in]  const zdb::db_setting& cfg   数据库连接设置\n
		* @param  	[out] std::string& error           错误信息\n
		* @return 	返回连接数据库是否成功
		* @return  	true   成功\n
		* @return  	false  失败\n
		* @note
		* @warning
		* @bug
		*/
        bool connect(const zdb::db_setting& cfg, std::string& error);
        /*
		* @brief	关闭数据库连接函数。
		* @param  	无\n
		* @return  	无\n
		* @note
		* @warning
		* @bug
		*/
        void close();
        /*
		* @brief	执行SQL语句返回结果集函数。
		* @param 	[in]  const char *sql      SQL语句\n
		* @param 	[out] std::string& error   错误信息\n
		* @return 	返回结果集
		* @return  	0    失败\n
		* @return  	!=0  成功\n
		* @note
		* @warning
		* @bug
		*/
        result_set* execute_query(const char* sql, std::string& error);
        /*
		* @brief	执行SQL语句返回mysql原始结果集函数。
		* @param 	[in]  const char *sql      SQL语句\n
		* @param 	[out] std::string& error   错误信息\n
		* @return 	返回的mysql原始结果集
		* @return  	==0  失败\n
		* @return  	!=0  成功\n
		* @note
		* @warning
		* @bug
		*/
        MYSQL_RES* query(const char* sql, std::string& error);
        /*
		* @brief	执行SQL语句函数。
		* @param 	[in]  const char *sql      SQL语句\n
		* @param 	[out] std::string& error   错误信息\n
		* @return 	返回影响到的记录数量
		* @return  	-1   失败\n
		* @return  	>=0  成功\n
		* @note
		* @warning
		* @bug
		*/
        my_ulonglong execute_affect_rows(const char* sql, std::string& error);
        /*
		* @brief	执行SQL语句函数。
		* @param 	[in]  const char *sql      SQL语句\n
		* @param 	[out] std::string& error   错误信息\n
		* @return 	返回影响到的记录数量
		* @return  	-1   失败\n
		* @return  	>=0  成功\n
		* @note
		* @warning
		* @bug
		*/
        my_ulonglong execute_real_affect_rows(const char* sql, std::string& error);
        /*
		* @brief	获得最后一次插入的ID函数。
		* @param 	[out] std::string& error  错误信息\n
		* @return 	返回最后一次插入的ID
		* @return  	-1   失败\n
		* @return  	>=0  成功\n
		* @note
		* @warning
		* @bug
		*/
        my_ulonglong get_last_inserted_id(std::string& error);
        /*
		* @brief	测试连接函数。
		* @param 	[out] std::string& error 错误信息\n
		* @return 	返回测试连接是否成功\n
		* @return	0  	 连接测试成功\n
		* @return	!=0  连接测试失败\n
		* @note
		* @warning
		* @bug
		*/
        int ping(std::string& error);
        /*
		* @brief	设置自动提交的函数。
		* @param 	[in]  bool mode 			是否是否自动提交\n
		* @param 	[out] std::string& error 	错误信息\n
		* @return 	返回设置自动提交是否成功
		* @return  	0  成功\n
		* @return  	!=0  失败\n
		* @note
		* @warning
		* @bug
		*/
        int auto_commit(bool mode, std::string& error);
        /*
		* @brief	提交函数。
		* @param 	[out] std::string& error 错误信息\n
		* @return 	返回提交是否成功
		* @return  	0  成功\n
		* @return  	!=0  失败\n
		* @note
		* @warning
		* @bug
		*/
        int commit(std::string& error);
        /*
		* @brief	回滚函数。
		* @param 	[out] std::string& error  错误信息\n
		* @return 	返回回滚是否成功
		* @return  	0  成功\n
		* @return  	!=0  失败\n
		* @note
		* @warning
		* @bug
		*/
        int roll_back(std::string& error);
        /*
		* @brief	准备stmt函数。
		* @param 	const char *sql 			预处理SQL语句\n
		* @param 	[out] std::string& error  	错误信息\n
		* @return 	返回准备stmt是否成功
		* @return  	false  失败\n
		* @return  	true  成功\n
		* @note
		* @warning
		* @bug
		*/
        bool prepare_stmt(const char* sql, std::string& error);
        /*
		* @brief	执行stmt函数。
		* @param 	[in]  MYSQL_BIND *binds    要执行的BIND\n
		* @param 	[in]  uint64_t *pid        执行后获得的id\n
		* @param 	[out] std::string& error   错误信息\n
		* @return 	返回执行stmt成功还是失败
		* @return  	false  失败\n
		* @return  	true  成功\n
		* @note
		* @warning
		* @bug
		*/
        bool stmt_execute(MYSQL_BIND* binds, int64_t* pid, std::string& error);
        /*
		* @brief	关闭stmt函数。
		* @param 	无\n
		* @return 	无\n
		* @note
		* @warning
		* @bug
		*/
        void stmt_close();
        /*
		* @brief	获得最后一次错误信息函数。
		* @param 	无\n
		* @return 	返回最后一次错误
		* @note
    	* @warning
		* @bug
		*/
        const char* get_last_error();
		/*
		* @brief	获得连接是否为临时连接状态。
		* @param 	无\n
		* @return 	返回连接是否为临时连接状态
		* @note
    	* @warning
		* @bug
		*/
        bool is_temp()
        {
            return m_tmp_flag;
        }
		/*
		* @brief	获得数据库连接。
		* @param 	无\n
		* @return 	返回数据库连接
		* @note
    	* @warning
		* @bug
		*/
        std::shared_ptr<connection> get()
        {
            return shared_from_this();
        }
		/*
		* @brief	数据库连接是否已连接。
		* @param 	无\n
		* @return 	返回数据库连接状态
		* @note
    	* @warning
		* @bug
		*/
        bool is_open()
        {
            return (m_conn == NULL)?false:true;
        }
    };

    typedef std::shared_ptr<zdb::connection> ptr_connection;
};

#endif