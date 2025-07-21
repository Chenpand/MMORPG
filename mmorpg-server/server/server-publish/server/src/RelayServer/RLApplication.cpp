#include "RLApplication.h"
#include <kingnet_analyzer.h>
#include <CLGameDefine.h>
#include <AvalonApplication.h>
#include <PerformanceCheck.h>
#include <CLSysNotifyDataEntry.h>
#include <CLSysNotify.h>
#include <CLDungeonVerifyProbDataEntry.h>

#include "RLChannels.h"
#include "RLInternalNetwork.h"
#include "RLPlayerMgr.h"
#include "RaceMgr.h"
#include "RLPlayerMonitorMgr.h"

RLApplication::RLApplication()
{
	m_ReloadConfig = false;
	SetIsSendUnsyncNotify(false);
	m_Version = 0;
	m_IsOpenReplay = false;
	m_OpenVerifyDungeon = true;
	m_OpenVerifyPK = false;
	m_ObserveWaitSec = 0;
	m_WaitVerifyRaceEndSec = 0;
}

RLApplication::~RLApplication()
{

}

bool RLApplication::OnInit()
{
	if (!CLApplication::OnInit())
	{
		return false;
	}

	std::string configpath = CONFIG_PATH;
	std::string rootpath = ROOT_PATH;

	if (!GetProperties()->LoadFromFile(rootpath + "Game.cfg"))
	{
		ErrorLogStream << "load game config failed!" << LogStream::eos;
		return false;
	}

	LoadDynamicConfig();

	if (UL_FAIL == _analyzer.Init() || NULL == _analyzer.GetInstance(UDPLOG_GATE_UID))
	{
		ErrorLogStream << "udplog init failed!" << LogStream::eos;
		return false;
	}

	if (!RLChannels::Instance()->Init(GetProperties()))
	{
		ErrorLogStream << "RLChannels init failed!" << LogStream::eos;
		return false;
	}

	//
	if (!RLInternalNetwork::Instance()->Startup(GetProperties(), GetNetAddress()))
	{
		ErrorLogStream << "RLInternalNetwork Startup failed!" << LogStream::eos;
		return false;
	}

	if (!RLPlayerMgr::Instance()->Init(GetProperties()))
	{
		ErrorLogStream << "RLPlayerMgr Init failed!" << LogStream::eos;
		return false;
	}

	if (!RaceMgr::Instance()->OnInit(GetProperties()))
	{
		ErrorLogStream << "RaceMgr Init failed!" << LogStream::eos;
		return false;
	}

	if (!LoadDataTables())
	{
		ErrorLogStream << "load tables failed!" << LogStream::eos;
		return false;
	}
	
    memCheckTimer.SetLastTime(Avalon::Time::CurrentTime().MSec());
    memCheckTimer.SetInterval(Avalon::Time::SECS_OF_MIN * Avalon::Time::MSECS_OF_SEC);

	RLPlayerMonitorMgr::Instance()->LoadFromString(GetProperties()->GetProperty("Game", "monitor_list").GetString());

	m_IsSendUnsyncNotify = GetProperties()->GetProperty("Game", "send_unsync_notify");

	InfoLogStream << "RLApplication::OnInit success!" << LogStream::eos;

	return true;
}

void RLApplication::OnTick(const Avalon::Time& now)
{
    Avalon::Time startTime = Avalon::Time::CurrentTime();

    RESET_PERF();

    PERF_SEG_BEGIN(CLApplication);
	CLApplication::OnTick(now);
    PERF_SEG_END();

    PERF_SEG_BEGIN(RLPlayerMgr);
	RLPlayerMgr::Instance()->OnTick(now);
    PERF_SEG_END();

    PERF_SEG_BEGIN(RLChannels);
	RLChannels::Instance()->OnTick(now);
    PERF_SEG_END();

    PERF_SEG_BEGIN(RLInternalNetwork);
	RLInternalNetwork::Instance()->OnTick(now);
    PERF_SEG_END();

    PERF_SEG_BEGIN(RaceMgr);
    RaceMgr::Instance()->OnTick(now);
    PERF_SEG_END();

    UInt64 offsetTime = Avalon::Time::CurrentTime().MSec() - startTime.MSec();
    if (offsetTime >= 300)
    {
        ErrorLogStream << "Too long tick time:" << offsetTime << " msecs!" << LogStream::eos;
        SHOW_PERF();
    }

    if(memCheckTimer.IsTimeout(now))
    {
        REPORT_MEMORY(gSysLogger);
    }

	if (m_ReloadConfig)
	{
		m_ReloadConfig = false;

		if (!GetProperties()->LoadFromFile("RelayServer.cfg"))
		{
			ErrorLogStream << "load RelayServer.cfg failed!" << LogStream::eos;
			return;
		}

		std::string rootpath = ROOT_PATH;
		if (!GetProperties()->LoadFromFile(rootpath + "Game.cfg"))
		{
			ErrorLogStream << "load game config failed!" << LogStream::eos;
			return;
		}

		LoadDynamicConfig();

		std::string configpath = CONFIG_PATH;
		configpath = configpath + "DataTables/";
		try
		{
			DungeonVerifyProbDataEntryMgr::Instance()->ReloadData(configpath + "DungeonVerifyProbTable.txt");
		}
		catch (std::string str)
		{
			ErrorLogStream << str << LogStream::eos;
		}
		
	}

	return;
}

void RLApplication::OnQuit()
{
	//other system OnQuit 
	//add here
	//
	RLChannels::Instance()->OnQuit();

	RLInternalNetwork::Instance()->Shutdown(100);

	RLPlayerMgr::Instance()->OnQuit();

	CLApplication::OnQuit();
	return;
}

