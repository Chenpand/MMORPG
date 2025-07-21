#include "WSApplication.h"

#include <AvalonMemoryTracker.h>
#include <AvalonLogger.h>
#include <AvalonTimer.h>
#include <CLRecordClient.h>
#include <CLGameLoggerMgr.h>
#include <CLForbitWord.h>
#include <CLSceneDataEntry.h>
#include <CLMallDataEntry.h>
#include <CLVipDataEntry.h>
#include <CLAnnouncementDataEntry.h>
#include <kingnet_analyzer.h>
#include <CLDBLogger.h>
#include <CLGiftBagDataEntry.h>
#include <CLDungeonDataEntry.h>
#include <CLDropItemDataEntry.h>
#include <CLSysNotifyDataEntry.h>
#include <CLSeasonRewardDataEntry.h>
#include <CLMonsterPrefixDataEntry.h>
#include <CLMonsterGroupDataEntry.h>
#include <CLMonsterAttributeTable.h>
#include <CLDungeonPrefixLimitDataEntry.h>
#include <CLDungeonDestructTable.h>
#include <CLDungeonHellDataEntry.h>
#include <CLDeathTowerAwardDataEntry.h>
#include <CLRaceEndDropDataEntry.h>
#include <CLMonsterDataEntry.h>
#include <CLDungeonAreaDataEntry.h>
#include <CLDungeonChestDataEntry.h>
#include <CLSystemValueDataEntry.h>
#include <CLRareItemDataEntry.h>
#include <CLFunctionDataEntry.h>
#include <CLShopItemDataEntry.h>
#include <CLGuildDataEntry.h>
#include <CLGuildBuildingDataEntry.h>
#include <CLGuildSkillDataEntry.h>
#include <CLGuildStatueDataEntry.h>
#include <CLGuildTableDataEntry.h>
#include <CLRedPacketDataEntry.h>
#include <CLGuildBattleDataEntry.h>
#include <CLGuildEmblemDataEntry.h>
#include <CLGuildBuildingPointDataEntry.h>
#include <CLTeamDungeonDataEntry.h>
#include <CLChargeMallDataEntry.h>
#include <CLChargePacketDataEntry.h>
#include <CLTeamRewardDataEntry.h>
#include <CLDungeonTimesDataEntry.h>
#include <CLLevelAdapterDataEntry.h>
#include <CLSeasonDataEntry.h>
#include <CLCurrencyCostDataEntry.h>
#include <CLDungeonGuaranteeDropDataEntry.h>
#include <CLMallGiftPackDataEntry.h>
#include <CLDungeonDailyDropDataEntry.h>
#include <CLDungeonTimeDataEntry.h>
#include <CLPremiumLeagueDataEntry.h>
#include <CLDungeonVerifyProbDataEntry.h>
#include <CLLoginDataEntry.h>
#include <CLActivityMonsterDataEntry.h>
#include <CLActivitySuitAdditionDataEntry.h>
#include <CLDrawPrizeDataEntry.h>
#include <CLEqualItemDataEntry.h>
#include <CLActivityMonsterProbDataEntry.h>
#include <CLTaskDataEntry.h>
#include <CLMasterSysGiftDataEntry.h>
#include <CLScoreWarDataEntry.h>
#include <CLDigDataEntry.h>
#include <CLFriendWelfareAddDataEntry.h>
#include <CLDropStrengthenControlDataEntry.h>
#include <CLAccountShopItemDataEntry.h>
#include <CLGuildDungeonTimeDataEntry.h>
#include <CLGuildDungeonTypeDataEntry.h>
#include <CLGuildDungeonLvlDataEntry.h>
#include <CLGuildDungeonRewardDataEntry.h>
#include <CLGuildDungeonLotteryDataEntry.h>
#include <CLGuildDungeonDamageCoeffDataEntry.h>
#include <CLGuildBattleScoreRankRewardDataEntry.h>
#include <CLItemBuffDataEntry.h>
#include <CLAdventureTeamDataEntry.h>
#include <CLAuctionSysValueDataEntry.h>
#include <CLAuctionPageStruct.h>
#include <CLBlackMarketItemDataEntry.h>
#include <CLRoleValueTotalScoreCalcDataEntry.h>
#include <CLAdventureTeamGradeDataEntry.h>
#include <CLAccountCounterDataEntry.h>
#include <CLExpeditionMapDataEntry.h>
#include <CLExpeditionMapRewardDataEntry.h>
#include <CLPlayerDataEntry.h>
#include <CLRechargePushDataEntry.h>
#include <CLAuctionRecoveItemDataEntry.h>
#include <CLNewTitleDataEntry.h>
#include <CLAuctionNoTreasAverPriceMinNumDataEntry.h>
#include <CLAuctionMagicStrengAdditDataEntry.h>
#include <CLDropSmellControlDataEntry.h>
#include <CLDailyTodoDataEntry.h>
#include <CLNpcDataEntry.h>
#include <CLGuildProsperityDataEntry.h>
#include <CLDungeonClearTimeDataEntry.h>
#include <CLMallShopMultiIDataEntry.h>
#include <CLDropItemVipLimitDataEntry.h>
#include <CLAdventurePassDataEntry.h>
#include <CLAuctionMagicExpectUpDataEntry.h>
#include <CLAdventWeeklyTaskAddRewardDataEntry.h>
#include <CL2V2ScoreWarDataEntry.h>
#include <CLHonorRewardDataEntry.h>
#include <CLHireTaskDataEntry.h>
#include <CLGuildHonorDataEntry.h>
#include <CLUnionServerProtocol.h>
#include <CLNetworkPerformance.h>

#include "WSRouter.h"
#include "WSNetwork.h"
#include "TimerScript.h"
#include "ScriptMgr.h"
#include "WSPlayerMgr.h"
#include "WSSceneMgr.h"
#include "ChatDataMgr.h"
#include "MailBox.h"
#include "WSTeamMgr.h"
#include "WSSortListMgr.h"
#include "WorldBossMgr.h"
#include "WSConsoleAgent.h"
#include "PunishmentMgr.h"
#include "AnnouncementMgr.h"
#include "BroadcastMailMgr.h"
#include "WSActivityMgr.h"
#include "GameParamMgr.h"
#include "MallMgr.h"
#include "OfflineNotifyMgr.h"
#include "DropItemMgr.h"
#include "PerformanceCheck.h"
#include "NewAuctionMgr.h"
#include "WSDungeonConfigMgr.h"
#include "DungeonRaceMgr.h"
#include "DungeonMonsterGenerator.h"
#include "SysRecordMgr.h"
#include "GuildMgr.h"
#include "RedPacketMgr.h"
#include "GuildBattleMgr.h"
#include "GuildDungeonMgr.h"
#include "GuildDungeonVerifyMgr.h"
#include "GuildAuctionMgr.h"
#include "ChargeManager.h"
#include "AccountMgr.h"
#include "TeamMatchMgr.h"
#include "SysRecordMgr.h"
#include "WSSeasonMgr.h"
#include "WSJarMgr.h"
#include "FigureStatueMgr.h"
#include "PremiumLeagueMgr.h"
#include "WSActivityMonsterMgr.h"
#include "AccountActivityTaskMgr.h"
#include "RoomMgr.h"
#include "WSServiceSwitchMgr.h"
#include "DigMapMgr.h"
#include "LoginPushMgr.h"
#include "WSShopMgr.h"
#include "AuctionTransRecord.h"
#include "AuctionAttentRecord.h"
#include "BlackMarketMgr.h"
#include "PlayerJoins.h"
#include "AdventurePassMgr.h"
#include "ItemFriendPresent.h"
#include "HireMgr.h"


