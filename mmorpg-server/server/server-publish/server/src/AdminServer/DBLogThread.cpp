#include "DBLogThread.h"

#include <CLSysProtocol.h>
#include <sstream>

DBLogThread::DBLogThread(const LogNameMap& logs)
	:m_DBLogs(logs)
{
	m_bRun = false;
	m_pDBConn = NULL;
}

DBLogThread::~DBLogThread()
{
	if(m_pDBConn != NULL)
	{
		m_pDBConn->Close();
		Avalon::RetryDBConnection::Destroy(m_pDBConn);
		m_pDBConn = NULL;
	}

	while(!m_LogQueue.empty())
	{
		Avalon::Packet* packet = m_LogQueue.pop();
		Avalon::Packet::Destroy(packet);
	}
}

bool DBLogThread::Start(const std::string& host, UInt32 port, const std::string& user, const std::string& passwd, const std::string& dbname)
{
	m_pDBConn = Avalon::RetryDBConnection::Create("mysql", 60 ,500);
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
		ErrorLogStream << "activate db log thread failed!" << LogStream::eos;
		m_bRun = false;
		return false;
	}
	return true;
}

void DBLogThread::Stop()
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

void DBLogThread::PutLogPacket(Avalon::Packet* packet)
{
	if(packet != NULL) m_LogQueue.push(packet);
}

void DBLogThread::Run()
{
	while(m_bRun || !m_LogQueue.empty())
	{
		Avalon::Thread::Sleep(10);

		while(!m_LogQueue.empty())
		{
			Avalon::Packet* packet = m_LogQueue.pop();
			ProcessLogPacket(packet);

			Avalon::Packet::Destroy(packet);
		}
	}
}

void DBLogThread::ProcessLogPacket(Avalon::Packet* packet)
{
	if(m_pDBConn == NULL) return;
	CLProtocol::SysDBLogger req;
	if(!req.Decode(packet)) return;

	if(m_DBLogs.find(req.name) == m_DBLogs.end())
	{
		ErrorLogStream << "wrong dblogger name:" << req.name << "!" << LogStream::eos;
		return;
	}

	if(req.playername.length() > 32) return;
	char namebuf[128];
	namebuf[m_pDBConn->EscapeString(namebuf, req.playername.c_str(), req.playername.length())] = 0;

	std::ostringstream paramstream;
	bool bFirst = true;
	for(std::map<std::string, std::string>::iterator iter = req.params.begin();
		iter != req.params.end(); ++iter)
	{
		if(bFirst) bFirst = false;
		else paramstream << "|";
		paramstream << iter->first << "=" << iter->second;
	}
	if(paramstream.str().length() >= 1024) return;
	char buf[2048];
	size_t len = m_pDBConn->EscapeString(buf, paramstream.str().c_str(), paramstream.str().length());
	buf[len] = 0;

	std::ostringstream sqlstream;
	sqlstream << "INSERT INTO `t_log_" << req.name << "`(`accid`,`playerid`,`playername`,`timestamp`,`params`)"
		" values(" << req.accid << "," << req.playerid << ",'" << namebuf << "'," << req.timestamp << ",'";
	sqlstream << buf << "')";

	if(m_pDBConn->Update(sqlstream.str()) < 0)
	{
		ErrorLogStream << "execute sql statement failed:" << sqlstream.str() << "!" << LogStream::eos;
		m_pDBConn->Close();
		Avalon::RetryDBConnection::Destroy(m_pDBConn);
		m_pDBConn = NULL;
	}
}
