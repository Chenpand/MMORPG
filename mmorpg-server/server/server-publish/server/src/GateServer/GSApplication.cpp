#include "GSApplication.h"

#include <AvalonLogger.h>
#include <CLRecordClient.h>
#include <CLForbitWord.h>
#include <kingnet_analyzer.h>
#include <CLJobDataEntry.h>
#include <CLSystemValueDataEntry.h>
#include <CLSysNotifyDataEntry.h>
#include <CLActiveEventDataEntry.h>
#include <CLAntiAddictTimeDataEntry.h>
#include <CLAntiAddictHolidayDataEntry.h>

#include <CLSysNotify.h>
#include <AvalonMemoryTracker.h>
#include <CLGateProtocol.h>
#include <CLNetworkPerformance.h>

#include "GSInternalNetwork.h"
#include "GSExternalNetwork.h"
#include "GSAccountMgr.h"
#include "PerformanceCheck.h"
#include "GSRouter.h"
#include "GSPlayerMgr.h"
#include "GSSceneMgr.h"
#include "GSPlayerMonitorMgr.h"
#include "GSActivityMgr.h"
#include "GSUnionProtocolMgr.h"

GSApplication::GSApplication()
{
	m_ReloadConfig = false;
	m_MaxConnNum = 0;
	m_MaxLoadingPlayer = 0;
	m_MaxPlayerNum = 0;
	m_serverStartTime = 0;
	m_LogTransProtocol = false;
	m_LogTeamCopyProtocol = false;
	m_ReloadTable = false;
	m_ClearPacketWhenKick = true;
	m_CheckAntiAddict = false;
	m_TourLoginInterval = 360;
	m_TourOnlineTime = 1;
	m_TickTimerMin.SetInterval(60 * 1000);
	m_OpenPerf = false;
}

GSApplication::~GSApplication()
{
}

bool GSApplication::OnInit()
{
	if(!CLApplication::OnInit()) return false;

	std::string configpath = CONFIG_PATH;
	std::string rootpath = ROOT_PATH;
	if(!GetProperties()->LoadFromFile(rootpath + "Game.cfg"))
	{
		ErrorLogStream << "load game config failed!" << LogStream::eos;
		return false;
	}
	m_CryptKey1 = GetProperties()->GetProperty("Crypt", "crypt_key1").GetString();
	m_CryptKey2 = GetProperties()->GetProperty("Crypt", "crypt_key2").GetString();
	GAME_VERSION = GetProperties()->GetProperty("Server", "version").GetString();
	
	LoadDynamicConfig();

	if(!LoadDataTables())
	{
		ErrorLogStream << "load tables failed!" << LogStream::eos;
		return false;
	}

	if(UL_FAIL == _analyzer.Init() || NULL == _analyzer.GetInstance(UDPLOG_GATE_UID))
	{
		ErrorLogStream << "udplog init failed!" << LogStream::eos;
		return false;
	}

	if(!GSInternalNetwork::Instance()->Startup(GetProperties(),GetNetAddress()))
	{
		ErrorLogStream << "internalnet work startup failed!" << LogStream::eos;
		return false;
	}

	if(!GSExternalNetwork::Instance()->Startup(GetProperties(),GetNetAddress()))
	{
		ErrorLogStream << "external network startup failed!" << LogStream::eos;
		return false;
	}

	std::vector<Avalon::NetConnectionPtr> connVec;
	GSInternalNetwork::Instance()->FindConnectionByType(ST_RECORD, connVec);

	if (connVec.empty() || !CLRecordClient::Instance()->Init(this, connVec))
	{
		ErrorLogStream << "record client init failed!" << LogStream::eos;
		return false;
	}

	if(!GSPlayerMgr::Instance()->Init(GetProperties()))
	{
		ErrorLogStream << "playermgr init failed!" << LogStream::eos;
		return false;
	}

	GSSceneMgr::Instance()->Init();
	GSAccountMgr::Instance()->Init();
	GSUnionProtocolMgr::Instance()->Init();

	GSPlayerMonitorMgr::Instance()->LoadFromString(GetProperties()->GetProperty("Game", "monitor_list").GetString());

	m_TickTime = Avalon::Time::CurrentTime().MSec();

	return true;
}