WSApplication::WSApplication()
	:m_InitDataLoadCount(0)
{
	m_TickTime = 0;
	m_ReloadConfig = false;
	m_MaxOnlinePlayer = 0;
	m_MaxLoadingPlayer = 0;
	m_OpenLoginQueue = false;
	m_CheckMd5 = true;
	m_ReloabTable = false;
	m_StayTeamWhenLeaveRace = false;
	m_KickWhenStaticValueCheckError = true;
	m_OpenVerifyDungeon = true;
	m_VerifyCheatPunishType = 0;
	m_KickWhenSpeedCheat = false;
	m_CheckEnterAreaFrame = false;
	m_CheckAreaStartIndex = 0;
	m_CheckPickDropFrame = false;
	m_IsRecordPkReplay = true;
	m_NeedRecordDropQuality = 0;
	m_GMQueryPlayerMaxNum = 0;
	m_SuperNewServiceFlag = false;
	m_CheckGmDataLoadSituation = true;
	m_CheckUpdateRelation = false;
	m_OptimUpdateRelation = false;

	m_AuctionQueryPriceListInterval = 1000;
	m_AuctionSqlJointModify = true;
	m_AuctionSqlInModify = true;
	m_AuctionPriceListCacheTime = 10;
	m_AccountDataFlush = false;
	m_BattleFlag = 0;
	m_AuctionDueOffSaleTime = 14;
	m_NotifyGiveupVerifyWhenRaceUnexist = true;
	m_DevilDropBug = true;

	m_TickTimer200ms.SetInterval(200);
	m_TickTimer500ms.SetInterval(500);
	m_TickTimer1000ms.SetInterval(1000);
	m_TickTimerMin.SetInterval(60 * 1000);

	m_AccountDelayedRefreshFlag = false;
	m_NewDailyDrop = true;
	m_CheckChargeAccount = true;
	m_LoadOtherRoleInfo = true;
	m_CheckLevelWhenFlyupGift = true;

	m_databaseTableCount = 1;

	m_CloseFixTeamMatch = false;
	m_GuildBattleScoreRank = 0;
	m_TeamAutoKick = true;
	m_HireTurn = false;
	m_IsUseAuctionNumCache = false;
	m_AuctionNumCacheTime = 5;
	m_PrintRelationsUid = false;
	m_NewReloadTable = false;
	m_AuctionOpUseMem = false;
	m_CloseMasterOffPubTask = false;
	m_CloseOffPresentGive = false;
}

WSApplication::~WSApplication()
{
}

bool WSApplication::OnInit()
{
	if(!CLApplication::OnInit()) return false;

	std::string configpath = CONFIG_PATH;
	std::string rootpath = ROOT_PATH;
	if(!GetProperties()->LoadFromFile(rootpath + "Game.cfg"))
	{
		ErrorLogStream << "load game config failed!" << LogStream::eos;
		return false;
	}
	m_BillingSigKey = GetProperties()->GetProperty("Billing", "billing_sig_key").GetString();
	PLATFORM = GetProperties()->GetProperty("Platform", "platform").GetString();
	NEUTRAL_ZONE_ID = GetProperties()->GetProperty("Game", "neutral_zone_id");
	GAME_VERSION = GetProperties()->GetProperty("Server", "version").GetString();
	LoadDynamicConfig();

	if(!LoadDataTables())
	{
		ErrorLogStream << "load tables failed!" << LogStream::eos;
		return false;
	}

	RegisterReloadDataTables();

	Avalon::TimerReactor::Instance();
	MailBox::Init();

	if(!ScriptMgr::Instance()->Init())
	{
		ErrorLogStream << "script system init failed!" << LogStream::eos;
		return false;
	}

	if(!WSNetwork::Instance()->Startup(GetProperties(), GetNetAddress()))
	{
		ErrorLogStream << "network startup failed!" << LogStream::eos;
		return false;
	}

	std::vector<Avalon::NetConnectionPtr> connVec;
	WSNetwork::Instance()->FindConnectionByType(ST_RECORD, connVec);

	if(connVec.empty() || !CLRecordClient::Instance()->Init(this, connVec))
	{
		ErrorLogStream << "record client init failed!" << LogStream::eos;
		return false;
	}

	GameParamMgr::Instance()->Init();
	WSConsoleAgent::Instance()->Init();
	ChatDataMgr::Instance()->Init();
	WSSortListMgr::Instance()->Init();
	PunishmentMgr::Instance()->Init();
	AnnouncementMgr::Instance()->Init();
	BroadcastMailMgr::Instance()->Init();
	OfflineNotifyMgr::Instance()->Init();
	PremiumLeagueMgr::Instance()->OnInit();

	if(!WSSceneMgr::Instance()->Init())
	{
		ErrorLogStream << "load scenes config failed!" << LogStream::eos;
		return false;
	}
	
    zjy_auction::AuctionMgr::Instance()->Init();
	AuctionTransRecordMgr::Instance()->Init();

	AuctionAttentMgr::Instance()->Init();

	if(!WSPlayerMgr::Instance()->Init())
	{
		ErrorLogStream << "playermgr init failed!" << LogStream::eos;
		return false;
	}

	if(!GameLoggerMgr::Instance()->Init(GAME_LOGGER_CFG, IsDebugMode(), WSNetwork::Instance()->FindConnectionByType(ST_ADMIN)))
	{
		ErrorLogStream << "game loggers init failed!" << LogStream::eos;
		return false;
	}

	BlackMarketMgr::Instance()->Init();

	PlayerJoinMgr::Instance()->Init();

	if(!WaitDataLoadFinished())
	{
		ErrorLogStream << "load database datas failed!" << LogStream::eos;
		return false;
	}

	if (!ChargeManager::Instance()->Init())
	{
		ErrorLogStream << "charge manager init failed!" << LogStream::eos;
		return false;
	}

	if (!TeamMatchMgr::Instance()->OnInit())
	{
		ErrorLogStream << "team match manager init failed!" << LogStream::eos;
		return false;
	}

	//ScriptMgr::Instance()->GetTimerScript()->Init();
	
	FigureStatueMgr::Instance()->Init();

	if (!MallMgr::Instance()->Init())
	{
		ErrorLogStream << "mall manager init failed!" << LogStream::eos;
		return false;
	}

	LoginPushMgr::Instance()->Init();

	if (!WSActivityMgr::Instance()->Init())
	{
		ErrorLogStream << "load OpActivity failed!" << LogStream::eos;
		return false;
	}

	SysRecordMgr::Instance()->Init();
	if (!DropItemMgr::Instance()->Init(SysRecordMgr::Instance()))
    {
        ErrorLogStream << "drop item manager init failed!" << LogStream::eos;
        return false;
    }

	if (!DungeonMonsterGenerator::Instance()->Init())
	{
		ErrorLogStream << "dungeon moster generator init failed!" << LogStream::eos;
		return false;
	}

	if(UL_FAIL == _analyzer.Init() || NULL == _analyzer.GetInstance(UDPLOG_WORLD_UID))
	{
		ErrorLogStream << "udplog init failed!" << LogStream::eos;
		return false;
	}

	if (!WSDungeonConfigMgr::Instance()->Initialize())
	{
		ErrorLogStream << "dungeon config manager init failed!" << LogStream::eos;
		return false;
	}

	if (!RedPacketMgr::Instance()->Init())
	{
		ErrorLogStream << "red packet manager init failed!" << LogStream::eos;
		return false;
	}

	GuildMgr::Instance()->Init();

	if (!GuildBattleMgr::Instance()->Init())
	{
		ErrorLogStream << "guild battle manager init failed!" << LogStream::eos;
		return false;
	}

	if (!WSSeasonMgr::Instance()->Init())
	{
		ErrorLogStream << "season manager init failed!" << LogStream::eos;
		return false;
	}

	//取消新区标志
	GameParamMgr::Instance()->SetValue("new_combinezone", 0);

	m_TickTime = Avalon::Time::CurrentTime().MSec();

	//初始化罐子记录
	JarRewardRecordsMgr::Instance()->Init();

	WSActivityMonsterMgr::Instance()->Init();

	if (!DigMapMgr::Instance()->Init())
	{
		ErrorLogStream << "dig map mgr init failed!" << LogStream::eos;
		return false;
	}


	GuildDungeonMgr::Instance()->Init();
	GuildDungeonVerifyMgr::Instance()->Init();
	GuildAuctionMgr::Instance()->Init();

	if (!WSShopMgr::Instance()->Init())
	{
		ErrorLogStream << "wsshop mgr init failed!" << LogStream::eos;
		return false;
	}

	//BlackMarketMgr::Instance()->InitOpActInfo();
	BlackMarketMgr::Instance()->InitActInfo();

	AdventurePassMgr::Instance()->Init();

	if (WSApplication::Instance()->IsHireTurn())
	{
		HireMgr::Instance()->Init();
	}

	FriendPresentMgr::Instance()->Init();

	CheckTime(Avalon::Time::CurrentTime());

	NotifyUnionZoneIDChanged();

	return true;
}

