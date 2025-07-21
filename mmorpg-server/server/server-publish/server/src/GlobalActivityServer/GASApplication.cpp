#include "GASApplication.h"

#include <AvalonLogger.h>
#include <AvalonXmlDocument.h>

#include <PerformanceCheck.h>
#include <CLRecordClient.h>
#include <CLActiveEventDataEntry.h>
#include <CLDrawPrizeDataEntry.h>
#include <CLItemDataEntry.h>
#include <CLSystemValueDataEntry.h>
#include <CLSysNotifyDataEntry.h>
#include <CLSysNotify.h>
#include <CLAnnouncementDataEntry.h>
#include <CLChannelNameSwitchDataEntry.h>
#include <CLBetHorseDataEntry.h>
#include <CLArtifactJarLotteryDataEntry.h>
#include <CLWeekSignDataEntry.h>
#include <CLWholeBargainDiscountDataEntry.h>
#include <kingnet_analyzer.h>

#include "GASInternalNetwork.h"
#include "GASGameZoneMgr.h"
#include "GambingMgr.h"
#include "GASAnnouncementMgr.h"
#include "ActivityMgr.h"
#include "GASPlayerMgr.h"
#include "BetHorseMgr.h"
#include "GASConsoleNetwork.h"
#include "GASConsoleAgent.h"
#include "HttpClient.h"
#include "ArtifactJarMgr.h"
#include "WeekSignMgr.h"
#include "GASHireMgr.h"
#include "WholeBargainMgr.h"

GASApplication::GASApplication()
{
	m_ReloadConfig = false;
	m_DBConn = NULL;
	m_InitDataLoadCount = 0;
	m_BetHorseSwitch = false;
}

GASApplication::~GASApplication()
{
}

bool GASApplication::OnInit()
{
	if (!CLApplication::OnInit()) return false;

	m_ServerIdInGM = GetProperties()->GetProperty("Server", "gm_server_id");

	LoadDynamicConfig();

	if (!LoadDataTables())
	{
		ErrorLogStream << "load tables failed!" << LogStream::eos;
		return false;
	}

	std::string cfg = GetProperties()->GetProperty("Database", "database_config").GetString();
	Avalon::XmlDocument doc(cfg);
	if (!doc.LoadFile())
	{
		ErrorLogStream << " load database xml faield!" << LogStream::eos;
		return false;
	}

	if (UL_FAIL == _analyzer.Init() || NULL == _analyzer.GetInstance(UDPLOG_GLOBAL_ACTIVITY_UID))
	{
		ErrorLogStream << "global activity udplog init failed!" << LogStream::eos;
		return false;
	}

	Avalon::XmlElement* root = doc.RootElement();
	if (root == NULL) return false;

	//加载本地数据库表
	Avalon::XmlElement* localDBNode = doc.FirstChildElement(root, "localdb");

	std::string host;
	UInt32 port = 0;
	std::string user;
	std::string passwd;
	std::string dbname;

	doc.GetAttribute(localDBNode, "host", host);
	doc.GetAttribute(localDBNode, "port", port);
	doc.GetAttribute(localDBNode, "user", user);
	doc.GetAttribute(localDBNode, "passwd", passwd);
	doc.GetAttribute(localDBNode, "dbname", dbname);

	m_DBConn = Avalon::RetryDBConnection::Create("mysql", 60, 500);
	if (!m_DBConn->Open(host.c_str(), port, user.c_str(), passwd.c_str(), dbname.c_str()))
	{
		ErrorLogStream << "connect to database " << host << ":" << port << " " << dbname << "!" << LogStream::eos;
		return false;
	}

	if (!GASInternalNetwork::Instance()->Startup(GetProperties(), GetNetAddress()))
	{
		ErrorLogStream << "internal network startup failed!" << LogStream::eos;
		return false;
	}

	if (!ConsoleNetwork::Instance()->Startup(GetProperties(), GetNetAddress()))
	{
		ErrorLogStream << "console network startup failed!" << LogStream::eos;
		return false;
	}

	GASConsoleAgent::Instance()->Init();

	if (!CLRecordClient::Instance()->Init(this, GASInternalNetwork::Instance()->FindConnectionByType(ST_GLOBAL_RECORD)))
	{
		ErrorLogStream << "record client init failed!" << LogStream::eos;
		return false;
	}

	if (!GASPlayerMgr::Instance()->Init())
	{
		ErrorLogStream << "player manager init failed!" << LogStream::eos;
		return false;
	}

	if (!ActivityMgr::Instance()->OnInit())
	{
		ErrorLogStream << "activity manager init failed!" << LogStream::eos;
		return false;
	}

	if (!BetHorseMgr::Instance()->OnInit())
	{
		ErrorLogStream << "bet horse manager init failed!" << LogStream::eos;
		return false;
	}

	if (!GASHireMgr::Instance()->OnInit())
	{
		ErrorLogStream << "GASHireMgr manager init failed!" << LogStream::eos;
		return false;
	}

	ArtifactJarMgr::Instance()->OnInit();

	WeekSignMgr::Instance()->OnInit();

	WholeBargainMgr::Instance()->OnInit();

	if (!WaitDataLoadFinished())
	{
		ErrorLogStream << "load database datas failed!" << LogStream::eos;
		return false;
	}

	InfoLogStream << GetName() << " init success!" << LogStream::eos;

	return true;
}

