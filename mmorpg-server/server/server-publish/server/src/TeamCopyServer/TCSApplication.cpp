#include "TCSApplication.h"

#include <DropItemMgr.h>
#include <CLItemDataEntry.h>
#include <CLSceneDataEntry.h>
#include <CLTeamCopyTargetDataEntry.h>
#include <CLTeamCopyFieldDataEntry.h>
#include <CLTeamCopyFlopDataEntry.h>
#include <CLTeamCopyValueDataEntry.h>
#include <CLTeamCopyBossDataEntry.h>
#include <CLTeamCopyMapDataEntry.h>
#include <CLTeamCopyGridParamDataEntry.h>
#include <CLTeamCopyGridMonsterDataEntry.h>
#include <CLTeamCopyRollDataEntry.h>
#include <CLSysNotifyDataEntry.h>
#include <CLSysNotify.h>
#include <CLRecordClient.h>
#include <kingnet_analyzer.h>
#include <PerformanceCheck.h>

#include "CLConfigLoadMgr.h"
#include "TCSNetwork.h"
#include "GameZoneMgr.h"
#include "TCSPlayerMgr.h"
#include "TCSTeamMgr.h"
#include "TCSGridCfgMgr.h"

TCSApplication::TCSApplication()
{
	m_TickTime = 0;
	m_IsMemberEnough = false;
	m_IsOpenGoldTeam = false;
	m_ReloadConfig = false;
	m_ReloadTable = false;
	m_LogTransProtocol = false;
	m_StopServerIsReturnNum = false;
	m_TC2FailedReturnCommission = false;
}

TCSApplication::~TCSApplication()
{

}

bool TCSApplication::OnInit()
{
	if(!CLApplication::OnInit()) return false;
	
	LoadDynamicConfig();

	if (!LoadDataTables())
	{
		ErrorLogStream << "load tables failed!" << LogStream::eos;
		return false;
	}

	if (!TCSNetwork::Instance()->Startup(GetProperties(), GetNetAddress()))
	{
		ErrorLogStream << "internal network startup failed!" << LogStream::eos;
		return false;
	}

	if (!CLRecordClient::Instance()->Init(this, TCSNetwork::Instance()->FindConnectionByType(ST_GLOBAL_RECORD)))
	{
		ErrorLogStream << "record client init failed!" << LogStream::eos;
		return false;
	}

	if (UL_FAIL == _analyzer.Init() || NULL == _analyzer.GetInstance(UDPLOG_TEAM_COPY_UID))
	{
		ErrorLogStream << "team copy activity udplog init failed!" << LogStream::eos;
		return false;
	}

	if (!GameZoneMgr::Instance()->Init())
	{
		ErrorLogStream << "game zone manager init failed!" << LogStream::eos;
		return false;
	}

	if (!DropItemMgr::Instance()->Init())
	{
		ErrorLogStream << "drop item manager init failed!" << LogStream::eos;
		return false;
	}

	TCSPlayerMgr::Instance()->Init();

	m_TickTime = Avalon::Time::CurrentTime().MSec();

	return true;
}

void TCSApplication::OnTick(const Avalon::Time &now)
{
	PERF_SEG_BEGIN(CLApplication);
	CLApplication::OnTick(now);
	PERF_SEG_END();

	/*UInt64 offsetTime = now.MSec() - m_TickTime;
	if (offsetTime > 1000)
	{
		ErrorLogStream << "too long tick time:" << offsetTime << " msecs!" << LogStream::eos;
	}*/

	m_TickTime = now.MSec();

	PERF_SEG_BEGIN(CLRecordClient);
	CLRecordClient::Instance()->ProcessCallbacks();
	PERF_SEG_END();

	PERF_SEG_BEGIN(TCSNetwork);
	TCSNetwork::Instance()->OnTick(now);
	PERF_SEG_END();

	PERF_SEG_BEGIN(GameZoneMgr);
	GameZoneMgr::Instance()->OnTick(now);
	PERF_SEG_END();

	PERF_SEG_BEGIN(TCSPlayerMgr);
	TCSPlayerMgr::Instance()->OnTick(now);
	PERF_SEG_END();

	PERF_SEG_BEGIN(TCSTeamMgr);
	TCSTeamMgr::Instance()->OnTick(now);
	PERF_SEG_END();

	if (m_ReloadConfig)
	{
		m_ReloadConfig = false;

		if (!GetProperties()->LoadFromFile("TeamCopyServer.cfg"))
		{
			ErrorLogStream << "load TeamCopyServer.cfg failed!" << LogStream::eos;
			return;
		}

		LoadDynamicConfig();
	}

	if (m_ReloadTable)
	{
		m_ReloadTable = false;
		ReloadDataTables();
	}
}