void WSApplication::OnTick(const Avalon::Time &now)
{
    UInt64 offsetTime = now.MSec() - m_TickTime;
    if (offsetTime > 1000)
    {
        ErrorLogStream << "Too long tick time:" << offsetTime << " msecs!" << LogStream::eos;
        //SHOW_PERF();
    }
    m_TickTime = now.MSec();

    RESET_PERF();

    PERF_SEG_BEGIN(Application);
	CLApplication::OnTick(now);
    PERF_SEG_END();

    PERF_SEG_BEGIN(TimerReactor);
	Avalon::TimerReactor::Instance()->HandleEvent(now);
    PERF_SEG_END();

    PERF_SEG_BEGIN(Network);
	WSNetwork::Instance()->OnTick(now);
    PERF_SEG_END();

    PERF_SEG_BEGIN(RecordClient);
	CLRecordClient::Instance()->ProcessCallbacks();
    PERF_SEG_END();

	PERF_SEG_BEGIN(WSSortListMgr);
	WSSortListMgr::Instance()->OnTick(now);
	PERF_SEG_END();

	PERF_SEG_BEGIN(WSDungeonConfigMgr);
	WSDungeonConfigMgr::Instance()->OnTick(now);
	PERF_SEG_END();

	PERF_SEG_BEGIN(DungeonRaceMgr);
	DungeonRaceMgr::Instance()->OnTick(now);
	PERF_SEG_END();

	if (m_TickTimer200ms.IsTimeout(now))
	{
		PERF_SEG_BEGIN(WSSceneMgr);
		WSSceneMgr::Instance()->OnTick(now);
		PERF_SEG_END();

		PERF_SEG_BEGIN(AccountMgr);
		AccountMgr::Instance()->OnTick(now);
		PERF_SEG_END();

		PERF_SEG_BEGIN(WSPlayerMgr);
		WSPlayerMgr::Instance()->OnTick(now);
		PERF_SEG_END();

		PERF_SEG_BEGIN(WSActivityMgr);
		WSActivityMgr::Instance()->OnTick(now);
		PERF_SEG_END();

		PERF_SEG_BEGIN(GuildMgr);
		GuildMgr::Instance()->OnTick(now);
		PERF_SEG_END();

		PERF_SEG_BEGIN(GuildBattleMgr);
		GuildBattleMgr::Instance()->OnTick(now);
		PERF_SEG_END();

		PERF_SEG_BEGIN(GuildDungeonMgr);
		GuildDungeonMgr::Instance()->OnTick(now);
		PERF_SEG_END();

		PERF_SEG_BEGIN(GuildDungeonVerifyMgr);
		GuildDungeonVerifyMgr::Instance()->OnTick(now);
		PERF_SEG_END();

		PERF_SEG_BEGIN(GuildAuctionMgr);
		GuildAuctionMgr::Instance()->OnTick(now);
		PERF_SEG_END();
	}

	if (m_TickTimer500ms.IsTimeout(now))
	{
		PERF_SEG_BEGIN(AnnouncementMgr);
		AnnouncementMgr::Instance()->OnTick(now);
		PERF_SEG_END();

		PERF_SEG_BEGIN(PunishmentMgr);
		PunishmentMgr::Instance()->OnTick(now);
		PERF_SEG_END();

		PERF_SEG_BEGIN(RedPacketMgr);
		RedPacketMgr::Instance()->OnTick(now);
		PERF_SEG_END();

		PERF_SEG_BEGIN(WSTeamMgr);
		WSTeamMgr::Instance()->OnTick(now);
		PERF_SEG_END();

		PERF_SEG_BEGIN(TeamMatchMgr);
		TeamMatchMgr::Instance()->OnTick(now);
		PERF_SEG_END();

		PERF_SEG_BEGIN(SysRecordMgr);
		SysRecordMgr::Instance()->OnTick(now);
		PERF_SEG_END();

		PERF_SEG_BEGIN(WSSeasonMgr);
		WSSeasonMgr::Instance()->OnTick(now);
		PERF_SEG_END();

		PERF_SEG_BEGIN(MallMgr);
		MallMgr::Instance()->OnTick(now);
		PERF_SEG_END();

		PERF_SEG_BEGIN(PremiumLeagueMgr);
		PremiumLeagueMgr::Instance()->OnTick(now);
		PERF_SEG_END();

		PERF_SEG_BEGIN(WSActivityMonsterMgr);
		WSActivityMonsterMgr::Instance()->OnTick(now);
		PERF_SEG_END();

		PERF_SEG_BEGIN(AccountActivityTaskMgr);
		AccountActivityTaskMgr::Instance()->OnTick(now);
		PERF_SEG_END();

		if (WSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_ROOM))
		{
			PERF_SEG_BEGIN(RoomMgr);
			RoomMgr::Instance()->OnTick(now);
			PERF_SEG_END();
		}

		if (WSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_DIG))
		{
			PERF_SEG_BEGIN(DigMapMgr);
			DigMapMgr::Instance()->OnTick(now);
			PERF_SEG_END();
		}

		PERF_SEG_BEGIN(AdventurePassMgr);
		AdventurePassMgr::Instance()->OnTick(now);
		PERF_SEG_END();
	}

	if (m_TickTimer1000ms.IsTimeout(now))
	{
		PERF_SEG_BEGIN(WSAuctionMgr);
		zjy_auction::AuctionMgr::Instance()->OnTick(now);
		PERF_SEG_END();

		PERF_SEG_BEGIN(AuctionTransRecordMgr);
		AuctionTransRecordMgr::Instance()->OnTick(now);
		PERF_SEG_END();

		PERF_SEG_BEGIN(BlackMarketMgr);
		BlackMarketMgr::Instance()->OnTick(now);
		PERF_SEG_END();

		PERF_SEG_BEGIN(PlayerJoinMgr);
		PlayerJoinMgr::Instance()->OnTick(now);
		PERF_SEG_END();

		if (WSApplication::Instance()->IsHireTurn())
		{
			PERF_SEG_BEGIN(HireMgr);
			HireMgr::Instance()->OnTick(now);
			PERF_SEG_END();
		}
	}

	if (m_TickTimerMin.IsTimeout(now))
	{
		REPORT_MEMORY_OBJECT(gSysLogger);
	}
	
	PERF_SEG_BEGIN(ScriptMgr);
	ScriptMgr::Instance()->OnTick(now);
	PERF_SEG_END();

    PERF_SEG_BEGIN(CheckTime);
	CheckTime(now);
    PERF_SEG_END();


	if (m_ReloadConfig)
	{
		m_ReloadConfig = false;

		if (!GetProperties()->LoadFromFile("WorldServer.cfg"))
		{
			ErrorLogStream << "load WorldServer.cfg failed!" << LogStream::eos;
			return;
		}

		LoadDynamicConfig();

		ScriptMgr::Instance()->ReloadScript(WORLD_SCRIPT_TIMER);
		ScriptMgr::Instance()->ReloadScript(WORLD_SCRIPT_CONFIG);
	}

	if (m_ReloabTable)
	{
		m_ReloabTable = false;
		ReloadDataTables();
	}
}

void WSApplication::OnQuit()
{
	_analyzer.DestoryInstance(UDPLOG_WORLD_UID);
	GameLoggerMgr::Instance()->Final();

	AccountMgr::Instance()->Final();
	WSPlayerMgr::Instance()->Final();
	WSSceneMgr::Instance()->Final();

	RedPacketMgr::Instance()->Final();

	OfflineNotifyMgr::Instance()->Final();
	BroadcastMailMgr::Instance()->Final();
	AnnouncementMgr::Instance()->Final();
	PunishmentMgr::Instance()->Final();
	WSSortListMgr::Instance()->Final();
	ChatDataMgr::Instance()->Final();

	CLRecordClient::Instance()->Final();
	WSNetwork::Instance()->Shutdown(100);

	ScriptMgr::Instance()->Final();
	GameParamMgr::Instance()->Final();

	Avalon::TimerReactor::Destroy();
	ForbitWordParser::Instance()->Final();
	WSConsoleAgent::Destroy();
	GuildDungeonMgr::Instance()->Final();

	CLApplication::OnQuit();
}

