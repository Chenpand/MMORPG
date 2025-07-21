#include "SSApplication.h"
#include <AvalonTimer.h>
#include <CLRecordClient.h>
#include <AvalonBase64.h>
#include <AvalonMemoryTracker.h>

#include <PerformanceCheck.h>

#include <CLDBLogger.h>
#include <CLSkillDataEntry.h>
#include <CLBuffDataEntry.h>
#include <CLPlayerDataEntry.h>
#include <CLTaskDataEntry.h>
#include <CLGameLoggerMgr.h>
#include <CLForbitWord.h>
#include <CLSceneDataEntry.h>
#include <CLActiveEventDataEntry.h>
#include <CLVipDataEntry.h>
#include <CLMallDataEntry.h>
#include <kingnet_analyzer.h>
#include <CLGameDefine.h>
#include <CLGiftBagDataEntry.h>
#include <CLFunctionDataEntry.h>
#include <CLStoryDataEntry.h>
#include <CLDungeonDataEntry.h>
#include <CLDropItemDataEntry.h>
#include <CLMonsterDataEntry.h>
#include <CLDungeonAreaDataEntry.h>
#include <CLDungeonChestDataEntry.h>
#include <CLShopItemDataEntry.h>
#include <CLSysNotifyDataEntry.h>
#include <CLMonsterPrefixDataEntry.h>
#include <CLJobDataEntry.h>
#include <CLMonsterGroupDataEntry.h>
#include <CLMonsterAttributeTable.h>
#include <CLDungeonPrefixLimitDataEntry.h>
#include <CLDungeonDestructTable.h>
#include <CLWarpStoneDataEntry.h>
#include <CLDungeonHellDataEntry.h>
#include <CLRetinueDataEntry.h>
#include <CLDeathTowerAwardDataEntry.h>
#include <CLRaceEndDropDataEntry.h>
#include <CLAnnouncementDataEntry.h>
#include <CLRewardAdapter.h>
#include <CLSeasonRewardDataEntry.h>
#include <CLSystemValueDataEntry.h>
#include <CLQuickBuyDataEntry.h>
#include <CLGuildBuildingDataEntry.h>
#include <CLEqualItemDataEntry.h>
#include <CLPkLevelDataEntry.h>
#include <CLTeamDungeonDataEntry.h>
#include <CLWudaoRewardDataEntry.h>
#include <CLDungeonTimesDataEntry.h>
#include <CLAuctionBoothDataEntry.h>
#include <CLCurrencyCostDataEntry.h>
#include <CLPetDataEntry.h>
#include <CLDungeonDailyDropDataEntry.h>
#include <CLMallPersonalTailorDataEntry.h>
#include <CLEquipSuitDataEntry.h>
#include <CLDrawPrizeDataEntry.h>
#include <CLEquipTransMapDataEntry.h>
#include <CLMallGiftPackDataEntry.h>
#include <CLGuildBattleDataEntry.h>
#include <CLEquipRecoveryRewardDataEntryBase.h>
#include <CLEquipRecoveryPriceDataEntryBase.h>
#include <CLEquipRecoScUpPrbWeightDataEntryBase.h>
#include <CLEquipRecoScUpConsRtiDataEntryBase.h>
#include <CLMoneyManageDataEntry.h>
#include <CLCityMonsterDataEntry.h>
#include <CLNpcDataEntry.h>
#include <CLChargeMallDataEntry.h>
#include <CLScoreWarDataEntry.h>
#include <CLLevelAdapterDataEntry.h>
#include <CLMasterSysGiftDataEntry.h>
#include <CLShopMainFrameDataEntry.h>
#include <CLSkillRecommendDataEntry.h>
#include <CLBuffRandomDataEntry.h>
#include <CLStrengthenTicketSynthesisDataEntry.h>
#include <CLFashionMergeChangeActiveDataEntry.h>
#include <CLStrengthenTicketFuseDataEntry.h>
#include <CLStrenTicketFuseMaterialDataEntry.h>
#include <CLStrenTicketFusePramMappingDataEntry.h>
#include <CLStandardNormalDistributionDataEntry.h>
#include <CLEquipInitAttriOnMakeDataEntry.h>
#include <CLAuctionSysValueDataEntry.h>
#include <CLEquieUpdateDataEntry.h>
#include <CLEquipBaseScoreDataEntry.h>
#include <CLEquipStrenLvScoreCoeDataEntry.h>
#include <CLHeadFrameCompensateDataEntry.h>
#include <CLNewTitleDataEntry.h>
#include <CLWeekSignDataEntry.h>
#include <CLWeekSignSumDataEntry.h>
#include <CLEqualPvPEuqipDataEntry.h>
#include <CLMonthSignAwardDataEntry.h>
#include <CLDropSmellControlDataEntry.h>
#include <CLEquipBaseScoreModDataEntry.h>
#include <CLEqSuitScoreDataEntry.h>
#include <CLUltimateChallengeDungeonDataEntry.h>
#include <CLUltimateChallengeBuffDataEntry.h>
#include <CLMallShopMultiIDataEntry.h>
#include <CLEquipInscriptionHoleDataEntry.h>
#include <CLFashionDecomposeDataEntry.h>
#include <CLInscriptionDataEntry.h>
#include <CLInscriptionDropItemDataEntry.h>
#include <CLInscriptionExtractDataEntry.h>
#include <CLInscriptionHoleSetDataEntry.h>
#include <CLInscriptionSynthesisDataEntry.h>
#include <CLDropItemVipLimitDataEntry.h>
#include <CLRechargePushDataEntry.h>
#include <CLMallFashionLimitedSuitDataEntry.h>
#include <CLChijiRewardDataEntry.h>
#include <CL2V2ScoreWarDataEntry.h>
#include <CLAntiAddictTimeDataEntry.h>
#include <CLAntiAddictHolidayDataEntry.h>
#include <CLBeadConvertCostDataEntry.h>
#include <CLBeadConvertDropDataEntry.h>
#include <CLHonorLevelDataEntry.h>
#include <CLHonorRankExpDataEntry.h>
#include <CLHonorRewardDataEntry.h>
#include <CLEquipChangeDataEntry.h>
#include <CLEquipChgConsDataEntry.h>
#include <CLWeaponBuildDataEntry.h>
#include <CLPlantOpActTreesDataEntry.h>
#include <CLLostDungeonDataEntry.h>
#include <CLLostDungeonTreasureChestDataEntry.h>
#include <CLOverlondDeviceValueDataEntry.h>
#include <CLGoldConsignmentValueDataEntry.h>
#include <CLNetworkPerformance.h>
#include <CLUltimateChallengeRewardDataEntry.h>
#include <CLItemPerfectGradeDataEntry.h>

#include <AsyncMsgCallbackMgr.h>
#include "SSNetwork.h"
#include "ScriptMgr.h"
#include "SceneMgr.h"
#include "PlayerMgr.h"
#include "SortListMgr.h"
#include "SSRouter.h"
#include "ItemReasonMgr.h"
#include "CondActivateEvent.h"
#include "SSConsoleAgent.h"
#include "DropItemMgr.h"
#include "InscriptionDropItemMgr.h"
#include "ActivityMgr.h"
#include "SSDungeonConfigMgr.h"
#include "ScoreWarMgr.h"
#include "StrengthenTicketPlanMgr.h"
#include "AbnormalTransactionMgr.h"
#include "EqualPvPMgr.h"
#include "ActiveRecordMgr.h"
#include "CurrencyFrozenMgr.h"
#include "TCSMgr.h"
#include "ScoreWar2V2Mgr.h"
#include "SceneChampionMgr.h"
#include "HonorMgr.h"
#include "CreditPointRecordMgr.h"
#include "ItemThingMgr.h"

