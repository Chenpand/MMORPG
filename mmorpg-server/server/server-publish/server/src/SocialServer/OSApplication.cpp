#include "OSApplication.h"

#include "OSNetwork.h"
#include "OnlineStateMgr.h"
#include "InviteDataMgr.h"
#include "CDKeyMgr.h"

OSApplication::OSApplication()
{
	m_TickTime = 0;
}

OSApplication::~OSApplication()
{
}

bool OSApplication::OnInit()
{
	if(!CLApplication::OnInit()) return false;

	if(!OSNetwork::Instance()->Startup(GetProperties(), GetNetAddress()))
	{
		ErrorLogStream << "network startup failed!" << LogStream::eos;
		return false;
	}

	if(!InviteDataMgr::Instance()->Init(GetProperties()))
	{
		ErrorLogStream << "invitedatamgr init failed!" << LogStream::eos;
		return false;
	}

	if(!CDKeyMgr::Instance()->Init(GetProperties()))
	{
		ErrorLogStream << "cdkeymgr init failed!" << LogStream::eos;
		return false;
	}

	m_TickTime = Avalon::Time::CurrentTime().MSec();
	return true;
}

void OSApplication::OnTick(const Avalon::Time &now)
{
	CLApplication::OnTick(now);

	OSNetwork::Instance()->OnTick(now);
	OnlineStateMgr::Instance()->OnTick(now);
	CDKeyMgr::Instance()->OnTick(now);

	UInt64 offsetTime = now.MSec() - m_TickTime;
	if(offsetTime > 1000)
	{
		ErrorLogStream << "Too long tick time:" << offsetTime << " msecs!" << LogStream::eos;
	}
	m_TickTime = now.MSec();
}

void OSApplication::OnQuit()
{
	CDKeyMgr::Instance()->Final();
	InviteDataMgr::Instance()->Final();
	OSNetwork::Instance()->Shutdown(0);

	CLApplication::OnQuit();
}



