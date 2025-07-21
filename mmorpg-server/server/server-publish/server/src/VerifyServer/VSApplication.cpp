#include "VSApplication.h"

#include <AvalonMemoryTracker.h>
#include <AvalonLogger.h>
#include <PerformanceCheck.h>
#include <CLGameLoggerMgr.h>
#include <CLRecordClient.h>
#include <CLDungeonDataEntry.h>

#include "VerifyMgr.h"
#include "VSInternalNetwork.h"

VSApplication::VSApplication()
{
	m_ReloadConfig = false;
	m_DumpMemory = false;
}

VSApplication::~VSApplication()
{
}

UInt32 VSApplication::GetAbility()
{
	UInt32 curNum = VerifyMgr::Instance()->GetVerifyTaskNum();
	if (curNum >= m_MaxVerifyTask)
	{
		return 0;
	}
	else
	{
		return m_MaxVerifyTask - curNum;
	}
}

bool VSApplication::OnInit()
{
	if (!CLApplication::OnInit()) return false;

	std::string configpath = CONFIG_PATH;
	std::string rootpath = ROOT_PATH;
	if (!GetProperties()->LoadFromFile(rootpath + "Game.cfg"))
	{
		ErrorLogStream << "load game config failed!" << LogStream::eos;
		return false;
	}

	if (!GameLoggerMgr::Instance()->Init(GAME_LOGGER_CFG, IsDebugMode()))
	{
		ErrorLogStream << "game loggermgr init failed!" << LogStream::eos;
		return false;
	}

	if (!VSInternalNetwork::Instance()->Startup(GetProperties(), GetNetAddress()))
	{
		ErrorLogStream << "internal network startup failed!" << LogStream::eos;
		return false;
	}

	if (!VerifyMgr::Instance()->Init())
	{
		ErrorLogStream << "VerifyMgr init failed!" << LogStream::eos;
		return false;
	}

	LoadDynamicConfig();

	InfoLogStream << GetName() << " init success!" << LogStream::eos;

	return true;
}

void VSApplication::OnTick(const Avalon::Time &now)
{
	static UInt64 s_TickTime = 0;
	s_TickTime = now.MSec();

	PERF_SEG_BEGIN(CLApplication);
	CLApplication::OnTick(now);
	PERF_SEG_END();

	PERF_SEG_BEGIN(VSInternalNetwork);
	VSInternalNetwork::Instance()->OnTick(now);
	PERF_SEG_END();

	PERF_SEG_BEGIN(VerifyMgr);
	VerifyMgr::Instance()->OnTick(now);
	PERF_SEG_END();

	UInt64 offsetTime = Avalon::Time::CurrentTime().MSec() - s_TickTime;
	if (offsetTime > 1000)
	{
		ErrorLogStream << "too long tick time:" << offsetTime << " msecs!" << LogStream::eos;
		SHOW_PERF();
	}

	if (m_ReloadConfig)
	{
		m_ReloadConfig = false;

		if (!GetProperties()->LoadFromFile("VerifyServer.cfg"))
		{
			ErrorLogStream << "load VerifyServer.cfg failed!" << LogStream::eos;
			return;
		}

		LoadDynamicConfig();
	}

	if (m_DumpMemory)
	{
		m_DumpMemory = false;
		InfoLogStream << "start to dump memory..." << LogStream::eos;
		VerifyMgr::Instance()->LC_DumpMemory();
		InfoLogStream << "dump memory finished..." << LogStream::eos;
	}
}

bool VSApplication::LoadDataTables()
{
	std::string configpath = CONFIG_PATH;
	configpath = configpath + "DataTables/";

	try
	{
		if (!DungeonDataEntryMgr::Instance()->LoadData(configpath + "DungeonTable.txt")) return false;
	}
	catch (std::string str)
	{
		ErrorLogStream << str << LogStream::eos;
		return false;
	}

	return true;
}

void VSApplication::LoadDynamicConfig()
{
	m_MaxVerifyTask = GetProperties()->GetProperty("Verify", "max_num");
	m_VerifyLogLevel = GetProperties()->GetProperty("Verify", "log_level");

	{
		std::string whiteListStr = GetProperties()->GetProperty("Verify", "white_list").GetString();
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(whiteListStr, strs, "|");
		m_DungeonWhiteList.clear();
		for (auto str : strs)
		{
			m_DungeonWhiteList.insert(Avalon::StringUtil::ConvertToValue<UInt32>(str));
		}
	}
	
	m_IsReportCheatWhenRaceEndUnsync = GetProperties()->GetProperty("Verify", "ReportCheatWhenRaceEndUnsync");

	m_MaxFramePerUpdate = GetProperties()->GetProperty("Verify", "MaxFramePerUpdate");
	if (m_MaxFramePerUpdate == 0)
	{
		m_MaxFramePerUpdate = 350;
	}

	InfoLogStream << "max verify task(" << m_MaxVerifyTask << ") MaxFramePerUpdate(" << m_MaxFramePerUpdate << ")" << LogStream::eos;
}

void VSApplication::OnQuit()
{
	GameLoggerMgr::Instance()->Final();
	VerifyMgr::Instance()->Final();
	CLApplication::OnQuit();
}

void VSApplication::OnSignal(int signal)
{
	InfoLogStream << "recv signal " << signal << LogStream::eos;
	switch (signal)
	{
	case SIG_RELOAD_CONFIG:
	{
		m_ReloadConfig = true;
		break;
	}
	case 12:
	{
		m_DumpMemory = true;
		break;
	}
	default:
	{
		ServerApplication::OnSignal(signal);
		break;
	}
	}
}