#include "DBLogQueryThread.h"

#include <AvalonStringUtil.h>
#include <sstream>
#include <json/writer.h>
#include "ASConsoleNetwork.h"

const char* CLEAR_DBLOGGER_STATEMENT = "DELETE FROM t_log_%s WHERE timestamp < %u";

DBLogQueryThread::DBLogQueryThread(const LogNameMap& lognames)
	:m_DBLogs(lognames)
{
	m_bRun = false;
	m_pDBConn = NULL;
	m_Timer.SetInterval(HOUR_TO_SEC * 1000);
}

DBLogQueryThread::~DBLogQueryThread()
{
	if(m_pDBConn != NULL)
	{
		m_pDBConn->Close();
		Avalon::RetryDBConnection::Destroy(m_pDBConn);
		m_pDBConn = NULL;
	}

	while(!m_RequestQueue.empty())
	{
		DBLogQueryRequest* request = m_RequestQueue.pop();
		delete request;
	}
}

bool DBLogQueryThread::Start(const std::string& host, UInt32 port, const std::string& user, const std::string& passwd, const std::string& dbname)
{
	m_pDBConn = Avalon::RetryDBConnection::Create("mysql", 60, 500);
	if(!m_pDBConn->Open(host.c_str(), port, user.c_str(), passwd.c_str(), dbname.c_str()))
	{
		ErrorLogStream << "connect to log database(" 
			<< host << "," << port << "," << user << "," << passwd << "," << dbname << ") failed!" << LogStream::eos;
		Avalon::RetryDBConnection::Destroy(m_pDBConn);
		m_pDBConn = NULL;
		return false;
	}

	m_bRun = true;
	if(!Activate())
	{
		ErrorLogStream << "activate db log query thread failed!" << LogStream::eos;
		m_bRun = false;
		return false;
	}
	return true;
}

void DBLogQueryThread::Stop()
{
	if(!m_bRun) return;

	m_bRun = false;
	Join();

	if(m_pDBConn != NULL)
	{
		m_pDBConn->Close();
		Avalon::RetryDBConnection::Destroy(m_pDBConn);
		m_pDBConn = NULL;
	}
}

void DBLogQueryThread::PutRequest(DBLogQueryRequest* request)
{
	if(request != NULL) m_RequestQueue.push(request);
}

void DBLogQueryThread::Run()
{
	while(m_bRun || !m_RequestQueue.empty())
	{
		Avalon::Thread::Sleep(10);
		Avalon::Time now = Avalon::Time::CurrentTime();

		while(!m_RequestQueue.empty())
		{
			DBLogQueryRequest* request = m_RequestQueue.pop();
			ProcessQueryRequest(request);

			delete request;
		}

		if(m_Timer.IsTimeout(now))
		{
			for(LogNameMap::iterator iter = m_DBLogs.begin();
				iter != m_DBLogs.end(); ++iter)
			{
				UInt32 duetime = UInt32(now.Sec()) - iter->second;
				char sqlbuff[128];
				avalon_snprintf(sqlbuff, 128, CLEAR_DBLOGGER_STATEMENT, iter->first.c_str(), duetime);
				if(m_pDBConn->Update(sqlbuff) < 0)
				{
					ErrorLogStream << "clear log " << iter->first << " failed!" << LogStream::eos;
				}
			}
		}
	}
}

void DBLogQueryThread::ProcessQueryRequest(DBLogQueryRequest* request)
{
	if(request->page == 0) request->page = 1;
	if(request->pagesize == 0) request->pagesize = 1;
	if(request->pagesize > 100) request->pagesize = 100;

	std::ostringstream sqlstream;
	sqlstream << "SELECT * FROM `t_log_" << request->type << "` WHERE";
	if(request->roleid != 0) sqlstream << " `playerid`=" << request->roleid << " AND";
	sqlstream << " `timestamp` >= " << request->beginTime << " AND `timestamp` <= " << request->endTime;
	if(request->itemId != 0)
	{
		sqlstream << " AND `params` LIKE 'itemid=" << request->itemId << "|%'" << request->itemId;
	}
	sqlstream << " ORDER BY timestamp DESC LIMIT " << (request->page - 1) * request->pagesize << "," << request->pagesize;

	Avalon::ResultSet* resultSet = m_pDBConn->Query(sqlstream.str());
	if(resultSet == NULL)
	{
		ErrorLogStream << "execute select statement failed:" << sqlstream.str() << LogStream::eos;
		return;
	}

	json::Object body;
	body["logs"] = json::Array();
	json::Array& logs = body["logs"];
	while(resultSet->NextRow())
	{
		json::Object obj;
		obj["accid"] = json::String(Avalon::StringUtil::ConvertToString<UInt32>(resultSet->GetField("accid").GetUInt32()));
		obj["playerid"] = json::String(Avalon::StringUtil::ConvertToString<UInt64>(resultSet->GetField("playerid").GetUInt64()));
		obj["playername"] = json::String(resultSet->GetField("playername").GetString());
		obj["timestamp"] = json::String(Avalon::StringUtil::ConvertToString<UInt32>(resultSet->GetField("timestamp").GetUInt32()));
		obj["params"] = json::String(resultSet->GetField("params").GetString());
		logs.Insert(obj);
	}
	m_pDBConn->FreeResultSet(resultSet);

	std::ostringstream sqlstream1;
	sqlstream1 << "SELECT count(*) FROM `t_log_" << request->type << "` WHERE";
	if(request->roleid != 0) sqlstream1 << " `playerid`=" << request->roleid << " AND";
	sqlstream1 << " `timestamp` >= " << request->beginTime << " AND `timestamp` <= " << request->endTime;
	if(request->itemId != 0)
	{
		sqlstream << " AND `params` LIKE 'itemid=" << request->itemId << "|%'" << request->itemId;
	}
	resultSet = m_pDBConn->Query(sqlstream1.str());
	if(resultSet == NULL || !resultSet->NextRow())
	{
		ErrorLogStream << "execute select statement failed:" << sqlstream1.str() << LogStream::eos;
		m_pDBConn->FreeResultSet(resultSet);
		return;
	}
	body["total"] = json::String(Avalon::StringUtil::ConvertToString<UInt32>(resultSet->GetField(0).GetUInt32()));
	m_pDBConn->FreeResultSet(resultSet);

	Avalon::NetConnectionPtr conn = ConsoleNetwork::Instance()->FindConnection(request->connid);
	if(conn == NULL)
	{
		GameErrorLogStream("Console") << "can not find connection " << request->connid << " when send response!" << LogStream::eos;
		return;
	}

	json::Object head;
	head["cmd"] = json::String("gameControl");
	head["para"] = json::String("logQuery");
	head["length"] = json::String("");
	head["timestamp"] = json::String(Avalon::StringUtil::ConvertToString<UInt32>(UInt32(Avalon::Time::CurrentTime().Sec())));
	head["len"] = json::String("");

	json::Object response;
	response["head"] = head;
	response["body"] = body;

	std::ostringstream jsonstream;
	try
	{
		json::Writer::Write(response, jsonstream);
	}
	catch(json::Exception&)
	{
		return;
	}

	UInt32 jsonlen = jsonstream.str().length() + 1;
	conn->Send(&jsonlen, sizeof(jsonlen));
	conn->Send(jsonstream.str().c_str(), jsonlen);
	conn->Close();
}
