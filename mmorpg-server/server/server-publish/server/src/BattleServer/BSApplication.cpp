#include "BSApplication.h"

#include <AvalonLogger.h>
#include <CLForbitWord.h>
#include <CLConfigLoadMgr.h>
#include <CLSystemValueDataEntry.h>
#include <CLSceneDataEntry.h>
#include <CLRobotDataEntry.h>
#include <CLItemDataEntry.h>
#include <CLChiJiTimeDataEntry.h>
#include <CLChiJiScoreDataEntry.h>
#include <CLChiJiSwitchDataEntry.h>
#include <CLChiJiShopTimeDataEntry.h>
#include <CLHonorRewardDataEntry.h>
#include <CLDungeonDataEntry.h>
#include <CLLostDungeonDataEntry.h>
#include <CLLostDungeonBattleDataEntry.h>

#include "BSNetwork.h"
#include "GameZoneMgr.h"
#include "CLSysNotify.h"
#include "BSTeamMgr.h"
#include "BSBattleMgr.h"
#include "RobotMgr.h"
#include "BSSortListMgr.h"
#include "BSBattleSwitchMgr.h"
#include "RelaySessionMgr.h"
#include "BSLostBattleMgr.h"
#include "BSRaceMgr.h"

BSApplication::BSApplication()
{
	m_ReloadConfig = false;
	m_TickTime = 0;
	m_DBConn = NULL;
	m_hasRobot = 0;
	m_NoWarCD = 0;
	m_BattleFlag = 0;
}

BSApplication::~BSApplication()
{
}

bool BSApplication::OnInit()
{
	if(!CLApplication::OnInit()) return false;

	
	std::string db_ip = GetProperties()->GetProperty("Database", "db_ip").GetString();
	UInt32 db_port = GetProperties()->GetProperty("Database", "db_port");
	std::string db_user = GetProperties()->GetProperty("Database", "db_user").GetString();
	std::string db_passwd = GetProperties()->GetProperty("Database", "db_passwd").GetString();
	std::string db_name = GetProperties()->GetProperty("Database", "db_name").GetString();
	m_DBConn = Avalon::RetryDBConnection::Create("mysql", 20, 500);
	if(!m_DBConn->Open(db_ip.c_str(), db_port, db_user.c_str(), db_passwd.c_str(), db_name.c_str()))
	{
		ErrorLogStream << "connect to database " << db_ip << ":" << db_port << " " << db_name << "!" << LogStream::eos;
		Avalon::RetryDBConnection::Destroy(m_DBConn);
		return false;
	}

	LoadDynamicConfig();

	if (!LoadDataTables())
	{
		ErrorLogStream << "load tables failed!" << LogStream::eos;
		return false;
	}

	if(!BSNetwork::Instance()->Startup(GetProperties(), GetNetAddress()))
	{
		ErrorLogStream << "internal network startup failed!" << LogStream::eos;
		return false;
	}

	if(!GameZoneMgr::Instance()->Init())
	{
		ErrorLogStream << "game zone manager init failed!" << LogStream::eos;
		return false;
	}

	if (!RobotMgr::Instance()->Init())
	{
		ErrorLogStream << "robot manager init failed!" << LogStream::eos;
		return false;
	}

	if (!BSSortListMgr::Instance()->Init())
	{
		ErrorLogStream << "BSSortListMgr init failed!" << LogStream::eos;
		return false;
	}

	if (!BSBattleSwitchMgr::Instance()->Init())
	{
		ErrorLogStream << "BSBattleSwitchMgr init failed!" << LogStream::eos;
		return false;
	}

	RelaySessionMgr::Instance()->Init();

	m_TickTime = Avalon::Time::CurrentTime().MSec();
	return true;
}

void BSApplication::OnTick(const Avalon::Time &now)
{
	CLApplication::OnTick(now);

	UInt64 offsetTime = now.MSec() - m_TickTime;
	if(offsetTime > 1000)
	{
		ErrorLogStream << "too long tick time:" << offsetTime << " msecs!" << LogStream::eos;
	}
	m_TickTime = now.MSec();

	BSNetwork::Instance()->OnTick(now);
	GameZoneMgr::Instance()->OnTick(now);
	BSTeamMgr::Instance()->OnTick(now);
	BSBattleMgr::Instance()->OnTick(now);
	BSBattleSwitchMgr::Instance()->OnTick(now);
	RelaySessionMgr::Instance()->OnTick(now);
	BSLostBattleMgr::Instance()->OnTick(now);
	BSRaceMgr::Instance()->OnTick(now);

	if (m_ReloadConfig)
	{
		m_ReloadConfig = false;

		if (!GetProperties()->LoadFromFile("BattleServer.cfg"))
		{
			ErrorLogStream << "load BattleServer.cfg failed!" << LogStream::eos;
			return;
		}

		LoadDynamicConfig();

		ReloadDataTables();
	}
}

