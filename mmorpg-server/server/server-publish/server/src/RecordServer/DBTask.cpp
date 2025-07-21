#include "DBTask.h"
#include "DBMgr.h"
#include "RSApplication.h"

DBTask::DBTask(SqlType type,const std::string& sql, DBRequestCallback* cb)
: m_Type(type), m_Sql(sql), m_CallBack(cb)
{

}

DBTask::~DBTask()
{

}

void DBTask::Run(Avalon::WorkerThread* thread)
{
	auto worker = dynamic_cast<DBWorkerThread*>(thread);
	if (!worker)
	{
		return;
	}
	
	auto conn = worker->GetDBConn();
	if (!conn)
	{
		ErrorLogStream << "no db connection to execute sql(" << m_Sql << ")." << LogStream::eos;
		return;
	}

	DBResult* result = new DBResult();
#ifdef __RECORD_SQL__
	DebugLogStream << "execute sql:" << m_Sql << "." << LogStream::eos;
#endif
	if (m_Type == SQL_SELECT)
	{
		auto resultSet = conn->Query(m_Sql);
		if (!resultSet)
		{
			ErrorLogStream << "execute sql failed:" << conn->GetError() << " sql:" << m_Sql << "!" << LogStream::eos;
			RSApplication::Instance()->Exit();
			return;
		}
		result->resultSet = resultSet;
		if (m_CallBack)
		{
			m_CallBack->SetSql(m_Sql);
		}
	}
	else
	{
		result->affectRow = conn->Update(m_Sql);
		if (m_CallBack)
		{
			m_CallBack->SetAffectRow(result->affectRow);
			m_CallBack->SetSql(m_Sql);
		}
	}

	result->callback = m_CallBack;
	result->conn = conn;
	DBMgr::Instance()->PushResult(result);
}

void DBTask::Release()
{

}