void TCSApplication::OnQuit()
{
	_analyzer.DestoryInstance(UDPLOG_TEAM_COPY_UID);
	TCSTeamMgr::Instance()->Final();
	GameZoneMgr::Instance()->Final();
	TCSNetwork::Instance()->Shutdown(200);

	CLApplication::OnQuit();
}

void TCSApplication::LoadDynamicConfig()
{
	// 性能统计
	CLNetworkPerfMgr::Instance()->Init(GetProperties());

	m_IsMemberEnough = GetProperties()->GetProperty("Config", "memberIsEnough");
	m_IsOpenGoldTeam = GetProperties()->GetProperty("Config", "isOpenGoldTeam");
	m_LogTransProtocol = GetProperties()->GetProperty("Config", "log_trans_protocol");
	m_StopServerIsReturnNum = GetProperties()->GetProperty("Config", "isReturnNumStopServer");
	m_TC2FailedReturnCommission = GetProperties()->GetProperty("Config", "tc2FailedReturnCommission");

	{
		std::string closeType = GetProperties()->GetProperty("Config", "closeTeamType").GetString();
		std::vector<std::string> params;
		Avalon::StringUtil::Split(closeType, params, "|");

		m_CloseTeamType.clear();
		for (auto param : params)
		{
			UInt32 type = Avalon::StringUtil::ConvertToValue<UInt32>(param);
			m_CloseTeamType.insert(type);
		}
	}
}

void TCSApplication::OnSignal(int signal)
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
		m_ReloadTable = true;
		break;
	}
	default:
	{
		ServerApplication::OnSignal(signal);
		break;
	}
	}
}

bool TCSApplication::LoadDataTables()
{
	std::string configpath = CONFIG_PATH;
	configpath = configpath + "DataTables/";

	try
	{
		DATA_TABLE_LOAD("CitySceneTable.txt", SceneDataEntryMgr::Instance());
		DATA_TABLE_LOAD("ItemTable.txt", ItemDataEntryMgr::Instance());
		DATA_TABLE_LOAD("CommonTipsDesc.txt", SysNotifyDataEntryMgr::Instance());
		SysNotifyMgr::Instance()->Load(NULL, std::string());

		DATA_TABLE_LOAD("TeamCopyFieldTable.txt", TeamCopyFieldDataEntryMgr::Instance());
		DATA_TABLE_LOAD("TeamCopyTargetTable.txt", TeamCopyTargetDataEntryMgr::Instance());
		DATA_TABLE_LOAD("TeamCopyFlopTable.txt", TeamCopyFlopDataEntryMgr::Instance());
		DATA_TABLE_LOAD("TeamCopyValueTable.txt", TeamCopyValueDataEntryMgr::Instance());
		DATA_TABLE_LOAD("TeamCopyBossTable.txt", TeamCopyBossDataEntryMgr::Instance());
		DATA_TABLE_LOAD("TeamTileResTable.txt", TeamCopyGridDataEntryMgr::Instance());
		DATA_TABLE_LOAD("TeamTileMapTable.txt", TeamCopyMapDataEntryMgr::Instance());
		DATA_TABLE_LOAD("TeamTileMapTableMap_2.txt", TeamCopyMapDataEntryMgr2::Instance());
		DATA_TABLE_LOAD("TeamCopyGridParamTable.txt", TeamCopyGridParamDataEntryMgr::Instance());
		DATA_TABLE_LOAD("TeamCopyGridMonsterTable.txt", TeamCopyGridMonsterDataEntryMgr::Instance());
		DATA_TABLE_LOAD("TeamCopyRollTable.txt", TeamCopyRollDataEntryMgr::Instance());
		TCSGridCfgMgr::Instance()->InitCfg();

		DATA_TABLE_LOAD("DropItemTable.txt", DropItemDataEntryMgr::Instance());
		DATA_TABLE_LOAD("DropItemTable2.txt", DropItemDataEntryMgr::Instance());
	}
	catch (std::string str)
	{
		ErrorLogStream << str << LogStream::eos;
		return false;
	}
	
	return true;
}

bool TCSApplication::ReloadDataTables()
{
	std::string configpath = CONFIG_PATH;
	configpath = configpath + "DataTables/";

	try
	{
		TeamCopyValueDataEntryMgr::Instance()->ReloadData(configpath + "TeamCopyValueTable.txt");
		TeamCopyRollDataEntryMgr::Instance()->ReloadData(configpath + "TeamCopyRollTable.txt");
	}
	catch (std::string str)
	{
		ErrorLogStream << str << LogStream::eos;
		return true;
	}

	return true;
}