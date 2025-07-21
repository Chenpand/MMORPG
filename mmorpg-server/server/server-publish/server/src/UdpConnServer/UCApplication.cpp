#include "UCApplication.h"
#include <PerformanceCheck.h>

#include "UCExternalNetwork.h"
#include "UCPlayerMgr.h"
#include "UCPlayerMonitorMgr.h"


UCApplication::UCApplication()
{
	m_ReloadConfig = false;
	m_UdpRecvTimeout = 1;
}

UCApplication::~UCApplication()
{

}

bool UCApplication::OnInit()
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
	if (!UdpNetworks::Instance()->Startup(GetProperties()))
	{
		ErrorLogStream << "udp network startup failed!" << LogStream::eos;
		return false;
	}

	UCPlayerMonitorMgr::Instance()->LoadFromString(GetProperties()->GetProperty("Game", "monitor_list").GetString());
	LoadDynamicConfig();
	
	DebugLogStream << "Application OnInit success!" << LogStream::eos;
	return true;
}

void UCApplication::OnTick(const Avalon::Time& now)
{
	Avalon::Time startTime = Avalon::Time::CurrentTime();

	RESET_PERF();

	PERF_SEG_BEGIN(UdpNetworks);
	//external network
	UdpNetworks::Instance()->OnTick(now);
	PERF_SEG_END();

	PERF_SEG_BEGIN(UCPlayerMgr);
	UCPlayerMgr::Instance()->OnTick(now);
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

		if (!GetProperties()->LoadFromFile("UdpConnServer.cfg"))
		{
			ErrorLogStream << "load UdpConnServer.cfg failed!" << LogStream::eos;
			return;
		}

		UCPlayerMonitorMgr::Instance()->LoadFromString(GetProperties()->GetProperty("Game", "monitor_list").GetString());
		LoadDynamicConfig();
	}
}

void UCApplication::OnSignal(int signal)
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

void UCApplication::OnQuit()
{
	DebugLogStream << "Application to  OnQuit!" << LogStream::eos;

	//关闭udp 网络层
	UdpNetworks::Instance()->Shutdown();

	//
	UCPlayerMgr::Instance()->OnQuit();

	CLApplication::OnQuit();
	//DebugLogStream << "Application OnQuit success!" << LogStream::eos;
}

void UCApplication::NotifyPing(UCPlayer* player)
{
	UdpNetworks::Instance()->NotifyClientPing(player);
}

void UCApplication::LoadDynamicConfig()
{
	UInt32 packetNumPerTick = GetProperties()->GetProperty("Game", "packet_num_pertick");
	if (packetNumPerTick != 0)
	{
		MAX_PACKET_PERTICK = packetNumPerTick;
	}

	m_UdpRecvTimeout = GetProperties()->GetProperty("Game", "udp_recv_timeout");

	m_timeoutLimit = GetProperties()->GetProperty("Game", "timeoutLimit");
	m_timeoutMinimum = GetProperties()->GetProperty("Game", "timeoutMinimum");
	m_timeoutMaximum = GetProperties()->GetProperty("Game", "timeoutMaximum");

	InfoLogStream << "packet_num_pertick(" << MAX_PACKET_PERTICK << ") udp_recv_timeout(" << m_UdpRecvTimeout << ") timeoutLimit(" 
					<< m_timeoutLimit << ") timeoutMinimum(" << m_timeoutMinimum << ") timeoutMaximum(" << m_timeoutMaximum << ")." << LogStream::eos;
}


