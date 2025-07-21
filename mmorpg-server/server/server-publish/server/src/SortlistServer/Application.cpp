#include "Application.h"

#include <AvalonLogger.h>

#include "Network.h"
#include "GameZoneMgr.h"
#include "SortListMgr.h"

Application::Application()
{
	m_TickTime = 0;
	m_DBConn = NULL;
}

Application::~Application()
{
}

bool Application::OnInit()
{
	if(!CLApplication::OnInit()) return false;

	std::string db_ip = GetProperties()->GetProperty("Database", "db_ip").GetString();
	UInt32 db_port = GetProperties()->GetProperty("Database", "db_port");
	std::string db_user = GetProperties()->GetProperty("Database", "db_user").GetString();
	std::string db_passwd = GetProperties()->GetProperty("Database", "db_passwd").GetString();
	std::string db_name = GetProperties()->GetProperty("Database", "db_name").GetString();
	m_DBConn = Avalon::RetryDBConnection::Create("mysql", 20, 500);
	if(!m_DBConn->Open(db_ip.c_str(), db_port, db_user.c_str(), db_passwd.c_str(), db_name.c_str()))
	{
		ErrorLogStream << "connect to database " << db_ip << ":" << db_port << " " << db_name << "!" << LogStream::eos;
		Avalon::RetryDBConnection::Destroy(m_DBConn);
		return false;
	}

	if(!Network::Instance()->Startup(GetProperties(), GetNetAddress()))
	{
		ErrorLogStream << "internal network startup failed!" << LogStream::eos;
		return false;
	}

	if(!GameZoneMgr::Instance()->Init())
	{
		ErrorLogStream << "gamezonemgr init failed!" << LogStream::eos;
		return false;
	}

	if(!SortListMgr::Instance()->Init())
	{
		ErrorLogStream << "sortlistmgr init failed!" << LogStream::eos;
		return false;
	}

	m_TickTime = Avalon::Time::CurrentTime().MSec();
	return true;
}

void Application::OnTick(const Avalon::Time &now)
{
	CLApplication::OnTick(now);

	UInt64 offsetTime = now.MSec() - m_TickTime;
	if(offsetTime > 1000)
	{
		ErrorLogStream << "too long tick time:" << offsetTime << " msecs!" << LogStream::eos;
	}
	m_TickTime = now.MSec();

	Network::Instance()->OnTick(now);
	GameZoneMgr::Instance()->OnTick(now);
	SortListMgr::Instance()->OnTick(now);
}

void Application::OnQuit()
{
	SortListMgr::Instance()->Final();
	GameZoneMgr::Instance()->Final();
	Network::Instance()->Shutdown(200);

	Avalon::RetryDBConnection::Destroy(m_DBConn);
	m_DBConn = NULL;

	CLApplication::OnQuit();
}
