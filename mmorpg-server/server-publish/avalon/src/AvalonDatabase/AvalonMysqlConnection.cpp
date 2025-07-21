#include "AvalonMysqlConnection.h"
#include <cassert>

namespace Avalon
{
	UInt32 MysqlConnection::s_ConnNum = 0;

	MysqlConnection::MysqlConnection()
	{
		if(++s_ConnNum == 1)
		{
			mysql_library_init(-1,NULL,NULL);
		}

		assert(mysql_thread_safe() == 1);

		mysql_init(&m_Conn);

		my_bool reconnect = 1;
		m_bPing = (mysql_options(&m_Conn,MYSQL_OPT_RECONNECT,&reconnect) != 0); //noted by aprilliu, 设置为自动重连
		unsigned int timeout = 60;
		mysql_options(&m_Conn, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);
		mysql_options(&m_Conn, MYSQL_OPT_READ_TIMEOUT, &timeout);
		mysql_options(&m_Conn, MYSQL_OPT_WRITE_TIMEOUT, &timeout);

		m_bAutoCommit = true;
	}

	MysqlConnection::~MysqlConnection()
	{
		Close();

		if(--s_ConnNum == 0)
		{
			mysql_library_end();
		}
	}

	bool MysqlConnection::Open(const char* host,UInt32 port,const char* user,const char* passwd,const char* db)
	{
		m_bAutoCommit = true;

		MYSQL* ret = mysql_real_connect(&m_Conn,host,user,passwd,db,port,NULL,0);
		if(NULL == ret)
		{
			mysql_close(&m_Conn);
			return false;
		}

		if(host != NULL)	m_Host = host;
		m_Port = port;
		if(user != NULL)	m_User = user;
		if(passwd != NULL)	m_Passwd = passwd;
		if(db != NULL)		m_Db = db;

		return true;
	}

	void MysqlConnection::Close()
	{
		mysql_close(&m_Conn);
	}

	bool MysqlConnection::SetCharSet(const char* csname)
	{
		return mysql_set_character_set(&m_Conn, csname) == 0;
	}

	bool MysqlConnection::BeginTransaction()
	{
		if(!m_bAutoCommit) return true;

		if(mysql_autocommit(&m_Conn,0) != 0)// noted by aprilliu InnoDB表引擎下关闭mysql自动事务提交可以大大提高数据插入的效率
		{
			return false;
		}
		
		m_bAutoCommit = false;
		return true;
	}

	void MysqlConnection::Commit()
	{
		if(m_bAutoCommit) return;

		mysql_commit(&m_Conn);
		
		if(mysql_autocommit(&m_Conn,1) == 0)
		{
			m_bAutoCommit = true;
		}
	}

	void MysqlConnection::Rollback()
	{
		if(m_bAutoCommit) return;

		mysql_rollback(&m_Conn);

		if(mysql_autocommit(&m_Conn,1) == 0)
		{
			m_bAutoCommit = true;
		}
	}

	ResultSet* MysqlConnection::Query(const char* sql, size_t len)
	{
		if(m_bPing)
		{
			mysql_ping(&m_Conn); //noted by aprilliu,执行query前 先ping，如果连接断开则会自动重连
		}

		if(mysql_real_query(&m_Conn, sql, len) != 0)
		{
			return NULL;
		}

		MYSQL_RES* res = mysql_store_result(&m_Conn);
		if(res == NULL)
		{
			return NULL;
		}

		return new MysqlResultSet(res);
	}

	void MysqlConnection::FreeResultSet(ResultSet* set)
	{
		delete set;
	}

	Int64 MysqlConnection::Update(const char* sql, size_t len)
	{
		if (m_bPing)
		{
			mysql_ping(&m_Conn); //noted by aprilliu,执行query前 先ping，如果连接断开则会自动重连
		}

		if(mysql_real_query(&m_Conn, sql, len) != 0)
		{
			return -1;
		}
		
		return (Int64)mysql_affected_rows(&m_Conn);
	}

	UInt64 MysqlConnection::GetInsertID()
	{
		return mysql_insert_id(&m_Conn);
	}

	const char* MysqlConnection::GetError()
	{
		return mysql_error(&m_Conn);
	}

	size_t MysqlConnection::EscapeString(char* to,const char* from,size_t len)
	{
		return mysql_real_escape_string(&m_Conn,to,from,len);
	}

	UInt32 MysqlConnection::GetErrorNo()
	{
		return mysql_errno(&m_Conn);
	}
}

