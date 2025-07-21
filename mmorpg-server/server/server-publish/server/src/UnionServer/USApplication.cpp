#include "USApplication.h"

#include <DropItemMgr.h>
#include <CLItemDataEntry.h>
#include <CLSceneDataEntry.h>
#include <CLSysNotifyDataEntry.h>
#include <CLSysNotify.h>
#include <CLRecordClient.h>
#include <kingnet_analyzer.h>
#include <PerformanceCheck.h>
#include <CLChampionDataEntry.h>
#include <CLSystemValueDataEntry.h>
#include <CLGoldConsignmentValueDataEntry.h>
#include <CLNetworkPerformance.h>

#include "CLConfigLoadMgr.h"
#include "USNetwork.h"
#include "USConsoleNetwork.h"
#include "GameZoneMgr.h"
#include "USPlayerMgr.h"
#include "UnionServiceMgr.h"
#include "ChampionRaceMgr.h"
#include "USChampionStatus.h"
#include "USGamble.h"
#include "USGameParamMgr.h"
#include "GoldConsignmentService.h"

USApplication::USApplication()
{
	m_TickTime = 0;
	m_ReloadConfig = false;
	m_ReloadTable = false;
	m_LogTransProtocol = false;
}

USApplication::~USApplication()
{

}

bool USApplication::OnInit()
{
	if(!CLApplication::OnInit()) return false;
	
	LoadDynamicConfig();

	if (!LoadDataTables())
	{
		ErrorLogStream << "load tables failed!" << LogStream::eos;
		return false;
	}

	if (!USNetwork::Instance()->Startup(GetProperties(), GetNetAddress()))
	{
		ErrorLogStream << "internal network startup failed!" << LogStream::eos;
		return false;
	}

	if (!USConsoleNetwork::Instance()->Startup(GetProperties(), GetNetAddress()))
	{
		ErrorLogStream << "console network startup failed!" << LogStream::eos;
		return false;
	}

	if (!CLRecordClient::Instance()->Init(this, USNetwork::Instance()->FindConnectionByType(ST_GLOBAL_RECORD)))
	{
		ErrorLogStream << "record client init failed!" << LogStream::eos;
		return false;
	}

	if (!USGameParamMgr::Instance()->Init())
	{
		ErrorLogStream << "USGameParamMgr init failed!" << LogStream::eos;
		return false;
	}

	if (!GameZoneMgr::Instance()->Init())
	{
		ErrorLogStream << "game zone manager init failed!" << LogStream::eos;
		return false;
	}

	if (!UnionServiceMgr::Instance()->Init(GeOpenFunction()))
	{
		ErrorLogStream << "UnionServiceMgr init failed!" << LogStream::eos;
		return false;
	}

	if (!ChampionRaceMgr::Instance()->Init())
	{
		ErrorLogStream << "ChampionRaceMgr manager init failed!" << LogStream::eos;
		return false;
	}
	
	if (!ChampionGroupInfoMgr::Instance()->Init())
	{
		ErrorLogStream << "ChampionGroupInfoMgr manager init failed!" << LogStream::eos;
		return false;
	}

	if (!USGambleMgr::Instance()->Init())
	{
		ErrorLogStream << "USGambleMgr manager init failed!" << LogStream::eos;
		return false;
	}

	m_TickTime = Avalon::Time::CurrentTime().MSec();

	return true;
}

