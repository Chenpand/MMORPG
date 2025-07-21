/**
 *@author	 
 *@email	 
 *@date		2011-03-22		
 *@brief	数据库连接
 */
#ifndef _AVALON_DBCONNECTION_H_
#define _AVALON_DBCONNECTION_H_

#include "AvalonDefine.h"
#include "AvalonResultSet.h"

namespace Avalon
{
	/**
	 *@brief 数据库连接
	 */
	class DBConnection
	{
	public:
		virtual ~DBConnection(){}

	public:
		/**
		 *@brief 创建一个连接
		 *@param dbtype 数据库类型 [mysql]
		 */
		static DBConnection* Create(const std::string& dbtype);

		/**
		 *@brief 销毁一个连接
		 */
		static void Destroy(DBConnection* conn);

		/**
		 *@brief 打开一个连接
		 *@param host 主机名或ip
		 *@param port 端口
		 *@param user 用户名
		 *@param passwd 密码
		 *@param db 数据库名
		 */
		virtual bool Open(const char* host,UInt32 port,const char* user,const char* passwd,const char* db) = 0;


		/**
		 *@brief 获取数据库相关信息
		 */
		const char* GetHost() const{ return m_Host.c_str(); }
		UInt32		GetPort() const{ return m_Port; }
		const char* GetUser() const{ return m_User.c_str(); }
		const char* GetPasswd() const{ return m_Passwd.c_str(); }
		const char* GetDB() const{ return m_Db.c_str(); }

		/**
		 *@brief 关闭连接
		 */
		virtual void Close() = 0;

		/**
		 *@brief 设置编码
		 */
		virtual bool SetCharSet(const char* csname) = 0;

		/**
		 *@brief 开始一个事务
		 */
		virtual bool BeginTransaction() = 0;

		/**
		 *@brief 提交事务
		 */
		virtual void Commit() = 0;

		/**
		 *@brief 回退事务
		 */
		virtual void Rollback() = 0;

		/**
		 *@brief 变参查询
		 */
		virtual ResultSet* QueryFmt(const char* sql,...);

		/**
		 *@brief 查询
		 */
		virtual ResultSet* Query(const char* sql, size_t len) = 0;
		ResultSet* Query(const std::string& sql){ return Query(sql.c_str(), sql.length()); }

		/**
		 *@brief 释放结果集
		 */
		virtual void FreeResultSet(ResultSet* set) = 0;

		/**
		 *@brief 执行更新语句
		 */
		virtual Int64 UpdateFmt(const char* sql,...);

		/**
		 *@brief 执行更新语句
		 */
		virtual Int64 Update(const char* sql, size_t len) = 0;
		Int64 Update(const std::string& sql){ return Update(sql.c_str(), sql.length()); }

		/**
		 *@brief 获取生成的id
		 */
		virtual UInt64 GetInsertID() = 0;

		/**
		 *@brief 获取错误信息
		 */
		virtual const char* GetError() = 0;

		/**
		 *@brief 对字符串进行转义
		 *@param to 目标字符串缓冲区，必须至少为原长度的2倍加1
		 */
		virtual size_t EscapeString(char* to, const char* from, size_t len) = 0;

		/**
		 *@brief 获取错误ID
		 */
		virtual UInt32 GetErrorNo() = 0;

	protected:
		//地址
		std::string	m_Host;
		//端口
		UInt32		m_Port;
		//用户名
		std::string	m_User;
		//密码
		std::string	m_Passwd;
		//数据库
		std::string	m_Db;
	};
}

#endif
