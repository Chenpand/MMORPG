/**
 *@author	 
 *@email	 
 *@date		2011-03-22		
 *@brief	mysql连接
 */
#ifndef _AVALON_MYSQL_CONNECTION_H_
#define _AVALON_MYSQL_CONNECTION_H_

#include "AvalonDBConnection.h"
#include "AvalonMysqlResultSet.h"

namespace Avalon
{
	/**
	 *@brief mysql连接
	 */
	class MysqlConnection : public DBConnection
	{
	public:
		MysqlConnection();
		~MysqlConnection();

	public:

		bool Open(const char* host,UInt32 port,const char* user,const char* passwd,const char* db);

		void Close();

		bool SetCharSet(const char* csname);

		bool BeginTransaction();

		void Commit();

		void Rollback();

		ResultSet* Query(const char* sql, size_t len);

		void FreeResultSet(ResultSet* set);

		Int64 Update(const char* sql, size_t len);

		UInt64 GetInsertID();

		const char* GetError();

		size_t EscapeString(char* to, const char* from, size_t len);

		UInt32 GetErrorNo();

	private:
		//mysql连接
		MYSQL	m_Conn;
		//ping
		bool	m_bPing;
		//是否自动提交
		bool	m_bAutoCommit;
		//连接数
		static UInt32 s_ConnNum;
	};
}

#endif