SSApplication::SSApplication()
{
	m_TickTime = 0;
	m_ReloadConfig = false;
	m_SceneMgrTickIntervalMSec = 0;
	m_NewOfflineCharge = false;
	m_IsHandleAbnormalTrans = true;
	m_ReloadTable = false;
	m_IsUseFreezeAbnormalNew = false;

	m_TickTimer200ms.SetInterval(200);
	m_TickTimer500ms.SetInterval(500);
	m_TickTimer1000ms.SetInterval(1000);
	m_TickTimerMin.SetInterval(60 * 1000);
	m_TickTimerMonth.SetLastTime(getNextMonthLogicBeginTime(CURRENT_TIME.MSec()) * 1000);

	m_OpenRestartDungeon = true;
	m_IsCheckStartDungeonFrequence = false;
	m_MaxStartDungeonNumInSecond = 0;
	m_CheckJarType = true;
	m_NewShopRefesh = true;
	m_NewChatCheck = true;
	m_OpenDetailLog = false;
	m_CheckAntiAddict = false;
	m_TourLoginInterval = 360;
	m_TourOnlineTime = 1;
	m_ClientLogMaxLength = 40;
	m_IndpentAtlPvpRatio = 1000;
	m_CheckStrengthenResult = true;
	m_OpenPerf = false;
	m_NewHeartbeat = false;
	m_OpenFastSellBat = false;
	m_OpenPlayerOnOnline = false;
	m_NewReloadTable = false;
}

SSApplication::~SSApplication()
{
}

bool SSApplication::OnInit()
{
	if(!CLApplication::OnInit()) return false;

	std::string configpath = CONFIG_PATH;
	std::string rootpath = ROOT_PATH;
	if(!GetProperties()->LoadFromFile(rootpath + "Game.cfg"))
	{
		ErrorLogStream << "load game config failed!" << LogStream::eos;
		return false;
	}
	NEUTRAL_ZONE_ID = GetProperties()->GetProperty("Game", "neutral_zone_id");
	PLATFORM = GetProperties()->GetProperty("Platform", "platform").GetString();
	GAME_VERSION = GetProperties()->GetProperty("Server", "version").GetString();
	
	LoadDynamicConfig();

	if(!LoadDataTables())
	{
		ErrorLogStream << "load tables failed!" << LogStream::eos;
		return false;
	}

	if(!SSNetwork::Instance()->Startup(GetProperties(),GetNetAddress()))
	{
		ErrorLogStream << "network startup failed!" << LogStream::eos;
		return false;
	}
	
	RegisterReloadDataTables();

	if(!ScriptMgr::Instance()->Init())
	{
		ErrorLogStream << "script system init failed!" << LogStream::eos;
		return false;
	}

	if(SERVER_TYPE != ST_WSCENE)
	{
		std::vector<Avalon::NetConnectionPtr> connVec;
		SSNetwork::Instance()->FindConnectionByType(ST_RECORD, connVec);

		if(connVec.empty() || !CLRecordClient::Instance()->Init(this, connVec))
		{
			ErrorLogStream << "record client init failed!" << LogStream::eos;
			return false;
		}
	}

	SSConsoleAgent::Instance()->Init();
	SortListMgr::Instance();

	Avalon::TimerReactor::Instance();

	if(!SceneMgr::Instance()->Init(GetProperties()))
	{
		ErrorLogStream << "scenemgr init failed!" << LogStream::eos;
		return false;
	}

	PlayerMgr::Instance()->Init();

	if(!GameLoggerMgr::Instance()->Init(GAME_LOGGER_CFG, IsDebugMode(), SSNetwork::Instance()->FindConnectionByType(ST_ADMIN)))
	{
		ErrorLogStream << "game loggers init failed!" << LogStream::eos;
		return false;
	}

	if(SERVER_TYPE != ST_WSCENE)
	{
		if(UL_FAIL == _analyzer.Init() || NULL == _analyzer.GetInstance(UDPLOG_SCENE_UID))
		{
			ErrorLogStream << "udplog init failed!" << LogStream::eos;
			return false;
		}
	}

    if (!DropItemMgr::Instance()->Init())
    {
        ErrorLogStream << "drop item manager init failed!" << LogStream::eos;
        return false;
    }

	if (!InscriptionDropItemMgr::Instance()->Init())
	{
		ErrorLogStream << "inscription drop item manager init failed!" << LogStream::eos;
		return false;
	}

	if (!StrengthenTicketPlanMgr::Instance()->Init())
	{
		ErrorLogStream << "synthesis probability manager init failed!" << LogStream::eos;
		return false;
	}

	if (!RetinueMgr::Init())
	{
		ErrorLogStream << "retinue manager init failed!" << LogStream::eos;
		return false;
	}

	if (!LegendTaskMgr::Init())
	{
		ErrorLogStream << "legend task manager init failed!" << LogStream::eos;
		return false;
	}

	if (!ScoreWarMgr::Instance()->Init())
	{
		ErrorLogStream << "score war manager init failed!" << LogStream::eos;
		return false;
	}


	if (!ScoreWar2V2Mgr::Instance()->Init())
	{
		ErrorLogStream << "ScoreWar2V2Mgr manager init failed!" << LogStream::eos;
		return false;
	}

	SSActivityDungeonMgr::Instance()->Init();

	if (!EqualPvPMgr::Instance()->Init())
	{
		ErrorLogStream << "EqualPvPMgr manager init failed!" << LogStream::eos;
		return false;
	}


	ActivateCondCheckEvent::Instance()->Init();

	AbnormalTransactionMgr::Init();

	OpActivityRecordMgr::Instance()->Init();

	CurrencyFrozenMgr::Instance()->Init();

	TCSMgr::Instance()->Init();

	HonorMgr::Instance()->Init();

	SceneChampionMgr::Instance()->Init();

	CreditPointRecordMgr::Instance()->Init();

	m_TickTime = Avalon::Time::CurrentTime().MSec();

	UInt32 ratio = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_INDPEND_ATK_PVP_RATIO);
	ratio = (ratio == 0) ? 1000 : ratio;
	SetIndpentPvpRatio(ratio);

	// 初始化转盘稀有控制
	RewardPoolDataEntryMgr::Instance()->Init(OpActivityRecordMgr::Instance());

	return true;
}

