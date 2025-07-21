#include "MSApplication.h"

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

#include "MSInternalNetwork.h"
#include "RelaySessionMgr.h"
#include "MatchMgr.h"
#include "MatchServiceMgr.h"
#include "RobotMgr.h"

MSApplication::MSApplication()
{
	m_ReloadConfig = false;
}

MSApplication::~MSApplication()
{
}

bool MSApplication::OnInit()
{
	if (!CLApplication::OnInit()) return false;

	std::string configpath = CONFIG_PATH;
	std::string rootpath = ROOT_PATH;
	if (!GetProperties()->LoadFromFile(rootpath + "Game.cfg"))
	{
		ErrorLogStream << "load game config failed!" << LogStream::eos;
		return false;
	}

	LoadDynamicConfig();

	if (!GameLoggerMgr::Instance()->Init(GAME_LOGGER_CFG, IsDebugMode()))
	{
		ErrorLogStream << "game loggermgr init failed!" << LogStream::eos;
		return false;
	}

	if (!MSInternalNetwork::Instance()->Startup(GetProperties(), GetNetAddress()))
	{
		ErrorLogStream << "internal network startup failed!" << LogStream::eos;
		return false;
	}

    if (!LoadDataTables())
    {
        ErrorLogStream << "load tables failed!" << LogStream::eos;
        return false;
    }

    if (!MatchMgr::Instance()->Init())
    {
        ErrorLogStream << "MatchMgr init failed!" << LogStream::eos;
        return false;
    }

    if (!MatchServiceMgr::Instance()->Init())
    {
        ErrorLogStream << "MatchServiceMgr init failed!" << LogStream::eos;
        return false;
    }

	if (!RobotMgr::Instance()->Init())
	{
		ErrorLogStream << "RobotMgr init failed!" << LogStream::eos;
		return false;
	}

	RelaySessionMgr::Instance()->Init();

	InfoLogStream << GetName() << " init success!" << LogStream::eos;

	return true;
}

void MSApplication::OnTick(const Avalon::Time &now)
{
	CLApplication::OnTick(now);

    MSInternalNetwork::Instance()->OnTick(now);
	RelaySessionMgr::Instance()->OnTick(now);
    MatchMgr::Instance()->OnTick(now);
    MatchServiceMgr::Instance()->OnTick(now);

	if (m_ReloadConfig)
	{
		if (!GetProperties()->LoadFromFile("MatchServer.cfg"))
		{
			ErrorLogStream << "load MatchServer.cfg failed!" << LogStream::eos;
			return;
		}

		LoadDynamicConfig();
		m_ReloadConfig = false;
	}
}

bool MSApplication::LoadDataTables()
{
    std::string configpath = CONFIG_PATH;
    configpath = configpath + "DataTables/";

    try
    {
		if (!DungeonDataEntryMgr::Instance()->LoadData(configpath + "DungeonTable.txt")) return false;
        if (!PkLevelDataEntryMgr::Instance()->LoadData(configpath + "PkLevelTable.txt")) return false;
		if (!DungeonMatchDataEntryMgr::Instance()->LoadData(configpath + "DungeonMatchTable.txt")) return false;
		if (!RobotDataEntryMgr::Instance()->LoadData(configpath + "RobotTable.txt")) return false;
		if (!ItemDataEntryMgr::Instance()->LoadData(configpath + "ItemTable.txt")) return false;
		if (!RandBaseDataEntryMgr::Instance()->LoadData(configpath + "RandBaseTable.txt")) return false;
		if (!EquipAttrDataMgr::Instance()->LoadData(configpath + "EquipAttrTable.txt")) return false;
		if (!SeasonLevelDataEntryMgr::Instance()->LoadData(configpath + "SeasonLevelTable.txt")) return false;
		if (!SystemValueDataEntryMgr::Instance()->LoadData(configpath + "SystemValueTable.txt")) return false;
    }
    catch (std::string str)
    {
        ErrorLogStream << str << LogStream::eos;
        return false;
    }

    return true;
}

void MSApplication::LoadDynamicConfig()
{
	InfoLogStream << "load dynamic config." << LogStream::eos;

	m_BattleFlag = GetProperties()->GetProperty("Game", "BattleFlag");
}

void MSApplication::OnQuit()
{
	GameLoggerMgr::Instance()->Final();
	RelaySessionMgr::Instance()->Final();
    MatchMgr::Instance()->Final();

	CLApplication::OnQuit();
}

void MSApplication::OnSignal(int signal)
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
