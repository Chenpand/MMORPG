#include "DBMgr.h"

DBMgr::DBMgr()
{

}

DBMgr::~DBMgr()
{

}

bool DBMgr::Init(int threadNum)
{
	/*if (!this->Open(threadNum))
	{
		ErrorLogStream << "open db woker thread failed." << LogStream::eos;
		return false;
	}*/

	return true;
}

bool DBMgr::OnTick(const Avalon::Time& now)
{
	UInt32 num = 0;
	while (num++ <= 100)
	{
		auto result = PopDBResult();
		if (!result)
		{
			break;
		}

		auto callback = result->callback;
		if (callback)
		{
			callback->OnSuccess(result->resultSet);

			delete callback;
			result->callback = NULL;
		}
		
		if (result->conn && result->resultSet)
		{
			result->conn->FreeResultSet(result->resultSet);
		}

		delete result;
	}
	return true;
}

void DBMgr::Final()
{

}

void DBMgr::ExcuteSql(const std::string& sql, SqlType type, DBRequestCallback* cb)
{
	DBTask* task = new DBTask(type, sql, cb);
	this->PostTask(task);

#ifdef __RECORD_SQL__
	DebugLogStream << "execute sql:" << sql << LogStream::eos;
#endif
}

void DBMgr::PushResult(DBResult* result)
{
	if (!result)
	{
		return;
	}

	m_ResultListMutex.Acquire();
	m_ResultList.push_back(result);
	m_ResultListMutex.Release();
}

Avalon::WorkerThread* DBMgr::CreateWorkerThread()
{
	return new DBWorkerThread(this);
}

DBResult* DBMgr::PopDBResult()
{
	DBResult* result = NULL;
	m_ResultListMutex.Acquire();

	if (!m_ResultList.empty())
	{
		auto itr = m_ResultList.begin();
		result = *itr;
		m_ResultList.pop_front();
	}

	m_ResultListMutex.Release();

	return result;
}