void WSApplication::OnDayChanged()
{
	WSSortListMgr::Instance()->OnDayChanged();
	WSPlayerMgr::Instance()->OnDayChanged();
	MallMgr::Instance()->OnDayChanged();
	GuildBattleMgr::Instance()->OnDayChanged();
	GameParamMgr::Instance()->CalcWorldLevel(CURRENT_TIME.Sec());
	zjy_auction::AuctionMgr::Instance()->OnDayChanged();
	AdventurePassMgr::Instance()->OnDayChanged();
}

void WSApplication::OnSignal(int signal)
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
		m_ReloabTable = true;
		break;
	}
	default:
	{
		ServerApplication::OnSignal(signal);
		break;
	}
	}
}

void WSApplication::OnWeekChanged()
{
	GuildMgr::Instance()->OnWeekChanged();
	WSSortListMgr::Instance()->OnWeekChanged();
	AdventurePassMgr::Instance()->OnWeekChange();
	if (WSApplication::Instance()->IsHireTurn())
	{
		HireMgr::Instance()->OnWeekChange();
	}
}

void WSApplication::OnMonthChanged()
{
	if (WSApplication::Instance()->IsHireTurn())
	{
		HireMgr::Instance()->OnMonthChange();
	}
}




bool WSApplication::IsOnChangedConditionReady()
{
	if (GuildMgr::Instance()->IsInitFinish())
	{
		return true;
	}
	return false;
}

void  WSApplication::LoadDynamicConfig()
{
	DebugLogStream << "load worldserver dynamic config." << LogStream::eos;

	// 性能统计
	CLNetworkPerfMgr::Instance()->Init(GetProperties());

	SERVER_NAME = GetProperties()->GetProperty("Server", "name").GetString();
	MERGED_SERVERS = GetProperties()->GetProperty("Server", "merged_servers").GetString();

	m_MaxOnlinePlayer = GetProperties()->GetProperty("Player", "max_online_player_num");
	// 没有配置的话，就用默认值
	if (m_MaxOnlinePlayer == 0)
	{
		m_MaxOnlinePlayer = 3000;
	}

	m_MaxLoadingPlayer = GetProperties()->GetProperty("Player", "max_loading_player_num");
	// 没有配置的话，就用默认值
	if (m_MaxLoadingPlayer == 0)
	{
		m_MaxLoadingPlayer = 200;
	}

	std::string roleListStr = GetProperties()->GetProperty("SortList", "BlackRoleList").GetString();
	WSSortListMgr::Instance()->ReloadBlackRoleList(roleListStr);

	m_CloseFixTeamMatch = GetProperties()->GetProperty("Game", "CloseFixTeamMatch");
	m_CheckRepeatStartDungeon = GetProperties()->GetProperty("Game", "CheckRepeatDungeon");
	m_CheckChargeOrder = !GetProperties()->GetProperty("Game", "CloseChargeOrderCheck");
	m_OpenLoginQueue = GetProperties()->GetProperty("Game", "OpenLoginQueue");
	m_CheatPunish = GetProperties()->GetProperty("Game", "CheatPunish");
	m_CheckMd5 = !GetProperties()->GetProperty("Game", "CloseMd5");
	m_StayTeamWhenLeaveRace = GetProperties()->GetProperty("Game", "StayTeamWhenLeaveRace");
	m_KickWhenStaticValueCheckError = !GetProperties()->GetProperty("Game", "OnlyLogWhenStaticValueCheckError");
	m_OpenRedPacket = !GetProperties()->GetProperty("Game", "CloseRedPacket");
	m_NeedRecordDropQuality = GetProperties()->GetProperty("Game", "RecordDropQuality");

	
	m_OpenVerifyDungeon = !GetProperties()->GetProperty("Verify", "CloseVerify");
	m_TowerKickWhenCheat = GetProperties()->GetProperty("Verify", "TowerKickWhenCheat");
	m_DevilddomDailyCheatNum = GetProperties()->GetProperty("Verify", "DevilddomDailyCheatNum");
	m_GuildDungeonDailyCheatNum = GetProperties()->GetProperty("Verify", "GuildDungeonDailyCheatNum");
	m_KickCheatNum = GetProperties()->GetProperty("Verify", "KickCheatNum");
	m_IsRecordDungeonLog = GetProperties()->GetProperty("Verify", "RecordDungeonLog");
	m_IsRecordDungeonReplay = GetProperties()->GetProperty("Verify", "RecordDungeonReplay");
	m_KickWhenSpeedCheat = GetProperties()->GetProperty("Verify", "KickWhenSpeedCheat");
	m_CheckEnterAreaFrame = GetProperties()->GetProperty("Verify", "CheckEnterAreaFrame");
	m_CheckAreaStartIndex = GetProperties()->GetProperty("Verify", "CheckAreaStartIndex");
	m_CheckPickDropFrame = GetProperties()->GetProperty("Verify", "CheckPickDropFrame");
	m_IsRecordPkReplay = !GetProperties()->GetProperty("Verify", "ClosePkReplay");
	m_NotifyGiveupVerifyWhenRaceUnexist = !GetProperties()->GetProperty("Verify", "CloseNotifyGiveupVerifyWhenRaceUnexist");
	m_TeamCopyDailyCheatNum = GetProperties()->GetProperty("Verify", "TeamCopyDailyCheatNum");

	{
		m_TeamCopyCheatForbidTime.clear();
		std::string str = GetProperties()->GetProperty("Verify", "TeamCopyCheatForbidTime").GetString();
		std::vector<std::string> strVec;
		Avalon::StringUtil::Split(str, strVec, "|");
		for (auto& elem : strVec)
		{
			m_TeamCopyCheatForbidTime.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(elem));
		}
	}

	m_databaseTableCount = GetProperties()->GetProperty("Database", "TableCount");

	{
		std::string str = GetProperties()->GetProperty("AccountConvert", "ConvertPf").GetString();
		Avalon::StringUtil::Split(str, m_ConvertPfs, "|");
		m_DownloadUrl = GetProperties()->GetProperty("AccountConvert", "download_url").GetString();
		InfoLogStream << "need convert pfs: " << str << LogStream::eos;
	}

	m_GMQueryPlayerMaxNum = GetProperties()->GetProperty("GM", "QueryPlayerMaxNum");
	if (m_GMQueryPlayerMaxNum == 0)
	{
		m_GMQueryPlayerMaxNum = 30;
	}

	m_SuperNewServiceFlag = GetProperties()->GetProperty("Game", "OpenSuperNewService");

	{
		std::string str = GetProperties()->GetProperty("Game", "SuperNewServiceOpActs").GetString();
		std::vector<std::string> strVec;
		Avalon::StringUtil::Split(str, strVec, "|");
		for (auto& elem : strVec)
		{
			m_SuperNewServiceOpActs.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(elem));
		}
	}

	//m_CheckGmDataLoadSituation = !GetProperties()->GetProperty("Game", "CloseCheckGmOpAct");
	m_CheckGmDataLoadSituation = GetProperties()->GetProperty("Game", "OpenCheckGmOpAct");

	m_CheckUpdateRelation = GetProperties()->GetProperty("Game", "CheckUpdateRelation");

	m_OptimUpdateRelation = GetProperties()->GetProperty("Game", "OptimUpdateRelation");

	m_DropCountPath = GetProperties()->GetProperty("Game", "DropCountPath").GetString();
	if (m_DropCountPath == "")
	{
		m_DropCountPath = std::string(ROOT_PATH) + std::string("DropItemCount/");
	}

	m_DigCountPath = GetProperties()->GetProperty("Game", "DigCountPath").GetString();
	if (m_DigCountPath == "")
	{
		m_DigCountPath = std::string(ROOT_PATH) + std::string("DigItemCount/");
	}

	m_AuctionQueryPriceListInterval = GetProperties()->GetProperty("Game", "AuctionQueryPriceListInterval");
	if (m_AuctionQueryPriceListInterval == 0)
	{
		m_AuctionQueryPriceListInterval = 1000;
	}

	m_AuctionPriceListCacheTime = GetProperties()->GetProperty("Game", "AuctionPriceListCacheTime");
	if (m_AuctionPriceListCacheTime == 0)
	{
		m_AuctionPriceListCacheTime = 10;
	}

	m_AuctionDueOffSaleTime = GetProperties()->GetProperty("Game", "AuctionDueOffSaleTime");
	if (m_AuctionDueOffSaleTime == 0)
	{
		m_AuctionDueOffSaleTime = 14;
	}

	m_AuctionSqlJointModify = !GetProperties()->GetProperty("Game", "CloseAuctionSqlJointModify");
	m_AuctionSqlInModify = GetProperties()->GetProperty("Game", "AuctionSqlInModify");
	m_AccountDataFlush = GetProperties()->GetProperty("Game", "AccountDataFlush");
	m_BattleFlag = GetProperties()->GetProperty("Game", "BattleFlag");
	m_DevilDropBug = !GetProperties()->GetProperty("Game", "CloseDevilDropBug");
	
	InfoLogStream << "max online player num(" << m_MaxOnlinePlayer << ") check dungeon(" << (UInt8)m_CheckRepeatStartDungeon << ") CheatPunish(" << m_CheatPunish << ") CheckMd5(" << (UInt8)m_CheckMd5 << ")." << LogStream::eos;
	InfoLogStream << "auction joint modify(" << (UInt8)m_AuctionSqlJointModify << ") in(" << (UInt8)m_AuctionSqlInModify << ") AccountDataFlush(" << m_AccountDataFlush << ")." << LogStream::eos;
	InfoLogStream << "battle flag(" << m_BattleFlag << ")." << LogStream::eos;


	{
		std::string needTransTableStr = GetProperties()->GetProperty("Record", "TransTables").GetString();
		std::vector<std::string> tables;
		Avalon::StringUtil::Split(needTransTableStr, tables, "|");
		CLRecordClient::Instance()->SetNeedTransTables(tables);

		InfoLogStream << "need trans tables: " + needTransTableStr << LogStream::eos;
	}

	m_AccountDelayedRefreshFlag = GetProperties()->GetProperty("Game", "AccountDelayedRefresh");
	m_NewDailyDrop = !GetProperties()->GetProperty("Game", "CloseNewDailyDrop");
	m_CheckChargeAccount = !GetProperties()->GetProperty("Game", "CloseCheckChargeAccount");
	m_AdjustOnOnline = !GetProperties()->GetProperty("Game", "CloseAdjustOnOnline");

	m_DropItemCountPath = GetProperties()->GetProperty("Game", "DropItemCountPath").GetString();
	if (m_DropItemCountPath == "")
	{
		m_DropItemCountPath = std::string(ROOT_PATH) + std::string("DropCountPath/");
	}
	InfoLogStream << "AdjustOnOnline(" << m_AdjustOnOnline << ")." << LogStream::eos;

	m_LogTeamCopyProtocol = GetProperties()->GetProperty("Game", "log_teamcopy_protocol");

	m_LoadOtherRoleInfo = !GetProperties()->GetProperty("Game", "close_load_other_role");
	m_CheckLevelWhenFlyupGift = !GetProperties()->GetProperty("Game", "close_check_flyup");
	InfoLogStream << "LoadOtherRoleInfo(" << m_LoadOtherRoleInfo << ") CheckLevelWhenFlyupGift(" << m_CheckLevelWhenFlyupGift << ")." << LogStream::eos;

	m_GuildBattleScoreRank = GetProperties()->GetProperty("Game", "guild_battle_score_rank");

	if (GetProperties()->GetProperty("Game", "team_auto_kick").GetString() != "")
	{
		m_TeamAutoKick = GetProperties()->GetProperty("Game", "team_auto_kick");
	}
	

	m_HireTurn = GetProperties()->GetProperty("Game", "hire_turn");

	m_IsUseAuctionNumCache = GetProperties()->GetProperty("Game", "use_auction_num_cache");

	m_AuctionNumCacheTime = GetProperties()->GetProperty("Game", "AuctionNumCacheTime");

	m_PrintRelationsUid = GetProperties()->GetProperty("Game", "PrintRelationsUid");
	{
		bool isObjectRecord = GetProperties()->GetProperty("Game", "IsObjectRecord");
		InfoLogStream << "Memory Object Record Switch:" << isObjectRecord << LogStream::eos;
		MEMORY_OBJECT_SWITCH(isObjectRecord);
	}

	// 拍卖行屏蔽道具
	{
		zjy_auction::AuctionMgr::Instance()->ClearShieldItem();
		std::string auctionShieldStr = GetProperties()->GetProperty("Game", "auction_shield").GetString();
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(auctionShieldStr, strs, "|");
		for (auto str : strs)
		{
			UInt32 itemId = Avalon::StringUtil::ConvertToValue<UInt32>(str);
			zjy_auction::AuctionMgr::Instance()->AddShieldItem(itemId);
		}
	}

	m_NewReloadTable = GetProperties()->GetProperty("Game", "new_reload_table");

	// 需要重载的表格
	{
		std::string reloadTables = GetProperties()->GetProperty("Game", "ReloadTables").GetString();
		LoadNeedReloadTables(reloadTables);
		InfoLogStream << "need reload table! " << reloadTables << LogStream::eos;
	}

	m_AuctionOpUseMem = GetProperties()->GetProperty("Game", "auction_op_use_mem");

	UInt8 tmpPriority = GetProperties()->GetProperty("Log", "priority");
	if (tmpPriority >= Avalon::LOG_DEBUG && tmpPriority < Avalon::LOG_MAX)
	{
		GetSysLog()->SetPriority((Avalon::LogPriority)tmpPriority);
	}

	m_HellMonsterCountPath = GetProperties()->GetProperty("Game", "HellMonsterCountPath").GetString();
	if (m_HellMonsterCountPath == "")
	{
		m_HellMonsterCountPath = std::string(ROOT_PATH) + std::string("HellMonsterCount/");
	}

	m_OpenDungeonRaceEndChecksum = GetProperties()->GetProperty("Verify", "OpenDungeonRaceEndChecksum");

	m_CloseMasterOffPubTask = GetProperties()->GetProperty("Game", "CloseMasterOffPubTask");
	
	m_CloseOffPresentGive = GetProperties()->GetProperty("Game", "CloseOffPresentGive");
}