void GSApplication::OnTick(const Avalon::Time &now)
{
	m_TickTime = now.MSec();

	PERF_SEG_BEGIN(CLApplication);
	CLApplication::OnTick(now);
	PERF_SEG_END();

	PERF_SEG_BEGIN(GSInternalNetwork);
	GSInternalNetwork::Instance()->OnTick(now);
	PERF_SEG_END();

	PERF_SEG_BEGIN(GSExternalNetwork);
	GSExternalNetwork::Instance()->OnTick(now);
	PERF_SEG_END();

	PERF_SEG_BEGIN(GSAccountMgr);
	GSAccountMgr::Instance()->OnTick(now);
	PERF_SEG_END();

	PERF_SEG_BEGIN(GSPlayerMgr);
	GSPlayerMgr::Instance()->OnTick(now);
	PERF_SEG_END();

	PERF_SEG_BEGIN(CLRecordClient);
	CLRecordClient::Instance()->ProcessCallbacks();
	PERF_SEG_END();

	PERF_SEG_BEGIN(GSActivityMgr);
	GSActivityMgr::Instance()->OnTick(now);
	PERF_SEG_END();

	if (m_TickTimerMin.IsTimeout(now))
	{
		REPORT_MEMORY_OBJECT(gSysLogger);
	}

	if (m_ReloadConfig)
	{
		m_ReloadConfig = false;

		if (!GetProperties()->LoadFromFile(std::string(ROOT_PATH) + "Game.cfg"))
		{
			ErrorLogStream << "load game config failed!" << LogStream::eos;
			return;
		}

		if (!GetProperties()->LoadFromFile("GateServer.cfg"))
		{
			ErrorLogStream << "load GateServer.cfg failed!" << LogStream::eos;
			return;
		}

		LoadDynamicConfig();
	}

	if (m_ReloadTable)
	{
		m_ReloadTable = false;

		ReloadDataTables();
	}

	UInt64 offsetTime = Avalon::Time::CurrentTime().MSec() - m_TickTime;
	if (offsetTime > 1000)
	{
		ErrorLogStream << "too long tick time:" << offsetTime << " msecs!" << LogStream::eos;
		SHOW_PERF();
	}
}

void GSApplication::OnSignal(int signal)
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
		m_ReloadTable = true;
		break;
	default:
	{
		ServerApplication::OnSignal(signal);
		break;
	}
	}
}

void GSApplication::LoadDynamicConfig()
{
	// 性能统计
	CLNetworkPerfMgr::Instance()->Init(GetProperties());

	m_MaxConnNum = GetProperties()->GetProperty("ExternalNetwork", "max_num_of_connections");
	m_MaxPlayerNum = GetProperties()->GetProperty("Player", "max_online_player_num");
	// 没有配置的话，就用默认值
	if (m_MaxPlayerNum == 0)
	{
		m_MaxPlayerNum = 3000;
	}

	m_MaxLoadingPlayer = GetProperties()->GetProperty("Player", "max_loading_player_num");
	// 没有配置的话，就用默认值
	if (m_MaxLoadingPlayer == 0)
	{
		m_MaxLoadingPlayer = 200;
	}

	GSPlayerMonitorMgr::Instance()->LoadFromString(GetProperties()->GetProperty("Game", "monitor_list").GetString());

	m_EncryptProtocol = !GetProperties()->GetProperty("Game", "close_encrypt_protocol");

	SERVER_NAME = GetProperties()->GetProperty("Server", "name").GetString();
	m_LogTransProtocol = GetProperties()->GetProperty("Game", "log_trans_protocol");
	m_ClearPacketWhenKick = GetProperties()->GetProperty("Game", "CloseClearPacketWhenKick");
	m_LogTeamCopyProtocol = GetProperties()->GetProperty("Game", "log_teamcopy_protocol");

	{
		m_FilterPacketBufferSet.clear();
		std::string packetStr = GetProperties()->GetProperty("Game", "filter_packet_buffer").GetString();
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(packetStr, strs, "|");
		for (auto str : strs)
		{
			UInt32 packetId = Avalon::StringUtil::ConvertToValue<UInt32>(str);
			m_FilterPacketBufferSet.insert(packetId);
		}
	}

	m_SaveConvertFile = !GetProperties()->GetProperty("Game", "CloseSaveConvertFile");
	m_ConvertScreenShot = !GetProperties()->GetProperty("Game", "CloseConvertScreenShot");
	m_CheckAntiAddict = GetProperties()->GetProperty("Game", "IsCheckAntiAddict");
	m_TourLoginInterval = GetProperties()->GetProperty("Game", "TourLoginInterval");
	m_TourOnlineTime = GetProperties()->GetProperty("Game", "TourOnlineTime");

	{
		bool isObjectRecord = GetProperties()->GetProperty("Game", "IsObjectRecord");
		InfoLogStream << "Memory Object Record Switch:" << isObjectRecord << LogStream::eos;
		MEMORY_OBJECT_SWITCH(isObjectRecord);
	}

	{
		m_PacketCountPlayerSet.clear();
		std::string playerStr = GetProperties()->GetProperty("Game", "packet_count_player").GetString();
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(playerStr, strs, "|");
		for (auto str : strs)
		{
			UInt64 playerId = Avalon::StringUtil::ConvertToValue<UInt64>(str);
			m_PacketCountPlayerSet.insert(playerId);
		}
	}

	m_OpenPerf = GetProperties()->GetProperty("Game", "OpenPerf");
	InfoLogStream << "m_OpenPerf:" << m_OpenPerf << LogStream::eos;
}

