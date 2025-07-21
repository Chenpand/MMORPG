#include "CSApplication.h"

#include <AvalonLogger.h>
#include <CLGuildBattleDataEntry.h>
#include <CLScoreWarDataEntry.h>
#include <CLForbitWord.h>
#include <CLSystemValueDataEntry.h>
#include <CLScriptMgr.h>
#include <CLSceneDataEntry.h>
#include <CL2V2ScoreWarDataEntry.h>

#include "CSNetwork.h"
#include "GameParamMgr.h"
#include "GameZoneMgr.h"
#include "ChallengeMgr.h"
#include "CSGuildBattleMgr.h"
#include "CSScoreWarMgr.h"
#include "CSRoomMgr.h"
#include "CLSysNotify.h"
#include "CS2V2ScoreWarMgr.h"

CSApplication::CSApplication()
{
	m_TickTime = 0;
	m_DBConn = NULL;
}

CSApplication::~CSApplication()
{
}

bool CSApplication::OnInit()
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

	if (!LoadDataTables())
	{
		ErrorLogStream << "load tables failed!" << LogStream::eos;
		return false;
	}

	if(!CSNetwork::Instance()->Startup(GetProperties(), GetNetAddress()))
	{
		ErrorLogStream << "internal network startup failed!" << LogStream::eos;
		return false;
	}

	if(!GameParamMgr::Instance()->Init())
	{
		ErrorLogStream << "gameparammgr init failed!" << LogStream::eos;
		return false;
	}

	if(!GameZoneMgr::Instance()->Init())
	{
		ErrorLogStream << "gamezonemgr init failed!" << LogStream::eos;
		return false;
	}

	if(!ChallengeMgr::Instance()->Init())
	{
		ErrorLogStream << "challengermgr init failed!" << LogStream::eos;
	}

	if (!CSGuildBattleMgr::Instance()->Init())
	{
		ErrorLogStream << "CSGuildBattleMgr init failed!" << LogStream::eos;
	}

	if (!CSScoreWarMgr::Instance()->Init())
	{
		ErrorLogStream << "CSScoreWarMgr init failed!" << LogStream::eos;
	}

	if (!CS2V2ScoreWarMgr::Instance()->Init())
	{
		ErrorLogStream << "CS2V2ScoreWarMgr init failed!" << LogStream::eos;
	}

	m_TickTime = Avalon::Time::CurrentTime().MSec();
	return true;
}

void CSApplication::OnTick(const Avalon::Time &now)
{
	CLApplication::OnTick(now);

	UInt64 offsetTime = now.MSec() - m_TickTime;
	if(offsetTime > 1000)
	{
		ErrorLogStream << "too long tick time:" << offsetTime << " msecs!" << LogStream::eos;
	}
	m_TickTime = now.MSec();

	CSNetwork::Instance()->OnTick(now);
	GameZoneMgr::Instance()->OnTick(now);
	ChallengeMgr::Instance()->OnTick(now);
	CSGuildBattleMgr::Instance()->OnTick(now);
	CSScoreWarMgr::Instance()->OnTick(now);
	CSRoomMgr::Instance()->OnTick(now);
	CS2V2ScoreWarMgr::Instance()->OnTick(now);
}

void CSApplication::OnQuit()
{
	ChallengeMgr::Instance()->Final();
	GameZoneMgr::Instance()->Final();

	GameParamMgr::Instance()->Final();
	CSNetwork::Instance()->Shutdown(200);

	Avalon::RetryDBConnection::Destroy(m_DBConn);
	m_DBConn = NULL;

	CLApplication::OnQuit();
}

bool CSApplication::LoadDataTables()
{
	std::string configpath = CONFIG_PATH;
	configpath = configpath + "DataTables/";

	if (!ForbitWordParser::Instance()->Init(configpath + "forbitword.txt")) return false;
	if (!ForbitWordParser::Instance()->InitValidCoolName(configpath + "coolname.txt")) return false;

	try
	{
		if (!SceneDataEntryMgr::Instance()->LoadData(configpath + "CitySceneTable.txt")) return false;		//adder by huchenhhui 2018.04.12
		if (!GuildTerritoryDataEntryMgr::Instance()->LoadData(configpath + "GuildTerritoryTable.txt")) return false;		//adder by huchenhhui 2018.04.12
		if (!GuildBattleTimeDataEntryMgr::Instance()->LoadData(configpath + "GuildBattleTimeTable.txt")) return false;		//adder by huchenhhui 2018.04.12
		if (!ScoreWarTimeDataEntryMgr::Instance()->LoadData(configpath + "ScoreWarTable.txt")) return false;				//adder by huchenhhui 2018.07.19
		if (!SystemValueDataEntryMgr::Instance()->LoadData(configpath + "SystemValueTable.txt")) return false;				//adder by huchenhhui 2018.07.19
		if (!ScoreWar2V2TimeDataEntryMgr::Instance()->LoadData(configpath + "ScoreWar2v2Table.txt")) return false;				//adder by huchenhhui 2018.07.19
		if (!SysNotifyDataEntryMgr::Instance()->LoadData(configpath + "CommonTipsDesc.txt")) return false;
		SysNotifyMgr::Instance()->Load(NULL, std::string());
		

	}
	catch (std::string str)
	{
		ErrorLogStream << str << LogStream::eos;
		return false;
	}
	
	return true;
}