bool WSApplication::WaitDataLoadFinished()
{
	CLRecordClient::Instance()->ProcessCallbacks();
	while(m_InitDataLoadCount != 0 && IsAlive())
	{
		Avalon::Thread::Sleep(100);

		CLRecordClient::Instance()->ProcessCallbacks();
	}
	WSNetwork::Instance()->OnTick(CURRENT_TIME);
	return m_InitDataLoadCount == 0 && IsAlive();
}

void WSApplication::ReloadDataTables()
{
	std::string configpath = CONFIG_PATH;
	configpath = configpath + "DataTables/";
	try
	{
		if (m_NewReloadTable)
		{
		CLConfigLoadMgr::ReloadDataTables(configpath);
	}
		else
		{
			DungeonFinishTimeDataEntryMgr::Instance()->ReloadData(configpath + "DungeonFinishTimeTable.txt");
			DungeonVerifyProbDataEntryMgr::Instance()->ReloadData(configpath + "DungeonVerifyProbTable.txt");
			SystemValueDataEntryMgr::Instance()->ReloadData(configpath + "SystemValueTable.txt");
			AccountShopItemDataEntryMgr::Instance()->ReloadData(configpath + "AccountShopItemTable.txt");
			ActivitySystemConfigDataEntryMgr::Instance()->ReloadData(configpath + "ActivitySystemConfig.txt");
			AuctionSystemValueDataEntryMgr::Instance()->ReloadData(configpath + "AuctionSysValueTable.txt");

			if (OpActivityDataEntryMgr::Instance()->ReloadData(configpath + "OpActivityTable.txt"))
			{
				WSActivityMgr::Instance()->ReloadOpActivityFromDataTable();
			}

			if (MallItemDataEntryMgr::Instance()->ReloadData(configpath + "MallItemTable.txt"))
			{
				MallMgr::Instance()->ReloadDataTable();
			}
			//放在读MallItemTable.txt之后
			if (LoginPushDataEntryMgr::Instance()->ReloadData(configpath + "PushExhibitionTable.txt"))
			{
				LoginPushMgr::Instance()->ReloadDataTable();
			}
			DungeonClearTimeDataEntryMgr::Instance()->ReloadData(configpath + "DungeonClearTimeTable.txt");

			AdventurePassSeasonDataEntryMgr::Instance()->ReloadData("AdventurePassSeasonTable.txt");
		}
	}
	catch (std::string str)
	{
		ErrorLogStream << str << LogStream::eos;
	}
}

