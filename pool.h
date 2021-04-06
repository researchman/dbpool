/*
* @file         
    pool.h

* @brief 
    数据库连接池

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
#ifndef zdb_pool_h
#define zdb_pool_h
#include <boost/serialization/singleton.hpp>
#include <list>
#include <string>
#include <mutex>
#include <vector>
#include <atomic>
#include "connection.h"

namespace zdb{
    class db_pool{
        private:
        std::list<ptr_connection> m_work_list;  // 工作连接池
        std::list<ptr_connection> m_idle_list;  // 空闲连接池

        db_pool_setting m_pool_setting;         // 连接池设置
        std::mutex m_mtx;                       // 池锁
        std::atomic<bool> m_running;            // 异步线程是否运行
        std::atomic<bool> m_is_exited;          // 异步线程退出标志
        ptr_connection m_async_conn;            // 异步线程使用的数据库连接

		public:
		std::mutex m_async_mtx;
    	std::vector<async_sql*> m_async_list;

		public:
		db_pool();
		~db_pool();
		
        private:
        /*
		* @brief    创建一个空闲连接函数。
		* @param    [out] std::string& error  错误信息\n
		* @param    [in]  bool is_temp        创建的连接是否是临时的\n
		* @return   返回创建的连接
		* @return   >0   成功\n
		* @return   ==0  失败\n
		* @note
		* @warning
		* @bug
		*/
        ptr_connection create_connection(std::string& error, bool is_temp = false);
        /*
		* @brief    创建异步执行线程所用的数据库连接。
		* @param    无\n
		* @return   无\n
		* @note
		* @warning
		* @bug
		*/
        void create_async_connection();
        /*
		* @brief    销毁异步执行线程所用的数据库连接。
		* @param    无\n
		* @return   无\n
		* @note
		* @warning
		* @bug
		*/
        void destroy_async_connection();
        /*
		* @brief    启动异步执行线程函数。
		* @param    无\n
		* @return   无
		* @note
		* @warning
		* @bug
		*/
        void start_async_thread();
        /*
		* @brief    停止异步执行线程。
		* @param    无\n
		* @return   无
		* @note
		* @warning
		* @bug
		*/
        void stop_async_thread();

        protected:
        /*
		* @brief    异步执行线程函数。
		* @param    [in] CDBConnPool* owner  线程所属对象\n
		* @return   无\n
		* @note
		* @warning
		* @bug
		*/
        void async_thread_func(db_pool* owner);
        /*
		* @brief    执行sql。
		* @param    [in] async_sql* ptr_data  待执行sql\n
		* @return   无\n
		* @note
		* @warning
		* @bug
		*/
        void execute_async_sql(async_sql* ptr_data);

        public:
        /*
		* @brief    创建数据库连接池函数。
		* @param    [in]  const db_pool_setting& cfg    数据库连接池设置\n
		* @param    [out] std::string& error      		错误信息\n
		* @return   返回创建数据库连接池是否成功
		* @return   true   成功
		* @return   false  失败
		* @note
		* @warning
		* @bug
		*/
        bool create(const db_pool_setting& cfg, std::string& error);
        bool create(const db_pool_setting& cfg, bool async, std::string& error);
        /*
		* @brief    关闭线程池
		* @param    无\n
		* @return   无\n
		* @note
		* @warning
		* @bug
		*/
        void close();
        /*
		* @brief    从连接池中获得一个数据库连接函数。
		* @param    [out] std::string& error  错误信息\n
		* @return   返回获得一个数据库连接是否成功
		* @return   0 失败\n
		* @return   !=0 获得的数据库连接\n
		* @note
		* @warning
		* @bug
		*/
        ptr_connection get_connect(std::string& error);
        /*
		* @brief    归还一个数据库连接到连接池中函数。
		* @param    [in] ptr_connection ptr_conn  数据库连接\n
		* @return   无\n
		* @note
		* @warning
		* @bug
		*/
        void back(ptr_connection ptr_conn);
        /*
		* @brief    把SQL语句加入异步执行队列函数。
		* @param    [in] const std::string& sql  要执行的SQL语句\n
		* @return   加入异步执行队列是否成功
		* @return   true成功
		* @return   false失败
		* @note
		* @warning
		* @bug
		*/
        bool push_async(const std::string& sql);
        /*
		* @brief    执行SQL语句返回结果集函数。
		* @param    [in]  const char *sql       SQL语句
		* @param    [out] CDBResultSet& res     结果集
		* @param    [out] std::string& error    错误信息
		* @return   返回查询是否成功
		* @return   true  成功
		* @return   false  失败
		* @note
		* @warning
		* @bug
		*/
        bool query(const char* sql, result_set& res, std::string& error);
        MYSQL_RES* query(const char* sql, std::string& error);
        /*
		* @brief    执行SQL语句函数获得受影响函数。
		* @param    [in]  const char *sql       SQL语句
		* @param    [out] std::string& error    错误信息
		* @return   返回影响到的记录数量
		* @return   <=0  失败
		* @return   >=0  成功
		* @note
		* @warning
		* @bug
		*/
        my_ulonglong execute_affect_rows(const char* sql, std::string& error);
        /*
		* @brief    执行SQL语句并获得受实际影响函数。
		* @param    [in]  const char *sql       SQL语句
		* @param    [out] std::string& error    错误信息
		* @return   返回影响到的记录数量
		* @return   <=0  失败
		* @return   >0   成功
		* @note
		* @warning
		* @bug
		*/
		my_ulonglong execute_real_affect_rows( const char *sql, std::string& error);
    };
}

typedef boost::serialization::singleton<zdb::db_pool> sdb_pool;
#define zdb_pool sdb_pool::get_mutable_instance()
#define zdb_pool_const sdb_pool::get_const_instance()

//: public boost::serialization::singleton<zdb::db_pool>
//#define zdb_pool zdb::db_pool::get_mutable_instance()
//#define zdb_pool_const zdb::db_pool::get_const_instance()

#endif