void SSApplication::OnTick(const Avalon::Time &now)
{
	CLApplication::OnTick(now);

	UInt64 offsetTime = now.MSec() - m_TickTime;
	if(offsetTime > 1000)
	{
		ErrorLogStream << "Too long tick time:" << offsetTime << " msecs!" << LogStream::eos;
        SHOW_PERF();
		PlayerMgr::Instance()->ShowPacketInOneTick();
	}
	m_TickTime = now.MSec();

    PERF_SEG_BEGIN(SceneMgr);
	SceneMgr::Instance()->OnTick(now);
    PERF_SEG_END();

    PERF_SEG_BEGIN(SSNetwork);
	SSNetwork::Instance()->OnTick(now);
    PERF_SEG_END();

    PERF_SEG_BEGIN(TimerReactor);
	Avalon::TimerReactor::Instance()->HandleEvent(now);
    PERF_SEG_END();

    PERF_SEG_BEGIN(CLRecordClient);
	CLRecordClient::Instance()->ProcessCallbacks();
    PERF_SEG_END();

    PERF_SEG_BEGIN(SceneObjectDestroyObjects);
	SceneObject::DestroyObjects();
    PERF_SEG_END();

	if (m_TickTimer200ms.IsTimeout(now))
	{
		PERF_SEG_BEGIN(PlayerMgr);
		PlayerMgr::Instance()->OnTick(now);
		PERF_SEG_END();

		PERF_SEG_BEGIN(AsyncMsgCallbackMgr);
		AsyncMsgCallbackMgr::Instance()->Tick(now);
		PERF_SEG_END();
	}

	if (m_TickTimer500ms.IsTimeout(now))
	{
		PERF_SEG_BEGIN(ScoreWarMgr);
		ScoreWarMgr::Instance()->OnTick(now);
		PERF_SEG_END();

		PERF_SEG_BEGIN(ScoreWar2V2Mgr);
		ScoreWar2V2Mgr::Instance()->OnTick(now);
		PERF_SEG_END();

		PERF_SEG_BEGIN(HonorMgr);
		HonorMgr::Instance()->OnTick(now);
		PERF_SEG_END();
	}

	if (m_TickTimer1000ms.IsTimeout(now))
	{
		AbnormalTransactionMgr::OnTick(now);
	}

	if (m_TickTimerMin.IsTimeout(now))
	{
		PERF_SEG_BEGIN(ItemThingMgr);
		ItemThingMgr::Instance()->OnTick(now);
		PERF_SEG_END();

		REPORT_MEMORY_OBJECT(gSysLogger);
	}

	if (m_TickTimerMonth.IsTimeout(now))
	{
		m_TickTimerMonth.SetLastTime(getNextMonthLogicBeginTime(CURRENT_TIME.MSec()) * 1000);
		PlayerMgr::Instance()->OnChangedMonth();
	}

	if (m_ReloadConfig)
	{
		m_ReloadConfig = false;

		if (!GetProperties()->LoadFromFile(std::string(ROOT_PATH) + "Game.cfg"))
		{
			ErrorLogStream << "load Game.cfg failed!" << LogStream::eos;
			return;
		}

		if (!GetProperties()->LoadFromFile("SceneServer.cfg"))
		{
			ErrorLogStream << "load SceneServer.cfg failed!" << LogStream::eos;
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
	
void SSApplication::OnQuit()
{
	_analyzer.DestoryInstance(UDPLOG_SCENE_UID);

	GameLoggerMgr::Instance()->Final();

	PlayerMgr::Instance()->Final();
	SceneMgr::Instance()->Final();

	SSConsoleAgent::Instance()->Final();

	CLRecordClient::Instance()->Final();
	SSNetwork::Instance()->Shutdown(100);

	ScriptMgr::Instance()->Final();
	SceneObject::DestroyObjects();
	ForbitWordParser::Instance()->Final();
	CreditPointRecordMgr::Instance()->Final();

	CLApplication::OnQuit();
}

void SSApplication::OnSignal(int signal)
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
		m_ReloadTable = true;
		break;

	default:
	{
		ServerApplication::OnSignal(signal);
		break;
	}
	}
}

bool SSApplication::LoadDataTables()
{
	std::string configpath = CONFIG_PATH;
	configpath = configpath + "DataTables/";

    try
    {
        if (!PlayerDataEntryMgr::Instance()->LoadData(configpath + "ExpTable.txt")) return false;
        if (!SkillDataEntryMgr::Instance()->LoadData(configpath + "SkillTable.txt")) return false;
		if (!SkillRecommendDataEntryMgr::Instance()->LoadData(configpath + "SkillRecommendTable.txt")) return false;
        if (!BuffDataEntryMgr::Instance()->LoadData(configpath + "BuffTable.txt")) return false;
		if (!BuffInfoEntryMgr::Instance()->LoadData(configpath + "BuffInfoTable.txt")) return false;
        if (!ItemDataEntryMgr::Instance()->LoadData(configpath + "ItemTable.txt")) return false;
        if (!RandBaseDataEntryMgr::Instance()->LoadData(configpath + "RandBaseTable.txt")) return false;
        if (!RandAttrDataEntryMgr::Instance()->LoadData(configpath + "RandPropTable.txt")) return false;
        if (!RandAttrNumDataEntryMgr::Instance()->LoadData(configpath + "RandPropNumTable.txt")) return false;
        if (!RandAttrValueDataEntryMgr::Instance()->LoadData(configpath + "RandPropValueTable.txt")) return false;

        if (!TaskDataEntryMgr::Instance()->LoadData(configpath + "MissionTable.txt")) return false;
		if (!RewardAdapterDataEntryMgr::Instance()->LoadData(configpath + "RewardAdapterTable.txt")) return false;
        if (!ForbitWordParser::Instance()->Init(configpath + "forbitword.txt")) return false;
		if (!ForbitWordParser::Instance()->InitValidCoolName(configpath + "coolname.txt")) return false;
        if (!SceneDataEntryMgr::Instance()->LoadData(configpath + "CitySceneTable.txt")) return false;
		if (!MallPersonalTailorDataEntryMgr::Instance()->LoadData(configpath + "PersonalTailorTriggerTable.txt")) return false;
        if (!FuncNotifyDataEntryMgr::Instance()->LoadData(configpath + "FunctionUnLock.txt")) return false;		//by huchenhui 2016.08.26
	
        if (!DungeonDataEntryMgr::Instance()->LoadData(configpath + "DungeonTable.txt")) return false;
		if (!DungeonTimesDataEntryMgr::Instance()->LoadData(configpath + "DungeonTimesTable.txt")) return false;
        if (!DropItemDataEntryMgr::Instance()->LoadData(configpath + "DropItemTable.txt")) return false;
		if (!DropItemDataEntryMgr::Instance()->LoadData(configpath + "DropItemTable2.txt")) return false;
        if (!MonsterDataEntryMgr::Instance()->LoadData(configpath + "UnitTable.txt")) return false;
        if (!DungeonAreaDataEntryMgr::Instance()->LoadData(configpath + "DungeonAreaTable.txt")) return false;
        if (!DungeonChestDataEntryMgr::Instance()->LoadData(configpath + "DungeonChestTable.txt")) return false;
	
		if (!DeComposeDataEntryMgr::Instance()->LoadData(configpath + "DecomposeTable.txt")) return false;
		if (!EquipStrengthenDataMgr::Instance()->LoadData(configpath + "EquipStrengthenTable.txt")) return false;
		if (!EquipStrRateDataMgr::Instance()->LoadData(configpath + "EquipStrRateTable.txt")) return false;
		if (!ShopItemDataEntryMgr::Instance()->LoadData(configpath + "ShopItemTable.txt")) return false;
		if (!ShopDataEntryMgr::Instance()->LoadData(configpath + "ShopTable.txt")) return false;
		if (!SysNotifyDataEntryMgr::Instance()->LoadData(configpath + "CommonTipsDesc.txt")) return false; //by huchenhui
        if (!MonsterPrefixDataEntryMgr::Instance()->LoadData(configpath + "MonsterPrefixTable.txt")) return false;
		if (!JobDataEntryMgr::Instance()->LoadData(configpath + "JobTable.txt")) return false;	//by huchenhui
        if (!MonsterGroupDataEntryMgr::Instance()->LoadData(configpath + "MonsterGroupTable.txt")) return false;
        if (!MonsterAttributeDataEntryMgr::Instance()->LoadData(configpath + "MonsterAttributeTable.txt")) return false;
        if (!DungeonPrefixLimitDataMgr::Instance()->LoadData(configpath + "DungeonPrefixLimitTable.txt")) return false;
        if (!DungeonDestructDataEntryMgr::Instance()->LoadData(configpath + "DestrucTable.txt")) return false;
        if (!DungeonHellDataEntryMgr::Instance()->LoadData(configpath + "DungeonHellTable.txt")) return false;
		if (!DeathTowerAwardDataEntryMgr::Instance()->LoadData(configpath + "DeathTowerAwardTable.txt")) return false;
		if (!MagicCardDataMgr::Instance()->LoadData(configpath + "MagicCardTable.txt")) return false;
		if (!PreciousBeadDataEntryMgr::Instance()->LoadData(configpath + "BeadTable.txt")) return false;
		if (!RaceEndDropDataEntryMgr::Instance()->LoadData(configpath + "RaceEndDropTable.txt")) return false;
		if (!AnnouncementDataEntryMgr::Instance()->LoadData(configpath + "AnnounceTable.txt")) return false;	//adder by huchenhui 2016.08.18
		if (!EquipAttrDataMgr::Instance()->LoadData(configpath + "EquipAttrTable.txt")) return false;
		if (!ActiveTaskDataEntryMgr::Instance()->LoadData(configpath + "ActiveTable.txt")) return false;
		if (!SeasonDailyDataEntryMgr::Instance()->LoadData(configpath + "SeasonDailyTable.txt")) return false;		//adder by huchenhhui 2016.09.30
		if (!VipDataEntryMgr::Instance()->LoadData(configpath + "VipTable.txt")) return false;						//adder by huchenhhui 2016.09.30
		if (!VipPrivilegeDataEntryMgr::Instance()->LoadData(configpath + "VipPrivilegeTable.txt")) return false;
		if (!SystemValueDataEntryMgr::Instance()->LoadData(configpath + "SystemValueTable.txt")) return false;		//adder by huchenhhui 2016.10.03
		if (!QuickBuyDataEntryMgr::Instance()->LoadData(configpath + "QuickBuyTable.txt")) return false;
		if (!JarBonusDataMgr::Instance()->LoadData(configpath + "JarBonus.txt")) return false;
		if (!PkLevelDataEntryMgr::Instance()->LoadData(configpath + "PkLevelTable.txt")) return false;
	
		if (!EquipStrModDataMgr::Instance()->LoadData(configpath + "EquipStrModTable.txt")) return false;
		if (!BuffDrugDataMgr::Instance()->LoadData(configpath + "BuffDrugConfigTable.txt")) return false;
		if (!JarItemPoolMgr::Instance()->LoadData(configpath + "JarItemPool.txt")) return false;
		if (!GuildBuildingDataMgr::Instance()->LoadData(configpath + "GuildBuildingTable.txt")) return false;
		if (!GuildTerritoryDataEntryMgr::Instance()->LoadData(configpath + "GuildTerritoryTable.txt")) return false;		//adder by huchenhhui 2018.04.24
		if (!DungeonMakeUpDataMgr::Instance()->LoadData(configpath + "DungeonMakeUp.txt")) return false;
		if (!FatigueMakeUpDataMgr::Instance()->LoadData(configpath + "FatigueMakeUp.txt")) return false;
		if (!EqualItemDataEntryMgr::Instance()->LoadData(configpath + "EqualItemTable.txt")) return false;
		if (!StrTicketDataMgr::Instance()->LoadData(configpath + "StrengthenTicketTable.txt")) return false;
	
		if (!TeamDungeonDataEntryMgr::Instance()->LoadData(configpath + "TeamDungeonTable.txt")) return false;
		if (!DailyTaskScoreRewardDataEntryMgr::Instance()->LoadData(configpath + "MissionScoreTable.txt")) return false;	//adder by huchenhui 2017.02.04
		if (!WudaoRewardDataEntryMgr::Instance()->LoadData(configpath + "BudoAwardTable.txt")) return false;
		if (!FashionComposeMgr::Instance()->LoadData(configpath + "FashionComposeTable.txt")) return false;
		if (!FashionComposeSkyMgr::Instance()->LoadData(configpath + "FashionComposeSkyTable.txt")) return false;
		if (!EquipMakeDataMgr::Instance()->LoadData(configpath + "EquipForgeTable.txt")) return false;
		if (!SeasonLevelDataEntryMgr::Instance()->LoadData(configpath + "SeasonLevelTable.txt")) return false;				//adder by huchenhui 2017.04.21
		if (!EquipValueSceorDataMgr::Instance()->LoadData(configpath + "EquipScoreTable.txt")) return false;
		if (!EquipQLRandDataMgr::Instance()->LoadData(configpath + "EquipQLRandTable.txt")) return false;
		if (!EquipQLValueDataMgr::Instance()->LoadData(configpath + "EquipQLValueTable.txt")) return false;
		
		if (!AuctionBoothDataEntryMgr::Instance()->LoadData(configpath + "AuctionBoothTable.txt")) return false;
		if (!CurrencyCostDataEntryMgr::Instance()->LoadData(configpath + "CurrencyConfigureTable.txt")) return false;
		if (!FashionAttrDataMgr::Instance()->LoadData(configpath + "FashionAttributesConfigTable.txt")) return false;
		if (!EquipStrColorDataMgr::Instance()->LoadData(configpath + "EquipStrColorRatioTable.txt")) return false;
		if (!GiftPackDataMgr::Instance()->LoadData(configpath + "GiftPackTable.txt")) return false;
		if (!GiftItemDataMgr::Instance()->LoadData(configpath + "GiftTable.txt")) return false;
		if (!FatigueMakeUpPriceMgr::Instance()->LoadData(configpath + "FatigueMakeUpPrice.txt")) return false;
		
		if (!PetDataEntryMgr::Instance()->LoadData(configpath + "PetTable.txt")) return false;								//add by huchenhui 2017.07.31
		if (!PetLevelDataEntryMgr::Instance()->LoadData(configpath + "PetLevelTable.txt")) return false;					//add by huchenhui 2017.07.31
		if (!PetFeedDataEntryMgr::Instance()->LoadData(configpath + "PetFeedTable.txt")) return false;						//add by huchenhui 2017.07.31
		if (!DungeonDailyDropDataEntryMgr::Instance()->LoadData(configpath + "DungeonDailyDropTable.txt")) return false;	//add by huchenhui 2017.08.26
		if (!FashionComposeRateMgr::Instance()->LoadData(configpath + "FashionComposeRateTable.txt")) return false;
		if (!FashionCompPartRateMgr::Instance()->LoadData(configpath + "FashionComposePartRateTable.txt")) return false;
		if (!ActivityTemplateDataEntryMgr::Instance()->LoadData(configpath + "ActiveMainTable.txt")) return false;
		if (!EquipSuitDataEntryMgr::Instance()->LoadData(configpath + "EquipSuitTable.txt")) return false;
		if (!AchievementLevelDataEntryMgr::Instance()->LoadData(configpath + "AchievementLevelInfoTable.txt")) return false;	//add by huchenhui 2018.03.05
		if (!OpActivityDataEntryMgr::Instance()->LoadData(configpath + "OpActivityTable.txt")) return false;
		if (!OpActivityTaskDataEntryMgr::Instance()->LoadData(configpath + "OpActivityTaskTable.txt")) return false;
		if (!DrawPrizeDataEntryMgr::Instance()->LoadData(configpath + "DrawPrizeTable.txt")) return false;
		if (!RewardPoolDataEntryMgr::Instance()->LoadData(configpath + "RewardPoolTable.txt")) return false;
		if (!MallGiftPackDataEntryMgr::Instance()->LoadData(configpath + "MallGiftPackTable.txt")) return false;
		if (!EquipRecoveryRewardDataEntryMgr::Instance()->LoadData(configpath + "EquipRecoveryRewardTable.txt")) return false;
		if (!EqRecoveryPriceDataEntryMgr::Instance()->LoadData(configpath + "EquipRecoveryPriceTable.txt")) return false;
		if (!EquipRecoScUpConsRtiDataEntryMgr::Instance()->LoadData(configpath + "EquipRecoScUpConsRtiTable.txt")) return false;
		if (!EquipRecoScUpPrbWeightDataEntryMgr::Instance()->LoadData(configpath + "EquipRecoScUpPrbWeightTable.txt")) return false;
		if (!EquipTransMapDataEntryMgr::Instance()->LoadData(configpath + "EquipTransMapTable.txt")) return false;
		if (!MoneyManageDataEntryMgr::Instance()->LoadData(configpath + "MoneyManageTable.txt")) return false;
		if (!GuildBattleTimeDataEntryMgr::Instance()->LoadData(configpath + "GuildBattleTimeTable.txt")) return false;		//adder by huchenhhui 2017.1.10
		if (!MysticalMerchantDataEntryMgr::Instance()->LoadData(configpath + "MysticalMerchantTable.txt")) return false;
		if (!CityMonsterGenerateDataEntryMgr::Instance()->LoadData(configpath + "CityMonsterGenerate.txt")) return false;
		if (!CityMonsterPositionDataEntryMgr::Instance()->LoadData(configpath + "CityMonsterPosition.txt")) return false;
		if (!NpcDataEntryMgr::Instance()->LoadData(configpath + "NpcTable.txt")) return false;
		if (!ChargeMallDataEntryMgr::Instance()->LoadData(configpath + "ChargeMallTable.txt")) return false;
		if (!LevelAdapterDataEntryMgr::Instance()->LoadData(configpath + "LevelAdapterTable.txt")) return false;
		if (!MasterSysGiftMapDataEntryMgr::Instance()->LoadData(configpath + "MasterSysGiftTable.txt")) return false;
		if (!ScoreWarRewardDataEntryMgr::Instance()->LoadData(configpath + "ScoreWarRewardTable.txt")) return false;
		if (!ScoreWar2V2RewardDataEntryMgr::Instance()->LoadData(configpath + "ScoreWar2v2RewardTable.txt")) return false;
		if (!ActivitySystemConfigDataEntryMgr::Instance()->LoadData(configpath + "ActivitySystemConfig.txt")) return false;
		if (!ShopMainframeDataEntryMgr::Instance()->LoadData(configpath + "ShopMainFrameTable.txt")) return false;
		if (!BuffRandomDataEntryMgr::Instance()->LoadData(configpath + "BuffRandomTable.txt")) return false;
		if (!FashionMergeChangeActiveDataEntryMgr::Instance()->LoadData(configpath + "ChangeFashionActiveMergeTable.txt")) return false;
		if (!StrengthenTicketSynthesisDataEntryMgr::Instance()->LoadData(configpath + "StrengthenTicketSynthesisTable.txt")) return false;
		if (!StrengthenTicketFuseDataEntryMgr::Instance()->LoadData(configpath + "StrengthenTicketFuseTable.txt")) return false;
		if (!StrenTicketFuseMaterialDataEntryMgr::Instance()->LoadData(configpath + "StrenTicketFuseMaterialTable.txt")) return false;
		if (!StrenTicketFusePramMappingDataEntryMgr::Instance()->LoadData(configpath + "StrenTicketFusePramMappingTable.txt")) return false;
		if (!StandardNormalDistributionDataEntryMgr::Instance()->LoadData(configpath + "StandardNormalDistributionTable.txt")) return false;
		if (!PreciousBeadExtirpeDataEntryMgr::Instance()->LoadData(configpath + "RemovejewelsTable.txt")) return false;
		if (!PreciousBeadReplaceDataEntryMgr::Instance()->LoadData(configpath + "ReplacejewelsTable.txt")) return false;
		if (!PrecBeadAddBuffRandomDataEntryMgr::Instance()->LoadData(configpath + "BeadRandomBuff.txt")) return false;
		if (!EquipInitAttriOnMakeDataEntryMgr::Instance()->LoadData(configpath + "EquipInitialAttribute.txt")) return false;
		if (!AuctionSystemValueDataEntryMgr::Instance()->LoadData(configpath + "AuctionSysValueTable.txt")) return false;
		if (!EquipBaseScoreDataEntryMgr::Instance()->LoadData(configpath + "EquipBaseScoreTable.txt")) return false;
		if (!EquipStrenLvScoreCoeDataEntryMgr::Instance()->LoadData(configpath + "EquipStrenLvScoreCoeTable.txt")) return false;
		DATA_TABLE_LOAD("EquipEnhanceAttributeTable.txt", EquipEnhanceAttributeDataEntryMgr::Instance());
		DATA_TABLE_LOAD("EquipEnhanceCostTable.txt", EquipEnhanceCostDataEntryMgr::Instance());
		DATA_TABLE_LOAD("EnhanceTicketTable.txt", EnhanceTicketDataMgr::Instance());
		DATA_TABLE_LOAD("MaterialsSynthesis.txt", MaterialsSynthesisDataEntryMgr::Instance());
		DATA_TABLE_LOAD("EquieUpdateTable.txt", EquieUpdateDataEntryMgr::Instance());
		DATA_TABLE_LOAD("HeadFrameCompensateTable.txt", HeadFrameCompensateDataEntryMgr::Instance());
		DATA_TABLE_LOAD("EqualPvPEuqipTable.txt", EqualPvPEuqipDataEntryMgr::Instance());

		DATA_TABLE_LOAD("NewTitleTable.txt", NewTitleItemDataEntryMgr::Instance());
		DATA_TABLE_LOAD("WeekSignTable.txt", WeekSignDataEntryMgr::Instance());
		DATA_TABLE_LOAD("WeekSignSumTable.txt", WeekSignSumDataEntryMgr::Instance());

		DATA_TABLE_LOAD("AssistEqStrengFouerDimAddTable.txt", AssistEqStrengFouerDimAddDataEntryMgr::Instance());

		DATA_TABLE_LOAD("MonthSignAward.txt", MonthSignAwardDataEntryMgr::Instance()); 
		DATA_TABLE_LOAD("MonthSignCollectAward.txt", MonthSignCollectAwardDataEntryMgr::Instance());

		DATA_TABLE_LOAD("DropSmellControlTable.txt", DropSmellControlDataEntryMgr::Instance());
		DATA_TABLE_LOAD("EquipBaseScoreModTable.txt", EquipBaseScoreModDataEntryMgr::Instance());
		DATA_TABLE_LOAD("EqSuitScore.txt", EqSuitScoreDataEntryMgr::Instance());

		DATA_TABLE_LOAD("UltimateChallengeBuffTable.txt", UltimateChallengeBuffDataEntryMgr::Instance());
		DATA_TABLE_LOAD("UltimateChallengeDungeonTable.txt", UltimateChallengeDungeonDataEntryMgr::Instance());

		DATA_TABLE_LOAD("MallShopMultiITable.txt", MallShopMultiIDataEntryMgr::Instance());

		DATA_TABLE_LOAD("EquipInscriptionHoleTable.txt", EquipInscriptionHoleDataEntryMgr::Instance());
		DATA_TABLE_LOAD("FashionDecomposeTable.txt", FashionDecomposeDataEntryMgr::Instance());
		DATA_TABLE_LOAD("InscriptionTable.txt", InscriptionDataEntryMgr::Instance());
		DATA_TABLE_LOAD("InscriptionDropItemTable.txt", InscriptionDropItemDataEntryMgr::Instance());
		DATA_TABLE_LOAD("InscriptionExtractTable.txt", InscriptionExtractDataEntryMgr::Instance());
		DATA_TABLE_LOAD("InscriptionHoleSetTable.txt", InscriptionHoleSetDataEntryMgr::Instance());
		DATA_TABLE_LOAD("InscriptionSynthesisTable.txt", InscriptionSynthesisDataEntryMgr::Instance());

		DATA_TABLE_LOAD("RechargePushTable.txt", RechargePushDataEntryMgr::Instance());
		DATA_TABLE_LOAD("MagicCardUpdateTable.txt", MagicCardUpdateDataMgr::Instance());
		DATA_TABLE_LOAD("MallFashionLimitedSuitTable.txt", MallFashionLimitedSuitDataEntryMgr::Instance());

		DATA_TABLE_LOAD("ChijiRewardTable.txt", ChijiRewardDataEntryMgr::Instance());

		DATA_TABLE_LOAD("OverlondDeviceValueTable.txt", EnhanceOverLoadDataEntryMgr::Instance());

		DATA_TABLE_LOAD("AntiAddictHolidayTable.txt", AntiAddictHolidayDataEntryMgr::Instance());
		DATA_TABLE_LOAD("AntiAddictTimeTable.txt", AntiAddictTimeDataEntryMgr::Instance());

		DATA_TABLE_LOAD("BeadConvertCostTable.txt", BeadConvertCostDataEntryMgr::Instance());
		DATA_TABLE_LOAD("BeadConvertDropTable.txt", BeadConvertDropDataEntryMgr::Instance());

		DATA_TABLE_LOAD("HonorLevelTable.txt", HonorLevelDataEntryMgr::Instance());
		DATA_TABLE_LOAD("HonorRankExpTable.txt", HonorRankExpDataEntryMgr::Instance());
		DATA_TABLE_LOAD("HonorRewardTable.txt", HonorRewardDataEntryMgr::Instance());

		DATA_TABLE_LOAD("EquipStrModIndAtkTable.txt", EquipIndpendatkStrengModDataEntryMgr::Instance());

		DATA_TABLE_LOAD("EquieChangeTable.txt", EquipChangeDataEntryMgr::Instance());
		DATA_TABLE_LOAD("EquChangeConsumeTable.txt", EquipChgConsDataEntryMgr::Instance());

		DATA_TABLE_LOAD("WeekSignSpringTable.txt", WeekSignSpringDataEntryMgr::Instance());

		DATA_TABLE_LOAD("WeaponBuildCostTable.txt", WeaponBuildCostDataEntryMgr::Instance());
		DATA_TABLE_LOAD("WeaponBuildQualityRandomTable.txt", WeaponBuildQualityRandomDataEntryMgr::Instance());
		DATA_TABLE_LOAD("WeaponBuildPrpertyPoolTable.txt", WeaponBuildPropertyPoolDataEntryMgr::Instance());
		DATA_TABLE_LOAD("WeaponBuildRandomPoolTable.txt", WeaponBuildRandomPoolDataEntryMgr::Instance());
		DATA_TABLE_LOAD("WeaponBuildTable.txt", WeaponBuildDataEntryMgr::Instance());

		DATA_TABLE_LOAD("AppraisalProbabilityTable.txt", PlantOpActTreesDataEntryMgr::Instance());

		DATA_TABLE_LOAD("LostDungeonFloorTable.txt", LostDungeonFloorDataEntryMgr::Instance());
		DATA_TABLE_LOAD("LostDungeonElementTable.txt", LostDungeonElementDataEntryMgr::Instance());
		DATA_TABLE_LOAD("LostDungeonTreasureChestTable.txt", LostDungeonTreasureDataEntryMgr::Instance());

		DATA_TABLE_LOAD("OverlondDeviceValueTable.txt", OverlondDeviceValueDataEntryMgr::Instance());
		DATA_TABLE_LOAD("OneStrengEnhanceTicketTable.txt", OnceStrTicketDataMgr::Instance());

		DATA_TABLE_LOAD("GoldConsignmentValueTable.txt", GoldConsignmentValueDataEntryMgr::Instance());

		DATA_TABLE_LOAD("UltimateChallengeRewardTable.txt", UltimateChallengeRewardDataEntryMgr::Instance());

		DATA_TABLE_LOAD("ItemPerfectGradeTable.txt", ItemPerfectGradeDataEntryMgr::Instance());
	}
    catch (std::string str)
    {
        ErrorLogStream << str << LogStream::eos;
        return false;
    }
	
	//注册道具使用次数计数
	std::vector<UInt32> itemIds = ItemDataEntryMgr::Instance()->GetDayUseItemCount();
	for (size_t i = 0; i < itemIds.size(); ++i)
	{
		std::string counterName = ITEM_DAYUSE_PRE + Avalon::StringUtil::ConvertToString(itemIds[i]);
		const CounterConfig* cfg = CounterCfg::Instance()->GetCycleConfig(counterName);
		if (!cfg)
		{
			CounterCfg::Instance()->RegCounter(counterName.c_str(), COUNTER_CYCLE_DAY, 6);
		}
	}

	// 注册团本道具
	std::vector<UInt32> teamCopyItems = ItemDataEntryMgr::Instance()->GetTeamCopyItem();
	for (size_t i = 0; i < teamCopyItems.size(); ++i)
	{
		std::string counterName = COUNTER_TEAM_COPY_ITEM_USE_CNT + Avalon::StringUtil::ConvertToString(teamCopyItems[i]);
		const CounterConfig* cfg = CounterCfg::Instance()->GetCycleConfig(counterName);
		if (!cfg)
		{
			CounterCfg::Instance()->RegCounter(counterName.c_str(), 0);
		}
	}

	//注册飞升深渊礼盒道具id
	std::vector<UInt32> flyHellGiftItemIds = ItemDataEntryMgr::Instance()->GetFlyHellGiftItems();
	for (size_t i = 0; i < flyHellGiftItemIds.size(); ++i)
	{
		std::string counterName = ITEM_FLY_HELL_GIFT_USE_PRE + Avalon::StringUtil::ConvertToString(flyHellGiftItemIds[i]);
		const CounterConfig* cfg = CounterCfg::Instance()->GetCycleConfig(counterName);
		if (!cfg)
		{
			CounterCfg::Instance()->RegCounter(counterName.c_str(), COUNTER_CYCLE_NONE);
		}
	}

	//注册道具周使用次数计数
	std::vector<UInt32> week6ItemIds = ItemDataEntryMgr::Instance()->GetWeek6UseItemCount();
	for (size_t i = 0; i < week6ItemIds.size(); ++i)
	{
		std::string counterName = ITEM_WEEK6USE_PRE + Avalon::StringUtil::ConvertToString(week6ItemIds[i]);
		const CounterConfig* cfg = CounterCfg::Instance()->GetCycleConfig(counterName);
		if (!cfg)
		{
			CounterCfg::Instance()->RegCounter(counterName.c_str(), COUNTER_CYCLE_WEEK_SPECIFY, 6);
		}
	}

	return true;
}

void SSApplication::ReloadDataTables()
{
	std::string configpath = CONFIG_PATH;
	configpath = configpath + "DataTables/";

	InfoLogStream << "Start reload tables..." << LogStream::eos;

	try
	{
		if (m_NewReloadTable)
		{
			CLConfigLoadMgr::ReloadDataTables(configpath);
		}
		else
		{
			SystemValueDataEntryMgr::Instance()->ReloadData(configpath + "SystemValueTable.txt");
			GiftPackDataMgr::Instance()->ReloadData(configpath + "GiftPackTable.txt");
			GiftItemDataMgr::Instance()->ReloadData(configpath + "GiftTable.txt");
			ActivitySystemConfigDataEntryMgr::Instance()->ReloadData(configpath + "ActivitySystemConfig.txt");
			GoldConsignmentValueDataEntryMgr::Instance()->ReloadData(configpath + "GoldConsignmentValueTable.txt");
		}
	}
	catch (std::string str)
	{
		ErrorLogStream << str << LogStream::eos;
	}

	UInt32 ratio = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_INDPEND_ATK_PVP_RATIO);
	ratio = (ratio == 0) ? 1000 : ratio;
	SetIndpentPvpRatio(ratio);
}

