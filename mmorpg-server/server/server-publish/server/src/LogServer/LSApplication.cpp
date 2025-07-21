#include "LSApplication.h"

#include <CLDataLogMgr.h>
#include "LSNetwork.h"


LSApplication::LSApplication()
{
}

LSApplication::~LSApplication()
{
}

bool LSApplication::OnInit()
{
	if(!Avalon::ServerApplication::OnInit()) return false;

	if(!LogNetwork::Instance()->Startup(GetProperties()))
	{
		ErrorLogStream << "network startup failed!" << LogStream::eos;
		return false;
	}
	
	if(!CLDataLogMgr::Instance()->Init("udplog.xml")) //参见 ..\Config\udplog.xml.new 以及.\DataLogServer\udplog.xml
	{
		ErrorLogStream << "loggers init failed!" << LogStream::eos;
		return false;
	}

	return true;
}

void LSApplication::OnTick(const Avalon::Time &now)
{
	Avalon::ServerApplication::OnTick(now);

	LogNetwork::Instance()->OnTick(now);
}

void LSApplication::OnQuit()
{
	LogNetwork::Instance()->Shutdown(0);

	CLDataLogMgr::Instance()->Final();
	Avalon::ServerApplication::OnQuit();
}
