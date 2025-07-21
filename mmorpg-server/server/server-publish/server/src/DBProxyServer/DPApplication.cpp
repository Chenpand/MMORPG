#include "DPApplication.h"

#include <AvalonLogger.h>

#include "DPNetwork.h"

#include "GameZoneMgr.h"

Application::Application()
{
	m_TickTime = 0;
}

Application::~Application()
{
}

bool Application::OnInit()
{
	if(!CLApplication::OnInit()) return false;

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
}

void Application::OnQuit()
{
	GameZoneMgr::Instance()->Final();

	Network::Instance()->Shutdown(200);

	CLApplication::OnQuit();
}