void WSApplication::RegisterReloadDataTables()
{
	REGISTER_DATA_TABLE_RELOAD("DungeonFinishTimeTable.txt", &DungeonFinishTimeDataEntryMgr::ReloadData, DungeonFinishTimeDataEntryMgr::Instance());
	REGISTER_DATA_TABLE_RELOAD("DungeonVerifyProbTable.txt", &DungeonVerifyProbDataEntryMgr::ReloadData, DungeonVerifyProbDataEntryMgr::Instance());
	REGISTER_DATA_TABLE_RELOAD("SystemValueTable.txt", &SystemValueDataEntryMgr::ReloadData, SystemValueDataEntryMgr::Instance());
	REGISTER_DATA_TABLE_RELOAD("AccountShopItemTable.txt", &AccountShopItemDataEntryMgr::ReloadData, AccountShopItemDataEntryMgr::Instance());
	REGISTER_DATA_TABLE_RELOAD("ActivitySystemConfig.txt", &ActivitySystemConfigDataEntryMgr::ReloadData, ActivitySystemConfigDataEntryMgr::Instance());
	REGISTER_DATA_TABLE_RELOAD("AuctionSysValueTable.txt", &DungeonFinishTimeDataEntryMgr::ReloadData, DungeonFinishTimeDataEntryMgr::Instance());

	REGISTER_DATA_TABLE_RELOAD("OpActivityTable.txt", &OpActivityDataEntryMgr::ReloadData, OpActivityDataEntryMgr::Instance());
	REGISTER_DATA_TABLE_EXTRA_RELOAD("OpActivityTable.txt", &WSActivityMgr::ReloadOpActivityFromDataTable, WSActivityMgr::Instance());

	REGISTER_DATA_TABLE_RELOAD("MallItemTable.txt", &MallItemDataEntryMgr::ReloadData, MallItemDataEntryMgr::Instance());
	REGISTER_DATA_TABLE_EXTRA_RELOAD("MallItemTable.txt", &MallMgr::ReloadDataTable, MallMgr::Instance());

	REGISTER_DATA_TABLE_RELOAD("PushExhibitionTable.txt", &LoginPushDataEntryMgr::ReloadData, LoginPushDataEntryMgr::Instance());
	REGISTER_DATA_TABLE_EXTRA_RELOAD("PushExhibitionTable.txt", &LoginPushMgr::ReloadDataTable, LoginPushMgr::Instance());

	REGISTER_DATA_TABLE_RELOAD("DungeonClearTimeTable.txt", &DungeonClearTimeDataEntryMgr::ReloadData, DungeonClearTimeDataEntryMgr::Instance());
	REGISTER_DATA_TABLE_RELOAD("AdventurePassSeasonTable.txt", &AdventurePassSeasonDataEntryMgr::ReloadData, AdventurePassSeasonDataEntryMgr::Instance());
}