void BSApplication::OnQuit()
{
	GameZoneMgr::Instance()->Final();
	RelaySessionMgr::Instance()->Final();

	BSNetwork::Instance()->Shutdown(200);

	Avalon::RetryDBConnection::Destroy(m_DBConn);
	m_DBConn = NULL;

	CLApplication::OnQuit();
}

void BSApplication::OnSignal(int signal)
{
	InfoLogStream << "recv signal " << signal << LogStream::eos;
	switch (signal)
	{
	case SIG_RELOAD_CONFIG:
		m_ReloadConfig = true;
		break;

	default:
		ServerApplication::OnSignal(signal);
		break;
	}
}

void BSApplication::LoadDynamicConfig()
{
	UInt32 teamCount = GetProperties()->GetProperty("Battle", "team_count_pre_battle");
	UInt32 battleTimeOut = GetProperties()->GetProperty("Battle", "battle_time_out");

	BSBattleMgr::Instance()->Init(teamCount, battleTimeOut);

	m_hasRobot = GetProperties()->GetProperty("Battle", "has_robot");
	m_NoWarCD = GetProperties()->GetProperty("Battle", "no_war_cd");
	if (m_NoWarCD == 0)
	{
		m_NoWarCD = 30;
	}

	m_BattleFlag = GetProperties()->GetProperty("Game", "BattleFlag");
}

bool BSApplication::LoadDataTables()
{
	std::string configpath = CONFIG_PATH;
	configpath = configpath + "DataTables/";

	if (!ForbitWordParser::Instance()->Init(configpath + "forbitword.txt")) return false;
	if (!ForbitWordParser::Instance()->InitValidCoolName(configpath + "coolname.txt")) return false;

	try
	{
		DATA_TABLE_LOAD("CitySceneTable.txt", SceneDataEntryMgr::Instance());
		DATA_TABLE_LOAD("SystemValueTable.txt", SystemValueDataEntryMgr::Instance());
		if (!SysNotifyDataEntryMgr::Instance()->LoadData(configpath + "CommonTipsDesc.txt")) return false;
		SysNotifyMgr::Instance()->Load(NULL, std::string());
		if (!RobotDataEntryMgr::Instance()->LoadData(configpath + "RobotTable.txt")) return false;
		if (!ItemDataEntryMgr::Instance()->LoadData(configpath + "ItemTable.txt")) return false;
		if (!ChiJiTimeDataEntryMgr::Instance()->LoadData(configpath + "ChiJiTimeTable.txt")) return false;
		if (!ChiJiScoreDataEntryMgr::Instance()->LoadData(configpath + "ChiJiScoreTable.txt")) return false;
		if (!ChiJiSwitchDataEntryMgr::Instance()->LoadData(configpath + "ChiJiSwitchTable.txt")) return false;
		if (!ChiJiShopTimeDataEntryMgr::Instance()->LoadData(configpath + "ChiJiShopTimeTable.txt")) return false;
		if (!HonorRewardDataEntryMgr::Instance()->LoadData(configpath + "HonorRewardTable.txt")) return false;
		if (!DungeonDataEntryMgr::Instance()->LoadData(configpath + "DungeonTable.txt")) return false;
		DATA_TABLE_LOAD("LostDungeonFloorTable.txt", LostDungeonFloorDataEntryMgr::Instance());
		DATA_TABLE_LOAD("LostDungeonBattleTable.txt", LostDungeonBattleDataEntryMgr::Instance());	
	}
	catch (std::string str)
	{
		ErrorLogStream << str << LogStream::eos;
		return false;
	}
	
	return true;
}

bool BSApplication::ReloadDataTables()
{
	std::string configpath = CONFIG_PATH;
	configpath = configpath + "DataTables/";

	try
	{
		if (!ChiJiScoreDataEntryMgr::Instance()->ReloadData(configpath + "ChiJiScoreTable.txt")) return false;
	}
	catch (std::string str)
	{
		ErrorLogStream << str << LogStream::eos;
		return true;
	}

	return true;
}