void SSApplication::LoadDynamicConfig()
{
	DebugLogStream << "load SceneServer dynamic config." << LogStream::eos;

	// 性能统计
	CLNetworkPerfMgr::Instance()->Init(GetProperties());

	SERVER_NAME = GetProperties()->GetProperty("Server", "name").GetString();
	m_IsCounterLog = GetProperties()->GetProperty("Server", "is_counter_log");

	m_IsOpenNewSync = GetProperties()->GetProperty("Game", "new_sync");

	m_SceneMgrTickIntervalMSec = GetProperties()->GetProperty("Server", "scenemgr_tick_interval_ms");

	{
		m_CheckAntiAddict = GetProperties()->GetProperty("Game", "IsCheckAntiAddict");
		m_TourLoginInterval = GetProperties()->GetProperty("Game", "TourLoginInterval");
		if (m_TourLoginInterval)
			m_TourLoginInterval = 360;

		m_TourOnlineTime = GetProperties()->GetProperty("Game", "TourOnlineTime");
		if (m_TourOnlineTime == 0)
			m_TourOnlineTime = 1;

		m_AntiAddictTipTime = GetProperties()->GetProperty("Game", "AntiAddictTipTime");
		if (m_AntiAddictTipTime == 0)
			m_AntiAddictTipTime = 10;

		std::string TourAddictNotifyTime = GetProperties()->GetProperty("Game", "TourAddictNotifyTime").GetString();
		std::string NoAdultAddictNotifyTime = GetProperties()->GetProperty("Game", "NoAdultAddictNotifyTime").GetString();
		LoadAntiAddictNotifyTime(TourAddictNotifyTime, NoAdultAddictNotifyTime);

		DebugLogStream << "CheckAntiAddict:" << m_CheckAntiAddict << " TourLoginInterval:" << m_TourLoginInterval
			<< " TourOnlineTime:" << m_TourOnlineTime << LogStream::eos;
	}

	m_PlayerBuffCacheTime = GetProperties()->GetProperty("Game", "player_buff_cache_sec");

	if (m_PlayerBuffCacheTime == 0)
	{
		// 默认缓存时间为30分钟
		m_PlayerBuffCacheTime = 30 * 60;
	}

	m_MaxPlayerBuffCacheNum = GetProperties()->GetProperty("Game", "player_buff_cache_num");
	if (m_MaxPlayerBuffCacheNum == 0)
	{
		// 默认缓存个数是5000个
		m_MaxPlayerBuffCacheNum = 5000;
	}
	InfoLogStream << "MaxPlayerBuffCacheNum(" << m_MaxPlayerBuffCacheNum << ")" << LogStream::eos;

	m_NewOfflineCharge = GetProperties()->GetProperty("Server", "use_new_offline_charge");

	m_IsHandleAbnormalTrans = !GetProperties()->GetProperty("Game", "close_handle_abtrans");
	m_IsCloseDevil = GetProperties()->GetProperty("Game", "CloseDevil");
	m_StrenTicketPlanRecordFilePath = GetProperties()->GetProperty("Game", "stren_ticket_plan_record_path").GetString();
	m_ArtifactJarRecordFilePath = GetProperties()->GetProperty("Game", "artifact_jar_record_path").GetString();
	m_StrenTicketFuseUseMaxTime = GetProperties()->GetProperty("Game", "stren_ticket_fuse_use_max_time");
	if (m_StrenTicketFuseUseMaxTime == 0)
	{
		m_StrenTicketFuseUseMaxTime = 1000;
	}
	m_IsOpenChargeSaveData = !GetProperties()->GetProperty("Game", "close_save_data_after_charge");

	m_MovePacketIntervalMSec = GetProperties()->GetProperty("Game", "move_packet_interval");
	if (m_MovePacketIntervalMSec == 0)
	{
		m_MovePacketIntervalMSec = 100;
	}

	m_WatchListUpdateIntervalMSec = GetProperties()->GetProperty("Game", "watch_list_update_interval");
	if (m_WatchListUpdateIntervalMSec == 0)
	{
		m_WatchListUpdateIntervalMSec = 1000;
	}

	UInt32 packetNumPerTick = GetProperties()->GetProperty("Game", "packet_num_pertick");
	if (packetNumPerTick != 0)
	{
		MAX_PACKET_PERTICK = packetNumPerTick;
	}

	m_JarCountPath = GetProperties()->GetProperty("Game", "JarCountPath").GetString();
	if (m_JarCountPath == "")
	{
		m_JarCountPath = std::string(ROOT_PATH) + std::string("JarCountPath/");
	}

	m_IsUseFreezeAbnormalNew = GetProperties()->GetProperty("Game", "use_freeze_abnormal_new");

	m_EnhanceCountPath = GetProperties()->GetProperty("Game", "EnhanceCountPath").GetString();
	if (m_EnhanceCountPath == "")
	{
		m_EnhanceCountPath = std::string(ROOT_PATH) + std::string("EnhanceCountPath/");
	}

	m_BeadConvertCountPath = GetProperties()->GetProperty("Game", "BeadConvertCountPath").GetString();
	if (m_BeadConvertCountPath == "")
	{
		m_BeadConvertCountPath = std::string(ROOT_PATH) + std::string("BeadConvertCountPath/");
	}

	m_PlantPath = GetProperties()->GetProperty("Game", "PlantCountPath").GetString();
	if (m_PlantPath == "")
	{
		m_PlantPath = std::string(ROOT_PATH) + std::string("PlantAppraCount/");
	}

	m_ComposeCardPath = GetProperties()->GetProperty("Game", "ComposeCountPath").GetString();
	if (m_ComposeCardPath == "")
	{
		m_ComposeCardPath = std::string(ROOT_PATH) + std::string("ComposeCountPath/");
	}

	m_GiftCountPath = GetProperties()->GetProperty("Game", "GiftCountPath").GetString();
	if (m_GiftCountPath == "")
	{
		m_GiftCountPath = std::string(ROOT_PATH) + std::string("GiftCountPath/");
	}

	InfoLogStream << "max packet per tick = " << MAX_PACKET_PERTICK << LogStream::eos;
	{
		std::string needTransTableStr = GetProperties()->GetProperty("Record", "TransTables").GetString();
		std::vector<std::string> tables;
		Avalon::StringUtil::Split(needTransTableStr, tables, "|");
		CLRecordClient::Instance()->SetNeedTransTables(tables);

		InfoLogStream << "need trans tables: " + needTransTableStr << LogStream::eos;
	}
	m_OpenRestartDungeon = !GetProperties()->GetProperty("Game", "CloseRestartDungeon");
	m_IsCheckStartDungeonFrequence = GetProperties()->GetProperty("Game", "CheckStartDungeonFrequence");
	m_MaxStartDungeonNumInSecond = GetProperties()->GetProperty("Game", "MaxStartDungeonNumInSecond");

	InfoLogStream << "OpenRestartDungeon(" << m_OpenRestartDungeon << ") IsCheckStartDungeonFrequence(" << m_IsCheckStartDungeonFrequence 
					<< ") MaxStartDungeonNumInSecond(" << m_MaxStartDungeonNumInSecond << ")." << LogStream::eos;

	m_CheckJarType = !GetProperties()->GetProperty("Game", "CloseCheckJarType");
	{
		std::string str = GetProperties()->GetProperty("Game", "OpenJarIds").GetString();
		std::vector<std::string> params;
		Avalon::StringUtil::Split(str, params, "|");

		m_CanOpenJarIds.clear();
		for (auto param : params)
		{
			UInt32 id = Avalon::StringUtil::ConvertToValue<UInt32>(param);
			if (id > 0)
			{
				m_CanOpenJarIds.push_back(id);
			}
		}
		InfoLogStream << "CheckJarType(" << m_CheckJarType << ") OpenJarIds(" << str << ")." << LogStream::eos;
	}

	m_NewShopRefesh = !GetProperties()->GetProperty("Game", "CloseNewShopRefesh");
	m_NewChatCheck = !GetProperties()->GetProperty("Game", "CloseNewChatCheck");
	m_OpenMoveCheck = !GetProperties()->GetProperty("Game", "CloseMoveCheck");
	m_OpenDetailLog = GetProperties()->GetProperty("Game", "OpenDetailLog");
	m_ClientLogMaxLength = GetProperties()->GetProperty("Game", "ClientLogMaxLength");
	if (m_ClientLogMaxLength == 0)
	{
		m_ClientLogMaxLength = 40;
	}
	m_CheckStrengthenResult = !GetProperties()->GetProperty("Game", "CloseCheckStrengthenResult");
	m_OpenPerf = GetProperties()->GetProperty("Game", "OpenPerf");
	m_OpenFastSellBat = GetProperties()->GetProperty("Game", "OpenFastSellBat");
	m_NewHeartbeat = GetProperties()->GetProperty("Game", "NewHeartbeat");
	m_OpenPlayerOnOnline = GetProperties()->GetProperty("Game", "OpenPlayerOnOnline");

	m_NewReloadTable = GetProperties()->GetProperty("Game", "new_reload_table");
	// 需要重载的表格
	{
		std::string reloadTables = GetProperties()->GetProperty("Game", "ReloadTables").GetString();
		LoadNeedReloadTables(reloadTables);
		InfoLogStream << "need reload table! "<< reloadTables << LogStream::eos;
	}

	{
		bool isObjectRecord = GetProperties()->GetProperty("Game", "IsObjectRecord");
		InfoLogStream << "Memory Object Record Switch:" << isObjectRecord << ", m_ClientLogMaxLength:" << m_ClientLogMaxLength 
							<< ", m_CheckStrengthenResult:" << m_CheckStrengthenResult << " m_OpenPerf:" << m_OpenPerf
							<< " m_NewHeartbeat:" << m_NewHeartbeat
							<< " m_OpenFastSellBat:" << m_OpenFastSellBat
							<< " m_OpenPlayerOnOnline:" << m_OpenPlayerOnOnline
							<< LogStream::eos;
		MEMORY_OBJECT_SWITCH(isObjectRecord);
	}

	UInt8 tmpPriority = GetProperties()->GetProperty("Log", "priority");
	if (tmpPriority >= Avalon::LOG_DEBUG && tmpPriority < Avalon::LOG_MAX)
	{
		GetSysLog()->SetPriority((Avalon::LogPriority)tmpPriority);
	}
}

