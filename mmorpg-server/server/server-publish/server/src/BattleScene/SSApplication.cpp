#include "SSApplication.h"
#include <AvalonTimer.h>
#include <CLRecordClient.h>
#include <AvalonBase64.h>

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
#include <CLChijiSpawnItemDataEntry.h>
#include <CLChiJiPointDataEntry.h>
#include <CLChiJiRegionDataEntry.h>
#include <CLChiJiTimeDataEntry.h>
#include <CLChiJiSkillDataEntry.h>
#include <CLChiJiNpcRewardDataEntry.h>
#include <CLChiJiShopTimeDataEntry.h>
#include <CLHeadFrameCompensateDataEntry.h>
#include <CLNewTitleDataEntry.h>
#include <CLEquipBaseScoreModDataEntry.h>
#include <CLLostDungeonResourcesDataEntry.h>
#include <CLLostDungeonMissionDataEntry.h>
#include <CLLostDungeonBattleDataEntry.h>
#include <CLNetworkPerformance.h>

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
#include "ActivityMgr.h"
#include "SSDungeonConfigMgr.h"
#include "StrengthenTicketPlanMgr.h"

#include <CLConfigLoadMgr.h>

SSApplication::SSApplication()
{
	m_TickTime = 0;
	m_ReloadConfig = false;
	m_SceneMgrTickIntervalMSec = 0;
	m_NewOfflineCharge = false;
	m_IsHandleAbnormalTrans = true;
	m_ReloadTable = false;
	m_ChiJiChoiceSkillNum = 0;

	m_TickTimer200ms.SetInterval(200);
	m_TickTimer500ms.SetInterval(500);
	m_TickTimer1000ms.SetInterval(1000);
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
	
	if(!ScriptMgr::Instance()->Init())
	{
		ErrorLogStream << "script system init failed!" << LogStream::eos;
		return false;
	}

	if(SERVER_TYPE == ST_SCENE)
	{
		if(!CLRecordClient::Instance()->Init(this, SSNetwork::Instance()->FindConnectionByType(ST_RECORD)))
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

	if(UL_FAIL == _analyzer.Init() || NULL == _analyzer.GetInstance(UDPLOG_SCENE_UID))
	{
		ErrorLogStream << "udplog init failed!" << LogStream::eos;
		return false;
	}

	if (SERVER_TYPE != ST_BSCENE)
	{
		if (!DropItemMgr::Instance()->Init())
		{
			ErrorLogStream << "drop item manager init failed!" << LogStream::eos;
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

		SSActivityDungeonMgr::Instance()->Init();

		ActivateCondCheckEvent::Instance()->Init();
	}

	m_TickTime = Avalon::Time::CurrentTime().MSec();
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

		if (SERVER_TYPE != ST_BSCENE)
		{
			DATA_TABLE_LOAD("BuffTable.txt", BuffDataEntryMgr::Instance());
			DATA_TABLE_LOAD("FunctionUnLock.txt", FuncNotifyDataEntryMgr::Instance());
			DATA_TABLE_LOAD("MissionTable.txt", TaskDataEntryMgr::Instance());
		}
		else
		{
			DATA_TABLE_LOAD("LostDungeonBuffTable.txt", BuffDataEntryMgr::Instance());
			DATA_TABLE_LOAD("ChijiBuffTable.txt", BuffDataEntryMgr::Instance());
			DATA_TABLE_LOAD("ChijiItemTable.txt", ItemDataEntryMgr::Instance());
			DATA_TABLE_LOAD("ChiJiSkillTable.txt", ChiJiSkillDataEntryMgr::Instance());
		}

		DATA_TABLE_LOAD("ItemTable.txt", ItemDataEntryMgr::Instance());

		if (!BuffInfoEntryMgr::Instance()->LoadData(configpath + "BuffInfoTable.txt")) return false;

        if (!RandBaseDataEntryMgr::Instance()->LoadData(configpath + "RandBaseTable.txt")) return false;
        if (!RandAttrDataEntryMgr::Instance()->LoadData(configpath + "RandPropTable.txt")) return false;
        if (!RandAttrNumDataEntryMgr::Instance()->LoadData(configpath + "RandPropNumTable.txt")) return false;
        if (!RandAttrValueDataEntryMgr::Instance()->LoadData(configpath + "RandPropValueTable.txt")) return false;

		if (!RewardAdapterDataEntryMgr::Instance()->LoadData(configpath + "RewardAdapterTable.txt")) return false;
        if (!ForbitWordParser::Instance()->Init(configpath + "forbitword.txt")) return false;
		if (!ForbitWordParser::Instance()->InitValidCoolName(configpath + "coolname.txt")) return false;
        if (!SceneDataEntryMgr::Instance()->LoadData(configpath + "CitySceneTable.txt")) return false;
		if (!MallPersonalTailorDataEntryMgr::Instance()->LoadData(configpath + "PersonalTailorTriggerTable.txt")) return false;
	
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
		if (!SysNotifyDataEntryMgr::Instance()->LoadData(configpath + "CommonTipsDesc.txt")) return false; //by huchenhui
        if (!MonsterPrefixDataEntryMgr::Instance()->LoadData(configpath + "MonsterPrefixTable.txt")) return false;
		if (!JobDataEntryMgr::Instance()->LoadData(configpath + "JobTable.txt")) return false;	//by huchenhui
        if (!MonsterGroupDataEntryMgr::Instance()->LoadData(configpath + "MonsterGroupTable.txt")) return false;
        if (!MonsterAttributeDataEntryMgr::Instance()->LoadData(configpath + "MonsterAttributeTable.txt")) return false;
        if (!DungeonPrefixLimitDataMgr::Instance()->LoadData(configpath + "DungeonPrefixLimitTable.txt")) return false;
        if (!DungeonDestructDataEntryMgr::Instance()->LoadData(configpath + "DestrucTable.txt")) return false;
		if (!WarpStoneLevelDataEntryMgr::Instance()->LoadData(configpath + "WarpStoneLevel.txt")) return false;	//adder by huchenhui 2016.07.21
		if (!WarpStoneDataEntryMgr::Instance()->LoadData(configpath + "WarpStone.txt")) return false;			//adder by huchenhui 2016.07.21
		if (!EnergyStoneDataEntryMgr::Instance()->LoadData(configpath + "EnergyStone.txt")) return false;		//adder by huchenhui 2016.07.21
        if (!DungeonHellDataEntryMgr::Instance()->LoadData(configpath + "DungeonHellTable.txt")) return false;
		if (!RetinueDataEntryMgr::Instance()->LoadData(configpath + "FollowPetTable.txt")) return false;					//adder by huchenhui 2016.07.28
		if (!RetinueSkillDataEntryMgr::Instance()->LoadData(configpath + "RetinueChangeBuffTable.txt")) return false;		//adder by huchenhui 2016.12.2
		if (!DeathTowerAwardDataEntryMgr::Instance()->LoadData(configpath + "DeathTowerAwardTable.txt")) return false;
		if (!MagicCardDataMgr::Instance()->LoadData(configpath + "MagicCardTable.txt")) return false;
		if (!PreciousBeadDataEntryMgr::Instance()->LoadData(configpath + "BeadTable.txt")) return false;
		if (!RaceEndDropDataEntryMgr::Instance()->LoadData(configpath + "RaceEndDropTable.txt")) return false;
		if (!AnnouncementDataEntryMgr::Instance()->LoadData(configpath + "AnnounceTable.txt")) return false;	//adder by huchenhui 2016.08.18
		if (!EquipAttrDataMgr::Instance()->LoadData(configpath + "EquipAttrTable.txt")) return false;
		if (SERVER_TYPE != ST_BSCENE)
		{
			DATA_TABLE_LOAD("ActiveTable.txt", ActiveTaskDataEntryMgr::Instance());
		}

		if (!SeasonDailyDataEntryMgr::Instance()->LoadData(configpath + "SeasonDailyTable.txt")) return false;		//adder by huchenhhui 2016.09.30
		if (!VipDataEntryMgr::Instance()->LoadData(configpath + "VipTable.txt")) return false;						//adder by huchenhhui 2016.09.30
		if (!VipPrivilegeDataEntryMgr::Instance()->LoadData(configpath + "VipPrivilegeTable.txt")) return false;
		if (!SystemValueDataEntryMgr::Instance()->LoadData(configpath + "SystemValueTable.txt")) return false;		//adder by huchenhhui 2016.10.03
		if (!QuickBuyDataEntryMgr::Instance()->LoadData(configpath + "QuickBuyTable.txt")) return false;
		if (!JarBonusDataMgr::Instance()->LoadData(configpath + "JarBonus.txt")) return false;
		if (!RetinueLevelDataEntryMgr::Instance()->LoadData(configpath + "RetinueLevelTable.txt")) return false;
		if (!PkLevelDataEntryMgr::Instance()->LoadData(configpath + "PkLevelTable.txt")) return false;
	
		if (!EquipStrModDataMgr::Instance()->LoadData(configpath + "EquipStrModTable.txt")) return false;
		if (!BuffDrugDataMgr::Instance()->LoadData(configpath + "BuffDrugConfigTable.txt")) return false;
		if (!GuildBuildingDataMgr::Instance()->LoadData(configpath + "GuildBuildingTable.txt")) return false;
		if (!GuildTerritoryDataEntryMgr::Instance()->LoadData(configpath + "GuildTerritoryTable.txt")) return false;		//adder by huchenhhui 2018.04.24
		if (!DungeonMakeUpDataMgr::Instance()->LoadData(configpath + "DungeonMakeUp.txt")) return false;
		if (!FatigueMakeUpDataMgr::Instance()->LoadData(configpath + "FatigueMakeUp.txt")) return false;
		if (!EqualItemDataEntryMgr::Instance()->LoadData(configpath + "EqualItemTable.txt")) return false;
		if (SERVER_TYPE != ST_BSCENE)
		{
			DATA_TABLE_LOAD("StrengthenTicketTable.txt", StrTicketDataMgr::Instance());
		}
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
		if (!GiftPackDataMgr::Instance()->LoadData(configpath + "ChiJiGiftPackTable.txt")) return false;
		if (!GiftItemDataMgr::Instance()->LoadData(configpath + "ChiJiGiftTable.txt")) return false;
		GiftItemDataMgr::Instance()->MergeToGiftPack();
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

		if (SERVER_TYPE != ST_BSCENE)
		{
			DATA_TABLE_LOAD("OpActivityTaskTable.txt", OpActivityTaskDataEntryMgr::Instance());
		}
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
		DATA_TABLE_LOAD("EquieUpdateTable.txt", EquieUpdateDataEntryMgr::Instance());
		DATA_TABLE_LOAD("HeadFrameCompensateTable.txt", HeadFrameCompensateDataEntryMgr::Instance());
		DATA_TABLE_LOAD("NewTitleTable.txt", NewTitleItemDataEntryMgr::Instance());
		if (SERVER_TYPE == ST_BSCENE)
		{
			DATA_TABLE_LOAD("ChijiSpawnItemTable.txt", ChijiSpawnItemDataEntryMgr::Instance());
			DATA_TABLE_LOAD("ChiJiPointTable.txt", ChiJiPointDataEntryMgr::Instance());
			DATA_TABLE_LOAD("ChiJiRegionTable.txt", ChiJiRegionDataEntryMgr::Instance());
			DATA_TABLE_LOAD("ChiJiTimeTable.txt", ChiJiTimeDataEntryMgr::Instance());
			DATA_TABLE_LOAD("ChiJiNpcRewardTable.txt", ChiJiNpcRewardDataEntryMgr::Instance());
			DATA_TABLE_LOAD("ChiJiShopItemTable.txt", ShopItemDataEntryMgr::Instance());
			DATA_TABLE_LOAD("ChiJiShopTable.txt", ShopDataEntryMgr::Instance());
			DATA_TABLE_LOAD("ChiJiShopTimeTable.txt", ChiJiShopTimeDataEntryMgr::Instance());
		}

		DATA_TABLE_LOAD("EquipBaseScoreModTable.txt", EquipBaseScoreModDataEntryMgr::Instance());
		DATA_TABLE_LOAD("LostDungeonResourcesTable.txt", LostDungeonResDataEntryMgr::Instance());
		DATA_TABLE_LOAD("LostDungeonMissionTable.txt", LostDungeonMissionDataEntryMgr::Instance());
		DATA_TABLE_LOAD("LostDungeonBattleTable.txt", LostDungeonBattleDataEntryMgr::Instance());


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
    }
    catch (std::string str)
    {
        ErrorLogStream << str << LogStream::eos;
        return false;
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
		SystemValueDataEntryMgr::Instance()->ReloadData(configpath + "SystemValueTable.txt");
		GiftPackDataMgr::Instance()->ReloadData(configpath + "ChiJiGiftPackTable.txt");
		GiftItemDataMgr::Instance()->ReloadData(configpath + "ChiJiGiftTable.txt");

		ActivitySystemConfigDataEntryMgr::Instance()->ReloadData(configpath + "ActivitySystemConfig.txt");
	}
	catch (std::string str)
	{
		ErrorLogStream << str << LogStream::eos;
	}

}

void SSApplication::LoadDynamicConfig()
{
	DebugLogStream << "load SceneServer dynamic config." << LogStream::eos;

	// 性能统计
	CLNetworkPerfMgr::Instance()->Init(GetProperties());

	SERVER_NAME = GetProperties()->GetProperty("Server", "name").GetString();
	m_IsCounterLog = GetProperties()->GetProperty("Server", "is_counter_log");

	m_SceneMgrTickIntervalMSec = GetProperties()->GetProperty("Server", "scenemgr_tick_interval_ms");

	m_CheckAntiAddict = !GetProperties()->GetProperty("Game", "close_antiaddict_check");

	m_PlayerBuffCacheTime = GetProperties()->GetProperty("Game", "player_buff_cache_sec");
	if (m_PlayerBuffCacheTime == 0)
	{
		// 默认缓存时间为30分钟
		m_PlayerBuffCacheTime = 30 * 60;
	}

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

	m_ChiJiChoiceSkillNum = GetProperties()->GetProperty("Game", "chi_ji_choice_skill");
	if (m_ChiJiChoiceSkillNum == 0)
	{
		m_ChiJiChoiceSkillNum = 6;
	}

	m_ChiJiMovePacketIntervalMs = GetProperties()->GetProperty("Game", "chiji_move_packet_interval");
	if (m_ChiJiMovePacketIntervalMs == 0)
	{
		m_ChiJiMovePacketIntervalMs = 100;
	}

	{
		m_DebugAccounts.clear();

		std::string debugAccounts = GetProperties()->GetProperty("Game", "debug_accounts").GetString();
		std::vector<std::string> accounts;
		Avalon::StringUtil::Split(debugAccounts, accounts, "|");
		for (auto& str : accounts)
		{
			auto accid = Avalon::StringUtil::ConvertToValue<UInt32>(str);
			m_DebugAccounts.push_back(accid);
		}

		InfoLogStream << "debug accounts(" << debugAccounts << ")." << LogStream::eos;
	}

	InfoLogStream << "max packet per tick = " << MAX_PACKET_PERTICK << ", m_ChiJiMovePacketIntervalMs = " << m_ChiJiMovePacketIntervalMs << LogStream::eos;
}