void RLApplication::OnSignal(int signal)
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

Avalon::SockAddr RLApplication::GetAddr()
{
	Avalon::SockAddr addr;
	std::string ip = RLApplication::Instance()->GetProperties()->GetProperty("Server", "ip").GetString();
	UInt16 port = RLApplication::Instance()->GetProperties()->GetProperty("Server", "port");
	addr.SetIP(ip);
	addr.SetPort(port);

	return addr;
}

bool RLApplication::IsDungeonNeedWaitVerifyRaceEnd(UInt32 dungeonid)
{
	return m_WaitVerifyRaceEndDungeons.find(dungeonid) != m_WaitVerifyRaceEndDungeons.end();
}

bool RLApplication::LoadDataTables()
{
	std::string configpath = CONFIG_PATH;
	configpath = configpath + "DataTables/";

	try
	{
		if (!SysNotifyDataEntryMgr::Instance()->LoadData(configpath + "CommonTipsDesc.txt")) return false; 
		SysNotifyMgr::Instance()->Load(NULL, std::string());
		if (!DungeonVerifyProbDataEntryMgr::Instance()->LoadData(configpath + "DungeonVerifyProbTable.txt")) return false;
	}
	catch (std::string str)
	{
		ErrorLogStream << str << LogStream::eos;
		return false;
	}

	return true;
}

void RLApplication::LoadDynamicConfig()
{
	InfoLogStream << "Begin load dynamic config." << LogStream::eos;

	LoadVersion();

	RLPlayerMonitorMgr::Instance()->LoadFromString(GetProperties()->GetProperty("Game", "monitor_list").GetString());
	m_IsSendUnsyncNotify = GetProperties()->GetProperty("Game", "send_unsync_notify");

	UInt32 maxPlayer = GetProperties()->GetProperty("server", "capacity");
	RLPlayerMgr::Instance()->set_max_player_num(maxPlayer);

	m_IsOpenReplay = GetProperties()->GetProperty("Game", "open_replay");

	m_OpenVerifyDungeon = !GetProperties()->GetProperty("Verify", "CloseVerify");
	m_OpenNewChecksumCheck = GetProperties()->GetProperty("Verify", "OpenNewChecksumCheck");
	m_KickPlayerWhenDungeonUnsync = GetProperties()->GetProperty("Verify", "KickPlayerWhenDungeonUnsync");

	InfoLogStream << "OpenNewChecksumCheck(" << m_OpenNewChecksumCheck << ") KickPlayerWhenDungeonUnsync(" << m_KickPlayerWhenDungeonUnsync << ")." << LogStream::eos;
	m_OpenVerifyPK = GetProperties()->GetProperty("Verify", "VerifyPK");
	m_NotifyGiveupVerifyWhenRaceUnexist = !GetProperties()->GetProperty("Verify", "CloseNotifyGiveupVerifyWhenRaceUnexist");

	m_OpenHeartbeat = GetProperties()->GetProperty("Heartbeat", "OpenHeartbeat");
	m_HeartbeatTimeoutSec = GetProperties()->GetProperty("Heartbeat", "HeartbeatTimeoutSec");
	m_HeartbeatIntervalSec = GetProperties()->GetProperty("Heartbeat", "HeartbeatIntervalSec");
	m_ObserveWaitSec = GetProperties()->GetProperty("Game", "ObserveWaitSec");
	InfoLogStream << "OpenHeartbeat(" << m_OpenHeartbeat << ") HeartbeatTimeoutSec(" << m_HeartbeatTimeoutSec 
					<< ") HeartbeatIntervalSec(" << m_HeartbeatIntervalSec << ")." << LogStream::eos;

	{
		m_WaitVerifyRaceEndSec = GetProperties()->GetProperty("Verify", "WaitVerifyRaceEndSecond");
		auto dungeonStrs = GetProperties()->GetProperty("Verify", "WaitVerifyDungeons").GetString();
		InfoLogStream << "WaitVerifyRaceEndSecond(" << m_WaitVerifyRaceEndSec << ") WaitVerifyDungeons(" << dungeonStrs << ")" << LogStream::eos;

		std::vector<std::string> strs;
		Avalon::StringUtil::Split(dungeonStrs, strs, "|");
		for (auto& str : strs)
		{
			UInt32 dungeonid = Avalon::StringUtil::ConvertToValue<UInt32>(str);
			if (dungeonid > 0)
			{
				m_WaitVerifyRaceEndDungeons.insert(dungeonid);
			}
		}
	}
}

void RLApplication::LoadVersion()
{
	std::string versionStr = GetProperties()->GetProperty("Server", "version").GetString();
	GAME_VERSION = versionStr;
	if (!ReadVersionFromStr(versionStr))
	{
		ErrorLogStream << "invalid server version(" << versionStr << ")." << LogStream::eos;
		return;
	}
}

bool RLApplication::ReadVersionFromStr(std::string str)
{
	std::vector<std::string> strs;
	Avalon::StringUtil::Split(str, strs, ".");
	if (strs.size() != 3)
	{
		return false;
	}

	UInt32 major = Avalon::StringUtil::ConvertToValue<UInt32>(strs[0]);
	UInt32 minor = Avalon::StringUtil::ConvertToValue<UInt32>(strs[1]);
	UInt32 develop = Avalon::StringUtil::ConvertToValue<UInt32>(strs[2]);
	m_Version = MakeVersion(major, minor, develop);

	InfoLogStream << "Make version(" << m_Version << ") from str(" << str << ")." << LogStream::eos;
	return true;
}

UInt32 RLApplication::MakeVersion(UInt32 major, UInt32 minor, UInt32 develop)
{
	return major << 24 | minor << 16 | (develop & (0xFFFF));
}