void SSApplication::RegisterReloadDataTables()
{
	REGISTER_DATA_TABLE_RELOAD("SystemValueTable.txt", &SystemValueDataEntryMgr::ReloadData, SystemValueDataEntryMgr::Instance());
	REGISTER_DATA_TABLE_RELOAD("GiftPackTable.txt", &GiftPackDataMgr::ReloadData, GiftPackDataMgr::Instance());
	REGISTER_DATA_TABLE_RELOAD("GiftTable.txt", &GiftItemDataMgr::ReloadData, GiftItemDataMgr::Instance());
	REGISTER_DATA_TABLE_RELOAD("ActivitySystemConfig.txt", &ActivitySystemConfigDataEntryMgr::ReloadData, ActivitySystemConfigDataEntryMgr::Instance());
	REGISTER_DATA_TABLE_RELOAD("GoldConsignmentValueTable.txt", &GoldConsignmentValueDataEntryMgr::ReloadData, GoldConsignmentValueDataEntryMgr::Instance());
}

void SSApplication::LoadAntiAddictNotifyTime(const std::string& tourTime, const std::string& noAdultTime)
{
	{
		m_AddictTourNotifyTimeVec.clear();
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(tourTime, strs, "|");
		for (auto& iter : strs)
		{
			UInt32 id = Avalon::StringUtil::ConvertToValue<UInt32>(iter);
			if (id > 0)
			{
				m_AddictTourNotifyTimeVec.push_back(id);
			}
		}
	}

	{
		m_AddictNoAdultNotifyTimeVec.clear();
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(noAdultTime, strs, "|");
		for (auto& iter : strs)
		{
			UInt32 id = Avalon::StringUtil::ConvertToValue<UInt32>(iter);
			if (id > 0)
			{
				m_AddictNoAdultNotifyTimeVec.push_back(id);
			}
		}
	}
}

UInt32 SSApplication::IsNotifyTourAntiAddict(UInt32 oddTime)
{
	for (auto& iter : m_AddictTourNotifyTimeVec)
	{
		if (oddTime == iter * MIN_TO_SEC)
		{
			return iter;
		}
	}

	return 0;
}

UInt32 SSApplication::IsNotifyNoAdultAntiAddict(UInt32 oddTime)
{
	for (auto& iter : m_AddictNoAdultNotifyTimeVec)
	{
		if (oddTime == iter * MIN_TO_SEC)
		{
			return iter;
		}
	}

	return 0;
}

