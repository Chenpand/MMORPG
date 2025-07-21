#include "OBSApplication.h"

#include <AvalonMemoryTracker.h>
#include <AvalonLogger.h>
#include <AvalonMd5.h>
#include <CLGameLoggerMgr.h>
#include <CLPkLevelDataEntry.h>
#include <CLDungeonMatchDataEntry.h>
#include <CLRobotDataEntry.h>
#include <CLItemDataEntry.h>
#include <CLDungeonDataEntry.h>
#include <CLSeasonDataEntry.h>
#include <CLSystemValueDataEntry.h>

#include "OBSInternalNetwork.h"
#include "RelaySessionMgr.h"
#include "ObserveRelaySessionMgr.h"
#include "OBSRaceMgr.h"

OBSApplication::OBSApplication()
{
}

OBSApplication::~OBSApplication()
{
}

bool OBSApplication::OnInit()
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

	if (!OBSInternalNetwork::Instance()->Startup(GetProperties(), GetNetAddress()))
	{
		ErrorLogStream << "internal network startup failed!" << LogStream::eos;
		return false;
	}

    if (!LoadDataTables())
    {
        ErrorLogStream << "load tables failed!" << LogStream::eos;
        return false;
    }

	RelaySessionMgr::Instance()->Init();
	ObserveRelaySessionMgr::Instance()->Init();
	OBSRaceMgr::Instance()->Init();

	InfoLogStream << GetName() << " init success!" << LogStream::eos;

	return true;
}

void OBSApplication::OnTick(const Avalon::Time &now)
{
	CLApplication::OnTick(now);

    OBSInternalNetwork::Instance()->OnTick(now);
	RelaySessionMgr::Instance()->OnTick(now);
	ObserveRelaySessionMgr::Instance()->OnTick(now);
	OBSRaceMgr::Instance()->OnTick(now);
}

bool OBSApplication::LoadDataTables()
{
    std::string configpath = CONFIG_PATH;
    configpath = configpath + "DataTables/";

    try
    {
		//if (!DungeonDataEntryMgr::Instance()->LoadData(configpath + "DungeonTable.txt")) return false;
		//if (!PkLevelDataEntryMgr::Instance()->LoadData(configpath + "PkLevelTable.txt")) return false;
		//if (!DungeonMatchDataEntryMgr::Instance()->LoadData(configpath + "DungeonMatchTable.txt")) return false;
		//if (!RobotDataEntryMgr::Instance()->LoadData(configpath + "RobotTable.txt")) return false;
		//if (!ItemDataEntryMgr::Instance()->LoadData(configpath + "ItemTable.txt")) return false;
		//if (!RandBaseDataEntryMgr::Instance()->LoadData(configpath + "RandBaseTable.txt")) return false;
		//if (!EquipAttrDataMgr::Instance()->LoadData(configpath + "EquipAttrTable.txt")) return false;
		//if (!SeasonLevelDataEntryMgr::Instance()->LoadData(configpath + "SeasonLevelTable.txt")) return false;
		if (!SystemValueDataEntryMgr::Instance()->LoadData(configpath + "SystemValueTable.txt")) return false;
    }
    catch (std::string str)
    {
        ErrorLogStream << str << LogStream::eos;
        return false;
    }

    return true;
}

void OBSApplication::OnQuit()
{
	GameLoggerMgr::Instance()->Final();
	RelaySessionMgr::Instance()->Final();
	ObserveRelaySessionMgr::Instance()->Final();
	OBSRaceMgr::Instance()->Final();

	CLApplication::OnQuit();
}

