#include "AvalonRetryDBConnection.h"
#include "AvalonNetworkAPI.h"
#include <cstdarg>
#include "mysql/mysqld_error.h"

#if defined(AVALON_OS_WINDOWS)
#include <windows.h>
#define	avalon_sleep(msec) Sleep(msec)
#else
#	include<unistd.h>
#define	avalon_sleep(msec) usleep(msec * 1000); 
#endif

namespace Avalon
{
	RetryDBConnection::RetryDBConnection(const std::string& dbtype, UInt32 retryTimes, UInt32 retryInterval,  Avalon::DBConnection* dbconn)
		:m_DbType(dbtype), m_RetryTimes(retryTimes), m_RetryInterval(retryInterval), m_pConn(dbconn)
	{
		m_lastErrNo = 0;
	}

	RetryDBConnection::~RetryDBConnection()
	{
		m_pConn->Close();
		Avalon::DBConnection::Destroy(m_pConn);
	}

	RetryDBConnection* RetryDBConnection::Create(const std::string& dbtype, UInt32 retryTimes, UInt32 retryInterval)
	{
		Avalon::DBConnection* conn = Avalon::DBConnection::Create(dbtype);
		if(conn == NULL) return NULL;
		if(retryTimes == 0) retryTimes = 1;
		if(retryInterval < 5) retryInterval = 5;

		return new RetryDBConnection(dbtype, retryTimes, retryInterval, conn);
	}

	void RetryDBConnection::Destroy(RetryDBConnection* conn)
	{
		delete conn;
	}

	bool RetryDBConnection::Open(const char* host,UInt32 port,const char* user,const char* passwd,const char* db)
	{
		return m_pConn->Open(host, port, user, passwd, db);
	}

	void RetryDBConnection::Close()
	{
		m_pConn->Close();
	}

	bool RetryDBConnection::SetCharSet(const char* csname)
	{
		return m_pConn->SetCharSet(csname);
	}

	ResultSet* RetryDBConnection::QueryFmt(const char* sql,...)
	{
		char buf[5120];

		va_list ap;
		va_start(ap, sql);
		int ret = avalon_vsnprintf(buf, 5100, sql, ap);
		va_end(ap);

		if(ret < 0) return NULL;
		return Query(buf, size_t(ret));
	}

	ResultSet* RetryDBConnection::Query(const char* sql, size_t len)
	{
		ResultSet* result = m_pConn->Query(sql, len);
		if(result != NULL) return result;

		NETLOG_INFO("database error, host:%s, db:%s, errno:%u, error:%s", m_pConn->GetHost(), m_pConn->GetDB(), m_pConn->GetErrorNo(), m_pConn->GetError());

		int times = m_RetryTimes;
		while(times-- > 0)
		{
			NETLOG_INFO("reconnect to database, host:%s, db:%s, times:%d", m_pConn->GetHost(), m_pConn->GetDB(), times);

			Avalon::DBConnection* conn = Avalon::DBConnection::Create(m_DbType);
			if(conn->Open(m_pConn->GetHost(), m_pConn->GetPort(), m_pConn->GetUser(), m_pConn->GetPasswd(), m_pConn->GetDB()))
			{
				m_pConn->Close();
				Avalon::DBConnection::Destroy(m_pConn);
				m_pConn = conn;
				return m_pConn->Query(sql, len);
			}

			avalon_sleep(m_RetryInterval);
		}

		return NULL;
	}

	void RetryDBConnection::FreeResultSet(ResultSet* set)
	{
		m_pConn->FreeResultSet(set);
	}

	Int64 RetryDBConnection::UpdateFmt(const char* sql,...)
	{
		char buf[5120];

		va_list ap;
		va_start(ap, sql);
		int ret = avalon_vsnprintf(buf, 5100, sql, ap);
		va_end(ap);

		if(ret < 0) return -1;
		return Update(buf, size_t(ret));
	}

	Int64 RetryDBConnection::Update(const char* sql, size_t len, bool checkErrNo)
	{
		m_lastErrNo = 0;

		Int64 result = m_pConn->Update(sql, len);
		if(result >= 0) return result;

		NETLOG_INFO("database error, host:%s, db:%s, errno:%u, error:%s", m_pConn->GetHost(), m_pConn->GetDB(), m_pConn->GetErrorNo(), m_pConn->GetError());

		if (checkErrNo && ER_DUP_ENTRY == m_pConn->GetErrorNo())
		{
			m_lastErrNo = ER_DUP_ENTRY;
			return -1;
		}

		int times = m_RetryTimes;
		while(times-- > 0)
		{
			NETLOG_INFO("reconnect to database, host:%s, db:%s, times:%d", m_pConn->GetHost(), m_pConn->GetDB(), times);

			Avalon::DBConnection* conn = Avalon::DBConnection::Create(m_DbType);
			if(conn->Open(m_pConn->GetHost(), m_pConn->GetPort(), m_pConn->GetUser(), m_pConn->GetPasswd(), m_pConn->GetDB()))
			{
				m_pConn->Close();
				Avalon::DBConnection::Destroy(m_pConn);
				m_pConn = conn;
				return m_pConn->Update(sql, len);
			}

			avalon_sleep(m_RetryInterval);
		}

		return -1;
	}

	UInt64 RetryDBConnection::GetInsertID()
	{
		return m_pConn->GetInsertID();
	}

	const char* RetryDBConnection::GetError()
	{
		return m_pConn->GetError();
	}

	size_t RetryDBConnection::EscapeString(char* to, const char* from, size_t len)
	{
		return m_pConn->EscapeString(to, from, len);
	}
}