void GSApplication::OnQuit()
{
	GSAccountMgr::Instance()->Final();
	GSSceneMgr::Instance()->Final();

	GSPlayerMgr::Instance()->Final();

	CLRecordClient::Instance()->Final();

	GSExternalNetwork::Instance()->Shutdown(0);
	GSInternalNetwork::Instance()->Shutdown(100);
	_analyzer.DestoryInstance(UDPLOG_GATE_UID);

	ForbitWordParser::Instance()->Final();

	CLApplication::OnQuit();
}

bool GSApplication::IsFilterPacketBuffer(UInt32 packetId)
{
	if (m_FilterPacketBufferSet.empty())
	{
		return false;
	}

	auto iter = m_FilterPacketBufferSet.find(packetId);
	return iter != m_FilterPacketBufferSet.end();
}

bool GSApplication::IsCountPacket(UInt64 playerId)
{
	if (m_PacketCountPlayerSet.empty())
	{
		return false;
	}

	auto iter = m_PacketCountPlayerSet.find(playerId);
	return iter != m_PacketCountPlayerSet.end();
}

bool GSApplication::LoadDataTables()
{
	std::string configpath = CONFIG_PATH;
	configpath = configpath + "DataTables/";

    try
    {
        if (!ForbitWordParser::Instance()->Init(configpath + "forbitword.txt")) return false;
		if (!ForbitWordParser::Instance()->InitValidCoolName(configpath + "coolname.txt")) return false;
        if (!JobDataEntryMgr::Instance()->LoadData(configpath + "JobTable.txt")) return false;
		if (!SystemValueDataEntryMgr::Instance()->LoadData(configpath + "SystemValueTable.txt")) return false;
		if (!SysNotifyDataEntryMgr::Instance()->LoadData(configpath + "CommonTipsDesc.txt")) return false; 
		if (!SystemSwitchDataMgr::Instance()->LoadData(configpath + "IOSFuncSwitchTable.txt")) return false;
		
		SysNotifyMgr::Instance()->Load(NULL, std::string());
		if (!ActivitySystemConfigDataEntryMgr::Instance()->LoadData(configpath + "ActivitySystemConfig.txt")) return false;
		if (!AntiAddictHolidayDataEntryMgr::Instance()->LoadData(configpath + "AntiAddictHolidayTable.txt")) return false;
		if (!AntiAddictTimeDataEntryMgr::Instance()->LoadData(configpath + "AntiAddictTimeTable.txt")) return false;

    }
    catch (std::string str)
    {
        ErrorLogStream << str << LogStream::eos;
        return false;
    }

	return true;
}

void GSApplication::ReloadDataTables()
{
	std::string configpath = CONFIG_PATH;
	configpath = configpath + "DataTables/";

	InfoLogStream << "Start reload tables..." << LogStream::eos;

	try
	{
		SystemValueDataEntryMgr::Instance()->ReloadData(configpath + "SystemValueTable.txt");
		ActivitySystemConfigDataEntryMgr::Instance()->ReloadData(configpath + "ActivitySystemConfig.txt");
		SystemSwitchDataMgr::Instance()->LoadData(configpath + "IOSFuncSwitchTable.txt");
	}
	catch (std::string str)
	{
		ErrorLogStream << str << LogStream::eos;
	}
}
