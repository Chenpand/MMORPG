#include "CSApplication.h"

#include <AvalonLogger.h>

#include "CSNetwork.h"
#include "GMNetwork.h"

#include "GameParamMgr.h"
#include "GameZoneMgr.h"
#include "GMClientMgr.h"
#include "PunishmentMgr.h"
#include "HttpClient.h"
#include "CLGameLoggerMgr.h"

CSApplication::CSApplication()
{
	m_TickTime = 0;
	m_DBConn = NULL;
	m_ReloadConfig = false;
}

CSApplication::~CSApplication()
{
}

bool CSApplication::OnInit()
{
	if(!CLApplication::OnInit()) return false;

	Avalon::LogStream s = DebugLogStream;
	s << "Init" << LogStream::eos;

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

	m_operateInfo.chatMonitorURL = GetProperties()->GetProperty("OperateInfo", "chat_monitor_url").GetString();
	m_operateInfo.chatMonitorMD5Key = GetProperties()->GetProperty("OperateInfo", "chat_monitor_md5_key").GetString();
	m_operateInfo.cdkVerifyURL = GetProperties()->GetProperty("OperateInfo", "cdk_verify_url").GetString();
	m_operateInfo.cdkVerifyMD5Key = GetProperties()->GetProperty("OperateInfo", "cdk_verify_md5_key").GetString();
	//if (!m_operateInfo.IsValide())
	//{
	//	ErrorLogStream << "operateInfo init failed!" << LogStream::eos;
	//	return false;
	//}

	m_PushUrl = GetProperties()->GetProperty("Push", "push_url").GetString();
	m_PushKey = GetProperties()->GetProperty("Push", "push_key").GetString();

	LoadDynamicConfig();

	if(!CSNetwork::Instance()->Startup(GetProperties(), GetNetAddress()))
	{
		ErrorLogStream << "internal network startup failed!" << LogStream::eos;
		return false;
	}

// 	if(!GMNetwork::Instance()->Startup(GetProperties(), GetNetAddress()))
// 	{
// 		ErrorLogStream << "gm network startup failed!" << LogStream::eos;
// 		return false;
// 	}

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

	if(!GMClientMgr::Instance()->Init())
	{
		ErrorLogStream << "gmclientmgr init failed!" << LogStream::eos;
		return false;
	}

 	if(!PunishmentMgr::Instance()->Init())
 	{
 		ErrorLogStream << "pinishmentmgr init failed!" << LogStream::eos;
 		//return false;
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

	GMNetwork::Instance()->OnTick(now);
	CSNetwork::Instance()->OnTick(now);
	
	GMClientMgr::Instance()->OnTick(now);
	GameZoneMgr::Instance()->OnTick(now);
	PunishmentMgr::Instance()->OnTick(now);

	HttpClient::getInstance()->updata();

	if (m_ReloadConfig)
	{
		m_ReloadConfig = false;

		if (!GetProperties()->LoadFromFile("CenterServer.cfg"))
		{
			ErrorLogStream << "load CenterServer.cfg failed!" << LogStream::eos;
			return;
		}

		LoadDynamicConfig();
	}
}

void CSApplication::OnQuit()
{
	PunishmentMgr::Instance()->Final();
	GameZoneMgr::Instance()->Final();

	GameParamMgr::Instance()->Final();

	GMClientMgr::Instance()->Final();

	GMNetwork::Instance()->Shutdown(0);
	CSNetwork::Instance()->Shutdown(200);

	Avalon::RetryDBConnection::Destroy(m_DBConn);
	m_DBConn = NULL;

	CLApplication::OnQuit();
}

void CSApplication::OnSignal(int signal)
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

void CSApplication::LoadDynamicConfig()
{
	DebugLogStream << "load centerserver dynamic config." << LogStream::eos;

	m_CanPush = !GetProperties()->GetProperty("Push", "ClosePush");
	m_CustomServerAgentAddr = GetProperties()->GetProperty("CustomService", "agent_addr").GetString();
	m_RoleListServerAddr = GetProperties()->GetProperty("RoleListServer", "role_list_addr").GetString();
	m_CheatReportAddr = GetProperties()->GetProperty("Cheat", "cheat_report_addr").GetString();
	m_PlayerVipInfoUrl = GetProperties()->GetProperty("GlobalServer", "playerInfo_query_url").GetString();
	m_PlayerAntiAddictInfoUrl = GetProperties()->GetProperty("GlobalServer", "player_antiaddict_url").GetString();
	m_AccLoginTimeUrl = GetProperties()->GetProperty("GlobalServer", "acc_login_time_url").GetString();
	m_ActivityYearSortListUrl = GetProperties()->GetProperty("HttpUrl", "activity_year_sortlist_url").GetString();
	m_AccountConvertQueryUrl = GetProperties()->GetProperty("HttpUrl", "account_convert_query_url").GetString();
	m_CheatQueryUrl = GetProperties()->GetProperty("Cheat", "cheat_query_addr").GetString();
	m_GmOpActivityLibUrl = GetProperties()->GetProperty("HttpUrl", "gm_op_activity_lib_url").GetString();
	m_GmOpActivityKey = GetProperties()->GetProperty("Key", "gm_op_activity").GetString();
	m_RedPacketRankUrl = GetProperties()->GetProperty("HttpUrl", "red_packet_rank_url").GetString();
	m_RechargePushCheckUrl = GetProperties()->GetProperty("HttpUrl", "recharge_push_check_url").GetString();

	DevLogStream << "can push(" << m_CanPush << ")!" << LogStream::eos;
}