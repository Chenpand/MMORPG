#include "DBThread.h"
#include "DBMgr.h"
#include "RSApplication.h"

DBThread::DBThread()
{
	m_DbConn = NULL;
}

DBThread::~DBThread()
{
	if (m_DbConn)
	{
		// 把没执行完的sql语句都执行了
		while (!m_SqlQueue.empty())
		{
			auto task = m_SqlQueue.pop();
			ExcuteSql(task);
			delete task;
		}

		m_DbConn->Close();
		Avalon::RetryDBConnection::Destroy(m_DbConn);
		m_DbConn = NULL;
	}
}

bool DBThread::Init(const std::string& host, UInt32 port, const std::string& user, const std::string& passwd, const std::string& dbname)
{
	m_DbConn = Avalon::RetryDBConnection::Create("mysql", 60, 500);
	if (!m_DbConn->Open(host.c_str(), port, user.c_str(), passwd.c_str(), dbname.c_str()))
	{
		ErrorLogStream << "can not connect database " << dbname << "! host="
			<< host << ", port=" << port << "." << LogStream::eos;
		Avalon::RetryDBConnection::Destroy(m_DbConn);
		return false;
	}

	return true;
}

void DBThread::Run()
{
	while (true)
	{
		Avalon::Thread::Sleep(10);

		while (!m_SqlQueue.empty())
		{
			auto task = m_SqlQueue.pop();
			ExcuteSql(task);
			delete task;
		}
	}
}

void DBThread::PushSql(const std::string& sql, SqlType type, DBRequestCallback* cb)
{
	auto task = new SqlTask();
	task->sql = sql;
	task->type = type;
	task->callback = cb;
	m_SqlQueue.push(task);
}

void DBThread::ExcuteSql(SqlTask* task)
{
	if (!task || m_DbConn == NULL)
	{
		return;
	}

	DBResult* result = new DBResult();
#ifdef __RECORD_SQL__
	DebugLogStream << "execute sql:" << task->sql << "." << LogStream::eos;
#endif
	if (task->type == SQL_SELECT)
	{
		auto resultSet = m_DbConn->Query(task->sql);
		if (!resultSet)
		{
			ErrorLogStream << "execute sql failed:" << m_DbConn->GetError() << " sql:" << task->sql << "!" << LogStream::eos;
			RSApplication::Instance()->Exit();
			return;
		}
		result->resultSet = resultSet;
		if (task->callback)
		{
			task->callback->SetSql(task->sql);
		}
	}
	else
	{
		result->affectRow = m_DbConn->Update(task->sql);
		if (task->callback)
		{
			task->callback->SetAffectRow(result->affectRow);
			task->callback->SetSql(task->sql);
		}
	}

	result->callback = task->callback;
	result->conn = m_DbConn;
	DBMgr::Instance()->PushResult(result);
}