bool WSApplication::LoadDataTables()
{
	std::string configpath = CONFIG_PATH;
	configpath = configpath + "DataTables/";

    try
    {
		DATA_TABLE_LOAD("AdventurePassActivityTable.txt", AdventurePassActivityDataEntryMgr::Instance());
		DATA_TABLE_LOAD("AdventurePassBuyLevelTable.txt", AdventurePassBuyLevelDataEntryMgr::Instance());
		DATA_TABLE_LOAD("AdventurePassBuyRewardTable.txt", AdventurePassBuyRewardDataEntryMgr::Instance());
		DATA_TABLE_LOAD("AdventurePassRewardTable.txt", AdventurePassRewardDataEntryMgr::Instance());
		DATA_TABLE_LOAD("AdventurePassSeasonTable.txt", AdventurePassSeasonDataEntryMgr::Instance());
		DATA_TABLE_LOAD("HireTask.txt", HireTaskDataEntryMgr::Instance());

		DATA_TABLE_LOAD("MonopolyMapTable.txt", MonopolyMapDataEntryMgr::Instance());
		DATA_TABLE_LOAD("MonopolyRandomTable.txt", MonopolyRandomDataEntryMgr::Instance());
		DATA_TABLE_LOAD("MonopolyCardTable.txt", MonopolyCardDataEntryMgr::Instance());
		DATA_TABLE_LOAD("MonopolyCardRewardTable.txt", MonopolyCardRewardDataEntryMgr::Instance());
		
		if (!PlayerDataEntryMgr::Instance()->LoadData(configpath + "ExpTable.txt")) return false;
		if (!SystemValueDataEntryMgr::Instance()->LoadData(configpath + "SystemValueTable.txt")) return false;		//adder by huchenhhui 2016.10.04
		if (!ForbitWordParser::Instance()->Init(configpath + "forbitword.txt")) return false;
        if (!SceneDataEntryMgr::Instance()->LoadData(configpath + "CitySceneTable.txt")) return false;
        if (!MallItemDataEntryMgr::Instance()->LoadData(configpath + "MallItemTable.txt")) return false;
		if (!MallGiftPackDataEntryMgr::Instance()->LoadData(configpath + "MallGiftPackTable.txt")) return false;
		if (!MallPersonalTailorDataEntryMgr::Instance()->LoadData(configpath + "PersonalTailorTriggerTable.txt")) return false;
        //if(!ItemDataEntryMgr::Instance()->LoadData(configpath + "item.txt")) return false;
        if (!ItemDataEntryMgr::Instance()->LoadData(configpath + "ItemTable.txt")) return false;
        //if(!SysNotifyDataEntryMgr::Instance()->LoadData(configpath + "sysnotify.txt")) return false; // by huchenhui
        //if (!AnnouncementDataEntryMgr::Instance()->LoadData(configpath + "sysnotify.txt")) return false; // by huchenhui 

    
        if (!SysNotifyDataEntryMgr::Instance()->LoadData(configpath + "CommonTipsDesc.txt")) return false; //by huchenhui
		if (!SeasonRewardDataEntryMgr::Instance()->LoadData(configpath + "SeasonRewardTable.txt")) return false; //by huchenhui
		if (!AnnouncementDataEntryMgr::Instance()->LoadData(configpath + "AnnounceTable.txt")) return false;	//adder by huchenhui 2016.08.18
		
		// 关卡相关
		if (!MonsterDataEntryMgr::Instance()->LoadData(configpath + "UnitTable.txt")) return false;
		if (!DungeonAreaDataEntryMgr::Instance()->LoadData(configpath + "DungeonAreaTable.txt")) return false;
		if (!DungeonChestDataEntryMgr::Instance()->LoadData(configpath + "DungeonChestTable.txt")) return false;
		if (!DungeonDataEntryMgr::Instance()->LoadData(configpath + "DungeonTable.txt")) return false;
		if (!DropItemDataEntryMgr::Instance()->LoadData(configpath + "DropItemTable.txt")) return false;
		if (!DropItemDataEntryMgr::Instance()->LoadData(configpath + "DropItemTable2.txt")) return false;
		if (!MonsterPrefixDataEntryMgr::Instance()->LoadData(configpath + "MonsterPrefixTable.txt")) return false;
		if (!MonsterGroupDataEntryMgr::Instance()->LoadData(configpath + "MonsterGroupTable.txt")) return false;
		if (!MonsterAttributeDataEntryMgr::Instance()->LoadData(configpath + "MonsterAttributeTable.txt")) return false;
		if (!DungeonPrefixLimitDataMgr::Instance()->LoadData(configpath + "DungeonPrefixLimitTable.txt")) return false;
		if (!DungeonDestructDataEntryMgr::Instance()->LoadData(configpath + "DestrucTable.txt")) return false;
		if (!DungeonHellDataEntryMgr::Instance()->LoadData(configpath + "DungeonHellTable.txt")) return false;
		if (!RaceEndDropDataEntryMgr::Instance()->LoadData(configpath + "RaceEndDropTable.txt")) return false;
		if (!VipDataEntryMgr::Instance()->LoadData(configpath + "VipTable.txt")) return false;						//adder by huchenhhui 2016.10.04
		if (!VipPrivilegeDataEntryMgr::Instance()->LoadData(configpath + "VipPrivilegeTable.txt")) return false;
		if (!RareItemDataEntryMgr::Instance()->LoadData(configpath + "RareItemTable.txt")) return false;
		if (!SeasonDailyDataEntryMgr::Instance()->LoadData(configpath + "SeasonDailyTable.txt")) return false;		//adder by huchenhhui 2016.10.07
		if (!FuncNotifyDataEntryMgr::Instance()->LoadData(configpath + "FunctionUnLock.txt")) return false;
		if (!ShopDataEntryMgr::Instance()->LoadData(configpath + "ShopTable.txt")) return false;
		if (!ItemBuffDataEntryMgr::Instance()->LoadData(configpath + "ItemBuffTable.txt")) return false;
		if (!GuildDataEntryMgr::Instance()->LoadData(configpath + "GuildTable.txt")) return false;
		if (!GuildBattleScoreRankRewardDataEntryMgr::Instance()->LoadData(configpath + "GuildBattleScoreRankRewardTable.txt")) return false;
		if (!GuildBuildingDataMgr::Instance()->LoadData(configpath + "GuildBuildingTable.txt")) return false;
		if (!GuildSkillDataEntryMgr::Instance()->LoadData(configpath + "GuildSkillTable.txt")) return false;
		if (!GuildStatueEntryMgr::Instance()->LoadData(configpath + "GuildStatueTable.txt")) return false;
		if (!GuildTableDataEntryMgr::Instance()->LoadData(configpath + "GuildRoundtableTable.txt")) return false;	
		if (!GuildEmblemDataEntryMgr::Instance()->LoadData(configpath + "GuildEmblemTable.txt")) return false;
		if (!GuildBuildingPointDataEntryMgr::Instance()->LoadData(configpath + "GuildBuildingPointTable.txt")) return false;
		if (!GuildDungeonTimeDataEntryMgr::Instance()->LoadData(configpath + "GuildDungeonTimeTable.txt")) return false;
		if (!GuildDungeonTypeDataEntryMgr::Instance()->LoadData(configpath + "GuildDungeonTypeTable.txt")) return false;
		if (!GuildDungeonLvlDataEntryMgr::Instance()->LoadData(configpath + "GuildDungeonLvlTable.txt")) return false;
		if (!GuildDungeonRewardDataEntryMgr::Instance()->LoadData(configpath + "GuildDungeonRewardTable.txt")) return false;
		if (!GuildDungeonLotteryDataEntryMgr::Instance()->LoadData(configpath + "GuildDungeonLotteryTable.txt")) return false;	
		if (!GuildDungeonDamageCoeffDataEntryMgr::Instance()->LoadData(configpath + "GuildDungeonDamageCoeffTable.txt")) return false;	
		if (!RedPacketDataEntryMgr::Instance()->LoadData(configpath + "RedPacketTable.txt")) return false;
		if (!GuildTerritoryDataEntryMgr::Instance()->LoadData(configpath + "GuildTerritoryTable.txt")) return false;		//adder by huchenhhui 2017.1.10
		if (!GuildInspireDataEntryMgr::Instance()->LoadData(configpath + "GuildInspireTable.txt")) return false;			//adder by huchenhhui 2017.1.10
		if (!GuildBattleTimeDataEntryMgr::Instance()->LoadData(configpath + "GuildBattleTimeTable.txt")) return false;		//adder by huchenhhui 2017.1.10
		if (!GuildRewardDataEntryMgr::Instance()->LoadData(configpath + "GuildRewardTable.txt")) return false;				//adder by huchenhhui 2017.1.17
		if (!TeamDungeonDataEntryMgr::Instance()->LoadData(configpath + "TeamDungeonTable.txt")) return false;
		if (!ChargeMallDataEntryMgr::Instance()->LoadData(configpath + "ChargeMallTable.txt")) return false;
		if (!ChargePacketDataEntryMgr::Instance()->LoadData(configpath + "ChargePacketTable.txt")) return false;
		if (!TeamRewardDataEntryMgr::Instance()->LoadData(configpath + "TeamRewardTable.txt")) return false;
		if (!DungeonTimesDataEntryMgr::Instance()->LoadData(configpath + "DungeonTimesTable.txt")) return false;
		if (!LevelAdapterDataEntryMgr::Instance()->LoadData(configpath + "LevelAdapterTable.txt")) return false;
		if (!SeasonDataEntryMgr::Instance()->LoadData(configpath + "SeasonTable.txt")) return false;						//adder by huchenhui 2017.04.20
		if (!SeasonLevelDataEntryMgr::Instance()->LoadData(configpath + "SeasonLevelTable.txt")) return false;				//adder by huchenhui 2017.04.20
		if (!CurrencyCostDataEntryMgr::Instance()->LoadData(configpath + "CurrencyConfigureTable.txt")) return false;
		if (!DungeonGuaranteeDropDataEntryMgr::Instance()->LoadData(configpath + "DungeonGuaranteeDropTable.txt")) return false;
		if (!DungeonDailyDropDataEntryMgr::Instance()->LoadData(configpath + "DungeonDailyDropTable.txt")) return false;
		if (!DungeonFinishTimeDataEntryMgr::Instance()->LoadData(configpath + "DungeonFinishTimeTable.txt")) return false;
		if (!GuildBattleLotteryNumDataEntryMgr::Instance()->LoadData(configpath + "GuildBattleLotteryNumTable.txt")) return false;		//adder by huchenhui 2017.09.21
		if (!PremiumLeagueDataEntryMgr::Instance()->LoadData(configpath + "PremiumLeagueTimeTable.txt")) return false;
		if (!DungeonVerifyProbDataEntryMgr::Instance()->LoadData(configpath + "DungeonVerifyProbTable.txt")) return false;
		if (!DeathTowerAwardDataEntryMgr::Instance()->LoadData(configpath + "DeathTowerAwardTable.txt")) return false;
		if (!LoginPushDataEntryMgr::Instance()->LoadData(configpath + "PushExhibitionTable.txt")) return false;
		if (!ActivityMonsterDataEntryMgr::Instance()->LoadData(configpath + "ActivityMonsterTable.txt")) return false;
		if (!ActivitySuitAdditionDataEntryMgr::Instance()->LoadData(configpath + "ActivitySuitAdditionTable.txt")) return false;
		if (!DrawPrizeDataEntryMgr::Instance()->LoadData(configpath + "DrawPrizeTable.txt")) return false;
		if (!RewardPoolDataEntryMgr::Instance()->LoadData(configpath + "RewardPoolTable.txt")) return false;
		if (!ActiveTaskDataEntryMgr::Instance()->LoadData(configpath + "ActiveTable.txt")) return false;
		if (!OpActivityDataEntryMgr::Instance()->LoadData(configpath + "OpActivityTable.txt")) return false;
		if (!OpActivityTaskDataEntryMgr::Instance()->LoadData(configpath + "OpActivityTaskTable.txt")) return false;
		if (!EqualItemDataEntryMgr::Instance()->LoadData(configpath + "EqualItemTable.txt")) return false;
		if (!JarBonusDataMgr::Instance()->LoadData(configpath + "JarBonus.txt")) return false;
		if (!ActivityMonsterProbDataEntryMgr::Instance()->LoadData(configpath + "ActivityMonsterProbTable.txt")) return false;
		if (!MysticalMerchantDataEntryMgr::Instance()->LoadData(configpath + "MysticalMerchantTable.txt")) return false;
		if (!TaskDataEntryMgr::Instance()->LoadData(configpath + "MissionTable.txt")) return false;
		if (!EquipAttrDataMgr::Instance()->LoadData(configpath + "EquipAttrTable.txt")) return false;
		if (!EquipQLValueDataMgr::Instance()->LoadData(configpath + "EquipQLValueTable.txt")) return false;
		if (!MasterSysGiftMapDataEntryMgr::Instance()->LoadData(configpath + "MasterSysGiftTable.txt")) return false;
		if (!ScoreWarRewardDataEntryMgr::Instance()->LoadData(configpath + "ScoreWarRewardTable.txt")) return false;
		if (!ScoreWar2V2RewardDataEntryMgr::Instance()->LoadData(configpath + "ScoreWar2v2RewardTable.txt")) return false;
		if (!DigItemRandomDataEntryMgr::Instance()->LoadData(configpath + "DigItemRandomTable.txt")) return false;
		if (!DigItemPoolDataEntryMgr::Instance()->LoadData(configpath + "DigItemPoolTable.txt")) return false;
		if (!DigMapDataEntryMgr::Instance()->LoadData(configpath + "DigMapTable.txt")) return false;
		if (!ActivitySystemConfigDataEntryMgr::Instance()->LoadData(configpath + "ActivitySystemConfig.txt")) return false;
		if (!FriendWelfareAddDataEntryMgr::Instance()->LoadData(configpath + "FriendWelfareAddTable.txt")) return false;
		if (!ActivityTabDataEntryMgr::Instance()->LoadData(configpath + "ActivityTabTable.txt")) return false;
		if (!DropStrengthenControlDataEntryMgr::Instance()->LoadData(configpath + "DropStrengthenControlTable.txt")) return false;
		if (!AccountShopItemDataEntryMgr::Instance()->LoadData(configpath + "AccountShopItemTable.txt")) return false;
		if (!PreciousBeadDataEntryMgr::Instance()->LoadData(configpath + "BeadTable.txt")) return false;
		if (!AuctionSystemValueDataEntryMgr::Instance()->LoadData(configpath + "AuctionSysValueTable.txt")) return false;
		if (!StrTicketDataMgr::Instance()->LoadData(configpath + "StrengthenTicketTable.txt")) return false;
		if (!JobDataEntryMgr::Instance()->LoadData(configpath + "JobTable.txt")) return false;
		if (!AuctionPageStructDataEntryMgr::Instance()->LoadData(configpath + "AuctionNewFrameTable.txt")) return false;
		if (!BlackMarketItemDataEntryMgr::Instance()->LoadData(configpath + "BlackMarketItemTable.txt")) return false;
		if (!AccountCounterDataEntryMgr::Instance()->LoadData(configpath + "AccountCounterTable.txt")) return false;
		if (!AdventureTeamDataEntryMgr::Instance()->LoadData(configpath + "AdventureTeamTable.txt")) return false;
		if (!AdventureTeamGradeDataEntryMgr::Instance()->LoadData(configpath + "AdventureTeamGradeTable.txt")) return false;
		if (!RoleValueTotalScoreCalcDataEntryMgr::Instance()->LoadData(configpath + "RoleValueTotalScoreCalcTable.txt")) return false;
		if (!ExpeditionMapDataEntryMgr::Instance()->LoadData(configpath + "ExpeditionMapTable.txt")) return false;
		if (!ExpeditionMapRewardDataEntryMgr::Instance()->LoadData(configpath + "ExpeditionMapRewardTable.txt")) return false;
		if (!AuctionRecoveItemDataEntryMgr::Instance()->LoadData(configpath + "AuctionRecoveItemTable.txt")) return false;
		if (!NewTitleItemDataEntryMgr::Instance()->LoadData(configpath + "NewTitleTable.txt")) return false;
		if (!AuctionNoTreasAverPriceMinNumDataEntryMgr::Instance()->LoadData(configpath + "AuctionNoTreasAverPriceMinNum.txt")) return false;

		DATA_TABLE_LOAD("RechargePushTable.txt", RechargePushDataEntryMgr::Instance());

		DATA_TABLE_LOAD("AuctionMagiStrengAdditTable.txt", AuctionMagicStrengAdditDataEntryMgr::Instance());
		DATA_TABLE_LOAD("MagicCardTable.txt", MagicCardDataMgr::Instance());
		DATA_TABLE_LOAD("DropSmellControlTable.txt", DropSmellControlDataEntryMgr::Instance());
		DATA_TABLE_LOAD("DailyTodoTable.txt", DailyTodoItemDataEntryMgr::Instance());
		DATA_TABLE_LOAD("NpcTable.txt", NpcDataEntryMgr::Instance());
		DATA_TABLE_LOAD("EquipEnhanceAttributeTable.txt", EquipEnhanceAttributeDataEntryMgr::Instance());
		DATA_TABLE_LOAD("GuildProsperityTable.txt", GuildProsperityDataEntryMgr::Instance());
		DATA_TABLE_LOAD("DungeonClearTimeTable.txt", DungeonClearTimeDataEntryMgr::Instance());
		DATA_TABLE_LOAD("MallShopMultiITable.txt", MallShopMultiIDataEntryMgr::Instance());

		
		DATA_TABLE_LOAD("DropItemVipLimitTable.txt", DropItemVipLimitDataEntryMgr::Instance());

		DATA_TABLE_LOAD("AuctionMagicExpectUpTimesTable.txt", AuctionMagicExpectUpDataEntryMgr::Instance());

		DATA_TABLE_LOAD("AdventWeeklyTaskAddRewardTable.txt", AdventWeeklyTaskAddRewardDataEntryMgr::Instance());

		DATA_TABLE_LOAD("AssistEqStrengFouerDimAddTable.txt", AssistEqStrengFouerDimAddDataEntryMgr::Instance());

		DATA_TABLE_LOAD("EquipStrModIndAtkTable.txt", EquipIndpendatkStrengModDataEntryMgr::Instance());

		DATA_TABLE_LOAD("HonorRewardTable.txt", HonorRewardDataEntryMgr::Instance());

		DATA_TABLE_LOAD("GuildHonorTable.txt", GuildHonorDataEntryMgr::Instance());
	}
    catch (std::string str)
    {
        ErrorLogStream << str << LogStream::eos;
        return false;
    }

	return true;
}