void GASApplication::OnTick(const Avalon::Time &now)
{
	static UInt64 s_TickTime = 0;
	s_TickTime = now.MSec();

	PERF_SEG_BEGIN(CLApplication);
	CLApplication::OnTick(now);
	PERF_SEG_END();

	PERF_SEG_BEGIN(CLRecordClient);
	CLRecordClient::Instance()->ProcessCallbacks();
	PERF_SEG_END();

	PERF_SEG_BEGIN(GASInternalNetwork);
	GASInternalNetwork::Instance()->OnTick(now);
	PERF_SEG_END();

	PERF_SEG_BEGIN(ConsoleNetwork);
	ConsoleNetwork::Instance()->OnTick(now);
	PERF_SEG_END();

	PERF_SEG_BEGIN(HttpClient);
	HttpClient::getInstance()->updata();
	PERF_SEG_END();

	PERF_SEG_BEGIN(ActivityMgr);
	ActivityMgr::Instance()->OnTick(now);
	PERF_SEG_END();

	PERF_SEG_BEGIN(GASPlayerMgr);
	GASPlayerMgr::Instance()->OnTick(now);
	PERF_SEG_END();

	PERF_SEG_BEGIN(GASAnnouncementMgr);
	GASAnnouncementMgr::Instance()->OnTick(now);
	PERF_SEG_END();

	PERF_SEG_BEGIN(GambingMgr);
	GambingMgr::Instance()->OnTick(now);
	PERF_SEG_END();

	PERF_SEG_BEGIN(BetHorseMgr);
	BetHorseMgr::Instance()->OnTick(now);
	PERF_SEG_END();

	PERF_SEG_BEGIN(ArtifactJarMgr);
	ArtifactJarMgr::Instance()->OnTick(now);
	PERF_SEG_END();

	PERF_SEG_BEGIN(WeekSignMgr);
	WeekSignMgr::Instance()->OnTick(now);
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

		if (!GetProperties()->LoadFromFile("GlobalActivityServer.cfg"))
		{
			ErrorLogStream << "load GlobalActivityServer.cfg failed!" << LogStream::eos;
			return;
		}

		LoadDynamicConfig();
		ReloadDataTables();
	}
}

void GASApplication::OnQuit()
{
	_analyzer.DestoryInstance(UDPLOG_GLOBAL_ACTIVITY_UID);
	GASGameZoneMgr::Instance()->Final();
	GambingMgr::Instance()->Final();
	ActivityMgr::Instance()->Final();
	GASAnnouncementMgr::Instance()->Final();
	GASPlayerMgr::Instance()->Final();
	BetHorseMgr::Instance()->Final();

	ConsoleNetwork::Instance()->Shutdown(0);
	GASInternalNetwork::Instance()->Shutdown(200);

	Avalon::RetryDBConnection::Destroy(m_DBConn);
	m_DBConn = NULL;

	CLApplication::OnQuit();
}

