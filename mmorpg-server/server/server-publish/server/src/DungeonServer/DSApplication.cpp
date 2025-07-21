#include "DSApplication.h"

#include <AvalonLogger.h>
#include <CLConfigLoadMgr.h>
#include <CLDungeonDestructTable.h>
#include <CLActivityMonsterDataEntry.h>
#include <CLItemDataEntry.h>
#include <CLDungeonAreaDataEntry.h>
#include <CLDropItemDataEntry.h>

#include "DSNetwork.h"
#include "DungeonRaceMgr.h"
#include "PerformanceCheck.h"
#include "RelaySessionMgr.h"
#include "DungeonMonsterGenerator.h"
#include "DungeonHandler.h"
#include "DropItemMgr.h"

DSApplication::DSApplication()
{
	m_TickTime = 0;
	m_CheckMd5 = true;
	m_IsRecordDungeonLog = true;
	m_IsRecordDungeonReplay = true;
	m_ReloadConfig = false;
	m_CheckPickDropFrame = false;
}

DSApplication::~DSApplication()
{
	m_CheckRepeatStartDungeon = true;
}

bool DSApplication::OnInit()
{
	if(!CLApplication::OnInit()) return false;

	LoadDynamicConfig();

	if (!LoadDataTables())
	{
		ErrorLogStream << "load tables failed!" << LogStream::eos;
		return false;
	}

	if(!DSNetwork::Instance()->Startup(GetProperties(), GetNetAddress()))
	{
		ErrorLogStream << "internal network startup failed!" << LogStream::eos;
		return false;
	}

	if (!DropItemMgr::Instance()->Init())
	{
		ErrorLogStream << "drop item manager init failed!" << LogStream::eos;
		return false;
	}

	RelaySessionMgr::Instance()->Init();

	if (!DungeonMonsterGenerator::Instance()->Init())
	{
		ErrorLogStream << "dungeon moster generator init failed!" << LogStream::eos;
		return false;
	}

	DSPlayer::InitPlayerProtocols();
	m_TickTime = Avalon::Time::CurrentTime().MSec();
	return true;
}

void DSApplication::OnTick(const Avalon::Time &now)
{
	UInt64 offsetTime = now.MSec() - m_TickTime;
	if (offsetTime > 1000)
	{
		ErrorLogStream << "too long tick time:" << offsetTime << " msecs!" << LogStream::eos;
	}
	m_TickTime = now.MSec();

	RESET_PERF();

	PERF_SEG_BEGIN(Application);
	CLApplication::OnTick(now);
	PERF_SEG_END();

	PERF_SEG_BEGIN(Network);
	DSNetwork::Instance()->OnTick(now);
	PERF_SEG_END();

	PERF_SEG_BEGIN(DungeonRaceMgr);
	DungeonRaceMgr::Instance()->OnTick(now);
	PERF_SEG_END();

	//bool test = false;
	//if (test)
	//{
	//	// Test
	//	Protocol::TeamCopyNotifyRaceDestroyDungeon pro;
	//	HashMap<UInt64, DungeonRace*>& races = DungeonRaceMgr::Instance()->GetRaces();
	//	for (auto iter : races)
	//	{
	//		DungeonRace* race = iter.second;
	//		if (!race)
	//		{
	//			continue;
	//		}
	//		pro.destroyDungeonId = 1;
	//		pro.notifySessionId = race->GetSessionId();
	//		HandleProtocol(pro);
	//	}
	//}

	//bool test1 = false;
	//if (test1)
	//{
	//	// Test
	//	Protocol::DungeonSvrEndRace pro;
	//	HashMap<UInt64, DungeonRace*>& races = DungeonRaceMgr::Instance()->GetRaces();
	//	for (auto iter : races)
	//	{
	//		DungeonRace* race = iter.second;
	//		if (!race)
	//		{
	//			continue;
	//		}
	//		pro.resaon = 1;
	//		pro.raceSessionId = race->GetSessionId();
	//		HandleProtocol(pro);
	//	}
	//}

	if (m_ReloadConfig)
	{
		m_ReloadConfig = false;

		if (!GetProperties()->LoadFromFile("DungeonServer.cfg"))
		{
			ErrorLogStream << "load DungeonServer.cfg failed!" << LogStream::eos;
			return;
		}

		LoadDynamicConfig();
	}
}

void DSApplication::OnQuit()
{
	DSNetwork::Instance()->Shutdown(200);

	CLApplication::OnQuit();
}

void DSApplication::OnSignal(int signal)
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

void DSApplication::LoadDynamicConfig()
{
	DebugLogStream << "load dungeonserver dynamic config." << LogStream::eos;
	m_CheckRepeatStartDungeon = GetProperties()->GetProperty("Game", "CheckRepeatDungeon");
	m_CheckMd5 = !GetProperties()->GetProperty("Game", "CloseMd5");

	m_IsRecordDungeonLog = GetProperties()->GetProperty("Verify", "RecordDungeonLog");
	m_IsRecordDungeonReplay = GetProperties()->GetProperty("Verify", "RecordDungeonReplay");
	m_CheckPickDropFrame = GetProperties()->GetProperty("Verify", "CheckPickDropFrame");
}

bool DSApplication::LoadDataTables()
{
	std::string configpath = CONFIG_PATH;
	configpath = configpath + "DataTables/";

	try
	{
		DATA_TABLE_LOAD("UnitTable.txt", MonsterDataEntryMgr::Instance());
		DATA_TABLE_LOAD("DestrucTable.txt", DungeonDestructDataEntryMgr::Instance());
		DATA_TABLE_LOAD("MonsterAttributeTable.txt", MonsterAttributeDataEntryMgr::Instance());
		DATA_TABLE_LOAD("ActivityMonsterTable.txt", ActivityMonsterDataEntryMgr::Instance());
		DATA_TABLE_LOAD("ItemTable.txt", ItemDataEntryMgr::Instance());
		DATA_TABLE_LOAD("DungeonTable.txt", DungeonDataEntryMgr::Instance());
		DATA_TABLE_LOAD("DungeonAreaTable.txt", DungeonAreaDataEntryMgr::Instance());
		DATA_TABLE_LOAD("MonsterPrefixTable.txt", MonsterPrefixDataEntryMgr::Instance());
		DATA_TABLE_LOAD("MonsterGroupTable.txt", MonsterGroupDataEntryMgr::Instance());
		DATA_TABLE_LOAD("DungeonPrefixLimitTable.txt", DungeonPrefixLimitDataMgr::Instance());
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
