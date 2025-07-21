#ifndef _AVALON_RETRY_DB_CONNECTION_H_
#define _AVALON_RETRY_DB_CONNECTION_H_

#include "AvalonDBConnection.h"

namespace Avalon
{
	/**
	 *@brief 自动重连的数据库连接
	 */
	class RetryDBConnection
	{
		RetryDBConnection(const std::string& dbtype, UInt32 retryTimes, UInt32 retryInterval, Avalon::DBConnection* dbconn);

	public:
		~RetryDBConnection();

	public:
		/**
		 *@brief 创建一个连接
		 */
		static RetryDBConnection* Create(const std::string& dbtype, UInt32 retryTimes = 1, UInt32 retryInterval = 100);

		/**
		 *@brief 销毁一个连接
		 */
		static void Destroy(RetryDBConnection* conn);

		/**
		 *@brief 打开一个连接
		 *@param host 主机名或ip
		 *@param port 端口
		 *@param user 用户名
		 *@param passwd 密码
		 *@param db 数据库名
		 */
		bool Open(const char* host,UInt32 port,const char* user,const char* passwd,const char* db);

		/**
		 *@brief 关闭连接
		 */
		void Close();

		/**
		 *@brief 设置编码
		 */
		bool SetCharSet(const char* csname);

		/**
		 *@brief 变参查询
		 */
		ResultSet* QueryFmt(const char* sql,...);

		/**
		 *@brief 查询
		 */
		ResultSet* Query(const char* sql, size_t len);
		ResultSet* Query(const std::string& sql){ return Query(sql.c_str(), sql.length()); }

		/**
		 *@brief 释放结果集
		 */
		void FreeResultSet(ResultSet* set);

		
		/**
		 *@brief 执行更新语句
		 */
		Int64 UpdateFmt(const char* sql,...);

		/**
		 *@brief 执行更新语句
		 */
		Int64 Update(const char* sql, size_t len, bool checkErrNo = false);
		Int64 Update(const std::string& sql, bool checkErrNo=false){ return Update(sql.c_str(), sql.length(), checkErrNo); }

		/**
		 *@brief 获取生成的id
		 */
		UInt64 GetInsertID();

		/**
		 *@brief 获取错误信息
		 */
		const char* GetError();

		/**
		 *@brief 对字符串进行转义
		 *@param to 目标字符串缓冲区，必须至少为原长度的2倍加1
		 */
		size_t EscapeString(char* to, const char* from, size_t len);


		UInt32 GetErrorNo() { return m_lastErrNo; }

	private:
		//数据库类型
		std::string	m_DbType;
		//重连次数
		UInt32		m_RetryTimes;
		//重连间隔
		UInt32		m_RetryInterval;
		//连接
		Avalon::DBConnection*	m_pConn;
		//上次错误ID
		UInt32 m_lastErrNo;
	};
}

#endif