bool GASApplication::WaitDataLoadFinished()
{
	CLRecordClient::Instance()->ProcessCallbacks();
	while (m_InitDataLoadCount != 0 && IsAlive())
	{
		Avalon::Thread::Sleep(100);
		CLRecordClient::Instance()->ProcessCallbacks();
	}

	// WSNetwork::Instance()->OnTick(CURRENT_TIME);
	return m_InitDataLoadCount == 0 && IsAlive();
}

bool GASApplication::LoadDataTables()
{
	std::string configpath = CONFIG_PATH;
	configpath = configpath + "DataTables/";

	try
	{
		if (!AnnouncementDataEntryMgr::Instance()->LoadData(configpath + "AnnounceTable.txt")) return false;
		if (!SystemValueDataEntryMgr::Instance()->LoadData(configpath + "SystemValueTable.txt")) return false;
		if (!SysNotifyDataEntryMgr::Instance()->LoadData(configpath + "CommonTipsDesc.txt")) return false;
		SysNotifyMgr::Instance()->Load(NULL, std::string());

		if (!ItemDataEntryMgr::Instance()->LoadData(configpath + "ItemTable.txt")) return false;
		if (!OpActivityDataEntryMgr::Instance()->LoadData(configpath + "OpActivityTable.txt")) return false;
		if (!OpActivityTaskDataEntryMgr::Instance()->LoadData(configpath + "OpActivityTaskTable.txt")) return false;
		if (!GambingItemDataEntryMgr::Instance()->LoadData(configpath + "GambingItemTable.txt")) return false;
		if (!ChannelNameSwitchDataEntryMgr::Instance()->LoadData(configpath + "ChannelNameSwitch.txt")) return false;

		if (!BetHorseCfgEntryMgr::Instance()->LoadData(configpath + "BetHorseCfg.txt")) return false;
		if (!BetHorseShooterEntryMgr::Instance()->LoadData(configpath + "BetHorseShooter.txt")) return false;
		if (!BetHorseMapEntryMgr::Instance()->LoadData(configpath + "BetHorseMap.txt")) return false;
		if (!BetHorseRandEntryMgr::Instance()->LoadData(configpath + "BetHorseRand.txt")) return false;
		if (!BetHorseRateEntryMgr::Instance()->LoadData(configpath + "BetHorseRate.txt")) return false;
		if (!BetHorseShooterStatusEntryMgr::Instance()->LoadData(configpath + "BetHorseShooterStatus.txt")) return false;
		if (!BetHorseOccuSuppressEntryMgr::Instance()->LoadData(configpath + "BetHorseOccuSuppress.txt")) return false;
		if (!JarBonusDataMgr::Instance()->LoadData(configpath + "JarBonus.txt")) return false;
		if (!ArtifactJarLotteryDataEntryMgr::Instance()->LoadData(configpath + "ArtifactJarLotteryTable.txt")) return false;
		if (!WeekSignDataEntryMgr::Instance()->LoadData(configpath + "WeekSignTable.txt")) return false;
		if (!WholeBargainDiscountDataEntryMgr::Instance()->LoadData(configpath + "WholeBargainDiscountTable.txt")) return false;
	}
	catch (std::string str)
	{
		ErrorLogStream << str << LogStream::eos;
		return false;
	}
	
	return true;
}

void GASApplication::ReloadDataTables()
{
	std::string configpath = CONFIG_PATH;
	configpath = configpath + "DataTables/";
	try
	{
		if (OpActivityDataEntryMgr::Instance()->ReloadData(configpath + "OpActivityTable.txt"))
		{
			ActivityMgr::Instance()->ReloadTable();
		}
	}
	catch (std::string str)
	{
		ErrorLogStream << str << LogStream::eos;
	}
}

void GASApplication::OnSignal(int signal)
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

void GASApplication::LoadDynamicConfig()
{
	DebugLogStream << "load dynamic config." << LogStream::eos;

	m_BetHorseSwitch = GetProperties()->GetProperty("FunSwitch", "bet_horse");
	m_OpenBetHorseResend = GetProperties()->GetProperty("FunSwitch", "bet_horse_reward_resend");
	m_GmOpActivityLibUrl = GetProperties()->GetProperty("HttpUrl", "gm_op_activity_lib_url").GetString();
	m_GmOpActivityKey = GetProperties()->GetProperty("Key", "gm_op_activity").GetString();
}