void WSApplication::CheckTime(const Avalon::Time& now)
{
	if (IsOnChangedConditionReady())
	{
		Avalon::Date date(now);
		date.Hour(6);
		date.Min(0);
		date.Sec(0);
		UInt32 dayChangeTime = GameParamMgr::Instance()->GetValue("daych_time");
		if (now.Sec() >= dayChangeTime + DAY_TO_SEC)
		{

			dayChangeTime = UInt32(date.ToTime().Sec());
			OnDayChanged();
			GameParamMgr::Instance()->SetValue("daych_time", dayChangeTime);
			
		}

		UInt32 weekChangeTime = GameParamMgr::Instance()->GetValue("weekch_time");
		if (now.Sec() >= weekChangeTime + 7 * DAY_TO_SEC)
		{
			InfoLogStream << "before weekChangeTime is " << weekChangeTime << " now is " << now.Sec() << LogStream::eos;
			weekChangeTime = UInt32(date.ToTime().Sec()) - (date.WDay() - 1) * DAY_TO_SEC;
			InfoLogStream << "after weekChangeTime is " << weekChangeTime << LogStream::eos;
			OnWeekChanged();
			GameParamMgr::Instance()->SetValue("weekch_time", weekChangeTime);
		}

		UInt64 monthChangeTime = GameParamMgr::Instance()->GetValue("monthch_time");
		Avalon::Time resetTimeOffset(6 * Avalon::Time::SECS_OF_HOUR * Avalon::Time::MSECS_OF_SEC);
		Avalon::Date updateDate(monthChangeTime* Avalon::Time::MSECS_OF_SEC - resetTimeOffset);
		Avalon::Date nowTime(CURRENT_TIME.MSec() - resetTimeOffset);
		if (updateDate.Month() != nowTime.Month()
			|| updateDate.Year() != nowTime.Year())
		{
			OnMonthChanged();
			GameParamMgr::Instance()->SetValue("monthch_time", CURRENT_TIME.Sec());
		}
	}
}

void WSApplication::NotifyUnionZoneIDChanged()
{
	if (MERGED_SERVERS == "")
	{
		return;
	}
	CLProtocol::UnionWorldChgZoneID notify;
	notify.newZoneID = ZONE_ID;
	InfoLogStream << "merged server's (" << MERGED_SERVERS << ") new zone id is " << ZONE_ID << LogStream::eos;

	std::vector<std::string> strs;
	Avalon::StringUtil::Split(MERGED_SERVERS, strs, ",");
	for (const auto& elem : strs)
	{
		if (Avalon::StringUtil::ConvertToValue<UInt32>(elem) == 0)
		{
			continue;
		}
		notify.oldZoneIDs.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(elem));
	}
	Router::SendToUnion(USFT_ALL, notify);
}

UInt32 WSApplication::GetTeamCopyCheatForbidTime(UInt32 num)
{
	if (num == 0 || m_TeamCopyCheatForbidTime.empty()) return 0;
	return m_TeamCopyCheatForbidTime.size() >= num ? m_TeamCopyCheatForbidTime[num - 1] : 12;
}