void USApplication::OnTick(const Avalon::Time &now)
{
	PERF_SEG_BEGIN(CLApplication);
	CLApplication::OnTick(now);
	PERF_SEG_END();

	UInt64 offsetTime = now.MSec() - m_TickTime;
	if (offsetTime > 1000)
	{
		ErrorLogStream << "too long tick time:" << offsetTime << " msecs!" << LogStream::eos;
		SHOW_PERF();
	}

	m_TickTime = now.MSec();

	PERF_SEG_BEGIN(CLRecordClient);
	CLRecordClient::Instance()->ProcessCallbacks();
	PERF_SEG_END();

	PERF_SEG_BEGIN(USNetwork);
	USNetwork::Instance()->OnTick(now);
	PERF_SEG_END();

	PERF_SEG_BEGIN(USConsoleNetwork);
	USConsoleNetwork::Instance()->OnTick(now);
	PERF_SEG_END();

	PERF_SEG_BEGIN(GameZoneMgr);
	GameZoneMgr::Instance()->OnTick(now);
	PERF_SEG_END();


	if (USGameParamMgr::Instance()->IsLoaded())
	{
		PERF_SEG_BEGIN(UnionServiceMgr);
		UnionServiceMgr::Instance()->OnTick(now);
		PERF_SEG_END();

		PERF_SEG_BEGIN(ChampionRaceMgr);
		ChampionRaceMgr::Instance()->OnTick(now);
		PERF_SEG_END();

		PERF_SEG_BEGIN(USGambleMgr);
		USGambleMgr::Instance()->OnTick(now);
		PERF_SEG_END();
	}

	if (m_ReloadConfig)
	{
		m_ReloadConfig = false;

		if (!GetProperties()->LoadFromFile("UnionServer.cfg"))
		{
			ErrorLogStream << "load UnionServer.cfg failed!" << LogStream::eos;
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

void USApplication::OnQuit()
{
	_analyzer.DestoryInstance(UDPLOG_TEAM_COPY_UID);
	GameZoneMgr::Instance()->Final();
	USNetwork::Instance()->Shutdown(200);
	USConsoleNetwork::Instance()->Shutdown(0);
	UnionServiceMgr::Instance()->OnQuit();

	CLApplication::OnQuit();
}

void USApplication::LoadDynamicConfig()
{
	// 性能统计
	CLNetworkPerfMgr::Instance()->Init(GetProperties());

	m_LogTransProtocol = GetProperties()->GetProperty("Config", "log_trans_protocol");
	m_MoveCnt = GetProperties()->GetProperty("Game", "move_cnt");
	m_PlayerLimit = GetProperties()->GetProperty("Game", "player_limit");
	{
		m_CloseMove = GetProperties()->GetProperty("Game", "close_move");
	}

	{
		std::string openFunction = GetProperties()->GetProperty("Game", "open_function").GetString();
		std::vector<std::string> typestr;
		Avalon::StringUtil::Split(openFunction, typestr, "|");
		for (auto &type : typestr)
		{
			m_OpenFunction.insert(static_cast<UnionServerFunctionType>(std::stoi(type)));
		}
	}

	{
		m_CloseOrderZone.clear();
		std::string zoneStr = GetProperties()->GetProperty("Game", "close_order_zone").GetString();
		std::vector<std::string> zoneVec;
		Avalon::StringUtil::Split(zoneStr, zoneVec, "|");
		for (auto &zone : zoneVec)
		{
			m_CloseOrderZone.insert(Avalon::StringUtil::ConvertToValue<UInt32>(zone));
		}

		InfoLogStream << "Gold Consignment close order zone: " << zoneStr << LogStream::eos;
	}

	{
		m_CloseOrderRole.clear();
		std::string roleStr = GetProperties()->GetProperty("Game", "close_order_role").GetString();
		std::vector<std::string> roleVec;
		Avalon::StringUtil::Split(roleStr, roleVec, "|");
		for (auto &roleid : roleVec)
		{
			m_CloseOrderRole.insert(Avalon::StringUtil::ConvertToValue<UInt64>(roleid));
		}

		InfoLogStream << "Gold Consignment close order role: " << roleStr << LogStream::eos;
	}

	{
		m_ReturnProfitZone.clear();
		std::string zoneStr = GetProperties()->GetProperty("Game", "return_profit_zone").GetString();
		std::vector<std::string> zoneVec;
		Avalon::StringUtil::Split(zoneStr, zoneVec, "|");
		for (auto &zone : zoneVec)
		{
			m_ReturnProfitZone.insert(Avalon::StringUtil::ConvertToValue<UInt32>(zone));
		}

		InfoLogStream << "Gold Consignment return profit zone: " << zoneStr << LogStream::eos;
	}

	{
		m_CloseOrder.clear();
		std::string orderStr = GetProperties()->GetProperty("Game", "close_order").GetString();
		std::vector<std::string> orderVec;
		Avalon::StringUtil::Split(orderStr, orderVec, "|");
		for (auto &orderid : orderVec)
		{
			m_CloseOrder.insert(Avalon::StringUtil::ConvertToValue<UInt64>(orderid));
		}

		InfoLogStream << "Gold Consignment close order: " << orderStr << LogStream::eos;
	}

	{
		bool resetGoldConsignmentPrice = GetProperties()->GetProperty("Game", "reset_gc_price");
		if (resetGoldConsignmentPrice)
		{
			auto service = UnionServiceMgr::Instance()->GetUnionService<GoldConsignmentService>();
			if (service == nullptr)
				return;

			service->ModifyUnitPrice();
		}
	}
}

void USApplication::OnSignal(int signal)
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

bool USApplication::LoadDataTables()
{
	std::string configpath = CONFIG_PATH;
	configpath = configpath + "DataTables/";

	try
	{
		DATA_TABLE_LOAD("CitySceneTable.txt", SceneDataEntryMgr::Instance());
		DATA_TABLE_LOAD("ItemTable.txt", ItemDataEntryMgr::Instance());
		DATA_TABLE_LOAD("CommonTipsDesc.txt", SysNotifyDataEntryMgr::Instance());
		DATA_TABLE_LOAD("ChampionTimeTable.txt", ChampionTimeDataEntryMgr::Instance());
		DATA_TABLE_LOAD("ChampionGambleTable.txt", ChampionGambleDataEntryMgr::Instance());
		DATA_TABLE_LOAD("ChampionRewardTable.txt", ChampionRewardDataEntryMgr::Instance());
		DATA_TABLE_LOAD("SystemValueTable.txt", SystemValueDataEntryMgr::Instance());
		DATA_TABLE_LOAD("GoldConsignmentValueTable.txt", GoldConsignmentValueDataEntryMgr::Instance());

		SysNotifyMgr::Instance()->Load(NULL, std::string());
	}
	catch (std::string str)
	{
		ErrorLogStream << str << LogStream::eos;
		return false;
	}
	
	return true;
}

bool USApplication::ReloadDataTables()
{
	std::string configpath = CONFIG_PATH;
	configpath = configpath + "DataTables/";

	try
	{
		GoldConsignmentValueDataEntryMgr::Instance()->ReloadData(configpath + "GoldConsignmentValueTable.txt");
	}
	catch (std::string str)
	{
		ErrorLogStream << str << LogStream::eos;
		return true;
	}

	return true;
}