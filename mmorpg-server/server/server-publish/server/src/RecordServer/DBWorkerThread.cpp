#include "DBWorkerThread.h"
#include <CLDefine.h>

DBWorkerThread::DBWorkerThread(Avalon::ThreadPool* pool)
:WorkerThread(pool)
{
	m_DbConn = NULL;
}

DBWorkerThread::~DBWorkerThread()
{
	if (m_DbConn)
	{
		m_DbConn->Close();
		Avalon::RetryDBConnection::Destroy(m_DbConn);
		m_DbConn = NULL;
	}
}

bool DBWorkerThread::Init()
{
	m_DbConn = CreateDBConn();
	if (!m_DbConn)
	{
		ErrorLogStream << "create db connection failed." << LogStream::eos;
		return false;
	}

	return true;
}

Avalon::RetryDBConnection* DBWorkerThread::GetDBConn() const
{
	return m_DbConn;
}

Avalon::RetryDBConnection* DBWorkerThread::CreateDBConn()
{
	std::string host = "127.0.0.1";
	UInt32 port = 3306;
	std::string user = "root";
	std::string passwd = "123456";
	std::string dbname = "dnf-db";

	Avalon::RetryDBConnection* conn = Avalon::RetryDBConnection::Create("mysql", 60, 500);
	if (!conn->Open(host.c_str(), port, user.c_str(), passwd.c_str(), dbname.c_str()))
	{
		ErrorLogStream << "can not connect database " << dbname << "! host="
			<< host << ", port=" << port << "." << LogStream::eos;
		Avalon::RetryDBConnection::Destroy(conn);
		return NULL;
	}

	return conn;
}