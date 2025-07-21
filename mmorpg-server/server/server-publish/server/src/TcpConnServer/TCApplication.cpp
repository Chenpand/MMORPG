#include "TCApplication.h"

#include <PerformanceCheck.h>

#include "TCExternalNetwork.h"
#include "TCPlayerMgr.h"

#include "TCPlayerMonitorMgr.h"

TCApplication::TCApplication()
{
	m_ReloadConfig = false;
}

TCApplication::~TCApplication()
{

}

bool TCApplication::OnInit()
{
	if (!CLApplication::OnInit()) //调用成功后，日志系统就可使用了:)
	{
		return false;
	}

	//std::string configpath = CONFIG_PATH;
	//std::string rootpath = ROOT_PATH;

	// 	if (!make_channels())
	// 	{
	// 		ErrorLogStream << "make channels failed!" << LogStream::eos;
	// 		return false;
	// 	}

	//
	//if (!UdpNetworks::Instance()->Startup(GetProperties()))
	//{
	//	ErrorLogStream << "udp network startup failed!" << LogStream::eos;
	//	return false;
	//}

	if (!TCExternalNetwork::Instance()->Startup(GetProperties()))
	{
		ErrorLogStream << "TCExternalNetwork startup failed!" << LogStream::eos;
		return false;
	}

	TCPlayerMonitorMgr::Instance()->LoadFromString(GetProperties()->GetProperty("Game", "monitor_list").GetString());

	DebugLogStream << "Application OnInit success!" << LogStream::eos;
	return true;
}

void TCApplication::OnTick(const Avalon::Time& now)
{
	Avalon::Time startTime = Avalon::Time::CurrentTime();

	RESET_PERF();


	//external network
	//UdpNetworks::Instance()->OnTick(now);
	PERF_SEG_BEGIN(TCExternalNetwork);
	TCExternalNetwork::Instance()->OnTick(now);
	PERF_SEG_END();

	PERF_SEG_BEGIN(UCPlayerMgr);
	TCPlayerMgr::Instance()->OnTick(now);
	PERF_SEG_END();

	UInt64 offsetTime = Avalon::Time::CurrentTime().MSec() - startTime.MSec();
	if (offsetTime >= 50)
	{
		ErrorLogStream << "Too long tick time:" << offsetTime << " msecs!" << LogStream::eos;
		SHOW_PERF();
	}

	if (m_ReloadConfig)
	{
		m_ReloadConfig = false;

		if (!GetProperties()->LoadFromFile("TcpConnServer.cfg"))
		{
			ErrorLogStream << "load UdpConnServer.cfg failed!" << LogStream::eos;
			return;
		}

		TCPlayerMonitorMgr::Instance()->LoadFromString(GetProperties()->GetProperty("Game", "monitor_list").GetString());
	}
}


void TCApplication::OnSignal(int signal)
{
	InfoLogStream << "recv signal " << signal << LogStream::eos;

	switch (signal)
	{
	case SIG_RELOAD_CONFIG:
	{
							  m_ReloadConfig = true;
							  break;
	}
	default:
	{
			   ServerApplication::OnSignal(signal);
			   break;
	}
	}
}

void TCApplication::OnQuit()
{
	DebugLogStream << "Application to  OnQuit!" << LogStream::eos;

	//关闭udp 网络层
	//UdpNetworks::Instance()->Shutdown();

	TCExternalNetwork::Instance()->Shutdown(0);

	//
	TCPlayerMgr::Instance()->OnQuit();

	CLApplication::OnQuit();
	//DebugLogStream << "Application OnQuit success!" << LogStream::eos;
}




