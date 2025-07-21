#include "Player.h"


#include <AvalonTimer.h>
#include <AvalonRandom.h>
#include <AvalonBase64.h>
#include <AvalonTimeUtility.h>

#include <CLGateProtocol.h>
#include <CLMoveProtocol.h>
#include <CLSceneProtocol.h>
#include <CLSyncObjectProtocol.h>
#include <CLRecordClient.h>
#include <CLEnterGameProtocol.h>
#include <CLLeaveGameProtocol.h>
#include <CLPlayerDataEntry.h>
#include <CLTaskProtocol.h>
#include <CLMsgParser.h>
#include <CLForbitWord.h>
#include <CLChatProtocol.h>
#include <CLTradeProtocol.h>
#include <CLMailProtocol.h>
#include <CLRelationProtocol.h>
#include <CLRedPacketProtocol.h>
#include <CLSceneDataEntry.h>
#include <CLItemProtocol.h>
#include <CLVipProtocol.h>
#include <CLMallDataEntry.h>
#include <CLTeamProtocol.h>
#include <kingnet_analyzer.h>
#include <CLSortListProtocol.h>
#include <CLInviteGameProtocol.h>
#include <CLGiftBagDataEntry.h>
#include <CLDBLogger.h>
#include <CLCenterProtocol.h>
#include <CLStoryProtocol.h>
#include <CLFunctionDataEntry.h>
#include <CLStoryDataEntry.h>
#include <CLRoutineProtocol.h>
#include <CLChangeZoneProtocol.h>
#include <CLMatchProtocol.h>
#include <CLJobDataEntry.h>
#include <CLSettingProtocol.h>
#include <CLMonsterDataEntry.h>
#include <CLDungeonProtocol.h>
#include <CLAnnouncementDefine.h>
#include <CLSystemValueDataEntry.h>
#include <DropItemMgr.h>
#include <CLGuildProtocol.h>
#include <CLGuildBuildingDataEntry.h>
#include <CLGameSessionDefine.h>
#include <CLEqualItemDataEntry.h>
#include <CLWudaoRewardDataEntry.h>
#include <AsyncMsgCallbackMgr.h>
#include <CLDungeonTimesDataEntry.h>
#include <CLAuctionBoothDataEntry.h>
#include <CLCurrencyCostDataEntry.h>
#include <CLReplayProtocol.h>
#include <CLMallProtocol.h>
#include <CLEventMgr.h>
#include <CLDungeonDailyDropDataEntry.h>
#include <CLRoomProtocol.h>
#include <CLEquipSuitDataEntry.h>
#include <CLMallGiftPackDataEntry.h>
#include <CLCrossProtocol.h>
#include <CLBattleProtocol.h>
#include <CLMoneyManageDataEntry.h>
#include <CLSysRecordProtocol.h>
#include <CLCityMonsterDataEntry.h>
#include <CLNpcDataEntry.h>
#include <CLTeamDungeonDataEntry.h>
#include <CLScoreWarDataEntry.h>
#include <CLScoreWarProtocol.h>
#include <CLDrawPrizeDataEntry.h>
#include <CLPlayerDefine.h>
#include <CLMasterSystemDefine.h>
#include <CLMasterSysGiftDataEntry.h>
#include <ActivityUtil.h>
#include <CLShopMainFrameDataEntry.h>
#include <CLSecurityLockDefine.h>
#include <CLSecurityLockProtocol.h>
#include <CLCenterProtocol.h>
#include <CLEquipBaseScoreDataEntry.h>
#include <CLEquipStrenLvScoreCoeDataEntry.h>
#include <CLNewTitleProtocol.h>
#include <CLSystemValueDataEntry.h>
#include <CLQuickBuyDataEntry.h>
#include <CLLostDungeonDefine.h>
#include <CLBattleDefine.h>
#include <CLNetworkPerformance.h>

#include "PlayerMgr.h"
#include "SSRouter.h"
#include "Scene.h"
#include "SceneMgr.h"
#include "SSNetwork.h"
#include "ScriptMgr.h"
#include "TaskScript.h"
#include "Trade.h"
#include "Team.h"
#include "SortListMgr.h"
#include "PlayerScript.h"
#include "SceneSysNotify.h"
#include "ItemMaker.h"
#include "FinalEquipScript.h"
#include "SceneScript.h"
#include "SysInterface.h"
#include "ActivityMgr.h"
#include "ItemReasonMgr.h"
#include "QuickBuyTransaction.h"
#include "JarItem.h"
#include "SSServiceSwitchMgr.h"
#include "SSApplication.h"
#include "PlayerLoading.h"
#include "PlayerLoadCallback.h"
#include "CondActivateEvent.h"
#include "CityMonster.h"
#include "BattleScene.h"
#include "SceneSysNotify.h"
#include "GameParamMgr.h"
#include "LostBattleScene.h"
#include "DungeonEntryScene.h"
#include "LostDungeonMgr.h"
#include "LostDungeonTeam.h"

using namespace ErrorCode;
Player::Player()
{
	m_AccId = 0;
	m_AccountGuid = 0;
	SetGameStatus(PS_INIT);
	m_Inviter = 0;

	m_watchRoleIdList.reserve(GetMaxWatchPlayerNum());
	m_watchRoleIdSet.reserve(GetMaxWatchPlayerNum());

	for(int i = 0; i < LOGICTICK_MAX; ++i)
	{
		m_TickTime[i] = CURRENT_TIME.MSec();
	}

	SetWudaoStatus(WUDAO_STATUS_INIT);
	m_DailyTimer.SetTime(6);

	m_pVipBuff = NULL;
	m_EnterScene = 0;
	
	//采用组件的方式组织用户数据，每个系统一个组件
	m_ItemMgr.SetOwner(this);
	m_ShopMgr.SetOwner(this);
	m_TaskMgr.SetOwner(this);
	m_DailyTaskMgr.SetOwner(this);
	m_AchievementTaskMgr.SetOwner(this);
	m_RedPacketTaskMgr.SetOwner(this);
	m_LegendTaskMgr.SetOwner(this);
	m_ItemCD.SetOwner(this);
	m_CounterMgr.SetOwner(this);
	m_ActionCounterMgr.SetOwner(this);
	m_RoutineMgr.SetOwner(this);
	m_InviteMgr.SetOwner(this);
	m_EquipShopMgr.SetOwner(this);
    m_dungeonMgr.SetOwner(this);
	m_SkillBar.SetOwner(this);
	m_PvpSkillBar.SetOwner(this);
	//m_ItemBar.SetOwner(this);
	m_Fatigue.SetOwner(this);
    m_pkStatisticMgr.SetOwner(this);
	m_RetinueMgr.SetOwner(this);
	m_Vip.SetOwner(this);
	m_CycleTaskMgr.SetOwner(this);
	m_JarMgr.SetOwner(this);
	m_SeasonLevel.SetOwner(this);
	m_PetMgr.SetOwner(this);
	m_PvpSkillMgr.SetOwner(this);
	m_AbnormalTransactionMgr.SetOwner(this);
	m_MasterDailyTaskMgr.SetOwner(this);
	m_MasterAcademicGrowthTaskMgr.SetOwner(this);
	m_PlayerActivityInfo.SetOwner(this);
	m_ActivityAttributeMgr.SetOwner(this);
	m_HeadFrameMgr.SetOwner(this);
	m_NetSp.SetOwner(this);
	m_LostDungeonTaskMgr.SetOwner(this);

	m_ClientOption = 0;
	m_GmAuthority = 0;

	m_PunishFlag = 0;
	m_ForbitTalkDueTime = 0;

	m_OnlineTime = 0;
	m_DayOnlineTime = 0;
	m_totOnlineTime = 0;
	m_RecoverTime = 0;

	m_BusyTime = 0;

	m_LocalSavebackTime = 0;

	m_MajorCityId = 0;

	m_pLevelDataEntry = NULL;

	m_ZoneTask = 0;
	m_SrcZoneId = 0;

	m_pTrade = NULL;
	m_AssetsLockTime = 0;

	m_pTeam = NULL;
	m_TeamIntimacyBuff = 0;

	m_LoadDataCount = 0;
	m_LoadDataSerial = 0;

	m_pExpAddBuff = NULL;

	m_WallowFactor = 10000;

	m_ReviveTime = 0;
	m_ReviveCost = 0;
	m_ReviveTimes = 0;

    m_Runing = false;
    m_SceneDir = 0;
    m_pkValue = 0;
    m_titleId = 0;
	m_TotalChargeNum = 0;

	m_SP = 0;
	m_Awaken = AWAKEN_NOTHING;
	m_quickBuyTrans = NULL;

	m_LastUpdatePosTime = 0;
	m_LastUpdateWatchListTime = 0;

	m_chatHiFrqCount = 0;

	m_DeathTowerWipeoutEndTime = 0;

	m_GuildShopId = 0;

	for (UInt32 i = 0; i < sizeof(m_GuildBuildingLevel) / sizeof(m_GuildBuildingLevel[0]); i++)
	{
		m_GuildBuildingLevel[i] = 1;
	}

	m_GuildBattleNumber = 0;
	m_GuildBattleScore = 0;
	m_GuildBattleLotteryStatus = (UInt8)GuildBattleLotteryStatus::GBLS_INVALID;
	m_GuildEmblem = 0;

	m_MoneyManageStatus = MoneyManageStatus::MMS_NOT_BUY;

	m_DailyTaskScore = 0;
	m_WudaoPrevSceneId = 0;
	m_lastScene = NULL;

	m_NewBoot = 0;

	m_bLogicDayChanged = false;
	m_RefTime = 0;

	m_LastSendHornTime = 0;
	m_HornCombo = 0;

	m_IsCheckSp = 1;

	m_MallItemId = 0;
	m_IsPhoneBind = false;
	m_bFirstTimeItemTick = false;
	m_EquipBindPlayerLevel = { PL_TEN, PL_FIFTEEN, PL_TWENTY, PL_TWENTY_FIVE, PL_THIRTY, PL_THIRTY_FIVE, PL_FORTY, PL_FORTY_FIVE, PL_FIFTY };
	m_OldLevel = PLAYER_LEVEL_ON_BEGIN_CHECK_ACTIVATE_COND;
	m_ConditionActivateDBLoadState = (UInt8)CADBLS_NONE;

#ifdef _DEBUG
	m_testEquipStrengthFlag = 0;
	m_testHappySky = 0;
#endif

	m_RoomId = 0;
	m_OppoVipLevel = 0;
	m_SyncAddictTimer.SetInterval(Avalon::Time::SECS_OF_MIN * Avalon::Time::MSECS_OF_SEC);
	m_DayChargeNum = 0;
	m_CrossSceneNodeId = 0;
	m_SrcSceneNodeId = 0;
	m_BattleSceneNodeId = 0;
	m_NotifyChangeScene = true;
	m_ScoreWarBattleCount = 0;
	m_ScoreWarWinBattleCount = 0;
	m_ScoreWarLastBattleTime = 0;
	m_ScoreWarScore = 0;
	m_AcademicTotalGrowth = 0;
	m_MasterDailyTaskGrowth = 0;
	m_MasterAcademicTaskGrowth = 0;
	m_MasterGiveEquipGrowth = 0;
	m_MasterUplevelGrowth = 0;
	m_MasterGiveGiftGrowth = 0;
	m_MasterTeamClearDungeonGrowth = 0;
	m_GoodTeacherValue = 0;
	m_ResistMagic = 0;
	m_SecurityStatus = 0;

	m_RoleReturnTime = 0;
	m_RoleReturnLevel= 0;
	m_AgainReturn = 0;
	m_EnergyCoin = 0;
	m_RoleValueScore = 0;
	m_HeadFrameId = 0;

	SetClientLoading(false);

	m_BattleID = 0;
	m_CheckBattleTimer.SetInterval(1000);

	m_ChijiHp = HP_MP_FULL_VALUE;
	m_ChijiMp = HP_MP_FULL_VALUE;
	m_ChijiScore = 0;
	m_ChijiScoreResetTime = 0;
	m_ChijiNoWarEnemyId = 0;
	m_ChijiNoWarBattleId = 0;
	m_ChijiNoWarChoiceTime = 0;
	m_ChijiPKDungeonId = 0;
	m_lastLostBattleSceneId = 0;
	m_dungChageMode = LDCM_SINGLE;
	m_dungChageTeamId = 0;
	m_LostDungeonHp = HP_MP_FULL_VALUE;
	m_LostDungeonMp = HP_MP_FULL_VALUE;
	m_LostDungeonScore = 0;
	m_LostDungHpBack = 0;
	m_LostDungMpBack = 0;
	m_lostDungBatSt = 0;
	m_lostProtTime = 0;
	m_LostDungSpeedBack = PLAYER_INIT_MOVE_SPEED;
	m_lostDungLeaving = false;
}

Player::~Player()
{
	if (m_quickBuyTrans)
	{
		m_quickBuyTrans->OnFinish(this, ErrorCode::QUICK_BUY_TIMEOUT);
		delete m_quickBuyTrans;
		m_quickBuyTrans = NULL;
	}

	OnUnRegistGameEvent();

	for (auto cb : m_LoadingCallback)
	{
		CL_SAFE_DELETE(cb);
	}
	m_LoadingCallback.clear();
}


bool Player::SetGateID(UInt32 nodeid)
{
	m_Conn = SSNetwork::Instance()->FindConnection(nodeid);
	return m_Conn != NULL;
}

UInt32 Player::GetGateID() const
{
	if(m_Conn != NULL) return m_Conn->GetID();
	return 0;
}

const char* Player::GetBase64ID() const
{
	static char idstr[64];
	memset(idstr, 0, sizeof(idstr));
	UInt64 id = GetID();
	Avalon::base64_encode(idstr, 64, &id, sizeof(id));
	return idstr;
}

bool Player::SetupBase()
{
	m_pLevelDataEntry = PlayerDataEntryMgr::Instance()->FindEntry(GetLevel());
	if(m_pLevelDataEntry == NULL) return false;

	m_SkillBar.Init();
	m_SeasonLevel.Init();
	//m_ItemBar.Init();

	if (SERVER_TYPE != ST_BSCENE)
	{
		// 计算外观
		CountAvatar();
	}

	return true;
}

void Player::SyncProperty()
{
	Creature::SyncProperty();

	//同步队伍信息
	SyncTeamAround(true);

	//同步自身脏数据
	SyncSelfData(true);
}

UInt32 Player::Awaken()
{
	if (IsInitOccu())
	{
		ErrorLogStream << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") Awaken: not change occu" << LogStream::eos;
		return ErrorCode::PLAYER_AWAKEN_NOT_TRANSFORM_OCCU;
	}
		
	if (GetLevel() < AWAKEN_ONE_LEVEL)
	{
		ErrorLogStream << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") Awaken: player level not enough" << LogStream::eos;
		return ErrorCode::PLAYER_AWAKEN_LEVEL_ERROR;
	}
		

	//以后有二转了再修改
	if ((AwakenStatus)GetAwaken() > AWAKEN_NOTHING)
	{
		ErrorLogStream << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") Awaken: already awaken" << LogStream::eos;
		return ErrorCode::PLAYER_AWAKEN_EXIST;
	}

	SetAwaken(AWAKEN_ONE);

	OnAwaken();

	return ErrorCode::SUCCESS;
}

UInt32 Player::GetBirthCity()
{
	UInt32 birthCity = PlayerMgr::Instance()->GetInitSceneID();
    auto scene = SceneDataEntryMgr::Instance()->FindEntry(GetSceneID());
    if (scene)
    {
		birthCity = scene->birthCity;
    }

    return birthCity;
}

void Player::AdjestChargeInfo(UInt8 vipLevel, UInt32 vipExp, UInt32 point, UInt32 totalChargeNum)
{
	InfoLogStream << PLAYERINFO(this) << " vip from (" << GetVipLvl() << ", " << GetVipExp() << ") to (" << vipLevel << ", " << vipExp << ")." << LogStream::eos;
	InfoLogStream << PLAYERINFO(this) << " totalcharge from (" << m_TotalChargeNum << ") to (" << totalChargeNum << ")." << LogStream::eos;
	InfoLogStream << PLAYERINFO(this) << " point from (" << m_Point << ") to (" << point << ")." << LogStream::eos;

	m_Vip.Init(vipLevel, vipExp);
	m_Vip.SetDirty();

	m_Point = point;
	m_TotalChargeNum = totalChargeNum;
}

bool Player::CanEnterMap(UInt32 uMapId)
{
	uMapId = GetRealSceneId(uMapId);
	SceneDataEntry	*pDataEntry = SceneDataEntryMgr::Instance()->FindEntry(uMapId);
	if(NULL == pDataEntry) return false;

	if(IsStaticMap(pDataEntry->type))
	{
		if(GetLevel() < pDataEntry->level)
		{
			SendNotify(151, pDataEntry->level);
			return false;
		}
	}

	return true;
}

bool Player::IsInDungeon()
{
	SceneDataEntry* data = SceneDataEntryMgr::Instance()->FindEntry(GetSceneID());
	if (!data)
	{
		ErrorLogStream << PLAYERINFO(this) << "SceneDataEntry is null! id = " << GetSceneID() << LogStream::eos;
		return false;
	}

	if (data->type == SCENE_TYPE_NORMAL || 
		data->type == SCENE_TYPE_SINGLE || 
		data->type == SCENE_TYPE_PK_PREPARE || 
		data->type == SCENE_TYPE_PK ||
		data->type == SCENE_TYPE_BATTLE ||
		data->type == SCENE_TYPE_CHIJI_PREPARE)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool Player::IsInPK()
{
	SceneDataEntry* data = SceneDataEntryMgr::Instance()->FindEntry(GetScene()->GetMapID());
	if (!data)
	{
		ErrorLogStream << PLAYERINFO(this) << "SceneDataEntry is null! id = " << GetScene()->GetMapID() << LogStream::eos;
		return false;
	}

	if (data->type == SCENE_TYPE_PK)
	{
		return true;
	}
	else
	{
		return false;
	}
}


const ScenePos Player::GetScenePos() const
{
    return m_ScenePos;
}

void Player::SetScenePos(UInt32 x, UInt32 y)
{
    m_ScenePos.SetPos(x, y);
}

void Player::UpdateScenePosition()
{
   
}

void Player::AdjuestScenePosition(int &x, int &y)
{
    if (x < 0)
    {
        x = 0;
    }

    if (y < 0)
    {
        y = 0;
    }
}

void Player::OnSceneChangeReq(UInt32 curDoorId, UInt32 dstSceneId, UInt32 dstDoorId)
{
    DebugLogStream << "player(" << GetID() << ") request change scene to " << dstSceneId << " current door(" << curDoorId << ") target door(" << dstDoorId << ")." << LogStream::eos;

	CLPosition topos;
	//if (SERVER_TYPE == ST_BSCENE && dstSceneId == MAPID_CHIJI_BATTLE)
	//{
	//	InfoLogStream << "player:" << GetID() << ", count avatar and get enter position" << LogStream::eos;
	//	// 计算外观
	//	CountAvatar();
	//	// 获取位置
	//	topos = GetEnterPos();
	//	if (MAPID_CHIJI_PREPARE == GetSceneID())
	//	{
	//		// 计算宠物
	//		InfoLogStream << "player:" << GetID() << ", clear follow" << LogStream::eos;
	//		GetPetMgr().SetPetFollow(0);
	//	}
	//}

	//if (SERVER_TYPE == ST_BSCENE && dstSceneId == MAPID_CHIJI_PREPARE)
	//{
	//	BattleScene* battleScene = dynamic_cast<BattleScene*>(GetScene());
	//	if (!battleScene)
	//	{
	//		InfoLogStream << "player:" << GetID() << ", not in battle scene" << LogStream::eos;
	//		return;
	//	}

	//	//退出战场
	//	InfoLogStream << "player:" << GetID() << ", leave battle:" << GetBattleID() << LogStream::eos;
	//	battleScene->LeaveBattle(GetBattleID(), GetID());
	//	SetBabttleID(0);
	//}

	if (GetScene() && GetScene()->GetMapID() == dstSceneId)
	{
		//通知客户端加载场景了
		NotifyLoadScene();

		//同步场景对象
		SyncAroundToMe(true);

        return;
    }

    SceneDataEntry* entry = SceneDataEntryMgr::Instance()->FindEntry(dstSceneId);
    if (!entry)
    {
		InfoLogStream << "player(" << GetID() << ") want to enter unexist scene(" << dstSceneId << ")." << LogStream::eos;

		CLProtocol::SceneNotifyEnterScene notify;
		notify.result = ErrorCode::SCENE_UNEXIST;
		SendProtocol(notify);

		// 客户端一定要等这个消息，发个空的下去
		CLProtocol::SceneSyncSceneObject a;
		SendProtocol(a);

        return;
    }

	// 如果要从跨服场景去非跨服场景，就要从当前场景离开
	if (SERVER_TYPE == ST_WSCENE && !entry->IsCrossScene())
	{
		CLProtocol::CrossExitSceneReq exit;
		exit.roleId = GetID();
		Router::SendToSelfScene(GetSrcSceneNodeID(), exit);

		InfoLogStream << "player(" << GetID() << ") request exit cross scene." << LogStream::eos;
		return;
	}

	if (SERVER_TYPE == ST_BSCENE && !entry->IsBattleScene() && !entry->IsLostDungeonBattleScene())
	{
		CLProtocol::BattleExitSceneReq exit;
		exit.roleId = GetID();
		exit.battleType = BTY_NONE;
		exit.battleDead = 0;

		if (GetScene() && GetScene()->GetType() == SCENE_TYPE_BATTLE)
		{
			exit.battleType = BTY_CHIJI;
		}
		else if(GetScene() && GetScene()->GetType() == SCENE_TYPE_LOST_DUNGEON)
		{
			exit.battleType = BTY_DILAO;
			exit.result = LDBR_FAIL;
			exit.battleId = GetBattleID();

			LostBattleScene* battleScene = dynamic_cast<LostBattleScene*>(GetScene());
			if (battleScene && battleScene->IsBattleExitDoor(curDoorId)
				&& this->IsCompLostDungeonTask())
			{
				exit.result = LDBR_PASS;
			}
			this->SetLostDungLeaveTag(true);
		}
		Router::SendToSelfScene(GetSrcSceneNodeID(), exit);
		InfoLogStream << "player(" << GetID() << ") request exit battle scene." << LogStream::eos;
		return;
	}

	// 请求进入跨服场景
	if (SERVER_TYPE == ST_SCENE && entry->IsCrossScene())
	{
		Avalon::Packet* packet = Avalon::Packet::Create();
		packet->SetID(CLProtocol::CrossEnterSceneReq::__ID);
		Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
		try
		{
			stream & GetID() & GetGateID() & NODE_ID & dstSceneId;
			Encode(stream, SST_AROUND | SST_CHALLENGE, false);
			stream & ObjID_t(0);
		}
		catch (const Avalon::NetException&)
		{
			Avalon::Packet::Destroy(packet);

			CLProtocol::SceneNotifyEnterScene notify;
			notify.result = ErrorCode::SCENE_UNEXIST;
			SendProtocol(notify);

			// 客户端一定要等这个消息，发个空的下去
			CLProtocol::SceneSyncSceneObject a;
			SendProtocol(a);
			return;
		}
		packet->SetSize(packet->GetSize() + UInt16(stream.GetPos()));
		
		Router::SendToCross(packet);
		Avalon::Packet::Destroy(packet);

		InfoLogStream << "player(" << GetID() << ") request enter cross scene(" << dstSceneId << ")." << LogStream::eos;
		return;
	}

	if (SERVER_TYPE == ST_SCENE && entry->IsBattleScene())
	{
		Avalon::Packet* packet = Avalon::Packet::Create();
		packet->SetID(CLProtocol::BattleEnterSceneReq::__ID);
		Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
		try
		{
			stream & GetAccID() & GetID() & GetGateID() & NODE_ID & dstSceneId & GetChijiScore() & dstDoorId;
			Encode(stream, SST_BATTLE, false);
			stream & ObjID_t(0);
		}
		catch (const Avalon::NetException&)
		{
			Avalon::Packet::Destroy(packet);

			CLProtocol::SceneNotifyEnterScene notify;
			notify.result = ErrorCode::SCENE_UNEXIST;
			SendProtocol(notify);

			// 客户端一定要等这个消息，发个空的下去
			CLProtocol::SceneSyncSceneObject a;
			SendProtocol(a);
			return;
		}
		packet->SetSize(packet->GetSize() + UInt16(stream.GetPos()));

		Router::SendToBattle(packet);
		Avalon::Packet::Destroy(packet);

		InfoLogStream << "player(" << GetID() << ") request enter battle scene(" << dstSceneId << ")." << LogStream::eos;
		return;
	}

	// 只有城镇和城镇之间需要判断等级
	if (GetScene() && GetScene()->GetType() == SCENE_TYPE_NORMAL && entry->type == SCENE_TYPE_NORMAL)
	{
		if (SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_SCENE_LEVEL_LIMIT) && GetLevel() < entry->levelLimit)
		{
			InfoLogStream << "player(" << GetID() << ") want to enter scene(" << dstSceneId << ") cur level(" << GetLevel() << ") need level(" << entry->levelLimit << ")." << LogStream::eos;

			CLProtocol::SceneNotifyEnterScene notify;
			notify.result = ErrorCode::SCENE_LEVEL_LIMIT;
			SendProtocol(notify);

			// 客户端一定要等这个消息，发个空的下去
			CLProtocol::SceneSyncSceneObject a;
			SendProtocol(a);
			return;
		}
	}

	UInt32 realDstSceneId = dstSceneId;
	if (entry->subType == SCENE_SUB_LOST_DUNGEON_BATTLE)
	{
		realDstSceneId = this->GetLastLostBattleSceneId();
		//topos = this->GetEnterPos();
	}
	else if (entry->subType == SCENE_SUB_LOST_DUNGEON_ENTERY) //切入地下城入口场景
	{
		Scene* scene = SceneMgr::Instance()->GetDungeonEntrySceneDynamic(GetLastLostBattleSceneId(), curDoorId, dstSceneId);
		if (scene)
		{
			realDstSceneId = scene->GetID();
		}
		else
		{
			ErrorLogStream << "OnSceneChangeReq fail not find dynamic dungeon entry, player id : " << GetID() << ", battle scene id : " << GetLastLostBattleSceneId() << ", curDoorId : " << curDoorId
				<< ", dstSceneId : " << dstSceneId << LogStream::eos;
			return;
		}
	}

	if (!ChangeScene(dstSceneId, topos, GetDir(), true, dstDoorId, realDstSceneId))
	{
		DebugLogStream << "player(" << GetID() << ") request change scene to " << dstSceneId << ", realDstSceneId: " << realDstSceneId
			<< " current door(" << curDoorId << ") target door(" << dstDoorId << ") failed." << LogStream::eos;

		CLProtocol::SceneNotifyEnterScene notify;
		notify.result = ErrorCode::SCENE_CHANGE_ERROR;
		SendProtocol(notify);

		// 客户端一定要等这个消息，发个空的下去
		CLProtocol::SceneSyncSceneObject a;
		SendProtocol(a);
	}
}

void Player::RequestStopMoving(ScenePos pos)
{
    if (!IsRuning() && GetScene())
    {
        return;
    }

    SetScenePos(pos.GetX(), pos.GetY());
    SetPos(pos.GetGridePos(GetScene()->GetGridWidth(), GetScene()->GetGridHeight()));
	SetSceneDir(SceneDir());
    SetRuning(false);
    SetPosChanged(true);
    SetStatus(CREATURE_STATUS_NORMAL);

    //同步移动
    CLProtocol::SceneSyncPlayerMove syncMove;
    syncMove.id = GetID();
    syncMove.pos = GetScenePos();

    Broadcast(syncMove);
}

bool Player::CanMoveToTarget(const ScenePos& pos) const
{
    Scene* scene = GetScene();
    if (!scene)
    {
        DebugLogStream << "player(" << GetID() << ") not in scene." << LogStream::eos;
        return false;
    }

    CLPosition gridPos = pos.GetGridePos(scene->GetGridWidth(), scene->GetGridHeight());
    if (!scene->IsValidPos(gridPos))
    {
        DebugLogStream << "player(" << GetID() << ") target pos(" << pos.GetX() << ", " << pos.GetY() << ") not a valid pos." << LogStream::eos;
        return false;
    }

    /*if (scene->CheckBlock(gridPos, SCENE_BLOCK_PHYSICS))
    {
        DebugLogStream << "player(" << GetID() << ") target pos(" << pos.GetX() << ", " << pos.GetY() << ") is physics obstruct." << LogStream::eos;
        return false;
    }*/

    return true;
}

UInt64 Player::DecExp(const char* reason, UInt64 exp)
{
	UInt64 oldExp = m_Exp;
	if(m_Exp < exp)
	{
		exp = m_Exp;
		m_Exp = 0;
	}
	else
	{
		m_Exp = m_Exp - exp;
	}

	LevelDown();

	
	if(m_Exp != oldExp)
	{
		//SortListMgr::Instance()->OnSortValueChanged(SORTLIST_LEVEL, GetID(), GetName(), 0, 0, 0, GetLevel(), m_Exp);
		GameInfoLogStream("Property") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") DecExp:"
			<< reason << "|" << m_Exp + oldExp << "|" << m_Exp << LogStream::eos;
	}
	
	return exp;
}

bool Player::IsFullLevel() const
{
	PlayerDataEntry* levelDataEntry = PlayerDataEntryMgr::Instance()->FindEntry(GetLevel() + 1);
	//by huchenhui
	//return levelDataEntry == NULL && GetExp() >= m_pLevelDataEntry->upgradeexp;
	return levelDataEntry == NULL;
}

void Player::IncExp(const char* reason, UInt64 exp)
{
	if (exp == 0) return;

	SyncBaseInfoToWorld(reason, exp);

	if (m_pLevelDataEntry->upgradeexp == 0)
	{
		return;
	}
	UInt64 oldExp = GetExp();
	UInt16 oldLevel = GetLevel();

	m_Exp = m_Exp + exp;
	Levelup();

	if (m_Exp != oldExp || GetLevel() != oldLevel)
	{
		UInt32 levelExp = (UInt32)m_Exp;
		SortListMgr::Instance()->OnSortValueChanged(SORTLIST_LEVEL, GetID(), GetName(), 0, GetOccu(), 0, GetLevel(), (UInt32)GetLevel(), levelExp, 0);
		SortListType sortListType = GetLevelSortListTypeByOccu(GetOccu());
		if (sortListType != SORTLIST_INVALID)
		{
			SortListMgr::Instance()->OnSortValueChanged(sortListType, GetID(), GetName(), 0, GetOccu(), 0, GetLevel(), (UInt32)GetLevel(), levelExp, 0);
		}

		if (ActivityMgr::Instance()->IsInOpActivity(OAID_LEVEL_FIGHTING_FOR_NEW_SERVER))
		{
			SortListMgr::Instance()->OnSortValueChanged(SortListType::SORTLIST_NEW_SERVER_LEVEL, GetID(), GetName(), 0, GetOccu(), 0, GetLevel(), (UInt32)GetLevel(), (UInt32)GetExp(), 0);
		}

		//DebugLogStream << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") IncExp:" << reason << "|" << exp << "|" << m_Exp << LogStream::eos;

		SendAssetUdpLog(AssetType::ASSET_EXP, reason, oldExp, exp, exp, m_Exp);

		if (GetLevel() != oldLevel)
		{
			SendAssetUdpLog(AssetType::ASSET_LEVEL, reason, (UInt64)oldLevel, (Int64)GetLevel() - (Int64)oldLevel, (Int64)GetLevel() - (Int64)oldLevel, (UInt64)GetLevel());
		}
	}
}

UInt64 Player::GetLevelExp(UInt8 level)
{
	UInt64 sumExp = PlayerDataEntryMgr::Instance()->GetSumExp(level);
	UInt64 nowSumExp = PlayerDataEntryMgr::Instance()->GetSumExp(GetLevel() - 1) + m_Exp;
	return sumExp >= nowSumExp ? sumExp - nowSumExp : 0;
}

void Player::DecBlessExp(UInt32 exp)
{
	if(m_BlessExp > exp) m_BlessExp = m_BlessExp - exp;
	else m_BlessExp = 0;
}

void Player::IncBlessExp(UInt32 exp)
{
	m_BlessExp = m_BlessExp + exp;
	if(m_BlessExp > 4000000000) m_BlessExp = 4000000000;
}

void Player::ChangeOccu(UInt8 occu)
{
	JobDataEntry* targetJob = JobDataEntryMgr::Instance()->FindEntry(occu);
	if (targetJob == NULL)
	{
		ErrorLogStream << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") TargetOccu:" << occu << "is not exist" << LogStream::eos;
		return;
	}
	if (!targetJob->isOpen)
	{
		ErrorLogStream << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") TargetOccu:" << occu << "is not open" << LogStream::eos;
		return;
	}

	JobDataEntry* job = JobDataEntryMgr::Instance()->FindEntry(m_Occu);
	if (job == NULL)
	{
		ErrorLogStream << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") Occu:" << m_Occu << "is not exist" << LogStream::eos;
		return;
	}

	//检查需要转职的职业是否是原职业的进阶职业
	if (!job->CheckTargetOccu(occu) && SERVER_TYPE == ST_SCENE)
	{
		ErrorLogStream << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") ChangeOccu:"
			<< GetOccu() << "==>" << occu << " is not same main occu" << LogStream::eos;
		return;
	}

	//检查玩家等级.
	if (GetLevel() < TRANSFORM_OCCU_LEVEL)
	{
		ErrorLogStream << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") ChangeOccu:"
			<< GetOccu() << "==>" << occu << " level error" << LogStream::eos;
		return;
	}

	UInt8 oldOccu = GetOccu();

	//移除预转职技能
	if (m_PreOccu != occu)
	{
		ClearPreOccuSkills();
	}

	UInt8 oldPreOccu = m_PreOccu;

	//转职
	m_Occu = occu;
	m_PreOccu = 0;

	OnChangeOccu(targetJob);

	this->SetCounter(REFRESH_LEASE_SHOP, 1);

	SendChangeOccuUdpLog((UInt8)GetBaseOccu(), oldPreOccu, m_Occu);

	InfoLogStream << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") ChangeOccu:"
		<< oldOccu << "==>" << occu << " succeed!!" << LogStream::eos;

	if (SERVER_TYPE == ST_BSCENE)
	{
		// 吃鸡初始化
		{
			CountMoveSpeed();
			InfoLogStream << "player(" << PLAYERINFO(this) << ") call OnChijiBirth" << LogStream::eos;

			/*UInt64 needExp = GetLevelExp((UInt8)60);
			IncExp(GetReason(SOURCE_TYPE_GM).c_str(), needExp);*/
			SetLevel(60);

			GetSkillMgr().ClearChiJiSkill();
			GetPvpSkillMgr().ClearChiJiSkill();
			GetPetMgr().Clear();
			ScriptMgr::Instance()->GetPlayerScript()->OnChijiBirth(this);
		}
	}
}

void Player::SetVipLvl(UInt8 level)
{
	if(m_VipLvl != level)
	{
		m_VipLvl = level;
		OnVipLvlChanged();
	}
}

UInt8 Player::GetVipLvl()
{
	return m_Vip.GetLevel();
}

UInt32 Player::GetVipExp()
{
	return m_Vip.GetExp();
}

void Player::InitTotalChargeNum(UInt32 num)
{
	m_TotalChargeNum = num;
}

void Player::AddTotalChargeNum(UInt32 num)
{
	if (num == 0)
	{
		return;
	}

	UInt32 oldNum = m_TotalChargeNum;
	m_TotalChargeNum = m_TotalChargeNum + num;
	m_DayChargeNum = m_DayChargeNum + num;

	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand(GetSrcZoneId(), "t_account", GetAccGuid(), true);
	cmd->PutData("total_charge_num", m_TotalChargeNum);
	CLRecordClient::Instance()->SendCommand(cmd);

	GameInfoLogStream("Charge") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") Charge:"
		<< num << "|" << oldNum << "|" << m_TotalChargeNum << LogStream::eos;

	SyncAccountInfo(ACCOUNT_TOTAL_CHARGE_NUM, m_TotalChargeNum);
}

void Player::CheckAppointmentOccu()
{
	if (!IsAppointmentOccu())
	{
		return;
	}

	bool activityOpen = false;
	auto appointmentActivitys = ActivityMgr::Instance()->GetOpActDataByTmpTypes(OAT_APPOINTMENT_OCCU);
	for (auto activity : appointmentActivitys)
	{
		if (activity == NULL)
		{
			continue;
		}

		if (activity->state != AS_IN)
		{
			continue;
		}

		if (std::find(activity->parm2.begin(), activity->parm2.end(), GetBaseOccu()) == activity->parm2.end())
		{
			continue;
		}

		if (GetCreateTime() < activity->startTime || GetCreateTime() > activity->endTime)
		{
			continue;
		}

		activityOpen = true;
		break;
	}

	if (activityOpen)
	{
		return;
	}

	OnAppointmentActivityClose();
}

void Player::OnAppointmentActivityClose()
{
	// 设为非预约角色
	ClearAppointmentOccu();

	// 清空预约币
	RemoveAppointmentCoin(GetReason(SOURCE_TYPE_ACTIVITY_CLOSE).c_str(), GetAppointmentCoin());
}

void Player::AddAppointmentCoin(const char* reason, UInt32 num)
{
	if (reason == NULL) return;
	UInt32 oldValue = GetCounter(COUNTER_APPOINTMENT_COIN);
	UInt32 newValue = oldValue;
	if (oldValue + num > MAX_MONEY)
	{
		newValue = MAX_MONEY;
	}
	else
	{
		newValue = oldValue + num;
	}

	if (newValue > oldValue)
	{
		GameInfoLogStream("Property") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") addappointmentcoin:"
			<< reason << "|" << newValue - oldValue << "|" << newValue << LogStream::eos;

		SendAssetUdpLog(AssetType::ASSET_APPOINTMENT_COIN, reason, (UInt64)oldValue, (Int64)num, (Int64)newValue - (Int64)oldValue, (UInt64)newValue);
		SetCounter(COUNTER_APPOINTMENT_COIN, newValue);
	}
}

void Player::RemoveAppointmentCoin(const char* reason, UInt32 num)
{
	if (reason == NULL) return;
	UInt32 oldValue = GetCounter(COUNTER_APPOINTMENT_COIN);
	UInt32 newValue = oldValue;
	if (num >= oldValue) newValue = 0;
	else newValue = oldValue - num;

	if (oldValue > newValue)
	{
		GameInfoLogStream("Property") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") removeappointmentcoin:"
			<< reason << "|" << oldValue - newValue << "|" << newValue << LogStream::eos;

		SendAssetUdpLog(AssetType::ASSET_APPOINTMENT_COIN, reason, (UInt64)oldValue, (Int64)num * -1, (Int64)newValue - (Int64)oldValue, (Int64)newValue);
		SetCounter(COUNTER_APPOINTMENT_COIN, newValue);
	}
}

UInt32 Player::GetAppointmentCoin()
{
	return GetCounter(COUNTER_APPOINTMENT_COIN);
}

void Player::SummonTaskNpc(Task* task, bool notify)
{
	// 只有没完成的任务需要刷怪
	if (task->GetStatus() != TASK_UNFINISH)
	{
		return;
	}

	// 根据任务信息刷怪
	bool refresh = true;
	ObjID_t npcGuid = Avalon::StringUtil::ConvertToValue<ObjID_t>(task->GetNameVar("npc_guid"));
	if (npcGuid != 0)
	{
		// 根据之前的信息刷怪
		UInt32 dataId = task->GetVar("npc_data");
		UInt32 sceneId = task->GetVar("npc_scene");
		NpcPos pos;
		pos.x = task->GetVar("npc_x");
		pos.y = task->GetVar("npc_y");
		UInt32 dungeonId = task->GetVar("npc_dun");
		std::string name;
		Scene* scene = NULL;
		do 
		{
			auto npcData = NpcDataEntryMgr::Instance()->FindEntry(dataId);
			if (!npcData)
			{
				break;
			}
			
			name = npcData->name;

			scene = SceneMgr::Instance()->FindNormalScene(sceneId);
			if (!scene)
			{
				break;
			}

			auto monster = scene->SummonCityMonster(npcGuid, GetID(), name, CITY_MONSTER_TASK, dataId, pos, dungeonId, 1, notify);
			if (monster)
			{
				refresh = false;
			}
		} while (0);
	}
	
	if (!refresh)
	{
		return;
	}

	// 说明还没刷过，刷一只怪
	// ToDo...

	auto monsterGroups = CityMonsterGenerateDataEntryMgr::Instance()->FindMonsterSolutionByType(CITY_MONSTER_TASK);
	std::vector<CityMonsterGenerateDataEntry*> suitableGroups;
	for (auto group : monsterGroups)
	{
		if (!group)
		{
			continue;
		}

		auto scene = SceneMgr::Instance()->FindNormalScene(group->RandScene());
		if (!scene || !scene->GetDataEntry() || scene->GetDataEntry()->level > GetLevel())
		{
			continue;
		}

		suitableGroups.push_back(group);
	}

	if (suitableGroups.empty())
	{
		ErrorLogStream << PLAYERINFO(this) << " task(" << task->GetDataId() << ") summon city monster failed, no suitable monster group." << LogStream::eos;
		return;
	}

	auto group = suitableGroups[Avalon::Random::RandBetween(0, suitableGroups.size() - 1)];
	UInt32 sceneId = group->RandScene();
	auto scene = SceneMgr::Instance()->FindNormalScene(sceneId);
	if (!scene)
	{
		ErrorLogStream << PLAYERINFO(this) << " task(" << task->GetDataId() << ") summon city monster failed, invlid scene(" << sceneId << ")." << LogStream::eos;
		return;
	}

	auto monsterDataId = group->RandMonster(sceneId);
	auto npc = NpcDataEntryMgr::Instance()->FindEntry(monsterDataId);
	if (!npc)
	{
		ErrorLogStream << PLAYERINFO(this) << " task(" << task->GetDataId() << ") summon city monster failed, monster group(" << group->id << ") invalid monster(" << monsterDataId << ")." << LogStream::eos;
		return;
	}

	// 获取所有的坐标点
	auto allPos = CityMonsterPositionDataEntryMgr::Instance()->GetAllPos(sceneId, group->posGroupId);
	if (allPos.empty())
	{
		ErrorLogStream << PLAYERINFO(this) << " task(" << task->GetDataId() << ") summon city monster failed, monster group(" << group->id << ") monster(" << monsterDataId << ") no pos." << LogStream::eos;
		return;
	}

	NpcPos pos;
	{
		// 随机生成一个坐标
		UInt32 index = Avalon::Random::RandBetween(0, allPos.size() - 1);
		auto itr = allPos.begin();
		for (UInt32 i = 0; i < index; i++)
		{
			itr++;
		}

		pos = *itr;
	}

	UInt32 dungeonId = npc->dungeonId;
	UInt32 times = npc->times;
	auto monster = scene->SummonCityMonster(CLRecordClient::Instance()->GenGuid(), GetID(), npc->name, group->monsterType, monsterDataId, pos, dungeonId, times, notify);
	if (!monster)
	{
		ErrorLogStream << PLAYERINFO(this) << " task(" << task->GetDataId() << ") summon city monster failed, monster group(" << group->id << ") monster(" << monsterDataId << ")." << LogStream::eos;
		return;
	}

	task->SetNameVar("npc_guid", Avalon::StringUtil::ConvertToString(monster->GetID()).c_str());
	task->SetVar("npc_data", monster->GetDataID());
	task->SetVar("npc_scene", monster->GetSceneID());
	task->SetVar("npc_x", monster->GetPos().x);
	task->SetVar("npc_y", monster->GetPos().y);
	task->SetVar("npc_dun", monster->GetDungeonID());

	
}

void Player::OnKillCityMonster(ObjID_t raceId, const SceneNpc& cityMonster, std::vector<ObjID_t>& teammateIds)
{
	auto monster = SceneMgr::Instance()->FindCityMonster(cityMonster.guid);
	// 在结算的时候扣次数
	if (monster)
	{
		monster->OnKilled(raceId);
	}

	if (cityMonster.type == SOT_CITYMONSTER && cityMonster.funcType == CITY_MONSTER_ACTIVITY)
	{
		IncDailyCityMonsterTimes();
	}


	//m_TaskMgr.OnKillCityMonster((CityMonsterType)cityMonster.funcType, cityMonster.id, cityMonster.guid);
	m_CycleTaskMgr.OnKillCityMonster((CityMonsterType)cityMonster.funcType, cityMonster.id, cityMonster.guid);
	m_DailyTaskMgr.OnKillCityMonster((CityMonsterType)cityMonster.funcType, cityMonster.id, cityMonster.guid);
	m_MasterDailyTaskMgr.OnKillCityMonster((CityMonsterType)cityMonster.funcType, cityMonster.id, cityMonster.guid);

	if (teammateIds.size() > 0)
	{
		m_MasterAcademicGrowthTaskMgr.OnTeamKillCityMonster((CityMonsterType)cityMonster.funcType, cityMonster.id, cityMonster.guid, teammateIds);
	}
}

UInt32 Player::GetDailyCityMonsterRemainTimes()
{
	auto times = GetCounter(COUNTER_CITY_MONSTER_DAILY_TIMES);
	auto dailyCount = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_CITY_MONSTER_DAILY_COUNT);
	if (times >= dailyCount)
	{
		return 0;
	}
	else
	{
		return dailyCount - times;
	}
}

void Player::IncDailyCityMonsterTimes()
{
	/*if (GetDailyCityMonsterRemainTimes() == 0)
	{
		return;
	}*/

	IncCounter(COUNTER_CITY_MONSTER_DAILY_TIMES, 1);
	SyncCityMonsterRemainTimesToWorld();
}

UInt32 Player::GetDailyCityMonsterTimes()
{
	return GetCounter(COUNTER_CITY_MONSTER_DAILY_TIMES);
}

void Player::SyncCityMonsterRemainTimesToWorld()
{
	CLProtocol::WorldTeamQueryTargetCondRes res;
	res.roleId = GetID();
	res.teamTargetId = TeamDungeonDataEntryMgr::Instance()->GetCityMonsterTeamTargetID();
	res.remainTimes = GetDailyCityMonsterRemainTimes();
	Router::SendToWorld(res);
}

void Player::OnSysRecordDataRes(const CLProtocol::SceneSysRecordDataRes& protocol)
{
	switch ((SysRecordEvent)protocol.sysRecordEvent)
	{
	case SYS_RECORD_EVENT_NULL:
		break;
	case SYS_RECORD_EVENT_MYSTICAL_MERCHANT:
		GetShopMgr().OnSysRecordDataRes(protocol);
		break;
	default:
		break;
	}
}

void Player::SyncSysRecordData(UInt32 rareType, UInt32 parm1, int num)
{
	CLProtocol::SceneSysRecordDataSync protocol;
	protocol.ownerId = GetID();
	protocol.type = rareType;
	protocol.param1 = parm1;
	protocol.addNum = num;

	Router::SendToWorld(protocol);
}

void Player::RequestSysRecordData(UInt8 sysRecordEvent, UInt8 rareControlType, const std::vector<UInt32>& params, const std::vector<UInt32>& extraParam)
{
	CLProtocol::SceneSysRecordDataReq protocol;
	protocol.ownerId = GetID();
	protocol.sysRecordEvent = sysRecordEvent;
	protocol.rareControlType = rareControlType;
	protocol.params = params;
	protocol.extraParam = extraParam;

	Router::SendToWorld(protocol);
}

void Player::SyncBaseInfoToWorld()
{
	CLProtocol::WorldSyncPlayerBaseInfo sync;
	sync.playerId = GetID();
	sync.oldLevel = 0;
	sync.level = GetLevel();
	sync.vip = GetVipLvl();
	sync.vipExp = GetVipExp();
	sync.point = GetPoint();
	sync.occu = GetOccu();
	sync.awaken = GetAwaken();
	sync.activeDegree = GetDailyTaskScore();
	sync.totalChargeNum = m_TotalChargeNum;
	sync.monthCardExpireTime = m_MonthCardExpireTime;
	sync.seasonLv = (UInt32)m_SeasonLevel.GetSeasonLevel();
	sync.auctionRefreshTime = m_AuctionRefreshTime;
	sync.auctionAdditionBooth = m_AuctionAdditionBooth;
	sync.totalPlayerChargeNum = m_TotleChargeNum;
	sync.moneyManageStatus = m_MoneyManageStatus;
	sync.headFrame = GetHeadFrameId();
	Router::SendToWorld(sync);
}

void Player::SyncBaseInfoToWorld(UInt16 oldLv)
{
	CLProtocol::WorldSyncPlayerBaseInfo sync;
	sync.playerId = GetID();
	sync.oldLevel = oldLv;
	sync.level = GetLevel();
	sync.vip = GetVipLvl();
	sync.vipExp = GetVipExp();
	sync.point = GetPoint();
	sync.occu = GetOccu();
	sync.awaken = GetAwaken();
	sync.activeDegree = GetDailyTaskScore();
	sync.totalChargeNum = m_TotalChargeNum;
	sync.monthCardExpireTime = m_MonthCardExpireTime;
	sync.seasonLv = (UInt32)m_SeasonLevel.GetSeasonLevel();
	sync.auctionRefreshTime = m_AuctionRefreshTime;
	sync.auctionAdditionBooth = m_AuctionAdditionBooth;
	sync.totalPlayerChargeNum = m_TotleChargeNum;
	Router::SendToWorld(sync);
}

void Player::SyncBaseInfoToWorld(const char* incExpReason, UInt64 incExp)
{
	CLProtocol::WorldSyncPlayerBaseInfo sync;
	sync.playerId = GetID();
	sync.oldLevel = 0;
	sync.level = GetLevel();
	sync.vip = GetVipLvl();
	sync.vipExp = GetVipExp();
	sync.point = GetPoint();
	sync.occu = GetOccu();
	sync.awaken = GetAwaken();
	sync.activeDegree = GetDailyTaskScore();
	sync.totalChargeNum = m_TotalChargeNum;
	sync.monthCardExpireTime = m_MonthCardExpireTime;
	sync.seasonLv = (UInt32)m_SeasonLevel.GetSeasonLevel();
	sync.auctionRefreshTime = m_AuctionRefreshTime;
	sync.auctionAdditionBooth = m_AuctionAdditionBooth;
	sync.totalPlayerChargeNum = m_TotleChargeNum;
	sync.moneyManageStatus = m_MoneyManageStatus;
	sync.expInfo.reason = incExpReason;
	sync.expInfo.exp = GetExp();
	sync.expInfo.incExp = incExp;
	Router::SendToWorld(sync);
}

void Player::SyncBaseInfoToGate()
{
	CLProtocol::GateSyncPlayerBaseInfo sync;
	sync.playerId = GetID();
	sync.level = GetLevel();
	SendToGate(sync);
}

bool Player::LoadItem(Item* item, bool newPos)
{
	return m_ItemMgr.LoadItem(item, newPos);
}

UInt32 Player::GetItemNum(UInt32 id)
{
	ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(id);
	if (dataEntry == NULL) return 0;

	if (dataEntry->type == ITEM_INCOME)
	{
		return GetIncome(dataEntry->subType);
	}

	return m_ItemMgr.GetItemNum(id, dataEntry->type, dataEntry->owner);
}

UInt32 Player::GetItemNumWithEqualItem(UInt32 id)
{
	// 先在等价关系表中查找有没有等价的物品
	std::vector<UInt32> itemIds;
	auto equalItemData = EqualItemDataEntryMgr::Instance()->FindEntry(id);
	if (equalItemData)
	{
		itemIds = equalItemData->equalItemIds;
	}
	itemIds.push_back(id);

	UInt32 num = 0;
	for (auto id : itemIds)
	{
		ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(id);
		if (dataEntry == NULL)
		{
			continue;
		}

		if (dataEntry->type == ITEM_INCOME)
		{
			return GetIncome(dataEntry->subType);
			continue;
		}

		num += m_ItemMgr.GetItemNum(id, dataEntry->type, dataEntry->owner);
	}

	return num;
}

bool Player::RemoveItem(const char* reason, UInt32 id, UInt32 num)
{
	if (reason == NULL) return false;
	ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(id);
	if (dataEntry == NULL) return false;

	UInt32 oldNum = GetItemNum(id);
	UInt32 newNum = 0;

	//收益货币
	if (dataEntry->type == ITEM_INCOME)
	{
		if (!RemoveMoney(reason, (ItemSubType)dataEntry->subType, num))
		{
			return false;
		}
		
		newNum = GetItemNum(id);

		if (newNum + num != oldNum)
		{
			ErrorLogStream << PLAYERINFO(this) << " remove item num error! reason:" << reason << " id:" << id << " needRmNum:" << num << " realRmNum:" << newNum - oldNum << LogStream::eos;
			return false;
		}
		
		return true;
	}

	// 吃鸡开宝箱被打断
	BattleScene* battleScene = dynamic_cast<BattleScene*>(this->GetScene());
	if (battleScene)
	{
		battleScene->OnOpenBoxBeInterrupted(this->GetBattleID(), this->GetID());
	}

	UInt8 pack = dataEntry->type;
	Package* pPack = m_ItemMgr.GetPackage(m_ItemMgr.GetPackType((ItemType)pack));

	UInt32 rmNum = m_ItemMgr.RemoveItem(reason, id, num, m_ItemMgr.GetPackType((ItemType)pack));

	if (rmNum != num)
	{
		ErrorLogStream << PLAYERINFO(this) << " remove item num error! reason:" << reason << " id:" << id << " needRmNum:" << num << " realRmNum:" << rmNum << LogStream::eos;
		return false;
	}

	if (rmNum > 0)
	{
		OnCostItem(reason, dataEntry, 0, (UInt16)rmNum, (UInt16)oldNum);
		if (pPack != NULL) pPack->OnItemChanged(this);

		return true;
	}

	return false;
}

Int32 Player::RemoveItemWithEqualItem(const char* reason, UInt32 id, UInt32 num)
{
	if (reason == NULL) return false;

	// 先在等价关系表中查找有没有等价的物品，优先消耗等价物表中的道具
	std::vector<UInt32> itemIds;
	auto equalItemData = EqualItemDataEntryMgr::Instance()->FindEntry(id);
	if (equalItemData)
	{
		itemIds = equalItemData->equalItemIds;
	}
	itemIds.push_back(id);

	UInt32 removeSuccessNum = 0;

	for (auto id : itemIds)
	{
		if (num == 0)
		{
			break;
		}

		ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(id);
		if (dataEntry == NULL)
		{
			continue;
		}

		//收益货币
		if (dataEntry->type == ITEM_INCOME)
		{
			if (!RemoveMoney(reason, (ItemSubType)dataEntry->subType, num))
			{
				return -1;
			}
		}

		UInt8 pack = dataEntry->type;
		Package* pPack = m_ItemMgr.GetPackage(m_ItemMgr.GetPackType((ItemType)pack));

		UInt16 oldNum = GetItemNum(id);
		UInt16 removeNum = m_ItemMgr.RemoveItem(reason, id, num, m_ItemMgr.GetPackType((ItemType)pack));

		if (removeNum > 0)
		{
			OnCostItem(reason, dataEntry, 0, removeNum, oldNum);
			if (pPack != NULL)
			{
				pPack->OnItemChanged(this);
			}
		}

		num -= removeNum;
		removeSuccessNum += removeNum;
	}

	return (Int32)removeSuccessNum;
}

UInt32 Player::GetBindItemNum(UInt32 id)
{

	ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(id);
	if(dataEntry == NULL) return 0;

	if (1 == dataEntry->maxNum)
		return 1;

	return m_ItemMgr.GetItemNum(id, dataEntry->type, 2);
}

UInt32 Player::GetNoBindItemNum(UInt32 id)
{

	ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(id);
	if(dataEntry == NULL) return 0;

	if (1 == dataEntry->maxNum)
		return 1;

	return m_ItemMgr.GetItemNum(id, dataEntry->type, 1);
}

void Player::RemoveNoBindItem(const char* reason, UInt32 id,UInt32 num)
{
	if(reason == NULL) return;
	ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(id);
	if(dataEntry == NULL) return;

	UInt8 pack = dataEntry->type;
	Package* pPack = m_ItemMgr.GetPackage(pack);

	UInt16 oldNum = GetItemNum(id);
	num = m_ItemMgr.RemoveItem(reason, id, num, pack);

	if(num > 0)
	{
		OnCostItem(reason, dataEntry, 0, num, oldNum);

		if(pPack != NULL) pPack->OnItemChanged(this);
	}
}

bool Player::RemoveLessScoreItem(const char* reason, UInt32 id, UInt32 num)
{
	if (reason == NULL) return false;
	ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(id);
	if (dataEntry == NULL) return false;

	UInt32 oldNum = GetItemNum(id);
	if (dataEntry->type == ITEM_INCOME)
	{
		ErrorLogStream << PLAYERINFO(this) << "Item type(" << dataEntry->type << ") is error!" << LogStream::eos;
		return false;
	}

	UInt8 pack = dataEntry->type;
	Package* pPack = m_ItemMgr.GetPackage(m_ItemMgr.GetPackType((ItemType)pack));

	UInt32 rmNum = m_ItemMgr.RemoveLessScoreItem(reason, id, num, m_ItemMgr.GetPackType((ItemType)pack));

	if (rmNum != num)
	{
		ErrorLogStream << PLAYERINFO(this) << " remove item num error! reason:" << reason << " id:" << id << " needRmNum:" << num << " realRmNum:" << rmNum << LogStream::eos;
		return false;
	}

	if (rmNum > 0)
	{
		OnCostItem(reason, dataEntry, 0, (UInt16)rmNum, (UInt16)oldNum);
		if (pPack != NULL) pPack->OnItemChanged(this);

		return true;
	}

	return false;
}

void Player::RemoveItemDirect(const char* reason, Item* item)
{
	if(reason == NULL || item == NULL) return;
	Package* pPack = m_ItemMgr.GetPackage(item->GetPos().pack);

	UInt16 oldNum = item->GetNum();
	if(m_ItemMgr.RemoveItem(item))
	{
		OnRemoveItem(reason, item, 1, 1);
		OnCostItem(reason, NULL, item->GetStrengthen(), item->GetNum(), oldNum, item);

		if(pPack != NULL) pPack->OnItemChanged(this);
		Item::DestroyItem(item);
	}
}

bool Player::CheckAddMoneyItem(UInt8 type, UInt32 num)
{
	if(num == 0) return true;

	switch(type)
	{
	case ST_ITEM_GOLD:
		{
			if(m_Gold + UInt64(num) >= MAX_MONEY) return false;
		}
		break;
	case ST_ITEM_POINT:
		{
			if(m_Point + UInt64(num) >= MAX_MONEY) return false;
		}
		break;
	case ST_WARRIOR_SOUL:
		{
			if (m_WarriorSoul + UInt64(num) >= MAX_MONEY) return false;
		}
		break;
	case ST_PK_COIN:
		{
			if (m_PkCoin + UInt64(num) >= MAX_MONEY) return false;
		}
		break;
	case ST_REVIVE_COIN:
		{
			if (m_reviveCoin + UInt64(num) >= MAX_MONEY) return false;
		}
		break;
	case ST_BIND_GOLD:
		{
			if (m_BindGold + UInt64(num) >= MAX_MONEY) return false;
		}
		break;
	case ST_BIND_POINT:
		{
			if (m_BindPoint + UInt64(num) >= MAX_MONEY) return false;
		}
		break;
	case ST_GUILD_CONTRI:
		{
			if (m_GuildContri + UInt64(num) >= MAX_MONEY) return false;
		}
		break;
	case ST_SP:
		{
			if (m_SP + UInt64(num) >= MAX_SP) return false;
		}
		break;
	case ST_MAGJAR_POINT:
		{
			if (m_MagJarPoint + UInt64(num) >= MAX_MONEY)	return false;
		}
		break;
	case ST_GOLDJAR_POINT:
		{
			if (m_GoldJarPoint + UInt64(num) >= MAX_MONEY)	return false;
		}
		break;
	case ST_ITEM_EXP:
	case ST_VIP_EXP:
	case ST_CHIJI_SHOP_COIN:
		return true;
    break;
	case ST_APPOINTMENT_COIN:
	{
		if (m_AppointmentOccu + UInt64(num) >= MAX_MONEY) return false;
	}
		break;
	case ST_DRESS_INTEGRAL_VALUE:
	{
		if (GetItemNum(600002533) + UInt64(num) >= MAX_MONEY) return false;
	}
		break;
	case ST_WEAPON_LEASE_TICKET:
	{
		if (GetWeaponLeaseTickets() + UInt64(num) >= MAX_MONEY) return false;
	}
		break;
	default:
		return false;
	}
	return true;
}

void Player::AddMoneyItem(const char *reason, UInt8 type, UInt32 num)
{
	switch(type)
	{
	case ST_ITEM_GOLD:
		AddGold(reason, num);
		break;
	case ST_ITEM_POINT:
		AddPoint(reason, num);
		break;
	case ST_BIND_GOLD:
		AddBindGold(reason, num);
		break;
	case ST_BIND_POINT:
		AddBindPoint(reason, num);
		break;
	case ST_ITEM_EXP:
		IncExp(reason, num);
		break;
	case ST_WARRIOR_SOUL:
        AddWarriorSoul(reason, num);
        break;
	case ST_PK_COIN:
		AddPkCoin(reason, num);
		break;
	case ST_REVIVE_COIN:
		AddReviveCoin(reason, num);
		break;
	case ST_GUILD_CONTRI:
		AddGuildContribution(reason, num, true);
		break;
	case ST_SP:
		AddSP(reason, num);
		break;
	case ST_VIP_EXP:
		_OnCharge(reason, num, num);
		break;
	case ST_GOLDJAR_POINT:
		AddGoldJarPoint(reason, num);
		break;
	case ST_MAGJAR_POINT:
		AddMagJarPoint(reason, num);
		break;
	case ST_APPOINTMENT_COIN:
		AddAppointmentCoin(reason, num);
		break;
	case ST_MASTER_ACADEMIC_VALUE:
		AddMasterSysAcademicGrowth(reason, num);
		break;
	case ST_MASTER_GOODTEACH_VALUE:
		AddMasterSysGoodTeachValue(reason, num);
		break;
	case ST_DRESS_INTEGRAL_VALUE:
		AddChangeFashionScore(reason, num);
		break;
	case ST_WEAPON_LEASE_TICKET:
		AddWeaponLeaseTickets(reason, num);
		break;
	case ST_CHIJI_SHOP_COIN:
		IncCounter(CHIJI_SHOP_COIN_NAME, num);
		break;
	default:
		break;
	}
}

bool Player::CheckAddItem(UInt32 id, UInt16 num)
{
	ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(id);
	if(dataEntry == NULL) return false;
	if (1 == dataEntry->isAbandon) return false;
	
	if(ITEM_INCOME == dataEntry->type)
	{
		return CheckAddMoneyItem(dataEntry->subType, num);
	}
	else if (ITEM_HEAD_FRAME == dataEntry->type)
	{
		return true;
	}
	else if (ITEM_NEWTITLE == dataEntry->type)
	{
		return true;
	}
	return m_ItemMgr.CheckAddItem(id, num, (ItemType)dataEntry->type, 0);
}

UInt16 Player::AddItem(const char* reason, UInt32 id, UInt32 num, UInt8 qualityLv, UInt8 strenth, bool getNotify)
{
	if(reason == NULL) return 0;
	ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(id);
	if(dataEntry == NULL) return 0;
	if (dataEntry->isAbandon == 1)	return 0;

	if(ITEM_INCOME == dataEntry->type)
	{
		AddMoneyItem(reason, dataEntry->subType, num);

		if (getNotify){
			CLProtocol::SceneNotifyGetItem notify;
			ItemReward item;
			item.id = id;
			item.num = num;
			notify.items.push_back(item);
			notify.sourceType = 0;
			ReasonInfo ri;
			if (ParseReason(reason, ri))
			{
				notify.sourceType = ri.type;
			}
			SendProtocol(notify);
		}
		
		return num;
	}
	else if (ITEM_HEAD_FRAME == dataEntry->type)
	{
		GetHeadFrameMgr().AddHeadFrame(dataEntry->id, dataEntry->time);
		SendItemUdpLog(reason, id, id, ItemType(dataEntry->type), ItemSubType(dataEntry->subType), dataEntry->name, 0, 1, 0);
		return 1;
	}
	else if (ITEM_NEWTITLE == dataEntry->type)
	{
		UInt32 dueTm = 0;
		if (dataEntry->time > 0)
		{
			dueTm = UInt32(CURRENT_TIME.Sec()) + dataEntry->time;
		}
		
		this->AddNewTitle(dataEntry->newTitleId, dueTm);
		return 1;
	}
	else if (ST_CHIJI_SKILL == dataEntry->subType)
	{
		GetPvpSkillMgr().SendChiJiChoiceSkill();
		return 1;
	}
	//if(dataEntry->bindtype == BIND_TYPE_ONGET) bind = 1;
	UInt8 pack = m_ItemMgr.GetPackType((ItemType)dataEntry->type);
	Package* pPack = m_ItemMgr.GetPackage(pack);

	
	UInt16 oldNum = GetItemNum(id);

	num = m_ItemMgr.AddItem(reason, id, num, qualityLv, strenth, (ItemType)dataEntry->type, 0);
	if(num > 0)
	{
		OnGetItem(reason, dataEntry, qualityLv, strenth, num, oldNum, NULL, getNotify);

		if(pPack != NULL) pPack->OnItemChanged(this);
	}
	return num;
}

UInt16 Player::AddItemWithCallBack(const char* reason, UInt32 id, UInt32 num,
	const std::function<bool(Item&)>& initCallback,
	const std::function<void(Item&)>& makedCallback,
	UInt8 qualityLv, UInt8 strenth, bool getNotify)
{
	if (reason == NULL) return 0;
	ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(id);
	if (dataEntry == NULL) return 0;
	if (dataEntry->isAbandon == 1)	return 0;

	if (ITEM_INCOME == dataEntry->type)
	{
		AddMoneyItem(reason, dataEntry->subType, num);

		if (getNotify){
			CLProtocol::SceneNotifyGetItem notify;
			ItemReward item;
			item.id = id;
			item.num = num;
			notify.items.push_back(item);
			notify.sourceType = 0;
			ReasonInfo ri;
			if (ParseReason(reason, ri))
			{
				notify.sourceType = ri.type;
			}
			SendProtocol(notify);
		}

		return num;
	}

	//if(dataEntry->bindtype == BIND_TYPE_ONGET) bind = 1;
	UInt8 pack = m_ItemMgr.GetPackType((ItemType)dataEntry->type);
	Package* pPack = m_ItemMgr.GetPackage(pack);


	UInt16 oldNum = GetItemNum(id);

	num = m_ItemMgr.AddItemWithCallBack(reason, id, num, qualityLv, strenth, (ItemType)dataEntry->type, initCallback, makedCallback);
	if (num > 0)
	{
		OnGetItem(reason, dataEntry, qualityLv, strenth, num, oldNum, NULL, getNotify);

		if (pPack != NULL) pPack->OnItemChanged(this);
	}
	return num;
}

UInt16 Player::AddItemDirect(const char* reason, Item* item, bool notify, bool mailGive)
{
	if (item == NULL || item->GetNum() == 0)
	{
		return 0;
	}

	if (reason == NULL)
	{
		if (mailGive)
		{
			ErrorLogStream << "player(" << PLAYERINFO(this) << ") AddItemDirect item(" << item->GetID() << "|" << item->GetDataID() << ")." << LogStream::eos;
		}
		return 0;
	}

	if(ITEM_INCOME == item->GetType())
	{
		UInt32 num = item->GetNum();
		AddMoneyItem(reason, item->GetSubType(), num);

		Item::DestroyItem(item);
		return num;
	}

	UInt8	uBind = item->GetBind();
	UInt8	uStrengthen = item->GetStrengthen();
	item->ClearDirty();

	PackType pack = m_ItemMgr.GetPackType((ItemType)item->GetType());
	Package* pPack = m_ItemMgr.GetPackage(pack);
	UInt16 oldNum = item->GetNum();
	ItemDataEntry* itemData = item->GetDataEntry();
	UInt16 num = m_ItemMgr.AddItem(reason, item, pack);
	if (num > 0)
	{
		OnGetItem(reason, itemData, uBind, uStrengthen, num, oldNum, NULL, notify, mailGive);

		if(pPack != NULL) pPack->OnItemChanged(this);
	}
	else
	{
		if (mailGive)
		{
			ErrorLogStream << "player(" << PLAYERINFO(this) << ") AddItemDirect item(" << itemData->id << ") num(" << num << ")." << LogStream::eos;
		}
	}
	return num;
}

void Player::EquipItem(UInt32 id)
{
	if (SERVER_TYPE == ST_BSCENE)
	{
		DebugLogStream << "player(" << PLAYERINFO(this) << ") equip item(" << id << ")." << LogStream::eos;
	}

	m_ItemMgr.EquipItem(id);
	CountAvatar();
}

RewardGroup* Player::GetRewardGroupNoClear()
{
	return &m_RewardGroup;
}

RewardGroup* Player::GetRewardGroup()
{
	m_RewardGroup.ClearRewards();
	return &m_RewardGroup;
}

UInt32 Player::CheckAddRewards(RewardGroup* group, Item* srcItem, UInt16 srcNum)
{
	if (group == NULL) return ITEM_DATA_INVALID;

	UInt16 realSrcNum = 0;
	UInt8 srcType = 0;
	if (srcItem != NULL && srcNum > 0)
	{
		if (srcItem->GetNum() <= srcNum)
			realSrcNum = 1;
		srcType = srcItem->GetType();
	}

	UInt32 taskGridUsed = 0;
	UInt32 equipGridUsed = 0;
	UInt32 matGridUsed = 0;
	UInt32 expendGridUsed = 0;
	UInt32 fashionGridUsed = 0;
	UInt32 titleGridUsed = 0;
	UInt32 energyGridUsed = 0;

	Int16 gridUsed = 0;
	const std::vector<ItemReward>& rewards = group->GetRewards();

	//合并奖励
	std::vector<ItemReward> mergeRewards;
	for (std::vector<ItemReward>::const_iterator iter = rewards.begin();
		iter != rewards.end(); ++iter)
	{
		bool bFind = false;
		for (int i = 0; i < (int)mergeRewards.size(); ++i)
		{
			if (iter->id == mergeRewards[i].id)
			{
				bFind = true;

				ItemDataEntry* data = ItemDataEntryMgr::Instance()->FindEntry(iter->id);
				if (!data)
					continue;

				if (data->maxNum == mergeRewards[i].num)
				{
					bFind = false;
					continue;
				}

				if (data->type == ITEM_INCOME)
				{
					mergeRewards[i].num += iter->num;
					break;
				}

				if ((iter->auctionCoolTimeStamp > CURRENT_TIME.Sec() || mergeRewards[i].auctionCoolTimeStamp > CURRENT_TIME.Sec())
					&& iter->auctionCoolTimeStamp != mergeRewards[i].auctionCoolTimeStamp)
				{
					bFind = false;
					continue;
				}

				if (data->maxNum >= iter->num + mergeRewards[i].num)
				{
					mergeRewards[i].num += iter->num;
				}
				else
				{
					UInt8 restNum = (iter->num + mergeRewards[i].num) % data->maxNum;
					UInt8 grid = (iter->num + mergeRewards[i].num) / data->maxNum;
					mergeRewards[i].num = data->maxNum;
					ItemReward newReward = *iter;
					newReward.num = restNum;
					mergeRewards.push_back(newReward);
					while (grid > 1)
					{
						ItemReward newReward = *iter;
						newReward.num = data->maxNum;

						grid -= 1;
						mergeRewards.push_back(newReward);
					}

				}

				break;
			}
		}

		if (!bFind)
			mergeRewards.push_back(*iter);
	}

	for (std::vector<ItemReward>::const_iterator iter = mergeRewards.begin();
		iter != mergeRewards.end(); ++iter)
	{
		ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(iter->id);
		if (dataEntry == NULL)
		{
			SendNotify("item data is null, key={0}", iter->id);
			return ITEM_DATA_INVALID;
		}
		
		if (1 == dataEntry->isAbandon)
		{
			return ITEM_ABANDON;
		}

		if (ITEM_INCOME != dataEntry->type && ITEM_HEAD_FRAME != dataEntry->type && ITEM_NEWTITLE != dataEntry->type)
		{
			gridUsed = m_ItemMgr.GetGridUsed(iter->id, iter->num, iter->auctionCoolTimeStamp);
		}

		if (gridUsed < 0)
			return ITEM_ADD_PACK_FULL;

		switch (dataEntry->type)
		{
		case ITEM_EQUIP:			
		{
			if (0 == m_ItemMgr.GetPackage(PACK_EQUIP)->GetCapacity())
				return ITEM_ADD_PACK_FULL;

			if (srcType == ITEM_EQUIP && gridUsed > 0)
				equipGridUsed += gridUsed - realSrcNum;
			else
				equipGridUsed += gridUsed;
			break;
		}
		case ITEM_MATERIAL:			
		{
			if (0 == m_ItemMgr.GetPackage(PACK_MATERIAL)->GetCapacity())
				return ITEM_ADD_PACK_FULL;

			if (srcType == ITEM_MATERIAL && gridUsed > 0)
				matGridUsed += gridUsed - realSrcNum;
			else
				matGridUsed += gridUsed;
			break;
		}
		case ITEM_EXPENDABLE:		
		{	
			if (0 == m_ItemMgr.GetPackage(PACK_EXPENDABLE)->GetCapacity())
				return ITEM_ADD_PACK_FULL;

			if (srcType == ITEM_EXPENDABLE && gridUsed > 0)
				expendGridUsed += gridUsed - realSrcNum;
			else
				expendGridUsed += gridUsed;
			break;
		}
		case ITEM_TASK:				
		{
			if (0 == m_ItemMgr.GetPackage(PACK_TASK)->GetCapacity())
				return ITEM_ADD_PACK_FULL;

			if (srcType == ITEM_TASK && gridUsed > 0)
				taskGridUsed += gridUsed - realSrcNum;
			else
				taskGridUsed += gridUsed;
			break;
		}
		case ITEM_FASHION:			
		{
			if (0 == m_ItemMgr.GetPackage(PACK_FASHION)->GetCapacity())
				return ITEM_ADD_PACK_FULL;

			if (srcType == ITEM_FASHION && gridUsed > 0)
				fashionGridUsed += gridUsed - realSrcNum;
			else
				fashionGridUsed += gridUsed;
			break;
		}
		case ITEM_TITLE:
		{
			if (0 == m_ItemMgr.GetPackage(PACK_TITLE)->GetCapacity())
				return ITEM_ADD_PACK_FULL;

			if (srcType == ITEM_TITLE && gridUsed > 0)
				titleGridUsed += gridUsed - realSrcNum;
			else
				titleGridUsed += gridUsed;
			break;
		}
		case ITEM_ENERGY_STONE:
		{
			if (0 == m_ItemMgr.GetPackage(PACK_ENERGY_STONE)->GetCapacity())
				return ITEM_ADD_PACK_FULL;

			if (srcType == ITEM_ENERGY_STONE && gridUsed > 0)
				energyGridUsed += gridUsed - realSrcNum;
			else
				energyGridUsed += gridUsed;
			break;
		}
		case ITEM_INCOME:
		{
			if (!CheckAddMoneyItem(dataEntry->subType, iter->num))	return ITEM_MONEY_ADD_FULL;
			break;
		}
		default:
			break;
		}
	}

	bool canAdd = (m_ItemMgr.GetPackage(PACK_EQUIP)->GetCapacity() >= equipGridUsed
		&& m_ItemMgr.GetPackage(PACK_MATERIAL)->GetCapacity() >= matGridUsed
		&& m_ItemMgr.GetPackage(PACK_EXPENDABLE)->GetCapacity() >= expendGridUsed
		&& m_ItemMgr.GetPackage(PACK_TASK)->GetCapacity() >= taskGridUsed
		&& m_ItemMgr.GetPackage(PACK_FASHION)->GetCapacity() >= fashionGridUsed
		&& m_ItemMgr.GetPackage(PACK_TITLE)->GetCapacity() >= titleGridUsed
		&& m_ItemMgr.GetPackage(PACK_ENERGY_STONE)->GetCapacity() >= energyGridUsed);

	if (!canAdd)
		return ITEM_ADD_PACK_FULL;
	else
		return SUCCESS;
}

UInt32 Player::CheckAddLimitItems(RewardGroup* group)
{
	if (group == NULL) return ITEM_DATA_INVALID;

	const std::vector<ItemReward>& rewards = group->GetRewards();

	std::vector<ItemReward> mergeItems;
	for (std::vector<ItemReward>::const_iterator iter = rewards.begin();
		iter != rewards.end(); ++iter)
	{
		ItemDataEntry* data = ItemDataEntryMgr::Instance()->FindEntry(iter->id);
		if (!data)
		{
			continue;
		}

		if (data->getLimitNum > 0)
		{
			for (UInt32 i = 0; i < mergeItems.size(); ++i)
			{
				if (mergeItems[i].id == data->id)
				{
					mergeItems[i].num += iter->num;
					if (GetItemNum(data->id) + mergeItems[i].num > data->getLimitNum)
					{
						return ITEM_ADD_PACK_FULL;
					}
				}
			}

			mergeItems.push_back(*iter);
			if (GetItemNum(data->id) + iter->num > data->getLimitNum)
			{
				return ITEM_ADD_PACK_FULL;
			}
		}
	}

	return SUCCESS;
}

void Player::AddRewardsBase(const char* reason, RewardGroup* group, bool notice, UInt32 validDay, bool sendMail, bool checkGetLimit, bool openJar, bool mailGive, bool notifyNewItem)
{
	if(reason == NULL) return;
	if(group == NULL) return;

	/*RewardGroup mailGroup;*/
	std::vector<Item*> mailGroup;

	const std::vector<ItemReward> rewards = group->GetRewards();

	std::vector<ItemReward> filterItems;
	//是否判断获得限制道具
	if (checkGetLimit)
	{
		for (size_t i = 0; i < rewards.size(); ++i)
		{
			ItemDataEntry* itemData = ItemDataEntryMgr::Instance()->FindEntry(rewards[i].id);
			if (!itemData)
			{
				continue;
			}

			if (itemData->getLimitNum > 0 && GetItemNum(rewards[i].id) + rewards[i].num > itemData->getLimitNum)
			{
				filterItems.push_back(rewards[i]);
			}
		}
	}

	std::vector<ItemReward> mergeRewards;
	
	//合并奖励
	m_ItemMgr.MergeItems(rewards, mergeRewards);

	std::string strSenderName;
	std::string	strTitle;
	std::string	strContent;
	GetMailInfoByReason(reason, strSenderName, strTitle, strContent);

	// DebugLogStream << PLAYERINFO(this) << "AddItemReward reason:" << reason << LogStream::eos;

	for(std::vector<ItemReward>::iterator iter = mergeRewards.begin();
		iter != mergeRewards.end(); ++iter)
	{
		bool isFilter = false;
		for (size_t i = 0; i < filterItems.size(); ++i)
		{
			if (filterItems[i].id == iter->id)
			{
				isFilter = true;
				break;
			}
		}

		if (isFilter)
		{
			continue;
		}

		ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(iter->id);
		if(dataEntry == NULL) continue;

#ifdef _DEBUG
		DebugLogStream << PLAYERINFO(this) << "AddItem id:" << iter->id << ",num:" << iter->num << LogStream::eos;
#endif

		if(dataEntry->type == ITEM_INCOME)
		{
			AddMoneyItem(reason, dataEntry->subType, iter->num);
			continue;
		}
		else if (ITEM_HEAD_FRAME == dataEntry->type)
		{
			GetHeadFrameMgr().AddHeadFrame(dataEntry->id, dataEntry->time);
			SendItemUdpLog(reason, dataEntry->id, dataEntry->id, ItemType(dataEntry->type), ItemSubType(dataEntry->subType), dataEntry->name, 0, 1, 0);
			continue;
		}
		else if (ITEM_NEWTITLE == dataEntry->type)
		{
			UInt32 dueTm = 0;
			if (dataEntry->time > 0)
			{
				dueTm = UInt32(CURRENT_TIME.Sec()) + dataEntry->time;
			}
			this->AddNewTitle(dataEntry->newTitleId, dueTm);
			continue;
		}

		UInt16 num = m_ItemMgr.AddItem(reason, iter->id, iter->num, iter->qualityLv, iter->strenth, (ItemType)dataEntry->type, iter->auctionCoolTimeStamp, notifyNewItem);
		if (num < iter->num && sendMail)
		{
			UInt16 remainNum = iter->num - num;
			while (remainNum > 0)
			{
				// 超出发邮件
				Item* item = ItemMaker::Instance()->MakeItem(this, iter->id, remainNum, 0, iter->strenth, iter->auctionCoolTimeStamp);
				if (!item)
				{
					ErrorLogStream << PLAYERINFO(this) << " make item error! id = " << iter->id << LogStream::eos;
					break;
				}
				mailGroup.push_back(item);

				//通知获得新道具
				GetNewItemParam param;
				param.openJar = openJar;
				param.itemId = item->GetID();
				param._newItem = (void*)item;

				CLEventMgr::Instance()->SendPlayerEvent(GetID(), CLEventMgr::ET_NOTIFY_NEWITEM, &param);

				if (mailGroup.size() == MAX_MAILITEM_NUM)
				{
					Sys::SendSysMail(GetID(), strSenderName.c_str(), strTitle.c_str(), strContent.c_str(), reason, validDay, NULL, mailGroup);
					for (auto item : mailGroup)
					{
						CL_SAFE_DELETE(item);
					}

					mailGroup.clear();
				}

				// 剩余的继续发邮件
				remainNum = remainNum - item->GetNum();
			}
		}
		else
		{
			// 添加完成
			//OnGetItem(reason, dataEntry, iter->bind, iter->strenth, num, oldNum);
			m_TaskMgr.OnGetItem(dataEntry->id, dataEntry->color, iter->strenth, num);
			m_DailyTaskMgr.OnGetItem(dataEntry->id, dataEntry->color, iter->strenth, num);
			m_AchievementTaskMgr.OnGetItem(dataEntry->id, dataEntry->color, iter->strenth, num);
			m_LegendTaskMgr.OnGetItem(dataEntry->id, dataEntry->color, iter->strenth, num);
			m_LostDungeonTaskMgr.OnCollectItem(dataEntry->id, num);
		}
	}

	// 通知客户端
	if (notice)
	{
		RewardGroup newGroup;
		newGroup.AddRewards(mergeRewards);
		OnGetRewardGroup(reason, &newGroup, mailGive);

		for (size_t i = 0; i < filterItems.size(); ++i)
		{
			ItemDataEntry* data = ItemDataEntryMgr::Instance()->FindEntry(filterItems[i].id);
			if (!data)
			{
				continue;
			}

			bool isDungeon = false;
			ReasonInfo ri;
			if (ParseReason(reason, ri))
			{
				if (ri.type == SOURCE_TYPE_DUNGEON)
				{
					isDungeon = true;
				}
			}

			if (isDungeon)
			{
				//提示拾取失败
				SendNotify(9101, data->name);
			}
			else if (sendMail)
			{
				// 发邮件
				Item* item = ItemMaker::Instance()->MakeItem(this, filterItems[i].id, filterItems[i].num, 0, filterItems[i].strenth, filterItems[i].auctionCoolTimeStamp);
				if (!item)
				{
					ErrorLogStream << PLAYERINFO(this) << " make item error! id = " << filterItems[i].id << LogStream::eos;
					continue;
				}
				mailGroup.push_back(item);

				//通知获得新道具
				GetNewItemParam param;
				param.openJar = openJar;
				param.itemId = item->GetID();
				param._newItem = (void*)item;
				CLEventMgr::Instance()->SendPlayerEvent(GetID(), CLEventMgr::ET_NOTIFY_NEWITEM, &param);

				if (mailGroup.size() == MAX_MAILITEM_NUM)
				{
					Sys::SendSysMail(GetID(), strSenderName.c_str(), strTitle.c_str(), strContent.c_str(), reason, validDay, NULL, mailGroup);
					for (auto item : mailGroup)
					{
						CL_SAFE_DELETE(item);
					}
					mailGroup.clear();
				}
			}

		}
	}
	
	// 剩余的发邮件部分
	if (mailGroup.size() > 0)
		Sys::SendSysMail(GetID(), strSenderName.c_str(), strTitle.c_str(), strContent.c_str(), reason, validDay, NULL, mailGroup);
	
	for (auto item : mailGroup)
	{
		CL_SAFE_DELETE(item);
	}
}

void Player::AddRewards(const char* reason, RewardGroup* group, bool notice, UInt32 validDay, bool sendMail, bool checkGetLimit)
{
	AddRewardsBase(reason, group, notice, validDay, sendMail, checkGetLimit, false);
}

void Player::AddJarRewards(const char* reason, RewardGroup* group, bool notice, UInt32 validDay)
{
	AddRewardsBase(reason, group, notice, validDay, true, false, true);
}

void Player::AddRewardsByMailGive(const char* reason, RewardGroup* group, bool notice, UInt32 validDay, bool sendMail, bool checkGetLimit)
{
	return AddRewardsBase(reason, group, notice, validDay, sendMail, checkGetLimit, false, true);
}

UInt32 Player::CheckAddRewardsScript(RewardGroup* group)
{
	return CheckAddRewards(group, NULL, 0);
}

void Player::AddRewardsNotNotify(const char* reason, RewardGroup* group, bool notice, UInt32 validDay, bool sendMail, bool checkGetLimit)
{
	AddRewardsBase(reason, group, notice, validDay, sendMail, checkGetLimit, false, false, false);
}

void Player::GetMailInfoByReason(const char* reason, std::string& senderName, std::string& title, std::string& content)
{
	//DungeonDataEntryMgr::Instance()->FindEntry

	UInt32 senderNameId = 2201;
	UInt32 titleId = 2202;
	UInt32 contentId = 2203;

	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));

	ReasonInfo reasonInfo;
	if (ParseReason(reason, reasonInfo))
	{
		LogSourceType type = (LogSourceType)((reasonInfo.type / LOG_SOURCE_TYPE_RATIO) * LOG_SOURCE_TYPE_RATIO);
		if (reasonInfo.type == SOURCE_TYPE_ITEM_JAR || reasonInfo.type == SOURCE_TYPE_ITEM_JAR_TEN)
		{
			senderNameId = 2207;
			titleId = 2208;
			contentId = 2209;
		}
		else if (type == SOURCE_TYPE_DUNGEON)
		{
			DungeonDataEntry* dungeonDataEntry = DungeonDataEntryMgr::Instance()->FindEntry((UInt32)reasonInfo.param1);
			if (dungeonDataEntry != NULL)
			{
				senderNameId = 2204;
				titleId = 2205;
				contentId = 2206;

				SysNotifyMgr::Instance()->MakeNotify(senderNameId, buffer, sizeof(buffer), dungeonDataEntry->name.c_str());
				senderName = buffer;
				SysNotifyMgr::Instance()->MakeNotify(titleId, buffer, sizeof(buffer), dungeonDataEntry->name.c_str());
				title = buffer;
				SysNotifyMgr::Instance()->MakeNotify(contentId, buffer, sizeof(buffer));
				content = buffer;

				return;
			}
		}
		else if (reasonInfo.type == SOURCE_GOLDJAR_RESET_POINT)
		{
			senderNameId = 2208;
			titleId = 1241;
			contentId = 1242;
		}
		else if (reasonInfo.type == SOURCE_MAGJAR_RESET_POINT)
		{
			senderNameId = 2208;
			titleId = 1239;
			contentId = 1240;
		}
		else if (reasonInfo.type == SOURCE_TYPE_SHOP_MALL)
		{
			senderNameId = 1247;
			titleId = 1244;
			contentId = 1246;
		}
		else if (reasonInfo.type == SOURCE_TYPE_MERGE_SERVER_STORAGE)
		{
			senderNameId = 1258;
			titleId = 1259;
			contentId = 1257;
		}
		else if (reasonInfo.type == SOURCE_TYPE_MERGE_SERVER_ROLECHANGENAME)
		{
			senderNameId = 1258;
			titleId = 1260;
			contentId = 1261;
		}
		else if (reasonInfo.type == SOURCE_TYPE_MERGE_SERVER_GUILDCHANGENAME)
		{
			senderNameId = 1258;
			titleId = 1262;
			contentId = 1263;
		}
		else if (reasonInfo.type == SOURCE_TYPE_GUILD_LOTTRY)
		{
			senderNameId = 8604;
			titleId = 8603;
			contentId = 8602;
		}
		else if (reasonInfo.type == SOURCE_TYPE_MASTER_GIVE)
		{
			senderNameId = 0;
			titleId = 8702;
			contentId = 8701;
		}
		else if (reasonInfo.type == SOURCE_TYPE_MASTER_ADDON_PAY)
		{
			senderNameId = 8604;
			titleId = 8603;
			contentId = 8602;
		}
		else if (reasonInfo.type == SOURCE_TYPE_CHARGE)
		{
			senderNameId = 9501;
			titleId = 9503;
			contentId = 9500;
		}
		else if (reasonInfo.type == SOURCE_TYPE_MERGE_SERVER_ADVENTURETEAMCHANGENAME)
		{
			senderNameId = 10007;
			titleId = 10005;
			contentId = 10006;
		}
	}

	if (senderNameId > 0)
	{
		SysNotifyMgr::Instance()->MakeNotify(senderNameId, buffer, sizeof(buffer));
		senderName = buffer;
	}
	
	if (titleId > 0)
	{
		SysNotifyMgr::Instance()->MakeNotify(titleId, buffer, sizeof(buffer));
		title = buffer;
	}
	
	if (contentId > 0)
	{
		SysNotifyMgr::Instance()->MakeNotify(contentId, buffer, sizeof(buffer));
		content = buffer;
	}
	
}

UInt32 Player::UseItem(ObjID_t uid, UInt8 useAll, UInt32 param1, UInt32 param2)
{
// 	if(IsAssetsLocked()) return;
// 	if(IsDujieScene()) return;
// 	m_ItemMgr.UseItem(id, target, num);

	UInt32 code = SUCCESS;
	Item* item = m_ItemMgr.FindItem(uid);
	if (!item)
	{
		SendNotify("item is null, uid={0}", uid);
		code = ITEM_DATA_INVALID;
	}
	else
	{
		if (1 == item->GetDataEntry()->isAbandon)
		{
			ReduceItemNum(GetReason(LogSourceType::SOURCE_TYPE_USE_ITEM, item->GetID()).c_str(), item, item->GetNum());
			
			return ITEM_ABANDON;
		}

		//装备&称号
		if (ITEM_EQUIP == item->GetType() || ITEM_TITLE == item->GetType())
		{
			code = m_ItemMgr.ChangeEquip(item, param1);
		}
		else if (ITEM_FASHION == item->GetType())	//时装
		{
			code = m_ItemMgr.ChangeFashion(item);
		}
		else	//道具
		{
			//在地下城使用地下城不可用的道具
			if (IsInDungeon() && 0 == item->GetDataEntry()->canDungeonUse)
			{
				ErrorLogStream << PLAYERINFO(this) << " item can not use in dungeon!" << LogStream::eos;
				return ITEM_CAN_NOT_USE_IN_DUNGEON;
			}

			if (IsInPK() && 0 == item->GetDataEntry()->canPkUse)
			{
				ErrorLogStream << PLAYERINFO(this) << " item can not use in pk!" << LogStream::eos;
				return ITEM_CAN_NOT_USE_IN_DUNGEON;
			}

			if ((!IsInDungeon() && !IsInPK()) && 0 == item->GetDataEntry()->canUse)
			{
				ErrorLogStream << PLAYERINFO(this) << " item can not use in town!" << LogStream::eos;
				return ITEM_CAN_NOT_USE_IN_TOWN;
			}

			if (useAll > 0 && item->GetDataEntry()->canUse != IUT_USEALL)
			{
				ErrorLogStream << PLAYERINFO(this) << " item(" << item->GetID() << ", " << item->GetDataID() << ") can not use all!" << LogStream::eos;
				return ITEM_USE_FAIL;
			}

			UInt16 num = 1;
			if (IUT_USEALL == item->GetDataEntry()->canUse &&
				useAll > 0)
			{
				num = item->GetNum();
			}

			code = m_ItemMgr.UseItem(uid, num, param1, param2);
			if (code == SUCCESS)
			{
				UseItemParam param;
				param._itemId = item->GetDataID();
				CLEventMgr::Instance()->SendPlayerEvent(GetID(), CLEventMgr::ET_USE_ITEM, &param);
			}
		}
	}

	return code;
}

UInt32 Player::QuickUseItem(UInt8 idx, UInt32 dungenID)
{
	DungeonDataEntry* data = DungeonDataEntryMgr::Instance()->FindEntry(dungenID);
	if (!data)
	{
		return DUNGEON_START_DUNGEON_NOT_EXIST;
	}

	if (idx >= (UInt8)data->quickUseItems.size())
	{
		return ITEM_USE_FAIL;
	}

	BuffDrugData* cfgData = BuffDrugDataMgr::Instance()->FindEntry(data->quickUseItems[idx]);
	if (!cfgData)
	{
		return ITEM_USE_FAIL;
	}

	QuickBuyDataEntry* quickData = QuickBuyDataEntryMgr::Instance()->FindEntry(data->quickUseItems[idx]);
	if (!quickData)
	{
		return ITEM_USE_FAIL;
	}

	if (idx > data->quickUseItems.size())
	{
		return ITEM_USE_FAIL;
	}
	
	if (GetLevel() <= cfgData->freeLevel)
	{
		return m_ItemMgr.UseItem(data->quickUseItems[idx]);
	}
	else
	{
		std::string reason = GetReason(SOURCE_TYPE_USE_ITEM, quickData->costItemId);

		if (GetItemNum(quickData->costItemId) < quickData->costNum)
		{
			return ITEM_NOT_ENOUGH_MONEY;
		}
		
		UInt32 ret = m_ItemMgr.UseItem(data->quickUseItems[idx]);
		if (ret == SUCCESS)
		{
			RemoveItem(reason.c_str(), quickData->costItemId, quickData->costNum);
		}

		return ret;
	}
}

UInt32 Player::SellItem(ObjID_t id, UInt16 num /*= 1*/)
{

	Item* item = m_ItemMgr.FindItem(id);
	if (!item)	return ITEM_DATA_INVALID;
	if (!item->GetDataEntry())
	{
		SendNotify("item data is null, key={0}", item->GetDataID());
		return ITEM_DATA_INVALID;
	}

	// 安全锁开启，不允许出售紫色以上品质的道具
	if (item->GetDataEntry()->color >= ITEM_QUALITY_PURPLE)
	{
		if (!SecurityLockCheck())
			return ErrorCode::SECURITY_LOCK_FORBID_OP;
	}

	// 装备锁住，不能出售
	if (item->IsItemLock(ILT_ITEM_LOCK))
	{
		return ErrorCode::ITEM_LOCKED;
	}

	if (item->IsItemLock(ILT_LEASE_LOCK))
	{
		return ITEM_IN_LEASING;
	}

	if (item->GetType() == ITEM_FASHION && item->IsItemLock(ILT_FASHION_LOCK))
	{
		return ITEM_FASHION_LOCK_FORBID_OP;
	}

	if (!item->GetDataEntry()->canTrade || item->GetDataEntry()->price == 0)
	{
		return ITEM_NOT_SELL;
	}
	
	if (item->GetPos().pack == PACK_WEAR || item->GetPos().pack == PACK_FASHION_WEAR)
		return ITEM_PACK_INVALID;

	if (ItemDataEntryMgr::Instance()->FindEntry(item->GetDataEntry()->sellItemId) == NULL)
	{
		return ITEM_SELL_ITEM_NOT_EXIST;
	}

	if (item->GetNum() < num)
	{
		ErrorLogStream << PLAYERINFO(this) << " sell num invalid! num:" << num << LogStream::eos;
		return ITEM_SELL_ITEM_NOT_EXIST;
	}

	if (item->GetDataEntry()->subType == ItemSubType::ST_EQUIP_WEAPON && IsWeaponExist(id))
	{
		return ITEM_WEAPON_BAR_NOT_OPERATION;
	}

	UInt32 itemId = item->GetDataEntry()->id;
	UInt32 thirdType = item->GetDataEntry()->thirdType;

	RewardGroup group;
	group.AddReward(item->GetDataEntry()->sellItemId, item->GetDataEntry()->price * num);
	if (CheckAddRewards(&group, item, num) != ErrorCode::SUCCESS)
	{
		return ITEM_ADD_PACK_FULL;
	}

	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_SHOP_SELL, item->GetID());

	UInt32 code = ReduceItemNum(reason.c_str(), item, num);
	if (code == SUCCESS)
	{
		AddRewards(reason.c_str(), &group, true);
		
		//AddGold("sell_item", money);

		//是单局药品用完要更新药品配置信息
		if (thirdType == TT_DUNGEON_POTION &&
			0 == GetItemNum(itemId))
		{
			std::vector<UInt32> pos = GetPotionPos();
			for (size_t i = 0; i < pos.size(); ++i)
			{
				if (pos[i] == itemId)
				{
					SetPotionPos(i, 0);
					break;
				}
			}

			SyncProperty();
		}
	}
	else
	{
		ErrorLogStream << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") SellItem uid:"
			<< item->GetID() << "|name:" << item->GetDataEntry()->name << "|num:" << num << "|ErrCode:" << code << LogStream::eos;
	}

	return code;
}

UInt32 Player::SellItemBat(std::vector<UInt64> itemUids)
{
	// 安全锁开启，禁止出售紫色以上品质物品
	if (!SecurityLockCheckByColor(itemUids, ITEM_QUALITY_PURPLE))
		return ErrorCode::SECURITY_LOCK_FORBID_OP;

	RewardGroup totalGroup; //发放的物品
	UInt16 num = 1;
	UInt32 ret = ErrorCode::SUCCESS;
	for (size_t i = 0; i < itemUids.size(); ++i)
	{
		UInt64 id = itemUids[i];
		Item* item = m_ItemMgr.FindItem(id);
		if (!item)
		{
			ret = ITEM_DATA_INVALID;
			break;
		}

		// 装备锁住，不能出售
		if (item->IsItemLock(ILT_ITEM_LOCK))
		{
			return ErrorCode::ITEM_LOCKED;
		}

		if (item->IsItemLock(ILT_LEASE_LOCK))
		{
			return ITEM_IN_LEASING;
		}

		if (!item->GetDataEntry())
		{
			SendNotify("item data is null, key={0}", item->GetDataID());
			ret = ITEM_DATA_INVALID;
			break;
		}

		if (!item->GetDataEntry()->canTrade || item->GetDataEntry()->price == 0)
		{
			ret = ITEM_NOT_SELL;
			break;
		}

		if (item->GetPos().pack == PACK_WEAR || item->GetPos().pack == PACK_FASHION_WEAR)
		{
			ret = ITEM_PACK_INVALID;
			break;
		}

		if (ItemDataEntryMgr::Instance()->FindEntry(item->GetDataEntry()->sellItemId) == NULL)
		{
			ret = ITEM_SELL_ITEM_NOT_EXIST;
			break;
		}

		if (item->GetNum() < num)
		{
			ErrorLogStream << PLAYERINFO(this) << " sell num invalid! num:" << num << LogStream::eos;
			ret = ITEM_SELL_ITEM_NOT_EXIST;
			break;
		}

		auto dataEntry = item->GetDataEntry();

		if (dataEntry->subType == ItemSubType::ST_EQUIP_WEAPON && IsWeaponExist(id))
		{
			ret = ITEM_WEAPON_BAR_NOT_OPERATION;
			break;
		}

		UInt32 itemId = dataEntry->id;
		UInt32 thirdType = dataEntry->thirdType;

		RewardGroup group;
		group.AddReward(dataEntry->sellItemId, dataEntry->price * num);
		if (CheckAddRewards(&group, item, num) != ErrorCode::SUCCESS)
		{
			ret = ITEM_ADD_PACK_FULL;
			break;
		}

		std::string reason = GetReason(LogSourceType::SOURCE_TYPE_SHOP_SELL, item->GetID());
		UInt32 code = ReduceItemNum(reason.c_str(), item, num);

		if (code == SUCCESS)
		{
			AddRewards(reason.c_str(), &group, false);
			totalGroup.AddReward(dataEntry->sellItemId, dataEntry->price * num);
			//AddGold("sell_item", money);

			//是单局药品用完要更新药品配置信息
			if (thirdType == TT_DUNGEON_POTION &&
				0 == GetItemNum(itemId))
			{
				std::vector<UInt32> pos = GetPotionPos();
				for (size_t i = 0; i < pos.size(); ++i)
				{
					if (pos[i] == itemId)
					{
						SetPotionPos(i, 0);
						break;
					}
				}

				SyncProperty();
			}
		}
		else
		{
			ErrorLogStream << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") SellItem uid:"
				<< id << "|name:" << dataEntry->name << "|num:" << num << "|ErrCode:" << code << LogStream::eos;
			ret = code;
			break;
		}
	}

	if (totalGroup.GetRewards().size() > 0)
	{
		const std::vector<ItemReward> rewards = totalGroup.GetRewards();
		std::vector<ItemReward> mergeRewards;
		//合并奖励
		m_ItemMgr.MergeItems(rewards, mergeRewards);
		CLProtocol::SceneNotifyGetItem notify;
		notify.items = mergeRewards;
		notify.sourceType = LogSourceType::SOURCE_TYPE_SHOP_SELL;
		SendProtocol(notify);
	}

	return ret;
}

UInt32 Player::PushStorage(ObjID_t id, UInt16 num, UInt8 storageType)
{
	Item* item = m_ItemMgr.FindItem(id);
	if (item == NULL || item->GetDataEntry() == NULL)
	{
		ErrorLogStream << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") PushStorage item data invalid!" << LogStream::eos;
		return ITEM_DATA_INVALID;
	}
	
	//角色绑定且没有转移石
	if (storageType == PACK_STORAGE && item->GetBind() == 2)
	{
		if (item->GetType() == ITEM_TITLE)  //称号
		{
			if (1 != item->GetSealState()) //未封装
			{
				return ITEM_TITLE_NOTSEAL_CANT_PUTSTORE;
			}
		}
		else if (item->GetTransferStoneid() == 0)
		{
			return ITEM_ROLEBIND_EQ_CANT_PUTSTORE;
		}
	}

	if (!m_ItemMgr.CheckAddStorage(item, num, storageType))
	{
		ErrorLogStream << "player(accid=" << GetAccID() << "," << "roleid=" << GetID() << ","
			<< ")" << "item(uid=" << id << " name=" << item->GetDataEntry()->name << ") " << "PushStorage pack is full!" << LogStream::eos;
		return ITEM_PUSH_STORAGE_FULL;
	}

	if (item->GetDataEntry()->subType == ItemSubType::ST_EQUIP_WEAPON && IsWeaponExist(id))
	{
		return ITEM_WEAPON_BAR_NOT_OPERATION;
	}

	UInt32 itemId = item->GetDataID();
	UInt8 itemQuality = item->GetQuality();
	UInt8 itemStrengthen = item->GetStrengthen();

	UInt16 pushNum = m_ItemMgr.PushStorage(item, num, storageType);
	if (pushNum > 0)
	{
		OnPushStorage(itemId, itemQuality, itemStrengthen, pushNum);
	}

	if (pushNum == num)
	{

		InfoLogStream << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") PushStorage suc!|uid:" << id << "|name" << item->GetDataEntry()->name << "|num:" << num <<"|storageType:" <<storageType << LogStream::eos;
		return SUCCESS;
	}
	else
	{
		ErrorLogStream << "player(accid=" << GetAccID() << "," << "roleid=" << GetID() << ","
			<< ")" << "item(uid=" << id << " name=" << item->GetDataEntry()->name << ") " << "PushStorage fail!|num:" << num << "|pushnum:" << pushNum << "|storageType:" << storageType << LogStream::eos;
		return ITEM_STORAGE_NUM_ERR;
	}
}

UInt32 Player::PullStorage(ObjID_t id, UInt16 num)
{
	Item* item = m_ItemMgr.FindItem(id);
	if (item == NULL || item->GetDataEntry() == NULL)
	{
		ErrorLogStream << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") PullStorage item data invalid!" << LogStream::eos;
		return ITEM_DATA_INVALID;
	}

	ObjID_t oldOwnerId = item->GetOwnerId();

	ItemDataEntry* dataEntry = item->GetDataEntry();
	if (!m_ItemMgr.CheckAddItem(item->GetDataID(), num, (ItemType)dataEntry->type, item->GetAuctionCoolTimeStamp()))
	{
		ErrorLogStream << "player(accid=" << GetAccID() << "," << "roleid=" << GetID() << ","
			<< ")" << "item(uid=" << id << "name=" << dataEntry->name << ") " << "PullStorage pack is full!" << LogStream::eos;
		return ITEM_ADD_PACK_FULL;
	}

	UInt32 itemId = item->GetDataID();
	UInt8 itemStrengthen = item->GetStrengthen();
	UInt8 itemQuality = item->GetQuality();
	UInt64 itemUid = item->GetID();
	UInt8 itemType = item->GetType();
	UInt8 itemSubType = item->GetSubType();
	UInt32 itemThirdType = item->GetThirdType();

	UInt16 pullNum = m_ItemMgr.PullStorage(item, num);
	if (pullNum > 0)
	{
		OnPullStorage(itemId, itemQuality, itemStrengthen, pullNum);
	}
	if (pullNum == num)
	{
		//by huchenhui 道具拥有者改变通知排行榜
		if (itemType == ITEM_EQUIP && IsWeapon(itemSubType) && oldOwnerId != GetID())
		{
			SortListMgr::Instance()->OnOwnerChanged(SORTLIST_WEAPON, itemUid, GetID(), GetName());
			SortListType listType = GetWeaponSortListTypeByItemThirdType(itemThirdType);
			if (SORTLIST_INVALID != listType)
			{
				SortListMgr::Instance()->OnOwnerChanged(listType, itemUid, GetID(), GetName());
			}
		}

		InfoLogStream << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") PullStorage suc!|uid:" << id << "|name:" << dataEntry->name << "|num:" << num << LogStream::eos;
		return SUCCESS;
	}
	else
	{
		ErrorLogStream << "player(accid=" << GetAccID() << "," << "roleid=" << GetID() << ","
			<< ")" << "item(uid=" << id << ") " << "PullStorage fail!|uid:" << id << "|name:" << dataEntry->name << "|num:" << num << "|pullnum:" << pullNum << LogStream::eos;
		return ITEM_STORAGE_NUM_ERR;
	}
}

UInt32 Player::OneKeyPushStorage()
{
	for (UInt8 i = PACK_MATERIAL; i <= PACK_EXPENDABLE; ++i)
	{
		Package* package = m_ItemMgr.GetPackage(i);
		if (!package)
			return ITEM_DATA_INVALID;

		class PushItemVisitor : public ItemVisitor
		{
		public:
			PushItemVisitor(ItemMgr* itemMgr, Player* player)
				:m_ItemMgr(itemMgr),m_player(player) {}
			~PushItemVisitor() {}

			bool operator()(Item* item)
			{
				if (m_ItemMgr->CheckOneKeyAddStorage(item->GetDataID(), item->GetAuctionCoolTimeStamp()))
				{
					UInt16 num = item->GetNum();
					UInt64 itemId = item->GetID();
					std::string name = item->GetDataEntry()->name;

					//UInt16 pushNum = m_ItemMgr->PushStorage(item, item->GetNum());
					UInt32 code = m_player->PushStorage(item->GetID(), item->GetNum());
					if (code == ErrorCode::SUCCESS)
					{
						InfoLogStream << "player(" << m_player->GetAccID() << "," << m_player->GetID() << "," << m_player->GetName()
							<< ") PushStorage suc!|uid:" << itemId << "|name:" << name << "|num:" << num << LogStream::eos;
						return true;
					}
					else
					{
						ErrorLogStream << "player(" << m_player->GetAccID() << "," << m_player->GetID() << "," << m_player->GetName()
							<< ") PushStorage fail!|uid:" << itemId << "|name:" << name << "|num:" << num << "|code:" << code << LogStream::eos;
						return false;
					}
				}

				return true;
			}

		private:
			ItemMgr* m_ItemMgr;
			Player* m_player;
		};
		PushItemVisitor visitor(&m_ItemMgr,this);
		package->VisitItems(visitor);
	}
	
	return SUCCESS;
}

Item* Player::FindCarryOnItem(ObjID_t id)
{
	Item* pItem = m_ItemMgr.FindCarryOnItem(id);
	if(pItem != NULL && pItem->GetNum() == 0)
	{
		m_ItemMgr.RemoveItem(pItem);
		Item::DestroyItem(pItem);
		return NULL;
	}
	return pItem;
}

Item* Player::FindItem(ObjID_t id)
{
	Item* pItem = m_ItemMgr.FindItem(id);
	if(pItem != NULL && pItem->GetNum() == 0)
	{
		m_ItemMgr.RemoveItem(pItem);
		Item::DestroyItem(pItem);
		return NULL;
	}
	return pItem;
}

Item* Player::FindItem(const ItemPos &pos)
{
	Item* pItem = m_ItemMgr.GetPackage(pos.pack)->FindItemByPos(pos.gridIndex);
	if(pItem != NULL && pItem->GetNum() == 0)
	{
		m_ItemMgr.RemoveItem(pItem);
		Item::DestroyItem(pItem);
		return NULL;
	}
	return pItem;
}


void Player::GetEquips(std::vector<Item*>& equips) const
{
	Package* wearPkg = m_ItemMgr.GetPackage(PACK_WEAR);
	Package* fashionPkg = m_ItemMgr.GetPackage(PACK_FASHION_WEAR);
	
	if (!wearPkg || !fashionPkg) return;
	class EquipVisitor : public ItemVisitor
	{
	public:
		EquipVisitor(std::vector<Item*>& equips) :m_Equips(equips) {}

		bool operator()(Item* item)
		{
			m_Equips.push_back(item);
			return true;
		}
	private:
		std::vector<Item*>& 	m_Equips;
	};

	EquipVisitor visitor(equips);
	wearPkg->VisitItems(visitor);
	fashionPkg->VisitItems(visitor);
}

void Player::GetSecondWeapons(std::vector<Item*>& weapons)
{
	Package* equipPackage = m_ItemMgr.GetPackage(PACK_EQUIP);
	if (!equipPackage) return;
	if (IsWeaponInvalid()) return;

	class SecondWeaponsVisitor : public ItemVisitor
	{
	public:
		SecondWeaponsVisitor(std::vector<Item*>& weapons, Player* player)
			: m_Weapons(weapons), m_Player(player) {}

		bool operator()(Item* item)
		{
			if (m_Player->IsWeaponExist(item->GetID()))
			{
				m_Weapons.push_back(item);
			}
			return true;
		}

	private:
		std::vector<Item*>&		m_Weapons;
		Player*	m_Player;
	};

	SecondWeaponsVisitor visitor(weapons, this);
	equipPackage->VisitItems(visitor);
}

Item* Player::GetEquip(UInt8 subType)
{
	Package* pkg = m_ItemMgr.GetPackage(PACK_WEAR);
	if (!pkg) return NULL;
	return pkg->FindItemByPos(subType - 1);
}

void Player::GetRaceItem(std::vector<RaceItem>& items)
{
	Package* pkg = m_ItemMgr.GetPackage(PACK_EXPENDABLE);
	if (!pkg) return;
	class RaceItemVisitor : public ItemVisitor
	{
	public:
		RaceItemVisitor(std::vector<RaceItem>& items) :m_Items(items) {}

		bool operator()(Item* item)
		{
			if (item->GetDataEntry()->canDungeonUse)
			{
				RaceItem tmpItem;
				tmpItem.id = item->GetDataID();
				tmpItem.num = item->GetNum();
				m_Items.push_back(tmpItem);
			}
			
			return true;
		}
	private:
		std::vector<RaceItem>& 	m_Items;
	};

	RaceItemVisitor visitor(items);
	pkg->VisitItems(visitor);

	Package* materialPackage = m_ItemMgr.GetPackage(PACK_MATERIAL);
	if (!materialPackage) return;
	RaceItemVisitor materialVisitor(items);
	materialPackage->VisitItems(materialVisitor);

	if (GetReviveCoin() > 0)
	{
		// 复活币
		RaceItem reviveCoinItem;
		reviveCoinItem.id = 600000006;
		reviveCoinItem.num = GetReviveCoin();
		items.push_back(reviveCoinItem);
	}
}

void Player::GetRaceBuffs(std::vector<RaceBuffInfo>& buffs)
{
	class RaceBuffVisitor : public BuffVisitor
	{
	public:
		RaceBuffVisitor(std::vector<RaceBuffInfo>& raceBuffs) :m_RaceBuffs(raceBuffs) {}

		bool operator()(Buff* buff)
		{
			RaceBuffInfo raceBuff;
			raceBuff.m_id = buff->GetId();
			raceBuff.m_overlayNums = buff->GetOverlay();
			raceBuff.m_startTime = buff->GetStartTime().MSec();

			if (buff->GetDataEntry()->durationType == BDT_DEADLINE_TIME_CONTINUE_PVP)
			{
				raceBuff.m_duration = buff->GetDuration() + 600;	//加一段时间
			}
			else
			{
				raceBuff.m_duration = buff->GetDuration();
			}

			m_RaceBuffs.push_back(raceBuff);
			return true;
		}
	private:
		std::vector<RaceBuffInfo>& m_RaceBuffs;
	};

	RaceBuffVisitor visitor(buffs);
	GetBuffMgr().VisitBuffs(visitor);
}

void Player::GetRaceRetinues(std::vector<RaceRetinue>& retinues)
{
	GetRetinueMgr().GetRaceRetinues(retinues);
}

void Player::GetRacePets(std::vector<RacePet>& pets)
{
	GetPetMgr().GetRacePets(pets);
}

UInt32 Player::ReduceItemNum(const char* reason, Item* item, UInt16 num)
{
	if(reason == NULL) return ITEM_NO_REASON;
	if (item == NULL) return  ITEM_DATA_INVALID;
	if (num == 0) return ITEM_NUM_INVALID;

	Package* pPack = m_ItemMgr.GetPackage(item->GetPos().pack);
	UInt16 oldNum = item->GetNum();

	if(item->GetNum() > num)
	{
		item->SetNum(item->GetNum() - num);

		item->SyncProperty(this);
		OnCostItem(reason, NULL, item->GetStrengthen(), num, oldNum, item);
		OnRemoveItem(reason, item, num, oldNum);
	}
	else
	{
		num = item->GetNum();
		if(!m_ItemMgr.RemoveItem(item)) num = 0;
		if (num > 0)
		{
			OnCostItem(reason, NULL, item->GetStrengthen(), num, oldNum, item);
			OnRemoveItem(reason, item, num, oldNum);
		}

		Item::DestroyItem(item);
	}

	if(num > 0)
	{
		if(pPack != NULL) pPack->OnItemChanged(this);
	}

	// 吃鸡开宝箱被打断
	BattleScene* battleScene = dynamic_cast<BattleScene*>(this->GetScene());
	if (battleScene)
	{
		battleScene->OnOpenBoxBeInterrupted(this->GetBattleID(), this->GetID());
	}

	return SUCCESS;
}

void Player::VisitItems(UInt8 pack, ItemVisitor& visitor)
{
	m_ItemMgr.VisitItems(pack, visitor);
}

UInt32 Player::GetPackItemNum(UInt8 pack) const
{
	Package* package = m_ItemMgr.GetPackage(pack);
	if(package == NULL) return 0;
	return package->GetItemNum();
}

UInt32 Player::DecomposeEquip(const char* reason, std::vector<ObjID_t>& uidList, ItemRewardVec& getItems, bool sendMail)
{
	// 安全锁开启，不允许分解紫色以上装备操作
	if (!SecurityLockCheckByColor(uidList, ITEM_QUALITY_PURPLE))
	{
		return ErrorCode::SECURITY_LOCK_FORBID_OP;
	}
	
	//批量分解条件过滤
	if (uidList.size() > 1)
	{
		SystemValueDataEntry* svdata = SystemValueDataEntryMgr::Instance()->FindEntry(SVT_ONEKEY_DECOMPOSE_LV);
		if (!svdata)
		{
			ErrorLogStream << "systemValueData is null, key=" << SVT_ONEKEY_DECOMPOSE_LV << LogStream::eos;
			SendNotify("systemValueData is null, key = {0}", SVT_ONEKEY_DECOMPOSE_LV);
			return ITEM_DATA_INVALID;
		}

		if (GetLevel() < svdata->value)
		{
			return ITEM_ONEKEY_DECOMPOSE_LV_NOT_ENOUGH;
		}

		//vip等级要求
// 		bool hasVip = false;
// 		GetVipValue(VIP_PRIVILEGE_DECOMPOSE, hasVip);
// 		if (!hasVip)
// 		{
// 			return PLAYER_VIPLV_NOT_ENOUGH;
// 		}

	}

	ClearRewardGroup();
	std::vector<Item*> equips;
	for (size_t i = 0; i < uidList.size(); ++i)
	{
		Item* item = FindItem(uidList[i]);
		if (!item)
		{
			SendNotify("item is null, uid = {0}", uidList[i]);
			return ITEM_DATA_INVALID;
		}

		// 是否有装备锁住
		if (item->IsItemLock(ILT_ITEM_LOCK))
		{
			return ErrorCode::ITEM_LOCKED;
		}

		if (item->IsItemLock(ILT_LEASE_LOCK))
		{
			return ITEM_IN_LEASING;
		}

		if (item->GetDataEntry()->subType == ItemSubType::ST_EQUIP_WEAPON && IsWeaponExist(item->GetID()))
		{
			return ITEM_WEAPON_BAR_NOT_OPERATION;
		}

		equips.push_back(item);

		UInt32 code = m_ItemMgr.DecomposeEquip(item, sendMail);

// 		InfoLogStream << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") DecomposeEquip uid:"
// 			<< item->GetID() << "|name:" << item->GetDataEntry()->name << "|code:" << code << LogStream::eos;

		if (code != SUCCESS)
		{
			return code;
		}
	}

	// 扣除
	for (size_t i = 0; i < equips.size(); ++i)
	{
		if (equips[i]->GetType() == ITEM_EQUIP)
		{
			OnDecomposeEquip();
		}

		ReduceItemNum(reason, equips[i], 1);
	}

	RewardGroup* rewardGrp = GetRewardGroupNoClear();

	m_ItemMgr.MergeItems(rewardGrp->GetRewards(), getItems);

	// 加材料
	AddRewards(reason, rewardGrp, false);

	return SUCCESS;
}

UInt32 Player::DecomposeEquip(ObjID_t uid)
{
	std::vector<ObjID_t> uidList;
	ItemRewardVec getItems;
	uidList.push_back(uid);
	return DecomposeEquip(GetReason(SOURCE_TYPE_EQUIP_DECOMPOSE).c_str(), uidList, getItems);
}

UInt32 Player::StrengthenEquip(ObjID_t equipUid, bool useUnbreak, ObjID_t strTicket)
{
	// 开启安全锁，不允许强化装备
	if (!SecurityLockCheck())
	{
		return ErrorCode::SECURITY_LOCK_FORBID_OP;
	}

	Item* equip = FindItem(equipUid);
	if (!equip)
	{
		SendNotify("equip is null, uid = {0}", equipUid);
		return ITEM_DATA_INVALID;
	}

	// 装备锁开启，不允许强化
	if (equip->IsItemLock(ILT_ITEM_LOCK))
	{
		return ErrorCode::ITEM_LOCKED;
	}

	if (equip->IsItemLock(ILT_LEASE_LOCK))
	{
		return ITEM_IN_LEASING;
	}

	//检查是否可以破碎
	if (equip->GetDataEntry()->canDecompose == 0)
	{
		InfoLogStream << "player(" << this->GetID() << "), strengthen equip err equip(" << equipUid << "), equip typeid(" 
			<< equip->GetDataID() << ")can not decompose!" << LogStream::eos;
		if (SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_WEEK_HELL_FAIL_RETURN_TICKETS))
		{
			return ITEM_CNA_NOT_DECOMPOSE;
		}
	}

	UInt32 tmpDataId = equip->GetDataID();
	UInt8 oldStrengthen = equip->GetStrengthen();

	//获取旧包裹
	UInt32 oldPack = equip->GetPos().pack;

	//获取旧数据
	UInt8 oldEquipSubType = equip->GetSubType();
	UInt8 oldEquipQuality = equip->GetQuality();

	//函数调用后如果code返回是强化碎裂那equip指针不能再使用了,已经被delete了
	UInt32 probability = 0;
	UInt32 code = m_ItemMgr.StrengthenEquip(equip, useUnbreak, strTicket, probability);

	if (code == ErrorCode::ITEM_STRENTH_FAIL_BROKE)
	{
		if (oldEquipSubType == ST_EQUIP_WEAPON && oldEquipQuality >= ITEM_QUALITY_PURPLE)
		{
			MallGiftPackActivateCond cond = CheckEquipBrokenActivateCond();
			ActivateMallGiftPackReq(cond);
		}
	}

	if (equip->IsItemLock(ILT_AUCTION_RETURN_LOCK))
	{
		equip->ClearItemLock(ILT_AUCTION_RETURN_LOCK);
	}
	OnStrengthenEquip(code, oldStrengthen, equipUid, tmpDataId, oldPack, useUnbreak, strTicket!=0, probability);
	return code;
}

bool Player::SimStrength(UInt32& strLv, UInt32 equipLv, UInt32 color, UInt32 subType, UInt32 destStrLv, 
	UInt32& fix1Ct, UInt32& fix2Ct, UInt32& strNum, UInt32& failNum, UInt32 lv2failNum[], 
	UInt32& useProtectNum, UInt32& costGold, UInt32& costColor, UInt32& costColorless)
{
	std::vector<EquipStrengthenData*> datas;
	EquipStrengthenDataMgr::Instance()->GetDatas(strLv, datas);
	if (0 == datas.size())
	{
		return false;
	}

	//有色晶体
	//UInt32 colorMatId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_STRCOLOR_ID);
	//无色晶体
	//UInt32 colorLessMatId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_STRCOLORLESS_ID);
	//保护劵
	//UInt32 protectID = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_STRPROTECT_ID);

	float ratioCost = 0.0f;
	if (IsWeapon(subType))
		ratioCost = (float)SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_STRENGTH_WP_COST_MOD) / 10;
	else if (IsDefend(subType))
		ratioCost = (float)SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_STRENGTH_DF_COST_MOD) / 10;
	else if (IsOrnaments(subType))
		ratioCost = (float)SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_STRENGTH_JW_COST_MOD) / 10;


	for (int i = 0; i < (int)datas.size(); ++i)
	{
		EquipStrengthenData* data = datas[i];
		if (equipLv >= data->minLv && equipLv <= data->maxLv)
		{
			StrenthCost needCost = data->cost[color];

			costGold += (UInt32)((float)needCost.goldCost * ratioCost + 0.5);
			costColor += (UInt32)((float)needCost.colorCost * ratioCost + 0.5);
			costColorless += (UInt32)((float)needCost.colorLessCost * ratioCost + 0.5);

			EquipStrRateData* sucRateData = EquipStrRateDataMgr::Instance()->GetDatas(strLv);
			if (!sucRateData)
			{
				return false;
			}

			float baseRate = (float)sucRateData->sucRate / 1000;
			float fix1 = fix1Ct * ((float)sucRateData->fix1 / 1000);
			if (fix1 > (float)sucRateData->fix1Max)
				fix1 = (float)sucRateData->fix1Max;

			float fix2 = lv2failNum[strLv] * ((float)sucRateData->fix2 / 1000);
			if (fix2 > (float)sucRateData->fix2Max)
				fix2 = (float)sucRateData->fix2Max;

			float fix3 = 0.0f;
			if (strNum >= sucRateData->fix3Condition[color - 1])
				fix3 = (float)sucRateData->fix3 / 1000;

			float rate = baseRate + fix1 + fix2 - fix3;
			if (rate < 0)
				rate = 0.0f;

			strNum++;

			if (Avalon::Random::SelectInThousand((UInt32)(rate * 1000)))
			{
				lv2failNum[strLv] = 0;
				fix1Ct = 0;

				//成功
				UInt32 oldStrLv = strLv;
				strLv++;

				DebugLogStream << oldStrLv << " => " << strLv << " success! rate = " << rate << LogStream::eos;
				return true;
			}
			else
			{
				failNum++;
				UInt32 oldStrLv = strLv;

				lv2failNum[strLv]++;
				fix1Ct++;

				//失败
				switch (sucRateData->failPunishment)
				{
				case 0:		//无惩罚
				{
								DebugLogStream << oldStrLv << " => " << strLv << " [" << failNum << "]" << "fail! rate = " << rate << LogStream::eos;
								return true;
				}
				case 1:		// 强化等级减一
				{
								strLv--;
								DebugLogStream << oldStrLv << " => " << strLv << " [" << failNum << "]" << "fail! rate = " << rate << LogStream::eos;
								return true;
				}
				case 2:		// 强化等级归0
				{
								strLv = 0;
								DebugLogStream << oldStrLv << " => " << strLv << " [" << failNum << "]" << "fail! rate = " << rate << LogStream::eos;
								return true;
				}
				case 3:		// 装备破碎,模拟不破碎
				{
								useProtectNum++;
								strLv = 0;
								DebugLogStream << oldStrLv << " => " << strLv << " [" << failNum << "]" << "fail! rate = " << rate << LogStream::eos;
								return true;
				}
				case 4:		// 强化等级减二到三
				{
								//随机惩罚数
								Int32 punish = Avalon::Random::RandBetween(2, 3);
								strLv -= punish;
								DebugLogStream << oldStrLv << " => " << strLv << " [" << failNum << "]" << "fail! rate = " << rate << LogStream::eos;
								return true;
				}
				default:
				{
						   return false;
				}
				}
			}
			break;
		}
	}
	return false;
}

void Player::_RegistOpActEvent()
{

}

void Player::_UnRegistOpActEvent()
{

}

bool Player::GetMaterials(const std::vector<ObjID_t>& ids, std::map<Item*, UInt32>& items, UInt8 type)
{
	//获取包裹中的道具
	for(std::vector<ObjID_t>::const_iterator iter = ids.begin(); 
		iter != ids.end(); ++iter)
	{
		ObjID_t id = *iter;
		if(id == 0) return false;

		Item* material = FindCarryOnItem(id);
		if(material == NULL/* || material->GetPos().pack != PACK_NORMAL*/
			|| material->GetDataEntry()->type != type) return false;

		++items[material];
	}

	//判断道具个数是否足够
	for(std::map<Item*,UInt32>::iterator iter = items.begin();
		iter != items.end();++iter)
	{
		if(iter->second > iter->first->GetNum()) return false;
	}

	return true;
}

void Player::RemoveMaterials(const std::map<Item*,UInt32>& items)
{
	for(std::map<Item*, UInt32>::const_iterator iter = items.begin();
		iter != items.end(); ++iter)
	{
		ReduceItemNum("equip", iter->first, iter->second);
	}
}

UInt32 Player::RemoveMaterials(const char* reason, std::vector<Item*>& items, UInt32 num)
{
	for(std::vector<Item*>::iterator iter = items.begin();
		iter != items.end() && num != 0;)
	{
		Item* item = *iter;
		if(item->GetNum() <= num)
		{
			num -= item->GetNum();
			ReduceItemNum(reason, item, item->GetNum());
			iter = items.erase(iter);
		}
		else
		{
			ReduceItemNum(reason, item, num);
			num = 0;
			++iter;
		}
	}
	return num;
}

void Player::AddGold(const char* reason, UInt32 num)
{
	if (reason == NULL) return;
	UInt32 oldGold = m_Gold;
	if (m_Gold + num > MAX_MONEY) m_Gold = MAX_MONEY;
	else m_Gold = m_Gold + num;

	if (m_Gold > oldGold)
	{
		GameInfoLogStream("Property") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") addgold:"
			<< reason << "|" << m_Gold - oldGold << "|" << m_Gold << LogStream::eos;

		OnChangeMoney(MoneyType::MONEY_GOLD, num);
		SendAssetUdpLog(AssetType::ASSET_GOLD, reason, (UInt64)oldGold, (Int64)num, (Int64)m_Gold - (Int64)oldGold, (UInt64)m_Gold);
	}
}

UInt32 Player::RemoveGold(const char* reason, UInt32 num)
{
	if (reason == NULL) return 0;
	UInt32 oldGold = m_Gold;
	if (num >= m_Gold) m_Gold = 0;
	else m_Gold = m_Gold - num;

	if (oldGold > m_Gold)
	{
		GameInfoLogStream("Property") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") removegold:"
			<< reason << "|" << oldGold - m_Gold << "|" << m_Gold << LogStream::eos;

		OnChangeMoney(MoneyType::MONEY_GOLD, num * -1);
		SendAssetUdpLog(AssetType::ASSET_GOLD, reason, (UInt64)oldGold, (Int64)num * -1, (Int64)m_Gold - (Int64)oldGold, (Int64)m_Gold);

		return oldGold - m_Gold;
	}

	return 0;
}

void Player::AddWarriorSoul(const char* reason, UInt32 num)
{
	if (reason == NULL) return;
	UInt32 oldValue = m_WarriorSoul;
	if (m_WarriorSoul + num > MAX_MONEY) m_WarriorSoul = MAX_MONEY;
	else m_WarriorSoul = m_WarriorSoul + num;

	if (m_WarriorSoul > oldValue)
	{
		GameInfoLogStream("Property") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") addWarriorSoul:"
			<< reason << "|" << m_WarriorSoul - oldValue << "|" << m_WarriorSoul << LogStream::eos;

		SendAssetUdpLog(AssetType::ASSET_WARRIOR_SOUL, reason, (UInt64)oldValue, (Int64)num, (Int64)m_WarriorSoul - (Int64)oldValue, (UInt64)m_WarriorSoul);
	}
}

void Player::RemoveWarriorSoul(const char* reason, UInt32 num)
{
	if (reason == NULL) return;
	UInt32 oldValue = m_WarriorSoul;
	if (num >= m_WarriorSoul) m_WarriorSoul = 0;
	else m_WarriorSoul = m_WarriorSoul - num;

	if (oldValue > m_WarriorSoul)
	{
		GameInfoLogStream("Property") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") removeWarriorSoul:"
		<< reason << "|" << oldValue - m_WarriorSoul << "|" << m_WarriorSoul << LogStream::eos;

		OnChangeMoney(MoneyType::MONEY_WARRIOR_SOUL, num * -1);
		SendAssetUdpLog(AssetType::ASSET_WARRIOR_SOUL, reason, (UInt64)oldValue, (Int64)num * -1, (Int64)m_WarriorSoul - (Int64)oldValue, (UInt64)m_WarriorSoul);
	}
}

void Player::AddPkCoin(const char* reason, UInt32 num)
{
	if (reason == NULL) return;
	UInt32 oldPkCoin = m_PkCoin;
	if (m_PkCoin + num > MAX_MONEY) m_PkCoin = MAX_MONEY;
	else m_PkCoin = m_PkCoin + num;

	if (m_PkCoin > oldPkCoin)
	{
		GameInfoLogStream("Property") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") addPkCoin:"
			<< reason << "|" << m_PkCoin - oldPkCoin << "|" << m_PkCoin << LogStream::eos;

		SendAssetUdpLog(AssetType::ASSET_PK_COIN, reason, (UInt64)oldPkCoin, (Int64)num, (Int64)m_PkCoin - (Int64)oldPkCoin, (UInt64)m_PkCoin);
	}
}

void Player::RemovePkCoin(const char* reason, UInt32 num)
{
	if (reason == NULL) return;
	UInt32 oldPkCoin = m_PkCoin;
	if (num >= m_PkCoin) m_PkCoin = 0;
	else m_PkCoin = m_PkCoin - num;

	if (oldPkCoin > m_PkCoin)
	{
		GameInfoLogStream("Property") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") removepkcoin:"
			<< reason << "|" << oldPkCoin - m_PkCoin << "|" << m_PkCoin << LogStream::eos;

		OnChangeMoney(MoneyType::MONEY_PK_COIN, num * -1);
		SendAssetUdpLog(AssetType::ASSET_PK_COIN, reason, (UInt64)oldPkCoin, (Int64)num * -1, (Int64)m_PkCoin - (Int64)oldPkCoin, (UInt64)m_PkCoin);
	}
}

UInt32 Player::AddPkCoinFromRace(UInt32 num)
{
	UInt32 dailyCount = GetCount(COUNTER_DAY_PKCOIN_FROM_RACE);
	UInt32 maxCount = DailyMaxPkCoinFromRace();
	
	if (dailyCount + num > maxCount)
	{
		num = maxCount - dailyCount;
	}

	SetCounter(COUNTER_DAY_PKCOIN_FROM_RACE, dailyCount + num);
	AddPkCoin(GetReason(LogSourceType::SOURCE_TYPE_PVP_RACE).c_str(), num);

	return num;
}

UInt32 Player::AddPkCoinFromActivity(UInt32 num)
{
	UInt32 dailyCount = GetCount(COUNTER_DAY_PKCOIN_FROM_ACTIVITY);
	UInt32 maxCount = DailyMaxPkCoinFromActivity();

	if (dailyCount + num > maxCount)
	{
		num = maxCount - dailyCount;
	}

	SetCounter(COUNTER_DAY_PKCOIN_FROM_ACTIVITY, dailyCount + num);
	AddPkCoin(GetReason(LogSourceType::SOURCE_TYPE_PVP_ACTIVITY).c_str(), num);

	return num;
}

void Player::AddGoldJarPoint(const char* reason, UInt32 num)
{
	if (reason == NULL) return;
	UInt32 oldPoint = m_GoldJarPoint;
	if (m_GoldJarPoint + num > MAX_MONEY) m_GoldJarPoint = MAX_MONEY;
	else m_GoldJarPoint = m_GoldJarPoint + num;

	if (m_GoldJarPoint > oldPoint)
	{
		DebugLogStream << PLAYERINFO(this) << " AddGoldJarPoint old:" << oldPoint << " new:" << m_GoldJarPoint << " add:" << num << " reason:" << reason << LogStream::eos;

		SendAssetUdpLog(AssetType::ASSET_GOLDJAR_POINT, reason, (UInt64)oldPoint, (Int64)num, (Int64)m_GoldJarPoint - (Int64)oldPoint, (UInt64)m_GoldJarPoint);
	}
}

void Player::AddMagJarPoint(const char* reason, UInt32 num)
{
	if (reason == NULL) return;
	UInt32 oldPoint = m_MagJarPoint;
	if (m_MagJarPoint + num > MAX_MONEY) m_MagJarPoint = MAX_MONEY;
	else m_MagJarPoint = m_MagJarPoint + num;

	if (m_MagJarPoint > oldPoint)
	{
		SendAssetUdpLog(AssetType::ASSET_MAGJAR_POINT, reason, (UInt64)oldPoint, (Int64)num, (Int64)m_MagJarPoint - (Int64)oldPoint, (UInt64)m_MagJarPoint);
	}
}

void Player::RemoveGoldJarPoint(const char* reason, UInt32 num)
{
	if (reason == NULL) return;
	UInt32 oldPoint = m_GoldJarPoint;
	if (num >= m_GoldJarPoint) m_GoldJarPoint = 0;
	else m_GoldJarPoint = m_GoldJarPoint - num;

	if (oldPoint > m_GoldJarPoint)
	{
		DebugLogStream << PLAYERINFO(this) << " RemoveGoldJarPoint old:" << oldPoint << " new:" << m_GoldJarPoint << " remove:" << num << " reason:" << reason << LogStream::eos;

		SendAssetUdpLog(AssetType::ASSET_GOLDJAR_POINT, reason, (UInt64)oldPoint, (Int64)num * -1, (Int64)m_GoldJarPoint - (Int64)oldPoint, (Int64)m_GoldJarPoint);
	}
}

void Player::RemoveMagJarPoint(const char* reason, UInt32 num)
{
	if (reason == NULL) return;
	UInt32 oldPoint = m_MagJarPoint;
	if (num >= m_MagJarPoint) m_MagJarPoint = 0;
	else m_MagJarPoint = m_MagJarPoint - num;

	if (oldPoint > m_MagJarPoint)
	{
		DebugLogStream << PLAYERINFO(this) << " RemoveMagJarPoint old:" << oldPoint << " new:" << m_MagJarPoint << " remove:" << num << " reason:" << reason << LogStream::eos;

		SendAssetUdpLog(AssetType::ASSET_MAGJAR_POINT, reason, (UInt64)oldPoint, (Int64)num * -1, (Int64)m_MagJarPoint - (Int64)oldPoint, (Int64)m_MagJarPoint);
	}
}

UInt32 Player::DailyMaxPkCoinFromRace()
{
	UInt32 vipAddition = 0;
	GetVipValue(VIP_PRIVILEGE_PKCOIN_MAX, vipAddition);
	return vipAddition + 200;
}

UInt32 Player::DailyMaxPkCoinFromActivity()
{
	return 200;
}

void Player::AddBindGold(const char* reason, UInt32 num)
{
	if(reason == NULL) return;
	UInt32 oldBindGold = m_BindGold;
	if(m_BindGold + num > MAX_MONEY) m_BindGold = MAX_MONEY;
	else m_BindGold = m_BindGold + num;

	if(m_BindGold > oldBindGold)
	{
		GameInfoLogStream("Property") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") addbindgold:" 
			<< reason << "|" << m_BindGold - oldBindGold << "|" << m_BindGold << LogStream::eos;

		SendAssetUdpLog(AssetType::ASSET_BIND_GOLD, reason, (UInt64)oldBindGold, (Int64)num, (Int64)m_BindGold - (Int64)oldBindGold, (UInt64)(m_BindGold));
	}
}

void Player::RemoveBindGold(const char* reason, UInt32 num, UInt32 itemId, UInt32 itemNum)
{
	if(reason == NULL) return;
	UInt32 oldBindGold = m_BindGold;
	if(num >= m_BindGold) m_BindGold = 0;
	else m_BindGold = m_BindGold - num;

	if(oldBindGold > m_BindGold)
	{
		GameInfoLogStream("Property") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") removebindgold:"
			<< reason << "|" << oldBindGold - m_BindGold << "|" << m_BindGold << LogStream::eos;

		OnChangeMoney(MoneyType::MONEY_GOLD, num * -1);
		SendAssetUdpLog(AssetType::ASSET_BIND_GOLD, reason, (UInt64)oldBindGold, (Int64)num * -1, (Int64)m_BindGold - (Int64)oldBindGold, (UInt64)m_BindGold);
	}
}

void Player::AddPoint(const char* reason, UInt32 num)
{
	if(reason == NULL) return;
	UInt32 oldPoint = m_Point;
	if(m_Point + num > MAX_MONEY) m_Point = MAX_MONEY;
	else m_Point = m_Point + num;

	if(m_Point > oldPoint)
	{
		SyncAccountInfo(ACCOUNT_POINT, m_Point);

#ifdef _SAVE_ACCOUNT_INFO_BY_SCENE_
		CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand(GetSrcZoneId(), "t_account", GetAccGuid(), true);
		cmd->PutData("point", GetPoint());
		CLRecordClient::Instance()->SendCommand(cmd);
#endif

		GameInfoLogStream("Property") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") addpoint:" 
			<< reason << "|" << m_Point - oldPoint << "|" << m_Point << LogStream::eos;

		SendAssetUdpLog(AssetType::ASSET_POINT, reason, (UInt64)oldPoint, (Int64)num, (Int64)m_Point - (Int64)oldPoint, (UInt64)m_Point);
	}
}

void Player::RemovePoint(const char* reason, UInt32 num, UInt32 itemId, UInt32 itemNum)
{
	if(reason == NULL) return;
	UInt32 oldPoint = m_Point;
	if(num >= m_Point) m_Point = 0;
	else m_Point = m_Point - num;

	if(oldPoint > m_Point)
	{
		SyncAccountInfo(ACCOUNT_POINT, m_Point);

#ifdef _SAVE_ACCOUNT_INFO_BY_SCENE_
		CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand(GetSrcZoneId(), "t_account", GetAccGuid(), true);
		cmd->PutData("point", GetPoint());
		CLRecordClient::Instance()->SendCommand(cmd);
#endif

		GameInfoLogStream("Property") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") removepoint:"
			<< reason << "|" << oldPoint - m_Point << "|" << m_Point << LogStream::eos;

		OnChangeMoney(MoneyType::MONEY_POINT, num * -1);
		SendAssetUdpLog(AssetType::ASSET_POINT, reason, (UInt64)oldPoint, (Int64)num * -1, (Int64)m_Point - (Int64)oldPoint, (UInt64)m_Point);
	}
}

bool Player::RemoveGoldBoth(const char* reason, UInt32 num)
{
	if (reason == NULL) return false;
	if (GetBindGold() >= num)
	{
		RemoveBindGold(reason, num);
		return true;
	}
	else if (GetBindGold() > 0)
	{
		num = num - GetBindGold();
		RemoveBindGold(reason, GetBindGold());
		RemoveGold(reason, num);
		return true;
	}
	else
	{
		RemoveGold(reason, num);
		//DebugLogStream << PLAYERINFO(this) << " RemoveGoldBoth, RemoveGold(reason, num)" << LogStream::eos;
		return true;
	}
	return false;
}

void Player::AddBindPoint(const char* reason, UInt32 num)
{
	if(reason == NULL) return;
	UInt32 oldBindPoint = m_BindPoint;
	if(m_BindPoint + num > MAX_MONEY) m_BindPoint = MAX_MONEY;
	else m_BindPoint = m_BindPoint + num;

	if (m_BindPoint > oldBindPoint)
	{
		GameInfoLogStream("Property")  <<  "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") addBindPoint:" 
			<< reason << "|" << m_BindPoint - oldBindPoint << "|" << m_BindPoint << LogStream::eos;

		SendAssetUdpLog(AssetType::ASSET_BIND_POINT, reason, (UInt64)oldBindPoint, (Int64)num, (Int64)m_BindPoint - (Int64)oldBindPoint, (UInt64)m_BindPoint);
	}
}

void Player::RemoveBindPoint(const char* reason, UInt32 num)
{
	if(reason == NULL) return;
	UInt32 oldBindPoint = m_BindPoint;
	if(num >= m_BindPoint) m_BindPoint = 0;
	else m_BindPoint = m_BindPoint - num;

	if (oldBindPoint > m_BindPoint)
	{
		GameInfoLogStream("Property")  <<  "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") removeBindPoint:" 
			<< reason << "|" << oldBindPoint - m_BindPoint << "|" << m_BindPoint << LogStream::eos;

		OnChangeMoney(MoneyType::MONEY_POINT, num * -1);
		SendAssetUdpLog(AssetType::ASSET_BIND_POINT, reason, (Int64)oldBindPoint, (Int64)num * -1, (Int64)m_BindPoint - oldBindPoint, (UInt64)m_BindPoint);
	}
}

void Player::SendBillingResult(UInt32 billingId, UInt8 result)
{
	if(billingId == 0) return;

	CLProtocol::WorldBillingGiveItemRet ret;
	ret.nodeId = billingId;
	ret.accId = GetAccID();
	ret.result = result;
	ret.level = GetLevel();
	ret.isqqvip = m_qqVipInfo.IsVip()?1:0;
	ret.qqviplvl = m_qqVipInfo.level;
	Router::SendToAdmin(ret);
}

UInt32 Player::EnlargePackage()
{
// 	if(size == 0) return false;
// 	if(packType != PACK_NORMAL && packType != PACK_STORAGE) return false;
// 
// 	Package* package = m_ItemMgr.GetPackage(packType);
// 
// 	if(package->GetSize() == package->GetMaxSize())
// 	{
// 		SendNotify(packType == PACK_NORMAL?36:37);
// 		return false;
// 	}
// 	
// 	UInt32 newsize = UInt32(package->GetSize()) + size;
// 	if(newsize > package->GetMaxSize()) newsize = package->GetMaxSize();
// 
// 	package->SetSize(newsize);
// 
// 	if(packType == PACK_NORMAL)
// 	{
// 		m_PackSize = newsize;
// 		SendNotify(53, newsize);
// 	}
// 	else
// 	{
// 		m_StorageSize = newsize;
// 		SendNotify(54, newsize);
// 	}


	UInt32 curPackSz = GetPackSize();
	if (curPackSz >= 30 && curPackSz <= 90)
	{
		UInt32 cost = SystemValueDataEntryMgr::Instance()->GetSystemValue((SystemValueType)(curPackSz + 10));
		if (cost == 0)
		{
			ErrorLogStream << "accid(" << GetAccID() << ") playerid(" << GetID() << ")SystemValueData value err! key = " << curPackSz + 10 << LogStream::eos;
			SendNotify("SystemValueData value err! key = {0}", curPackSz + 10);
			return ITEM_DATA_INVALID;
		}

		UInt32 costItemId = SystemValueDataEntryMgr::Instance()->GetSystemValue((SystemValueType)(curPackSz + 11));
		
		if (GetItemNum(costItemId) < cost)
		{
			return ITEM_NOT_ENOUGH_POINT;
		}

		RemoveItem(GetReason(SOURCE_TYPE_ENLARGE_PACK).c_str(), costItemId, cost);

		SetPackSize(curPackSz + 10);
		SyncProperty();

		OnEnlargePackage(GetEnlargePackageSize());

		GameInfoLogStream("Package") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") EnlargePackage success" << LogStream::eos;

		return SUCCESS;
	}
	else
	{
		GameErrorLogStream("Package") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") EnlargePackage pack size max!" << LogStream::eos;
		return ITEM_PACKSIZE_MAX;
	}
		
}

UInt32 Player::EnlargeStorage(UInt8 count)
{
	UInt8 addSz = 0;
	if (count > 0)
	{
		if (count % 10 != 0)
		{
			SendNotify("pram error! count = {0}", count);
			return ITEM_DATA_INVALID;
		}

		addSz = count;
	}
	else
	{
		addSz = 10;
	}

	UInt8 curPackSz = GetStorageSize();
	if (curPackSz >= 30 && curPackSz <= 90)
	{
		UInt32 cost = SystemValueDataEntryMgr::Instance()->GetSystemValue((SystemValueType)(curPackSz + 10));
		if (cost == 0)
		{
			ErrorLogStream << "accid(" << GetAccID() << ") playerid(" << GetID() << ")SystemValueData value err! key = " << curPackSz + 10 << LogStream::eos;
			SendNotify("SystemValueData value err! key = {0}", curPackSz + 10);
			return ITEM_DATA_INVALID;
		}

		UInt32 costItemId = SystemValueDataEntryMgr::Instance()->GetSystemValue((SystemValueType)(curPackSz + 11));

		if (GetItemNum(costItemId) < cost)
		{
			return ITEM_NOT_ENOUGH_POINT;
		}

		//先扣钱
		RemoveItem(GetReason(SOURCE_TYPE_ENLARGE_PACK).c_str(), costItemId, cost);

		InfoLogStream << PLAYERINFO(this) << " costItemId = " << costItemId << " cost = " << cost << " curPackSz =  " << curPackSz << " addSz = " << addSz << LogStream::eos;

//		RemovePointBoth(GetReason(SOURCE_TYPE_ENLARGE_STORAGE).c_str(), cost);
// 		SetStorageSize(curPackSz + addSz);
// 		SyncProperty();

		CLProtocol::SWEnlargeStorageReq req;
		req.accId = GetAccID();
		req.newSize = curPackSz + addSz;
		req.playerId = GetID();
		req.cost = cost;
		req.costItemId = costItemId;
		Router::SendToWorld(req);

		GameInfoLogStream("Storage") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") EnlargeStorage success" << LogStream::eos;
		return SUCCESS;
	}
	else
	{
		GameErrorLogStream("Storage") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") EnlargeStorage pack size max!" << LogStream::eos;
		return ITEM_PACKSIZE_MAX;
	}
}

void Player::InitPackage()
{
	UInt32 initSz = PlayerMgr::Instance()->GetPlayerAttr()->GetProperty("PlayerInitInfo", "packsize");
	if (m_PackSize < initSz)
	{
		m_PackSize = initSz;
	}

	for (UInt32 i = PACK_EQUIP; i <= PACK_TASK; ++i)
	{
		UInt32 finalSize = m_PackSize;
		finalSize += GetPackageAdditionSize((PackType)i);

		Package* pPack = m_ItemMgr.GetPackage((UInt8)i);
		if (pPack != NULL)
		{
			pPack->SetSize(finalSize);
			pPack->RemoveOverSizeItem();
		}
	}

	UInt32 initSz_1 = PlayerMgr::Instance()->GetPlayerAttr()->GetProperty("PlayerInitInfo", "packsize_1");
	Package* pFashionPack = m_ItemMgr.GetPackage(PACK_FASHION);
	if (pFashionPack != NULL)
	{
		UInt32 finalSize = initSz_1;
		finalSize += GetPackageAdditionSize(PACK_FASHION);

		pFashionPack->SetSize(finalSize);
		pFashionPack->RemoveOverSizeItem();

		m_PackageTypeSizeMap[PACK_FASHION] = initSz_1;
	}

	Package* pTitlePack = m_ItemMgr.GetPackage(PACK_TITLE);
	if (pTitlePack != NULL)
	{
		UInt32 finalSize = initSz_1;
		finalSize += GetPackageAdditionSize(PACK_TITLE);

		pTitlePack->SetSize(finalSize);
		pTitlePack->RemoveOverSizeItem();

		m_PackageTypeSizeMap[PACK_TITLE] = initSz_1;
	}

	UpdatePackageTypeSize();

	Package* pEqrecoPack = m_ItemMgr.GetPackage(PACK_EQRCOV);
	if (pEqrecoPack != NULL)
	{
		UInt32 finalSize = m_PackSize;
		finalSize += GetPackageAdditionSize(PACK_EQRCOV);

		pEqrecoPack->SetSize(finalSize);
		pEqrecoPack->RemoveOverSizeItem();
	}
}

void Player::InitStoragePack(UInt8 sz)
{
	UInt8 initSz = PlayerMgr::Instance()->GetPlayerAttr()->GetProperty("PlayerInitInfo", "storagesize");
	if (sz < initSz)
	{
		m_StorageSize = initSz;
	}
	else
	{
		m_StorageSize = sz;
	}

	Package* pPack = m_ItemMgr.GetPackage(PACK_STORAGE);
	if (pPack != NULL)
	{
		pPack->SetSize(m_StorageSize);

		InfoLogStream << PLAYERINFO(this) << " init storage size = " << m_StorageSize << LogStream::eos;
	}
}

UInt32 Player::GetEnlargePackageSize()
{
	UInt32 initSz = PlayerMgr::Instance()->GetPlayerAttr()->GetProperty("PlayerInitInfo", "packsize");
	if (m_PackSize > initSz)
		return m_PackSize - initSz;
	return 0;
}

UInt32 Player::GetEnlargeStoragePackSize()
{
	UInt8 initSz = PlayerMgr::Instance()->GetPlayerAttr()->GetProperty("PlayerInitInfo", "storagesize");
	if (m_StorageSize > initSz)
		return m_StorageSize - initSz;
	return 0;
}

UInt32 Player::GetPackageAdditionSize(PackType type)
{
	UInt32 additionSize = 0;

	switch (type)
	{
	case PACK_FASHION:
		additionSize = VipPrivilegeDataEntryMgr::Instance()->FindPrivilegeValue(VIP_FASHION_PACKAGE_SIZE_ADDITION, GetVipLvl());

		break;

	case PACK_TITLE:
		additionSize = VipPrivilegeDataEntryMgr::Instance()->FindPrivilegeValue(VIP_TITLE_PACKAGE_SIZE_ADDITION, GetVipLvl());

		break;

	default: break;
	}

	m_PackSizeAddition.SetSize(type, additionSize);
	
	UInt32 finalAddition = m_PackSizeAddition.GetSize(type);

	if (finalAddition == UINT32_MAX)
	{
		ErrorLogStream << PLAYERINFO(this) << "Package's type(" << (UInt32)type << ") over flow!" << LogStream::eos;
		return 0;
	}

	return finalAddition;
}

bool Player::CheckUseStorage()
{
// 	VipPrivilegeDataEntry* dataEntry = GetVipPrivilege(VIP_PRIVILEGE_CARRYONSTORAGE);
// 	if(dataEntry != NULL) return true;

// 	Npc* npc = GetScene()->FindNpc(GetSelectObject());
// 	if(npc == NULL || npc->GetNpcType() != NPC_STORAGE || Distance(npc) >= GetScene()->GetSyncRange()) return false;
	return true;
}

void Player::CountMoveSpeed()
{
	LostBattleScene* lostScene = dynamic_cast<LostBattleScene*>(GetScene());
	if (lostScene)
	{
		UInt32 oldSpeed = this->GetLostDungeonSpeedBack();
		UInt32 buffSpeed = GetBuffMgr().GetBuffAddition(BT_SPEED_ADD);
		UInt32 newSpeed = oldSpeed + buffSpeed;
		if (newSpeed < 0)
			newSpeed = 0;

		SetMoveSpeed(newSpeed);
		return;
	}

	class ItemMovespeedVisitor : public ItemVisitor
	{
	public:
		ItemMovespeedVisitor() : movespeedAddition(0) {}
		~ItemMovespeedVisitor() {}

		bool operator()(Item *item)
		{
			if (!item)
			{
				return true;
			}

			if (item && item->GetAttrDataEntry())
			{
				movespeedAddition += item->GetAttrDataEntry()->TownSpeedRate;
			}

			if (item->GetDataEntry() && item->GetDataEntry()->suitId > 0)
			{
				suitEquipNums[item->GetDataEntry()->suitId]++;
			}

			return true;
		}

	public:
		UInt16		movespeedAddition;
		// 各个套装的数量
		std::map<UInt32, UInt32> suitEquipNums;
	};
	
	ItemMovespeedVisitor visitor;
	m_ItemMgr.VisitItems(PACK_WEAR, visitor);
	m_ItemMgr.VisitItems(PACK_FASHION_WEAR, visitor);
	
	// 遍历所有的套装效果，看看有没有城镇属性加成的
	UInt32 suitAddition = 0;
	for (auto itr : visitor.suitEquipNums)
	{
		UInt32 suitId = itr.first;
		UInt32 num = itr.second;

		auto suitData = EquipSuitDataEntryMgr::Instance()->FindEntry(suitId);
		if (!suitData)
		{
			continue;
}

		UInt32 maxNum = sizeof(suitData->attrIdByEquipNum) / sizeof(suitData->attrIdByEquipNum[0]);
		if (num >= maxNum)
		{
			ErrorLogStream << "player(" << GetID() << ") has suit(" << suitId << ") equip num(" << num << ")." << LogStream::eos;
			num = maxNum;
		}

		UInt32 attr = suitData->attrIdByEquipNum[num - 1];
		if (attr == 0)
		{
			continue;
		}

		auto attrData = EquipAttrDataMgr::Instance()->FindEntry(attr);
		if (attrData)
		{
			DebugLogStream << "player(" << GetID() << ") has suit(" << suitId << ", " << num << ") attr(" << attr << ") movespeed(" << attrData->TownSpeedRate << ")." << LogStream::eos;
			suitAddition += attrData->TownSpeedRate;
		}
	}
	
	// buff对速度的加成
	Int32 buffSpeed = 0;
	if (SERVER_TYPE == ST_BSCENE)
	{
		buffSpeed = GetBuffMgr().GetBuffAddition(BT_SPEED_ADD);
	}

	Int32 finalSpeed = PLAYER_INIT_MOVE_SPEED + visitor.movespeedAddition + suitAddition + buffSpeed;
	if (finalSpeed < 0)
		finalSpeed = 0;
	
	SetMoveSpeed(finalSpeed);

	DebugLogStream << "player(" << GetID() << ") speed(" << finalSpeed << ") suitAddition(" << suitAddition << ") buffSpeed(" << buffSpeed << ")." << LogStream::eos;
}

UInt32 Player::OpenJar(UInt32 jarId, UInt8 comboNum, UInt32 opActId)
{
	CLProtocol::SceneUseMagicJarRet ret;
	ret.jarId = jarId;
	ret.getPointId = 0;
	ret.getPoint = 0;
	ret.getPointTime = 0;

	ClearRewardGroup();

	GetJarMgr().StartOpenJar();
	std::vector<Item*> getItems;
	ret.code = GetJarMgr().OpenJar(jarId, comboNum, ret.getPointId, ret.getPoint, ret.getPointTime, ret.baseItem.id, ret.baseItem.num, opActId);

	GetJarMgr().GetOpenResult(getItems, ret.getItems);

	// 如果是安全锁禁止操作，这里不需要回消息
	if (ret.code == ErrorCode::SECURITY_LOCK_FORBID_OP)
		return ret.code;

	Avalon::Packet* packet = Avalon::Packet::Create();
	ret.Encode(packet);
	
	for (size_t i = 0; i < getItems.size(); ++i)
	{
		Item* item = getItems[i];
		if (!item)	continue;

		Avalon::NetOutputStream stream(packet->GetBuffer() + packet->GetSize(), packet->GetMaxSize() - packet->GetSize());

		try
		{
			stream & item->GetID() & item->GetDataID();
			item->Encode(stream, SST_SELF, false);
		}
		catch (const Avalon::NetException& exception)
		{
			ErrorLogStream << "player(" << GetID() << "," << GetName()
				<< ") encode items failed:" << exception.what() << "!" << LogStream::eos;
			continue;
		}
		

		if (i == getItems.size() - 1)
		{
			stream & (UInt64)0;
		}

		packet->SetSize(packet->GetSize() + UInt16(stream.GetPos()));
	}

	SendPacket(packet);
	Avalon::Packet::Destroy(packet);

	GetJarMgr().FinishOpenJar();
	GetJarMgr().ClearHitItems();
	return ret.code;
}

void Player::RecordGetSkyFashion(UInt32 fashionId)
{
// 	UInt64 bit = m_FashionSkyGetBit.GetDBValue();
// 	bit |= ((UInt64)1 << idx);
// 	m_FashionSkyGetBit = bit;

	std::ostringstream os;
	os << GET_SKY_FASHION_ID << fashionId;
	const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(os.str());
	if (!counterConfig)
	{
		CounterCfg::Instance()->RegCounter(os.str().c_str(), COUNTER_CYCLE_NONE);
		}
	
	SetCounter(os.str().c_str(), 1);
}

void Player::ResetGetSkyRecord(UInt32 fashionId)
{
	std::ostringstream os;
	os << GET_SKY_FASHION_ID << fashionId;
	const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(os.str());
	if (!counterConfig)
	{
		CounterCfg::Instance()->RegCounter(os.str().c_str(), COUNTER_CYCLE_NONE);
	}

	SetCounter(os.str().c_str(), 0);

// 	UInt64 delbit = 0;
// 	UInt64 curbit = m_FashionSkyGetBit.GetDBValue();
// 	for (size_t i = 0; i < resetBit.size(); ++i)
// 	{
// 		if (!resetBit[i])
// 		{
// 			continue;
// 		}
// 
// 		if (resetBit[i]->index >= 64)
// 		{
// 			ErrorLogStream << PLAYERINFO(this) << " ResetGetSkyRecord param err! bit:" << resetBit[i]->index << LogStream::eos;
// 			continue;
// 		}
// 
// 		delbit |= ((UInt64)1 << resetBit[i]->index);
// 	}
// 	delbit = ~delbit;
// 	curbit &= delbit;
// 	m_FashionSkyGetBit = curbit;
}

bool Player::HasGetSkyFashion(UInt32 fashionId)
{
	std::ostringstream os;
	os << GET_SKY_FASHION_ID << fashionId;
	const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(os.str());
	if (!counterConfig)
	{
		CounterCfg::Instance()->RegCounter(os.str().c_str(), COUNTER_CYCLE_NONE);
		}

	return GetCounter(os.str().c_str()) > 0;
}

UInt32 Player::GetIncome(UInt32 type)
{
	switch ((ItemSubType)type)
	{
		case ST_ITEM_GOLD:		return GetGold();
		case ST_BIND_GOLD:		return GetGoldBoth();
		case ST_ITEM_POINT:		return GetPoint();
		case ST_BIND_POINT:		return GetPointBoth();
		case ST_WARRIOR_SOUL:	return GetWarriorSoul();
		case ST_REVIVE_COIN:	return GetReviveCoin();
		case ST_PK_COIN:		return GetPkCoin();
		case ST_GUILD_CONTRI:	return GetGuildContribution();
		case ST_GOLDJAR_POINT:	return GetGoldJarPoint();
		case ST_MAGJAR_POINT:	return GetMagJarPoint();
		//case ST_SP:				return GetSP();
		case ST_VIP_EXP:		return GetVipExp();
		case ST_APPOINTMENT_COIN:	return GetAppointmentCoin();
		case ST_MASTER_GOODTEACH_VALUE: return GetMasterSysGoodTeachValue();
		case ST_DRESS_INTEGRAL_VALUE: return GetChangeFashionScore();
		case ST_WEAPON_LEASE_TICKET: return  GetWeaponLeaseTickets();
		case ST_CHIJI_SHOP_COIN: return GetCounter(CHIJI_SHOP_COIN_NAME);
		default:
		{
			ErrorLogStream << PLAYERINFO(this) << " item sub type error:" << type << LogStream::eos;
			return 0;
		}
	}
}

bool Player::LoadJarItem(JarItem* jarItem)
{
	return m_JarMgr.LoadJarItem(jarItem);
}

void Player::InitJarMgr()
{
	//m_JarMgr.Init();
}

void Player::OpenJarSample(UInt32 type, UInt32 combo, UInt32 num)
{
	m_JarMgr.OpenJarSample(type, combo, num);
}

bool Player::AddBuff(UInt32 buffId)
{
	return GetBuffMgr().AddBuff(buffId);
}

void Player::SyncAccountInfo(AccountInfoType type, UInt32 value)
{
	CLProtocol::SceneSyncAccountInfo sync;
	sync.accid = GetAccID();
	sync.type = type;
	sync.value = value;
	Router::SendToWorld(sync);
}

Task* Player::FindTask(UInt32 id)
{
	return (Task*)m_TaskMgr.FindTask(id);
}

Task* Player::FindAchievementTaskTask(UInt32 id)
{
	return (Task*)m_AchievementTaskMgr.FindTask(id);
}

bool Player::AcceptTask(UInt32 id, UInt8 quality, bool bForce)
{
	if(m_TaskMgr.AcceptTask(id, quality, bForce))
	{
		//刷新npc的任务状态
		//modified by aprilliu, 2016.04.12
		//RefreshNpcTaskStatus();
		return true;
	}
	return false;
}

bool Player::SubmitTask(UInt32 id, bool force /*=false*/)
{
	Task* task = FindTask(id);
	if (task == NULL)
	{
		ErrorLogStream << "in Player::SubmitTask, player(" << GetAccID() << "," << GetID() << "," << GetName() << ") is not task:(" << id << ")" << LogStream::eos;
		return false;
	}

	if(m_TaskMgr.SubmitTask(task, force))
	{
		//刷新npc的任务状态
		//modified by aprilliu
		//RefreshNpcTaskStatus();

		return true;
	}
	return false;
}

void Player::AbandonTask(UInt32 id)
{
	Task* task = FindTask(id);
	if(task == NULL) return;

	if(m_TaskMgr.AbandonTask(task))
	{
		//刷新npc的任务状态
		//modified by aprilliu
		//RefreshNpcTaskStatus();
	}
}

bool Player::SubmitLegendTask(UInt32 taskId)
{
	BaseTask* task = m_LegendTaskMgr.FindTask(taskId);
	return m_LegendTaskMgr.SubmitTask(task);
}

void Player::ResetTask()
{
	m_TaskMask.ClearMask();
	m_TaskMgr.ClearTask();
}

UInt32 Player::GetDailyTaskSelectorNum()
{
	std::vector<DailyTaskSelector>::iterator iter = m_pLevelDataEntry->dailyTaskPool.begin();
	UInt32 result = TaskDataEntryMgr::Instance()->GetSureDailyTasks(GetLevel()).size();
	for (UInt32 i = 0; i < m_pLevelDataEntry->dailyTaskPool.size(); ++i)
	{
		result += m_pLevelDataEntry->dailyTaskPool[i].num;
	}
	return result;
}

bool Player::CheckTaskFinished(UInt32 id)
{
	if (id > TASK_MAIN * MAX_TYPE_TASKNUM && id <= (TASK_ACHIEVEMENT + 1) * MAX_TYPE_TASKNUM)
	{
		return m_TaskMask.CheckMask(id - TASK_MAIN * MAX_TYPE_TASKNUM);
	}
	return false;
	/*
	if(id > TASK_MAIN * MAX_TYPE_TASKNUM && id <= (TASK_BRANCH + 1) * MAX_TYPE_TASKNUM)
	{
		return m_TaskMask.CheckMask(id - TASK_MAIN * MAX_TYPE_TASKNUM);
	}
	return false;
	*/
}

void Player::SetTaskFinished(UInt32 id)
{

	if (id > TASK_MAIN * MAX_TYPE_TASKNUM && id <= (TASK_ACHIEVEMENT + 1) * MAX_TYPE_TASKNUM)
	{
		m_TaskMask.SetMask(id - TASK_MAIN * MAX_TYPE_TASKNUM);
	}
	/*
	if(id > TASK_MAIN * MAX_TYPE_TASKNUM && id <= (TASK_BRANCH + 1) * MAX_TYPE_TASKNUM)
	{
		m_TaskMask.SetMask(id - TASK_MAIN * MAX_TYPE_TASKNUM);
	}
	*/
}

bool Player::AcceptLegendTask(UInt32 taskId)
{
	TaskDataEntry* taskData = TaskDataEntryMgr::Instance()->FindEntry(taskId);
	if (taskData == NULL) return false;
	return m_LegendTaskMgr.AcceptTask(taskData);
}

bool Player::CheckLegendTaskFinished(UInt32 taskId)
{
	return m_LegendTaskMgr.CheckFinishTask(taskId);
}

bool Player::CheckAchievementTaskFinished(UInt32 taskId)
{
	return m_AchievementTaskMgr.CheckFinishTask(taskId);
}

bool Player::IsTaskAcceptable(UInt32 id, bool bNotifyErr)
{
	return m_TaskMgr.IsTaskAcceptable(id, bNotifyErr);
}

BaseTask* Player::GetTask(UInt32 taskId)
{
	TaskDataEntry* dataEntry = TaskDataEntryMgr::Instance()->FindEntry(taskId);
	if (dataEntry == NULL) return NULL;
	switch (dataEntry->type)
	{
	case TASK_DAILY:
		return GetDailyTaskMgr().FindTask(taskId);
	case TASK_ACHIEVEMENT:
		return GetAchievementTaskMgr().FindTask(taskId);
	case TASK_CYCLE:
		return GetCycleTaskMgr().FindTask(taskId);
	case TASK_RED_PACKET:
		return GetRedPacketTaskMgr().FindTask(taskId);
	default:
		return GetTaskMgr().FindTask(taskId);
	}
}

UInt32 Player::SetTaskItem(UInt32 taskId, std::vector<UInt64> items)
{
	BaseTask* task = GetTask(taskId);
	if (task == NULL) return ErrorCode::TASK_NOT_EXIST;
	if (task->GetStatus() != TASK_UNFINISH) return ErrorCode::TASK_NOT_UNFINISH;
	
	TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
	if (script == NULL) return ErrorCode::TASK_SCRIPT_NOT_EXIST;

	for (UInt32 i = 0; i < items.size(); ++i)
	{
		Item* item = FindItem(items[i]);
		if (item == NULL) continue;
		if (item->GetDataEntry()->subType == ItemSubType::ST_EQUIP_WEAPON && IsWeaponExist(item->GetID()))
		{
			//return ITEM_WEAPON_BAR_NOT_OPERATION;
			continue;
		}
		UInt32 itemNum = 0;
		if (item->GetMaxNum() <= 1)
		{
			if (script->CheckSetTaskItemEvent((ItemQuality)item->GetQuality(), item->GetNeedLevel(), 0, itemNum))
			{
				script->OnSetTaskItem(this, task, item, 0, itemNum);
			}
			else
			{
				SendNotify(ErrorCode::TASK_SET_ITEM_TASK_TYPE_ERROR);
				return ErrorCode::TASK_SET_ITEM_TASK_TYPE_ERROR;
			}
		}
		else
		{
			if (script->CheckSetTaskItemEvent(ITEM_QUALITY_INVLID, 0, item->GetDataID(), itemNum))
			{
				UInt32 newNum = GetItemNum(item->GetDataID());
				if (newNum >= itemNum)
				{
					script->OnSetTaskItem(this, task, NULL, item->GetDataID(), itemNum);
				}
				else
				{
					SendNotify(ErrorCode::TASK_SET_ITEM_NUM_ERROR);
					return ErrorCode::TASK_SET_ITEM_NUM_ERROR;
				}
			}
		}
	}

	return ErrorCode::SUCCESS;
}

UInt32 Player::FinishAllTask(TaskType taskType , UInt32 taskId)
{
	m_TaskMgr.FinishAllTask(taskType, taskId);

	return ErrorCode::SUCCESS;
}

void Player::IncDailyTaskScore(UInt32 score)
{
	m_DailyTaskScore = m_DailyTaskScore + score;

	m_AbnormalTransactionMgr.OnIncDailyTaskScore(m_DailyTaskScore);

	m_MasterAcademicGrowthTaskMgr.OnIncDailyTaskScore(m_DailyTaskScore);
}

UInt32 Player::ReceiveDailyTaskScoreReward(UInt32 boxId)
{

	DailyTaskScoreRewardDataEntry* dataEntry = DailyTaskScoreRewardDataEntryMgr::Instance()->FindEntry(boxId);
	if (dataEntry == NULL) return ErrorCode::TASK_DATILY_TASK_SCORE_BOX_NOT_FOUNT;

	if (m_DailyTaskScore < dataEntry->score) return ErrorCode::TASK_DATILY_TASK_SCORE_BOX_SCORE;
	if (CheckDailyTaskRewardMask(boxId)) return ErrorCode::TASK_DATILY_TASK_SCORE_BOX_RECEIVE;

	RewardGroup* rewards = GetRewardGroup();
	rewards->AddRewards(dataEntry->itemReward);
	UInt32 result = CheckAddRewards(rewards);
	if (result == ErrorCode::SUCCESS)
	{
		AddRewards(GetReason(LogSourceType::SOURCE_TYPE_TASK_SCORE_REWARD, boxId).c_str(), rewards, true);
		SetDailyTaskRewardMask(boxId);
	}

	return result;
}

UInt32 Player::ReceiveAchievementScoreReward(UInt32 rewardId)
{
	AchievementLevelDataEntry* dataEntry = AchievementLevelDataEntryMgr::Instance()->FindEntry(rewardId);
	if (dataEntry == NULL) return ErrorCode::TASK_ACHIEVEMENT_SCORE_BOX_NOT_FOUNT;

	if (m_AchievementScore < dataEntry->maxScore) return ErrorCode::TASK_ACHIEVEMENT_SCORE_BOX_SCORE;
	if (CheckAchievementRewardMask(rewardId)) return ErrorCode::TASK_ACHIEVEMENT_SCORE_BOX_RECEIVE;

	RewardGroup* rewards = GetRewardGroup();
	rewards->AddRewards(dataEntry->itemReward);
	UInt32 result = CheckAddRewards(rewards);
	if (result == ErrorCode::SUCCESS)
	{
		AddRewards(GetReason(LogSourceType::SOURCE_TYPE_TASK_ACHIEVEMENT_SCORE_REWARD, rewardId).c_str(), rewards, true);
		SetAchievementRewardMask(rewardId);
	}

	return result;

}

void Player::AddAchievementScore(UInt32 score)
{
	if (score == 0) return;

	UInt32 oldScore = m_AchievementScore;
	m_AchievementScore = m_AchievementScore + score;

	SendNotify(9057, score);

	SortListMgr::Instance()->OnSortValueChanged(SortListType::SORTLIST_ACHIEVEMENT_SCORE, GetID(), GetName(), 0, GetOccu(), 0, GetLevel(), (UInt32)m_AchievementScore);

	InfoLogStream << PLAYERINFO(this) << " achievement score("<< oldScore << " | " << score << " >> " << m_AchievementScore <<")" << LogStream::eos;
}

void Player::LoadTask(UInt64 guid, UInt32 dataId, TaskStatus status, std::string parameter, UInt32 finTime, UInt8 submitCount)
{
	TaskDataEntry* dataEntry = TaskDataEntryMgr::Instance()->FindEntry(dataId);
	if (dataEntry == NULL || !dataEntry->open) return;
	BaseTask* task = NULL;

	switch (dataEntry->type)
	{
	case TASK_DAILY:
		task = new DailyTask(this, dataId, guid, status, parameter);
		if (!task->SetupBase() || !GetDailyTaskMgr().LoadTask(task))
		{
			GameErrorLogStream("Player") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") has invalid daily task " << task->GetDataId() << ".";
			CL_SAFE_DELETE(task);
		}
		break;
	case TASK_MAIN:
	case TASK_BRANCH:
	case TASK_TRANSFER:
	case TASK_AWAKEN:
	case TASK_TITLE:
		task = new Task(this, dataId, guid, status, parameter);
		if (!task->SetupBase() || !GetTaskMgr().LoadTask(task))
		{
			GameErrorLogStream("Player") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") has invalid task " << task->GetDataId() << ".";
			CL_SAFE_DELETE(task);
		}
		break;
	case TASK_LEGEND:
		task = new LegendTask(this, dataId, guid, status, parameter, submitCount);
		if (!task->SetupBase() || !GetLegendTaskMgr().LoadTask(task))
		{
			GameErrorLogStream("Player") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") has invalid Legend Task " << task->GetDataId() << ".";
			CL_SAFE_DELETE(task);
		}
		else
		{
			//2018.12.04 by huchenhui 查传奇之路未解锁的问题
			InfoLogStream << PLAYERINFO(this) << "Player::LoadTask, LEGEND Load Task(id:" << task->GetDataId() << ", status(" << status << "|" << task->GetStatus() << "))" << LogStream::eos;
		}
		break;
	case TASK_ACHIEVEMENT:
		task = new AchievementTask(this, dataId, guid, status, parameter, finTime);
		if (!task->SetupBase() || !GetAchievementTaskMgr().LoadTask(task))
		{
			GameErrorLogStream("Player") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") has invalid achievement task " << task->GetDataId() << ".";
			CL_SAFE_DELETE(task);
		}
		break;
	case TASK_SYSTEM:
		break;
	case TASK_ACTIVITY:
		break;
	case TASK_EXTENTION:
		break;
	case TASK_CYCLE:
		task = new CycleTask(this, dataId, guid, status, parameter);
		if (!task->SetupBase() || !GetCycleTaskMgr().LoadTask(task))
		{
			GameErrorLogStream("Player") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") has invalid cycle task " << task->GetDataId() << ".";
			CL_SAFE_DELETE(task);
		}
		break;
	case TASK_RED_PACKET:
		task = new RedPacketTask(this, dataId, guid, status, parameter, finTime);
		if (!task->SetupBase() || !GetRedPacketTaskMgr().LoadTask(task))
		{
			GameErrorLogStream("Player") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") has invalid red packet task " << task->GetDataId() << ".";
			CL_SAFE_DELETE(task);
		}
		break;
	case TASK_MASTER_DAILY:
		task = new MasterDailyTask(this, dataId, guid, status, parameter);
		if (!task->SetupBase() || !GetMasterDailyTaskMgr().LoadTask(task))
		{
			GameErrorLogStream("Player") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") has invalid master daily task " << task->GetDataId() << ".";
			CL_SAFE_DELETE(task);
		}
		break;
	case TASK_MASTER_ACADEMIC:
		task = new MasterAcademicGrowthTask(this, dataId, guid, status, parameter);
		if (!task->SetupBase() || !GetMasterAcademicGrowthTaskMgr().LoadTask(task))
		{
			GameErrorLogStream("Player") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") has invalid master academic growth task " << task->GetDataId() << ".";
			CL_SAFE_DELETE(task);
		}
		break;
	case TASK_MAX:
		break;
	default:
		break;
	}
}

void Player::OnLoadTaskFinished()
{
	GetDailyTaskMgr().CheckFinish();
	GetTaskMgr().CheckFinish();
	GetAchievementTaskMgr().CheckFinish();
	GetLegendTaskMgr().CheckFinish();
	GetCycleTaskMgr().CheckFinish();
	GetRedPacketTaskMgr().CheckFinish();
	GetMasterDailyTaskMgr().CheckFinish();
	GetMasterAcademicGrowthTaskMgr().CheckFinish();
}

bool Player::InviteTrade(ObjID_t id)
{
	if(GetTrade() != NULL) return false; //交易中

	if(GetSrcZoneId() != 0)
	{
		SendNotify(306);
		return false;
	}

	Player* other = GetScene()->FindPlayer(id);
	if(other == NULL) return false;
	if(Distance(other) > GetScene()->GetSyncRange())
	{
		SendNotify(301);
		return false;
	}

	if(other->GetTrade() != NULL)
	{
		SendNotify(305);
		return false;
	}

	m_Requests.AddRequest(REQUEST_TRADE, id);

	other->SyncRequest(REQUEST_TRADE, this);
	return true;
}

void Player::AcceptTrade(ObjID_t id)
{
	if(GetTrade() != NULL) return;

	if(GetSrcZoneId() != 0)
	{
		SendNotify(306);
		return;
	}

	Player* other = GetScene()->FindPlayer(id);
	if(other == NULL) return;

	if(!other->m_Requests.HasRequest(REQUEST_TRADE, GetID()))
	{
		SendNotify(82);
		return;
	}

	if(Distance(other) > GetScene()->GetSyncRange())
	{
		SendNotify(301);
		return;
	}

	if(other->GetTrade() != NULL)
	{
		SendNotify(305);
		return;
	}

	other->m_Requests.RemoveRequest(REQUEST_TRADE, GetID());

	//Trade* trade = new Trade(this, other);
	//trade->Open();
}

void Player::CancelTrade()
{
	if(m_pTrade != NULL)
	{
		m_pTrade->Cancel();
		m_pTrade = NULL;
	}
}

void Player::SendSysMail(const char* senderName, const char* title, const char* content, const char* reason, RewardGroup* rewards)
{
	Sys::SendSysMail(GetID(), senderName, title, content, reason, rewards);
}

void Player::RemoveRelation(RelationType type, ObjID_t tarId)
{
	if (type < RelationType::RELATION_FRIEND || type > RelationType::RELATION_MAX)
	{
		return;
	}

	std::map<ObjID_t, UInt16>& rList = m_Relations[type - 1];
	auto itr = rList.find(tarId);
	if (itr != rList.end())
	{
		rList.erase(itr);
	}
}

bool Player::HasRelation(RelationType type, ObjID_t tarId)
{
	if (type < RelationType::RELATION_FRIEND || type > RelationType::RELATION_MAX)
	{
		return false;
	}

	std::map<ObjID_t, UInt16>& rList = m_Relations[type - 1];
	auto itr = rList.find(tarId);
	return itr != rList.end();
}

bool Player::HasRelation(RelationType type)
{
	if (type < RelationType::RELATION_FRIEND || type > RelationType::RELATION_MAX)
	{
		return false;
	}

	std::map<ObjID_t, UInt16>& rList = m_Relations[type - 1];
	return rList.size() > 0;
}

Player* Player::GetOnlineMaster()
{
	std::map<ObjID_t, UInt16>& rList = m_Relations[RelationType::RELATION_MASTER - 1];
	if (rList.size() != 1)
	{
		return NULL;
	}

	auto itr = rList.begin();
	return PlayerMgr::Instance()->FindNormalPlayer(itr->first);
}

bool Player::IsMaster(ObjID_t roleId)
{
	return this->HasRelation(RelationType::RELATION_MASTER, roleId);
}

ObjID_t Player::GetMasterId()
{
	std::map<ObjID_t, UInt16>& rList = m_Relations[RelationType::RELATION_MASTER - 1];
	if (rList.size() != 1)
	{
		return 0;
	}
	auto itr = rList.begin();
	return itr->first;
}

void Player::AddFatigueToMaster(UInt32 usedFatigue)
{
	// 没有师傅
	if (!HasRelation(RelationType::RELATION_MASTER))
	{
		return;
	}

	// 累计徒弟扣除的疲劳
	IncCounter(DISCIPLE_USED_FATIGUE, usedFatigue);

	// 师傅不在线
	Player* master = GetOnlineMaster();
	if (!master)
	{
		return;
	}

	// 师傅每日获得的疲劳达到上限
	UInt32 dayGetFatigueMax = master->GetCounter(MASTER_DAY_FATIGU_MAX);
	if (dayGetFatigueMax >= SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_TAP_SYSTEM_MASTER_GET_FATIGUE_MAX))
	{
		return;
	}

	UInt32 totUsedFatigue = GetCounter(DISCIPLE_USED_FATIGUE);
	// 徒弟到了可返回的疲劳使用数
	if (totUsedFatigue >= SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_TAP_SYSTEM_DISCIPLE_USED_FATIGUE))
	{
		SetCounter(DISCIPLE_USED_FATIGUE, 0);

		std::string reason = GetReason(SOURCE_TYPE_MASTER_GET_FATIGUE);
		UInt32 addFatigue = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_TAP_SYSTEM_MASTER_GET_FATIGUE);
		master->AddFatigue(reason.c_str(), addFatigue);
		master->IncCounter(MASTER_DAY_FATIGU_MAX, addFatigue);

		//你的徒弟XXX为你增加了疲劳
		master->SendNotify(8922, GetName().c_str(), addFatigue);
	}

}

UInt8 Player::GetMasterDiscipleType()
{
	if (HasRelation(RELATION_MASTER)) return 2;
	if (HasRelation(RELATION_DISCIPLE)) return 1;
	return 0;
}

void Player::OnSyncIntimacy(ObjID_t id, UInt16 intimacy, UInt16 incIntimacy)
{
	FriendMap::iterator iter = m_Friends.find(id);
	if(iter != m_Friends.end())
	{
		iter->second = intimacy;
	}

	this->GetMasterAcademicGrowthTaskMgr().OnIncIntimacy(id, incIntimacy, intimacy);
	this->GetMasterDailyTaskMgr().OnIncIntimacy(id, incIntimacy, intimacy);
}

void Player::AddIntimacy(ObjID_t id, UInt16 intimacy)
{
	FriendMap::iterator iter = m_Friends.find(id);
	if(iter == m_Friends.end()) return;

	if(iter->second >= 60000) return;
	CLProtocol::WorldRelationAddIntimacy req;
	req.id = GetID();
	req.friendId = id;
	req.intimacy = intimacy;
	Router::SendToWorld(req);
}

UInt16 Player::GetIntimacy(ObjID_t id) const
{
	FriendMap::const_iterator iter = m_Friends.find(id);
	if(iter != m_Friends.end()) return iter->second;
	return 0;
}

void Player::SetTestLevel(UInt16 level)
{
	PlayerDataEntry* levelDataEntry = PlayerDataEntryMgr::Instance()->FindEntry(level);
	if(levelDataEntry == NULL) return;

	UInt16 oldLevel = GetLevel();
	UInt64 oldExp = m_Exp;
	if(m_Exp > levelDataEntry->upgradeexp)
	{
		m_Exp = levelDataEntry->upgradeexp;
	}
	SetLevel(level);
	m_pLevelDataEntry = levelDataEntry;

	SyncBaseInfoToWorld();
	SyncBaseInfoToGate();

	if(GetLevel() != oldLevel ||m_Exp != oldExp)
	{
		//SortListMgr::Instance()->OnSortValueChanged(SORTLIST_LEVEL, GetID(), GetName(), 0, 0, 0, GetLevel(), m_Exp); 
	}

	UpdateFuncNotify();
}

void Player::Levelup()
{
	bool bLevelup = false;

	UInt16 oldLv = GetLevel();

	while(m_Exp >= m_pLevelDataEntry->upgradeexp)
	{
		PlayerDataEntry* levelDataEntry = PlayerDataEntryMgr::Instance()->FindEntry(GetLevel() + 1);
		if (levelDataEntry == NULL)
		{
			//已经满级
			break;
		}

		if (levelDataEntry->upgradeexp == 0)
		{
			m_Exp = levelDataEntry->upgradeexp;
		}
		else
		{
			m_Exp = m_Exp - m_pLevelDataEntry->upgradeexp;
		}

		//modified by huchenhui
		//DecExp(m_pLevelDataEntry->upgradeexp);

		SetLevel(GetLevel() + 1);
		m_pLevelDataEntry = levelDataEntry;

		//是否提升觉醒技能. huchenhui
		//SetupPkValues();
		//IncHp(GetMaxHp());

		OnLevelup();
		bLevelup = true;

	}

	UInt16 newLv = GetLevel();
	if(bLevelup)
	{
		LevelUpParam param;
		param._oldLevel = oldLv;
		param._newLevel = newLv;
		CLEventMgr::Instance()->SendPlayerEvent(GetID(), CLEventMgr::ET_LEVEL_UP, &param);

		//modified by aprilliu
		//RefreshNpcTaskStatus();

		SyncBaseInfoToWorld(oldLv);
		SyncBaseInfoToGate();

		//通知social
		CLProtocol::SocialInviteNotifyLevelup notify;
		notify.nodeId = NODE_ID;
		notify.accId = GetAccID();
		notify.playerId = GetID();
		notify.level = GetLevel();
		Router::SendToSocial(notify);

		//modified by huchenhui
		//m_ActGiftbagMgr.OnLevelUp();

		//add by huchenhui
		//SendNotify(1001, GetLevel());

        // 计算新开放的关卡
        GetDungeonMgr().SyncDungeonList();

		// 等级改变之后马上写到数据库
		auto cmd = CLRecordClient::Instance()->PrepareUpdateCommand(GetSrcZoneId(), "t_player_info", GetID(), true);
		cmd->PutData("level", GetLevel());
		CLRecordClient::Instance()->SendCommand(cmd);
	}
}


void Player::LevelDown()
{
	bool bLeveldown = false;

	while (true)
	{
		PlayerDataEntry* levelDataEntry = PlayerDataEntryMgr::Instance()->FindEntry(GetLevel() - 1);
		if (levelDataEntry == NULL) break;
		if (m_Exp >= levelDataEntry->upgradeexp) break;

		//DecExp(m_pLevelDataEntry->upgradeexp);

		SetLevel(GetLevel() - 1);
		m_pLevelDataEntry = levelDataEntry;

		//SetupPkValues();

		//IncHp(GetMaxHp());

		//OnLevelup();
		bLeveldown = true;
	}

	if (bLeveldown)
	{
		//modified by aprilliu
		//RefreshNpcTaskStatus();

		SyncBaseInfoToWorld();
		SyncBaseInfoToGate();

		//通知social
		CLProtocol::SocialInviteNotifyLevelup notify;
		notify.nodeId = NODE_ID;
		notify.accId = GetAccID();
		notify.playerId = GetID();
		notify.level = GetLevel();
		Router::SendToSocial(notify);

		//m_ActGiftbagMgr.OnLevelUp();

        // 计算新开放的关卡
        GetDungeonMgr().SyncDungeonList();
	}
}


void Player::Speak(UInt8 channel, const std::string& word, const std::string voiceKey, UInt8 voiceDur, ObjID_t targetId, UInt8 isShare)
{
	UInt32 actScore = 0;
	UInt32 needActScore = 0;
	UInt32 vipFreeNum = 0;
	UInt32 dayFreeNum = 0;

	if(channel == 0 || channel > CHAT_CHANNEL_MAX) return;
	if (channel != CHAT_CHANNEL_TEAM_SPECIAL)
	{
		if (word.empty() || word.length() > CL_MAX_CHAT_LEN) return;
	}
	
	switch(channel)
	{
	case CHAT_CHANNEL_TEAM:
	case CHAT_CHANNEL_TEAM_SPECIAL:
		{
			if(GetTeam() == NULL) return;
		}
		break;
	case CHAT_CHANNEL_TEAM_INVITE:
		{
			UInt32 lvLimit = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_TEAM_CHAT_LV_LIMIT);
			if (GetLevel() < lvLimit)
			{
				SendNotify(3130, lvLimit);
				return;
			}
		}
		break;
	case CHAT_CHANNEL_WORLD:
		{
			UInt32 lvLimit = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_WORLD_CHAT_LV_LIMIT);
			if (GetLevel() < lvLimit)
			{
				SendNotify(3130, lvLimit);
				return;
			}

			GetVipValue(VIP_WORLD_CHAT_FREE, vipFreeNum);
			dayFreeNum = GetCounter(DAILY_WORLD_CHAT_FREE);
			//每日免费次数不够
			if (dayFreeNum >= vipFreeNum)
			{
				//检查活跃度
				actScore = GetCounter(DAILY_ACTIVE_SCORE);
				needActScore = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_WORLDCHAT_ACT_NEED);
				if (actScore < needActScore)
				{
					SendNotify(3131);
					return;
				}
			}
		}
		break;
	case CHAT_CHANNEL_PRIVATE:
		{
			if(GetLevel() < MIN_CHAT_LEVEL)
			{
				SendNotify(9200, MIN_CHAT_LEVEL);
				return;
			}

			auto targetPlayer = PlayerMgr::Instance()->FindNormalPlayer(targetId);
			if (targetPlayer && targetPlayer->GetZoneId() != GetZoneId())
			{
				// 说明不是一个服的人,不能私聊
				SendNotify(9064);
				return;
			}
		}
		break;
	case CHAT_CHANNEL_TRIBE:
		{
			if (GetGuildPost() == GUILD_POST_INVALID)
			{
				SendNotify(2300003);
				return;
			}
		}
		break;
	case CHAT_CHANNEL_AROUND:
		{
			UInt32 lvLimit = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AROUND_CHAT_LV_LIMIT);
			if (GetLevel() < lvLimit)
			{
				SendNotify(3130, lvLimit);
				return;
			}

			if (GetSceneID() == SCENE_TYPE_SINGLE)
			{
				SendNotify(3111);
				return;
			}
		}
		break;
	case CHAT_CHANNEL_ROOM:
		if (GetRoomId() == 0) return;
		break;
	}

	bool bForbid = false;

	//检查聊天时间
	UInt64 leftTime = m_ChatTimer.CheckChatTime(channel, CURRENT_TIME.MSec());
	if (leftTime > 0)
	{
		// 提示发送太快
		SendNotify(3129, leftTime / 1000);
		return;
	}
	
	//if (CheckPunishFlag(PUNISH_FORBIDTALK) && m_ForbitTalkDueTime > CURRENT_TIME.Sec()) bForbid = true;
	if (m_ForbitTalkDueTime > CURRENT_TIME.Sec()) bForbid = true;
	
	//被禁言
	if (bForbid)
	{
		if (m_ForbitTalkDueTime <= CHAT_HI_FRQ_TIME)
		{
			// 提示发送太快
			SendNotify(1063);
		}
		else
		{

			std::string timestr = SysNotifyMgr::Instance()->TimeToStr(m_ForbitTalkDueTime - UInt32(CURRENT_TIME.Sec()));
			// 提示禁言
			SendNotify(1064, timestr.c_str());
		}
		
		return;
	}
	
	//解析文本
	std::string retWords;
	std::set<Item*> linkItems;
	std::set<ObjID_t> linkAuctionItems;
	bool isLink;
	if (SUCCESS != ParseSpeakWords(word, retWords, linkItems, linkAuctionItems, isLink))
	{
		return;
	}

	//for(std::set<Item*>::iterator iter = linkItems.begin();
	//	iter != linkItems.end(); ++iter)
	//{
	//	WorldCacheItemData(*iter);
	//}

	//for(std::set<ObjID_t>::iterator iter = linkAuctionItems.begin();
	//	iter != linkAuctionItems.end(); iter++)
	//{
	//	Avalon::Packet* packet = Avalon::Packet::Create();
	//	packet->SetID(CLProtocol::WORLD_CACHE_CHATDATA_REQ);

	//	Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
	//	try
	//	{
	//		stream & 'A' & *iter;
	//	}
	//	catch(Avalon::NetException&)
	//	{
	//		Avalon::Packet::Destroy(packet);
	//		return;
	//	}
	//	packet->SetSize(UInt16(stream.GetPos()));
	//	Router::SendToWorld(packet);

	//	Avalon::Packet::Destroy(packet);
	//}

	m_ChatTimer.SetChatTime(channel, CURRENT_TIME.MSec());

	
	if(channel == CHAT_CHANNEL_WORLD || channel == CHAT_CHANNEL_AROUND 
		|| channel == CHAT_CHANNEL_PRIVATE || channel == CHAT_CHANNEL_TRIBE)
	{
		CLProtocol::CenterSyncChatInfo protocol;
		protocol.serverId = ZONE_ID;
		protocol.accId = GetAccID();
		protocol.channel = channel;
		protocol.time = (UInt32)CURRENT_TIME.Sec();
		protocol.roleUid = GetID();
		protocol.name = GetName();
		protocol.word = word;
		protocol.tarId = 0;
		protocol.tarName = "";
		protocol.pf = PLATFORM;
		protocol.qudao = GetPf();
		if (channel == CHAT_CHANNEL_PRIVATE)
		{
			protocol.tarId = targetId;

			Player* tar = PlayerMgr::Instance()->FindNormalPlayer(targetId);
			if (tar)
			{
				protocol.tarName = tar->GetName();
			}
			
		}
		protocol.level = GetLevel();
		 
		
		Router::SendToCenter(protocol);
	}

	Creature::Speak(channel, retWords, voiceKey, voiceDur, targetId, isLink, GetHeadFrameId());

	// 如果是分享，返回分享成功
	if (isShare != 0)
		SendNotify(5011);

	if (GetRoomId() != 0 && channel == CHAT_CHANNEL_ROOM)
	{
		CLProtocol::SceneSyncChat crossProtocol;
		crossProtocol.channel = channel;
		crossProtocol.objid = GetID();
		crossProtocol.occu = GetOccu();
		crossProtocol.sex = GetSex();
		crossProtocol.level = GetLevel();
		crossProtocol.viplvl = GetVipLvl();
		crossProtocol.objname = GetName();
		crossProtocol.receiverId = targetId;
		crossProtocol.word = word;
		crossProtocol.bLink = isLink;
		crossProtocol.voiceKey = voiceKey;
		crossProtocol.voiceDuration = voiceDur;

		crossProtocol.isGm = HasGmAuthority(GM_AUTHORITY_SUPER);
		crossProtocol.headFrame = GetHeadFrameId();

		Avalon::Packet* packet = Avalon::Packet::Create();
		packet->SetID(CLProtocol::CROSS_TRANSMIT_CHAT);
		Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
		try
		{
			stream & GetRoomId() & crossProtocol;
		}
		catch (Avalon::NetException&)
		{
			Avalon::Packet::Destroy(packet);
			return;
		}
		packet->SetSize(stream.GetPos());
		Router::SendToCross(packet);
		Avalon::Packet::Destroy(packet);
	}

	if (channel == CHAT_CHANNEL_BATTLE)
	{
		UInt32 battleId = this->GetBattleID();
		LostBattleScene* battleScene = dynamic_cast<LostBattleScene*>(SceneMgr::Instance()->GetDynamicScene(battleId));

		if (!battleScene)
		{
			return;
		}

		CLProtocol::SceneSyncChat protocol;
		protocol.channel = channel;
		protocol.objid = GetID();
		protocol.occu = GetOccu();
		protocol.sex = GetSex();
		protocol.level = GetLevel();
		protocol.viplvl = GetVipLvl();
		protocol.objname = GetName();
		protocol.receiverId = targetId;
		protocol.word = word;
		protocol.bLink = isLink;
		protocol.voiceKey = voiceKey;
		protocol.voiceDuration = voiceDur;

		protocol.isGm = HasGmAuthority(GM_AUTHORITY_SUPER);
		protocol.headFrame = GetHeadFrameId();

		battleScene->Broadcast(protocol);
	}
	else if (channel == CHAT_CHANNEL_BAT_LOST_TEAM)
	{
		UInt32 battleId = this->GetBattleID();
		LostBattleScene* battleScene = dynamic_cast<LostBattleScene*>(SceneMgr::Instance()->GetDynamicScene(battleId));
		if (!battleScene)
		{
			return;
		}

		DungeonEntryScene* scene = dynamic_cast<DungeonEntryScene*>(GetScene());
		if (!scene)
		{
			return;
		}

		CLProtocol::SceneSyncChat protocol;
		protocol.channel = channel;
		protocol.objid = GetID();
		protocol.occu = GetOccu();
		protocol.sex = GetSex();
		protocol.level = GetLevel();
		protocol.viplvl = GetVipLvl();
		protocol.objname = GetName();
		protocol.receiverId = targetId;
		protocol.word = word;
		protocol.bLink = isLink;
		protocol.voiceKey = voiceKey;
		protocol.voiceDuration = voiceDur;

		protocol.isGm = HasGmAuthority(GM_AUTHORITY_SUPER);
		protocol.headFrame = GetHeadFrameId();

		LostDungeonTeam* team = LostDungeonMgr::Instance()->GetPlayerTeam(GetScene()->GetID(), GetID());
		team->VistMembers([&](LostDungeonTeam* team, LostDungTeamMember* teamMember)
		{
			auto member = PlayerMgr::Instance()->FindPlayer(teamMember->roleId);
			if (member)
			{
				member->SendProtocol(protocol);
			}
			return true;
		});
	}


	//扣除世界聊天需要的活跃度
	if (channel == CHAT_CHANNEL_WORLD)
	{
		if (dayFreeNum >= vipFreeNum)
		{
			SetCounter(DAILY_ACTIVE_SCORE, actScore - needActScore);
		}
		else
		{
			IncCounter(DAILY_WORLD_CHAT_FREE, 1);
		}
	}
}

UInt32 Player::ParseSpeakWords(const std::string inWord, std::string& outWords, std::set<Item*>& linkItems, 
	std::set<ObjID_t>& linkAuctionItems, bool& isLink, UInt32 maxShowWidth)
{
	char wordstr[CL_MAX_CHAT_LEN + 1];
	memset(wordstr, 0, sizeof(wordstr));
	avalon_strncpy(wordstr, inWord.c_str(), CL_MAX_CHAT_LEN);

	//解析
	MsgParser parser;
	if (!parser.Parse(wordstr)) return CHAT_PARSER_ERR;

	//标签访问
	class ChatTagVisitor : public TagVisitor
	{
	public:
		ChatTagVisitor(Player* player, std::set<Item*>& items, std::set<ObjID_t> &auctionItems)
			:m_pPlayer(player), m_LinkItems(items), m_LinkAuctionItems(auctionItems), m_ShowWidth(0), m_bLink(false){}

		bool operator()(MsgTag* tag)
		{
			switch (tag->GetType())
			{
			case 'I'://道具链接
			{
						 char idstr[64];
						 memset(idstr, 0, sizeof(idstr));
						 if (!tag->GetParam(0, idstr, 64)) return false;
						 UInt64 objid = Avalon::StringUtil::ConvertToValue<UInt64>(idstr);

						 Item* item = m_pPlayer->FindCarryOnItem(objid);
						 if (item == NULL) return false;
						 if (m_LinkItems.find(item) == m_LinkItems.end())
							 m_LinkItems.insert(item);

						 //tag->ClearParams();

						 if (tag->GetContent() != NULL) return false;

						 int len = GetUtf8ShowWidth(item->GetName().c_str());
						 if (len < 0) return false;
						 m_ShowWidth += len;
						 m_bLink = true;
			}
				break;
			case 'A'://商城道具
			{
						 char idstr[64];
						 memset(idstr, 0, sizeof(idstr));
						 if (!tag->GetParam(0, idstr, 64)) return false;
						 UInt64 objid = Avalon::StringUtil::ConvertToValue<UInt64>(idstr);

						 UInt32	uDataId = 0;
						 char szBuff[64];
						 if (!tag->GetParam(1, szBuff, 64)) return false;
						 uDataId = UInt32(atoi(szBuff));

						 ItemDataEntry	*pDataEntry = ItemDataEntryMgr::Instance()->FindEntry(uDataId);
						 if (NULL == pDataEntry) return false;

						 m_LinkAuctionItems.insert(objid);

						 //tag->ClearParams();
						 if (tag->GetContent() != NULL) return false;

						 int len = GetUtf8ShowWidth(pDataEntry->name.c_str());
						 if (len < 0) return false;
						 m_ShowWidth += len;
						 m_bLink = true;
			}
				break;
			case 'E'://宠物
			{
						 char idstr[64];
						 memset(idstr, 0, sizeof(idstr));
						 if (!tag->GetParam(0, idstr, 64)) return false;
						 
						 //SleepingPet* pet = m_pPlayer->GetPetMgr().FindPet(objid);
						 //if(pet == NULL) return false;
						 //if(m_LinkPets.find(pet) != m_LinkPets.end()) return false;

						 //tag->ClearParams();

						 //m_LinkPets.insert(pet);
						 //tag->AddParam(idstr);
						 //tag->AddParam(pet->GetQuality());

						 //if(tag->GetContent() == NULL || strncmp(tag->GetContent(), pet->GetName().c_str(), NAME_LENGTH) != 0) return false;

						 int len = GetUtf8ShowWidth(tag->GetContent());
						 if (len < 0) return false;
						 m_ShowWidth += len;
						 m_bLink = true;
			}
				break;
			case 'F'://表情
			{
						 m_ShowWidth += 2;
						 if (tag->GetContent() != NULL) return false;
						 m_bLink = true;
			}
				break;
			case 'P'://玩家
			{
						 m_bLink = true;
			}
				break;
			case 0:
			case 1:
			{
					  if (!tag->GetContent())
					  {
						  return false;
					  }

					  int len = GetUtf8ShowWidth(tag->GetContent());
					  if (len < 0) return false;
					  m_ShowWidth += len;

					  //检查屏蔽字
					  if (ForbitWordParser::Instance()->Parse(tag->GetContent()) == FORBIT_WORD_NOUTF8) return false;
			}
				return true;
			case 'L':
			case 'T':
			case 'O':
			case 'H':
			case 'S':
			case 'D':
			case 'X':
				m_bLink = true;
				return true;
			default://不允许
				return false;
			}

			return true;
		}

		int GetShowWidth() const { return m_ShowWidth; }
		bool	IsLink() const { return m_bLink; }
	private:
		//玩家
		Player*	m_pPlayer;
		//链接中的装备
		std::set<Item*>&	m_LinkItems;
		//链接中的拍卖行道具
		std::set<ObjID_t>	&m_LinkAuctionItems;
		//显示宽度
		int		m_ShowWidth;
		//是否非纯文字
		bool	m_bLink;
	};

	ChatTagVisitor visitor(this, linkItems, linkAuctionItems);
	if (!parser.Visit(visitor)) return CHAT_PARSER_ERR;
	if ((UInt32)visitor.GetShowWidth() > maxShowWidth) return CHAT_MAX_SHOW_WIDTH_ERR;

	//重新导出
	int len = parser.Dump(wordstr, CL_MAX_CHAT_LEN);
	if (len < 0) return CHAT_DUMP_WORD_ERR;
	wordstr[len] = 0;

	outWords = std::string(wordstr);
	isLink = visitor.IsLink();

	return SUCCESS;
}

void Player::ScriptSpeak(UInt8 channel, const char* word)
{
	Speak(channel, word);
}

UInt32 Player::SendHorn(const std::string& content, UInt8 num)
{
	if (m_ForbitTalkDueTime > CURRENT_TIME.Sec())
	{
		std::string timestr = SysNotifyMgr::Instance()->TimeToStr(m_ForbitTalkDueTime - UInt32(CURRENT_TIME.Sec()));
		// 提示禁言
		SendNotify(1064, timestr.c_str());
		return ErrorCode::HORN_FORBID_TALK;
	}

	// 目前只支持单个喇叭或者10连发
	if (num != 1 && num != 10)
	{
		ErrorLogStream << PLAYERINFO(this) << " want to send horn, invalid num(" << num << ")." << LogStream::eos;
		return ErrorCode::HORN_SYSTEM_ERROR;
	}

	// 处理屏蔽字
	std::string newContent;
	std::set<Item*> linkItems;
	std::set<ObjID_t> linkAuctionItems;
	bool isLink;
	auto maxWidth = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_HORN_CONTENT_MAX_WIDTH);
	auto checkResult = ParseSpeakWords(content, newContent, linkItems, linkAuctionItems, isLink, maxWidth);
	if (SUCCESS != checkResult)
	{
		ErrorLogStream << PLAYERINFO(this) << " want to send horn, content invalid(" << checkResult << ")." << LogStream::eos;
		return checkResult;
	}

	// 判断喇叭够不够，并且扣相应的道具
	UInt32 hornItemId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_HORN_ID);
	UInt32 hornItemNum = GetItemNum(hornItemId);
	std::string reason = GetReason(SOURCE_SEND_HORN, num);
	if (hornItemNum < num)
	{
		// 喇叭不够直接用钱
		/*QuickBuyDataEntry* quickData = QuickBuyDataEntryMgr::Instance()->FindEntry(hornItemId);
		if (!quickData)
		{
			ErrorLogStream << PLAYERINFO(this) << " want to send horn, can't find horn item's quick buy data(" << hornItemId << ")." << LogStream::eos;
			return ErrorCode::HORN_SYSTEM_ERROR;
		}

		UInt32 needNum = (UInt32)num - hornItemNum;
		if (GetItemNum(quickData->costItemId) < quickData->costNum * needNum)
		{
			ErrorLogStream << PLAYERINFO(this) << " want to send horn, need item(" << quickData->costItemId << ") num(" << needNum << ")." << LogStream::eos;
			return ErrorCode::HORN_NOT_ENOUGH_MONEY;
		}

		// 先扣有的喇叭
		RemoveItem(reason.c_str(), ITEM_HORN, hornItemNum);
		// 再扣快速购买需要的道具
		RemoveItem(reason.c_str(), quickData->costItemId, quickData->costNum * needNum);*/

		ErrorLogStream << PLAYERINFO(this) << " want to send horn, need hornitem(" << hornItemId << ") num(" << num << ")." << LogStream::eos;
		return ErrorCode::HORN_NOT_ENOUGH;
	}
	else
	{
		RemoveItem(reason.c_str(), hornItemId, num);
	}

	// 计算连发
	if ((UInt32)CURRENT_TIME.Sec() > m_LastSendHornTime + SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_HORN_COMBO_WAIT_SEC))
	{
		m_HornCombo = 0;
	}

	m_HornCombo += num;
	m_LastSendHornTime = (UInt32)CURRENT_TIME.Sec();

	// 让World去发送
	CLProtocol::WorldChatHorn req;
	req.info.roldId = GetID();
	req.info.name = GetName();
	req.info.level = GetLevel();
	req.info.viplvl = GetVipLvl();
	req.info.occu = GetOccu();
	req.info.content = newContent;
	req.info.num = num;
	req.info.headFrame = GetHeadFrameId();
	req.info.minTime = num == 1 ? SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_HORN_MIN_SEC) : SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_HORN_10_MIN_SEC);
	req.info.maxTime = num == 1 ? SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_HORN_MAX_SEC) : SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_HORN_10_MAX_SEC);
	req.info.combo = m_HornCombo;
	Router::SendToWorld(req);

	// 链接信息
	for (std::set<Item*>::iterator iter = linkItems.begin();
		iter != linkItems.end(); ++iter)
	{
		WorldCacheItemData(*iter);
	}

	for (std::set<ObjID_t>::iterator iter = linkAuctionItems.begin();
		iter != linkAuctionItems.end(); iter++)
	{
		Avalon::Packet* packet = Avalon::Packet::Create();
		packet->SetID(CLProtocol::WORLD_CACHE_CHATDATA_REQ);

		Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
		try
		{
			stream & 'A' & *iter;
		}
		catch (Avalon::NetException& e)
		{
			ErrorLogStream << PLAYERINFO(this) << " encode cache failed, " << e.what() << LogStream::eos;
			Avalon::Packet::Destroy(packet);
			return ErrorCode::HORN_SYSTEM_ERROR;
		}
		packet->SetSize(UInt16(stream.GetPos()));
		Router::SendToWorld(packet);

		Avalon::Packet::Destroy(packet);
	}

	CLProtocol::CenterSyncChatInfo protocol;
	protocol.serverId = ZONE_ID;
	protocol.accId = GetAccID();
	protocol.channel = CHAT_CHANNEL_HORN;
	protocol.time = (UInt32)CURRENT_TIME.Sec();
	protocol.roleUid = GetID();
	protocol.name = GetName();
	protocol.word = newContent;
	protocol.pf = PLATFORM;
	protocol.qudao = GetPf();
	Router::SendToCenter(protocol);

	OnSendHorn();

	return ErrorCode::SUCCESS;
}

void Player::ReportCustomLog(CustomLogReportType type, const std::string& param)
{
	std::vector<std::string> paramStrs;
	Avalon::StringUtil::Split(param, paramStrs, "|");
	if (paramStrs.empty())
	{
		ErrorLogStream << PLAYERINFO(this) << "Report custom log(" << (UInt8)type << "), param is empty!" << LogStream::eos;
		return;
	}

	DebugLogStream << PLAYERINFO(this) << "Report custom log(type=" << (UInt8)type << ", param=" << param << ")." << LogStream::eos;

	switch (type)
	{
	case CLRT_JOIN_VOICE_ROOM:
	case CLRT_QUIT_VOICE_ROOM:
		if (paramStrs.size() != 2)
		{
			ErrorLogStream << PLAYERINFO(this) << "Report custom log(" << (UInt8)type << "), error param size(" << (UInt32)param.size() << ")!" << LogStream::eos;
			return;
		}
		SendLiveVoiceDataUdpLog((UInt8)type, paramStrs[0], paramStrs[1]);
		break;

	case CLRT_SEND_RECORD_VOICE:
	case CLRT_LOAD_RECORD_VOICE:
		SendChatVoiceDataUdpLog((UInt8)type, paramStrs.front());
		break;

	default:
		ErrorLogStream << PLAYERINFO(this) << "Error custom log type(" << (UInt8)type << ")!" << LogStream::eos;
		break;
	}
}

void Player::SendPos(UInt8 channel, ObjID_t targetId)
{
	if(channel == 0 || channel > CHAT_CHANNEL_MAX) return;

	switch(channel)
	{
	case CHAT_CHANNEL_TEAM:
	case CHAT_CHANNEL_TEAM_SPECIAL:
		{
			if(GetTeam() == NULL) return;
		}
		break;
	}

	//检查聊天时间
	if(m_ChatTimer.CheckChatTime(channel, CURRENT_TIME.MSec())) return;

	char wordstr[CL_MAX_CHAT_LEN + 1];
	memset(wordstr, 0, sizeof(wordstr));
	avalon_snprintf(wordstr, CL_MAX_CHAT_LEN, "{W %u 0 %u %u 0|%s(%u,%u)}",  GetScene()->GetMapID(), GetPos().x, GetPos().y
		, GetScene()->GetName().c_str(), GetPos().x, GetPos().y);
	Creature::Speak(channel, wordstr, "", 0, targetId);
	m_ChatTimer.SetChatTime(channel, CURRENT_TIME.MSec());
}

void Player::Notify(UInt16 type, UInt8 color, const char* word)
{
	if(word == NULL) return;

	CLProtocol::SysNotify notify;
	notify.type = type;
	notify.channel = 0;
	notify.color = color;
	notify.word = word;
	SendProtocol(notify);
}

void Player::SetTeam(Team* team)
{
	if(m_pTeam != team)
	{
		m_pTeam = team;
	}
}

void Player::CreateTeam()
{
	CLProtocol::WorldSceneCreateTeamReq req;
	req.playerId = GetID();
	Router::SendToWorld(req);
}

void Player::UpdateTeamInfo()
{	
	if(m_pTeam != NULL)
	{
		m_isTeamMaster = UInt8(m_pTeam->GetMaster() == GetID() ? 1 : 0);
		m_TeamSize = m_pTeam->GetSize();
	}
	else
	{
		m_isTeamMaster = 0;
		m_TeamSize = 0;
	}
}

bool Player::IsTeamMaster() const
{
	if(m_pTeam != NULL && m_pTeam->GetMaster() == GetID()) return true;
	return false;
}

UInt8 Player::GetTeamSize() const
{
	UInt8 size = 0;
	if (m_pTeam != NULL)
	{
		size = m_pTeam->GetSize();
	}
	return size;
}

bool Player::IsTeammate(Creature* creature) const
{
	if(creature == NULL || creature->GetType() != SOT_PLAYER) return false;
	if(m_pTeam == NULL) return false;

	return m_pTeam->IsMember(creature->GetID());
}

void Player::SetRelationList(RelationType type, std::map<ObjID_t, UInt16>& list)
{
	if (type < RelationType::RELATION_FRIEND || type > RelationType::RELATION_MAX)
	{
		return;
	}

	std::map<ObjID_t, UInt16>& rList = m_Relations[type - 1];
	for (auto itr = list.begin(); itr != list.end(); ++itr)
	{
		rList.insert(std::make_pair(itr->first, itr->second));
	}
}

void Player::AddRelation(RelationType type, ObjID_t tarId)
{
	if (type < RelationType::RELATION_FRIEND || type > RelationType::RELATION_MAX)
	{
		return;
	}

	for (int i = RELATION_FRIEND; i < RELATION_MAX; ++i)
	{
		auto itr = m_Relations[i].find(tarId);
		if (itr != m_Relations[i].end())
		{
			m_Relations[i].erase(itr);
			break;
		}
	}
	
	std::map<ObjID_t, UInt16>& rList = m_Relations[type - 1];
	rList.insert(std::make_pair(tarId, 0));
}

//废弃
void Player::SendWatchData(ObjID_t watcher, const std::string& tribe)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::SCENE_SYNC_WATCH_INFO);
	Avalon::NetOutputStream stream1(packet->GetBuffer(), packet->GetMaxSize());
	try
	{
		stream1 & GetID();
		stream1 & tribe;
		Encode(stream1, SST_WATCH, false);
	}
	catch(const Avalon::NetException&)
	{
		Avalon::Packet::Destroy(packet);
		return;
	}
	packet->SetSize(stream1.GetPos());
	Router::TransmitToPlayer(watcher, packet);
	Avalon::Packet::Destroy(packet);

	m_ItemMgr.SyncEquipsToWatcher(watcher);
}

void Player::SetItemBar(UInt8 slot, UInt32 id)
{
}

void Player::GetRaceSkills(std::vector<RaceSkillInfo>& raceSkills, bool isPvp)
{

	// 获取技能信息
	class RaceSkillVistor : public SkillVisitor
	{
	public:
		RaceSkillVistor(Player* player, std::vector<RaceSkillInfo>& raceSkills, bool isPvp)
			:m_player(player), m_RaceSkills(raceSkills), m_IsPvp(isPvp) { }

		virtual bool operator()(Skill* skill)
		{
			if (skill && m_player)
			{
				UInt8 slot = 0;
				bool isSync = false;
				if (IsGuildSkill(skill->dataEntry->type))	//公会技能下发
				{
					isSync = true;
				}
				else if (IsAwaken(skill->dataEntry->type))
				{
					isSync = true;
				}
				else if (IsOccupation(skill->dataEntry->type) && IsPassiveSkill(skill->dataEntry->useType))		// 职业被动技能
				{
					isSync = true;
				}
				else if (skill->dataEntry->isBuff)			//buff下发
				{
					isSync = true;
				}
				else if (skill->dataEntry->isQTE)			//QTE技能下发
				{
					isSync = true;
				}
				else
				{
					if (SERVER_TYPE == ST_BSCENE)
					{
						//是PVP战斗.且使用pvp配置
						if (m_player->GetPvpSkillMgr().IsSetting(skill->dataEntry))
						{
							slot = m_player->GetPvpSkillSlot(skill->id);
							if (slot)
							{
								isSync = true;
							}
						}
					}
					else if (m_IsPvp && m_player->GetCounter(PVP_SKILL_CONFIG))
					{
						//是PVP战斗.且使用pvp配置
						if (m_player->GetPvpSkillMgr().IsSetting(skill->dataEntry))
						{
							slot = m_player->GetPvpSkillSlot(skill->id);
							if (slot)
							{
								isSync = true;
							}
						}
					}
					else
					{
						if (m_player->GetSkillMgr().IsSetting(skill->dataEntry))
						{
							slot = m_player->GetSkillSlot(skill->id);
							if (slot)
							{
								isSync = true;
							}
						}
					}
				}

				//如果强制下发,即不管任何条件都下发
				if (skill->dataEntry->isForceSync)
				{
					isSync = true;
				}

				if (isSync)
				{
					RaceSkillInfo skillInfo;
					skillInfo.id = skill->id;
					skillInfo.level = skill->level;
					skillInfo.slot = slot;
					m_RaceSkills.push_back(skillInfo);
				}
			}
			return true;
		}

		Player*							m_player;
		std::vector<RaceSkillInfo>&		m_RaceSkills;
		bool							m_IsPvp;
	};
	RaceSkillVistor vistor(this, raceSkills, isPvp);

	if (SERVER_TYPE == ST_BSCENE)
	{
		GetPvpSkillMgr().VisitSkills(vistor);
	}
	else if (isPvp && GetCounter(PVP_SKILL_CONFIG))
	{
		GetPvpSkillMgr().VisitSkills(vistor);
	}
	else
	{
		GetSkillMgr().VisitSkills(vistor);
	}
}

void Player::WorldCacheItemData(Item* item)
{
	if (!item)
	{
		return;
	}

	Avalon::Packet* packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::WORLD_CACHE_CHATDATA_REQ);

	Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
	try
	{
		stream & 'I';
		stream & item->GetID() & item->GetDataID();
		item->PackWatchData(stream);
		stream & ObjID_t(0);
	}
	catch (Avalon::NetException&)
	{
		Avalon::Packet::Destroy(packet);
		return;
	}
	packet->SetSize(UInt16(stream.GetPos()));
	Router::SendToWorld(packet);

	Avalon::Packet::Destroy(packet);
}

void Player::WorldCacheRetinueData(Retinue* retinue)
{
	if (retinue == NULL) return;

	Avalon::Packet* packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::WORLD_CACHE_CHATDATA_REQ);
	Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());

	try
	{
		stream & MSG_TAGS[CLT_RETINUE];
		retinue->PackWatchData(stream);
		stream & ObjID_t(0);
	}
	catch (Avalon::NetException&)
	{
		Avalon::Packet::Destroy(packet);
		return;
	}
	packet->SetSize(UInt16(stream.GetPos()));
	Router::SendToWorld(packet);

	Avalon::Packet::Destroy(packet);
}

UInt32 Player::SetWeaponBarByIndex(UInt32 index, ObjID_t newId)
{
	if (newId != 0)
	{
		Item* item = m_ItemMgr.FindItem(newId);
		if (item == NULL) return ErrorCode::WEAPON_SETTING_NOT_FOUNT;

		if (item->GetSealState() == 1)
		{
			item->SetSealState(0);
			item->SyncProperty(this);
		}
	}

	return m_WeaponBar.SetBarByIndex(index, newId);
}

std::vector<UInt16> Player::GetPreOccuSkillId()
{
	std::vector<UInt16> result;
	if (GetPreOccu() == 0) return result;
	JobDataEntry* jobData = JobDataEntryMgr::Instance()->FindEntry(GetPreOccu());
	if (jobData == NULL) return result;
	return jobData->preJobSkills;
}

void Player::OnLeaveTimeout()
{
	Player* player = this;
	if (player == NULL || player->GetGameStatus() != PS_LEAVEGAME) return;

	DebugLogStream << "player(" << GetID() << ") on leave game timeout." << LogStream::eos;

	bool bDynMap = false;
	//离开场景
	if (player->GetScene() != NULL)
	{
		bDynMap = IsDynMap(player->GetScene()->GetType());
		player->GetScene()->RemoveObject(player);
	}

	//下线处理
	player->OnOffline();

	if (SERVER_TYPE == ST_WSCENE || SERVER_TYPE == ST_BSCENE)
	{
		PlayerMgr::Instance()->RemovePlayer(player, true);
		SceneObject::FreeObject(player);
	}
	else if (bDynMap)
	{
		/**
		*@brief 离开游戏保存
		*/
		class SavebackCallback : public CLRecordCallback
		{
		public:
			SavebackCallback(Player* player) :m_pPlayer(player){}
			~SavebackCallback(){ SceneObject::FreeObject(m_pPlayer); }

			void OnFinished(){ Final(); }

			void OnFailed(UInt32 errorcode)
			{
				Final();
				ErrorLogStream << "player(" << m_pPlayer->GetAccID() << "," << m_pPlayer->GetID() << "," <<
					m_pPlayer->GetName() << ") saveback to database failed!" << LogStream::eos;
			}

			void Final()
			{
				//向world注销玩家
				m_pPlayer->UnRegisterWorldPlayer();
			}

		private:
			//玩家
			Player*	m_pPlayer;
		};

		PlayerMgr::Instance()->RemovePlayer(player);
		player->Saveback(new SavebackCallback(player));
	}
	else
	{
		player->Saveback(NULL);

		//向world注销玩家
		player->UnRegisterWorldPlayer();
	}
}

void Player::LeaveGame()
{
	if (SERVER_TYPE == ST_BSCENE)
	{
		//离开吃鸡战场
		BattleScene* battleScene = dynamic_cast<BattleScene*>(GetScene());
		if (battleScene)
		{
			battleScene->LeaveBattle(GetBattleID(), GetID());
			SendNoWarToBattle();
			SetBabttleID(0);
		}
	}

	/**
	 *@brief 离开游戏定时器
	 */
	class LeaveGameTimer : public Avalon::TimerHandler
	{
	public:
		LeaveGameTimer(ObjID_t id):m_PlayerId(id){}

		void OnTimeout(const Avalon::Time& now)
		{	auto player = PlayerMgr::Instance()->FindPlayer(m_PlayerId);
			if (player)
			{
				player->OnLeaveTimeout();
			}
		}

	private:
		//玩家id
		ObjID_t m_PlayerId;
	};

	switch(GetGameStatus())
	{
	case PS_FINAL:
    //case PS_RECONNECT:
	case PS_LEAVEGAME: //正在离开游戏
	{
		DebugLogStream << "player(" << GetID() << ") want to logout, invalid status(" << (UInt8)GetGameStatus() << ")." << LogStream::eos;
		return;
	}
	case PS_RECONNECT:
	case PS_INIT:	//还没上线，只是释放对象数据
		{	
			UnRegisterWorldPlayer();

			if (GetScene() != NULL)
			{
				ErrorLogStream << "player(" << GetID() << ") need to destory, already in scene." << LogStream::eos;
				GetScene()->RemoveObject(this);
			}

			PlayerMgr::Instance()->RemovePlayer(this, true);
			SceneObject::FreeObject(this);
		}
		return;
	case PS_NORMAL:
		{
			DebugLogStream << "player(" << GetID() << ") start to logout." << LogStream::eos;

			//逻辑离线
			OnLogicOffLine();

			//等待存档
			SetGameStatus(PS_LEAVEGAME);
			Avalon::TimerReactor::Instance()->ScheduleOnce(new LeaveGameTimer(GetID()), Player::LEAVEGAME_TIMEOUT);
		}
		return;
	}

}

void Player::UnRegisterWorldPlayer()
{
	DebugLogStream << "player(" << GetID() << ") logout, send unregister request to worldserver." << LogStream::eos;
	CLProtocol::WorldUnregisterPlayerReq req;
	req.id = GetID();
	req.sceneId = GetSceneID();
	req.pos = GetPos();
	Router::SendToWorld(req);
}

void Player::LeaveGameForTour(UInt32 dstZoneId, UInt32 mapId, const Avalon::SockAddr& addr, UInt8* key)
{
	if(SERVER_TYPE == ST_WSCENE) return;
	if(dstZoneId == ZONE_ID) return;

	SetGameStatus(PS_LEAVEGAME);

	if(GetScene() != NULL) GetScene()->RemoveObject(this);
	OnOffline();

	class SavebackCallback : public CLRecordCallback
	{
	public:
		SavebackCallback(Player* player, UInt32 dstZoneId, const Avalon::SockAddr& addr, UInt8* key)
			:m_pPlayer(player),m_dstZoneId(dstZoneId),m_Addr(addr){ memmove(m_Key, key, 20); }
		~SavebackCallback(){ SceneObject::FreeObject(m_pPlayer); }

		void OnFinished(){ Final(); }

		void OnFailed(UInt32 errorcode)
		{
			Final();
			ErrorLogStream << "player(" << m_pPlayer->GetAccID() << "," << m_pPlayer->GetID() << "," <<
				m_pPlayer->GetName() << ") saveback to database failed!" << LogStream::eos;
		}

		void Final()
		{
			//通知world跨服
			CLProtocol::WorldChangeZoneNotify notify;
			notify.playerId = m_pPlayer->GetID();
			notify.dstZoneId = m_dstZoneId;
			notify.attr = m_Addr;
			memmove(notify.key, m_Key, 20);
			Router::SendToWorld(notify);
		}

	private:
		//玩家
		Player*	m_pPlayer;
		//目标区id
		UInt32	m_dstZoneId;
		//目标区网关地址
		Avalon::SockAddr m_Addr;
		//口令
		UInt8	m_Key[20];
	};

	if(GetSrcZoneId() == dstZoneId)	m_TourZoneId = 0;
	else m_TourZoneId = dstZoneId;
	m_MajorCityId = mapId;
	PlayerMgr::Instance()->RemovePlayer(this);
	Saveback(new SavebackCallback(this, dstZoneId, addr, key));
}

void Player::KickOff(UInt32 erroCode)
{
	//通知网关踢下线
	CLProtocol::GateKickOffPlayer kickOff;
	kickOff.id = GetID();
	kickOff.errorCode = erroCode;
	SendToGate(kickOff);
	m_Conn = NULL;
}

bool Player::SetActionFlag(UInt32 flag)
{
	if(flag >= ACTION_FLAG_MAX) return false;

	UInt32 value = m_ActionFlag;
	flag = 1 << flag;
	if((value & flag) != 0) return false;

	m_ActionFlag = value | flag;
	return true;
}

bool Player::TestActionFlag(UInt32 flag) const
{
	if(flag >= ACTION_FLAG_MAX) return false;

	UInt32 value = m_ActionFlag;
	flag = 1 << flag;
	return (value & flag) != 0;
}

UInt8 Player::IncLoadDataCount()
{ 
	++m_LoadDataCount;
	m_LoadDataSerial |= (UInt64)1 << m_LoadDataCount;
	return m_LoadDataCount;
}

bool Player::CheckLoadDataSerial(UInt8 serial)
{ 
	if ((m_LoadDataSerial & ((UInt64)1 << serial)) != 0)
	{
		return true;
	}

	return false; 
}

void Player::SetLoadDataFinished(UInt8 serial)
{
	if (CheckLoadDataSerial(serial))
	{
		m_LoadDataSerial = m_LoadDataSerial & (~((UInt64)1 << serial));
		if(m_LoadDataSerial == 0)
		{
			LoadingFinish();
			m_LoadDataSerial = 0;
		}
	}
}

void Player::LoadingFinish()
{
    for (UInt32 i = 0; i < m_LoadingCallback.size(); i++)
    {
        auto cb = m_LoadingCallback[i];
        if (cb)
        {
            cb->OnFinish(this);
            delete cb;
            m_LoadingCallback[i] = NULL;
        }
    }

    m_LoadingCallback.erase(std::remove(m_LoadingCallback.begin(), m_LoadingCallback.end(), (PlayerLoadingCallback*)NULL), m_LoadingCallback.end());

	CLProtocol::SceneNotifyWorldLoadFinish protocol;
	protocol.roleId = GetID();
	Router::SendToWorld(protocol);
}

void Player::LoadingFailed()
{
	for (UInt32 i = 0; i < m_LoadingCallback.size(); i++)
	{
		auto cb = m_LoadingCallback[i];
		if (cb)
		{
			cb->OnFailure(this);
			delete cb;
			m_LoadingCallback[i] = NULL;
		}
	}

	m_LoadingCallback.erase(std::remove(m_LoadingCallback.begin(), m_LoadingCallback.end(), (PlayerLoadingCallback*)NULL), m_LoadingCallback.end());

	// 加载失败，直接进入代摧毁状态
	SetGameStatus(PS_DESTORY);
}

void Player::StartLoadData(PlayerLoadingCallback* cb)
{
	if (SERVER_TYPE == ST_BSCENE)
	{
		return;
	}

    if (!cb)
    {
        ErrorLogStream << "player(" << GetID() << ") want to start load data, but no callback." << LogStream::eos;
        return;
    }

	auto player = this;

    // 加载各个数据
    CLSelectCommand* selectcmd = CLRecordClient::Instance()->PrepareSelectCommand(player->GetSrcZoneId(), "t_player_info");
	selectcmd->PutCond("guid", GetID());
    selectcmd->PutCond("deletetime", 0);
    CLRecordClient::Instance()->SendCommand(selectcmd, new SelectMainDataCallback(player));

    //请求计数数据
    CLSelectCommand* selectcounters = CLRecordClient::Instance()->PrepareSelectCommand(player->GetSrcZoneId(), "t_counter");
	selectcounters->PutCond("owner", GetID());
    CLRecordClient::Instance()->SendCommand(selectcounters, new SelectCountersCallback(player));

    //请求活动礼包数据
//     CLSelectCommand* selectActGift = CLRecordClient::Instance()->PrepareSelectCommand(player->GetSrcZoneId(), "t_activitygiftbag");
// 	selectActGift->PutCond("owner", GetID());
//     CLRecordClient::Instance()->SendCommand(selectActGift, new SelectActGiftBagCallback(player));

    //请求地下城数据
    CLSelectCommand* selectLevel = CLRecordClient::Instance()->PrepareSelectCommand(player->GetSrcZoneId(), "t_dungeon");
	selectLevel->PutCond("owner", GetID());
    CLRecordClient::Instance()->SendCommand(selectLevel, new SelectDungeonsCallback(player));

    //请求地下城难度数据
    /*
	CLSelectCommand* selectDungeonHard = CLRecordClient::Instance()->PrepareSelectCommand(player->GetSrcZoneId(), "t_g_dungeon_hard");
	selectDungeonHard->PutCond("account", GetAccID());
    CLRecordClient::Instance()->SendCommand(selectDungeonHard, new SelectDungeonsHardInfoCallback(player));
	*/

    //请求仓库道具数据
	//LoadAccountStorage(NULL, false);

    //请求商店数据
    CLSelectCommand* selectShop = CLRecordClient::Instance()->PrepareSelectCommand(player->GetSrcZoneId(), "t_shop");
    selectShop->PutCond("owner", player->GetID());
    CLRecordClient::Instance()->SendCommand(selectShop, new SelectShopCallback(player));

    //请求商品数据
    CLSelectCommand* selectShopItem = CLRecordClient::Instance()->PrepareSelectCommand(player->GetSrcZoneId(), "t_shopitem");
    selectShopItem->PutCond("owner", player->GetID());
    CLRecordClient::Instance()->SendCommand(selectShopItem, new SelectShopItemCallback(player));

    //请求pk统计数据
    CLSelectCommand* selectPkInfoes = CLRecordClient::Instance()->PrepareSelectCommand(player->GetSrcZoneId(), "t_pk_statistic");
    selectPkInfoes->PutCond("owner", player->GetID());
    CLRecordClient::Instance()->SendCommand(selectPkInfoes, new SelectPkStatisticCallback(player));

    //请求随从数据
	if (SSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_RETINUE))
	{
		CLSelectCommand* selectRetinue = CLRecordClient::Instance()->PrepareSelectCommand(player->GetSrcZoneId(), "t_retinue");
		selectRetinue->PutCond("owner", GetID());
		CLRecordClient::Instance()->SendCommand(selectRetinue, new SelectRetinueCallback(player));
	}

	//宠物数据
	CLSelectCommand* selectPet = CLRecordClient::Instance()->PrepareSelectCommand(player->GetSrcZoneId(), "t_pet");
	selectPet->PutCond("owner", GetID());
	CLRecordClient::Instance()->SendCommand(selectPet, new SelectPetCallback(player));

    //请求任务数据
    CLSelectCommand* selectTask = CLRecordClient::Instance()->PrepareSelectCommand(player->GetSrcZoneId(), "t_task");
	selectTask->PutCond("owner", GetID());
    CLRecordClient::Instance()->SendCommand(selectTask, new SelectTaskCallback(player));

    //请求活动任务数据
    CLSelectCommand* selectActiveTask = CLRecordClient::Instance()->PrepareSelectCommand(player->GetSrcZoneId(), "t_active_task");
	selectActiveTask->PutCond("owner", GetID());
    CLRecordClient::Instance()->SendCommand(selectActiveTask, new SelectActiveTaskCallback(player));

    //请求罐子奖池
    CLSelectCommand* selectJarPool = CLRecordClient::Instance()->PrepareSelectCommand(player->GetSrcZoneId(), "t_jaritem_pool");
	selectJarPool->PutCond("owner", GetID());
    CLRecordClient::Instance()->SendCommand(selectJarPool, new SelectJarPoolCallback(player));

	//请求运营活动任务数据
	//CLSelectCommand* selectOpTask = CLRecordClient::Instance()->PrepareSelectCommand(player->GetSrcZoneId(), "t_activity_op_task");
	//selectOpTask->PutCond("guid", GetID());
	//CLRecordClient::Instance()->SendCommand(selectOpTask, new SelectOpTaskCallback(player));

	//请求新运营活动任务数据
	CLSelectCommand* selectNewOpTask = CLRecordClient::Instance()->PrepareSelectCommand(player->GetSrcZoneId(), "t_activity_op_task_new");
	selectNewOpTask->PutCond("owner_id", GetID());
	CLRecordClient::Instance()->SendCommand(selectNewOpTask, new SelectNewOpTaskCallback(player));

	{
		GetAbnormalTransactionMgr().ResetPlayerOnlineLoadDBCount();

		CLSelectCommand* selectCmd = CLRecordClient::Instance()->PrepareSelectCommand(player->GetSrcZoneId(), "t_abnormal_transaction");
		selectCmd->PutCond("seller", GetID());
		selectCmd->PutCond("freeze_object", (UInt8)AT_FREEZE_OBJECT_SELLER);
		std::string limitStr = "AND `handle_status` BETWEEN ";
		limitStr += Avalon::StringUtil::ConvertToString<UInt8>((UInt8)ATHS_FROZEN_TAIL);
		limitStr += " AND ";
		limitStr += Avalon::StringUtil::ConvertToString<UInt8>((UInt8)ATHS_NEVER_BACK_MONEY);
		limitStr += " ORDER BY guid";
		selectCmd->SetLimit(limitStr);
		CLRecordClient::Instance()->SendCommand(selectCmd, new SelectFrozenAndNeverBackAbnormalTransactionCallback(player));
	}

	{
		CLSelectCommand* selectCmd = CLRecordClient::Instance()->PrepareSelectCommand(GetSrcZoneId(), "t_abnormal_transaction");
		selectCmd->PutCond("buyer", GetID());
		selectCmd->PutCond("freeze_object", (UInt8)AT_FREEZE_OBJECT_BUYER);
		std::string limitStr = "AND `handle_status` BETWEEN ";
		limitStr += Avalon::StringUtil::ConvertToString<UInt8>((UInt8)ATHS_FROZEN_TAIL);
		limitStr += " AND ";
		limitStr += Avalon::StringUtil::ConvertToString<UInt8>((UInt8)ATHS_NEVER_BACK_MONEY);
		limitStr += " ORDER BY guid";
		selectCmd->SetLimit(limitStr);
		CLRecordClient::Instance()->SendCommand(selectCmd, new SelectBuyerFrozenAndNeverBackAbnormalTransactionCallback(player));
	}

	//请求新运营活动任务数据
	CLSelectCommand* selectOpActAttrCmd = CLRecordClient::Instance()->PrepareSelectCommand(player->GetSrcZoneId(), "t_activity_op_attribute");
	selectOpActAttrCmd->PutCond("owner", GetID());
	CLRecordClient::Instance()->SendCommand(selectOpActAttrCmd, new SelectOpActivityAttibuteCallback(player));

    //请求道具数据，这个放在最后
    CLSelectCommand* selectitem = CLRecordClient::Instance()->PrepareSelectCommand(player->GetSrcZoneId(), "t_item");
	selectitem->PutCond("owner", GetID());
	selectitem->PutCond("pack", PACK_STORAGE, "<>");
    CLRecordClient::Instance()->SendCommand(selectitem, new SelectItemsCallback(player));

	//向ws加载账号信息

	/*auto accountCb = new SelectAccountDataCallback(this);
	AsyncMsgCallbackMgr::Instance()->Add(accountCb);
	CLProtocol::SceneQueryAccountReq accountReq;
	accountReq.accid = GetAccID();
	SEND_ASYNC_MSG(Router::SendToWorld, accountCb->GetRequestPacket(accountReq));*/

	//请求问卷调查信息
	CLSelectCommand* selectQuestionnaire = CLRecordClient::Instance()->PrepareSelectCommand(player->GetSrcZoneId(), "t_questionnaire");
	selectQuestionnaire->PutCond("owner", GetID());
	CLRecordClient::Instance()->SendCommand(selectQuestionnaire, new SelectQuestionnaireCallback(player));

	//请求活动账号数据
	CLSelectCommand* actAcc = CLRecordClient::Instance()->PrepareSelectCommand(player->GetSrcZoneId(), "t_activity_account_record");
	actAcc->PutCond("acc_id", GetAccID());
	CLRecordClient::Instance()->SendCommand(actAcc, new SelectOpActivityAccCallback(player));

	//请求头像框数据
	CLSelectCommand* headFrame = CLRecordClient::Instance()->PrepareSelectCommand(player->GetSrcZoneId(), "t_head_frame");
	headFrame->PutCond("owner_id", GetID());
	CLRecordClient::Instance()->SendCommand(headFrame, new SelectHeadFrameCallback(player));

	LoadAccountInfo(NULL);

    AddLoadingCallback(cb);

	CLProtocol::SceneNotifyWorldLoadStart notify;
	notify.roleId = GetID();
	Router::SendToWorld(notify);
}

void Player::LoadAccountInfo(PlayerLoadingCallback* cb)
{
	if (0 == GetAccID())
	{
		return;
	}

	// 清空仓库数据
	GetItemMgr().ClearPackageChache(PACK_STORAGE);

	CLProtocol::SceneQueryAccountReq accountReq;
	accountReq.accid = GetAccID();

	SEND_ASYNC_MSG2(Router::SendToWorld, new SelectAccountDataCallback(this), accountReq);

	AddLoadingCallback(cb);
}

void Player::LoadAccountStorage()
{
	CLSelectCommand* selectstorage = CLRecordClient::Instance()->PrepareSelectCommand(this->GetSrcZoneId(), "t_item");
	selectstorage->PutCond("accid", GetAccID());
	selectstorage->PutCond("pack", PACK_STORAGE);
	
	CLRecordClient::Instance()->SendCommand(selectstorage, new SelectStorageCallback(this));
}

void Player::AddLoadingCallback(PlayerLoadingCallback* cb)
{
    if (!cb)
    {
        return;
    }

    m_LoadingCallback.push_back(cb);
}

void Player::Saveback(CLRecordCallback* callback)
{
	if(GetGameStatus() == PS_INIT) return;

	//设置保存时间
	SetSavebackTime(CURRENT_TIME.MSec());

	//存回道具
	class SaveItemVisitor : public ItemVisitor
	{
	public:
		SaveItemVisitor(Player* player):m_pPlayer(player){}
		~SaveItemVisitor(){}

		bool operator()(Item* item)
		{
			if (!item)	return true;
		
			CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand(m_pPlayer->GetSrcZoneId(), "t_item", item->GetID(), true);
			item->GetDBData(cmd,true);
			CLRecordClient::Instance()->SendCommand(cmd);
			return true;
		}

	private:
		Player* m_pPlayer;
	};
	SaveItemVisitor saveItemVisitor(this);
	m_ItemMgr.VisitItems(saveItemVisitor);

	// 存回活动任务
	//m_ActiveTaskMgr.SaveToDB();

	//存回任务
	m_TaskMgr.SaveToDB();
	m_DailyTaskMgr.SaveToDB();
	m_AchievementTaskMgr.SaveToDB();
	m_CycleTaskMgr.SaveToDB();

	//存回随从数据
	m_RetinueMgr.SaveToDB();


	//存回玩家数据库，最后存
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand(GetSrcZoneId(), "t_player_info", GetID(), true);
	GetDBData(cmd,true);
	SceneDataEntry* dataEntry = SceneDataEntryMgr::Instance()->FindEntry(m_MajorCityId);
	if(dataEntry != NULL && IsStaticMap(dataEntry->type))
	{
		cmd->PutData("sceneid", GetSceneID());
		cmd->PutData("mapid",	m_MajorCityId);
		cmd->PutData("dir",		GetDir());
		cmd->PutData("pos",		GetPos().ToString());
	}
	cmd->PutData("savetime",	GetSavebackTime());
	cmd->PutData("totleonlinetime", m_totOnlineTime);
	if(GetSrcZoneId() == 0)
	{
		SetLocalSavebackTime(CURRENT_TIME.MSec());
		cmd->PutData("localsavetime", GetLocalSavebackTime());
	}
	CLRecordClient::Instance()->SendCommand(cmd, callback);
}

void Player::SaveOnlineData()
{
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand(GetSrcZoneId(), "t_player_info", GetID(), true);
	cmd->PutData("onlinetime", m_OnlineTime / 1000);
	cmd->PutData("login_ip", m_Ip);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void Player::OnTick(const Avalon::Time& now)
{
	// 检查加载回调
	for (UInt32 i = 0; i < m_LoadingCallback.size(); i++)
	{
		auto cb = m_LoadingCallback[i];
		if (cb && cb->IsTimeout(now))
		{
			cb->OnFailure(this);
			delete cb;
			m_LoadingCallback[i] = NULL;
		}
	}
	m_LoadingCallback.erase(std::remove(m_LoadingCallback.begin(), m_LoadingCallback.end(), (PlayerLoadingCallback*)NULL), m_LoadingCallback.end());
	
	if (SERVER_TYPE == ST_BSCENE)
	{
		BattleScene* battleScene = dynamic_cast<BattleScene*>(GetScene());
		if (!battleScene)
		{
			//InfoLogStream << "player:" << GetID() << ", not in battle scene" << LogStream::eos;
			return;
		}

		if (m_CheckBattleTimer.IsTimeout(now) && GetBattleID() != 0)
		{
			battleScene->CheckPlayerInPoisonRing(GetBattleID(), GetID(), GetScenePos());
		}

		// 免战牌超时14秒，开战
		if (m_ChijiNoWarChoiceTime != 0 && (UInt32)CURRENT_TIME.Sec() - m_ChijiNoWarChoiceTime > 14)
		{
			CLProtocol::BattleCheckNoWarRes res;
			res.roleId = GetChijiNoWarEnemyId();
			res.dstId = GetID();
			res.battleID = GetChijiNoWarBattleId();
			res.dungeonID = GetChijiPKDungeonId();
			res.isNoWar = 0;
			Router::SendToBattle(res);

			SetChijiNoWarInfo(0, 0, 0, 0);
		}
	}	
}

void Player::OnHeartbeat(const Avalon::Time& now)
{
	Creature::OnHeartbeat(now);

	if (SERVER_TYPE == ST_WSCENE)
	{
		/*if (now.MSec() >= m_LastUpdateWatchListTime + SSApplication::Instance()->GeWatchListUpdateIntervalMSec())
		{
			m_LastUpdateWatchListTime = now.MSec();
			if (GetScene() && GetScene()->CanSeeOthers() && SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_NEW_SCENE_SYNC))
			{
				CountNewWatchList2(true);
			}
		}*/

		if (now.MSec() >= m_LastUpdatePosTime + SSApplication::Instance()->GetMovePacketIntervalMSec())
		{
			m_LastUpdatePosTime = now.MSec();

			if (m_ScenePos.IsDirty(SST_MOVE) || m_SceneDir.IsDirty(SST_MOVE))
			{
				// 同步给周围的人

				CLProtocol::SceneSyncPlayerMove syncMove;
				syncMove.id = GetID();
				syncMove.pos = GetScenePos();
				syncMove.dir = GetSceneDir();
				BroadcastExceptMe(syncMove);

				m_ScenePos.ClearDirty(SST_MOVE);
				m_SceneDir.ClearDirty(SST_MOVE);

				if (SSApplication::Instance()->IsDebugAccount(GetAccID()))
				{
					DebugLogStream << "[test_move] player(" << GetID() << ") sync pos(" << syncMove.pos.GetX() << ", " << syncMove.pos.GetY()
						<< ") dir(" << syncMove.dir.GetX() << ", " << syncMove.dir.GetY() << ")" << LogStream::eos;
				}
			}
		}

		return;
	}

	if (SERVER_TYPE == ST_BSCENE)
	{
		int tickType = LOGICTICK_MAX;
		for (int i = 0; i < LOGICTICK_MAX; ++i)
		{
			if (now.MSec() >= m_TickTime[i] + LOGICTICK_INTERVAL[i])
			{
				tickType = i;
				m_TickTime[i] = now.MSec();
				break;
			}
		}

		if (tickType == LOGICTICK_SECOND)
		{
			m_LostDungeonTaskMgr.OnHeartbeat(now);
		}
	}

	if (SERVER_TYPE == ST_SCENE)
	{
		//tick类型
		int tickType = LOGICTICK_MAX;
		for(int i = 0; i < LOGICTICK_MAX; ++i)
		{
			if(now.MSec() >= m_TickTime[i] + LOGICTICK_INTERVAL[i])
			{
				tickType = i;
				m_TickTime[i] = now.MSec();
				break;
			}
		}

		if(tickType == LOGICTICK_SECOND)
		{
			if (m_ConditionActivateDBLoadState == (UInt8)CADBLS_DONE && GetLevel() > PLAYER_LEVEL_ON_BEGIN_CHECK_ACTIVATE_COND && GetLevel() != m_OldLevel)
			{
				if (m_OldLevel == PLAYER_LEVEL_ON_BEGIN_CHECK_ACTIVATE_COND)
				{
					ActivateCondCheckEvent::Instance()->OnActivate(this, CAT_MALL_PERSONAL_TAILOR, CAST_ALL);
				}
				else
				{
					ActivateCondCheckEvent::Instance()->OnActivate(this, CAT_MALL_PERSONAL_TAILOR, CAST_PLAYER_LEVEL);
				}
				m_OldLevel = GetLevel();
			}

			m_DayOnlineTime = m_DayOnlineTime + 1;
			m_totOnlineTime = m_totOnlineTime + 1;

			m_PetMgr.OnTick(now);
		}

		//任务定时
		m_TaskMgr.OnHeartbeat(now, tickType);

		m_RedPacketTaskMgr.OnHeartbeat(now, tickType);

		m_ItemMgr.OnHeartbeat(now, tickType);
		m_AbnormalTransactionMgr.OnHeartbeat(now, tickType);

		if (LOGICTICK_FIVESEC == tickType)
		{
			//头像框
			GetHeadFrameMgr().OnHeartbeat(now);
		}
		
		//疲劳值
		//m_Fatigue.OnHeartbeat(now, tickType);

		//判断vip超时
		if(m_VipLvl != 0 && now.Sec() > m_VipDueTime)
		{
			SetVipLvl(0);
		}

		// 快速购买
		if (m_quickBuyTrans && m_quickBuyTrans->IsTimeout(now))
		{
			m_quickBuyTrans->OnFinish(this, ErrorCode::QUICK_BUY_TIMEOUT);
			delete m_quickBuyTrans;
			m_quickBuyTrans = NULL;
		}

		if (m_DailyTimer.IsTimeout(now))
		{
			RefreshDeathTowerCount();
		}

		// 判断逻辑跨天
		if (CheckLogicDayChange(now, false))
		{
			OnLogicDayChanged();
		}

		// 判断逻辑跨周
		if (_CheckLogicWeekChange(now))
		{
			OnLogicWeekChanged();
		}

		//这个放最后吧
		if(tickType == LOGICTICK_SECOND)
		{
			//同步到record服务器
			SyncToRecord();
		}

		if (tickType == LOGICTICK_MIN)
		{
			SetCounter(DAY_ONLINE_TIME, m_DayOnlineTime);
		}
	}

	// 检查需不需要更新位置
	//if (now.MSec() >= m_LastUpdatePosTime + SSApplication::Instance()->GetMovePacketIntervalMSec())
	//if (tickType == LOGICTICK_SECOND)

	// 计算周围的人，先不加时间间隔
	if (now.MSec() >= m_LastUpdateWatchListTime + SSApplication::Instance()->GeWatchListUpdateIntervalMSec())
	{
		m_LastUpdateWatchListTime = now.MSec();
		if (GetScene() && GetScene()->CanSeeOthers() && SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_NEW_SCENE_SYNC))
		{
			CountNewWatchList2(true);
		}
	}

	if (now.MSec() >= m_LastUpdatePosTime + SSApplication::Instance()->GetMovePacketIntervalMSec())
	{
		m_LastUpdatePosTime = now.MSec();

		if (m_ScenePos.IsDirty(SST_MOVE) || m_SceneDir.IsDirty(SST_MOVE))
		{
			// 同步给周围的人

			CLProtocol::SceneSyncPlayerMove syncMove;
			syncMove.id = GetID();
			syncMove.pos = GetScenePos();
			syncMove.dir = GetSceneDir();
			BroadcastExceptMe(syncMove);

			m_ScenePos.ClearDirty(SST_MOVE);
			m_SceneDir.ClearDirty(SST_MOVE);

			if (SSApplication::Instance()->IsDebugAccount(GetAccID()))
			{
				DebugLogStream << "[test_move] player(" << GetID() << ") sync pos(" << syncMove.pos.GetX() << ", " << syncMove.pos.GetY()
					<< ") dir(" << syncMove.dir.GetX() << ", " << syncMove.dir.GetY() << ")" << LogStream::eos;
			}
		}
	}
}

void Player::OnRegistGameEvent()
{
	_RegistOpActEvent();
}

void Player::OnUnRegistGameEvent()
{
	CLEventMgr::Instance()->RemovePlayerEventsHandler(GetID());
	//_UnRegistOpActEvent();
}

void Player::OnOnline()
{
	m_OldLevel = PLAYER_LEVEL_ON_BEGIN_CHECK_ACTIVATE_COND;

	OnRegistGameEvent();

	CountMoveSpeed();

	InitJarMgr();

	m_OnlineTime = CURRENT_TIME.MSec();
	SetBusyTime();

	for(int i = 0; i < LOGICTICK_MAX; ++i)
	{
		m_TickTime[i] = CURRENT_TIME.MSec();
	}

	Avalon::Time offlineTime(GetSavebackTime());
	UInt64 offlineDay = GetDayBeginTime2(offlineTime);
	UInt64 currentDay = GetDayBeginTime2(CURRENT_TIME);

	bool bDayChanged = CheckLogicDayChange(CURRENT_TIME, true);

	//自然跨天
	UInt32 od = GetOfflineDay();
	if (od > 0)
	{
		m_DayChargeNum = 0;
	}

	m_DayOnlineTime = GetCounter(DAY_ONLINE_TIME);
	
	DebugLogStream << PLAYERINFO(this) << "offlineDay = " << offlineDay << ", currentDay = " << currentDay << ", bDayChanged = " << bDayChanged << LogStream::eos;
	if(m_ContinuousDays != 0 && bDayChanged)
	{
		if(currentDay == offlineDay + DAY_TO_SEC * 1000) m_ContinuousDays = m_ContinuousDays + 1;
		else m_ContinuousDays = 0;
	}

	CheckNewBoot();

	UpdateMoneyManage();

	UpdateFuncNotify();

	InitPackage();

	SyncBaseInfoToGate();

	// 计算外观
	CountAvatar();

	//发送初始数据
	SyncSelfData(false);

	//发送道具数据
	m_ItemMgr.OnOnline(bDayChanged);
	m_JarMgr.OnOnline();
    m_dungeonMgr.OnOnline();

    m_pkStatisticMgr.OnOnline();

	//发送任务数据
	m_TaskMgr.OnOnline(bDayChanged);
	m_CycleTaskMgr.OnOnline(bDayChanged);
	m_DailyTaskMgr.OnOnline(bDayChanged);
	m_AchievementTaskMgr.OnOnline(bDayChanged);
	m_RedPacketTaskMgr.OnOnline(bDayChanged);
	m_LegendTaskMgr.OnOnline(bDayChanged);

	m_MasterDailyTaskMgr.OnOnline(bDayChanged);
	m_MasterAcademicGrowthTaskMgr.OnOnline(bDayChanged);
	OnLoadTaskFinished();

	m_Fatigue.OnOnline(bDayChanged);

	//随从
	m_RetinueMgr.OnOnline();
	//宠物
	m_PetMgr.OnOnline(bDayChanged);
	 
	m_ShopMgr.OnOnline(bDayChanged);
	m_AbnormalTransactionMgr.OnOnline();

	// check vip max level
	m_Vip.AddExp(GetReason(LogSourceType::SOURCE_TYPE_PLAYER_ONLINE).c_str(), 0);

	//活动上线放最后,活动和其他系统有依赖
	m_CounterMgr.OnOnline(bDayChanged);

	m_SeasonLevel.OnOnline();

	GetHeadFrameMgr().Online();

	SSServiceSwitchMgr::Instance()->OnPlayerOnline(this);

	OnMonthCardWelfare(GetMonthCardRemainDays(), false, bDayChanged, true);

	// 计算外观
	//CountAvatar();

	if (GetMatchScore() > GetBestMatchScore())
	{
		SetBestMatchScore(GetMatchScore());
	}

	if (CheckRefreshDeathTowerCount())
	{
		RefreshDeathTowerCount();
	}

	//技能
	GetSkillMgr().OnOnline();
	GetPvpSkillMgr().OnOnline();
	if (!IsCheckSP())
	{
		FinishCheckSP();
	}

	CheckWudaodahuiStatus(bDayChanged);

	//刷等级排行榜
	SortListType sortListType = GetLevelSortListTypeByOccu(GetOccu());
	if (sortListType != SORTLIST_INVALID)
	{
		SortListMgr::Instance()->OnSortValueChanged(sortListType, GetID(), GetName(), 0, GetOccu(), 0, GetLevel(), (UInt32)GetLevel(), (UInt32)GetExp(), 0);
	}
	//刷勇者之塔排行榜
	UInt32 floor = GetDeathTowerTopFloor();
	UInt32 usedTime = GetDeathUsedTime();
	SortListType towerSortListType = GetTowerSortListTypeByOccu(GetOccu());
	if (towerSortListType != SORTLIST_INVALID)
	{
		SortListMgr::Instance()->OnSortValueChanged(towerSortListType, GetID(), GetName(), 0, GetOccu(), 0, GetLevel(), floor, usedTime, 0);
	}
	SortListMgr::Instance()->OnSortValueChanged(SORTLIST_TOWER_1, GetID(), GetName(), 0, GetOccu(), 0, GetLevel(), floor, usedTime, 0);

	// 手机绑定活动
	if (IsPhoneBind())
	{
		DebugLogStream << "player" << GetID() << ") send phone bind event." << LogStream::eos;
		CLEventMgr::Instance()->SendPlayerEvent(GetID(), CLEventMgr::ET_PHONE_BIND, NULL);
	}

	if (ActivityMgr::Instance()->IsInOpActivity(OAID_LEVEL_FIGHTING_FOR_NEW_SERVER))
	{
		SortListMgr::Instance()->OnSortValueChanged(SortListType::SORTLIST_NEW_SERVER_LEVEL, GetID(), GetName(), 0, GetOccu(), 0, GetLevel(), (UInt32)GetLevel(), (UInt32)GetExp(), 0);
	}

	// 通知所有的城镇npc
	{
		CLProtocol::SceneNpcList notify;
		notify.infoes = SceneMgr::Instance()->GetSceneNpcInfo(this);
		SendProtocol(notify);
	}

	// 不是预约角色了，有两种可能，之前
	CheckAppointmentOccu();

	// 计算角色评分
	CalcRoleValueScore(true);
	
	SaveOnlineData();

// 	if (bDayChanged)
// 	{
// 		OnLogicDayChanged();
// 	}

	// 重新登录后代表放弃了之前的重连，转发到world
	CLProtocol::SceneServerGiveupReconn giveupReconn;
	giveupReconn.roleId = GetID();
	Router::SendToMatch(giveupReconn);

	SendLoginUdpLog();

	//调用仓库扩展事件
	OnEnlargeStorage(GetEnlargeStoragePackSize());

	SendScoreWarSortListInfo();

	// 同步游戏设置到world
	SyncGameSetWorld();
	
    /*
	//feed
	m_FeedMgr.OnOnline(bDayChanged);
	

	m_CopySceneTimes.OnOnline(bDayChanged);
	//发送宠物数据
	m_PetMgr.OnOnline();
	m_PetMgr.OnOnline();
	
	m_RoutineMgr.OnOnline(bDayChanged);

	
	m_GiftbagMgr.OnOnline();

	m_DragonTower.OnOnline(bDayChanged);



	m_DujieMgr.OnOnline(bDayChanged);


	//发送自动挂机配置
	SyncAutoHookInfo();

	SyncTribeExpInfo();

	SendQQVipInfo();

	if(bDayChanged)
		// 发送道具库存到数据中心
		SendItemUdpLog();
		
		m_SignInMgr.OnFirstLogin(offlineTime);
		m_ExpRetrieveMgr.OnFirstLogin();
	}
	m_ActGiftbagMgr.OnOnline(offlineTime);
	m_TWSKMgr.OnOnline(bDayChanged);

	m_DarkTowerMgr.OnOnline(bDayChanged);
	m_InviteMgr.OnOnline();
	m_MountMgr.OnOnLine();
	m_ItemLockMgr.OnOnLine();
	m_ExpRetrieveMgr.OnOnLine();
	m_WingMgr.OnOnline();
	m_EquipShopMgr.OnOnline();
	m_FashionMgr.OnOnLine();
	m_GrowthSceneMgr.OnOnline();

	GiveVipLevelupAward();


	if(strchr(GetName().c_str(), NAME_ZONE_SPLITTER) != NULL)
	{
	{
		SendNotify(209);
	}

	SyncBlessTimes();

	// 登录消息发送到数据中心
	SendUdpLog(m_Source1.c_str(), m_Source2.c_str(), "", "", "", "", "login");

	if(m_Gold > 50000)
	{
		GoldAlarm("online", "", 0);
	}*/
	// 在这个过程中可能会有数据修改，最后再同步一次玩家数据
	SyncSelfData(true);

	// 通知录像服务器
	CLProtocol::ReplayReportPlayerOnline notify;
	notify.roleId = GetID();
	Router::SendToReplay(notify);

	// 上线后通知跨服场景那边退出跨服场景
	{
		CLProtocol::CrossExitSceneRes res;
		res.roleId = GetID();
		res.result = ErrorCode::SUCCESS;
		Router::SendToCross(res);
	}
}

void Player::OnOnlineBattle()
{
	DebugLogStream << "player(" << PLAYERINFO(this) << ") OnOnlineBattle." << LogStream::eos;

	UInt64 needExp = GetLevelExp((UInt8)59);
	IncExp(GetReason(SOURCE_TYPE_GM).c_str(), needExp);

	SetPveSP(10000);
	SetPvpSP(10000);
	SetChijiHp(HP_MP_FULL_VALUE);
	SetChijiMp(HP_MP_FULL_VALUE);

	CountMoveSpeed();

	m_OnlineTime = CURRENT_TIME.MSec();
	SetBusyTime();

	for (int i = 0; i < LOGICTICK_MAX; ++i)
	{
		m_TickTime[i] = CURRENT_TIME.MSec();
	}

	UpdateFuncNotify();

	InitChijiPackage();

	SyncBaseInfoToGate();

	//发送初始数据
	SyncSelfData(false);

	//发送道具数据
	m_ItemMgr.Clear();
	m_ItemMgr.OnOnline(false);

	//宠物
	m_PetMgr.Clear();

	m_CounterMgr.OnOnline(false);

	GetBuffMgr().ClearBuff();

	//技能
	//GetSkillMgr().OnOnline();
	//GetPvpSkillMgr().OnOnline();

	// 通知所有的城镇npc
	{
		CLProtocol::SceneNpcList notify;
		notify.infoes = SceneMgr::Instance()->GetSceneNpcInfo(this);
		SendProtocol(notify);
	}

	// 吃鸡初始化

	// 在这个过程中可能会有数据修改，最后再同步一次玩家数据
	SyncSelfData(true);
}

void Player::OnOffline(bool bShutdown)
{
	DebugLogStream << PLAYERINFO(this) << " offline bShutdown:" << bShutdown << LogStream::eos;

	//OnUnRegistGameEvent();

	CancelTrade();

	if(bShutdown)
	{
		BaseTask* task = m_TaskMgr.FindTask(TRANS_WOOD_TASKID);
		if(task != NULL && task->GetStatus() != TASK_FAILED)
		{
			task->SetStatus(TASK_FINISHED);
			SubmitTask(TRANS_WOOD_TASKID);
		}
	}
	
	m_RoutineMgr.OnOffline();
	m_ItemMgr.OnOffline();
	m_JarMgr.OnOffline();
	m_PetMgr.OnOffline();

	m_pTeam = NULL;

	m_pVipBuff = NULL;

	Creature::OnOffline();

	// 下线了，把属于这个玩家的所有城镇怪物都删掉
	SceneMgr::Instance()->RemovePersionalCityMonsters(GetID(), false);

	GameInfoLogStream("Player") << "player(" << GetAccID() << "," << GetID() << ","
		<< GetName() << ") sceneserver offline!" << LogStream::eos;
	
	// 下线消息发送到数据中心
	//std::ostringstream	os;
	//os << (CURRENT_TIME.MSec() - m_OnlineTime) / 1000;
	//SendUdpLog(m_Source1.c_str(), m_Source2.c_str(), "", "", "", os.str().c_str(), "login");
	SendLogoutUdpLog();

    // 离开地下城
	NotifyLeaveDungeon(true);

	// 同步离线数据到world
	SyncShareDataToWorld();

	//放最后
	SetGameStatus(PS_FINAL);
}

void Player::ScriptOnOnline(UInt64 offlineTime)
{
	ScriptMgr::Instance()->GetPlayerScript()->OnOnline(this, offlineTime);
}

void Player::OnBirth()
{
	if (SERVER_TYPE == ST_BSCENE)
	{
		return;
	}

	ScriptMgr::Instance()->GetPlayerScript()->OnBirth(this);

	if(m_Inviter != 0) //通知social被邀请者进入游戏
	{
		CLProtocol::SocialNotifyInviteeEnterGame notify;
		notify.inviter = m_Inviter;
		notify.invitee = GetAccID();
		notify.openid = m_OpenId;
		Router::SendToSocial(notify);
	}
	
	Avalon::Time offlineTime(GetSavebackTime());

	SortListMgr::Instance()->OnSortValueChanged(SortListType::SORTLIST_1V1_SEASON, GetID(), GetName(), 0, GetOccu(), 0, GetLevel(),GetSeasonLevel(),GetSeasonStar(), 0);
	SortListMgr::Instance()->OnSortValueChanged(SortListType::SORTLIST_LEVEL, GetID(), GetName(), 0, GetOccu(), 0, GetLevel(), (UInt32)GetLevel(), (UInt32)GetExp(),0);

	m_Fatigue.OnBirth();

	//学习初始技能 huchenhui
	GetSkillMgr().OnBirth();
	GetPvpSkillMgr().OnBirth();
	SyncSelfData(true);
}

/**
 *@brief 同步周围对象
 */
class SyncObjectVisitor: public SceneObjectVisitor
{
public:
	SyncObjectVisitor(Player* player):m_pPlayer(player)
	{
		m_pPacket = Avalon::Packet::Create();
		m_pPacket->SetID(CLProtocol::SCENE_SYNC_SCENEOBJECT);
	}
	~SyncObjectVisitor()
	{
		Avalon::Packet::Destroy(m_pPacket);
	}

	bool operator()(SceneObject* obj)
	{
		if( obj == m_pPlayer) return true;
        if (obj->GetID() == m_pPlayer->GetID())
        {
            return true;
        }

		if(obj->IsHidden() && !obj->IsCanSeeMe(m_pPlayer)) return true;

		auto player = (Player*)obj;
		if (!m_pPlayer->IsWatchThisPlayer(player->GetID())) return true;

		Avalon::NetOutputStream stream(m_pPacket->GetBuffer()  + m_pPacket->GetSize(), m_pPacket->GetMaxSize() - m_pPacket->GetSize());
		bool bflush = false;
		try
		{
			if(m_pPacket->GetSize() == 0) stream & UInt8(0) & obj->GetScene()->GetMapID();
			stream & obj->GetID() & UInt8(obj->GetType());
			if(m_pPlayer->GetTeam() != NULL && obj->GetType() == SOT_PLAYER
				&& m_pPlayer->GetTeam()->IsMember(obj->GetID()))
			{
				//是队友附带队伍信息
				obj->Encode(stream, SST_TEAM | SST_AROUND | SST_MOVE | SST_SCENE_ENTER, false);
			}
			else
			{
				obj->Encode(stream, SST_AROUND | SST_MOVE | SST_SCENE_ENTER, false);
			}
			if(m_pPacket->GetSize() + UInt16(stream.GetPos()) > m_pPacket->GetMaxSize() - 5 * 1024)
			{
				stream & ObjID_t(0);
				bflush = true;
			}
			m_pPacket->SetSize(m_pPacket->GetSize() + UInt16(stream.GetPos()));
		}
		catch(const Avalon::NetException&)
		{
			return false;
		}
		if(bflush)
		{
			m_pPlayer->SendPacket(m_pPacket);
			m_pPacket->SetSize(0);
		}
		return true;
	}

	void Flush(bool isForceSendMsg = false)
	{
		if(m_pPacket->GetSize() > 0)
		{
			Avalon::NetOutputStream stream(m_pPacket->GetBuffer() + m_pPacket->GetSize(), m_pPacket->GetMaxSize() - m_pPacket->GetSize());
			stream & ObjID_t(0);
			m_pPacket->SetSize(m_pPacket->GetSize() + UInt16(stream.GetPos()));
			m_pPlayer->SendPacket(m_pPacket);
			m_pPacket->SetSize(0);
		}
        else if (isForceSendMsg)
        {
            m_pPlayer->SendPacket(m_pPacket);
        }
	}

private:
	//消息包
	Avalon::Packet* m_pPacket;
	//玩家
	Player*		m_pPlayer;
};

/**
 *@brief 删除周围玩家
 */
class DelObjectVisitor : public SceneObjectVisitor
{
public:
	DelObjectVisitor(Player* player, Avalon::Packet* delPacket) :m_pPlayer(player), m_delPacket(delPacket)
	{
		m_pPacket = Avalon::Packet::Create();
		m_pPacket->SetID(CLProtocol::SCENE_DELETE_SCENEOBJECT);
	}
	~DelObjectVisitor()
	{
		Avalon::Packet::Destroy(m_pPacket);
	}

	bool operator()(SceneObject* obj)
	{
		if( obj == m_pPlayer) return true;
		if(obj->IsHidden() && !obj->IsCanSeeMe(m_pPlayer)) return true;

		auto player = (Player*)obj;
		if (!player->IsWatchThisPlayer(m_pPlayer->GetID())) return true;

		player->SendPacket(m_delPacket);

		Avalon::NetOutputStream stream(m_pPacket->GetBuffer() + m_pPacket->GetSize(), m_pPacket->GetMaxSize() - m_pPacket->GetSize());
		bool bflush = false;
		try
		{
			stream & obj->GetID();
			if(m_pPacket->GetSize() + UInt16(stream.GetPos()) > m_pPacket->GetMaxSize() - 5 * 1024)
			{
				stream & ObjID_t(0);
				bflush = true;
			}
			m_pPacket->SetSize(m_pPacket->GetSize() + UInt16(stream.GetPos()));
		}
		catch(const Avalon::NetException&)
		{
			return false;
		}
		if(bflush)
		{
			m_pPlayer->SendPacket(m_pPacket);
			m_pPacket->SetSize(0);
		}
		return true;
	}

	void Flush()
	{
		if(m_pPacket->GetSize() > 0)
		{
			Avalon::NetOutputStream stream(m_pPacket->GetBuffer() + m_pPacket->GetSize(), m_pPacket->GetMaxSize() - m_pPacket->GetSize());
			stream & ObjID_t(0);
			m_pPacket->SetSize(m_pPacket->GetSize() + UInt16(stream.GetPos()));
			m_pPlayer->SendPacket(m_pPacket);
			m_pPacket->SetSize(0);
		}
	}

private:
	//玩家
	Player*	m_pPlayer;
	// 删除玩家包
	Avalon::Packet*	m_delPacket;
	//消息包
	Avalon::Packet* m_pPacket;
};

void Player::OnBeforeEnterScene()
{
	if (GetDoorID() == 0)
	{
		ScenePos scenePos = ScenePos::GetScenePosByGridPos(GetPos(), GetScene()->GetGridWidth(), GetScene()->GetGridHeight());
		SetScenePos(scenePos.GetX(), scenePos.GetY());
	}
	else
	{
		ScenePos scenePos = GetScene()->GetDoorBirthScenePos(GetDoorID());
		SetScenePos(scenePos.GetX(), scenePos.GetY());
	}

	SetDoorID(0);

    SetStatus(CREATURE_STATUS_NORMAL);

	Creature::OnBeforeEnterScene();

	GetScene()->OnBeforePlayerEnter(this);

	if (m_lastScene && m_lastScene->GetType() != SCENE_TYPE_PK && GetScene() && GetScene()->GetDataEntry() && (GetScene()->GetDataEntry()->subType == SCENE_SUB_WUDAO || GetScene()->GetDataEntry()->subType == SCENE_SUB_ROOM))
	{
		m_WudaoPrevSceneId = m_EnterScene;
		m_WudaoPrevScenePos = m_EnterPos;
	}
}

void Player::NotifyGateEnterScene()
{
	CLProtocol::GateNotifyEnterScene req;
	req.id = GetID();
	req.sceneid = GetScene()->GetID();
	req.gridindex = GetGridIndex();
	SendToGate(req);
}

void Player::NotifyLoadScene()
{
	CLProtocol::SceneNotifyEnterScene notify;
	notify.mapid = GetScene()->GetMapID();
	notify.name = GetScene()->GetName();
	notify.pos = GetScenePos();
	notify.dir = GetSceneDir();
	SendProtocol(notify);

	InfoLogStream << PLAYERINFO(this) << " notify enter scene(" << notify.mapid << ")." << LogStream::eos;
}

void Player::EventProcess(SyncEventType type, ObjID_t param1, ObjID_t param2)
{
	switch (type)
	{
	case SET_INVALID:
		break;
	case SET_FRIEND_GIVE:
		OnFriendGive();
		break;
	case SET_REVIVE:
		OnReviveCoin(param1 == GetID());
		break;
	case SET_ADD_AUCTION:
		OnAddAuction();
		break;
	case SET_SELL_AUCTION:
		OnSellAuction((UInt32)param1);
		break;
	case SET_BUY_AUCTION:
		OnBuyAuction((UInt32)param1);
		break;
	case SET_ADD_REDPOINT:
		AddRedPoint((RedPointFlag)param1);
		break;
	case SET_CLR_REDPOINT:
		ClearRedPoint((RedPointFlag)param1);
		break;
	case SET_OPEN_FREE_GOLD_CHEST:
		IncCounter(COUNTER_VIP_FREE_GOLD_CHEST_TIMES, 1);
		break;
	case SET_AUCTION_REFRESH_TIME:
		SetAuctionRefreshTime((UInt32)param1);
		break;
	case SET_NEW_YEAR_RED_PACKET:
		OnNewYearRedPacket((UInt32)param1);
		break;
	case SET_PREMIUM_LEAGUE_ENROLL:
		OnPremiumLeagueEnroll();
		break;
	case SET_JOIN_GUILD:
		OnJoinGuild();
		break;
	case SET_GUILD_TERR_CHEST_DOUBLE:
		// 计算刷新，领地占领之后要刷新一次
		//GetDungeonMgr().RefreshGuildTerrDungeonTimes();
		IncCounter(COUNTER_GUILD_TERR_DUNGEON_TIMES, 1);
		break;
	case SET_GUILD_BATTLE_SCORE:
		OnIncGuildBattleScore((UInt32)param1);
		break;
	case SET_GUILD_OCCUPY_TERRITORY:
		OnGuildOccupyTerritory((UInt8)param1);
		break;
	case SET_RELATION_RELIEVE_MASTER:
		OnRelationRelieveMaster(param1);
		break;
	case SET_RELATION_DISCIPLE_FINISHSCHOOL:
		OnRelationDiscipleFinishSchool(param1);
		break;
	case SET_ADD_GOODTEACH_VALUE:
		OnAddGoodTeacherValueByFinishSchool(param1, param2);
		break;
	case SET_OPPO_VIP_LEVEL:
		SetOppoVipLevel((UInt8)param1);
		InfoLogStream << "player(" << PLAYERINFO(this) << ") oppo vip level(" << GetOppoVipLevel() << ")." << LogStream::eos;
		break;
	case SET_LOST_DUNG_HP_MP:
		SetLostDungeonHp((Int32)param1);
		SetLostDungeonMp((Int32)param2);
		break;
	case SET_LOST_DUNG_BATTLE_ST:
		SetLostDungBatSt((UInt8)param1);
		break;
	case SET_LOST_DUNG_PROT_TIME:
		SetLostDungProtTime((UInt32)param1);
		break;
	default:
		break;
	}
}

void Player::SyncAroundToMe(bool isForceSendMsg)
{
	SyncObjectVisitor syncVisitor(this);
	GetScene()->VisitObjectsAround(GetPos(),syncVisitor);
	syncVisitor.Flush(isForceSendMsg);
}

void Player::SyncAvatarToTeam()
{
	CLProtocol::SceneSyncTeamMemberAvatar sync;
	sync.id = GetID();
	sync.avatar = m_avatar;
	Router::SendToWorld(sync);
}

void Player::SyncAvatarToWorld()
{
	CLProtocol::SceneSyncPlayerAvatar sync;
	sync.id = GetID();
	sync.avatar = m_avatar;
	Router::SendToWorld(sync);
}

void Player::OnEnterScene()
{
	if (IsNotifyChangeScene())
	{
		// 将周围玩家加入到自己的观察列表中，并将自己加入到周围人的观察列表中
		AddAllAroundPlayers();

		//通知网关进入场景
		NotifyGateEnterScene();

		// 通知world进入场景
		CLProtocol::SceneNotifyWorldEnterScene notify;
		notify.roleId = GetID();
		notify.sceneId = GetScene()->GetID();
		Router::SendToWorld(notify);

		if (SERVER_TYPE == ST_WSCENE)
		{
			Router::SendToCross(notify);
		}

		if (SERVER_TYPE == ST_BSCENE)
		{
			Router::SendToBattle(notify);
		}

		// 通知客户端加载场景了
		NotifyLoadScene();
	}
	else
	{
		SetNotifyChangeScene(true);
	}
	
    GetScene()->OnPlayerEnter(this);

	// 计算新的列表
	if (SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_NEW_SCENE_SYNC))
	{
		CountNewWatchList2(false);
	}

    //同步场景对象
    SyncAroundToMe(true);

    Creature::OnEnterScene();

	//其他处理放这里
    SetBusyTime();

	auto sceneType = GetScene() ? GetScene()->GetType() : SCENE_TYPE_INVALID;

	m_PetMgr.OnEnterScene(sceneType);

	if (sceneType == SCENE_TYPE_NORMAL)
	{
		CLSaveCommand* cmd  = CLRecordClient::Instance()->PrepareUpdateCommand("t_player_info", GetID(), true);
		cmd->PutData("sceneid", GetSceneID());
		CLRecordClient::Instance()->SendCommand(cmd);

		SetMajorCityID(GetScene()->GetDataEntry()->birthCity);
	}
	else if (sceneType == SCENE_TYPE_SINGLE || sceneType == SCENE_TYPE_NORMAL)
	{
		SetMajorCityID(GetSceneID());
	}
}

void Player::OnLeaveScene()
{
	m_lastScene = GetScene();
	GetScene()->OnPlayerLeave(this);

	if (GetScene()->GetType() == SCENE_TYPE_NORMAL || GetScene()->GetType() == SCENE_TYPE_SINGLE 
		|| GetScene()->GetType() == SCENE_TYPE_PK_PREPARE || GetScene()->GetType() == SCENE_TYPE_BATTLE 
		|| GetScene()->GetType() == SCENE_TYPE_CHIJI_PREPARE || GetScene()->GetType() == SCENE_TYPE_LOST_DUNGEON)
	{
		if (GetScene()->GetType() == SCENE_TYPE_LOST_DUNGEON)
		{
			m_EnterSceneDyncId = GetScene()->GetID();
			m_EnterScene = GetScene()->GetMapID();
		}
		else
		{
			m_EnterSceneDyncId = 0;
			m_EnterScene = GetScene()->GetID();
		}
		
		m_EnterPos = GetPos();
	}

	//其他处理放这里

    if (GetScene() && GetScene()->GetType() == SCENE_TYPE_DUNGEON_ENTRY_DYNAMIC)
    {
		NotifyLeaveDungeon(false);
    }
	
	Creature::OnLeaveScene();

	DelAllAroundPlayers();

	//通知网关离开场景
	CLProtocol::GateNotifyLeaveScene req;
	req.sceneId = GetScene()->GetID();
	req.id = GetID();
	SendToGate(req);

	SyncToRecord(true);
	SyncSelfData(true);
}

void Player::OnRefreshFatigue()
{
	GetDailyTaskMgr().RefreshDailyTask();
	GetCycleTaskMgr().RefreshCycleTaskDailyCount();
}

void Player::OnGridChanged()
{
	//向网关同步格子
	CLProtocol::GateSyncGridIndex sync;
	sync.id = GetID();
	sync.gridIndex = GetGridIndex();
	SendToGate(sync);

    if (!GetScene()->CanSeeOthers())
    {
        return;
    }

	//删除原区域对象
	Avalon::Packet* delpacket = MakeDelObjectPacket();
	DelObjectVisitor delVisitor(this, delpacket);
	GetScene()->VisitObjectsOldZone(this, delVisitor);
	delVisitor.Flush();
	Avalon::Packet::Destroy(delpacket);

	class OldZonePlayerVisitor : public CLVisitor<Player>
	{
	public:
		OldZonePlayerVisitor(Player* player) :m_player(player)
		{
		}

		bool operator()(Player* player)
		{
			if (!player || player == m_player)
			{
				return true;
			}

			player->DelAroundPlayer(m_player, true);
			m_player->DelAroundPlayer(player, true);
			return true;
		}

		Player* m_player;
	};
	OldZonePlayerVisitor oldZoneVistor(this);
	GetScene()->VisitPlayersOldZone(this, oldZoneVistor);

	class NewZonePlayerVisitor : public CLVisitor<Player>
	{
	public:
		NewZonePlayerVisitor(Player* player, Avalon::Packet* syncPacket) :m_player(player), m_syncPacket(syncPacket)
		{
		}

		bool operator()(Player* player)
		{
			if (!player || player == m_player)
			{
				return true;
			}

			player->AddAroundPlayer(m_player, true);
			m_player->AddAroundPlayer(player, true);

			if (player->IsWatchThisPlayer(m_player->GetID()))
			{
				player->SendPacket(m_syncPacket);
			}

			return true;
		}

		Player* m_player;
		Avalon::Packet* m_syncPacket;
	};
	Avalon::Packet* syncpacket = MakeSyncObjectPacket(false);
	NewZonePlayerVisitor newZoneVistor(this, syncpacket);
	GetScene()->VisitPlayersNewZone(this, newZoneVistor);

	//如果新区域有玩家
	
	//BroadcastNewZone(syncpacket);
	Avalon::Packet::Destroy(syncpacket);

	//同步新区域对象
	SyncObjectVisitor syncVisitor(this);
	GetScene()->VisitObjectsNewZone(this, syncVisitor);
	syncVisitor.Flush();

	//Creature::OnGridChanged();
	SyncTeamNewZone();
}

void Player::OnLevelup()
{
	//奖励SP huchenhui
	AddSP(GetReason(SOURCE_TYPE_PLAYER_UP_LEVEL).c_str(), m_pLevelDataEntry->sp);

	//huchenhui 技能槽解锁
	m_SkillBar.OnLevelup();
	//m_ItemBar.OnLevelup();

	m_TaskMgr.OnLevelup();
	m_DailyTaskMgr.OnLevelup();
	m_AchievementTaskMgr.OnLevelup();
	m_CycleTaskMgr.OnLevelup();
	m_LegendTaskMgr.OnLevelup();
	//m_EquipShopMgr.OnLevelUp();
	m_MasterAcademicGrowthTaskMgr.OnLevelup();
	this->OnLevelUpMasterSys();

	//added by huchenhui 2016.09.01
	GetSkillMgr().OnLevelUp();
	GetPvpSkillMgr().OnLevelUp();

	ScriptMgr::Instance()->GetPlayerScript()->OnLevelup(this);

	//CheckMallGiftPackOnLevelUp();

	//计算评分
	CalcRoleValueScore();

	UpdateFuncNotify();

	GameInfoLogStream("Player") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") levelup to " << GetLevel() << "." << LogStream::eos;

	SendUdpLog("level", LOG_LEVEL_FORMAT, GetLevel());
}

void Player::OnUseItem(UInt32 itemid)
{
	m_TaskMgr.OnUseItem(itemid);
	m_DailyTaskMgr.OnUseItem(itemid);
	m_AchievementTaskMgr.OnUseItem(itemid);
	SetBusyTime();
}

void Player::OnGetItem(const char* reason, ItemDataEntry* data, UInt8 qualityLv, UInt8 strenth, UInt16 num, UInt16 oldNum, Item* pItem, bool getNotify, bool mailGive)
{
	if (!data && !pItem)
		return;

	if (!data)
		data = pItem->GetDataEntry();

	GameInfoLogStream("Property") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") additem:" 
		<< reason << "|" << data->id << "|" << num << "|" << Int32(qualityLv) << "|" << Int32(strenth) << LogStream::eos;

	//获得buff
	if (data->onGetBuff != 0)
	{
		GetBuffMgr().AddBuff(data->onGetBuff);
	}

	m_TaskMgr.OnGetItem(data->id, data->color, strenth, num);
	m_DailyTaskMgr.OnGetItem(data->id, data->color, strenth, num);
	m_AchievementTaskMgr.OnGetItem(data->id, data->color, strenth, num);
	m_CycleTaskMgr.OnGetItem(data->id, data->color, strenth, num);
	m_LegendTaskMgr.OnGetItem(data->id, data->color, strenth, num);
	m_LostDungeonTaskMgr.OnCollectItem(data->id, num);

	if (getNotify)
	{
		CLProtocol::SceneNotifyGetItem notify;
		if (data->type == ITEM_EQUIP && num > 1)
		{
			for (UInt16 i = 0; i < num; i++)
			{
				ItemReward item;
				item.id = data->id;
				item.num = 1;
				notify.items.push_back(item);
			}
		}
		else
		{
			ItemReward item;
			item.id = data->id;
			item.num = num;
			notify.items.push_back(item);
		}

		notify.sourceType = 0;
		//notify.mailGive = mailGive;
		//notify.mailGive = 1;
		if (mailGive)
		{
			notify.notifyType = 0;
		}
		ReasonInfo ri;
		if (ParseReason(reason, ri))
		{
			notify.sourceType = ri.type;
		}
		SendProtocol(notify);
	}
}

void Player::OnGetRewardGroup(const char* reason, RewardGroup* group, bool mailGive)
{
	if (!group)
		return;

	const ItemRewardVec items = group->GetRewards();
	int sz = (int)items.size();

	//获得buff
	for (size_t i = 0; i < items.size(); ++i)
	{
		ItemDataEntry* data = ItemDataEntryMgr::Instance()->FindEntry(items[i].id);
		if (data && data->onGetBuff != 0)
		{
			GetBuffMgr().AddBuff(data->onGetBuff);
		}
	}

	if (sz > 0)
	{
// 		for (int i = 0; i < sz; ++i)
// 		{
// 			InfoLogStream << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") OnGetRewardGroup itemId:"
// 				<< items[i].id << "|num:" << items[i].num << LogStream::eos;
// 		}
		
		CLProtocol::SceneNotifyGetItem notify;
		notify.items = items;
		notify.sourceType = 0;
		//notify.mailGive = mailGive;
		//notify.mailGive = 1;
		if (mailGive)
		{
			notify.notifyType = 0;
		}
		ReasonInfo ri;
		if (ParseReason(reason, ri))
		{
			notify.sourceType = ri.type;
		}
		SendProtocol(notify);
	}
	
}

void Player::OnCostItem(const char* reason, ItemDataEntry* data, UInt8 strengthen, UInt16 newNum, UInt16 oldNum, Item* pItem)
{
	if (!data && !pItem)
		return;

	if (!data)
		data = pItem->GetDataEntry();

	UInt8 strengthenLvl = 0;
	std::string stonesStr;
	std::string randAttrStr;
	if(pItem != NULL)
	{
		strengthenLvl = pItem->GetStrengthen();
		std::ostringstream stoneoss;
		//pItem->GetGemStones().EncodeString(stoneoss);
		stonesStr = stoneoss.str();
		std::ostringstream randattross;
		pItem->GetRandAttrs().EncodeString(randattross);
		randAttrStr = randattross.str();
	}

	GameInfoLogStream("Property") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") removeitem:" 
		<< reason << "|" << data->id << "|" << newNum << "|" << Int32(strengthen) << "|" << Int32(strengthenLvl)
		<< "|" << stonesStr << "|" << randAttrStr << LogStream::eos;

	// 通知任务系统
	m_TaskMgr.OnRmItem(data->id, data->color, strengthen, newNum);
	m_DailyTaskMgr.OnRmItem(data->id, data->color, strengthen, newNum);
	m_AchievementTaskMgr.OnRmItem(data->id, data->color, strengthen, newNum);
	m_LegendTaskMgr.OnRmItem(data->id, data->color, strengthen, newNum);

	if(true)
	{
		CLProtocol::SceneNotifyCostItem notify;
		notify.itemId = data->id;
		notify.quality = data->color;
		notify.num = newNum;
		SendProtocol(notify);
	}
}


void Player::OnPushStorage(UInt32 itemId, UInt8 quality, UInt8 strengthen, UInt16 num)
{
	// 通知任务系统
	m_TaskMgr.OnRmItem(itemId, quality, strengthen, num);
	m_DailyTaskMgr.OnRmItem(itemId, quality, strengthen, num);
	m_AchievementTaskMgr.OnRmItem(itemId, quality, strengthen, num);
	m_LegendTaskMgr.OnRmItem(itemId, quality, strengthen, num);
}

void Player::OnPullStorage(UInt32 itemId, UInt8 quality, UInt8 strengthen, UInt16 num)
{
	m_TaskMgr.OnGetItem(itemId, quality, strengthen, num);
	m_DailyTaskMgr.OnGetItem(itemId, quality, strengthen, num);
	m_AchievementTaskMgr.OnGetItem(itemId, quality, strengthen, num);
	m_CycleTaskMgr.OnGetItem(itemId, quality, strengthen, num);
	m_LegendTaskMgr.OnGetItem(itemId, quality, strengthen, num);
}

void Player::OnOwnedNpcDied(Npc* npc,  bool bDeathOwner)
{
	if(npc == NULL) return;

	//modified by aprilliu, 2016.04.13 暂时注释掉
	//m_TaskMgr.OnOwnedNpcDied(npc->GetDataID(), npc->GetLevel());

	if(bDeathOwner)
	{
	
	}
}

//added by aprilliu, 2016.04.13
void Player::OnNpcDead(UInt32 npcId, UInt32 dungeon)
{
	MonsterDataEntry* data = MonsterDataEntryMgr::Instance()->FindEntry(npcId);
	if (NULL == data)
	{
		DebugLogStream << "in  Player::OnNpcDead(UInt32 npcId), player(role:" << GetID() << ", accId:" 
			<< m_AccId << ") can't find npc(" << npcId << ") just return" << LogStream::eos;
		return;
	}

	m_TaskMgr.OnOwnedNpcDied(MonsterDataEntry::GetMonsterTypID(npcId), data->type, dungeon);
	m_DailyTaskMgr.OnOwnedNpcDied(MonsterDataEntry::GetMonsterTypID(npcId), data->type, dungeon);
	m_AchievementTaskMgr.OnOwnedNpcDied(MonsterDataEntry::GetMonsterTypID(npcId), data->type, dungeon);
	m_CycleTaskMgr.OnOwnedNpcDied(MonsterDataEntry::GetMonsterTypID(npcId), data->type, dungeon);

}

void Player::OnMoved(const CLPosition& oldpos)
{
    //UpdateScenePosition();
	Creature::OnMoved(oldpos);

	m_TaskMgr.OnMoved();
	if(GetTeam() != NULL) GetTeam()->OnMoved(this);
	SetBusyTime();

	if(m_pTrade != NULL)
	{
		//距离过远，关闭交易
		TradeInfo* otherInfo = m_pTrade->GetOtherInfo(this);
		if(Distance(otherInfo->player) > GetScene()->GetSyncRange())
		{
			CancelTrade();
		}
	}
}

void Player::OnSceneChanged()
{
	Creature::OnSceneChanged();

	GameInfoLogStream("Player") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") changed to scene " << GetScene()->GetID() << "." << LogStream::eos;

	GetLostDungeonTaskMgr().OnSceneChanged(GetScene()->GetID());
}

void Player::OnDayChanged()
{
	if (SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_CHECK_SAME_DAY_ON_DAY_CHANGE))
	{
		InfoLogStream << PLAYERINFO(this) << " start to check same day..." << LogStream::eos;
		Avalon::Time onlineTime(m_OnlineTime);
		if (IsSameDay(CURRENT_TIME, onlineTime)) return;
	}

	m_DayChargeNum = 0;

	if(m_ContinuousDays != 0)
	{
		m_ContinuousDays = m_ContinuousDays + 1;
	}

	m_RoutineMgr.OnDayChanged();
	m_TaskMgr.OnDayChanged();
	m_ItemMgr.OnDayChanged();

	m_ShopMgr.OnDayChange();
	m_SeasonLevel.OnDayChange();
	m_CounterMgr.OnDayChanged();

	ScriptMgr::Instance()->GetPlayerScript()->OnDayChanged(this);

	SyncBlessTimes();
}

void Player::OnLogicDayChanged()
{
	if (SERVER_TYPE == ST_BSCENE)
	{
		return;
	}

	m_Fatigue.RefreshFatigue(CURRENT_TIME);

	//宠物系统
	m_PetMgr.OnLogicDayChanged();

	//活动放其他系统后面,对其他系统有依赖
	m_LegendTaskMgr.OnLogicDayChanged();

	//计数器放活动后面
	m_CounterMgr.OnLogicDayChanged();

	m_ItemMgr.OnLogicDayChanged();

	//师门日常任务
	m_MasterDailyTaskMgr.OnLogicDayChanged();

	//by huchenhui 2018.11.14 修复在线时间偶尔不刷新的问题
	if (SSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_DAY_ONLINE_TIME))
	{
		m_DayOnlineTime = 0;
	}
}

void Player::OnLogicWeekChanged()
{
	InfoLogStream << PLAYERINFO(this) << " on logic week changed..." << LogStream::eos;
}

void Player::OnBeginDungeon(UInt32 dungeonId, bool isHell)
{
	if (isHell)
	{
		m_TaskMgr.OnBeginHell(dungeonId);
		m_DailyTaskMgr.OnBeginHell(dungeonId);
	}

	m_PetMgr.OnEnterDungeonRace();
}

void Player::OnClearDungeon(UInt32 dungeonId, UInt16 beated, UInt16 areas, LevelScore score, UInt32 usedTime, UInt32 reviveCount, UInt32 endAreaId, std::vector<ObjID_t> teammateIds)
{
	auto dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
	if (!dungeonData)
	{
		return;
	}

	if (score <= LS_C)
	{
		return;
	}

	if (dungeonData->type == LT_NORMAL || dungeonData->type == LT_STORY)
	{
		// 任务...
		m_TaskMgr.OnClearDungeon(dungeonId, beated, areas, score, usedTime, reviveCount);
		m_AchievementTaskMgr.OnClearDungeon(dungeonId, beated, areas, score, usedTime, reviveCount);
		m_DailyTaskMgr.OnClearDungeon(dungeonId, beated, areas, score, usedTime, reviveCount);
		m_CycleTaskMgr.OnClearDungeon(dungeonId, beated, areas, score, usedTime, reviveCount);

		m_MasterDailyTaskMgr.OnClearDungeon(dungeonId, beated, areas, score, usedTime, reviveCount);
		m_MasterAcademicGrowthTaskMgr.OnClearDungeon(dungeonId, beated, areas, score, usedTime, reviveCount);
		
		if (teammateIds.size() > 0)
		{
			m_MasterDailyTaskMgr.OnTeamClearDungeon(dungeonId, beated, areas, score, usedTime, reviveCount, teammateIds);
			m_MasterAcademicGrowthTaskMgr.OnTeamClearDungeon(dungeonId, beated, areas, score, usedTime, reviveCount, teammateIds);
		}

		m_AchievementTaskMgr.OnClearDungeonCareScore(dungeonId, beated, areas, score, usedTime, reviveCount);
	}
	else if (dungeonData->type == LT_ACTIVITY)
	{
		if (dungeonData->IsHellDungeon())
		{
			// 深渊成就
			UInt32 hellEntryDungeonId = DungeonDataEntryMgr::Instance()->GetHellEntry(dungeonData->hellSplitLevel, dungeonData->hardType);
			OnClearHell(hellEntryDungeonId, usedTime, score, teammateIds);
		}
		else
		{
			m_TaskMgr.OnClearActivityDungeon(dungeonId, beated, areas, score, usedTime, reviveCount);
			m_AchievementTaskMgr.OnClearActivityDungeon(dungeonId, beated, areas, score, usedTime, reviveCount);
			m_DailyTaskMgr.OnClearActivityDungeon(dungeonId, beated, areas, score, usedTime, reviveCount);
			m_CycleTaskMgr.OnClearActivityDungeon(dungeonId, beated, areas, score, usedTime, reviveCount);

			m_MasterDailyTaskMgr.OnClearActivityDungeon(dungeonId, beated, areas, score, usedTime, reviveCount);
			m_MasterAcademicGrowthTaskMgr.OnClearActivityDungeon(dungeonId, beated, areas, score, usedTime, reviveCount);

			if (teammateIds.size() > 0)
			{
				m_MasterDailyTaskMgr.OnTeamClearActivityDungeon(dungeonId, beated, areas, score, usedTime, reviveCount, teammateIds);
				m_MasterAcademicGrowthTaskMgr.OnTeamClearActivityDungeon(dungeonId, beated, areas, score, usedTime, reviveCount, teammateIds);
			}
		}

		if (dungeonData->subType == DST_YUANGU)
		{
			UInt32 normalDungeonId = GetDungeonMgr().GetNormalHardDungeonID(dungeonId);
			DungeonDailyDropDataEntry* dailyDropData = DungeonDailyDropDataEntryMgr::Instance()->FindEntry(normalDungeonId);
			if (dailyDropData != NULL)
			{
				std::string counterName = DUNGEON_DAILY_DROP + Avalon::StringUtil::ConvertToString(normalDungeonId);
				if (GetCounter(counterName.c_str()) < dailyDropData->dailyNum)
				{
					IncCounter(counterName.c_str(), 1);
				}
			}
		}
	}
	else if (dungeonData->type == LT_TOWER)
	{
		UInt32 floor = Player::GetTowerFloorByAreaID(dungeonId, endAreaId);
		OnDeathTower(dungeonId, floor, usedTime);
	}

	if (teammateIds.size() > 0)
	{
		for (UInt32 i = 0; i < teammateIds.size(); i++)
		{
			ObjID_t teammateId = teammateIds[i];
			if (this->IsMaster(teammateId))
			{
				if (GetCounter(CLEAR_DUNGEON_ADDGROWTH_VALUE) < SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_CLEARDUNGEN_ADDGROWTH_MAX))
				{
					IncCounter(CLEAR_DUNGEON_ADDGROWTH_VALUE, 1);
					std::string  resaon = GetReason(MASTERSYS_ADDGROWTH_DUNGEON_END, dungeonId, 1);

					//this->AddMasterSysAcademicGrowth(resaon.c_str(), 1);
					this->AddMasterTeamClearDungeonGrowth(resaon.c_str(), 1, dungeonId);
				}
				break;
			}
		}
	}
}

void Player::OnConsumed(UInt32 itemId, UInt16 num, UInt32 cost)
{
	// 累计玩家消费
// 	IncCounter("point_use", cost);
// 	
// 	if(cost >= 100)
// 	{
// 		CLProtocol::SocialInviteNotifyConsume notify;
// 		notify.uid = GetAccID();
// 		notify.gold = cost * 10 / 100; //暂定5%
// 		Router::SendToSocial(notify);
// 	}
// 
// 	PlayerScript* pScript = ScriptMgr::Instance()->GetPlayerScript();
// 	pScript->OnConsumePoint(this, itemId, num, cost);
}

void Player::OnNameChanged()
{

}

void Player::OnSexChanged()
{

}

void Player::OnVipLvlChanged()
{
	SyncAccountInfo(ACCOUNT_VIP_LEVEL, GetVipLvl());
	InfoLogStream << "player(" << GetID() << ", " << GetAccID() << ") notify vip level changed to (" << GetVipLvl() << ")." << LogStream::eos;
}

void Player::OnVipExpChanged()
{
	SyncAccountInfo(ACCOUNT_VIP_EXP, GetVipExp());

	InitPackage();
	SyncSelfData(true);
}

void Player::OnTaskSucced(BaseTask* task)
{
	m_TaskMgr.OnTaskSucced(task);
	m_DailyTaskMgr.OnTaskSucced(task);
	m_AchievementTaskMgr.OnTaskSucced(task);

    // 任务可提交时重新计算开放的地下城列表
    m_dungeonMgr.SyncDungeonList(false);
}

void Player::OnTaskFailed(UInt32 taskId)
{
	m_TaskMgr.OnTaskFailed(taskId);
	m_DailyTaskMgr.OnTaskFailed(taskId);
	m_AchievementTaskMgr.OnTaskFailed(taskId);
}

void Player::OnStrengthenEquip(UInt32 code, UInt8 oldStrengthen, UInt64 itemId, UInt32 itemDataId, UInt32 oldPack, UInt32 useUnbreak, UInt32 isTicket, UInt32 probability)
{
	// 这个item对象可能为空
	Item* item = FindItem(itemId);
	UInt32 newPack = oldPack;

	UInt32 newStrengthen = 0;
	if (item != NULL)
	{
		newStrengthen = item->GetStrengthen();
		newPack = item->GetPos().pack;
	}
	
	SendStrengthEquipUdpLog(itemId, itemDataId, oldStrengthen, newStrengthen, code, newPack, useUnbreak, isTicket, probability);

	if (code == ErrorCode::SUCCESS)
	{
		if (item == NULL) return;
		UInt8 strengthen = item->GetStrengthen();
		m_TaskMgr.OnStrengthenEquip(code, oldStrengthen, strengthen);
		m_DailyTaskMgr.OnStrengthenEquip(code, oldStrengthen, strengthen);
		m_AchievementTaskMgr.OnStrengthenEquip(code, oldStrengthen, strengthen);
		m_RedPacketTaskMgr.OnStrengthenEquip(code, oldStrengthen, strengthen);
		
		if (strengthen == ANNOUNCE_STRENGTHEN_TEN)
		{
			ActivateMallGiftPackReq(MGPAC_STRENGEN_TEN);
		}

		if (strengthen >= (UInt8)SLL_EIGHT)
		{
			ActivateCondCheckEvent::Instance()->OnActivate(this, CAT_MALL_PERSONAL_TAILOR, CAST_ITEM_STRENGEN_LEVEL);
		}

		//公告
		if (strengthen >= ANNOUNCE_STRENGTHEN_LOWER_BEGIN)
		//if (strengthen >= 1)
		{
			std::string linkStr;

			std::string playerLinkStr;
			if (!MsgPackage::GetPlayerMsgTag(playerLinkStr, GetID(), GetName(), GetOccu(), GetLevel()))
			{
				playerLinkStr = GetName();
			}

			if (MsgPackage::GetItemMsgTag(linkStr, item->GetID(), item->GetDataID(), (UInt32)item->GetStrengthen()))
			{
				std::string systemLinkStr;
				MsgPackage::GetSystemSuffixesMsgTag(systemLinkStr, SystemSuffixesType::SYSTEM_SUFFIXES_TYPE_STRENGTHEN, 0);
				if (strengthen <= ANNOUNCE_STRENGTHEN_LOWER_END)
				{
					Sys::SendAnnouncement(3, playerLinkStr.c_str(), linkStr.c_str(), strengthen, systemLinkStr.c_str());
				}
				else if (strengthen <= ANNOUNCE_STRENGTHEN_MIDDLE_END)
				{
					Sys::SendAnnouncement(4, playerLinkStr.c_str(), linkStr.c_str(), strengthen, systemLinkStr.c_str());
				}
				else if (strengthen >= ANNOUNCE_STRENGTHEN_LAST)
				{
					Sys::SendAnnouncement(5, playerLinkStr.c_str(), linkStr.c_str(), strengthen, systemLinkStr.c_str());
				}
			}
		}

		//SetCounter(COUNTER_STRENGTHEN_FAIL_COUNT, 0);
	}
	else
	{
		if (code == ErrorCode::ITEM_STRENTH_FAIL_BROKE)
		{
			//碎裂
			m_TaskMgr.OnStrengthenEquip(code, oldStrengthen, 0);
			m_DailyTaskMgr.OnStrengthenEquip(code, oldStrengthen, 0);
			m_AchievementTaskMgr.OnStrengthenEquip(code, oldStrengthen, 0);
			//IncCounter(COUNTER_STRENGTHEN_FAIL_COUNT, 1);
		}
		else
		{
			if (item == NULL) return;
			m_TaskMgr.OnStrengthenEquip(code, oldStrengthen, item->GetStrengthen());
			m_DailyTaskMgr.OnStrengthenEquip(code, oldStrengthen, item->GetStrengthen());
			m_AchievementTaskMgr.OnStrengthenEquip(code, oldStrengthen, item->GetStrengthen());
			//IncCounter(COUNTER_STRENGTHEN_FAIL_COUNT, 1);
		}
		
		if (oldStrengthen + 1 >= ANNOUNCE_STRENGTHEN_LOWER_BEGIN)
		{
			std::string playerLinkStr;
			if (!MsgPackage::GetPlayerMsgTag(playerLinkStr, GetID(), GetName(), GetOccu(), GetLevel()))
			{
				playerLinkStr = GetName();
			}

			ItemDataEntry *itemDataEntry = ItemDataEntryMgr::Instance()->FindEntry(itemDataId);
			if (itemDataEntry != NULL)
			{
				Sys::SendAnnouncement(33, playerLinkStr.c_str(), itemDataEntry->name.c_str(), (UInt32)(oldStrengthen + 1));
			}
		}
		
	}

	/*
	if (GetCounter(COUNTER_STRENGTHEN_FAIL_COUNT) == 3)
	{
		std::string playerLinkStr;
		if (!MsgPackage::GetPlayerMsgTag(playerLinkStr, GetID(), GetName(), GetOccu(), GetLevel()))
		{
			playerLinkStr = GetName();
		}
		//3连跪
		Sys::SendAnnouncement(6, playerLinkStr.c_str());
	}
	*/

	// 计算外观
	CountAvatar();

	// 计算装备分数
	if (code != ErrorCode::ITEM_STRENTH_FAIL_BROKE && item)
	{
		GetItemMgr().CalculateValuedScore(item);
	}
}

void Player::OnDecomposeEquip()
{
	m_TaskMgr.OnDecomposeEquip();
	m_DailyTaskMgr.OnDecomposeEquip();
	m_AchievementTaskMgr.OnDecomposeEquip();
}

void Player::OnChangeMoney(UInt32 type, Int32 num)
{
	m_TaskMgr.OnChangeMoney(type, num);
	m_DailyTaskMgr.OnChangeMoney(type, num);
	m_AchievementTaskMgr.OnChangeMoney(type, num);
	m_RedPacketTaskMgr.OnChangeMoney(type, num);
	m_LegendTaskMgr.OnChangeMoney(type, num);
}

void Player::OnPutEquip(UInt16 level, UInt32 subType, UInt8 color, UInt32 strengthen)
{
	//m_TaskMgr.OnPutEquip(level, subType, color, strengthen);
	m_DailyTaskMgr.OnPutEquip(level, subType, color, strengthen);
	m_AchievementTaskMgr.OnPutEquip(level, subType, color, strengthen);
}

void Player::OnPutFashion(ItemSubType subType, UInt8 color)
{
	m_AchievementTaskMgr.OnPutFashion(subType, color);
}

void Player::OnAddRetinue(Retinue* retinue)
{
	if (retinue == NULL) return;

	m_TaskMgr.OnAddRetinue(retinue->GetQuality());
	m_DailyTaskMgr.OnAddRetinue(retinue->GetQuality());
	m_AchievementTaskMgr.OnAddRetinue(retinue->GetQuality());
	m_RedPacketTaskMgr.OnAddRetinue(retinue->GetQuality());

	SendRetinueUnlockUdpLog(retinue->GetID(), retinue->GetDataId());
	SendFunctionUdpLog(FUNCTION_RETINUE_UNLOCK, retinue->GetDataId());

	/*
	if (retinue->GetQuality() >= RetinueQuality::RETINUE_PURPLE)
	{
		//公告
		std::string linkStr;
		if (MsgPackage::GetRetinueMsgTag(linkStr, retinue->GetID(), retinue->GetDataId(), retinue->GetName().c_str()))
		{
			std::string playerLinkStr;
			if (!MsgPackage::GetPlayerMsgTag(playerLinkStr, GetID(), GetName(), GetOccu(), GetLevel()))
			{
				playerLinkStr = GetName();
			}

			WorldCacheRetinueData(retinue);
			Sys::SendAnnouncement(1, playerLinkStr.c_str(), linkStr.c_str());
		}
	}
	*/
}

void Player::OnRetinueUpLevel(Retinue* retinue)
{
	if (retinue == NULL) return;

	m_TaskMgr.OnRetinueUpLevel(retinue->GetDataId(), retinue->GetLevel());
	m_DailyTaskMgr.OnRetinueUpLevel(retinue->GetDataId(), retinue->GetLevel());
	m_AchievementTaskMgr.OnRetinueUpLevel(retinue->GetDataId(), retinue->GetLevel());

	SendRetinueUpLevelUdpLog(retinue->GetID(), retinue->GetDataId(), retinue->GetLevel());
	SendFunctionUdpLog(FUNCTION_RETINUE_UPLEVEL, retinue->GetDataId());

	//随从等级为10的倍数 发公告
	/*
	if (retinue->GetLevel() % 10 == 0)
	{
		Sys::SendAnnouncement(2, GetName().c_str(), retinue->GetLevel());
	}
	*/
}

void Player::OnRetinueUpStar(Retinue* retinue)
{
	if (retinue == NULL) return;

	m_TaskMgr.OnRetinueUpStar(retinue->GetDataId(), retinue->GetStarLevel());
	SendFunctionUdpLog(FunctionLogType::FUNCTION_RETINUE_UPSTAR, retinue->GetDataId());

}

void Player::OnRetinueChangeSkill(Retinue* retinue, std::vector<RetinueSkill> lockSkills, std::vector<RetinueSkill> oldSkills, std::vector<RetinueSkill> newSkills)
{
	if (retinue == NULL) return;

	m_TaskMgr.OnRetinueChangeSkill(newSkills);
	m_DailyTaskMgr.OnRetinueChangeSkill(newSkills);
	m_AchievementTaskMgr.OnRetinueChangeSkill(newSkills);

	SendRetinueChangeSkillUdpLog(retinue->GetID(), retinue->GetDataId(), retinue->GetUdpLogString(lockSkills), retinue->GetUdpLogString(oldSkills), retinue->GetUdpLogString(newSkills));
	SendFunctionUdpLog(FunctionLogType::FUNCTION_RETINUE_CHANGE_SKILL, retinue->GetDataId());
}

void Player::OnPVP(const CLProtocol::MatchServerRaceEnd& raceEnd)
{
	CLProtocol::SceneMatchPkRaceEnd ret;
	ret.pkType = raceEnd.type;
	ret.result = raceEnd.result.resultFlag;
	ret.oldPkValue = GetPkValue();
	ret.oldMatchScore = GetMatchScore();
	ret.oldPkCoin = GetPkCoin();
	ret.oldSeasonLevel = GetSeasonLevel();
	ret.oldSeasonStar = GetSeasonStar();
	ret.oldSeasonExp = GetSeasonExp();

	PkType type = (PkType)raceEnd.type;
	if (raceEnd.type == PK_WUDAODAHUI)
	{
		type = PK_SEASON_1V1;
	}

	PkRaceResult result = PKRR_INVALID;
	if (raceEnd.result.resultFlag & resultBitmask_win)
	{
		if (type != PK_PRACTICE)
		{
			IncPkValue(500);
			GetPkStatisticMgr().OnUpdatePkStatistic(type, (Occupation)raceEnd.opponent.occu, PKRR_WIN);
		}
		
		result = PKRR_WIN;
	}
	else if (raceEnd.result.resultFlag & resultBitmask_lose)
	{
		if (type != PK_PRACTICE)
		{
			DecPkValue(500);
			GetPkStatisticMgr().OnUpdatePkStatistic(type, (Occupation)raceEnd.opponent.occu, PKRR_LOSE);
		}
		result = PKRR_LOSE;

	}
	else if (raceEnd.result.resultFlag & resultBitmask_draw)
	{
		if (type != PK_PRACTICE)
		{
			IncPkValue(100);
			GetPkStatisticMgr().OnUpdatePkStatistic(type, (Occupation)raceEnd.opponent.occu, PKRR_DOGFALL);
		}
		result = PKRR_DOGFALL;
	}

	if (type == PK_SEASON_1V1 && raceEnd.seasonScoreChange != 0)
	{
		UInt32 newScore = GetMatchScore() + raceEnd.seasonScoreChange;
		if (newScore <= SEASON_MIN_MATCH_SCORE)
		{
			newScore = SEASON_MIN_MATCH_SCORE;
		}

		SetMatchScore(newScore);
	}
	
	if (type == PK_SEASON_1V1)
	{
		m_SeasonLevel.OnRaceEnd(raceEnd, ret.changeSeasonExp);
	}

	bool pvpActivityOpen = ActivityMgr::Instance()->IsInActivity(1002);

	std::string playerName;
	if (!MsgPackage::GetPlayerMsgTag(playerName, GetID(), GetName(), GetOccu(), GetLevel()))
	{
		playerName = GetName();
	}

	float additionRate = (float)ActivityMgr::Instance()->GetOpActivityAddition(OAT_PVP_PK_COIN, this) / 100 + 1;

	if (type == PK_SEASON_1V1)
	{
		if (result == PKRR_WIN)
		{
			ret.addPkCoinFromRace = AddPkCoinFromRace(20 * additionRate);
			if (pvpActivityOpen)
			{
				ret.addPkCoinFromActivity = AddPkCoinFromActivity(20);
			}
		}
		else if (result == PKRR_DOGFALL)
		{
			ret.addPkCoinFromRace = AddPkCoinFromRace(10 * additionRate);
			if (pvpActivityOpen)
			{
				ret.addPkCoinFromActivity = AddPkCoinFromActivity(10);
			}
		}
		else if (result == PKRR_LOSE)
		{
			ret.addPkCoinFromRace = AddPkCoinFromRace(5 * additionRate);
			if (pvpActivityOpen)
			{
				ret.addPkCoinFromActivity = AddPkCoinFromActivity(5);
			}
		}

		UpdateSeasonPkStraightCount(result);

		auto statistic = m_pkStatisticMgr.FindPkStatistic(PK_SEASON_1V1);
		if (statistic)
		{
			std::string systemLinkStr;
			MsgPackage::GetSystemSuffixesMsgTag(systemLinkStr, SystemSuffixesType::SYSTEM_SUFFIXES_TYPE_PVP, 0);
			UInt32 curWinStreak = statistic->GetCurWinStreak();
			switch (curWinStreak)
			{
			case ANNOUNCE_PVP_MAX_WIN_TEN:
				Sys::SendAnnouncement(22, playerName.c_str(), systemLinkStr.c_str());
				break;
			case ANNOUNCE_PVP_MAX_WIN_TWENTY:
				Sys::SendAnnouncement(23, playerName.c_str(), systemLinkStr.c_str());
				break;
			case ANNOUNCE_PVP_MAX_WIN_FIFTY:
				Sys::SendAnnouncement(24, playerName.c_str(), systemLinkStr.c_str());
				break;
			default:
				break;
			}
		}
	}

	m_TaskMgr.OnPVP(type, result);
	m_DailyTaskMgr.OnPVP(type, result);
	m_AchievementTaskMgr.OnPVP(type, result);
	m_MasterDailyTaskMgr.OnPVP(type, result);

	if (result == PKRR_WIN)
	{
		m_LostDungeonTaskMgr.OnKillSomeOne(raceEnd.opponent.roleId);
	}
	else if (result == PKRR_LOSE)
	{
		// 自己被杀，所有其他人 要杀自己的任务都可以完成
		m_LostDungeonTaskMgr.OnOwnerDead();
	}

	if (raceEnd.type == PK_WUDAODAHUI)
	{
		OnWudaodahuiPkEnd(result);
	}

	ret.isInPvPActivity = pvpActivityOpen ? 1 : 0;
	ret.totalPkCoinFromRace = GetCounter(COUNTER_DAY_PKCOIN_FROM_RACE);
	ret.totalPkCoinFromActivity = GetCounter(COUNTER_DAY_PKCOIN_FROM_ACTIVITY);
	ret.newMatchScore = GetMatchScore();
	ret.newPkValue = GetPkValue();
	ret.newSeasonLevel = GetSeasonLevel();
	ret.newSeasonStar = GetSeasonStar();
	ret.newSeasonExp = GetSeasonExp();
	SendProtocol(ret);

	UInt32 seasonId = m_SeasonLevel.GetSeasonId();

	SendPvpEndUdpLog(raceEnd.opponent.zoneId, raceEnd.opponent.roleId, (PkType)raceEnd.type, result, ret.oldMatchScore, ret.newMatchScore,
		ret.oldPkValue, ret.newPkValue, ret.oldSeasonLevel, ret.newSeasonLevel, ret.oldSeasonStar, ret.newSeasonStar, ret.oldSeasonExp, ret.newSeasonExp, seasonId, raceEnd.dungeonID, raceEnd.usedTime);
}

void Player::OnRoomPvp(const CLProtocol::MatchServerPkRoomRaceEnd& raceEnd, const Pk3V3PlayerRaceEndInfo& playerInfo)
{

	DebugLogStream << PLAYERINFO(this) << " OnRoomPvp raceId = " << raceEnd.raceId << LogStream::eos;

	CLProtocol::SceneRoomMatchPkRaceEnd ret;
	std::vector<ObjID_t>  teammateIds;
	for (UInt32 i = 0; i < raceEnd.results.size(); ++i)
	{
		RoomSlotBattleEndInfo slotInfo;
		slotInfo.resultFlag = raceEnd.results[i].info.resultFlag;
		slotInfo.roleId = raceEnd.results[i].info.roleId;
		slotInfo.roomId = raceEnd.results[i].roomId;
		slotInfo.roomType = (UInt8)RoomType::ROOM_TYPE_INVALID;
		if (raceEnd.type == PkType::PK_3V3_MATCH)
		{
			slotInfo.roomType = (UInt8)RoomType::ROOM_TYPE_THREE_MATCH;
		}
		else if (raceEnd.type == PkType::PK_3V3_ROOM)
		{
			slotInfo.roomType = (UInt8)RoomType::ROOM_TYPE_THREE_FREE;
		}
		else if (raceEnd.type == PkType::PK_3V3_ACTIVITY)
		{
			slotInfo.roomType = (UInt8)RoomType::ROOM_TYPE_THREE_SCORE_WAR;
			slotInfo.scoreWarBaseScore = raceEnd.results[i].scoreWarBaseScore;
			slotInfo.scoreWarContriScore = raceEnd.results[i].scoreWarContriScore;
		}
		else if (raceEnd.type == PkType::PK_3V3_TUMBLE)
		{
			slotInfo.roomType = (UInt8)RoomType::ROOM_TYPE_THREE_TUMBLE;
		}

		slotInfo.seasonExp = raceEnd.results[i].seasonExp;
		slotInfo.seasonLevel = raceEnd.results[i].seasonLevel;
		slotInfo.seasonStar = raceEnd.results[i].seasonStar;
		slotInfo.seat = raceEnd.results[i].info.pos;
		ret.slotInfos.push_back(slotInfo);
		if (playerInfo.info.resultFlag == slotInfo.resultFlag)
		{
			teammateIds.push_back(slotInfo.roleId);
		}
	}

	ret.pkType = raceEnd.type;
	ret.raceId = raceEnd.raceId;
	ret.result = playerInfo.info.resultFlag;
	ret.oldPkValue = GetPkValue();
	ret.oldMatchScore = GetMatchScore();
	ret.oldPkCoin = GetPkCoin();
	ret.oldSeasonLevel = GetSeasonLevel();
	ret.oldSeasonStar = GetSeasonStar();
	ret.oldSeasonExp = GetSeasonExp();

	PkType type = (PkType)raceEnd.type;

	PkRaceResult result = PKRR_INVALID;
	if (playerInfo.info.resultFlag & resultBitmask_win)
	{
		result = PKRR_WIN;
	}
	else if (playerInfo.info.resultFlag & resultBitmask_lose)
	{
		result = PKRR_LOSE;
	}
	else if (playerInfo.info.resultFlag & resultBitmask_draw)
	{
		result = PKRR_DOGFALL;
	}

	if (type == PkType::PK_3V3_MATCH)
	{
		std::vector<UInt32> winExps;
		std::vector<UInt32> lostExps;

		for (UInt32 i = 0; i < raceEnd.results.size(); ++i)
		{
			if ((raceEnd.results[i].info.resultFlag & resultBitmask_win) && result != PKRR_WIN)
			{
				UInt32 tmpSumExp = SeasonLevelDataEntryMgr::Instance()->GetSeasonLevelSumExp(raceEnd.results[i].seasonLevel) + raceEnd.results[i].seasonExp;
				winExps.push_back(tmpSumExp);
			}
			else if ((raceEnd.results[i].info.resultFlag & resultBitmask_lose) && result != PKRR_LOSE)
			{
				UInt32 tmpSumExp = SeasonLevelDataEntryMgr::Instance()->GetSeasonLevelSumExp(raceEnd.results[i].seasonLevel) + raceEnd.results[i].seasonExp;
				lostExps.push_back(tmpSumExp);
			}
		}

		UInt32 targetSeasonLevel = 0;
		if (result == PKRR_WIN)
		{
			UInt32 targetSeasonSumExp = GetRoomSumExp(lostExps);
			targetSeasonLevel = SeasonLevelDataEntryMgr::Instance()->GetSeasonLevelBySumExp(targetSeasonSumExp);
			ret.changeSeasonExp = m_SeasonLevel.CalculationSeasonExp(playerInfo.seasonLevel, targetSeasonLevel);
		}
		else if (result == PKRR_LOSE)
		{
			UInt32 targetSeasonSumExp = GetRoomSumExp(winExps);
			targetSeasonLevel = SeasonLevelDataEntryMgr::Instance()->GetSeasonLevelBySumExp(targetSeasonSumExp);
			ret.changeSeasonExp = m_SeasonLevel.CalculationSeasonExp(targetSeasonLevel, playerInfo.seasonLevel);
		}
		else if (result == PKRR_DOGFALL || result == PKRR_INVALID)
		{
			ret.changeSeasonExp = 0;
		}

		DebugLogStream << PLAYERINFO(this) << " OnRoomPvp plevel=" << playerInfo.seasonLevel << " targetSeasonLevel=" << targetSeasonLevel
			<< ", result:" << result << ")" << LogStream::eos;

		if (result == PKRR_WIN)
		{
			IncPkValue(500);
			GetPkStatisticMgr().OnUpdatePkStatistic(type, Occupation::OCCU_NONE, PKRR_WIN);
		}
		if (result == PKRR_LOSE)
		{
			DecPkValue(500);
			GetPkStatisticMgr().OnUpdatePkStatistic(type, Occupation::OCCU_NONE, PKRR_LOSE);
		}
		if (result == PKRR_DOGFALL)
		{
			IncPkValue(100);
			GetPkStatisticMgr().OnUpdatePkStatistic(type, Occupation::OCCU_NONE, PKRR_DOGFALL);
		}

		if (playerInfo.matchScoreChange != 0)
		{
			UInt32 newScore = GetMatchScore() + playerInfo.matchScoreChange;
			if (newScore <= SEASON_MIN_MATCH_SCORE)
			{
				newScore = SEASON_MIN_MATCH_SCORE;
			}

			SetMatchScore(newScore);
		}

		m_SeasonLevel.OnRoomRaceEnd(playerInfo, ret.changeSeasonExp);

		bool pvpActivityOpen = ActivityMgr::Instance()->IsInActivity(1002);
		float additionRate = (float)ActivityMgr::Instance()->GetOpActivityAddition(OAT_PVP_PK_COIN, this) / 100 + 1;

		if (result == PKRR_WIN)
		{
			ret.addPkCoinFromRace = AddPkCoinFromRace(20 * additionRate);
			if (pvpActivityOpen)
			{
				ret.addPkCoinFromActivity = AddPkCoinFromActivity(20);
			}
		}
		else if (result == PKRR_DOGFALL)
		{
			ret.addPkCoinFromRace = AddPkCoinFromRace(10 * additionRate);
			if (pvpActivityOpen)
			{
				ret.addPkCoinFromActivity = AddPkCoinFromActivity(10);
			}
		}
		else if (result == PKRR_LOSE)
		{
			ret.addPkCoinFromRace = AddPkCoinFromRace(5 * additionRate);
			if (pvpActivityOpen)
			{
				ret.addPkCoinFromActivity = AddPkCoinFromActivity(5);
			}
		}

		UpdateSeasonPkStraightCount(result);

		ret.isInPvPActivity = pvpActivityOpen ? 1 : 0;
		ret.totalPkCoinFromRace = GetCounter(COUNTER_DAY_PKCOIN_FROM_RACE);
		ret.totalPkCoinFromActivity = GetCounter(COUNTER_DAY_PKCOIN_FROM_ACTIVITY);
		ret.newMatchScore = GetMatchScore();
		ret.newPkValue = GetPkValue();
		ret.newSeasonLevel = GetSeasonLevel();
		ret.newSeasonStar = GetSeasonStar();
		ret.newSeasonExp = GetSeasonExp();
	}
	else if (type == PkType::PK_3V3_ACTIVITY)
	{
		
	}

	SendProtocol(ret);

	m_TaskMgr.OnPVP(type, result);
	m_DailyTaskMgr.OnPVP(type, result);
	m_AchievementTaskMgr.OnPVP(type, result);

	if (teammateIds.size() > 0)
	{
		m_MasterAcademicGrowthTaskMgr.OnTeamPVP(type, result, teammateIds);
	}
	

	UInt32 seasonId = m_SeasonLevel.GetSeasonId();

	SendPvpEndUdpLog(0, 0, (PkType)raceEnd.type, result, ret.oldMatchScore, ret.newMatchScore, ret.oldPkValue, ret.newPkValue, 
		ret.oldSeasonLevel, ret.newSeasonLevel, ret.oldSeasonStar, ret.newSeasonStar, ret.oldSeasonExp, ret.newSeasonExp, seasonId, raceEnd.dungeonID, raceEnd.usedTime);

	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand(GetSrcZoneId(), "t_player_info", GetID(), true);
	GetDBData(cmd, true);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void Player::OnChangeOccu(JobDataEntry* targetJob)
{
	if (SERVER_TYPE == ST_BSCENE)
	{
		return;
	}

	if (targetJob == NULL) return;
	//重置技能与SP点数.添加新的技能.
	GetSkillMgr().OnChangeOccu();
	GetPvpSkillMgr().OnChangeOccu();

	m_TaskMgr.OnChangeOccu();
	m_DailyTaskMgr.OnChangeOccu();
	m_AchievementTaskMgr.OnChangeOccu();

	SortListType sortListType = GetLevelSortListTypeByOccu(GetOccu());
	if (sortListType != SORTLIST_INVALID)
	{
		SortListMgr::Instance()->OnSortValueChanged(sortListType, GetID(), GetName(), 0, GetOccu(), 0, GetLevel(), (UInt32)GetLevel(), (UInt32)GetExp(), 0);
	}

	//发个礼包
	RewardGroup* rewardGroup = GetRewardGroup();
	rewardGroup->AddReward(targetJob->itemId, 1);
	AddRewards(GetReason(SOURCE_TYPE_PLAYER_CHANGE_OCCU, targetJob->occu).c_str(), rewardGroup);

	//发个通告
	//Sys::SendAnnouncement(11, GetName().c_str());

	SyncBaseInfoToWorld();
}

void Player::OnAddMagic(ItemDataEntry* data, Item* equip)
{
	if (!data || !equip)
		return;

	if (data->color == ITEM_QUALITY_BLUE)
	{
		SetCounter(COUNTER_BLUE_MAGIC_CARD, GetCounter(COUNTER_BLUE_MAGIC_CARD) + 1);
	}
	else if (data->color == ITEM_QUALITY_PURPLE)
	{
		SetCounter(COUNTER_PP_MAGIC_CARD, GetCounter(COUNTER_PP_MAGIC_CARD) + 1);
	}

	m_TaskMgr.OnAddMagic();
	m_DailyTaskMgr.OnAddMagic();
	m_AchievementTaskMgr.OnAddMagic();

	SendFunctionUdpLog(FunctionLogType::FUNCTION_ADD_MAGIC, data->id);

	//计算装备评分
	GetItemMgr().CalculateValuedScore(equip);
}

void Player::OnAddMagicCard()
{
	m_TaskMgr.OnAddMagicCard();
	m_DailyTaskMgr.OnAddMagicCard();
	m_AchievementTaskMgr.OnAddMagicCard();
}

void Player::OnOpenMagicJar(JarType type, UInt32 combo)
{
	//m_TaskMgr.OnOpenMagicJar(combo);
	m_DailyTaskMgr.OnOpenMagicJar(type, combo);
	m_AchievementTaskMgr.OnOpenMagicJar(type, combo);
	
	SendFunctionUdpLog(FunctionLogType::FUNCTION_OPEN_JAR, type, combo);
}

void Player::OnFriendGive()
{
	//m_TaskMgr.OnFriendGive();
	m_DailyTaskMgr.OnFriendGive();
	m_AchievementTaskMgr.OnFriendGive();
}

void Player::OnRefreshShop()
{
	//m_TaskMgr.OnRefreshShop();
	m_DailyTaskMgr.OnRefreshShop();
	m_AchievementTaskMgr.OnRefreshShop();
}

void Player::OnShopBuy(UInt32 shopId, UInt32 shopItemId, UInt32 num, UInt32 newNum, UInt32 dungeonId)
{
	m_TaskMgr.OnShopBuy(shopId, shopItemId, num);
	m_DailyTaskMgr.OnShopBuy(shopId, shopItemId, num);

	SendShopBuyUdpLog(shopId, shopItemId, num, newNum, dungeonId);
}

void Player::OnBuyMallItem(UInt32 mallItemId, UInt32 num)
{
	m_DailyTaskMgr.OnBuyMallItem(mallItemId, num);

	SendMallBuyUdpLog(mallItemId, num);
}

void Player::OnRemoveFatigue(UInt16 usedFatigue)
{
	m_PetMgr.OnRemoveFatigue(usedFatigue);

	//m_TaskMgr.OnRemoveFatigue(usedFatigue);
	m_DailyTaskMgr.OnRemoveFatigue(usedFatigue);
	m_AchievementTaskMgr.OnRemoveFatigue(usedFatigue);
	m_MasterDailyTaskMgr.OnRemoveFatigue(usedFatigue);
	m_MasterAcademicGrowthTaskMgr.OnRemoveFatigue(usedFatigue);

	auto contriPerFatigue = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_CONTRI_FROM_FATIGUE);
	AddGuildContribution(GetReason(SOURCE_TYPE_GUILD).c_str(), contriPerFatigue * usedFatigue, true);

	// 给师傅加疲劳
	AddFatigueToMaster(usedFatigue);

	// 回归期间消耗疲劳获得精力币
	if (IsVeteranReturn())
	{
		RefreshEnergyCoin(GetEnergyCoin() + usedFatigue);
	}
}

void Player::OnClearHell(UInt32 dungeonId, UInt32 usedTime, LevelScore score, std::vector<ObjID_t> teammateIds)
{
	m_TaskMgr.OnClearHell(dungeonId, usedTime, score);
	m_DailyTaskMgr.OnClearHell(dungeonId, usedTime, score);
	m_AchievementTaskMgr.OnClearHell(dungeonId, usedTime, score);
	m_MasterDailyTaskMgr.OnClearHell(dungeonId, usedTime, score);
	m_MasterAcademicGrowthTaskMgr.OnClearHell(dungeonId, usedTime, score);

	if (teammateIds.size() > 0)
	{
		m_MasterDailyTaskMgr.OnTeamClearHell(dungeonId, usedTime, score, teammateIds);
		m_MasterAcademicGrowthTaskMgr.OnTeamClearHell(dungeonId, usedTime, score, teammateIds);
	}
}

void Player::OnDeathTowerBegin(UInt32 towerId)
{
	m_TaskMgr.OnDeathTowerBegin(towerId);
	m_DailyTaskMgr.OnDeathTowerBegin(towerId);
	//m_AchievementTaskMgr.OnDeathTowerBegin(towerId);
}

void Player::OnDeathTower(UInt32 towerId, UInt32 layer, UInt32 usedTime)
{
	DebugLogStream << "player(" << GetID() << ") tower(" << towerId << ") layer(" << layer << ") usedTime(" << usedTime << ")." << LogStream::eos;
	m_TaskMgr.OnDeathTower(towerId, layer, usedTime);
	m_DailyTaskMgr.OnDeathTower(towerId, layer, usedTime);
	m_AchievementTaskMgr.OnDeathTower(towerId, layer, usedTime);
	//m_ActiveTaskMgr.OnDeathTower(towerId, layer, usedTime);
	m_RedPacketTaskMgr.OnDeathTower(towerId, layer, usedTime);
	m_MasterAcademicGrowthTaskMgr.OnDeathTower(towerId, layer, usedTime);
}

void Player::OnAwaken()
{
	GetSkillMgr().OnAwaken();
	GetPvpSkillMgr().OnAwaken();

	m_TaskMgr.OnAwaken();
	m_DailyTaskMgr.OnAwaken();
	m_AchievementTaskMgr.OnAwaken();

	//发个公告
	//Sys::SendAnnouncement(12, GetName().c_str());
}

void Player::OnReviveCoin(bool isSelf)
{
	//m_TaskMgr.OnReviveCoin(isSelf);
	m_DailyTaskMgr.OnReviveCoin(isSelf);
	m_AchievementTaskMgr.OnReviveCoin(isSelf);
}

void Player::OnAddAuction()
{
	//m_TaskMgr.OnAddAuction();
	m_DailyTaskMgr.OnAddAuction();
	m_AchievementTaskMgr.OnAddAuction();
}

void Player::OnSellAuction(UInt32 money)
{
	//m_TaskMgr.OnSellAuction();
	m_DailyTaskMgr.OnSellAuction(money);
	m_AchievementTaskMgr.OnSellAuction(money);
}

void Player::OnBuyAuction(UInt32 money)
{
	m_AchievementTaskMgr.OnBuyAuction(money);
}

void Player::OnAcceptTask(UInt32 taskId, TaskType type)
{
	if (type == TASK_MAIN)
	{
		// 计算新开放的关卡
		GetDungeonMgr().SyncDungeonList();
	}

	// 日志
	//SendUdpLog("task_accept", LOG_TASK_ACCEPT_FORMAT, taskId, type);
}

void Player::OnAbandonTask(UInt32 taskId, TaskType type)
{
	// 日志
	SendUdpLog("task_abandon", LOG_TASK_ABANDON_FORMAT, taskId, type);
}

void Player::OnDailyTaskSubmitted(UInt32 taskId, TaskType type, TaskSubType subType)
{
	if (subType == TaskSubType::TST_DAILY_TASK)
	{
		m_TaskMgr.OnDailyTaskSubmitted(taskId, type);
		m_DailyTaskMgr.OnDailyTaskSubmitted(taskId, type);
		m_AchievementTaskMgr.OnDailyTaskSubmitted(taskId, type);
	}
}

void Player::OnSubmitTaskSucceed(UInt32 taskId, TaskType type)
{
	m_DailyTaskMgr.OnSubmitTaskSucceed(taskId);
	m_AchievementTaskMgr.OnSubmitTaskSucceed(taskId);

	if (type == TaskType::TASK_MAIN)
	{
		// 计算新开放的关卡
		GetDungeonMgr().SyncDungeonList();
	}

	// 日志
	UInt32 param = 0;
	if (type == TaskType::TASK_DAILY)
	{
		TaskDataEntry* task = TaskDataEntryMgr::Instance()->FindEntry(taskId);
		if (task != NULL)
		{
			param = task->GetDailyTaskScore();
		}
	}
	SendUdpLog("task_submit", LOG_TASK_SUBMIT_FORMAT, taskId, type, param);
}

void Player::OnSubmitActiveTaskSucceed(UInt32 templateId, UInt32 taskId)
{
	m_DailyTaskMgr.OnSubmitActiveTaskSucceed(templateId, taskId);
	m_AchievementTaskMgr.OnSubmitActiveTaskSucceed(templateId, taskId);

	SendActiveTaskFinishUdpLog(templateId, taskId);
}

void Player::OnSubmitCycleTaskSucceed(UInt32 taskId)
{
	m_DailyTaskMgr.OnSubmitCycleTaskSucceed(taskId);
	m_AchievementTaskMgr.OnSubmitCycleTaskSucceed(taskId);
	m_MasterDailyTaskMgr.OnSubmitCycleTaskSucceed(taskId);
	//m_CycleTaskMgr.OnSubmitCycleTaskSucceed(taskId);
}

void Player::OnSubmitLegendTaskSucceed(BaseTask* task)
{
	if (task == NULL) return;
	if (task->IsAnnouncement())
	{
		std::string playerLinkStr;
		if (!MsgPackage::GetPlayerMsgTag(playerLinkStr, GetID(), GetName(), GetOccu(), GetLevel()))
		{
			playerLinkStr = GetName();
		}

		std::string systemLinkStr;
		MsgPackage::GetSystemSuffixesMsgTag(systemLinkStr, SystemSuffixesType::SYSTEM_SUFFIXES_TYPE_LEGENT, task->GetDataId());

		Sys::SendAnnouncementByStartTime(41, ((UInt32)CURRENT_TIME.Sec()) + 5, playerLinkStr.c_str(), task->GetName().c_str(), systemLinkStr.c_str());
	}

}

void Player::OnMakeItem(const char* reason, Item* item, Int32 num, bool isSendLog)
{
	if (item == NULL) return;
		ReasonInfo param;
	if (!ParseReason(reason, param)) return;

	if (item->GetCanAnnounce() ||
		(param.type == SOURCE_TYPE_CHARGE && item->GetStrengthen() >= 10 ))
		{
			std::string linkStr;

			std::string playerLinkStr;
			if (!MsgPackage::GetPlayerMsgTag(playerLinkStr, GetID(), GetName(), GetOccu(), GetLevel()))
			{
				playerLinkStr = GetName();
			}

			if (MsgPackage::GetItemMsgTag(linkStr, item->GetID(), item->GetDataID(), (UInt32)item->GetStrengthen()))
			{
				WorldCacheItemData(item);
				LogSourceType type = (LogSourceType)((param.type / LOG_SOURCE_TYPE_RATIO) * LOG_SOURCE_TYPE_RATIO);
				if (type == SOURCE_TYPE_DUNGEON)
				{
					DungeonDataEntry* dungeonData = DungeonDataEntryMgr::Instance()->FindEntry((UInt32)param.param1);
					// 引导关卡不弹提示
					if (dungeonData != NULL && (dungeonData->subType != DST_HELL || dungeonData->hellSplitLevel > 25))
					{
						UInt32 announcementId = 9;
						if (dungeonData->subType == DungeonSubType::DST_HELL)
						{
							announcementId = 35 + (UInt32)dungeonData->hardType;
							UInt32 hellEntryDungeonId = DungeonDataEntryMgr::Instance()->GetHellEntry(dungeonData->hellSplitLevel, dungeonData->hardType);
							DungeonDataEntry* tmpData = DungeonDataEntryMgr::Instance()->FindEntry(hellEntryDungeonId);
							if (tmpData != NULL) dungeonData = tmpData;
						}
						else if (dungeonData->subType == DungeonSubType::DST_YUANGU)
						{
							announcementId = 35 + (UInt32)dungeonData->hardType;
						}

						std::string systemLinkStr;
						MsgPackage::GetSystemSuffixesMsgTag(systemLinkStr, SystemSuffixesType::SYSTEM_SUFFIXES_TYPE_DUNGEON, dungeonData->id);

						Sys::SendAnnouncement(announcementId, playerLinkStr.c_str(), dungeonData->name, linkStr.c_str(), systemLinkStr.c_str());
					}
				}
				else if (type == SOURCE_TYPE_SHOP)
				{
					ShopDataEntry* shopData = ShopDataEntryMgr::Instance()->FindEntry((UInt32)param.param1);
					if (shopData != NULL)
					{
						if (shopData->shopType == ST_DUNGEON)
						{
							//Sys::SendAnnouncement(74, playerLinkStr.c_str(), shopData->name, linkStr.c_str());
						}
						else
						{
							std::string systemLinkStr;
							if (shopData->shopType == ST_MAGIC)
							{
								MsgPackage::GetSystemSuffixesMsgTag(systemLinkStr, SystemSuffixesType::SYSTEM_SUFFIXES_TYPE_MAGIC_SHOP, shopData->shopId);
							}
							else
							{
								UInt32 shopMainframeId = ShopMainframeDataEntryMgr::Instance()->FindKeyByShopId(shopData->GetKey());
								MsgPackage::GetSystemSuffixesMsgTag(systemLinkStr, SystemSuffixesType::SYSTEM_SUFFIXES_TYPE_SHOP, shopData->shopId, shopMainframeId);
							}
							Sys::SendAnnouncement(7, playerLinkStr.c_str(), shopData->name, linkStr.c_str(), systemLinkStr.c_str());
						}
					}
				}
				else if (param.type == SOURCE_TYPE_CHARGE)
				{
					UInt32 firstChargeIdHead = 83;
					UInt32 heapChargeIdHead = 84;
					UInt32 idHead = (UInt32)(param.param1 / 100);

					std::string numStr = "";
					if (num > 1)
					{
						numStr = "*" + Avalon::StringUtil::ConvertToString(num);
					}

					if (idHead == firstChargeIdHead || idHead == heapChargeIdHead)
					{
						std::string systemLinkStr;
						SystemSuffixesType systemId = SystemSuffixesType::SYSTEM_SUFFIXES_TYPE_RECHARGE;
						if (idHead == firstChargeIdHead)
						{
							systemId = SystemSuffixesType::SYSTEM_SUFFIXES_TYPE_FIRST_RECHARGE;
						}

						MsgPackage::GetSystemSuffixesMsgTag(systemLinkStr, systemId, 0);
						Sys::SendAnnouncement(39, playerLinkStr.c_str(), linkStr.c_str(), numStr.c_str(), systemLinkStr.c_str());
					}
				}
				else if (param.type == SOURCE_TYPE_TASK_LEGEND)
				{
					TaskDataEntry* taskDataEntry = TaskDataEntryMgr::Instance()->FindEntry((UInt32)param.param1);
					if (taskDataEntry != NULL)
					{
						std::string systemLinkStr;
						MsgPackage::GetSystemSuffixesMsgTag(systemLinkStr, SystemSuffixesType::SYSTEM_SUFFIXES_TYPE_LEGENT, taskDataEntry->id);
						Sys::SendAnnouncementByStartTime(40, ((UInt32)CURRENT_TIME.Sec()) + 5, playerLinkStr.c_str(), taskDataEntry->name, linkStr.c_str(), systemLinkStr.c_str());
					}
				}

// 				else if (param.type == SOURCE_TYPE_ITEM_JAR)
// 				{
// 					Sys::SendAnnouncement(10, playerLinkStr.c_str(), linkStr.c_str());
// 				}
			}
	
		}

	if (isSendLog)
	{
		SendItemUdpLog(reason, item->GetID(), item->GetDataID(), (ItemType)item->GetType(), (ItemSubType)item->GetSubType(), item->GetName(), (UInt32)item->GetQuality(), num, 0);
	}
}

void Player::OnRemoveItem(const char* reason, Item* item, Int32 num, UInt32 oldNum)
{
	SendItemUdpLog(reason, item->GetID(), item->GetDataID(), (ItemType)item->GetType(), (ItemSubType)item->GetSubType(), item->GetName(), (UInt32)item->GetQuality(), num * -1, oldNum);
}

void Player::OnMerge(const char* reason, Item* item, Int32 num, UInt32 oldNum)
{
	SendItemUdpLog(reason, item->GetID(), item->GetDataID(), (ItemType)item->GetType(), (ItemSubType)item->GetSubType(), item->GetName(), (UInt32)item->GetQuality(), num, oldNum);
}

void Player::OnNewSkill(UInt16 skillId, bool isSetting)
{
	m_SkillBar.OnNewSkill(skillId, isSetting);
}

void Player::OnChangeSkill(UInt16 skillId, std::string skillName, Int8 changeLevel, UInt8 sourceLevel, UInt8 newLevel, bool isSetting)
{
	/*
	//自动把技能放到技能栏里
	if (sourceLevel <= 0 && newLevel >= 0)
	{
		OnNewSkill(skillId, isSetting);
	}
	*/

	SendChangeSkillUdpLog(skillId, skillName, changeLevel, sourceLevel, newLevel);
}

void Player::OnSeasonLevelUp()
{

}

void Player::OnFeedPet(PetFeedType type)
{
	m_DailyTaskMgr.OnFeedPet(type);
	m_AchievementTaskMgr.OnFeedPet(type);
}

void Player::OnAddPet(PetType petType, PetQuality petQuality)
{
	m_AchievementTaskMgr.OnAddPet(petType, petQuality);
}

void Player::OnPetUpLevel(PetType petType, PetQuality petQuality, UInt16 level)
{
	m_AchievementTaskMgr.OnPetUpLevel(petType, petQuality, level);
}

void Player::OnAddRelation(RelationType type)
{
	m_TaskMgr.OnAddRelation(type);
	m_AchievementTaskMgr.OnAddRelation(type);
}

void Player::OnRemakeEquip(UInt32 itemId, UInt8 qualityLevel)
{
	m_AchievementTaskMgr.OnRemakeEquip(itemId, qualityLevel);
}

void Player::OnCalculateValuedScore(ItemSubType subType, ItemQuality quality, UInt32 score)
{
	m_AchievementTaskMgr.OnCalculateValuedScore(subType, quality, score);
}

void Player::OnAddPreciousBead()
{
	m_AchievementTaskMgr.OnAddPreciousBead();
}

void Player::OnSendHorn()
{
	m_AchievementTaskMgr.OnSendHorn();
}

void Player::OnJoinGuild()
{
	m_AchievementTaskMgr.OnJoinGuild();
}

void Player::OnIncGuildBattleScore(UInt32 score)
{
	m_AchievementTaskMgr.OnIncGuildBattleScore(score);
}

void Player::OnGuildOccupyTerritory(UInt8 terrId)
{
	m_AchievementTaskMgr.OnGuildOccupyTerritory(terrId);
}

void Player::OnRelationRelieveMaster(ObjID_t masterId)
{
	m_AcademicTotalGrowth = 0;
	m_MasterDailyTaskGrowth = 0;
	m_MasterAcademicTaskGrowth = 0;
	m_MasterGiveEquipGrowth = 0;
	m_MasterUplevelGrowth = 0;
	m_MasterGiveGiftGrowth = 0;
	m_MasterDailyTaskMgr.OnRelationRelieveMaster(masterId);
	m_MasterAcademicGrowthTaskMgr.OnRelationRelieveMaster(masterId);
	//去除师徒弟子称号
	std::string reason = GetReason(MASTERSYS_RELIEVED, masterId);
	UInt32 titleId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_MASTERSYS_DISCIPLE_TITLEID);
	this->RemoveItem(reason.c_str(), titleId, 1);

	if (this->GetGameStatus() != PS_NORMAL)
	{
		this->SyncToRecord();
	}
}

void Player::OnRelationApprenticeMaster(ObjID_t masterId)
{
	InfoLogStream << "enter  OnRelationApprenticeMaster masterId : " << masterId << LogStream::eos;
	m_AcademicTotalGrowth = 0;
	m_MasterDailyTaskGrowth = 0;
	m_MasterAcademicTaskGrowth = 0;
	m_MasterGiveEquipGrowth = 0;
	m_MasterUplevelGrowth = 0;
	m_MasterGiveGiftGrowth = 0;

	m_MasterDailyTaskMgr.AbandonAllTask(false);
	m_MasterAcademicGrowthTaskMgr.AbandonAllTask(false);
	m_MasterAcademicGrowthTaskMgr.AcceptAllTask(true);
	
	if (this->GetGameStatus() != PS_NORMAL)
	{
		//同步师门任务离线数据
		CLProtocol::SceneSyncMasterTaskShareDataToWorld protocol;
		protocol.roleId = this->GetID();
		this->GetMasterTaskShareData(protocol.masterTaskShareData);
		Router::SendToWorld(protocol);
		this->SyncToRecord();
	}
	//徒弟获取称号
	std::string reason = GetReason(MASTERSYS_APPRENTICE, masterId);
	UInt32 titleId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_MASTERSYS_DISCIPLE_TITLEID);
	RewardGroup rewardgrop;
	rewardgrop.AddReward(titleId,1);
	this->AddRewards(reason.c_str(), &rewardgrop);
}

void Player::OnRelationDiscipleFinishSchool(ObjID_t masterId)
{
	m_MasterDailyTaskMgr.OnRelationDiscipleFinishSchool(masterId);
	m_MasterAcademicGrowthTaskMgr.OnRelationDiscipleFinishSchool(masterId);

	//徒弟获取称号
	std::string reason = GetReason(MASTERSYS_RECV_FINSCH_RWD, masterId);
	UInt32 titleId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_MASTERSYS_FINSCH_TITLEID);
	RewardGroup rewardgrop;
	rewardgrop.AddReward(titleId, 1);
	this->AddRewards(reason.c_str(), &rewardgrop);
}

void Player::OnAddGoodTeacherValueByFinishSchool(UInt64 addValue, ObjID_t discipleId)
{
	std::string reason = GetReason(MASTERSYS_ADD_GOODTEACH, discipleId);
	this->AddItem(reason.c_str(), 600000302, addValue, 0, 0, true);
}

void Player::OnWinWudaodahuiPkEnd(UInt32 winNum)
{
	m_AchievementTaskMgr.OnWinWudaodahuiPkEnd(winNum);
}

void Player::OnEnlargePackage(UInt32 size)
{
	m_AchievementTaskMgr.OnEnlargePackage(size);
}

void Player::OnEnlargeStorage(UInt32 size)
{
	m_AchievementTaskMgr.OnEnlargeStorage(size);
}

void Player::OnWeekHellFail(DungeonDataEntry& dungeonData)
{
	if (!SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_WEEK_HELL_FAIL_RETURN_TICKETS))
	{
		InfoLogStream << PLAYERINFO(this) << "SERVICE_WEEK_HELL_FAIL_RETURN_TICKETS is close" << LogStream::eos;
		return;
	}
	
	if (dungeonData.ticket.itemId == 0 || dungeonData.ticket.num <= 0)
	{
		InfoLogStream << PLAYERINFO(this) << "ticket itemId = " << dungeonData.ticket.itemId << ", num = " << dungeonData.ticket.num << LogStream::eos;
		return;
	}
	ItemDataEntry* itemData = ItemDataEntryMgr::Instance()->FindEntry(dungeonData.ticket.itemId);
	if (!itemData)
	{
		InfoLogStream << PLAYERINFO(this) << "itemData of ticket is null, ticket itemId = " << dungeonData.ticket.itemId << LogStream::eos;
		return;
	}
	
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	SysNotifyMgr::Instance()->MakeNotify(10026, buffer, sizeof(buffer));
	std::string strSenderName(buffer);

	std::string	strTitle;
	std::string	strContent;
	std::string strReason;

	SysNotifyMgr::Instance()->MakeNotify(10025, buffer, sizeof(buffer));
	strTitle = buffer;

	SysNotifyMgr::Instance()->MakeNotify(10027, buffer, sizeof(buffer));
	strContent = buffer;

	UInt32 ticketNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_WEEK_HELL_FAIL_RETURN_TICKETS);
	RewardGroup rewardGroup;
	rewardGroup.AddReward(dungeonData.ticket.itemId, ticketNum);
	
	strReason = GetReason(SOURCE_TYPE_WEEKHELL_FAIL_RETURN_TICKET, dungeonData.id);
	Sys::SendSysMail(GetID(), strSenderName.c_str(), strTitle.c_str(), strContent.c_str(), strReason.c_str(), &rewardGroup);
	InfoLogStream << PLAYERINFO(this) << " send mail for back ticket OnWeekHellFail, ticket itemId = " << dungeonData.ticket.itemId << ", num = " << ticketNum << LogStream::eos;
}

void Player::OnDataLoaded()
{
	DebugLogStream << "player(" << GetID() << ") load data finished." << LogStream::eos;

	if((m_TourZoneId != 0 && m_TourZoneId != ZONE_ID) || !SetupBase())
	{
		ErrorLogStream << "player(" << GetAccID() << "," << GetID() << "," 
			<< GetName() << ") setup level data failed!" << LogStream::eos;
		
		//进入失败
		CLProtocol::SceneEnterGameRet ret;
		ret.id = GetID();
		ret.result = ErrorCode::ENTERGAME_INIT_FAILED;
		SendToGate(ret);

		LeaveGame();
		return;
	}

	/*
	// by huchenhui
	if(m_VipLvl != 0 && CURRENT_TIME.Sec() > m_VipDueTime)
	{
		m_VipLvl = 0;
	}
	*/

	//通知world上线
	CLProtocol::WorldRegisterPlayerReq req;
	req.id = GetID();
	req.accid = GetAccID();
	req.srcZoneId = GetSrcZoneId();
	req.name = GetName();
	req.zoneid = GetZoneId();
	req.level = GetLevel();
	req.sex = GetSex();
	req.headFrame = GetHeadFrameId();
	req.viplvl = GetVipLvl();
	req.qqVipInfo = m_qqVipInfo;
	req.point = GetPoint();
	req.occu = GetOccu();
	req.createtime = m_CreateTime;
	req.offlinetime = GetSavebackTime();
	req.localOfflineTime = GetLocalSavebackTime();
	req.gateid = m_Conn->GetID();
	req.openid = m_OpenId;
	req.city = m_City;
	req.ip = m_Ip;
	req.pf = m_Pf;
	req.did = m_DeviceId;
	req.model = m_Model;
	req.deviceVersion = m_DeviceVersion;
	req.auctionAdditionBooth = GetAuctionAdditionBooth();
	req.auctionRefreshTime = GetAuctionRefreshTime();
	req.monthCardExpireTime = m_MonthCardExpireTime;
	req.bornZoneId = GetBornZoneId();
	req.roleReturnTime = GetRoleReturnTime();

	Router::SendToWorld(req);

	SetGameStatus(PS_INIT);
}

void Player::SendToGate(Avalon::Protocol& protocol)
{
	if(m_Conn == NULL) return;

	CL_SEND_PROTOCOL(m_Conn, protocol);
}

void Player::SendProtocol(Avalon::Protocol& protocol)
{
	if(m_Conn == NULL) return;

	CLProtocol::GateTransmitPacket req;
	req.id = GetID();
	if(protocol.Encode(req.packet))
	{
		CLNetworkPerfMgr::Instance()->OnSendPacket(GetAccID(), req.packet->GetID(), req.packet->GetSize());
		CL_SEND_PROTOCOL(m_Conn,req);
	}
}

void Player::SendPacket(Avalon::Packet* packet)
{
	if(m_Conn == NULL) return;

	CLNetworkPerfMgr::Instance()->OnSendPacket(GetAccID(), packet->GetID(), packet->GetSize());

	CLProtocol::GateTransmitPacket req(packet);
	req.id = GetID();
	CL_SEND_PROTOCOL(m_Conn,req);
	req.Detach();
}

void Player::SendToGlobalActivity(Avalon::Protocol& protocol)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	if (protocol.Encode(packet))
	{
		SendToGlobalActivity(packet);
	}
	Avalon::Packet::Destroy(packet);
}

void Player::SendToGlobalActivity(Avalon::Packet* packet)
{
	CLProtocol::SysTransmitPlayerFormNodesToGlobal transmit(packet);
	transmit.playerInfo.roleId = GetID();
	transmit.playerInfo.accId = GetAccID();
	transmit.playerInfo.name = GetName();
	transmit.playerInfo.platform = GetPf();
	transmit.playerInfo.serverZoneId = ZONE_ID;
	transmit.playerInfo.serverName = SERVER_NAME;
	transmit.playerInfo.sourceNode[ST_SCENE] = NODE_ID;
	transmit.playerInfo.sourceNode[ST_GATE] = GetGateID();

	transmit.destNode = GetGlobalActivityNodeID(0);

	Router::SendToAdmin(transmit);
	transmit.Detach();
}

void Player::SendToSelfScene(Avalon::Protocol& protocol)
{
	Router::SendToSelfScene(this->GetSrcSceneNodeID(), protocol);
}

void Player::SyncSelfData(bool bDirty)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::SCENE_SYNC_SELF/* | CL_PACKET_COMPRESSED_FLAG*/);
	Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
	bool hasaddr = false;
	try
	{
		//modified by aprilliu
		hasaddr = Encode(stream, SST_TEST, bDirty);
		//hasaddr = Encode(stream, SST_SELF, bDirty);
	}
	catch(Avalon::NetException&)
	{
		Avalon::Packet::Destroy(packet);
		return;
	}
	if(hasaddr)
	{
		//DebugLogStream << "player(" << GetID() << ") sync self." << LogStream::eos;

		packet->SetSize(packet->GetSize() + UInt16(stream.GetPos()));
		SendPacket(packet);
	}
	Avalon::Packet::Destroy(packet);
}

void Player::SyncSelfData(UInt32 type, bool bDirty)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::SCENE_SYNC_SELF/* | CL_PACKET_COMPRESSED_FLAG*/);
	Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
	bool hasaddr = false;
	try
	{
		//modified by aprilliu
		hasaddr = Encode(stream, type, bDirty);
		//hasaddr = Encode(stream, SST_SELF, bDirty);
	}
	catch (Avalon::NetException&)
	{
		Avalon::Packet::Destroy(packet);
		return;
	}
	if (hasaddr)
	{
		//DebugLogStream << "player(" << GetID() << ") sync self." << LogStream::eos;

		packet->SetSize(packet->GetSize() + UInt16(stream.GetPos()));
		SendPacket(packet);
	}
	Avalon::Packet::Destroy(packet);
}

class SyncTeamInfoVisitor : public PlayerVisitor
{
public:
	SyncTeamInfoVisitor(Player* player, Avalon::Packet* packet):m_pPlayer(player), m_pPacket(packet){}
	~SyncTeamInfoVisitor(){}

	bool operator()(Player* player)
	{
		if(m_pPlayer == player) return true;
		if(m_pPlayer->GetTeam()->IsMember(player->GetID()))
		{
			player->SendPacket(m_pPacket);
		}
		return true;
	}

private:
	Player* m_pPlayer;
	Avalon::Packet* m_pPacket;
};

void Player::SyncTeamAround(bool bDirty)
{
	if(GetTeam() == NULL || GetTeam()->GetSize() <= 1) return;

	Avalon::Packet* packet = MakeSyncPropertyPacket(SST_TEAM, bDirty);
	if(packet == NULL) return;
	SyncTeamInfoVisitor visitor(this, packet);
	GetScene()->VisitPlayersAround(GetPos(), visitor);
	Avalon::Packet::Destroy(packet);
}

class SyncTeamInfoToMeVisitor : public PlayerVisitor
{
public:
	SyncTeamInfoToMeVisitor(Player* player):m_pPlayer(player){}
	~SyncTeamInfoToMeVisitor(){}

	bool operator()(Player* player)
	{
		if(m_pPlayer == player) return true;
		if(m_pPlayer->GetTeam()->IsMember(player->GetID()))
		{
			Avalon::Packet* packet = player->MakeSyncPropertyPacket(SST_TEAM, false);
			m_pPlayer->SendPacket(packet);
			Avalon::Packet::Destroy(packet);
		}
		return true;
	}

private:
	Player* m_pPlayer;
};

void Player::SyncTeamAroundToMe()
{
	if(GetTeam() == NULL || GetTeam()->GetSize() <= 1) return;

	SyncTeamInfoToMeVisitor visitor(this);
	GetScene()->VisitPlayersAround(GetPos(), visitor);
}

void Player::SyncTeamNewZone()
{
	if(GetTeam() == NULL || GetTeam()->GetSize() <= 1) return;

	Avalon::Packet* packet = MakeSyncPropertyPacket(SST_TEAM, false);
	if(packet == NULL) return;
	SyncTeamInfoVisitor visitor(this, packet);
	GetScene()->VisitPlayersNewZone(this, visitor);
	Avalon::Packet::Destroy(packet);
}

void Player::SyncToRecord(bool bPos)
{
	//定时保存数据，不包括位置
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand(GetSrcZoneId(), "t_player_info", GetID(), false);
	GetDBData(cmd,true);
	SceneDataEntry* dataEntry = SceneDataEntryMgr::Instance()->FindEntry(m_MajorCityId);
	if(bPos && dataEntry != NULL && IsStaticMap(dataEntry->type))
	{
		cmd->PutData("sceneid", GetSceneID());
		cmd->PutData("mapid", m_MajorCityId);
		cmd->PutData("dir", GetDir());
		cmd->PutData("pos", GetPos().ToString());
	}
	if(cmd->IsValid())
	{
		SetSavebackTime(CURRENT_TIME.MSec());
		cmd->PutData("savetime", GetSavebackTime());
		cmd->PutData("totleonlinetime", GetTotelOnlineTime());

		if(GetSrcZoneId() == 0)
		{
			SetLocalSavebackTime(CURRENT_TIME.MSec());
			cmd->PutData("localsavetime", GetLocalSavebackTime());
		}
	}
	CLRecordClient::Instance()->SendCommand(cmd);
}

void Player::ClearInvalidData()
{
	Creature::ClearInvalidData();
}

void Player::SendItemUdpLog(const char* reason, UInt64 itemId, UInt32 itemDataId, ItemType itemType, ItemSubType itemSubType, std::string itemName, UInt32 itemQuality, Int32 number, UInt32 oldNumber)
{
	if (reason == NULL)
	{
		return;
	}
	SendUdpLog("item", LOG_ITEM_FORMAT, reason, itemId, itemDataId, itemType, itemSubType, itemName.c_str(), number, oldNumber, oldNumber + number, itemQuality);
}

void Player::SendEquipLoginUdpLog(UInt64 itemId, UInt32 itemDataId, ItemType itemType, ItemSubType itemSubType, ItemQuality quality, UInt32 strengthen, UInt32 qualityLevel, std::string itemName
	,UInt32 preciousId, ItemQuality preciousQuality, UInt32 cardId, ItemQuality cardQuality, UInt32 slot)
{
	SendUdpLog("equip_login", LOG_EQUIP_LOGIN_FORMAT, itemId, itemDataId, 1, itemType, itemSubType, quality, strengthen, qualityLevel,
		itemName.c_str(), preciousId, preciousQuality, cardId, cardQuality, slot);
}

void Player::SendItemLoginUdpLog(UInt64 itemId, UInt32 itemDataId, UInt32 itemNum, ItemType itemType, ItemSubType itemSubType, ItemQuality quality, UInt32 strengthen, UInt32 qualityLevel, std::string itemName
	, UInt32 preciousId, ItemQuality preciousQuality, UInt32 cardId, ItemQuality cardQuality, UInt32 slot)
{
	SendUdpLog("item_login", LOG_EQUIP_LOGIN_FORMAT, itemId, itemDataId, itemNum, itemType, itemSubType, quality, strengthen, qualityLevel,
		itemName.c_str(), preciousId, preciousQuality, cardId, cardQuality, slot);
}

void Player::SendOpenJarUdpLog(UInt32 jarId, UInt32 combo, ItemDataEntry* itemData, UInt32 itemNum)
{
	if (itemData == NULL) return;
	SendUdpLog("open_jar", LOG_OPEN_JAR_FORMAT, jarId, combo, itemData->id, itemData->type, itemData->color, itemData->name.c_str(), itemNum);
}

void Player::SendAssetUdpLog(AssetType type, const char* reason, UInt64 oldAsset, Int64 assetNum, Int64 nowAssetNum, UInt64 nowAsset)
{
	SendUdpLog("asset", LOG_ASSET_FORMAT, reason, type, oldAsset, assetNum, nowAssetNum, nowAsset);
}

void Player::SendLoginUdpLog()
{
	SendUdpLog("login", LOG_LOGIN_FORMAT, GetName().c_str(), 0, GetPoint(), GetBindPoint(), GetGold(), GetBindGold(), GetDeathTowerTopFloor(), (UInt32)GetAdventureTeamLevel(), (UInt32)GetAdventureTeamGrade());
}

void Player::SendLogoutUdpLog()
{
	SendUdpLog("logout", LOG_LOGOUT_FORMAT, m_OnlineTime, GetPoint(), GetBindPoint(), GetGold(), GetBindGold(), GetMasterDiscipleType(), GetResistMagic());
}

void Player::SendFunctionUdpLog(FunctionLogType type, UInt32 p1, UInt32 p2)
{
	SendUdpLog("function", LOG_FUNCTION_FORMAT, (UInt32)type, p1, p2);
}

void Player::SendChangeSkillUdpLog(UInt16 skillId, std::string skillName, Int8 changeLevel, UInt8 sourceLevel, UInt8 newLevel)
{
	SendUdpLog("skill_change", LOG_SKILL_CHANGE_FORMAT, skillId, skillName.c_str(), changeLevel, sourceLevel, newLevel);
}

void Player::SendRetinueUnlockUdpLog(UInt64 retinueId, UInt32 dataId)
{
	SendUdpLog("retinue_unlock", LOG_RETINUE_UNLOCK_FORMAT, retinueId, dataId);
}

void Player::SendRetinueUpLevelUdpLog(UInt64 retinueId, UInt32 dataId, UInt8 level)
{
	SendUdpLog("retinue_uplevel", LOG_RETINUE_UPLEVEL_FORMAT, retinueId, dataId, level);
}

void Player::SendRetinueUpStarUdpLog(UInt64 retinueId, UInt32 dataId, UInt8 starLevel)
{
	SendUdpLog("retinue_upstar", LOG_RETINUE_UPLEVEL_FORMAT, retinueId, dataId, starLevel);
}

void Player::SendRetinueChangeSkillUdpLog(UInt64 retinueId, UInt32 dataId, std::string lockSkills, std::string oldSkills, std::string newSkills)
{
	SendUdpLog("retinue_change_skill", LOG_RETINUE_CHANGE_SKILL_FORMAT, retinueId, dataId, lockSkills.c_str(), oldSkills.c_str(), newSkills.c_str());
}

void Player::SendActiveTaskFinishUdpLog(UInt32 activeId, UInt32 taskId)
{
	SendUdpLog("active_finish", LOG_ACTIVE_FINISH_FORMAT, activeId, taskId);
}

void Player::SendShopBuyUdpLog(UInt32 shopId, UInt32 shopItemId, UInt32 num, UInt32 newNum, UInt32 dungeonId)
{
	SendUdpLog("shop_buy", LOG_SHOP_BUY_FORMAT, shopId, shopItemId, num, newNum, dungeonId);
}

void Player::SendShopItemUdpLog(UInt32 shopId, UInt32 shopItemId, UInt32 costNum)
{
	SendUdpLog("shop_item", LOG_SHOP_ITEM_FORMAT, shopId, shopItemId, costNum);
}

void Player::SendMallBuyUdpLog(UInt32 mallItemId, UInt32 num)
{
	SendUdpLog("mall_buy", LOG_MALL_BUY_FORMAT, mallItemId, num);
}

void Player::SendNewBootUdpLog(UInt32 bootId)
{
	SendUdpLog("new_boot", LOG_PLAYER_NEW_BOOT, bootId);
}

void Player::SendStrengthEquipUdpLog(UInt64 itemId, UInt32 itemDataId, UInt32 oldStrength, UInt32 newStrength, UInt32 errorcode, UInt32 newPack, UInt32 useUnbreak, UInt32 isTicket, UInt32 probability)
{
	SendUdpLog("strength_equip", LOG_STRENGTH_EQUIP_FORMAT, itemId, itemDataId, oldStrength, newStrength, errorcode, newPack, useUnbreak, isTicket, probability);
}

void Player::SendFirstClearDeathTowerUdpLog(UInt32 dungeonId, UInt32 floor, UInt32 usedSec)
{
	SendUdpLog("first_clear_death_tower", LOG_FIRST_CLEAR_DEATH_TOWER, dungeonId, floor, usedSec);
}

void Player::SendMatchStartUdpLog(PkType type)
{
	SendUdpLog("match_start", LOG_MATCH_START_FORMAT, type);
}

void Player::SendPvpEndUdpLog(UInt32 targetZoneId, UInt64 targetId, PkType type, PkRaceResult result, UInt32 oldMatchScore, UInt32 newMatchScore, UInt32 oldPkValue, UInt32 newPkValue, UInt32 oldSeasonLevel, UInt32 newSeasonLevel, UInt32 oldSeasonStar, UInt32 newSeasonStar, UInt32 oldSeasonExp, UInt32 newSeasonExp, UInt32 seasonId, UInt32 dungeonId, UInt32 usedTime)
{
	SendUdpLog("pvp_end", LOG_PVP_END_FORMAT, targetZoneId, targetId, type, result, oldMatchScore, newMatchScore, oldPkValue, newPkValue, oldSeasonLevel, newSeasonLevel, oldSeasonStar, newSeasonStar, oldSeasonExp, newSeasonExp, seasonId, dungeonId, usedTime);
}

void Player::SendWelfareCardUdpLog(ItemSubType type)
{
	SendUdpLog("welfare_card", LOG_WELFARE_CARD_FORMAT, (UInt32)type);
}

void Player::SendComposeMagicCardUdpLog(UInt32 card1, UInt32 card2, UInt32 targetCard, UInt32 code)
{
	SendUdpLog("compose_magic_card", LOG_COMPOSE_MAGIC_CARD_FORMAT, card1, card2, targetCard, code);
}

void Player::SendFashionComposeUdpLog(UInt32 fashion1, UInt32 fashion2, UInt32 commonFashionId, UInt32 skyFashionId)
{
	SendUdpLog("compose_fashion", LOG_COMPOSE_FASHION_FORMAT, fashion1, fashion2, commonFashionId, skyFashionId);
}

void Player::SendReMakeEquipQlvUdpLog(UInt64 itemId, UInt32 itemDataId, UInt8 oldLv, UInt8 nowLv, UInt32 oldMatNum, UInt32 newMatNum, UInt32 oldPoint, UInt32 newPoint, UInt8 isFrenzy, UInt8 eqColor, UInt8 eqPost)
{
	SendUdpLog("remake_equip_level", LOG_REMAKE_EQUIP_LEVEL_FORMAT, itemId, itemDataId, oldLv, nowLv, oldMatNum, newMatNum, oldPoint, newPoint, isFrenzy, eqColor, eqPost);
}

void Player::SendPayEndUdpLog(std::string orderId, UInt8 mallType, UInt32 chargeGoodsId, UInt32 chargeItemId, UInt32 chargeItemNum, UInt32 vipScore, UInt32 chargeMoney, UInt32 time, UInt32 sendGoodTimeoutTime)
{
	SendUdpLog("pay_end", LOG_PAY_END_FORMAT, orderId.c_str(), mallType, chargeGoodsId, chargeItemId, chargeItemNum, vipScore, chargeMoney, time, sendGoodTimeoutTime);
}

void Player::SendChangeOccuUdpLog(UInt8 mainOccu, UInt8 preOccu, UInt8 occu)
{
	SendUdpLog("change_occu", LOG_CHANGE_OCCU_FORMAT, mainOccu, preOccu, occu);
}

void Player::SendPetLoginUdpLog(UInt64 petId, UInt32 dataId, UInt32 level, UInt32 exp, PetType petType, UInt32 slot)
{
	SendUdpLog("pet_login", LOG_PET_LOGIN_FORMAT, petId, dataId, level, exp, petType, slot);
}

void Player::SendPetAddUdpLog(UInt64 petId, UInt32 dataId, UInt32 itemId, PetType petType, PetQuality petQuality)
{
	SendUdpLog("pet_add", LOG_PET_ADD_FORMAT, petId, dataId, itemId, petType, petQuality);
}

void Player::SendPetBattleUdpLog(UInt64 petId, UInt32 dataId, UInt64 oldPetId, UInt32 oldDataId)
{
	SendUdpLog("pet_battle", LOG_PET_BATTLE_FORMAT, petId, dataId, oldPetId, oldDataId);
}

void Player::SendPetDevourUdpLog(UInt64 petId, UInt32 dataId, UInt16 oldLevel, UInt16 newLevel, UInt32 oldExp, UInt32 newExp, UInt32 exp, UInt64 devourId, UInt32 devourDataId, UInt16 devourLevel, UInt32 devourExp)
{
	SendUdpLog("pet_devour", LOG_PET_DEVOUR_FORMAT, petId, dataId, oldLevel, newLevel, oldExp, newExp, exp, devourId, devourDataId, devourLevel, devourExp);
}

void Player::SendPetChangeSkillUdpLog(UInt64 petId, UInt32 dataId, UInt8 oldIndex, UInt8 newIndex)
{
	SendUdpLog("pet_change_skill", LOG_PET_CHANGE_SKILL_FORMAT, petId, dataId, oldIndex, newIndex);
}

void Player::SendLiveVoiceDataUdpLog(UInt8 subType, const std::string& roomId, const std::string& time)
{
	SendUdpLog("live_voice_data", LOG_LIVE_VOICE_DATA_FORMAT, subType, roomId.c_str(), time.c_str());
}

void Player::SendChatVoiceDataUdpLog(UInt8 subType, const std::string& voiceId)
{
	SendUdpLog("chat_voice_data", LOG_CHAT_VOICE_DATA_FORMAT, subType, voiceId.c_str());
}

void Player::SendDiscipleCompDailyTaskLog(ObjID_t discipleId, ObjID_t masterId, UInt32 timestamp)
{
	SendUdpLog("mastersys_comp_dailytask", LOG_MASTERSYS_COMP_DAILYTASK, discipleId, masterId, timestamp);
}

void Player::SendPreciousBeadUdpLog(PreciousBeadOpType opType, ObjID_t itemObjectId, UInt32 itemId, UInt32 srcBeadId, UInt32 newBeadId, UInt32 costItemId, UInt32 costItemNum, UInt32 isSuccess, UInt32 param1, UInt32 param2 )
{
	SendUdpLog("precious_bead", LOG_PRECIOUS_BEAD, opType, itemObjectId, itemId, srcBeadId, newBeadId, costItemId, costItemNum, isSuccess, param1, param2);
}

void Player::SendExtractArtifactJarDiscountUdpLog(UInt32 opActId, UInt32 rate, UInt32 times)
{
	SendUdpLog("extract_artifact_jar_discount", LOG_EXTRACT_ARTIFACT_JAR_DISCOUNT, opActId, rate, times);
}

void Player::SendChiJiBalanceUdpLog(UInt32 rank, UInt32 survivalTime, UInt32 reason)
{
	SendUdpLog("chiji_balance", LOG_CHIJI_BALANCE, rank, survivalTime, reason);
}

void Player::IncLevel()
{
	UInt32 upExp = m_pLevelDataEntry->upgradeexp;
	if (upExp <= GetExp()) return;

	m_Exp = upExp;
	Levelup();

	//SortListMgr::Instance()->OnSortValueChanged(SORTLIST_LEVEL, GetID(), GetName(),0, 0, 0, GetLevel(), m_Exp); 
}

void Player::SyncRequest(UInt8 type, Player* pRequester, const std::string& param)
{
	if(pRequester == NULL) return;

	CLProtocol::SceneSyncRequest sync;
	sync.type = type;
	sync.requester = pRequester->GetID();
	sync.requesterName = pRequester->GetName();
	sync.requesterOccu = pRequester->GetOccu();
	sync.requesterLevel = pRequester->GetLevel();
	sync.param1 = param;
	sync.vipLv = pRequester->GetVipLvl();
	SendProtocol(sync);
}

void Player::SyncBlessTimes()
{
	CLProtocol::SceneRelationSyncBlessTimes sync;
	sync.times = GetCounter("bless_times");
	SendProtocol(sync);
}

void Player::SendFriendPresentInfo()
{
	CLProtocol::SceneRelationPresentInfoRet ret;
	ret.continuousDays = GetContinuousOnlineDays();
	UInt32 flag = GetCounter("present_flag");
	if(flag & RELATION_PRESENT_GIVE) ret.giveState1 = 1;
	if(flag & RELATION_PRESENT_DRAW) ret.drawState1 = 1;
	if(flag & RELATION_PRESENT_GIVE15) ret.giveState2 = 1;
	if(flag & RELATION_PRESENT_DRAW15) ret.drawState2 = 1;
	SendProtocol(ret);
}

bool Player::FriendGiveGift(const std::string& friendName, UInt32 itemId)
{
	if(GetCounter("present_times") >= MAX_RECEIVE_FRIENDGIFT_TIMES) return false;

	char titlebuf[128];
	SysNotifyMgr::Instance()->MakeNotify(623, titlebuf, 128);
	char contentbuf[256];
	SysNotifyMgr::Instance()->MakeNotify(624, contentbuf, 256, friendName.c_str());

	RewardGroup* pGroup = GetRewardGroup();
	pGroup->AddReward(itemId, 1, 1, 0);
	//SendSysMail(titlebuf, contentbuf, 0, pGroup);
	IncCounter("present_times", 1);

	return true;
}

void Player::OnGiveFriendGiftRet(UInt32 gamestartDay, UInt8 type, UInt8 result)
{
	if(result == 0)
	{
		SendNotify(625);
	}
	else
	{
		UInt32 presentFlag = 0;
		if(type == 1) presentFlag = RELATION_PRESENT_GIVE;
		else presentFlag = RELATION_PRESENT_GIVE15;

		//失败处理
		UInt32 days = GetGameStartDays();
		if(gamestartDay == days)
		{
			UInt32 flag = GetCounter("present_flag");
			flag &= ~presentFlag;
			SetCounter("present_flag", flag);
		}

		if(result == 1) SendNotify(626);
		else SendNotify(627, MAX_RECEIVE_FRIENDGIFT_TIMES);
	}

	SendFriendPresentInfo();
}

void Player::UpdateFuncNotify()
{
	class FuncNotifyVisitor : public Avalon::DataEntryVisitor<FuncNotifyDataEntry>
	{
	public:
		FuncNotifyVisitor(Player* player, UInt8 oldFunc):m_pPlayer(player),m_OldFuncId(oldFunc),m_FuncId(0){}

		bool operator()(FuncNotifyDataEntry* dataEntry)
		{
			if(m_pPlayer->CheckFuncFlag(dataEntry->id)) return true;

			bool bBegin = false;
			if(m_OldFuncId == dataEntry->id) bBegin = true;
			else
			{
				if(dataEntry->beginTask != 0)
				{
					if(m_pPlayer->CheckTaskFinished(dataEntry->beginTask) 
						|| m_pPlayer->FindTask(dataEntry->beginTask) != NULL) bBegin = true;
				}
				if(!bBegin && dataEntry->beginLevel != 0)
				{
					if(m_pPlayer->GetLevel() >= dataEntry->beginLevel) bBegin = true;
				}
			}

			bool bEnd = false;
			if(dataEntry->endTask != 0)
			{
				if(m_pPlayer->CheckTaskFinished(dataEntry->endTask)) bEnd = true;
			}
			if(!bEnd && dataEntry->endLevel != 0)
			{
				if(m_pPlayer->GetLevel() >= dataEntry->endLevel) bEnd = true;
			}
			
			if((bBegin && !bEnd) && dataEntry->type == UnlockType::UNLOCK_FUNCTION)
			{
				m_FuncId = dataEntry->id;
				return true;
			}

			if (bEnd)
			{
				//给奖励
				if (!dataEntry->rewards.empty())
				{
					std::vector<ItemReward> itemReward;
					if (dataEntry->GetRewards(m_pPlayer->GetOccupation(), itemReward))
					{
						RewardGroup* rewardGroup = m_pPlayer->GetRewardGroup();
						rewardGroup->AddRewards(itemReward);
						m_pPlayer->AddRewards(GetReason(LogSourceType::SOURCE_TYPE_FUNCTION_UNLOCK, dataEntry->id).c_str(), rewardGroup);
					}
				}

				m_pPlayer->SetFuncFlag(dataEntry->id);

				//added by huchenhui
				if (dataEntry->endLevel > 1)
				{
					CLProtocol::SceneSyncFuncUnlock protocol;
					protocol.funcId = dataEntry->id;
					m_pPlayer->SendProtocol(protocol);
				}
			}

			return true;
		}

		UInt8 GetFuncId() const{ return m_FuncId; }

	private:
		Player*	m_pPlayer;
		UInt8	m_OldFuncId;
		UInt8	m_FuncId;
	};

	FuncNotifyVisitor visitor(this, m_FuncNotify);
	FuncNotifyDataEntryMgr::Instance()->Visit(visitor);

	m_FuncNotify = visitor.GetFuncId();
}

void Player::OpenFunction(UInt32 id)
{
	FuncNotifyDataEntry* dataEntry = FuncNotifyDataEntryMgr::Instance()->FindEntry(id);
	if (dataEntry == NULL) return;
	if (CheckFuncFlag(dataEntry->id)) return;

	//给奖励
	if (!dataEntry->rewards.empty())
	{
		std::vector<ItemReward> itemReward;
		if (dataEntry->GetRewards(GetOccupation(), itemReward))
		{
			RewardGroup* rewardGroup = GetRewardGroup();
			rewardGroup->AddRewards(itemReward);
			AddRewards(GetReason(LogSourceType::SOURCE_TYPE_FUNCTION_UNLOCK, dataEntry->id).c_str(), rewardGroup);
		}
	}

	SetFuncFlag(dataEntry->id);

	if (dataEntry->endLevel > 1)
	{
		CLProtocol::SceneSyncFuncUnlock protocol;
		protocol.funcId = dataEntry->id;
		SendProtocol(protocol);
	}
}

bool Player::StartStory(UInt32 id)
{
	

	return true;
}

void Player::StopStory(UInt32 id)
{
	
}

void Player::SetBootFlag(UInt32 id)
{
	if (!m_BootFlag.CheckMask(id))
	{
		m_BootFlag.SetMask(id);
	}
}

void Player::ResetBootFlag(UInt32 id)
{
	if (m_BootFlag.CheckMask(id))
	{
		m_BootFlag.UnsetMask(id);
	}
	else
	{
		m_BootFlag.SetMask(id);
	}
}

void Player::CancelStory()
{
	m_StoryId = 0;
}

void Player::SetNewBoot(UInt32 id)
{
	m_NewBoot = id;
	SendNewBootUdpLog(id);
}

UInt32 Player::GetNewBoot()
{
	return m_NewBoot.GetDBValue();
}

void Player::CheckNewBoot()
{
	if (m_NewBoot == 0) m_NewBoot = NEW_BOOT_FIRST_BATTLE_ID;
}

void Player::ResetSeasonPlayStatus()
{
	m_SeasonLevel.ResetSeasonPlayStatus();
}

UInt32 Player::AddRetinueByItemId(UInt32 itemId)
{
	RetinueDataEntry* dataEntry = RetinueDataEntryMgr::Instance()->GetRetinueDataEntry(itemId);
	if (dataEntry == NULL) return ErrorCode::RETINUE_DATA_NOT_EXIST;
	return AddRetinue(dataEntry->id);
}

UInt32 Player::CheckAddRetinueByItemId(UInt32 itemId)
{
	return m_RetinueMgr.CheckAddRetinueByItemId(itemId);
}

void Player::_OnCharge(const char* reason, UInt32 vipExp, UInt32 chargeNum)
{
	AddVipExp(reason, vipExp);

	// 添加总充值金额
	AddTotalChargeNum(chargeNum);

	//添加玩家总充值金额
	AddPlayerChargeTotal(chargeNum);

	m_AbnormalTransactionMgr.OnCharge(chargeNum);

 	ChargeEventParam param;
 	param._chargeNum = chargeNum;
 	CLEventMgr::Instance()->SendPlayerEvent(GetID(), CLEventMgr::ET_CHARGE_RMB, &param);
}

void Player::OnChargeMakeup(const char* reason, UInt32 vipExp, UInt32 chargeNum)
{
	AddVipExp(reason, vipExp);

	// 添加总充值金额
	AddTotalChargeNum(chargeNum);

	//添加玩家总充值金额
	AddPlayerChargeTotal(chargeNum);

	ChargeEventParam param;
	param._chargeNum = chargeNum;
	CLEventMgr::Instance()->SendPlayerEvent(GetID(), CLEventMgr::ET_CHARGE_RMB, &param);
}

bool Player::CheckLogicDayChange(const Avalon::Time& now, bool isOnline)
{
	if (SSServiceSwitchMgr::Instance()->IsOpen(ServiceType::NEW_DAY_CACL))
	{
		return _NewCheckLogicDayChange(now);
	}
	else
	{
		if (isOnline)
		{
			return CheckLogicDayChangeOld();
		}
		else
		{
			return _OldCheckLogicDayChange();
		}
		
	}
}

bool Player::_NewCheckLogicDayChange(const Avalon::Time& now)
{
	UInt32 lastTime = GetCounter(LAST_LGC_DAYCHANGE_TIME);

	if (0 == lastTime)
	{
		SetCounter(LAST_LGC_DAYCHANGE_TIME, (UInt32)now.Sec());

		// 老玩家
		if (!(GetSavebackTime() == 0 && GetLevel() == 1))
		{
			return CheckLogicDayChangeOld();
		}
	}

	Avalon::Time time((UInt64)lastTime * 1000);
	UInt32 checkTime = (UInt32)(GetDayBeginTime2(time) / 1000);
	if (checkTime + DAY_TO_SEC <= (UInt32)now.Sec())
	{
		SetCounter(LAST_LGC_DAYCHANGE_TIME, (UInt32)now.Sec());
		return true;
	}

	return false;
}

bool Player::_OldCheckLogicDayChange()
{
	if (!m_bLogicDayChanged)
	{
		if (CURRENT_DATE.Hour() == LOGIC_DAYCHANGE_HOUR && CURRENT_DATE.Min() == 0 && CURRENT_DATE.Sec() < 5)
		{
			m_bLogicDayChanged = true;
			return true;
		}
	}
	else
	{
		if (CURRENT_DATE.Hour() == LOGIC_DAYCHANGE_HOUR && CURRENT_DATE.Min() > 0)
		{
			m_bLogicDayChanged = false;
		}
	}

	return false;
}

bool Player::CheckLogicDayChangeOld()
{
	Avalon::Time offlineTime(GetSavebackTime());
	UInt64 offlineDay = GetDayBeginTime2(offlineTime);
	UInt64 currentDay = GetDayBeginTime2(CURRENT_TIME);
	bool bDayChanged = (GetSavebackTime() != 0 && (offlineDay != currentDay));

	return bDayChanged;
}

bool Player::CheckDayChange(const Avalon::Time& now)
{
	if (SSServiceSwitchMgr::Instance()->IsOpen(ServiceType::NEW_DAY_CACL))
	{
		return _NewCheckDayChange(now);
	}
	else
	{
		return _OldCheckDayChange();
	}
}

bool Player::CheckDayChangeOld()
{
	Avalon::Time offlineTime(GetSavebackTime());
	UInt64 offlineDay = GetDayBeginTime(offlineTime);
	UInt64 currentDay = GetDayBeginTime(CURRENT_TIME);
	bool bDayChanged = (GetSavebackTime() != 0 && (offlineDay != currentDay));

	return bDayChanged;
}

bool Player::_NewCheckDayChange(const Avalon::Time& now)
{
	UInt32 lastTime = GetCounter(LAST_DAYCHANGE_TIME);

	if (0 == lastTime)
	{
		SetCounter(LAST_DAYCHANGE_TIME, (UInt32)now.Sec());

		// 老玩家
		if (!(GetSavebackTime() == 0 && GetLevel() == 1))
		{
			return CheckDayChangeOld();
		}
	}

	Avalon::Time time((UInt64)lastTime * 1000);
	UInt32 checkTime = (UInt32)(GetDayBeginTime(time) / 1000);
	if (checkTime + DAY_TO_SEC <= (UInt32)now.Sec())
	{
		SetCounter(LAST_DAYCHANGE_TIME, (UInt32)now.Sec());
		return true;
	}

	return false;
}

bool Player::_OldCheckDayChange()
{
	if (m_bDayChanged == 0)
	{
		if (CURRENT_DATE.Hour() == 0 && CURRENT_DATE.Min() == 0 && CURRENT_DATE.Sec() < 5)
		{
			m_bDayChanged = 1;
			return true;
		}
	}
	else
	{
		if (CURRENT_DATE.Hour() == 0 && CURRENT_DATE.Min() > 0)
		{
			m_bDayChanged = 0;
		}
	}

	return false;
}

bool Player::_CheckLogicWeekChange(const Avalon::Time& now)
{
	UInt32 refreshTime = GetCounter(NEXT_LGC_WEEKCHANGE_TIME);

	if (refreshTime > (UInt32)now.Sec())
	{
		return false;
	}

	TimeUtil::RefreshDate refreshDate;
	refreshDate.weekDay = 1;
	refreshDate.hour = 6;
	UInt64 nextRefreshTimestamp = 0;
	if (!TimeUtil::CalculateNextRefreshTimestamp(TimeUtil::REFRESH_TYPE_PER_WEEK, refreshDate, nextRefreshTimestamp))
	{
		return false;
	}

	UInt32 nextRefreshTimestamp32 = (UInt32)(nextRefreshTimestamp / SEC_TO_MSEC);

	if (refreshTime == nextRefreshTimestamp32)
	{
		return false;
	}

	SetCounter(NEXT_LGC_WEEKCHANGE_TIME, nextRefreshTimestamp32);

	InfoLogStream << PLAYERINFO(this) << " set next logic week change time(" << TimeUtil::TimestampToStandardFormat(nextRefreshTimestamp32)
		<< "), last time(" << TimeUtil::TimestampToStandardFormat(refreshTime) << ") ." << LogStream::eos;

	// 第一次设置
	if (refreshTime == 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void Player::OnGetMallItem(IEventParam* param)
{
	if (!param)
	{
		return;
	}

	GetNewItemParam* myParam = (GetNewItemParam*)param;
	m_MallItemId = myParam->itemId;
}

bool Player::IsReadedNotify(UInt32 type, UInt32 param)
{
	std::ostringstream os;
	os << READ_NOTIFY << type << "_" << param;
	const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(os.str());
	if (!counterConfig)
	{
		CounterCfg::Instance()->RegCounter(os.str().c_str(), COUNTER_CYCLE_NONE);
	}

	return (GetCounter(os.str().c_str()) > 0) ? true : false;
}

void Player::ReadNotify(UInt32 type, UInt64 param)
{
	if (type >= NT_MAX)
	{
		return;
	}

	if (type == NT_TIME_ITEM)
	{
		Item* item = FindItem(param);
		if (!item)	return;
		
		item->SetNotifyTimeState(false);
	}
	else
	{
	std::ostringstream os;
	os << READ_NOTIFY << type << "_" << param;
	const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(os.str());
	if (!counterConfig)
	{
		CounterCfg::Instance()->RegCounter(os.str().c_str(), COUNTER_CYCLE_NONE);
	}

	SetCounter(os.str().c_str(), 1);
}

}

void Player::ClearReadNotify(UInt32 type, UInt32 param)
{
	std::ostringstream os;
	os << READ_NOTIFY << type << "_" << param;
	const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(os.str());
	if (!counterConfig)
	{
		CounterCfg::Instance()->RegCounter(os.str().c_str(), COUNTER_CYCLE_NONE);
	}

	SetCounter(os.str().c_str(), 0);
}

void Player::SendNotifyTimeItemState(std::vector<Item*>& items)
{
	std::vector<TimeItem> timeItems;
	for (size_t i = 0; i < items.size(); ++i)
	{
		if (!items[i])
		{
			continue;
		}

		TimeItem timeItem;
		timeItem.itemUid = items[i]->GetID();
		timeItem.state = (UInt32)items[i]->GetTimeState();
		timeItems.push_back(timeItem);
	}
	
	if (timeItems.empty())
	{
		return;
	}

	CLProtocol::SCNotifyTimeItem protocol;
	protocol.items = timeItems;

	SendProtocol(protocol);
}

#ifdef _DEBUG

void Player::SyncDebugChat(std::string word)
{
	CLProtocol::SceneSyncChat sync;
	sync.channel = CHAT_CHANNEL_PRIVATE;
	sync.objid = 0;
	sync.occu = 0;
	sync.level = 0;
	sync.viplvl = 0;
	sync.objname = "SYSTEM";
	sync.receiverId = GetID();
	sync.bLink = false;
	sync.isGm = true;
	sync.word = word;
	sync.voiceKey = "";
	sync.voiceDuration = 0;
	sync.headFrame = GetHeadFrameId();

	SendProtocol(sync);
}
#endif // _DEBUG

void Player::CountAvatar()
{
	PlayerAvatar avatar;

	class FashionEquipsVisitor : public ItemVisitor
	{
	public:
		FashionEquipsVisitor(Player* player, PlayerAvatar& avatar) :m_avatar(avatar) 
		{
			if (!player)
			{
				return;
			}

			auto oldAvatar = player->GetAvatar();
			for (auto equipId : oldAvatar.equipItemIDs)
			{
				auto data = ItemDataEntryMgr::Instance()->FindEntry(equipId);
				if (!data || data->isTransparentFashion)
				{
					continue;
				}

				m_oldEquipIdBySubType[data->subType] = equipId;
			}
		}

		bool operator()(Item* item)
		{
			if (!item || !item->GetDataEntry())
			{
				return true;
			}

			if (!item->GetDataEntry()->isTransparentFashion)
			{
				m_avatar.equipItemIDs.push_back(item->GetDataID());
			}
			else
			{
				auto subType = item->GetDataEntry()->subType;
				if (m_oldEquipIdBySubType.find(subType) != m_oldEquipIdBySubType.end())
				{
					m_avatar.equipItemIDs.push_back(m_oldEquipIdBySubType[subType]);
				}
			}
			

			return true;
		}

	private:
		std::map<UInt32, UInt32> m_oldEquipIdBySubType;
		PlayerAvatar& m_avatar;
	};

	// 先获取所有时装
	FashionEquipsVisitor vistor(this, avatar);
	m_ItemMgr.GetPackage(PACK_FASHION_WEAR)->VisitItems(vistor);

	// 吃鸡场景服务器，所有装备都要同步
	if (SERVER_TYPE == ST_BSCENE)
	{
		class EquipsVisitor : public ItemVisitor
		{
		public:
			EquipsVisitor(Player* player, PlayerAvatar& avatar) : m_player(player), m_avatar(avatar)
			{
				
			}

			bool operator()(Item* item)
			{
				if (!item || !item->GetDataEntry())
				{
					return true;
				}

				if (item->GetDataEntry()->subType == ST_EQUIP_WEAPON)
				{
					m_avatar.weaponStrengthen = item->GetStrengthen();

					//DebugLogStream << "player(" << PLAYERINFO(m_player) << ") avatar add weapon(" << item->GetDataID() << ", " << item->GetStrengthen() << ")." << LogStream::eos;
				}

				//DebugLogStream << "player(" << PLAYERINFO(m_player) << ") avatar add equip(" << item->GetDataID() << ")." << LogStream::eos;

				m_avatar.equipItemIDs.push_back(item->GetDataID());
				return true;
			}

		private:
			Player* m_player;
			PlayerAvatar& m_avatar;
		};

		EquipsVisitor vistor(this, avatar);
		m_ItemMgr.GetPackage(PACK_WEAR)->VisitItems(vistor);
	}
	else
	{
		auto weapon = GetEquip(ST_EQUIP_WEAPON);
		if (weapon)
		{
			avatar.equipItemIDs.push_back(weapon->GetDataID());
			avatar.weaponStrengthen = weapon->GetStrengthen();
		}
	}

	avatar.isShoWeapon = this->GetShowFashionWeapon();
	m_avatar = avatar;

	if (GetTeam())
	{
		SyncAvatarToTeam();
	}

	SyncAvatarToWorld();
}

bool Player::CheckMallEx( UInt32 num )
{
	if(GetItemNum(PANIC_BUY_CARD) < num) return false;
	return true;
}

void Player::RemoveMallEx( UInt32 num )
{
	RemoveItem("panicex", PANIC_BUY_CARD, num);
}

bool Player::IsMallPanicBuyId( UInt32 mallId )
{
	PlayerScript* pScript = ScriptMgr::Instance()->GetPlayerScript();
	if(NULL == pScript) return false;
	return pScript->IsMallPanicBuyId(mallId);
}

UInt32 Player::OnPlayerRefreshAuction()
{
	DebugLogStream << "player(" << GetID() << ") request to refresh auction." << LogStream::eos;

	UInt32 curTime = (UInt32)CURRENT_TIME.Sec();
	UInt32 intervalSec = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_AUTO_REFRESH_SEC);
	if (m_AuctionRefreshTime + intervalSec > curTime)
	{
		auto costData = CurrencyCostDataEntryMgr::Instance()->FindEntry(1);
		if (!costData)
		{
			ErrorLogStream << "can't find currency cost data." << LogStream::eos;
			return ErrorCode::AUCTION_REFRESH_ERROR;
		}

		// 安全锁开启，不允许消耗点券，刷新拍卖行时间
		if (costData->itemId == ITEM_POINT && !SecurityLockCheck())
		{
			InfoLogStream << "player(" << GetID() << ") request to refresh auction security lock not allow." << LogStream::eos;
			return ErrorCode::SECURITY_LOCK_FORBID_OP;
		}

		// 判断钱够不够
		if (GetItemNum(costData->itemId) < costData->num)
		{
			return ErrorCode::AUCTION_REFRESH_NO_ENOUGH_MONEY;
		}

		RemoveItem(GetReason(SOURCE_TYPE_AUCTION_REFRESH).c_str(), costData->itemId, costData->num);
	}

	SetAuctionRefreshTime(curTime);
	SyncBaseInfoToWorld();

	return ErrorCode::SUCCESS;
}

UInt32 Player::OnPlayerBuyAuctionBooth()
{
	DebugLogStream << "player(" << GetID() << ") request to buy auction booth." << LogStream::eos;

	UInt32 boothId = m_AuctionAdditionBooth + 1;
	auto data = AuctionBoothDataEntryMgr::Instance()->FindEntry(boothId);
	if (!data)
	{
		// 说明已经买满了
		InfoLogStream << "player(" << GetID() << ") request to buy auction booth is full." << LogStream::eos;
		return ErrorCode::AUCTION_BOOTH_FULL;
	}

	// 安全锁开启，不允许消耗点券，购买拍卖行栏位
	if (data->costItemId == ITEM_POINT && !SecurityLockCheck())
	{
		InfoLogStream << "player(" << GetID() << ") request to buy auction security lock not allow." << LogStream::eos;
		return ErrorCode::SECURITY_LOCK_FORBID_OP;
	}

	if (GetItemNum(data->costItemId) < data->num)
	{
		InfoLogStream << "player(" << GetID() << ") request to buy auction booth, no enough money." << LogStream::eos;
		return ErrorCode::AUCTION_BUY_BOOTH_NO_ENOUGH_MONEY;
	}

	RemoveItem(GetReason(SOURCE_TYPE_AUCTION_BUY_BOOTH, boothId).c_str(), data->costItemId, data->num);
	SetAuctionAdditionBooth(boothId);
	SyncBaseInfoToWorld();

	return ErrorCode::SUCCESS;
}

void Player::ChangeZone(UInt32 zoneId, UInt32 mapId)
{
	if(zoneId == ZONE_ID) return;
	if(SERVER_TYPE == ST_WSCENE) return;
	if(GetSrcZoneId() != 0) return;	//已经在跨服

	//TODO 检查能否跨服

	CLProtocol::GateChangeZoneNotify notify;
	notify.playerId = GetID();
	notify.dstZoneId = zoneId;
	notify.dstMapId = mapId;
	SendToGate(notify);
}

void Player::ReturnZone()
{
	if(m_TourZoneId == 0 || m_TourZoneId != ZONE_ID) return;

	CLProtocol::GateChangeZoneNotify notify;
	notify.playerId = GetID();
	notify.dstZoneId = m_SrcZoneId;
	notify.dstMapId = MAJORCITY_MAPID;
	SendToGate(notify);
}

void Player::AddReviveCoin(const char* reason, UInt32 num)
{
	if (reason == NULL) return;
	UInt32 oldReviveCoin = m_reviveCoin;
	if (m_reviveCoin + num > MAX_MONEY) m_reviveCoin = MAX_MONEY;
	else m_reviveCoin = m_reviveCoin + num;

	if (m_reviveCoin > oldReviveCoin)
	{
		GameInfoLogStream("Property") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") addReviveCoin:"
			<< reason << "|" << m_reviveCoin - oldReviveCoin << "|" << m_reviveCoin << LogStream::eos;

		SendAssetUdpLog(AssetType::ASSET_REVIVE_COIN, reason, (UInt64)oldReviveCoin, (Int64)num, (Int64)m_reviveCoin - (Int64)oldReviveCoin, (UInt64)m_reviveCoin);
	}
}

void Player::RemoveReviveCoin(const char* reason, UInt32 num)
{
	if (reason == NULL) return;
	UInt32 oldValue = m_reviveCoin;
	if (num >= m_reviveCoin) m_reviveCoin = 0;
	else m_reviveCoin = m_reviveCoin - num;

	if (oldValue > m_reviveCoin)
	{
		GameInfoLogStream("Property") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") remove_revive_coin:"
			<< reason << "|" << oldValue - m_reviveCoin << "|" << m_reviveCoin << LogStream::eos;

		OnChangeMoney(MoneyType::MONEY_REVIVE_COIN, num * -1);
		SendAssetUdpLog(AssetType::ASSET_REVIVE_COIN, reason, (UInt64)oldValue, (Int64)num * -1, (Int64)m_reviveCoin - (Int64)oldValue, (UInt64)m_reviveCoin);
	}
}

void Player::AddSP(const char* reason, UInt32 num)
{
	AddPveSP(reason, num);
	AddPvpSP(reason, num);
}

void Player::AddPveSP(const char* reason, UInt32 num)
{
	if (reason == NULL) return;
	UInt32 oldSp = m_SP;
	if (m_SP + num > MAX_SP) m_SP = MAX_SP;
	else m_SP = m_SP + num;

	if (m_SP > oldSp)
	{
		GameInfoLogStream("Property") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") spType:" <<
			AssetType::ASSET_SP << " addSP:" << reason << "|" << m_SP - oldSp << "|" << m_SP << LogStream::eos;

		SendAssetUdpLog(AssetType::ASSET_SP, reason, (UInt64)oldSp, (Int64)num, (Int64)m_SP - (Int64)oldSp, (UInt64)m_SP);
	}
}

void Player::AddPvpSP(const char* reason, UInt32 num)
{
	if (reason == NULL) return;
	UInt32 oldSp = m_PvpSP;
	if (m_PvpSP + num > MAX_SP) m_PvpSP = MAX_SP;
	else m_PvpSP = m_PvpSP + num;

	if (m_PvpSP > oldSp)
	{
		GameInfoLogStream("Property") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") spType:" <<
			AssetType::ASSET_PVP_SP << " addSP:" << reason << "|" << m_PvpSP - oldSp << "|" << m_PvpSP << LogStream::eos;

		SendAssetUdpLog(AssetType::ASSET_PVP_SP, reason, (UInt64)oldSp, (Int64)num, (Int64)m_PvpSP - (Int64)oldSp, (UInt64)m_PvpSP);
	}
}

bool Player::RemovePveSP(const char* reason, UInt32 num)
{
	if (reason == NULL) return false;
	UInt32 oldSP = m_SP;
	if (num > m_SP) return false;
	else m_SP = m_SP - num;

	if (oldSP > m_SP)
	{
		GameInfoLogStream("Property") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") spType:" 
			<< AssetType::ASSET_SP << " removeSP:" << reason << "|" << oldSP - m_SP << "|" << m_SP << LogStream::eos;

		SendAssetUdpLog(AssetType::ASSET_SP, reason, (UInt64)oldSP, (Int64)num * -1, (Int64)m_SP - (Int64)oldSP, (UInt64)m_SP);
	}
	return true;
}

bool Player::RemovePvpSP(const char* reason, UInt32 num)
{
	if (reason == NULL) return false;
	UInt32 oldSP = m_PvpSP;
	if (num > m_PvpSP) return false;
	else m_PvpSP = m_PvpSP - num;

	if (oldSP > m_PvpSP)
	{
		GameInfoLogStream("Property") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") spType:"
			<< AssetType::ASSET_PVP_SP << " removeSP:" << reason << "|" << oldSP - m_PvpSP << "|" << m_PvpSP << LogStream::eos;

		SendAssetUdpLog(AssetType::ASSET_PVP_SP, reason, (UInt64)oldSP, (Int64)num * -1, (Int64)m_PvpSP - (Int64)oldSP, (UInt64)m_PvpSP);
	}
	return true;
}

UInt32 Player::GetAllSP()
{
	//1.升级获得
	//2.转职任务获得
	//3.觉醒获得

	UInt32 levelSp = PlayerDataEntryMgr::Instance()->GetSumSP(GetLevel());

	InfoLogStream << PLAYERINFO(this) << "GetAllSP level sp = " << levelSp << LogStream::eos;
	
	UInt32 occuSp = 0;
	if (!IsInitOccu())
	{
		occuSp = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_CHANGE_OCCU_SP_NUM);
	}

	InfoLogStream << PLAYERINFO(this) << "GetAllSP change occu sp = " << occuSp << LogStream::eos;

	UInt32 awakenSp = 0;
	if (GetAwaken() != 0)
	{
		awakenSp = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_AWAKEN_SP_NUM);
	}
	InfoLogStream << PLAYERINFO(this) << "GetAllSP awaken sp = " << awakenSp << LogStream::eos;

	UInt32 result = levelSp + occuSp + awakenSp;

	//UInt32 oldSp = GetSkillMgr().GetSkillSP() + GetSP();

	return result;
}

void Player::NotifyLeaveDungeon(bool isDisconnect)
{
	CLProtocol::WorldDungeonLeaveRace protocol;
	protocol.roleId = GetID();
	protocol.isDisconnect = isDisconnect ? 1 : 0;
	Router::SendToBattle(protocol);
}

bool Player::IsDungeonAnyHardPassed(UInt32 dungeonId)
{ 
	return m_dungeonMgr.IsDungeonAnyHardPassed(dungeonId); 
}

bool Player::IsDungeonPassed(UInt32 dungeonId)
{ 
	return m_dungeonMgr.IsDungeonPassed(dungeonId); 
}

void Player::SyncEventToWorld(SyncEventType type, ObjID_t param1)
{
	CLProtocol::SceneSyncEvent protocol;
	protocol.playerId = GetID();
	protocol.type = type;
	protocol.param1 = param1;
	Router::SendToWorld(protocol);
}

void Player::GetPlayerLabelInfo(PlayerLabelInfo& info)
{
	info.awakenStatus = GetAwaken();
	info.returnStatus = IsVeteranReturn() ? 1 : 0;
	info.headFrame = GetHeadFrameId();
}

UInt32 Player::RequestBuyDungeonTimes(DungeonSubType subType)
{
	auto data = DungeonTimesDataEntryMgr::Instance()->FindEntry(subType);
	if (!data)
	{
		DebugLogStream << "player(" << GetID() << ") want buy dungeon(" << (UInt8)subType << ") times, can't find in table." << LogStream::eos;
		return ErrorCode::DUNGEON_TIMES_CANT_BUY;
	}

	// 安全锁开启，不允许消耗点券，购买挑战次数
	if (data->buyTimesCostItemId == ITEM_POINT && !SecurityLockCheck())
	{
		DebugLogStream << "player(" << GetID() << ") want buy dungeon security lock not allow." << LogStream::eos;
		return ErrorCode::SECURITY_LOCK_FORBID_OP;
	}

	// 当前可购买次数
	UInt32 maxBuyTimes = data->baseBuyTimes + GetVipValue(data->buyTimesVipPrivilege);
	// 已经购买了的次数
	UInt32 buyedTimes = GetCount(data->buyTimesCounter.c_str());

	// 先判断次数够不够
	if (buyedTimes >= maxBuyTimes)
	{
		DebugLogStream << "player(" << GetID() << ") want buy dungeon(" << (UInt8)subType << ") times, current buy times(" << buyedTimes << ") no remain times." << LogStream::eos;
		return ErrorCode::DUNGEON_TIMES_NO_REMAIN_TIMES;
	}

	// 获取当前消耗
	if (buyedTimes >= data->buyTimesCosts.size())
	{
		ErrorLogStream << "player(" << GetID() << ") want buy dungeon(" << (UInt8)subType << ") times, current buy times(" << buyedTimes + 1 << ") can't find cost." << LogStream::eos;
		return ErrorCode::DUNGEON_TIMES_NO_REMAIN_TIMES;
	}
	UInt32 cost = data->buyTimesCosts[buyedTimes];

	// 判断钱够不够
	if (this->GetItemNum(data->buyTimesCostItemId) < cost)
	{
		DebugLogStream << "player(" << GetID() << ") want buy dungeon(" << (UInt8)subType << ") times, current buy times(" << buyedTimes << ") not enough money(" 
						<< data->buyTimesCostItemId << ") need(" << cost << ")." << LogStream::eos;
		return ErrorCode::DUNGEON_TIMES_NO_ENOUGH_MONEY;
	}

	// 扣货币
	auto reason = GetReason(SOURCE_TYPE_DUNGEON_BUY_TIMES, subType, buyedTimes + 1);
	RemoveItem(reason.c_str(), data->buyTimesCostItemId, cost);

	// 加次数
	IncCounter(data->buyTimesCounter.c_str(), 1);

	SyncEventToWorld(SET_BUY_DUNGEON_TIMES, (ObjID_t)subType);

	return ErrorCode::SUCCESS;
}

void Player::AddDungeonTimes(UInt8 subType, UInt32 times)
{
	if (subType >= DungeonSubType::DST_NUM) return;
	auto data = DungeonTimesDataEntryMgr::Instance()->FindEntry(subType);
	if (!data)
	{
		DebugLogStream << "player(" << GetID() << ") want add dungeon(" << subType << ") times, can't find in table." << LogStream::eos;
		return;
	}

	// 加次数
	IncCounter(data->buyTimesCounter.c_str(), times);
	return;
}

UInt32 Player::GetDungeonBestScore(UInt32 dungeonId)
{
	return m_dungeonMgr.GetDungeonBestScore(dungeonId);
}

UInt32 Player::RequestBuyTowerTimes()
{
	// ToDo...

	// 没有次数了，如果有VIP额外购买权限，就去购买
	if (GetCounter(COUNTER_VIP_TOWER_PAY_TIMES) >= GetVipValue(VIP_PRIVILEGE_DEATH_TOWER_RESET))
	{
		return ErrorCode::TOWER_RESET_NO_REMAIN_COUNT;
	}
	else
	{
		UInt32 cost = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_VIP_RESET_TOWER_COST);
		if (GetPointBoth() < cost)
		{
			return ErrorCode::TOWER_RESET_NO_REMAIN_COUNT;
		}

		auto reason = GetReason(SOURCE_TYPE_VIP_BUY_TOWER_RESET);
		RemovePointBoth(reason.c_str(), cost);

		IncCounter(COUNTER_VIP_TOWER_PAY_TIMES, 1);
		IncCounter(COUNTER_TOWER_RESET_REMAIN_TIMES, 1);
	}

	return ErrorCode::SUCCESS;
}

void Player::OnClearDeathTowerFloorMonsters(UInt32 dungeonId, UInt32 areaId, UInt32 usedTime, UInt32 remainHp, UInt32 remainMp)
{
	DebugLogStream << "player(" << GetID() << ") dungeon(" << dungeonId << ") clear area(" << areaId << ") usedTime(" << usedTime << ")." << LogStream::eos;

	UInt32 floor = GetTowerFloorByAreaID(dungeonId, areaId);
	if (floor == 0)
	{
		return;
	}

	auto floorData = DeathTowerAwardDataEntryMgr::Instance()->FindEntry(floor);
	if (!floorData || GetLevel() < floorData->levelLimit)
	{
		ErrorLogStream << "player(" << GetID() << ") clear invalid area(" << areaId << ") floor(" << floor << ") usedTime(" << usedTime << ")." << LogStream::eos;
		return;
	}

	// 每5层奖励
	if (floor % 5 == 0)
	{
		// 保存数据到数据库，最高层数
		GetCounterMgr().SetCounter(COUNTER_TOWER_TOP_FLOOR, floor);

		RewardGroup rewardGroup;
		UInt32 exp = 0;
		UInt32 warriorSoul = 0;
		std::vector<UInt32> taskList;
		GetTaskMgr().GetDungeonTaskList(DEATH_TOWER_DUNGEON_ID, taskList);
		for (UInt32 startFloor = (floor / 5 - 1) * 5 + 1; startFloor <= floor; startFloor++)
		{
			auto award = GetDeathTowerFloorAward(startFloor);
			if (award)
			{
				exp += award->exp;
				warriorSoul += award->warriorSoul;

				/*if (award->awardItemId > 0 && award->awardItemNum > 0)
				{
					rewardGroup.AddReward(award->awardItemId, award->awardItemNum);
				}*/

				// 随机掉落
				if (award->dropSet > 0)
				{
					DungeonAddition addition;

					auto dropResult = DropItemMgr::Instance()->GenDropItems(dungeonId, GetID(), award->dropSet, GetVipLvl(), GetOccu(), taskList, addition);
					for (auto dropItem : dropResult.dropItems)
					{
						DebugLogStream << PLAYERINFO(this) << " dropSet(" << award->dropSet << ") item(" << dropItem.typeId << "," << dropItem.num << "), addition(" << addition.dropAddition[DIT_BERYL] << ")" << LogStream::eos;
						rewardGroup.AddReward(dropItem.typeId, dropItem.num);
					}
			}
		}
		}

		std::string reason = GetReason(SOURCE_TYPE_DUNGEON_DEATH_TOWER, dungeonId, floor);
		IncExp(reason.c_str(), exp);
		AddWarriorSoul(reason.c_str(), warriorSoul);
		AddRewards(reason.c_str(), &rewardGroup);

		CLProtocol::SceneDungeonEndDropRes res;
		res.multi = 1;

		if (exp > 0)
		{
			SceneDungeonDropItem expItem;
			expItem.typeId = ITEM_EXP;
			expItem.num = exp;
			res.items.push_back(expItem);
		}

		if (warriorSoul > 0)
		{
			SceneDungeonDropItem warriorSoulItem;
			warriorSoulItem.typeId = ITEM_WARRIOR_SOUL;
			warriorSoulItem.num = warriorSoul;
			res.items.push_back(warriorSoulItem);
		}

		for (auto& reward : rewardGroup.GetRewards())
		{
			SceneDungeonDropItem item;
			item.typeId = reward.id;
			item.num = reward.num;
			res.items.push_back(item);
		}

		SendProtocol(res);
	}

	if (floor >= GetDeathTowerTopFloor())
	{
		UInt32 usedSec = usedTime / 1000;
		if (floor > GetDeathTowerTopFloor())
		{
			SendFirstClearDeathTowerUdpLog(dungeonId, floor, usedSec);
		}
		if ((floor == GetDeathTowerTopFloor() && usedTime < GetDeathUsedTime()) || floor > GetDeathTowerTopFloor())
		{
			SetDeathUsedTime(usedTime);

			// 如果刷新最高层数,需要记录到排行榜
			SortListType towerSortListType = GetTowerSortListTypeByOccu(GetOccu());
			if (towerSortListType != SORTLIST_INVALID)
			{
				SortListMgr::Instance()->OnSortValueChanged(towerSortListType, GetID(), GetName(), 0, GetOccu(), 0, GetLevel(), floor, usedTime, 0);
			}
			SortListMgr::Instance()->OnSortValueChanged(SORTLIST_TOWER_1, GetID(), GetName(), 0, GetOccu(), 0, GetLevel(), floor, usedTime, 0);
		}
		SetDeathTowerTopFloor(floor);
	}
}

// 获取死亡之塔层数
UInt32 Player::GetTowerFloorByAreaID(UInt32 dungeonId, UInt32 areaId)
{
	auto dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
	if (!dungeonData)
	{
		return 0;
	}
	
	auto index = dungeonData->GetAreaIndex(areaId);
	if (index < 0)
	{
		ErrorLogStream << "dungeon(" << dungeonId << ") can't find area(" << areaId << ") in area list." << LogStream::eos;
		return 0;
	}

	return index + 1;
}

DeathTowerAwardDataEntry* Player::GetDeathTowerFloorAward(UInt32 floor)
{
	return DeathTowerAwardDataEntryMgr::Instance()->FindEntry(floor);
}

UInt32 Player::GetDeathTowerTopFloor()
{
	return GetCounter(COUNTER_TOWER_TOP_FLOOR_TOTAL);
}

void Player::SetDeathTowerTopFloor(UInt32 floor)
{
	SetCounter(COUNTER_TOWER_TOP_FLOOR_TOTAL, floor);
}

UInt32 Player::GetDeathUsedTime()
{
	return GetCounter(COUNTER_TOWER_USED_TIME_TOTAL);
}

void Player::SetDeathUsedTime(UInt32 usedTime)
{
	SetCounter(COUNTER_TOWER_USED_TIME_TOTAL, usedTime);
}

bool Player::CheckRefreshDeathTowerCount()
{
	UInt32 updateTime = m_CounterMgr.GetCounterUpdateTime(COUNTER_TOWER_RESET_REMAIN_TIMES);
	if (updateTime == 0)
	{
		return true;
	}

	return CounterCfg::Instance()->CheckReset(updateTime, COUNTER_CYCLE_DAY, 6);
}

void Player::RefreshDeathTowerCount()
{
	SetCounter(COUNTER_TOWER_RESET_REMAIN_TIMES, 1);
}

UInt32 Player::OnResetDeathTower()
{
	// 如果已经完成扫荡了，直接领奖
	if (m_DeathTowerWipeoutEndTime > 0 && (UInt32)CURRENT_TIME.Sec() > m_DeathTowerWipeoutEndTime)
	{
		OnRequestDeathTowerWipeoutResult();
	}

	// 正在扫荡
	if (m_DeathTowerWipeoutEndTime > 0)
	{
		ErrorLogStream << "player(" << GetID() << ") is doing wipeout death tower." << LogStream::eos;
		return ErrorCode::TOWER_DOING_WIPEOUT;
	}

	if (GetCounter(COUNTER_TOWER_TOP_FLOOR) == 0)
	{
		ErrorLogStream << "player(" << GetID() << ") death tower no need to reset." << LogStream::eos;
		return ErrorCode::TOWER_NO_NEED_RESET;
	}

	UInt32 remainTimes = GetCounter(COUNTER_TOWER_RESET_REMAIN_TIMES);
	if (remainTimes == 0)
	{
		// 没有次数了，如果有VIP额外购买权限，就去购买
		if (GetCounter(COUNTER_VIP_TOWER_PAY_TIMES) >= GetVipValue(VIP_PRIVILEGE_DEATH_TOWER_RESET))
		{
			return ErrorCode::TOWER_RESET_NO_REMAIN_COUNT;
		}
		else
		{
			UInt32 cost = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_VIP_RESET_TOWER_COST);
			if (GetPointBoth() < cost)
			{
				return ErrorCode::TOWER_RESET_NO_REMAIN_COUNT;
			}

			auto reason = GetReason(SOURCE_TYPE_VIP_BUY_TOWER_RESET);
			RemovePointBoth(reason.c_str(), cost);

			IncCounter(COUNTER_VIP_TOWER_PAY_TIMES, 1);
		}
	}
	else
	{
		SetCounter(COUNTER_TOWER_RESET_REMAIN_TIMES, remainTimes - 1);
	}

	SetCounter(COUNTER_TOWER_TOP_FLOOR, 0);

	// 重置勇者之塔日志
	SendUdpLog("tower_reset", LOG_TOWER_RESET_FORMAT, GetCounter(COUNTER_TOWER_TOP_FLOOR_TOTAL));

	return ErrorCode::SUCCESS;
}

UInt32 Player::OnBeginWipeoutDeathTower()
{
	// 如果已经完成扫荡了，直接领奖
	if (m_DeathTowerWipeoutEndTime > 0 && (UInt32)CURRENT_TIME.Sec() > m_DeathTowerWipeoutEndTime)
	{
		OnRequestDeathTowerWipeoutResult();
	}

	// 正在扫荡
	if (m_DeathTowerWipeoutEndTime > 0)
	{
		ErrorLogStream << "player(" << GetID() << ") is doing wipeout death tower." << LogStream::eos;
		return ErrorCode::TOWER_DOING_WIPEOUT;
	}

	UInt32 beginFloor = GetCounter(COUNTER_TOWER_TOP_FLOOR) / 5 * 5;
	UInt32 endFloor = GetCounter(COUNTER_TOWER_TOP_FLOOR_TOTAL) / 5 * 5;

	if (endFloor <= beginFloor)
	{
		ErrorLogStream << "player(" << GetID() << ") wipeout death tower failed, floor (" << beginFloor << ") => (" << endFloor << ")." << LogStream::eos;
		return ErrorCode::TOWER_NO_FLOOR_WIPEOUT;
	}

	// 开始扫荡
	DebugLogStream << "player(" << GetID() << ") request to wipeout death tower, floor (" << beginFloor << ") => (" << endFloor << ")." << LogStream::eos;

	// 计算时间
	UInt32 needSec = (endFloor - beginFloor) * SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_DEATH_TOWER_WIPEOUT_SEC);
	m_DeathTowerWipeoutEndTime = (UInt32)CURRENT_TIME.Sec() + needSec;

	//by huchenhui 每日任务
	m_DailyTaskMgr.OnDeathTowerWipeoutBegin();

	// 开始扫荡日志
	SendUdpLog("tower_begin_wipeout", LOG_TOWER_START_WIPEOUT_FORMAT, beginFloor, endFloor);

	return ErrorCode::SUCCESS;
}

UInt32 Player::OnRequestDeathTowerWipeoutResult()
{
	if (m_DeathTowerWipeoutEndTime == 0)
	{
		ErrorLogStream << "player(" << GetID() << ") is not in wipeout death tower." << LogStream::eos;
		return ErrorCode::TOWER_NOT_DOING_WIPEOUT;
	}

	if (m_DeathTowerWipeoutEndTime > (UInt32)CURRENT_TIME.Sec())
	{
		ErrorLogStream << "player(" << GetID() << ") is wipeout not finish." << LogStream::eos;
		return ErrorCode::TOWER_WIPEOUT_NOT_FINISH;
	}

	UInt32 beginFloor = GetCounter(COUNTER_TOWER_TOP_FLOOR) / 5 * 5;
	UInt32 endFloor = GetCounter(COUNTER_TOWER_TOP_FLOOR_TOTAL) / 5 * 5;

	UInt32 exp = 0;
	UInt32 warriorSoul = 0;
	RewardGroup rewardGroup;
	std::vector<UInt32> taskList;
	GetTaskMgr().GetDungeonTaskList(DEATH_TOWER_DUNGEON_ID, taskList);
	for (UInt32 floor = beginFloor + 1; floor <= endFloor; floor++)
	{
		auto award = GetDeathTowerFloorAward(floor);
		if (award)
		{
			exp += award->exp;
			warriorSoul += award->warriorSoul;

			// 随机掉落
			if (award->dropSet > 0)
			{
				DungeonAddition addition;

				auto dropResult = DropItemMgr::Instance()->GenDropItems(DEATH_TOWER_DUNGEON_ID, GetID(), award->dropSet, GetVipLvl(), GetOccu(), taskList, addition);
				for (auto dropItem : dropResult.dropItems)
				{
					rewardGroup.AddReward(dropItem.typeId, dropItem.num);
				}
			}
		}
	}

	m_DeathTowerWipeoutEndTime = 0;
	SetCounter(COUNTER_TOWER_TOP_FLOOR, endFloor);

	std::string reason = GetReason(SOURCE_TYPE_DEATH_TOWER_WIPEOUT);
	IncExp(reason.c_str(), exp);
	AddWarriorSoul(reason.c_str(), warriorSoul);
	AddRewards(reason.c_str(), &rewardGroup);

	// 发送给客户端
	CLProtocol::SceneTowerWipeoutResultRes res;
	res.result = ErrorCode::SUCCESS;

	if (exp > 0)
	{
		SceneDungeonDropItem expItem;
		expItem.typeId = ITEM_EXP;
		expItem.num = exp;
		res.items.push_back(expItem);
	}

	if (warriorSoul > 0)
	{
		SceneDungeonDropItem warriorSoulItem;
		warriorSoulItem.typeId = ITEM_WARRIOR_SOUL;
		warriorSoulItem.num = warriorSoul;
		res.items.push_back(warriorSoulItem);
	}

	for (auto& reward : rewardGroup.GetRewards())
	{
		SceneDungeonDropItem item;
		item.typeId = reward.id;
		item.num = reward.num;
		res.items.push_back(item);
	}

	SendProtocol(res);

	return ErrorCode::SUCCESS;
}

UInt32 Player::OnRequestDeathTowerWipeouttQueryResult(UInt32 beginFloor, UInt32 endFloor)
{
	/*if (m_DeathTowerWipeoutEndTime == 0)
	{
		ErrorLogStream << "player(" << GetID() << ") is not in wipeout death tower." << LogStream::eos;
		return ErrorCode::TOWER_NOT_DOING_WIPEOUT;
	}*/

	UInt32 exp = 0;
	UInt32 warriorSoul = 0;
	RewardGroup rewardGroup;
	std::vector<UInt32> taskList;
	GetTaskMgr().GetDungeonTaskList(DEATH_TOWER_DUNGEON_ID, taskList);
	for (UInt32 floor = beginFloor; floor <= endFloor; floor++)
	{
		auto award = GetDeathTowerFloorAward(floor);
		if (award)
		{
			exp += award->exp;
			warriorSoul += award->warriorSoul;

			// 随机掉落
			if (award->dropSet > 0)
			{
				DungeonAddition addition;

				auto dropResult = DropItemMgr::Instance()->GenDropItems(DEATH_TOWER_DUNGEON_ID, GetID(), award->dropSet, GetVipLvl(), GetOccu(), taskList, addition);
				for (auto dropItem : dropResult.dropItems)
				{
					rewardGroup.AddReward(dropItem.typeId, dropItem.num);
				}
			}
		}
	}

	//m_DeathTowerWipeoutEndTime = 0;
	//SetCounter(COUNTER_TOWER_TOP_FLOOR, endFloor);

	/*std::string reason = GetReason(SOURCE_TYPE_DEATH_TOWER_WIPEOUT);
	IncExp(reason.c_str(), exp);
	AddWarriorSoul(reason.c_str(), warriorSoul);
	AddRewards(reason.c_str(), &rewardGroup);*/

	// 发送给客户端
	CLProtocol::SceneTowerWipeoutQueryResultRes res;
	res.result = ErrorCode::SUCCESS;

	if (exp > 0)
	{
		SceneDungeonDropItem expItem;
		expItem.typeId = ITEM_EXP;
		expItem.num = exp;
		res.items.push_back(expItem);
	}

	if (warriorSoul > 0)
	{
		SceneDungeonDropItem warriorSoulItem;
		warriorSoulItem.typeId = ITEM_WARRIOR_SOUL;
		warriorSoulItem.num = warriorSoul;
		res.items.push_back(warriorSoulItem);
	}

	for (auto& reward : rewardGroup.GetRewards())
	{
		SceneDungeonDropItem item;
		item.typeId = reward.id;
		item.num = reward.num;
		res.items.push_back(item);
	}

	SendProtocol(res);

	return ErrorCode::SUCCESS;
}

UInt32 Player::OnQuickFinishDeathTowerWipeout()
{
	if (m_DeathTowerWipeoutEndTime == 0)
	{
		ErrorLogStream << "player(" << GetID() << ") is not in wipeout death tower." << LogStream::eos;
		return ErrorCode::TOWER_NOT_DOING_WIPEOUT;
	}

	if (m_DeathTowerWipeoutEndTime <= (UInt32)CURRENT_TIME.Sec())
	{
		ErrorLogStream << "player(" << GetID() << ") is wipeout not finish." << LogStream::eos;
		return ErrorCode::TOWER_WIPEOUT_FINISHED;
	}

	UInt32 remaimSec = m_DeathTowerWipeoutEndTime - (UInt32)CURRENT_TIME.Sec();
	UInt32 perFloorSec = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_DEATH_TOWER_WIPEOUT_SEC);
	UInt32 perFloorCost = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_DEATH_TOWER_FINISH_WIPEOUT_COST);
	UInt32 remainFloor = remaimSec / perFloorSec + (remaimSec % perFloorSec == 0 ? 0 : 1);
	UInt32 needPoint = remainFloor * perFloorCost;

	if (GetPoint() < needPoint)
	{
		ErrorLogStream << "player(" << GetID() << ") want quick finish death tower wipeout, don't have enough point(" 
						<< GetPoint() << ") need point(" << needPoint << ")." << LogStream::eos;
		return ErrorCode::TOWER_WIPEOUT_NOT_ENOUGH_POINT;
	}

	auto reason = GetReason(SOURCE_TYPE_DEATH_TOWER_WIPEOUT);
	RemovePoint(reason.c_str(), needPoint);

	m_DeathTowerWipeoutEndTime = (UInt32)CURRENT_TIME.Sec();

	return ErrorCode::SUCCESS;
}

UInt32 Player::OnRequestDeathTowerPassFloorAward(UInt32 floor)
{
	DebugLogStream << "player(" << GetID() << ") request floor(" << floor << ") award." << LogStream::eos;
	if (GetDeathTowerTopFloor() < floor)
	{
		DebugLogStream << "player(" << GetID() << ") request floor(" << floor << ") award, have not pass this floor." << LogStream::eos;
		return ErrorCode::TOWER_AWARD_NOT_PASS_FLOOR;
	}

	UInt32 index = floor / 5 - 1;
	UInt32 mask = GetCounter(COUNTER_TOWER_AWARD_MASK);
	if ((mask & (1 << index)) != 0)
	{
		DebugLogStream << "player(" << GetID() << ") request floor(" << floor << ") award, repeat get award." << LogStream::eos;
		return ErrorCode::TOWER_AWARD_REPEAT_RECEIVE;
	}

	auto award = GetDeathTowerFloorAward(floor);
	if (!award || award->awardItemId == 0 || award->awardItemNum == 0 || floor % 5 != 0)
	{
		DebugLogStream << "player(" << GetID() << ") request floor(" << floor << ") award, invalid floor." << LogStream::eos;
		return ErrorCode::TOWER_AWARD_INVALID_FLOOR;
	}

	RewardGroup rewardGroup;
	if (award->awardItemId > 0 && award->awardItemNum > 0)
	{
		rewardGroup.AddReward(award->awardItemId, award->awardItemNum);
	}

	std::string reason = GetReason(SOURCE_TYPE_DEATH_TOWER_PASS_FLOOR_AWARD, floor);
	AddRewards(reason.c_str(), &rewardGroup);
	
	mask |= 1 << index;
	SetCounter(COUNTER_TOWER_AWARD_MASK, mask);

	/*CLProtocol::SceneTowerFloorAwardRes res;
	res.result = ErrorCode::SUCCESS;
	for (auto& reward : rewardGroup.GetRewards())
	{
	SceneDungeonDropItem item;
	item.typeId = reward.id;
	item.num = reward.num;
	res.items.push_back(item);
	}
	SendProtocol(res);*/
	CLProtocol::SceneNotifyGetItem notify;
	notify.sourceType = SOURCE_TYPE_DEATH_TOWER_PASS_FLOOR_AWARD;
	for (auto& reward : rewardGroup.GetRewards())
	{
		ItemReward item;
		item.id = reward.id;
		item.num = reward.num;
		notify.items.push_back(item);
	}
	SendProtocol(notify);

	return ErrorCode::SUCCESS;
}

UInt32 Player::ResetTowerCounter()
{
	SetDeathUsedTime(0);
	SetDeathTowerTopFloor(0);
	SetCounter(COUNTER_TOWER_TOP_FLOOR, 0);
	return ErrorCode::SUCCESS;
}

void Player::SetPackSize(UInt32 pksz)
{
	UInt32 initSz = PlayerMgr::Instance()->GetPlayerAttr()->GetProperty("PlayerInitInfo", "packsize");
	if (pksz < initSz)
	{
		ErrorLogStream << PLAYERINFO(this) << "set value illegal! packet sz = " << pksz << LogStream::eos;
		return;
	}
	
	m_PackSize = pksz;
	UInt32 finalSize = m_PackSize;

	for (UInt32 i = PACK_EQUIP; i <= PACK_TASK; ++i)
	{
		finalSize = m_PackSize;
		finalSize += GetPackageAdditionSize((PackType)i);

		Package* pPack = m_ItemMgr.GetPackage((UInt32)i);
		if (pPack != NULL)
		{
			pPack->SetSize(finalSize);
		}
	}

	Package* ppPack = m_ItemMgr.GetPackage(PACK_EQRCOV);
	if (ppPack != NULL)
	{
		finalSize = m_PackSize;
		finalSize += GetPackageAdditionSize(PACK_EQRCOV);

		ppPack->SetSize(finalSize);
	}
}

void Player::SetStorageSize(UInt8 sz)
{
	UInt8 initSz = PlayerMgr::Instance()->GetPlayerAttr()->GetProperty("PlayerInitInfo", "storagesize");
	if (sz < initSz)
	{
		ErrorLogStream << PLAYERINFO(this) << "set value illegal! stroage sz = " << sz << LogStream::eos;
		return;
	}
	
	m_StorageSize = sz;

	Package* pPack = m_ItemMgr.GetPackage(PACK_STORAGE);
	if (pPack != NULL)
	{
		pPack->SetSize(m_StorageSize);

		InfoLogStream << PLAYERINFO(this) << " set storage size = " << m_StorageSize << " add size = " << sz << LogStream::eos;
	}
}

void Player::UpdateNewItem(UInt8 packageType)
{
	class UpdateNewItemVisitor : public ItemVisitor
	{
	public:
		UpdateNewItemVisitor(Player* player):m_player(player) {}

		bool operator()(Item* item)
		{
			if (item->GetParam1() > 0)
			{
				item->SetParam1(0);
				item->SyncProperty(m_player);
			}
			return true;
		}

	private:
		Player* m_player;
	};

	UpdateNewItemVisitor visitor(this);
	m_ItemMgr.VisitItems(packageType, visitor);
}

UInt32 Player::CanStartPkMatch(PkType type)
{
	if ((type != PK_GUILD_BATTLE && type != PK_3V3_ACTIVITY) && GetScene()->GetDataEntry()->type != SCENE_TYPE_PK_PREPARE)
	{
		return ErrorCode::MATCH_START_NOT_IN_PK_PARPARE;
	}


	if (GetTeam())
	{
		return ErrorCode::MATCH_START_IN_TEAM;
	}

	if (type == PK_WUDAODAHUI)
	{
		if (GetWudaoStatus() == WUDAO_STATUS_INIT)
		{
			return ErrorCode::MATCH_START_WUDAO_NOT_JOIN;
		}
		else if (GetWudaoStatus() == WUDAO_STATUS_COMPLETE)
		{
			return ErrorCode::MATCH_START_WUDAO_COMPLETE;
		}

		/*UInt32 activityId = 0;
		if (!ActivityMgr::Instance()->IsInActivity(activityId, this))
		{
			return ErrorCode::PK_WUDAO_ACTIVITY_COND;
		}*/
	}

	if (type == PK_3V3_ACTIVITY)
	{
		if (GetLevel() < SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_SCORE_WAR_LEVEL))
		{
			return ErrorCode::ROOM_SYSTEM_JOIN_PLAYER_LEVEL;
		}

		if (GetScoreWarBattleCount() <= 0)
		{
			return ErrorCode::SCORE_WAR_REWARD_BATTLE_COUNT_ERROR;
		}
	}
	else if (type == PK_SEASON_1V1)
	{
		if (!CheckFuncFlag(FUNC_PK))
		{
			return ErrorCode::DUNGEON_START_LEVEL_LIMIT;
		}
	}

	return ErrorCode::SUCCESS;
}

RacePlayerInfo Player::GetMatchRacePlayerInfo(bool pvp)
{
    RacePlayerInfo info;
	info.zoneId = ZONE_ID;
	info.serverName = SERVER_NAME;
    info.accid = GetAccID();
    info.roleId = GetID();
    info.name = GetName();
    info.occu = GetOccu();
    info.level = GetLevel();
    info.pkValue = GetPkValue();
	info.matchScore = GetMatchScore();
	if (SERVER_TYPE == ST_BSCENE)
	{
		info.remainHp = GetChijiHp();
		info.remainMp = GetChijiMp();
	}
	else
	{
	info.remainHp = HP_MP_FULL_VALUE;
	info.remainMp = HP_MP_FULL_VALUE;
	}

	info.seasonLevel = GetSeasonLevel();
	info.seasonStar = GetSeasonStar();
	info.seasonAttr = GetSeasonAttr();
	info.monthcard = HasMonthCard() ? 1 : 0;
	info.potionPos = m_potionSet.GetPosionPos();

	GetRaceSkills(info.skills, pvp);

    std::vector<Item*> equips;
    GetEquips(equips);
    for (auto equip : equips)
    {
        if (!equip)
        {
            continue;
        }
        RaceEquip raceEquip;
        equip->GetRaceEquip(raceEquip, pvp);
        info.equips.push_back(raceEquip);
		info.wearingEqIds.push_back(equip->GetID());
    }

	GetRaceItem(info.raceItems);
	GetRaceBuffs(info.buffs);

	GetRaceRetinues(info.retinues);
	
	GetRacePets(info.pets);

	if (SERVER_TYPE != ST_BSCENE)
	{
		info.pkValue = GetPkValue();
		info.matchScore = GetMatchScore();

		info.seasonLevel = GetSeasonLevel();
		info.seasonStar = GetSeasonStar();
		info.seasonAttr = GetSeasonAttr();
		info.monthcard = HasMonthCard() ? 1 : 0;

		std::vector<Item*> secondWeapons;
		GetSecondWeapons(secondWeapons);
		for (auto weapon : secondWeapons)
		{
			if (!weapon)
			{
				continue;
			}
			RaceEquip raceWeapon;
			weapon->GetRaceEquip(raceWeapon, pvp);
			info.secondWeapons.push_back(raceWeapon);
		}

		GetPlayerLabelInfo(info.playerLabelInfo);
	}

	if (SERVER_TYPE == ST_BSCENE)
	{
		// 计算外观
		CountAvatar();
	}

	info.avatar = GetAvatar();

    return info;
}

void Player::SetMatchScore(UInt32 score)
{
	if (m_MatchScore == score)
	{
		return;
	}

	m_MatchScore = score;
	if (score > GetBestMatchScore())
	{
		SetBestMatchScore(score);
	}
}

UInt32 Player::GetBestMatchScore()
{
	return GetCounter(COUNTER_BEST_MATCH_SCORE);
}

void Player::SetBestMatchScore(UInt32 score)
{
	SetCounter(COUNTER_BEST_MATCH_SCORE, score);
}

void Player::UpdateSeasonPkStraightCount(PkRaceResult result)
{
	/*
		胜负情况	修正分值	输/赢1局变化
		连胜1		0			连负1
		连胜2		100			连负1
		连胜3		150			降为连胜1
		连胜4		350			降为连胜2
		连胜5		500			降为连胜3
		连胜6以上	600			降为连胜4
		连负1		0			连胜1
		连负2		-100		连胜1
		连负3		-150		升为连负1
		连负4		-350		升为连负2
		连负5		-500		升为连负3
		连负6以上	-600		升为连负4
	*/

	if (result != PKRR_WIN && result != PKRR_LOSE)
	{
		return;
	}

	// 最高位为0代表目前正在连胜，1就代表正在连败
	int curValue = (int)GetCounter(COUNTER_SEASON_PVP_STRAIGHT_COUNT);
	bool wining = curValue > 0;
	int count = abs(curValue);
	bool win = result == PKRR_WIN;

	int newValue = 1;
	if (win == wining)
	{
		count++;
		newValue = count * (win ? 1 : -1);
	}
	else
	{
		if (count <= 2)
		{
			count = 1;
			newValue = count * (win ? 1 : -1);
		}
		else if (count < 6)
		{
			count -= 2;
			newValue = count * (wining ? 1 : -1);
		}
		else
		{
			count = 4;
			newValue = count * (wining ? 1 : -1);
		}
	}

	
	DebugLogStream << PLAYERINFO(this) << " season pk straight count(" << newValue << ")." << LogStream::eos;
	SetCounter(COUNTER_SEASON_PVP_STRAIGHT_COUNT, newValue);
}

UInt32 Player::GetSeasonPkWinStraight()
{
	int curValue = (int)GetCounter(COUNTER_SEASON_PVP_STRAIGHT_COUNT);
	if (curValue < 0)
	{
		return 0;
	}

	return (UInt32)curValue;
}

UInt32 Player::GetSeasonPkLoseStraight()
{
	int curValue = (int)GetCounter(COUNTER_SEASON_PVP_STRAIGHT_COUNT);
	if (curValue > 0)
	{
		return 0;
	}

	return (UInt32)(-curValue);
}

void Player::UpdateScoreWarStraightCount(PkRaceResult result)
{
	/*
	胜负情况	修正分值	输/赢1局变化
	连胜1		100			连负1
	连胜2		250			连负1
	连胜3		450			降为连胜1
	连胜4		500			降为连胜2
	连胜5		600			降为连胜3
	连负1		-100		连胜1
	连负2		-250		连胜1
	连负3		-450		升为连负1
	连负4		-500		升为连负2
	连负5		-600		升为连负3
	*/

	if (result != PKRR_WIN && result != PKRR_LOSE)
	{
		return;
	}

	// 最高位为0代表目前正在连胜，1就代表正在连败
	int curValue = (int)GetCounter(COUNTER_SCORE_WAR_STRAIGHT_COUNT);
	bool wining = curValue > 0;
	int count = abs(curValue);
	bool win = result == PKRR_WIN;

	int newValue = 1;
	if (win == wining)
	{
		count++;
		newValue = count * (win ? 1 : -1);
	}
	else
	{
		if (count <= 2)
		{
			count = 1;
			newValue = count * (win ? 1 : -1);
		}
		else if (count < 6)
		{
			count -= 2;
			newValue = count * (wining ? 1 : -1);
		}
		else
		{
			count = 4;
			newValue = count * (wining ? 1 : -1);
		}
	}


	DebugLogStream << PLAYERINFO(this) << " score war pk straight count(" << newValue << ")." << LogStream::eos;
	SetCounter(COUNTER_SCORE_WAR_STRAIGHT_COUNT, newValue);
}

UInt32 Player::GetScoreWarWinStraight()
{
	int curValue = (int)GetCounter(COUNTER_SCORE_WAR_STRAIGHT_COUNT);
	if (curValue < 0)
	{
		return 0;
	}

	return (UInt32)curValue;
}

UInt32 Player::GetScoreWarLoseStraight()
{
	int curValue = (int)GetCounter(COUNTER_SCORE_WAR_STRAIGHT_COUNT);
	if (curValue > 0)
	{
		return 0;
	}

	return (UInt32)(-curValue);
}

void Player::InitScoreWarStraightCount()
{
	SetCounter(COUNTER_SCORE_WAR_STRAIGHT_COUNT, 0);
}

UInt32 Player::GetPkValue() const
{
    return (UInt32)m_pkValue;
}

PkLevelType Player::GetPkLevelType() const
{
	PkLevelType type = PKLV_BRONZE;
	UInt8 level = 0;
	PkLevelDataEntryMgr::Instance()->GetLevelByPkValue(GetPkValue(), type, level);
	return type;
}

UInt8 Player::GetPkLevel() const
{
	PkLevelType type = PKLV_BRONZE;
	UInt8 level = 0;
	PkLevelDataEntryMgr::Instance()->GetLevelByPkValue(GetPkValue(), type, level);
	return level;
}

void Player::IncPkValue(UInt32 value)
{
    if (value == 0)
    {
        return;
    }

	UInt32 oldPkVal = m_pkValue;
    m_pkValue = m_pkValue + value;

	//判断是否段位等级变化
	PkLevelType oldType = PKLV_BRONZE;
	PkLevelType type = PKLV_BRONZE;
	UInt8 oldLevel = 0;
	UInt8 level = 0;
	PkLevelDataEntryMgr::Instance()->GetLevelByPkValue(oldPkVal, oldType, oldLevel);
	PkLevelDataEntryMgr::Instance()->GetLevelByPkValue(m_pkValue, type, level);
}

void Player::DecPkValue(UInt32 value)
{
    if (value == 0)
    {
        return;
    }

    if (m_pkValue <= value)
    {
        m_pkValue = 0;
    }
    else
    {
        m_pkValue = m_pkValue - value;
    }
}

UInt32 Player::GetMaxWinStreak(PkType type)
{
	PkStatistic* pk = m_pkStatisticMgr.FindPkStatistic(type);
	if (pk == NULL) return 0;
	return pk->GetMaxWinStreak();
}

UInt32 Player::GetCurWinStreak(PkType type)
{
	PkStatistic* pk = m_pkStatisticMgr.FindPkStatistic(type);
	if (pk == NULL) return 0;
	return pk->GetCurWinStreak();
}

UInt32 Player::GetTotalWinNum(PkType type)
{
	PkStatistic* pk = m_pkStatisticMgr.FindPkStatistic(type);
	if (pk == NULL) return 0;
	return pk->GetTotalWinNum();
}

void Player::SetActiveTitle(UInt32 titleId)
{
    m_titleId = titleId;
}

UInt32 Player::JoinWudaodahui()
{
	/*
		武道大会有多个活动，只要其中一个开始了就可以了
	*/
	bool activityOpened = false;
	UInt32 startActivityId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_WUDAO_ACTIVITY_ID_BEGIN);
	UInt32 endActivityId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_WUDAO_ACTIVITY_ID_END);
	for (auto activityId = startActivityId; activityId <= endActivityId; ++activityId)
	{
		if (!ActivityMgr::Instance()->IsInActivity(activityId))
		{
			continue;
		}

		// 是否满足活动开启条件
		if (!ActivityMgr::Instance()->IsInActivity(activityId, this))
		{
			return ErrorCode::PK_WUDAO_ACTIVITY_COND;
		}

		activityOpened = true;
		break;
	}

	if (!activityOpened)
	{
		return ErrorCode::PK_WUDAO_ACTIVITY_NOT_OPEN;
	}

	// 如果是可领奖状态，帮他领了
	if (GetWudaoStatus() == WUDAO_STATUS_COMPLETE)
	{
		DebugLogStream << "player(" << GetID() << ") client bug, auto get wudaodahui reward." << LogStream::eos;
		RequestWudaodahuiReward();
	}

	// 判断武道大会状态
	if (GetWudaoStatus() != WUDAO_STATUS_INIT)
	{
		return ErrorCode::PK_WUDAO_JOINED;
	}

	// 第一次免费
	if (GetCounter(COUNTER_WUDAO_TIMES) > 0)
	{
		// 判断门票够不够
		UInt32 ticketId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_WUDAO_TICKET_ID);;
		if (GetItemNumWithEqualItem(ticketId) <= 0)
		{
			return ErrorCode::PK_WUDAO_NO_TICKET;
		}

		auto reason = GetReason(LogSourceType::SOURCE_TYPE_WUDAO);
		RemoveItemWithEqualItem(reason.c_str(), ticketId, 1);
	}

	SetWudaoStatus(WUDAO_STATUS_PLAYING);
	IncCounter(COUNTER_WUDAO_TIMES, 1);
	SetCounter(COUNTER_WUDAO_WIN_NUMS, 0);
	SetCounter(COUNTER_WUDAO_LOSE_NUMS, 0);

	DebugLogStream << "player(" << GetID() << ") join wudaodahui." << LogStream::eos;

	return ErrorCode::SUCCESS;
}

void Player::OnWudaodahuiPkEnd(PkRaceResult result)
{
	if (GetWudaoStatus() != WUDAO_STATUS_PLAYING)
	{
		return;
	}

	if (result == PKRR_WIN)
	{
		IncCounter(COUNTER_WUDAO_WIN_NUMS, 1);
		OnWinWudaodahuiPkEnd(GetCounter(COUNTER_WUDAO_WIN_NUMS));
	}
	else if (result == PKRR_LOSE)
	{
		IncCounter(COUNTER_WUDAO_LOSE_NUMS, 1);
	}
	else
	{
		return;
	}

	CheckWudaodahuiStatus(false);
}

UInt32 Player::RequestWudaodahuiReward()
{
	DebugLogStream << "player(" << GetID() << ") request to get wudao reward." << LogStream::eos;

	if (GetWudaoStatus() != WUDAO_STATUS_COMPLETE)
	{
		InfoLogStream << "player(" << GetID() << ") request to get wudao reward, status(" << (UInt8)GetWudaoStatus() << ")." << LogStream::eos;
		return ErrorCode::PK_WUDAO_NOT_COMPLETE;
	}
	
	auto winNum = GetCounter(COUNTER_WUDAO_WIN_NUMS);
	auto jarType = WudaoRewardDataEntryMgr::Instance()->GetRewardJar(winNum);
	if (jarType == 0)
	{
		InfoLogStream << "player(" << GetID() << ") request to get wudao reward, gen reward failed, winNum(" << winNum << ")." << LogStream::eos;
		return ErrorCode::PK_WUDAO_SYSTEM_ERROR;
	}

	std::string reasonStr = GetReason(LogSourceType::SOURCE_TYPE_WUDAO_REWARD);
	auto result = m_JarMgr.GiveJarRewards(reasonStr, jarType);
	if (result != ErrorCode::SUCCESS)
	{
		return result;
	}

	SetWudaoStatus(WUDAO_STATUS_INIT);
	SetCounter(COUNTER_WUDAO_WIN_NUMS, 0);
	SetCounter(COUNTER_WUDAO_LOSE_NUMS, 0);

	return ErrorCode::SUCCESS;
}

void Player::CheckWudaodahuiStatus(bool dayChanged)
{
	if (GetWudaoStatus() != WUDAO_STATUS_PLAYING)
	{
		return;
	}

	UInt32 winNum = GetCounter(COUNTER_WUDAO_WIN_NUMS);
	UInt32 loseNum = GetCounter(COUNTER_WUDAO_LOSE_NUMS);
	UInt32 maxWinNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_WUDAO_MAX_WIN_NUM);
	UInt32 maxLoseNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_WUDAO_MAX_LOSE_NUM);

	bool activityOpened = false;
	UInt32 startActivityId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_WUDAO_ACTIVITY_ID_BEGIN);
	UInt32 endActivityId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_WUDAO_ACTIVITY_ID_END);
	for (auto activityId = startActivityId; activityId <= endActivityId; ++activityId)
	{
		if (!ActivityMgr::Instance()->IsInActivity(activityId))
		{
			continue;
		}

		activityOpened = true;
	}

	if (winNum >= maxWinNum || loseNum >= maxLoseNum ||
		// 活动已经关闭或者已经过了一天，需要修改武道大会状态
		(!activityOpened || dayChanged))
	{
		DebugLogStream << "activity(" << startActivityId << ", " << endActivityId << ") maxWinNum(" << maxWinNum << ") maxLoseNum(" << maxLoseNum << ")." << LogStream::eos;
		// 设置状态为可领奖状态
		SetWudaoStatus(WUDAO_STATUS_COMPLETE);
	}
}

void Player::AddGuildContribution(const char* reason, UInt32 num, bool syncToWorld)
{
	if (reason == NULL) return;
	UInt32 oldValue = m_GuildContri;
	if (m_GuildContri + num > MAX_MONEY) m_GuildContri = MAX_MONEY;
	else m_GuildContri = m_GuildContri + num;

	UInt32 newValue = m_GuildContri;
	if (newValue > oldValue)
	{
		GameInfoLogStream("Property") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") addGuildContribution:"
			<< reason << "|" << newValue - oldValue << "|" << newValue << LogStream::eos;

		SendAssetUdpLog(AssetType::ASSET_GUILD_CONTRI, reason, (UInt64)oldValue, (Int64)num, (Int64)newValue - (Int64)oldValue, (UInt64)newValue);

		if (syncToWorld)
		{
			CLProtocol::WorldGuildSyncContribution sync;
			sync.id = GetID();
			sync.isAdd = 1;
			sync.value = newValue - oldValue;
			Router::SendToWorld(sync);
		}
	}
}

void Player::RemoveGuildContribution(const char* reason, UInt32 num)
{
	if (reason == NULL) return;
	UInt32 oldValue = m_GuildContri;
	if (num >= m_GuildContri) m_GuildContri = 0;
	else m_GuildContri = m_GuildContri - num;

	UInt32 newValue = m_GuildContri;
	if (oldValue > newValue)
	{
		GameInfoLogStream("Property") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") removeGuildContri:"
			<< reason << "|" << oldValue - newValue << "|" << newValue << LogStream::eos;

		OnChangeMoney(MoneyType::MONEY_GUILD_CONTRI, num * -1);
		SendAssetUdpLog(AssetType::ASSET_GUILD_CONTRI, reason, (UInt64)oldValue, (Int64)num * -1, (Int64)newValue - (Int64)oldValue, (UInt64)newValue);
	}
}

void Player::RequestGuildExchange()
{
	CLProtocol::WorldGuildOperRes res;
	res.type = GUILD_OP_EXCHANGE;
	
	if (CURRENT_TIME.Sec() < GetCounter(COUNTER_GUILD_EXCHANGE_COLDTIME))
	{
		res.result = GUILD_EXCHANGE_CD;
		SendProtocol(res);
		return;
	}

	UInt8 welfareLevel = GetGuildBuildingLevel(GUILD_BUILDING_WELFARE);
	auto buildingData = GuildBuildingDataMgr::Instance()->FindEntry(welfareLevel);
	if (!buildingData || buildingData->welfareGiftId == 0)
	{
		ErrorLogStream << "player(" << GetID() << ") welfare level(" << welfareLevel << ") invalid data res." << LogStream::eos;
		return;
	}


	UInt32 maxNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_EXCHANGE_MAX_TIME);
	if (GetCount(COUNTER_GUILD_EXCHANGE) >= maxNum)
	{
		res.result = GUILD_EXCHANGE_NO_REMAIN_TIMES;
		SendProtocol(res);
		return;
	}

	// 判断贡献够不够
	UInt32 cost = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_EXCHANGE_COST);
	if (GetGuildContribution() < cost)
	{
		res.result = GUILD_EXCHANGE_NOT_ENOUGH_CONTRI;
		SendProtocol(res);
		return;
	}

	RemoveGuildContribution("exchange", cost);

	RewardGroup group;
	group.AddReward(buildingData->welfareGiftId, 1);
	AddRewards(GetReason(SOURCE_TYPE_GUILD_EXCHANGE).c_str(), &group, true);

	UInt32 coldtime = (UInt32)CURRENT_TIME.Sec() + GUILD_EXCHANGE_COLDDOWN;

	res.param1 = coldtime;
	res.result = ErrorCode::SUCCESS;
	SendProtocol(res);

	IncCounter(COUNTER_GUILD_EXCHANGE, 1);
	SetCounter(COUNTER_GUILD_EXCHANGE_COLDTIME, coldtime);
}

void Player::SetGuildBuildingLevel(GuildBuildingType type, UInt8 level)
{
	if (type >= GUILD_BUILDING_NUM)
	{
		return;
	}

	m_GuildBuildingLevel[type] = level;
}

UInt8 Player::GetGuildBuildingLevel(GuildBuildingType type)
{
	if (type >= GUILD_BUILDING_NUM)
	{
		return 0;
	}

	return m_GuildBuildingLevel[type];
}

void Player::SetGuildBattleReward(std::string mask)
{
	std::istringstream is(mask);
	m_GuildBattleRewardMask.DecodeString(is);
	m_GuildBattleRewardMask.SetDirty();
}

void Player::SetGuildEmblem(UInt32 lvl)
{
	if (lvl == 0) return;

	m_GuildEmblem = lvl;
	SyncProperty();

	m_AchievementTaskMgr.OnGuildEmblemUp(lvl);
}

void Player::OnRemoveFigureStatus(FigureStatueInfo info)
{
	if (info.statueType == FigureStatueType::FST_GUILD)
	{
		GetSkillMgr().DelStatueSkill(SystemValueType::SVT_GUILD_BATTLE_STATUE_SKILL_ID);
		GetPvpSkillMgr().DelStatueSkill(SystemValueType::SVT_GUILD_BATTLE_STATUE_SKILL_ID);
	}
	else if (info.statueType == FigureStatueType::FST_GUILD_ASSISTANT)
	{
		GetSkillMgr().DelStatueSkill(SystemValueType::SVT_GUILD_BATTLE_ASSISTANT_STATUE_SKILL_ID);
		GetPvpSkillMgr().DelStatueSkill(SystemValueType::SVT_GUILD_BATTLE_ASSISTANT_STATUE_SKILL_ID);
	}
	else if (info.statueType == FigureStatueType::FST_GUILD_ASSISTANT_TWO)
	{
		GetSkillMgr().DelStatueSkill(SystemValueType::SVT_GUILD_BATTLE_ASSISTANT_STATUE_SKILL_ID);
		GetPvpSkillMgr().DelStatueSkill(SystemValueType::SVT_GUILD_BATTLE_ASSISTANT_STATUE_SKILL_ID);
	}
}

void Player::OnCreateFigureStatus(FigureStatueInfo info)
{
	if (info.statueType == FigureStatueType::FST_GUILD)
	{
		GetSkillMgr().AddStatueSkill(SystemValueType::SVT_GUILD_BATTLE_STATUE_SKILL_ID);
		GetPvpSkillMgr().AddStatueSkill(SystemValueType::SVT_GUILD_BATTLE_STATUE_SKILL_ID);
	}
	else if (info.statueType == FigureStatueType::FST_GUILD_ASSISTANT)
	{
		GetSkillMgr().AddStatueSkill(SystemValueType::SVT_GUILD_BATTLE_ASSISTANT_STATUE_SKILL_ID);
		GetPvpSkillMgr().AddStatueSkill(SystemValueType::SVT_GUILD_BATTLE_ASSISTANT_STATUE_SKILL_ID);
	}
	else if (info.statueType == FigureStatueType::FST_GUILD_ASSISTANT_TWO)
	{
		GetSkillMgr().AddStatueSkill(SystemValueType::SVT_GUILD_BATTLE_ASSISTANT_STATUE_SKILL_ID);
		GetPvpSkillMgr().AddStatueSkill(SystemValueType::SVT_GUILD_BATTLE_ASSISTANT_STATUE_SKILL_ID);
	}
}

void Player::BroadcastNewZone(Avalon::Packet* packet)
{
	if (!IsInScene()) return;

	CLProtocol::GateBroadcastExcept req(packet);
	req.sceneid = GetScene()->GetID();
	req.grid1 = GetGridIndex();
	req.grid2 = GetOldGridIndex();
	req.exceptobj = GetID();
	GetWatchPlayerList(req.targetObjList, false);
	Router::BroadcastToGate(req);
	req.Detach();
}

void Player::BroadcastOldZone(Avalon::Packet* packet)
{
	if (!IsInScene()) return;

	CLProtocol::GateBroadcastExcept req(packet);
	req.sceneid = GetScene()->GetID();
	req.grid1 = GetOldGridIndex();
	req.grid2 = GetGridIndex();
	req.exceptobj = GetID();
	GetWatchPlayerList(req.targetObjList, false);
	Router::BroadcastToGate(req);
	req.Detach();
}

void Player::Broadcast(Avalon::Protocol& protocol)
{
	if (!IsInScene()) return;

	Avalon::Packet* packet = Avalon::Packet::Create();
	if (protocol.Encode(packet))
	{
		Broadcast(packet);
	}
	Avalon::Packet::Destroy(packet);
}

void Player::Broadcast(Avalon::Packet* packet)
{
	if (packet == NULL) return;
	if (!IsInScene()) return;

	CLNetworkPerfMgr::Instance()->OnSendPacket(GetAccID(), packet->GetID(), packet->GetSize());

	CLProtocol::GateBroadcastAround req(packet);
	req.sceneid = GetScene()->GetID();
	req.grid = GetGridIndex();
	req.exceptobj = 0;
	GetWatchPlayerList(req.targetObjList, true);
	Router::BroadcastToGate(req);
	req.Detach();
}

void Player::BroadcastExceptMe(Avalon::Protocol& protocol)
{
	if (!IsInScene()) return;

	Avalon::Packet* packet = Avalon::Packet::Create();
	if (protocol.Encode(packet))
	{
		BroadcastExceptMe(packet);
	}
	Avalon::Packet::Destroy(packet);
}

void Player::BroadcastExceptMe(Avalon::Packet* packet)
{
	if (packet == NULL) return;
	if (!IsInScene()) return;

	CLNetworkPerfMgr::Instance()->OnSendPacket(GetAccID(), packet->GetID(), packet->GetSize());

	CLProtocol::GateBroadcastAround req(packet);
	req.sceneid = GetScene()->GetID();
	req.grid = GetGridIndex();
	req.exceptobj = GetID();
	GetWatchPlayerList(req.targetObjList, false);
	Router::BroadcastToGate(req);
	req.Detach();
}

void Player::AddAroundPlayer(Player* player, bool notify)
{
	if (SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_NEW_SCENE_SYNC))
	{
		return;
	}

	if (!player || player == this)
	{
		return;
	}

	// 如果是房间场景,只显示房间号一样的玩家
	if (GetScene() && GetScene()->GetDataEntry() && GetScene()->GetDataEntry()->subType == SCENE_SUB_ROOM && player->GetRoomId() != GetRoomId())
	{
		return;
	}

	// 如果是公会场景,只显示公会一样的玩家
	if (GetScene() && GetScene()->GetDataEntry() && GetScene()->GetDataEntry()->subType == SCENE_SUB_GUILD && player->GetGuildId() != GetGuildId())
	{
		return;
	}

	PlayerSightPriority priority;
	priority.id = player->GetID();
	priority.level = player->GetLevel();
	priority.isFriend = IsFriend(player->GetID());
	m_aroundPlayerList.insert(priority);

	//DebugLogStream << "player(" << GetAccID() << ") add around player(" << player->GetAccID() << ")." << LogStream::eos;

	CountNewWatchList(player->GetID(), notify);
}


void Player::AddAllAroundPlayers()
{
	if (SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_NEW_SCENE_SYNC))
	{
		return;
	}

	m_aroundPlayerList.clear();
	m_watchRoleIdList.clear();
	m_watchRoleIdSet.clear();

	class AddAroundPlayerVisitor : public SceneObjectVisitor
	{
	public:
		AddAroundPlayerVisitor(Player* player) :m_player(player)
		{
		}

		bool operator()(SceneObject* obj)
		{
			if (!obj || obj->GetType() != SOT_PLAYER || obj == m_player)
			{
				return true;
			}

			Player* player = (Player*)obj;
			player->AddAroundPlayer(m_player, true);
			m_player->AddAroundPlayer((Player*)obj, false);
			return true;
		}

		Player* m_player;
	};

	AddAroundPlayerVisitor vistor(this);
	GetScene()->VisitObjectsAround(GetPos(), vistor);
}

void Player::DelAroundPlayer(Player* player, bool notify)
{
	if (SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_NEW_SCENE_SYNC))
	{
		return;
	}

	if (!player)
	{
		return;
	}

	//DebugLogStream << "player(" << GetAccID() << ") del around player(" << player->GetAccID() << ")." << LogStream::eos;

	for (auto itr = m_aroundPlayerList.begin(); itr != m_aroundPlayerList.end(); ++itr)
	{
		if (itr->id == player->GetID())
		{
			m_aroundPlayerList.erase(itr);
			CountNewWatchList(player->GetID(), notify);
			return;
		}
	}
}

void Player::DelAllAroundPlayers()
{
	m_watchRoleIdList.clear();
	m_aroundPlayerList.clear();
	m_watchRoleIdSet.clear();

	class DelAroundPlayerVisitor : public SceneObjectVisitor
	{
	public:
		DelAroundPlayerVisitor(Player* player) :m_player(player)
		{
		}

		bool operator()(SceneObject* obj)
		{
			if (!obj || obj->GetType() != SOT_PLAYER || obj == m_player)
			{
				return true;
			}

			Player* player = (Player*)obj;
			player->DelAroundPlayer(m_player, true);
			return true;
		}

		Player* m_player;
	};

	if (GetScene() == NULL)
	{
		return;
	}

	DelAroundPlayerVisitor vistor(this);
	GetScene()->VisitObjectsAround(GetPos(), vistor);
}

void Player::GetWatchPlayerList(std::vector<ObjID_t>& roleIdList, bool containSelf)
{
	class AroundPlayerVisitor : public SceneObjectVisitor
	{
	public:
		AroundPlayerVisitor(Player* player) : m_player(player) {}
		bool operator()(SceneObject* obj)
		{
			if (!obj || obj->GetType() != SOT_PLAYER)
			{
				return true;
			}

			Player* player = (Player*)obj;
			m_playerList.push_back(player);
			return true;
		}

		Player*					m_player;
		std::vector<Player*>	m_playerList;
	};

	AroundPlayerVisitor vistor(this);
	GetScene()->VisitObjectsAround(GetPos(), vistor);

	for (auto player : vistor.m_playerList)
	{
		if (!player)
		{
			continue;
		}

		if ((player == this && !containSelf) || (player != this && !player->IsWatchThisPlayer(GetID())))
		{
			continue;
		}

		roleIdList.push_back(player->GetID());
	}
}

bool Player::IsWatchThisPlayer(ObjID_t id)
{
	return std::binary_search(m_watchRoleIdSet.begin(), m_watchRoleIdSet.end(), id);
}

void Player::CountNewWatchList(ObjID_t id, bool notify)
{
	if (SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_NEW_SCENE_SYNC))
	{
		return;
	}

	std::vector<ObjID_t> oldList(m_watchRoleIdList);
	m_watchRoleIdSet.clear();
	m_watchRoleIdList.clear();

	for (auto itr : m_aroundPlayerList)
	{
		m_watchRoleIdList.push_back(itr.id);
		m_watchRoleIdSet.push_back(itr.id);

		if (m_watchRoleIdList.size() >= GetMaxWatchPlayerNum())
		{
			break;
		}
	}
	std::sort(m_watchRoleIdSet.begin(), m_watchRoleIdSet.end());

	if (!notify)
	{
		return;
	}

	// 新加入列表，删除被顶替掉的玩家
	auto itrInOldList = std::find(oldList.begin(), oldList.end(), id);
	auto itrInNewList = std::find(m_watchRoleIdList.begin(), m_watchRoleIdList.end(), id);
	if (itrInOldList == oldList.end() && itrInNewList != m_watchRoleIdList.end() && !oldList.empty())
	{
		auto lastPlayerId = oldList[oldList.size() - 1];
		if (std::find(m_watchRoleIdList.begin(), m_watchRoleIdList.end(), lastPlayerId) != m_watchRoleIdList.end())
		{
			return;
		}

		auto lastPlayer = PlayerMgr::Instance()->FindPlayer(lastPlayerId);
		if (lastPlayer)
		{
			Avalon::Packet* delpacket = lastPlayer->MakeDelObjectPacket();
			SendPacket(delpacket);
			Avalon::Packet::Destroy(delpacket);
		}
	}

	// 从列表中删除
	if (itrInOldList != oldList.end() && itrInNewList == m_watchRoleIdList.end() && !m_watchRoleIdList.empty())
	{
		auto newPlayerId = m_watchRoleIdList[m_watchRoleIdList.size() - 1];
		if (std::find(oldList.begin(), oldList.end(), newPlayerId) != oldList.end())
		{
			return;
		}

		auto newPlayer = PlayerMgr::Instance()->FindPlayer(newPlayerId);
		if (newPlayer)
		{
			Avalon::Packet* syncpacket = newPlayer->MakeSyncObjectPacket(false);
			SendPacket(syncpacket);
			Avalon::Packet::Destroy(syncpacket);
		}
	}
}

void Player::CountNewWatchList2(bool sync)
{
	if (SERVER_TYPE == ST_WSCENE)
	{
		if (GetScene() != NULL && GetScene()->GetDataEntry() && GetScene()->GetDataEntry()->subType == SCENE_SUB_SCORE_WAR)
		{
			CountNewWatchList_3V3(sync);
			return;
		}
	}
	

	auto oldList = m_watchRoleList;
	m_watchRoleList.clear();
	m_watchRoleIdList.clear();
	m_watchRoleIdSet.clear();

	do
	{
		auto scene = GetScene();
		if (!scene)
		{
			break;
		}

		if (scene->GetType() == SCENE_TYPE_SINGLE)
		{
			break;
		}

		class AroundObjectVisitor : public SceneObjectVisitor
		{
		public:
			AroundObjectVisitor(Player* player) :m_pPlayer(player)
			{
			}

			bool operator()(SceneObject* obj)
			{
				if (obj == m_pPlayer) return true;
				if (obj->GetID() == m_pPlayer->GetID())
				{
					return true;
				}

				auto player = (Player*)obj;

				auto scene = player->GetScene();
				if (!scene)
				{
					return true;
				}

				auto data = scene->GetDataEntry();
				if (!data)
				{
					return true;
				}

				// 如果是房间场景,只显示房间号一样的玩家
				if (data->subType == SCENE_SUB_ROOM && player->GetRoomId() != m_pPlayer->GetRoomId())
				{
					return true;
				}

				// 如果是公会场景,只显示公会一样的玩家
				if (data->subType == SCENE_SUB_GUILD && player->GetGuildId() != m_pPlayer->GetGuildId())
				{
					return true;
				}
				
				if (player->GetBattleID() != m_pPlayer->GetBattleID())
				{
					return true;
				}

				PlayerSightPriority priority;
				priority.id = obj->GetID();
				priority.isFriend = false;
				priority.level = player->GetLevel();

				playerList.insert(priority);

				return true;
			}

			//玩家
			Player*							m_pPlayer;
			std::set<PlayerSightPriority>	playerList;
		};

		AroundObjectVisitor vistor(this);
		scene->VisitObjectsAround(GetPos(), vistor);

		auto itr = vistor.playerList.begin();
		UInt32 num = 0;
		UInt32 maxNum = GetMaxWatchPlayerNum();
		while (itr != vistor.playerList.end() && num++ < maxNum)
		{
			m_watchRoleList.insert(itr->id);
			itr++;
		}
	} while (0);

	for (auto roleid : m_watchRoleList)
	{
		m_watchRoleIdList.push_back(roleid);
		m_watchRoleIdSet.push_back(roleid);
	}
	std::sort(m_watchRoleIdSet.begin(), m_watchRoleIdSet.end());

	// 不需要同步
	if (!sync)
	{
		return;
	}

	// 找到目前不在观察列表里的玩家
	do {
		std::vector<ObjID_t> delPlayerIds(GetMaxWatchPlayerNum());
		auto itr = std::set_difference(oldList.begin(), oldList.end(), m_watchRoleList.begin(), m_watchRoleList.end(), delPlayerIds.begin());
		delPlayerIds.resize(itr - delPlayerIds.begin());

		if (delPlayerIds.empty())
		{
			break;
		}

		Avalon::Packet* packet = Avalon::Packet::Create();
		packet->SetID(CLProtocol::SCENE_DELETE_SCENEOBJECT);
		Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
		try
		{
			for (auto roleid : delPlayerIds)
			{
				stream & roleid;
			}
			stream & 0;
			packet->SetSize(packet->GetSize() + UInt16(stream.GetPos()));
			SendPacket(packet);

		}
		catch (const Avalon::NetException& e)
		{
			ErrorLogStream<< "player(" << GetID() << ") encode del packet failed, "<< e.what() << LogStream::eos;
		}

		Avalon::Packet::Destroy(packet);
	} while (0);

	// 同步新加入列表的玩家
	{
		std::vector<ObjID_t> newPlayerIds(GetMaxWatchPlayerNum());
		auto itr = std::set_difference(m_watchRoleList.begin(), m_watchRoleList.end(), oldList.begin(), oldList.end(), newPlayerIds.begin());
		newPlayerIds.resize(itr - newPlayerIds.begin());

		for (auto roleid : newPlayerIds)
		{
			auto newPlayer = PlayerMgr::Instance()->FindPlayer(roleid);
			if (newPlayer)
			{
				Avalon::Packet* syncpacket = newPlayer->MakeSyncObjectPacket(false);
				SendPacket(syncpacket);
				Avalon::Packet::Destroy(syncpacket);
			}
		}
	}
}

UInt32 Player::GetMaxWatchPlayerNum()
{
	return 20;
}

void Player::CountNewWatchList_3V3(bool sync)
{
	auto oldList = m_watchRoleList;
	m_watchRoleList.clear();
	m_watchRoleIdList.clear();
	m_watchRoleIdSet.clear();

	do
	{
		auto scene = GetScene();
		if (!scene)
		{
			break;
		}

		class AroundObjectVisitor : public SceneObjectVisitor
		{
		public:
			AroundObjectVisitor(Player* player) :m_pPlayer(player)
			{
			}

			bool operator()(SceneObject* obj)
			{
				if (obj == m_pPlayer) return true;
				if (obj->GetID() == m_pPlayer->GetID())
				{
					return true;
				}

				auto player = (Player*)obj;

				
				playerList.push_back(player);

				return true;
			}

			//玩家
			Player*							m_pPlayer;
			std::vector<Player*>			playerList;
		};

		AroundObjectVisitor vistor(this);
		scene->VisitObjectsAround(GetPos(), vistor);

		UInt32 maxNum = GetMaxWatchPlayerNum();
		if (vistor.playerList.size() > maxNum)
		{
			std::random_shuffle(vistor.playerList.begin(), vistor.playerList.end());
		}

		// 计算新的列表
		for (UInt32 i = 0; i < vistor.playerList.size() && i < maxNum; i++)
		{
			auto roleid = vistor.playerList[i]->GetID();
			m_watchRoleIdList.push_back(roleid);
			m_watchRoleIdSet.push_back(roleid);
		}

		std::sort(m_watchRoleIdSet.begin(), m_watchRoleIdSet.end());
	} while (0);

	// 不需要同步
	if (!sync)
	{
		return;
	}

	// 找到目前不在观察列表里的玩家
	do {
		std::vector<ObjID_t> delPlayerIds(GetMaxWatchPlayerNum());
		auto itr = std::set_difference(oldList.begin(), oldList.end(), m_watchRoleList.begin(), m_watchRoleList.end(), delPlayerIds.begin());
		delPlayerIds.resize(itr - delPlayerIds.begin());

		if (delPlayerIds.empty())
		{
			break;
		}

		Avalon::Packet* packet = Avalon::Packet::Create();
		packet->SetID(CLProtocol::SCENE_DELETE_SCENEOBJECT);
		Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
		try
		{
			for (auto roleid : delPlayerIds)
			{
				stream & roleid;
			}
			stream & 0;
			packet->SetSize(packet->GetSize() + UInt16(stream.GetPos()));
			SendPacket(packet);

		}
		catch (const Avalon::NetException& e)
		{
			ErrorLogStream << "player(" << GetID() << ") encode del packet failed, " << e.what() << LogStream::eos;
		}

		Avalon::Packet::Destroy(packet);
	} while (0);

	// 同步新加入列表的玩家
	{
		std::vector<ObjID_t> newPlayerIds(GetMaxWatchPlayerNum());
		auto itr = std::set_difference(m_watchRoleList.begin(), m_watchRoleList.end(), oldList.begin(), oldList.end(), newPlayerIds.begin());
		newPlayerIds.resize(itr - newPlayerIds.begin());

		for (auto roleid : newPlayerIds)
		{
			auto newPlayer = PlayerMgr::Instance()->FindPlayer(roleid);
			if (newPlayer)
			{
				Avalon::Packet* syncpacket = newPlayer->MakeSyncObjectPacket(false);
				SendPacket(syncpacket);
				Avalon::Packet::Destroy(syncpacket);
			}
		}
	}
}

UInt32 Player::BeginQuickBuyTransaction(QuickBuyTransaction* trans)
{
	DebugLogStream << "player(" << GetID() << ") start quick buy transaction." << LogStream::eos;

	if (!trans)
	{
		return ErrorCode::QUICK_BUY_SYSTEM_ERROR;
	}

	if (m_quickBuyTrans)
	{
		delete trans;
		return ErrorCode::QUICK_BUY_LAST_TRANS_NOT_FINISH;
	}

	m_quickBuyTrans = trans;

	// 开始执行事务，如果失败了，会在OnFailure中返回消息给客户端，所以这里直接返回成功
	if (!m_quickBuyTrans->BeginTransaction(this))
	{
		delete trans;
		m_quickBuyTrans = NULL;
	}

	return ErrorCode::SUCCESS;
}

void Player::OnQuickBuyTransactionRes(UInt32 id, UInt32 result)
{
	if (!m_quickBuyTrans)
	{
		return;
	}

	if (m_quickBuyTrans->GetID() != id)
	{
		ErrorLogStream << "player(" << GetID() << ") invalid quick buy transaction(" << id << ") current transaction(" << m_quickBuyTrans->GetID() << ")." << LogStream::eos;
		return;
	}

	m_quickBuyTrans->OnFinish(this, result);
	delete m_quickBuyTrans;
	m_quickBuyTrans = NULL;
}

void Player::OnOhterPlayerWatch(PlayerWatchInfo& info)
{
	info.id = GetID();
	info.name = GetName();
	info.occu = GetOccu();
	info.level = GetLevel();
	info.vipLevel = GetVipLvl();
	info.guildTitle.guildName = GetGuildName();
	info.guildTitle.guildPost = GetGuildPost();
	info.emblemLevel = GetGuildEmblem();

	std::vector<Item*> equips;
	GetEquips(equips);
	for (auto equip : equips)
	{
		ItemBaseInfo item;
		item.id = equip->GetID();
		item.pos = equip->GetPos().gridIndex;
		item.typeId = equip->GetDataID();
		item.strengthen = equip->GetStrengthen();

		if (equip->GetPos().pack == PACK_WEAR)
		{
			info.equips.push_back(item);
		}
		else
		{
			info.fashionEquips.push_back(item);
		}

		// 生成物品缓存
		WorldCacheItemData(equip);
	}

	// 随从
	auto retinue = m_RetinueMgr.FindRetinue(m_RetinueMgr.GetRetinueFighting());
	if (retinue)
	{
		info.retinue.id = retinue->GetID();
		info.retinue.dataId = retinue->GetDataId();
		info.retinue.level = retinue->GetLevel();
		info.retinue.skills = retinue->GetSkills();
	}

	// 战绩
	auto pkInfo = m_pkStatisticMgr.FindPkStatistic(PK_SEASON_1V1);
	if (pkInfo)
	{
		info.pkInfo.totalWinNum = pkInfo->GetTotalWinNum();
		info.pkInfo.totalLoseNum = pkInfo->GetTotalLoseNum();
		info.pkInfo.totalNum = pkInfo->GetTotalNum();
	}

	info.pkValue = GetPkValue();
	info.matchScore = GetMatchScore();
	info.seasonLevel = GetSeasonLevel();
	info.seasonStar = GetSeasonStar();
	GetPetBaseInfos(info.pets);
	info.avatar = GetAvatar();
	info.activeTimeType = GetActiveTimeType();
	info.masterType = GetMasterType();
	info.regionId = GetRegionId();
	info.declaration = GetDeclaration();
	GetPlayerLabelInfo(info.playerLabelInfo);
}

//added by aprilliu
void Player::HandleProtocol(CLProtocol::SceneAcceptTask& request)
{
	if (TASK_PUBLISH_NPC ==	request.publishType ) //npc接取，
	{
		if (!request.publishId)
		{
			DebugLogStream << "in Player::HandleProtocol(CLProtocol::SceneAcceptTask& request), publish type:" << (UInt16)request.publishType
				<< ", and npc:" << request.publishId << " so just discard!" << LogStream::eos;
			return;
		}
		
		if (!CheckPublishTask(request.publishId, request.taskId))
		{
			DebugLogStream << "in Player::HandleProtocol(CLProtocol::SceneAcceptTask& request), publish type:" << (UInt16)request.publishType
				<< ", and npc(" << request.publishId << ") has not task( " << request.taskId << ") so just discard!"  << LogStream::eos;
			return;
		}
	}

	//
	AcceptTask(request.taskId, 0, false);
}

//added by aprilliu
void Player::HandleProtocol(CLProtocol::SceneSubmitTask& request)
{
	TaskDataEntry* dataEntry = TaskDataEntryMgr::Instance()->FindEntry(request.taskId);
	if (dataEntry != NULL && dataEntry->type == TASK_CYCLE)
	{
		BaseTask* task = m_CycleTaskMgr.FindTask(request.taskId);
		if (task != NULL)
		{
			m_CycleTaskMgr.SubmitTask(task);
			return;
		}
	}

	if (TASK_SUBMIT_NPC == request.submitType) //npc提交
	{
		if (!request.npcId)
		{
			DebugLogStream << "in Player::HandleProtocol(CLProtocol::SceneSubmitTask& request), player(role:" << GetID() << ", accId:" << m_AccId
				<< ") request submit task(" << request.taskId << ") to npc(" << request.npcId << ") with submit type("
				<< (UInt16)request.submitType << "), just discard!" << LogStream::eos;
			return;
		}

		if (!CheckSubmitTask(request.npcId, request.taskId))
		{
			DebugLogStream << "in Player::HandleProtocol(CLProtocol::SceneSubmitTask& request), player(role:" << GetID() << ", accId:" << m_AccId
				<< ") request submit task(" << request.taskId << ") to npc(" << request.npcId << ") with submit type("
				<< (UInt16)request.submitType << "), but can't submit, just discard!" << LogStream::eos;
			return;
		}
	}

	SubmitTask(request.taskId);
}

void Player::AddRedPacket(UInt16 reason)
{
	CLProtocol::SceneAddRedPacketReq req;
	req.id = GetID();
	req.reason = reason;

	Router::SendToWorld(req);
}

void Player::OnNewYearRedPacket(UInt32 num)
{
	UInt32 oldNum = GetCounter(NEW_YEAR_RED_PACKET_NUM);
	UInt32 itemId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_NEW_YEAR_RED_PACKET_REWARD_ID);
	UInt32 needPacketNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_NEW_YEAR_RED_PACKET_REWARD_NEED_NUM);
	UInt32 itemNum = num / needPacketNum;
	ItemReward itemReward(itemId, itemNum, 1, 0);

	std::vector<ItemReward> itemRewards;
	itemRewards.push_back(itemReward);

	RewardGroup* rewardGroup = GetRewardGroup();
	rewardGroup->AddRewards(itemRewards);
	AddRewards(GetReason(LogSourceType::SOURCE_TYPE_RED_PACKET).c_str(), rewardGroup, true);

	UInt32 newNum = oldNum + num;

	SetCounter(NEW_YEAR_RED_PACKET_NUM, newNum);

	SortListMgr::Instance()->OnSortValueChanged(SortListType::SORTLIST_NEW_YEAR_RED_PACKET, GetID(), GetName(), 0, GetOccu(), 0, GetLevel(), newNum, 0, 0);

	SyncRedPacketInfoToCenter();
}

void Player::SyncRedPacketInfoToCenter()
{
	UInt32 totalMoney = GetCounter(NEW_YEAR_RED_PACKET_NUM);
	if (totalMoney <= 0)
	{
		return;
	}

	CLProtocol::CenterSyncRedPacketInfo stReq;
	stReq.zoneId = ZONE_ID;
	stReq.accId = GetAccID();
	stReq.roleUid = GetID();
	stReq.totalMoney = totalMoney;
	stReq.roleName = GetName();
	stReq.serverName = SERVER_NAME;

	Router::SendToCenter(stReq);
}

void Player::OnPremiumLeagueEnroll()
{
	InfoLogStream << "player(" << GetID() << ") premium league enroll." << LogStream::eos;

	UInt32 rewardItemId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_PREMIUM_LEAGUE_ENROLL_REWARD_ITEM);
	UInt32 rewardItemNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_PREMIUM_LEAGUE_ENROLL_REWARD_ITEM_NUM);

	ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(rewardItemId);
	if (dataEntry == NULL)
	{
		ErrorLogStream << "player(" << GetID() << ") invalid reward item(" << rewardItemId << ")." << LogStream::eos;
		return;
	}

	auto reason = GetReason(SOURCE_TYPE_PREMIUM_LEAGUE_ENROLL);
	if (!GetItemMgr().CheckAddItem(rewardItemId, rewardItemNum, (ItemType)dataEntry->type, 0))
	{
		InfoLogStream << "player(" << GetID() << ") send premium league enroll reward(" << rewardItemId << ", " << rewardItemNum << ") to mail." << LogStream::eos;

		// 发邮件
		RewardGroup group;
		group.AddReward(rewardItemId, rewardItemNum);

		auto sender = SysNotifyMgr::Instance()->MakeString(1258);
		auto title = SysNotifyMgr::Instance()->MakeString(9308);
		auto content = SysNotifyMgr::Instance()->MakeString(9309);
		Sys::SendSysMail(GetID(), sender.c_str(), title.c_str(), content.c_str(), reason.c_str(), &group);
		return;
	}

	AddItem(reason.c_str(), rewardItemId, rewardItemNum);

	InfoLogStream << "player(" << GetID() << ") send premium league enroll reward(" << rewardItemId << ", " << rewardItemNum << ") to bag." << LogStream::eos;
}

UInt32 Player::GiveChargeGoods(const CLProtocol::SceneBillingSendGoods& goods)
{
	InfoLogStream << "player(" << GetID() << ", " << GetAccID() << ") start to give charge goods, order(" << goods.orderId << ") money(" << goods.chargeMoney << ") vipscore(" << goods.vipScore << ")." << LogStream::eos;
	
	// 根据world调整玩家的点劵等信息
	if (SSApplication::Instance()->UseNewOfflineCharge())
	{
		if (goods.onlinePlayerID != GetID())
		{
			if (GetVipLvl() != goods.vipLevel || GetVipExp() != goods.vipExp)
			{
				InfoLogStream << PLAYERINFO(this) << " vip from (" << GetVipLvl() << ", " << GetVipExp() << ") to (" << goods.vipLevel << ", " << goods.vipExp << ")." << LogStream::eos;
				InitVip(goods.vipLevel, goods.vipExp);
			}

			if (m_TotalChargeNum != goods.totalChargeNum)
			{
				InfoLogStream << PLAYERINFO(this) << " totalcharge from (" << m_TotalChargeNum << ") to (" << goods.totalChargeNum << ")." << LogStream::eos;
				InitTotalChargeNum(goods.totalChargeNum);
			}

			if (GetPoint() != goods.point)
			{
				InfoLogStream << PLAYERINFO(this) << " point from (" << GetPoint() << ") to (" << goods.point << ")." << LogStream::eos;
				InitPoint(goods.point);
			}
		}
	}
	
	// 添加道具
	UInt32 monthCardItemId = 0;
	UInt32 moneyManageCardItemId = 0;
	RewardGroup group;
	for (auto reward : goods.rewards)
	{
		auto itemId = reward.id;
		auto num = reward.num;

		auto itemData = ItemDataEntryMgr::Instance()->FindEntry(itemId);
		if (!itemData)
		{
			ErrorLogStream << "player(" << GetID() << ", " << GetAccID() << ") order(" << goods.orderId << ") item(" << itemId << ") not exist." << LogStream::eos;
			return ErrorCode::BILLING_GOODS_UNEXIST;
		}

		if (itemData->subType == ST_MONTH_CARD)
		{
			monthCardItemId = itemId;
		}
		else if (itemData->subType == ST_MONEY_MANAGE_CARD)
		{
			moneyManageCardItemId = itemId;
		}
		else
		{
			group.AddReward(itemId, num);
		}
	}

	//通知购买商城礼包
	if (goods.mallType == CHARGE_MALL_PACKET)
	{
		BuyMallGiftParam param(goods.chargeGoodsId);
		CLEventMgr::Instance()->SendPlayerEvent(GetID(), CLEventMgr::ET_BUY_MALL_GIFTPACK, &param);
	}

	auto reason = GetReason(SOURCE_TYPE_BILLING_GOODS, goods.chargeMoney);
	if (monthCardItemId)
	{
		// 如果是月卡，计算月卡权益
		auto reason = GetReason(SOURCE_TYPE_BILLING_GOODS, goods.chargeMoney, monthCardItemId);
		OnBuyMonthCard(reason.c_str());
	}
	else if (moneyManageCardItemId)
	{
		// 如果是理财卡
		auto reason = GetReason(SOURCE_TYPE_BILLING_GOODS, goods.chargeMoney, moneyManageCardItemId);
		OnBuyMoneyManageCard(reason.c_str(), false);
	}
	else
	{
		AddRewards(reason.c_str(), &group, true);
	}
	
	if (goods.vipScore > 0)
	{
		_OnCharge(reason.c_str(), goods.vipScore, goods.chargeMoney);
	}

	CLProtocol::SceneBillingSendGoodsNotify notify;
	SendProtocol(notify);

	SendPayEndUdpLog(goods.orderId, goods.mallType, goods.chargeGoodsId, goods.chargeItemId, goods.chargeItemNum, goods.vipScore, goods.chargeMoney, goods.time, goods.sendGoodTimeoutTime);


	// 如果当前在线的玩家不是充值的玩家，根据充值之后的值修改当前在线的玩家的点劵等信息
	if (SSApplication::Instance()->UseNewOfflineCharge())
	{
		if (goods.onlinePlayerID != GetID())
		{
			auto onlinePlayer = PlayerMgr::Instance()->FindNormalPlayer(goods.onlinePlayerID);
			if (onlinePlayer)
			{
				onlinePlayer->AdjestChargeInfo(GetVipLvl(), GetVipExp(), GetPoint(), m_TotalChargeNum);
			}
		}
	}

	if (SSApplication::Instance()->OpenSaveDataAfterCharge())
	{
		CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand(GetSrcZoneId(), "t_player_info", GetID(), false);
		GetDBData(cmd, true);

		if (SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_SET_PLAYER_SAVE_BACK_TIME))
		{
			// 如果当前玩家在线，就设置saveback时间
			if (goods.onlinePlayerID == GetID())
			{
				SetSavebackTime(CURRENT_TIME.MSec());
				cmd->PutData("savetime", GetSavebackTime());
				InfoLogStream << PLAYERINFO(this) << " set save back time(" << GetSavebackTime() << ")." << LogStream::eos;
			}
		}

		CLRecordClient::Instance()->SendCommand(cmd);
		InfoLogStream << PLAYERINFO(this) << " save dirty data..." << LogStream::eos;
	}

	return ErrorCode::SUCCESS;
}

void Player::OnBuyMonthCard(const char* reason)
{
	/*
		购买月卡：
			1. 计算月卡到期时间
			2. 月卡福利
				1. 添加每日体力上限（20点）
				2. 每日领取绑点活动
			3. 获得300点劵
	*/

	// 计算月卡到期时间
	bool isRenew = !HasMonthCard();
	UInt32 validDay = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_MONTH_CARD_VALID_DAY);
	if (HasMonthCard())
	{
		m_MonthCardExpireTime = m_MonthCardExpireTime + validDay * Avalon::Time::SECS_OF_DAY;
	}
	else
	{
		Avalon::Date date = CURRENT_DATE;
		if (CURRENT_DATE.Hour() < 6)
		{
			date = Avalon::Date(Avalon::Time(CURRENT_TIME.MSec() - Avalon::Time::MSECS_OF_DAY));
			date.Hour(6);
			date.Min(0);
			date.Sec(0);
			date.MSec(0);
		}
		else
		{
			date.Hour(6);
			date.Min(0);
			date.Sec(0);
			date.MSec(0);
		}

		m_MonthCardExpireTime = (UInt32)date.ToTime().Sec() + validDay * Avalon::Time::SECS_OF_DAY;
	}

	//SyncAccountInfo(ACCOUNT_MONTH_CARD_EXPIRE_TIME, m_MonthCardExpireTime);

	// 月卡福利
	auto remainDays = GetMonthCardRemainDays();
	OnMonthCardWelfare(remainDays, isRenew);

	// 保存到数据库
	auto cmd = CLRecordClient::Instance()->PrepareUpdateCommand(0, "t_player_info", GetID(), true);
	cmd->PutData("month_card_expire_time", m_MonthCardExpireTime);
	cmd->PutData("fatigue", m_Fatigue.GetDBValue());
	CLRecordClient::Instance()->SendCommand(cmd);

	// 获得300点劵
	//AddPoint(reason, SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_MONTH_CARD_ADD_POINT));

	SyncBaseInfoToWorld();

	SendWelfareCardUdpLog(ItemSubType::ST_MONTH_CARD);
}

void Player::OnMonthCardWelfare(UInt32 remainDays, bool isRenew, bool bDayChange, bool isOnline)
{
	/*
		月卡福利：
			1. 添加每日体力上限（20点）
			2. 每日领取绑点活动 
	*/
	// ToDo...
	if (HasMonthCard())
	{
		UInt16 maxFatigue = (UInt16)SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_FATIGUE_MAX);
		maxFatigue += (UInt16)SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_MONTH_CARD_ADD_FATIGUE);
		m_Fatigue.SetMaxFatigue(maxFatigue);

		//判断是否加过疲劳
		bool isInc = (GetCounter(COUNTER_IS_INC_MONTH_CARD_FATIGUE) != 0); //modified by aprilliu 2017.04.22
		if (!isInc)
		{
			m_Fatigue.AddFatigue(GetReason(LogSourceType::SOURCE_TYPE_MONTH_CARD).c_str(), (UInt16)SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_MONTH_CARD_ADD_FATIGUE));
			SetCounter(COUNTER_IS_INC_MONTH_CARD_FATIGUE, 1);
		}

	}
}

void Player::InitMonthCardExpireTime(UInt32 time)
{
	m_MonthCardExpireTime = time;
}

bool Player::HasMonthCard() const
{
	return (UInt32)CURRENT_TIME.Sec() <= m_MonthCardExpireTime;
}

UInt32 Player::GetMonthCardRemainDays() const
{
	return Avalon::TimeUtil::CountIntervalDays((UInt32)CURRENT_TIME.Sec(), m_MonthCardExpireTime, 6);
}

UInt32 Player::GetTotChargeNum() const
{
	return m_TotalChargeNum;
}

UInt32 Player::GetDayChargeNum() const
{
	return m_DayChargeNum;
}

void Player::AddPlayerChargeTotal(UInt32 deltaCharge)
{
	if (deltaCharge == 0)
	{
		return;
	} 

	UInt32 oldNum = m_TotleChargeNum;
	m_TotleChargeNum = m_TotleChargeNum + deltaCharge;

	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand(GetSrcZoneId(), "t_player_info", GetID(), true);
	cmd->PutData("totlechargenum", m_TotleChargeNum);
	CLRecordClient::Instance()->SendCommand(cmd);

	ActivateCondCheckEvent::Instance()->OnActivate(this, CAT_MALL_PERSONAL_TAILOR, CAST_CHARGE_NUM);

	if (GetLevel() >= (UInt16)PLL_FIFTY)
	{
		ActivateCondCheckEvent::Instance()->OnActivate(this, CAT_MALL_PERSONAL_TAILOR, CAST_PLAYER_LEVEL);
	}

	SyncBaseInfoToWorld();
	DebugLogStream << PLAYERINFO(this) << ",deltaCharge = " << deltaCharge << ",oldNum = " << oldNum << ",m_TotleChargeNum = " << m_TotleChargeNum << LogStream::eos;
}

UInt32 Player::GetPlayerChargeTotal() const
{
	return m_TotleChargeNum;
}

void Player::OnBuyMoneyManageCard(const char* reason, bool isSendPoint)
{
	if (isSendPoint)
	{
		AddPoint(reason, SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_MONEY_MANAGE_CARD_ADD_POINT));
	}

	if (m_MoneyManageAccountStatus == MoneyManageStatus::MMS_ACCOUNT_BUY)
	{
		return;
	}

	//m_MoneyManageStatus = MoneyManageStatus::MMS_PLAYER_BUY;
	m_MoneyManageStatus = MoneyManageStatus::MMS_NEW_PLAYER_BUY;

	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand(GetSrcZoneId(), "t_player_info", GetID(), true);
	cmd->PutData("money_manage_status", m_MoneyManageStatus);
	CLRecordClient::Instance()->SendCommand(cmd);

	m_MoneyManageAccountStatus = MoneyManageStatus::MMS_ACCOUNT_BUY;
	//SyncBaseInfoToWorld();
	SyncAccountInfo(ACCOUNT_MONEY_MANAGE_STATUS, m_MoneyManageStatus);

	SendWelfareCardUdpLog(ItemSubType::ST_MONEY_MANAGE_CARD);

	//播放公告
	std::string systemLinkStr;
	MsgPackage::GetSystemSuffixesMsgTag(systemLinkStr, SystemSuffixesType::SYSTEM_SUFFIXES_TYPE_MONEY_MANAGE, 0);
	std::string playerLinkStr;
	if (!MsgPackage::GetPlayerMsgTag(playerLinkStr, GetID(), GetName(), GetOccu(), GetLevel()))
	{
		playerLinkStr = GetName();
	}
	Sys::SendAnnouncement(73, playerLinkStr.c_str(), systemLinkStr.c_str());

}

void Player::InitMoneyManage(UInt8 status)
{
	m_MoneyManageAccountStatus = status;
}

UInt32 Player::GiveMoneyManageReward(UInt32 rewardId)
{
	MoneyManageDataEntry* dataEntry = MoneyManageDataEntryMgr::Instance()->FindEntry(rewardId);
	if (dataEntry == NULL)
	{
		return ErrorCode::PLAYER_MONEY_MANAGE_REWARD_NOT_FOUNT;
	}

	if (m_MoneyManageStatus != MoneyManageStatus::MMS_PLAYER_BUY && m_MoneyManageStatus != MoneyManageStatus::MMS_NEW_PLAYER_BUY)
	{
		return ErrorCode::PLAYER_MONEY_MANAGE_REWARD_ROLE_ERROR;
	}

	if (GetLevel() < dataEntry->level && dataEntry->level != 0)
	{
		return ErrorCode::PLAYER_MONEY_MANAGE_LEVEL_ERROR;
	}

	if (dataEntry->level == 0 && m_MoneyManageStatus == MoneyManageStatus::MMS_PLAYER_BUY)
	{
		return ErrorCode::PLAYER_MONEY_MANAGE_REWARD_ROLE_ERROR;
	}

	if (m_MoneyManageRewardMask.CheckMask(rewardId))
	{
		return ErrorCode::PLAYER_MONEY_MANAGE_REWARD_EXIST;
	}

	RewardGroup* rewards = GetRewardGroup();
	rewards->AddRewards(dataEntry->itemRewards);
	UInt32 result = CheckAddRewards(rewards);
	if (result == ErrorCode::SUCCESS)
	{
		m_MoneyManageRewardMask.SetMask(rewardId);
		AddRewards(GetReason(LogSourceType::SOURCE_TYPE_MONEY_MANAGE, rewardId).c_str(), rewards, true);
	}
	return result;
}

void Player::UpdateMoneyManage()
{
	if (m_MoneyManageStatus == MoneyManageStatus::MMS_NOT_BUY)
	{
		m_MoneyManageStatus = m_MoneyManageAccountStatus;
	}
	else
	{
		//处理之前的老数据,老账号不能领取新的初始化奖励 by huchenhui 20190327
		if (m_MoneyManageStatus == MoneyManageStatus::MMS_PLAYER_BUY)
		{
			MoneyManageDataEntry* data = MoneyManageDataEntryMgr::Instance()->GetInitMoneyManage();
			if (data != NULL)
			{
				if (!m_MoneyManageRewardMask.CheckMask(data->id))
				{
					m_MoneyManageRewardMask.SetMask(data->id);
				}
			}
		}
	}
}

//检查npc上有无该任务可接
bool Player::CheckPublishTask(UInt32 npcID, UInt32 taskID)
{
	//TODO: 应该首先检查 有无此npc

	TaskDataEntryMgr::TaskIdVec acceptTasks;
	TaskDataEntryMgr::Instance()->GetAcceptTasks(npcID, acceptTasks);
	for (TaskDataEntryMgr::TaskIdVec::const_iterator iter = acceptTasks.begin();
	iter != acceptTasks.end(); ++iter)
	{
		if (*iter == taskID) 
		{
			return true;
		}
	}

	return false;
}

bool Player::CheckSubmitTask(UInt32 npcID, UInt32 taskID)
{
	TaskDataEntryMgr::TaskIdVec submitTasks;
	TaskDataEntryMgr::Instance()->GetSubmitTasks(npcID, submitTasks);
	for (TaskDataEntryMgr::TaskIdVec::const_iterator iter = submitTasks.begin();
	iter != submitTasks.end(); ++iter)
	{
		if (*iter == taskID)
		{
			return true;
		}
	}

	return false;
}

UInt32 Player::SealEquip(ObjID_t equipUid)
{
	Item* equip = FindItem(equipUid);
	if (!equip)
	{
		SendNotify("equip is null, uid = {0}", equipUid);
		return ITEM_DATA_INVALID;
	}
		

	UInt32 code = m_ItemMgr.SealEquip(equip);

	InfoLogStream << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") SealEquip uid:"
		<< equip->GetID() << "|name:" << equip->GetDataEntry()->name << "|code:" << code << LogStream::eos;

	return code;
	
}

void Player::SendMagicJarResult(const char* bonusPool, UInt32 getBonusID, UInt16 getNum)
{
// 	std::string bp(bonusPool);
// 	std::vector<std::string> strIds;
// 	Avalon::StringUtil::Split(bp, strIds, "|");
// 
// 	std::vector<UInt32> ids;
// 	for (size_t i = 0; i < strIds.size(); ++i)
// 	{
// 		ids.push_back((UInt32)atoi(strIds[i].c_str()));
// 	}
// 
// 	CLProtocol::SceneUseMagicJarRet ret;
// 
// 	ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(getBonusID);
// 	if (dataEntry == NULL)
// 	{
// 		SendNotify("item data is null, key = {0}", getBonusID);
// 
// 		ret.code = ITEM_DATA_INVALID;
// 		ret.bonusPool = ids;
// 		ret.getItemID = getBonusID;
// 		ret.getNum = getNum;
// 		SendProtocol(ret);
// 		return;
// 	}
// 
// 	UInt16 addNum = AddItem("openMagciJar", getBonusID, getNum, 0, 0, false);
// 	if (addNum != getNum)
// 	{
// 		SendNotify("additem num != getnum, addnum={0}, getnum={1}", addNum, getNum);
// 
// 		ret.code = ITEM_DATA_INVALID;
// 		ret.bonusPool = ids;
// 		ret.getItemID = getBonusID;
// 		ret.getNum = getNum;
// 		SendProtocol(ret);
// 		return;
// 	}
// 
// 	ret.code = SUCCESS;
// 	ret.bonusPool = ids;
// 	ret.getItemID = getBonusID;
// 	ret.getNum = getNum;
// 	SendProtocol(ret);
// 
// 	OnOpenMagicJar();
}

UInt32 Player::CheckSealEquip(ObjID_t equipUid, UInt16& needNum, UInt32& needId)
{
	Item* equip = FindItem(equipUid);
	if (!equip)
	{
		SendNotify("equip is null, uid={0}", equipUid);
		return ITEM_DATA_INVALID;
	}
		

	UInt32 code = m_ItemMgr.CheckSealEquip(equip, needNum, needId);

	InfoLogStream << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") CheckSealEquip uid:"
		<< equip->GetID() << "|name:" << equip->GetDataEntry()->name << "|needMatNum:" << needNum << "|code:" << code << LogStream::eos;

	return code;
}

UInt32 Player::ReMakeEquipQlv(ObjID_t equipUid, UInt8 autoUsePoint, UInt8 usePerfect)
{
	Item* equip = FindItem(equipUid);
	if (!equip)
	{
		SendNotify("equip is null, uid={0}", equipUid);
		return ITEM_DATA_INVALID;
	}
	
	if (equip->IsItemLock(ILT_LEASE_LOCK))
	{
		return ITEM_IN_LEASING;
	}

	UInt32 code = ITEM_DATA_INVALID;
	if (usePerfect > 0)
	{
		code = m_ItemMgr.ReMakeEquipQlvUsePerfect(equip);
	}
	else
	{
		code = m_ItemMgr.ReMakeEquipQlv(equip, autoUsePoint);
	}

	if (code == SUCCESS)
	{
		//计算装备评分
		GetItemMgr().CalculateValuedScore(equip);
		if (equip->IsItemLock(ILT_AUCTION_RETURN_LOCK))
		{
			equip->ClearItemLock(ILT_AUCTION_RETURN_LOCK);
		}
	}

	return code;
}

UInt32 Player::FashionCompose(ObjID_t fashionA, ObjID_t fashionB)
{
	Item* fa = FindItem(fashionA); 
	Item* fb = FindItem(fashionB);
	if (!fa || !fb)
	{
		DebugLogStream << PLAYERINFO(this) << " fashion item is null! fashionA uid = " << fashionA << " fashionB uid = " << fashionB << LogStream::eos;
		return ITEM_DATA_INVALID;
	}

	return m_ItemMgr.FashionCompose(fa, fb);
}

void Player::SendFashionComposeBack(UInt32 code, UInt8 type, UInt32 commonId, UInt32 advanceId, UInt32 wingId)
{
	CLProtocol::SceneFashionComposeRet ret;
	ret.code = code;
	ret.type = type;
	ret.itemAId = commonId;
	ret.numA = 1;
	ret.itemBId = advanceId;
	ret.numB = 1;
	ret.wingId = wingId;
	SendProtocol(ret);
}

void Player::SendFashionCompBackChangeActive(UInt32 ret, UInt8 type, UInt32 commonId, UInt32 advanceId, bool allMerged)
{
	CLProtocol::SceneFashionChangeActiveMergeRet protocl;
	protocl.ret = ret;
	protocl.type = type;
	protocl.commonId = commonId;
	protocl.advanceId = advanceId;
	protocl.allMerged = (UInt8)allMerged;
	SendProtocol(protocl);
}

UInt32 Player::EuqipMake(UInt32 equipId)
{
	return m_ItemMgr.EuqipMake(equipId);
}

UInt32 Player::AddMagic(Item* card, Item* item)
{
	if (!card || !item)
		return ITEM_DATA_INVALID;

	auto data = card->GetDataEntry();
	UInt32 code = m_ItemMgr.AddMagic(card, item);

	if (code == ErrorCode::SUCCESS)
	{
		OnAddMagic(data, item);
	}

	return code;
}

UInt32 Player::ComposeMagicCard(ObjID_t card_1, ObjID_t card_2, UInt32& cardId)
{
	Item* card1 = FindItem(card_1);
	Item* card2 = FindItem(card_2);
	if (!card1 || !card2)
		return ITEM_DATA_INVALID;

	UInt32 code = m_ItemMgr.ComposeMagicCard(card1, card2, cardId);

	if (code == ErrorCode::SUCCESS)
	{
		OnAddMagicCard();
	}

	SendComposeMagicCardUdpLog(card1->GetDataID(), card2->GetDataID(), cardId, code);

	InfoLogStream << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") ComposeMagicCard Card1:"
		<< card_1 << "|Card2:" << card_2 << "|code:" << code << LogStream::eos;

	return code;
}

UInt32 Player::AddPreciousBeadToItem(Item* preciousBead, Item* item)
{
	return m_ItemMgr.AddPreciousBead(preciousBead, item);
}

bool Player::SubmitActiveTask(UInt32 id)
{
	return false;
}

bool Player::AcceptActiveTask(UInt32 id)
{
	return false;
}

bool Player::RemoveActiveTask(UInt32 id)
{
	return false;
}

void Player::OnActiveClosed(UInt32 id)
{
	UInt32 startActivityId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_WUDAO_ACTIVITY_ID_BEGIN);
	UInt32 endActivityId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_WUDAO_ACTIVITY_ID_END);

	if (id >= startActivityId && id <= endActivityId)
	{
		CheckWudaodahuiStatus(false);
	}
	return;
}

bool Player::RemovePointBoth(const char* reason, UInt32 num)
{
	if (reason == NULL) return false;
	if (GetBindPoint() >= num)
	{
		RemoveBindPoint(reason, num);
		return true;
	}
	else if (GetBindPoint() > 0)
	{
		num = num - GetBindPoint();
		RemoveBindPoint(reason, GetBindPoint());
		RemovePoint(reason, num);
		return true;
	}
	else
	{
		RemovePoint(reason, num);
		return true;
	}
	return false;
}

UInt64 Player::GetOfflineTime()
{
	Avalon::Time lastTime = Avalon::Time(GetSavebackTime());
	if (0 == lastTime.MSec())
		return 0;

	UInt64 deltaT = CURRENT_TIME.MSec() - lastTime.MSec();
	return deltaT;
}

UInt32 Player::GetOfflineDay()
{
	Avalon::Time lastTime = Avalon::Time(GetSavebackTime());
	if (0 == lastTime.MSec())
		return 0;

	if (lastTime > CURRENT_TIME.MSec())
		return 0;

	UInt64 deltaT = CURRENT_TIME.MSec() - GetDayBeginTime(lastTime);
	
	UInt32 day = (UInt32)(deltaT / (24 * 3600 * 1000));
	return day;
}

UInt32 Player::GetOfflineDay(UInt32 Hour)
{
	UInt64 lastTime = GetSavebackTime();
	if (0 == lastTime)
		return 0;

	if (lastTime > CURRENT_TIME.MSec())
		return 0;

	Avalon::Date date = CURRENT_DATE;
	if (CURRENT_DATE.Hour() < 6)
	{
		date = Avalon::Date(Avalon::Time(CURRENT_TIME.MSec() - 24 * 3600 * 1000));
		date.Hour(Hour);
		date.Min(0);
		date.Sec(0);
		date.MSec(0);
	}
	else
	{
		date.Hour(Hour);
		date.Min(0);
		date.Sec(0);
		date.MSec(0);
	}

	if (date.ToTime().MSec() < lastTime)
	{
		return 0;
	}

	UInt64 deltaT = date.ToTime().MSec() - lastTime;

	UInt32 day = (UInt32)(deltaT / (24 * 3600 * 1000));
	
	return day + 1;
}

UInt32 Player::GetDayOnlineTime()
{
	if (m_CounterMgr.GetCounter(DAY_ONLINE_TIME) == 0)
	{
		m_DayOnlineTime = 0;
	}

	return m_DayOnlineTime;
}

UInt32 Player::GetIntervalDay(UInt64 startTime, UInt64 endTime, UInt32 hour)
{
	UInt64 lastTime = startTime;
	if (0 == lastTime)
		return 0;

	
	if (lastTime > endTime)
		return 0;

	Avalon::Time endTmpTime = Avalon::Time(endTime);
	Avalon::Date date = Avalon::Date(endTmpTime);
	if ((UInt32)date.Hour() < hour)
	{
		date = Avalon::Date(Avalon::Time(endTime - Avalon::Time::MSECS_OF_DAY));
		date.Hour(hour);
		date.Min(0);
		date.Sec(0);
		date.MSec(0);
	}
	else
	{
		date.Hour(hour);
		date.Min(0);
		date.Sec(0);
		date.MSec(0);
	}

	if (date.ToTime().MSec() < lastTime)
	{
		return 0;
	}

	UInt64 deltaT = date.ToTime().MSec() - lastTime;

	UInt32 day = (UInt32)(deltaT / Avalon::Time::MSECS_OF_DAY);

	return day;
}

UInt32 Player::GetOfflineDay(UInt32 Hour, UInt32 updateTime)
{
	if (updateTime > CURRENT_TIME.Sec())
		return 0;

	Avalon::Date date = CURRENT_DATE;
	if (CURRENT_DATE.Hour() < 6)
	{
		date = Avalon::Date(Avalon::Time(CURRENT_TIME.MSec() - 24 * 3600 * 1000));
		date.Hour(Hour);
		date.Min(0);
		date.Sec(0);
		date.MSec(0);
	}
	else
	{
		date.Hour(Hour);
		date.Min(0);
		date.Sec(0);
		date.MSec(0);
	}

	if (date.ToTime().Sec() < updateTime)
	{
		return 0;
	}

	UInt32 deltaT = (UInt32)date.ToTime().Sec() - updateTime;

	UInt32 day = deltaT / (24 * 3600);

	return day + 1;
}

UInt32 Player::GetAllEquipLvMin()
{
	Package* wearPkg = m_ItemMgr.GetPackage(PACK_WEAR);

	if (!wearPkg) return 0;
	class EquipVisitor : public ItemVisitor
	{
	public:
		EquipVisitor() :m_minLv(0),m_num(0) {}

		bool operator()(Item* item)
		{
			if (item->GetDataEntry()->subType >= ST_EQUIP_WEAPON &&
				item->GetDataEntry()->subType <= ST_EQUIP_BRACELET)
			{
				m_num++;
				if (m_minLv == 0)
				{
					m_minLv = item->GetDataEntry()->needLevel;
				}
				else if (item->GetDataEntry()->needLevel < m_minLv)
				{
					m_minLv = item->GetDataEntry()->needLevel;
				}
			}
			
			return true;
		}

		UInt32 GetMinLv()
		{
			if (m_num < 9)
				return 0;
			else
				return m_minLv;
		}

	private:
		UInt32  m_minLv;
		UInt8	m_num;
	};

	EquipVisitor visitor;
	wearPkg->VisitItems(visitor);

	return visitor.GetMinLv();
}

//单位s
UInt32 Player::GetCreateFromNowOn()
{
	if (m_CreateTime > (Int32)CURRENT_TIME.Sec())
	{
		return 0;
	}

	Avalon::Time creatT(Avalon::UInt64(m_CreateTime) * 1000);
	UInt32 deltaT = (UInt32)CURRENT_TIME.Sec() - (UInt32)(GetDayBeginTime(creatT) / 1000);
	return (deltaT+24*3600);
}

//从1开始
UInt32 Player::GetCreateFromNowOnDay()
{
	if (m_CreateTime > (Int32)CURRENT_TIME.Sec())
	{
		return 0;
	}

	Avalon::Time creatT(Avalon::UInt64(m_CreateTime) * 1000);
	UInt32 deltaT = (UInt32)CURRENT_TIME.Sec() - (UInt32)(GetDayBeginTime(creatT) / 1000);
	return (deltaT / (24*3600) + 1);
}

UInt32 Player::GetCreateFromNowOnLogicDay()
{
	if (m_CreateTime > (Int32)CURRENT_TIME.Sec())
	{
		return 0;
	}

	Avalon::Time creatT(Avalon::UInt64(m_CreateTime) * 1000);
	UInt32 deltaT = (UInt32)CURRENT_TIME.Sec() - (UInt32)(GetDayBeginTime2(creatT) / 1000);
	return (deltaT / (24 * 3600) + 1);
}

UInt32 Player::GetAllEquipQualityMin()
{
	Package* wearPkg = m_ItemMgr.GetPackage(PACK_WEAR);

	if (!wearPkg) return 0;
	class EquipVisitor : public ItemVisitor
	{
	public:
		EquipVisitor() :m_minQuality(0), m_num(0) {}

		bool operator()(Item* item)
		{
			if (item->GetDataEntry()->subType >= ST_EQUIP_WEAPON &&
				item->GetDataEntry()->subType <= ST_EQUIP_BRACELET)
			{
				m_num++;
				if (m_minQuality == 0)
				{
					m_minQuality = item->GetDataEntry()->color;
				}
				else if (item->GetDataEntry()->color < m_minQuality)
				{
					m_minQuality = item->GetDataEntry()->color;
				}
			}

			return true;
		}

		UInt32 GetMinQuality()
		{
			if (m_num < 9)
				return 0;
			else
				return m_minQuality;
		}

	private:
		UInt32  m_minQuality;
		UInt8	m_num;
	};

	EquipVisitor visitor;
	wearPkg->VisitItems(visitor);

	return visitor.GetMinQuality();
}

void Player::SendNotifyById(UInt32 id)
{
	SendNotify(id);
}

UInt8 Player::GetEquipNumByStrLv(UInt32 strLv)
{
	Package* wearPkg = m_ItemMgr.GetPackage(PACK_WEAR);
	//Package* equipPkg = m_ItemMgr.GetPackage(PACK_EQUIP);

	//if (!wearPkg || !equipPkg) return 0;
	if (!wearPkg) return 0;
	class EquipVisitor : public ItemVisitor
	{
	public:
		EquipVisitor(UInt32 strLv) :m_num(0), m_strLv(strLv){}

		bool operator()(Item* item)
		{
			if (item->GetDataEntry()->subType >= ST_EQUIP_WEAPON &&
				item->GetDataEntry()->subType <= ST_EQUIP_BRACELET)
			{
				if (item->GetStrengthen() >= m_strLv)
				{
					m_num += 1;
				}
			}

			return true;
		}

		UInt8 GetNum()
		{
			return m_num;
		}

	private:
		UInt8  m_num;
		UInt32 m_strLv;
	};

	EquipVisitor visitor(strLv);
	wearPkg->VisitItems(visitor);
	//equipPkg->VisitItems(visitor);

	return visitor.GetNum();
}

UInt8 Player::GetStrMaxLv(UInt8 num, UInt8 needStrLv)
{
	Package* wearPkg = m_ItemMgr.GetPackage(PACK_WEAR);

	if (!wearPkg) return 0;
	class EquipVisitor : public ItemVisitor
	{
	public:
		EquipVisitor(UInt8 num, UInt8 needStrLv)
		{
			m_retStrLv = 0;
			memset(m_num, 0, sizeof(UInt8)* (ITEM_STRENTH_MAX + 1));
			m_needNum = num;
			m_needStrLv = needStrLv;

			if (m_needNum > EQUIP_GRID_NUM)
			{
				m_needNum = EQUIP_GRID_NUM;
			}
			if (m_needStrLv > ITEM_STRENTH_MAX)
			{
				m_needStrLv = ITEM_STRENTH_MAX;
			}

		}

		bool operator()(Item* item)
		{
			if (item->GetDataEntry()->subType >= ST_EQUIP_WEAPON &&
				item->GetDataEntry()->subType <= ST_EQUIP_BRACELET)
			{
				if (item->GetStrengthen() == 0)
				{
					return true;
				}
				if (item->GetStrengthen() > ITEM_STRENTH_MAX)
				{
					return true;
				}
				m_num[item->GetStrengthen()]++;

				/*
				m_num[item->GetStrengthen()]++;

				if (m_num[m_needStrLv] >= m_needNum)
				{	
					m_retStrLv = m_needStrLv;
					return false;
				}
				else
				{
					if (m_num[item->GetStrengthen()] >= m_needNum &&
						item->GetStrengthen() > m_retStrLv)
					{
						m_retStrLv = item->GetStrengthen();
					}
				}
				*/
				
			}

			return true;
		}

		UInt8 GetRetStrLv()
		{
			UInt32 num = m_needNum;
			for (UInt32 i = ITEM_STRENTH_MAX; i > 0; i--)
			{
				//从大到小,第m_needNum个强化等级返回
				if (num >= m_num[i])
				{
					num -= m_num[i];
				}
				else
				{
					num = 0;
				}
				if (num == 0) return i;
			}
			return 0;

			//return m_retStrLv;
		}

	private:
		UInt8  m_retStrLv;
		UInt8  m_num[ITEM_STRENTH_MAX + 1];	//各强化等级的数量
		UInt8  m_needNum;
		UInt8  m_needStrLv;
	};

	EquipVisitor visitor(num, needStrLv);
	wearPkg->VisitItems(visitor);
	return visitor.GetRetStrLv();
}

UInt8 Player::GetStrMaxLvAll()
{
	Package* wearPkg = m_ItemMgr.GetPackage(PACK_WEAR);
	Package* equipPkg = m_ItemMgr.GetPackage(PACK_EQUIP);

	if (!wearPkg) return 0;
	class EquipVisitor : public ItemVisitor
	{
	public:
		EquipVisitor() :m_retStrLv(0){}

		bool operator()(Item* item)
		{
			if (item->GetDataEntry()->subType >= ST_EQUIP_WEAPON &&
				item->GetDataEntry()->subType <= ST_EQUIP_BRACELET)
			{
				if (item->GetStrengthen() >= m_retStrLv)
				{
					
					m_retStrLv = item->GetStrengthen();
				}
			}

			return true;
		}

		UInt8 GetRetStrLv()
		{
			return m_retStrLv;
		}

	private:
		UInt8  m_retStrLv;
	};

	EquipVisitor visitorWearPkg;
	wearPkg->VisitItems(visitorWearPkg);

	EquipVisitor visitorEquipPkg;
	equipPkg->VisitItems(visitorEquipPkg);

	return visitorWearPkg.GetRetStrLv() > visitorEquipPkg.GetRetStrLv() ? visitorWearPkg.GetRetStrLv() : visitorEquipPkg.GetRetStrLv();
}

UInt8 Player::GetEquipNumByLv(UInt32 lv)
{
	Package* wearPkg = m_ItemMgr.GetPackage(PACK_WEAR);
	
	if (!wearPkg) return 0;
	class EquipVisitor : public ItemVisitor
	{
	public:
		EquipVisitor(UInt32 lv) :m_num(0), m_lv(lv){}

		bool operator()(Item* item)
		{
			if (item->GetDataEntry()->subType >= ST_EQUIP_WEAPON &&
				item->GetDataEntry()->subType <= ST_EQUIP_BRACELET)
			{
				if (item->GetNeedLevel() >= m_lv)
				{
					m_num += 1;
				}
			}

			return true;
		}

		UInt8 GetNum()
		{
			return m_num;
		}

	private:
		UInt8  m_num;
		UInt32 m_lv;
	};

	EquipVisitor visitor(lv);
	wearPkg->VisitItems(visitor);

	return visitor.GetNum();
}

UInt32 Player::GetWearFashionNum()
{
	Package* pkg = m_ItemMgr.GetPackage(PACK_FASHION_WEAR);
	if (!pkg)
		return 0;

	return pkg->GetItemNum();
}

UInt32 Player::GetTitleNum()
{
	UInt32 wearTitleNum = 0;
	UInt32 pkgTitleNum = 0;
	Package* titlePkg = m_ItemMgr.GetPackage(PACK_TITLE);
	if (titlePkg)
		pkgTitleNum = titlePkg->GetItemNum();
	
	Item* item = GetEquip(ST_EQUIP_TITLE);
	if (item)
		wearTitleNum = 1;

	return pkgTitleNum + wearTitleNum;
}

UInt32 Player::GetWearEquipNumByQua(UInt8 color)
{
	Package* wearPkg = m_ItemMgr.GetPackage(PACK_WEAR);

	if (!wearPkg) return 0;
	class EquipVisitor : public ItemVisitor
	{
	public:
		EquipVisitor(UInt8 color) :m_num(0), m_color(color){}

		bool operator()(Item* item)
		{
			if (item->GetDataEntry()->subType >= ST_EQUIP_WEAPON &&
				item->GetDataEntry()->subType <= ST_EQUIP_BRACELET)
			{
				if (item->GetDataEntry()->color >= m_color)
				{
					m_num += 1;
				}
			}
			
			return true;
		}

		UInt8 GetNum()
		{
			return m_num;
		}

	private:
		UInt8  m_num;
		UInt8 m_color;
	};

	EquipVisitor visitor(color);
	wearPkg->VisitItems(visitor);

	return visitor.GetNum();
}

UInt32	Player::GetSignInCount()
{
	return m_CounterMgr.GetCounter(SIGN_IN_TT_COUNT);
}

bool Player::CheckRmMoney(ItemSubType moneyType, UInt32 cost)
{
	switch (moneyType)
	{
	case ST_ITEM_GOLD:	return GetGold() >= cost;		
	case ST_ITEM_POINT:	return GetPoint() >= cost;		
	case ST_BIND_GOLD:	return GetGoldBoth() >= cost;	
	case ST_BIND_POINT:	return GetPointBoth() >= cost;	
	case ST_WARRIOR_SOUL:	return GetWarriorSoul() >= cost;
	case ST_PK_COIN:	return GetPkCoin() >= cost;
	case ST_GUILD_CONTRI:	return GetGuildContribution() >= cost;
	case ST_GOLDJAR_POINT:	return GetGoldJarPoint() >= cost;
	case ST_MAGJAR_POINT:	return GetMagJarPoint() >= cost;
	//case ST_SP:			return GetSP() >= cost;
	case ST_REVIVE_COIN:	return GetReviveCoin() >= cost;
	case ST_APPOINTMENT_COIN: return GetAppointmentCoin() >= cost;
	case ST_MASTER_GOODTEACH_VALUE: return  GetMasterSysGoodTeachValue() >= cost;
	case ST_DRESS_INTEGRAL_VALUE: return  GetChangeFashionScore() >= cost;
	case ST_WEAPON_LEASE_TICKET: return GetWeaponLeaseTickets() >= cost;
	case ST_CHIJI_SHOP_COIN: return GetCounter(CHIJI_SHOP_COIN_NAME) >= cost;

	default:			return false;					
	}
}

bool Player::CheckRmMoney(ItemDataEntry* data, UInt32 cost)
{
	if (!data)
	{
		return false;
	}

	if (data->type != ITEM_INCOME)
	{
		ErrorLogStream << PLAYERINFO(this) << "moneydata type is err!type = " << data->type << LogStream::eos;
		SendNotify("moneydata type is err! type = {0}", data->type);
		
		return false;
	}

	return CheckRmMoney((ItemSubType)data->subType, cost);
}

bool Player::RemoveMoney(const char* reason, ItemSubType moneyType, UInt32 num)
{
	switch (moneyType)
	{
	case ST_ITEM_GOLD:		RemoveGold(reason, num);		break;
	case ST_ITEM_POINT:		RemovePoint(reason, num);		break;
	case ST_BIND_GOLD:		RemoveGoldBoth(reason, num);	break;
	case ST_BIND_POINT:		RemovePointBoth(reason, num);	break;
	case ST_WARRIOR_SOUL:	RemoveWarriorSoul(reason, num);	break;
	case ST_PK_COIN:		RemovePkCoin(reason, num);		break;
	case ST_GUILD_CONTRI:	RemoveGuildContribution(reason, num);	break;
	case ST_GOLDJAR_POINT:	RemoveGoldJarPoint(reason, num);		break;
	case ST_MAGJAR_POINT:	RemoveMagJarPoint(reason, num);		break;
	case ST_REVIVE_COIN:	RemoveReviveCoin(reason, num);		break;
	case ST_APPOINTMENT_COIN:	RemoveAppointmentCoin(reason, num);	break;
	case ST_MASTER_GOODTEACH_VALUE: RemoveSysGoodTeachValue(reason, num);	break;
	case ST_DRESS_INTEGRAL_VALUE: RemoveChangeFashionScore(reason, num);	break;
	case ST_WEAPON_LEASE_TICKET: RemoveWeaponLeaseTickets(reason, num);		break;
	case ST_CHIJI_SHOP_COIN: RemoveChiJiShopCoin(reason, num);	break;
	default:												return false;
	}

	return true;
}

bool Player::AddExpFromRewardAdapter(UInt32 key, UInt16 subKey)
{
	RewardAdapterDataEntry* data = RewardAdapterDataEntryMgr::Instance()->FindEntry(key);
	if (!data)
		return false;

	auto itr = data->levelRewards.find(subKey);
	if (itr == data->levelRewards.end())
		return false;

	std::vector<ItemReward>& rewards = itr->second;
	RewardGroup grp(rewards);

	AddRewards("addexp", &grp, true);
	return true;
}

void Player::AddExpByLevelPercent(const char* reason, UInt32 percent)
{
	auto data = PlayerDataEntryMgr::Instance()->FindEntry(GetLevel());
	if (!data)
	{
		return;
	}

	if (data->upgradeexp == 0)
	{
		return;
	}

	UInt64 exp = ((UInt64)data->upgradeexp * percent / 100);
	IncExp(reason, exp);
}

void Player::OnLogicOffLine()
{
	SetCounter(DAY_ONLINE_TIME, m_DayOnlineTime);

	//存回仓库道具
// 	class SaveItemVisitor : public ItemVisitor
// 	{
// 	public:
// 		SaveItemVisitor(Player* player) :m_pPlayer(player){}
// 		~SaveItemVisitor(){}
// 
// 		bool operator()(Item* item)
// 		{
// 			CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand(m_pPlayer->GetSrcZoneId(), "t_item", item->GetID(), true);
// 			item->GetDBData(cmd, true);
// 			CLRecordClient::Instance()->SendCommand(cmd);
// 			return true;
// 		}
// 
// 	private:
// 		Player* m_pPlayer;
// 	};
// 	SaveItemVisitor saveItemVisitor(this);
// 	m_ItemMgr.VisitItems(PACK_STORAGE, saveItemVisitor);
}

MallGiftPackActivateCond Player::CheckEquipBrokenActivateCond()
{
	UInt16 playerLevel = GetLevel();
	UInt16 lowLimit = (UInt16)PL_TEN;
	UInt16 upperLimit = (UInt16)PL_FIFTY + (UInt16)PLAYER_LEVEL_INTERVAL;

	if (playerLevel < lowLimit)
	{
		return MGPAC_INVALID;
	}
	if (playerLevel >= upperLimit)
	{
		return MGPAC_STRENGEN_BROKE_FIFTY;
	}

	UInt16 val, rem;
	for (auto & level : m_EquipBindPlayerLevel)
	{
		if (level == 0)
		{
			ErrorLogStream << PLAYERINFO(this) << "Error elem(" << level << ") in m_EquipBindPlayerLevel!" << LogStream::eos;
			continue;
		}

		val = playerLevel / (UInt16)level;
		rem = playerLevel % (UInt16)level;
		if (val == 1 && rem >= 0 && rem < PLAYER_LEVEL_INTERVAL)
		{
			return GetActivateCond((PlayerLevel)level);
		}
	}

	ErrorLogStream << PLAYERINFO(this) << "player's level(" << playerLevel << ") Algorithm, that how to get condition(activate equip broken), is error!" << LogStream::eos;
	return MGPAC_INVALID;
}

MallGiftPackActivateCond Player::GetActivateCond(PlayerLevel level)
{
	switch (level)
	{
	case PL_TEN: return MGPAC_STRENGEN_BROKE_TEN;
	case PL_FIFTEEN: return MGPAC_STRENGEN_BROKE_FIFTEEN;
	case PL_TWENTY: return MGPAC_STRENGEN_BROKE_TWENTY;
	case PL_TWENTY_FIVE: return MGPAC_STRENGEN_BROKE_TWENTY_FIVE;
	case PL_THIRTY: return MGPAC_STRENGEN_BROKE_THIRTY;
	case PL_THIRTY_FIVE: return MGPAC_STRENGEN_BROKE_THIRTY_FIVE;
	case PL_FORTY: return MGPAC_STRENGEN_BROKE_FORTY;
	case PL_FORTY_FIVE: return MGPAC_STRENGEN_BROKE_FORTY_FIVE;
	case PL_FIFTY: return MGPAC_STRENGEN_BROKE_FIFTY;
	default: 
		ErrorLogStream << PLAYERINFO(this) << "playerlevel is error! level = " << level << LogStream::eos;
		return MGPAC_INVALID;
	};
}

void Player::ActivateMallGiftPackReq(MallGiftPackActivateCond cond)
{
	CLProtocol::MallGiftPackActivateReq req;
	req.m_PlayerId = GetID();
	req.m_GiftPackActCond = (UInt8)cond;

	Router::SendToWorld(req);
}

void Player::CheckMallGiftPackOnLevelUp()
{
	//std::vector<UInt32> gifts;
	//MallGiftPackDataEntryMgr::Instance()->GetGiftsByCondition(MGPAC_PLAYER_LEVEL_UP_GET_GIFT, gifts);
	//for (const auto& gift : gifts)
	//{
	//	auto dataEntry = MallGiftPackDataEntryMgr::Instance()->FindEntry(gift);
	//	if (!dataEntry) continue;

	//	if (dataEntry->params.empty())
	//	{
	//		ErrorLogStream << PLAYERINFO(this) << "Mall gift pack data(" << gift << ")'s params were empty!" << LogStream::eos;
	//		continue;
	//	}

	//	if (GetLevel() >= dataEntry->params.front())
	//	{
	//		ActivateMallGiftPackReq(MGPAC_PLAYER_LEVEL_UP_GET_GIFT);
	//	}
	//}
}

void Player::OnBuyMallGoods(CLProtocol::WorldMallCheckBuy& req)
{
	MallCheckBuyInfo pBuy;

	pBuy.Init(req);
	pBuy.SetOwner(this);
	pBuy.OnBuy();
}

//装备转移
UInt32 Player::TransferEquip(ObjID_t equipUid, ObjID_t transferId)
{
	UInt32 code = ITEM_DATA_INVALID;
	Item* equip = FindItem(equipUid);
	if (!equip)
	{
		SendNotify("equip is null, uid={0}", equipUid);
		return ITEM_DATA_INVALID;
	}

	Item* transferStone = FindItem(transferId);
	if (!transferStone)
	{
		SendNotify("transferStone is null, uid={0}", transferId);
		return ITEM_DATA_INVALID;
	}
	code = m_ItemMgr.TransferEquip(equip, transferStone);

	return code;
}

//装备回收
//处理装备回收提交请求
UInt32 Player::handleEqRecSubcmt(std::vector<UInt64>& eqUids, std::vector<EqRecScoreItem>& scoreItems, UInt32& score)
{
	// 安全锁开启，不允许回收紫色以上装备操作
	if (!SecurityLockCheckByColor(eqUids, ITEM_QUALITY_PURPLE))
		return ErrorCode::SECURITY_LOCK_FORBID_OP;

	UInt32 code = ITEM_DATA_INVALID;
	if (eqUids.size() == 0)
	{
		return ITEM_EQUIPRECO_EMPTY;
	}
	/*for (size_t i = 0; i < eqUids.size(); ++i)
	{
	Item* equip = FindItem(eqUids[i]);
	if (!equip)
	{
	SendNotify("equip is null, uid={0}", eqUids[i]);
	return ITEM_DATA_INVALID;
	}
	}*/

	code = m_ItemMgr.handleEqRecSubcmt(eqUids, scoreItems, score);

	return code;
}

//装备赎回请求
UInt32 Player::handleEqRecRedeem(UInt64 equid, UInt32& consScore)
{
	UInt32 code = ITEM_DATA_INVALID;
	Item* equip = FindItem(equid);
	if (!equip)
	{
		SendNotify("equip is null, uid={0}", equid);
		return ITEM_DATA_INVALID;
	}

	code = m_ItemMgr.handleEqRecRedeem(equip, consScore);

	return code;
}

//装备回收积分提升请求
UInt32 Player::handleEqRecUpscore(UInt64 equid, UInt32& upscore)
{
	UInt32 code = ITEM_DATA_INVALID;
	Item* equip = FindItem(equid);
	if (!equip)
	{
		SendNotify("equip is null, uid={0}", equid);
		return ITEM_DATA_INVALID;
	}

	code = m_ItemMgr.handleEqRecUpscore(equip, upscore);

	return code;
}

UInt32 Player::GiveScoreWarReward(UInt32 rewardId)
{
	ScoreWarRewardDataEntry* dataEntry = ScoreWarRewardDataEntryMgr::Instance()->GetBaseDataEntry(rewardId);
	if (dataEntry == NULL) return ErrorCode::SCORE_WAR_REWARD_NOT_EXIST;
	UInt32 maxBattleCount = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_SCORE_WAR_BATTLE_COUNT);
	UInt32 battleCount = 0;
	if (maxBattleCount >= GetScoreWarBattleCount())
		battleCount = maxBattleCount - GetScoreWarBattleCount();
	if (dataEntry->battleCount > battleCount) return ErrorCode::SCORE_WAR_REWARD_BATTLE_COUNT_ERROR;
	if (dataEntry->winCount != 0 && dataEntry->winCount > GetScoreWarWinBattleCount()) return ErrorCode::SCORE_WAR_REWARD_WIN_COUNT_ERROR;
	if (CheckScoreWarReward(rewardId)) return ErrorCode::SCORE_WAR_REWARD_GIVE;

	RewardGroup* rewards = GetRewardGroup();
	rewards->AddRewards(dataEntry->rewards);
	UInt32 result = CheckAddRewards(rewards);
	if (result == ErrorCode::SUCCESS)
	{
		SetScoreWarReward(rewardId);
		AddRewards(GetReason(LogSourceType::SOURCE_TYPE_SCORE_WAR, rewardId).c_str(), rewards, true);
	}

	return result;
}

void Player::SendScoreWarSortListInfo()
{
	
}

void  Player::AddMasterSysAcademicGrowth(const char* reason, UInt32 num)
{
	if (reason == NULL) return;
	UInt32 oldGrowth = m_AcademicTotalGrowth;
	if (m_AcademicTotalGrowth + num > MAX_MONEY) m_AcademicTotalGrowth = MAX_MONEY;
	else m_AcademicTotalGrowth = m_AcademicTotalGrowth + num;

	if (m_AcademicTotalGrowth > oldGrowth)
	{
		SyncMasterSysDataToWorld();

		DebugLogStream << PLAYERINFO(this) << " AddMasterSysAcademicGrowth old:" << oldGrowth << " new:" << m_AcademicTotalGrowth << " add:" << num << " reason:" << reason << LogStream::eos;

		SendAssetUdpLog(AssetType::ASSET_MASTER_ACADEMIC_GROWTH, reason, (UInt64)oldGrowth, (Int64)num, (Int64)m_AcademicTotalGrowth - (Int64)oldGrowth, (UInt64)m_AcademicTotalGrowth);
	}
}

void Player::AddMasterSysGoodTeachValue(const char* reason, UInt32 num)
{
	if (reason == NULL) return;

	UInt32 oldValue = m_GoodTeacherValue;
	if (m_GoodTeacherValue + num > MAX_MONEY) m_GoodTeacherValue = MAX_MONEY;
	else m_GoodTeacherValue = m_GoodTeacherValue + num;

	if (m_GoodTeacherValue > oldValue)
	{
		SyncMasterSysDataToWorld();

		DebugLogStream << PLAYERINFO(this) << " AddMasterSysGoodTeachValue old:" << oldValue << " new:" << m_GoodTeacherValue << " add:" << num << " reason:" << reason << LogStream::eos;

		SendAssetUdpLog(AssetType::ASSET_MASTER_GOODTEACH_VALUE, reason, (UInt64)oldValue, (Int64)num, (Int64)m_GoodTeacherValue - (Int64)oldValue, (UInt64)m_GoodTeacherValue);
	}
}

void Player::RemoveSysGoodTeachValue(const char* reason, UInt32 num)
{
	if (reason == NULL) return;
	UInt32 oldValue = m_GoodTeacherValue;
	UInt32 newValue = oldValue;
	if (num >= oldValue) newValue = 0;
	else newValue = oldValue - num;

	if (oldValue > newValue)
	{
		GameInfoLogStream("Property") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") RemoveSysGoodTeachValue:"
			<< reason << "|" << oldValue - newValue << "|" << newValue << LogStream::eos;

		SendAssetUdpLog(AssetType::ASSET_MASTER_GOODTEACH_VALUE, reason, (UInt64)oldValue, (Int64)num * -1, (Int64)newValue - (Int64)oldValue, (Int64)newValue);
		m_GoodTeacherValue = newValue;
	}
}

void Player::AddMasterDailyTaskGrowth(const char* reason, UInt32 num)
{
	if (reason == NULL) return;
	UInt32 oldGrowth = m_MasterDailyTaskGrowth;
	if (m_MasterDailyTaskGrowth + num > MAX_MONEY) m_MasterDailyTaskGrowth = MAX_MONEY;
	else m_MasterDailyTaskGrowth = m_MasterDailyTaskGrowth + num;

	if (m_MasterDailyTaskGrowth > oldGrowth)
	{
		DebugLogStream << PLAYERINFO(this) << " AddMasterDailyTaskGrowth old:" << oldGrowth << " new:" << m_MasterDailyTaskGrowth << " add:" << num << " reason:" << reason << LogStream::eos;

		//SendAssetUdpLog(AssetType::ASSET_MASTER_ACADEMIC_GROWTH, reason, (UInt64)oldGrowth, (Int64)num, (Int64)m_MasterDailyTaskGrowth - (Int64)oldGrowth, (UInt64)m_MasterDailyTaskGrowth);
	}
}

void Player::AddMasterAcademicTaskGrowth(const char* reason, UInt32 num)
{
	if (reason == NULL) return;
	UInt32 oldGrowth = m_MasterAcademicTaskGrowth;
	if (m_MasterAcademicTaskGrowth + num > MAX_MONEY) m_MasterAcademicTaskGrowth = MAX_MONEY;
	else m_MasterAcademicTaskGrowth = m_MasterAcademicTaskGrowth + num;

	if (m_MasterAcademicTaskGrowth > oldGrowth)
	{
		DebugLogStream << PLAYERINFO(this) << " AddMasterSysAcademicGrowth old:" << oldGrowth << " new:" << m_MasterAcademicTaskGrowth << " add:" << num << " reason:" << reason << LogStream::eos;

		//SendAssetUdpLog(AssetType::ASSET_MASTER_ACADEMIC_GROWTH, reason, (UInt64)oldGrowth, (Int64)num, (Int64)m_MasterAcademicTaskGrowth - (Int64)oldGrowth, (UInt64)m_MasterAcademicTaskGrowth);
	}
}

void Player::AddMasterTeamClearDungeonGrowth(const char* reason, UInt32 num, UInt32 dungeonId)
{
	if (reason == NULL) return;
	UInt32 oldGrowth = m_MasterTeamClearDungeonGrowth;
	if (m_MasterTeamClearDungeonGrowth + num > MAX_MONEY) m_MasterTeamClearDungeonGrowth = MAX_MONEY;
	else m_MasterTeamClearDungeonGrowth = m_MasterTeamClearDungeonGrowth + num;

	if (m_MasterTeamClearDungeonGrowth > oldGrowth)
	{
		std::string  resaon = GetReason(MASTERSYS_ADDGROWTH_DUNGEON_END, dungeonId, m_MasterTeamClearDungeonGrowth - oldGrowth);
		this->AddItem(resaon.c_str(), 600000301, m_MasterTeamClearDungeonGrowth - oldGrowth, 0, 0, true);
		DebugLogStream << PLAYERINFO(this) << " m_MasterTeamClearDungeonGrowth old:" << oldGrowth << " new:" << m_MasterTeamClearDungeonGrowth << " add:" << num << " reason:" << reason << LogStream::eos;

		//SendAssetUdpLog(AssetType::ASSET_MASTER_ACADEMIC_GROWTH, reason, (UInt64)oldGrowth, (Int64)num, (Int64)m_MasterAcademicTaskGrowth - (Int64)oldGrowth, (UInt64)m_MasterAcademicTaskGrowth);
	}
}

void Player::AddMasterUplevelGrowth(const char* reason, UInt32 num)
{
	if (reason == NULL) return;
	UInt32 oldGrowth = m_MasterUplevelGrowth;
	if (m_MasterUplevelGrowth + num > MAX_MONEY) m_MasterUplevelGrowth = MAX_MONEY;
	else m_MasterUplevelGrowth = m_MasterUplevelGrowth + num;

	if (m_MasterUplevelGrowth > oldGrowth)
	{
		std::string  resaon = GetReason(MASTERSYS_ADDGROWTH_UPLEVEL, this->GetLevel(), m_MasterUplevelGrowth > oldGrowth);
		this->AddItem(resaon.c_str(), 600000301, m_MasterUplevelGrowth - oldGrowth, 0, 0, true);
		DebugLogStream << PLAYERINFO(this) << " m_MasterUplevelGrowth old:" << oldGrowth << " new:" << m_MasterUplevelGrowth << " add:" << num << " reason:" << reason << LogStream::eos;

		//SendAssetUdpLog(AssetType::ASSET_MASTER_ACADEMIC_GROWTH, reason, (UInt64)oldGrowth, (Int64)num, (Int64)m_MasterAcademicTaskGrowth - (Int64)oldGrowth, (UInt64)m_MasterAcademicTaskGrowth);
	}
}

void Player::AddMasterGiveEquipGrowth(const char* reason, UInt32 num)
{
	if (reason == NULL) return;
	UInt32 oldGrowth = m_MasterGiveEquipGrowth;
	if (m_MasterGiveEquipGrowth + num > MAX_MONEY) m_MasterGiveEquipGrowth = MAX_MONEY;
	else m_MasterGiveEquipGrowth = m_MasterGiveEquipGrowth + num;

	if (m_MasterGiveEquipGrowth > oldGrowth)
	{
		std::string  resaon = GetReason(LogSourceType::MASTERSYS_ADDGROWTH_MASTER_GIVE, this->GetID(), 1);
		this->AddItem(resaon.c_str(), 600000301, m_MasterGiveEquipGrowth - oldGrowth, 0, 0, true);
		DebugLogStream << PLAYERINFO(this) << " AddMasterGiveEquipGrowth old:" << oldGrowth << " new:" << m_MasterGiveEquipGrowth << " add:" << num << " reason:" << reason << LogStream::eos;

		//SendAssetUdpLog(AssetType::ASSET_MASTER_ACADEMIC_GROWTH, reason, (UInt64)oldGrowth, (Int64)num, (Int64)m_MasterAcademicTaskGrowth - (Int64)oldGrowth, (UInt64)m_MasterAcademicTaskGrowth);
	}
}

void Player::SetFinSchDiscipleSum(UInt32 value)
{
	m_FinSchDiscipleSum = value;
	
	//刷新成就
	this->GetAchievementTaskMgr().OnDiscipleFinsch();
	
}

UInt32 Player::GetDiscipleNum()
{
	std::map<ObjID_t, UInt16>& rList = m_Relations[RelationType::RELATION_DISCIPLE - 1];
	return rList.size();
}

void Player::GetMasterTaskShareData(MasterTaskShareData& masterTaskShareData)
{
	masterTaskShareData.m_AcademicTotalGrowth = this->m_AcademicTotalGrowth;
	masterTaskShareData.m_MasterDailyTaskGrowth = this->m_MasterDailyTaskGrowth;
	masterTaskShareData.m_MasterAcademicTaskGrowth = this->m_MasterAcademicTaskGrowth;
	masterTaskShareData.m_MasterGiveEquipGrowth = this->m_MasterGiveEquipGrowth;
	masterTaskShareData.m_MasterUplevelGrowth = this->m_MasterUplevelGrowth;
	masterTaskShareData.m_MasterGiveGiftGrowth = this->m_MasterGiveGiftGrowth;
	masterTaskShareData.m_MasterTeamClearDungeonGrowth = this->m_MasterTeamClearDungeonGrowth;
	masterTaskShareData.m_GoodTeachValue = this->m_GoodTeacherValue;
	this->GetMasterDailyTaskMgr().GetShareData(masterTaskShareData);
	this->GetMasterAcademicGrowthTaskMgr().GetShareData(masterTaskShareData);
}

void Player::HandleOfflineRelieveMaster(ObjID_t discipleId)
{
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_player_info", discipleId, true);
	cmd->PutData("academic_total_growth", 0);
	cmd->PutData("master_dailytask_growth", 0);
	cmd->PutData("master_academictask_growth", 0);
	cmd->PutData("master_giveequip_growth", 0);
	cmd->PutData("master_uplevel_growth", 0);
	cmd->PutData("master_givegift_growth", 0);
	CLRecordClient::Instance()->SendCommand(cmd);

	MasterDailyTaskMgr::ClearOfflinePlayerAllTask(discipleId);
	MasterAcademicGrowthTaskMgr::ClearOfflinePlayerAllTask(discipleId);
}

UInt32 Player::OnMasterSysGiveGift(UInt8 type, UInt32 giftDataId)
{
	MasterSysGiftDataEntry* entryData = MasterSysGiftMapDataEntryMgr::Instance()->FindEntry(giftDataId);
	if (entryData == NULL)
	{
		InfoLogStream << PLAYERINFO(this) << "entryData is null of id=" << giftDataId << LogStream::eos;
		return ErrorCode::MASTERSYS_RECEIEVEGIFT_FAIL;
	}
	if (type < MSGT_MASTER_DAILYTASK || type > MSGT_FISNISHCL_DISCIPLE)
	{
		return ErrorCode::MASTERSYS_RECEIEVEGIFT_FAIL;
	}
	UInt32 reasonId = 0;
	if (type == MSGT_MASTER_DAILYTASK || type == MSGT_DISCIPLE_DAILYTASK)
	{
		reasonId = MASTERSYS_RECV_DAILYTASK_RWD;
	}
	else if (type == MSGT_DISCIPLE_ACADEMIC)
	{
		reasonId = MASTERSYS_RECV_ACADEMIC_RWD;
		//检查成长值
		if (this->GetMasterSysAcademicGrowth() < entryData->GetParam())
		{
			return ErrorCode::MASTERSYS_RECEIEVEGIFT_GROWTHFAIL;
		}
		//检查是否领取过
		const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(entryData->counterKey);
		if (!counterConfig)
		{
			CounterCfg::Instance()->RegCounter(entryData->counterKey.c_str(), COUNTER_CYCLE_NONE);
		}
		UInt32 jarst = this->GetCounter(entryData->counterKey.c_str());
		if (jarst == MasterAcademicRewardRecvState::MARRS_RECVED)
		{
			return ErrorCode::MASTERSYS_RECEIEVEGIFT_RECVED;
		}
	}
	else if (type == MSGT_FISNISHCL_MASTER || type == MSGT_FISNISHCL_DISCIPLE)
	{
		reasonId = MASTERSYS_RECV_FINSCH_RWD;
	}

	if (entryData->giftIds.size() > 0)
	{
		RewardGroup group;
		std::string reason = GetReason((LogSourceType)reasonId, this->GetID(), entryData->GetKey());
		for (UInt32 i = 0; i < entryData->giftIds.size(); i++)
		{
			group.AddReward(entryData->giftIds[i], 1);
		}
		this->AddRewards(reason.c_str(), &group, true);
	}
	else
	{
		InfoLogStream << PLAYERINFO(this) << "giftIds is empty of id=" << giftDataId << LogStream::eos;
		return ErrorCode::MASTERSYS_RECEIEVEGIFT_FAIL;
	}
	/*std::string reason = GetReason((LogSourceType)reasonId, this->GetID(), entryData->giftId);
	RewardGroup group;
	group.AddReward(entryData->giftId, 1);
	this->AddRewards(reason.c_str(), &group, true);*/

	//成长奖励领取要修改状态
	if (type == MasterSysGiftType::MSGT_DISCIPLE_ACADEMIC)
	{
		this->SetCounter(entryData->counterKey.c_str(), MasterAcademicRewardRecvState::MARRS_RECVED);
	}
	
	return ErrorCode::SUCCESS;
}

void Player::OnLevelUpMasterSys()
{
	if (this->HasRelation(RELATION_MASTER))
	{
		
		std::string  resaon = GetReason(MASTERSYS_ADDGROWTH_UPLEVEL, this->GetLevel(), 2);
		this->AddMasterUplevelGrowth(resaon.c_str(), 2);
		//this->AddMasterSysAcademicGrowth(resaon.c_str(), 2);
		
	}
}

void Player::OnGiveEquipMasterSys(UInt32 num)
{
	UInt32 upLimit = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_MASTER_GIVEEQ_ADDGROWTH_MAX);
	bool update = false;
	for (UInt32 i = 1; i <= num; i++)
	{
		if (GetCounter(MASTER_GIVEEQUIP_ADDGROWTH) >= upLimit)
		{
			break;
		}
		IncCounter(MASTER_GIVEEQUIP_ADDGROWTH, 1);
		std::string  resaon = GetReason(LogSourceType::MASTERSYS_ADDGROWTH_MASTER_GIVE, this->GetID(), 1);

		this->AddMasterGiveEquipGrowth(resaon.c_str(), 1);
		update = true;
		//this->AddMasterSysAcademicGrowth(resaon.c_str(), 1);
	}

	if (update)
	{
		this->SyncToRecord();
		this->SyncMasterSysDataToWorld();
	}
}

Int32 Player::GetChijiHp() const
{
	return m_ChijiHp;
}

void Player::SetChijiHp(Int32 hp)
{
	m_ChijiHp = hp;

	if (m_ChijiHp > HP_MP_FULL_VALUE)
	{
		DevLogStream << "player(" << PLAYERINFO(this) << ") invalid hp(" << m_ChijiHp << ")." << LogStream::eos;
		m_ChijiHp = HP_MP_FULL_VALUE;
	}
}

void Player::DecChijiHp(Int32 value)
{
	if (value < 0)
	{
		return;
	}

	m_ChijiHp = m_ChijiHp - value;
	m_ChijiHp = m_ChijiHp < 0 ? 0 : m_ChijiHp;
}

void Player::IncChijiHp(Int32 value)
{
	if (value < 0)
	{
		return;
	}

	SetChijiHp(GetChijiHp() + value);
}

Int32 Player::GetChijiMp() const
{
	return m_ChijiMp;
}

void Player::SetChijiMp(Int32 mp)
{
	m_ChijiMp = mp;

	if (m_ChijiMp > HP_MP_FULL_VALUE)
	{
		DevLogStream << "player(" << PLAYERINFO(this) << ") invalid mp(" << m_ChijiMp << ")." << LogStream::eos;
		m_ChijiMp = HP_MP_FULL_VALUE;
	}
}

void Player::DecChijiMp(Int32 value)
{
	if (value < 0)
	{
		return;
	}

	m_ChijiMp = m_ChijiMp - value;
	m_ChijiMp = m_ChijiMp < 0 ? 0 : m_ChijiMp;
}

void Player::IncChijiMp(Int32 value)
{
	if (value < 0)
	{
		return;
	}

	SetChijiMp(GetChijiMp() + value);
}

void Player::InitChijiPackage()
{
	// 吃鸡包裹统一100个
	m_PackSize = 100;

	for (UInt32 i = PACK_EQUIP; i <= PACK_FASHION; ++i)
	{
		Package* pPack = m_ItemMgr.GetPackage((UInt8)i);
		if (pPack != NULL)
		{
			pPack->SetSize(m_PackSize);
			pPack->RemoveOverSizeItem();
		}
	}

	Package* pTitlePack = m_ItemMgr.GetPackage(PACK_TITLE);
	if (pTitlePack != NULL)
	{
		pTitlePack->SetSize(m_PackSize);
		pTitlePack->RemoveOverSizeItem();
	}
}

void Player::StudyChiJiSkill(UInt32 skillId, UInt32 skillLvl)
{
	GetPvpSkillMgr().StudyChiJiSkill(skillId, skillLvl);
}

void Player::RemoveChiJiShopCoin(const char* reason, UInt32 num)
{
	UInt32 oldValue = GetCounter(CHIJI_SHOP_COIN_NAME);
	UInt32 oddValue = oldValue > num ? oldValue - num : 0;
	
	SetCounter(CHIJI_SHOP_COIN_NAME, oddValue);
}

void Player::SetChijiNoWarInfo(UInt64 enemyRoleId, UInt32 battleId, UInt32 dungeonId, UInt32 choiceTime)
{
	m_ChijiNoWarEnemyId = enemyRoleId;
	m_ChijiNoWarBattleId = battleId;
	m_ChijiNoWarChoiceTime = choiceTime;
	m_ChijiPKDungeonId = dungeonId;
}

void Player::SendNoWarToBattle()
{
	if (GetChijiNoWarEnemyId() == 0)
	{
		return;
	}

	CLProtocol::BattleCheckNoWarRes res;
	res.roleId = GetChijiNoWarEnemyId();
	res.dstId = GetID();
	res.battleID = GetChijiNoWarBattleId();
	res.dungeonID = GetChijiPKDungeonId();
	res.isNoWar = 1;
	Router::SendToBattle(res);

	SetChijiNoWarInfo(0, 0, 0, 0);
}

void Player::CreateChijiOccu()
{
	m_ChijiOccuVec.clear();

	std::vector<UInt32>& chiJiVec = JobDataEntryMgr::Instance()->GetChiJiOccuVec();

	if (IsChangeOccu(GetOccu()))
	{
		for (auto& it : chiJiVec)
		{
			if (it == GetOccu())
			{
				m_ChijiOccuVec.push_back(GetOccu());
				break;
			}
		}
	}

	// 吃鸡三个职业选择
	UInt32 randNum = 3 - m_ChijiOccuVec.size();

	std::random_shuffle(chiJiVec.begin(), chiJiVec.end());
	for (auto& iter : chiJiVec)
	{
		if (iter == GetOccu())
			continue;

		m_ChijiOccuVec.push_back(iter);

		if (--randNum == 0)
			break;
	}

	if ( !m_ChijiOccuVec.empty())
	{
		std::random_shuffle(m_ChijiOccuVec.begin(), m_ChijiOccuVec.end());
	}

	CLProtocol::SceneBattleOccuListRes res;
	res.occuVec = m_ChijiOccuVec;
	SendProtocol(res);
}

bool Player::IsHasChijiOccu(UInt32 occu)
{
	for (auto& iter : m_ChijiOccuVec)
	{
		if (iter == occu)
			return true;
	}

	return false;
}

void Player::GetShareData(PlayerSenceShareData& sharedata)
{
	this->GetMasterTaskShareData(sharedata.m_MasterTaskShareData);
}

void Player::SyncShareDataToWorld()
{
	CLProtocol::SceneSyncShareDataToWorld protocol;
	protocol.m_Guid = this->GetID();
	this->GetShareData(protocol.sharedata);
	Router::SendToWorld(protocol);
}

void Player::SyncMasterSysDataToWorld()
{
	CLProtocol::SceneSyncMasterSysDataToWorld protocl;
	protocl.m_roleId = this->GetID();
	protocl.m_AcademicTotalGrowth = this->GetMasterSysAcademicGrowth();
	protocl.m_GoodTeacherValue = this->GetMasterSysGoodTeachValue();
	protocl.m_MasterGiveEquipGrowth = this->GetMasterGiveEquipGrowth();
	Router::SendToWorld(protocl);
}

UInt32 Player::DrawPrizeForHellTicket(UInt32 drawPrizeId, UInt32& rewardId)
{
	// 抽奖次数key
	std::string drawCountName = Avalon::StringUtil::ConvertToString((UInt32)OAT_HELL_TICKET_FOR_DRAW_PRIZE) + OPACT_DARW_PRIZE_NUM;
	if (CounterCfg::Instance()->GetCycleType(drawCountName) == COUNTER_CYCLE_INVALID)
	{
		CounterCfg::Instance()->RegCounter(drawCountName.c_str(), COUNTER_CYCLE_NONE);
	}

	// 用掉的抽奖次数key
	std::string usedDrawCntName = Avalon::StringUtil::ConvertToString((UInt32)OAT_HELL_TICKET_FOR_DRAW_PRIZE) + OPACT_USED_DARW_COUNT;
	if (CounterCfg::Instance()->GetCycleType(usedDrawCntName) == COUNTER_CYCLE_INVALID)
	{
		CounterCfg::Instance()->RegCounter(usedDrawCntName.c_str(), COUNTER_CYCLE_NONE);
	}

	UInt32 drawCnt = GetCount(drawCountName.c_str());
	UInt32 uesdDrawCnt = GetCount(usedDrawCntName.c_str());

	Int32 restNum = (Int32)drawCnt - 1;
	if (restNum < 0)
	{
		ErrorLogStream << PLAYERINFO(this) << "Draw prize num had run out!" << LogStream::eos;
		SendNotify(9056);
		return ITEM_DATA_INVALID;
	}

	UInt32 chargeNum = GetTotalChargeNumOnRole();

	InfoLogStream << PLAYERINFO(this) << "Request draw prize(" << drawPrizeId << "), charge on role(" << chargeNum
		<< "), now draw count(" << drawCnt << "), used draw count(" << uesdDrawCnt << ")." << LogStream::eos;

	RewardPoolDataEntry* rewardData = RewardPoolDataEntryMgr::Instance()->RandReward(drawPrizeId, uesdDrawCnt, chargeNum);
	if (!rewardData)
	{
		ErrorLogStream << PLAYERINFO(this) << "Rand reward failed by draw prize id=" << drawPrizeId << "!" << LogStream::eos;
		return ITEM_DATA_INVALID;
	}

	rewardId = rewardData->dataId;

	SetCounter(drawCountName.c_str(), (UInt32)restNum);
	IncCounter(usedDrawCntName.c_str(), 1);

	//发奖励
	std::string reason = GetReason(SOURCE_TYPE_DRAW_PRIZE, drawPrizeId);
	RewardGroup* grp = GetRewardGroup();
	grp->AddReward(rewardData->itemId, rewardData->itemNum);
	AddRewards(reason.c_str(), grp, false);

	InfoLogStream << PLAYERINFO(this) << "Get reward(id=" << rewardData->dataId << ", itemid=" << rewardData->itemId << ", num=" << rewardData->itemNum << ")!" << LogStream::eos;

	return SUCCESS;
}

UInt32 Player::DrawPrizeForRaffleTicket(UInt32 drawPrizeId, UInt32& rewardId)
{
	DrawPrizeDataEntry* drawPrizeData = DrawPrizeDataEntryMgr::Instance()->FindEntry(drawPrizeId);
	if (!drawPrizeData)
	{
		ErrorLogStream << PLAYERINFO(this) << "Can not find draw prize data by type(" << drawPrizeId << ")!" << LogStream::eos;
		return ITEM_DATA_INVALID;
	}

	if (drawPrizeData->parms.size() != 1)
	{
		ErrorLogStream << PLAYERINFO(this) << "Draw prize data's parms are empty!" << LogStream::eos;
		return ITEM_DATA_INVALID;
	}

	UInt32 raffleTicketId = drawPrizeData->parms.front();
	if (raffleTicketId == 0)
	{
		ErrorLogStream << PLAYERINFO(this) << "Raffle ticket id is zero!" << LogStream::eos;
		return ITEM_DATA_INVALID;
	}

	InfoLogStream << PLAYERINFO(this) << "Request draw prize(" << drawPrizeId << "), raffle ticket item data id=" << raffleTicketId << LogStream::eos;

	class SelectRaffleTicketsVisitor : public ItemVisitor
	{
	public:
		SelectRaffleTicketsVisitor(UInt32 raffleTicketId) : m_RaffleTicket(NULL), m_RaffleTicketId(raffleTicketId){}

		bool operator()(Item *item)
		{
			if (!item)
			{
				ErrorLogStream << "Item ptr is null!" << LogStream::eos;
				return true;
			}

			if (item->GetDataID() == m_RaffleTicketId)
			{
				m_RaffleTicket = item;
				return false;
			}

			return true;
		}

		Item* m_RaffleTicket;
		UInt32 m_RaffleTicketId;
	};

	SelectRaffleTicketsVisitor selectRaffleTicketVisitor(raffleTicketId);
	GetItemMgr().VisitItems((UInt8)PACK_EXPENDABLE, selectRaffleTicketVisitor);

	Item* raffleTicketItem = selectRaffleTicketVisitor.m_RaffleTicket;

	if (raffleTicketItem == NULL)
	{
		ErrorLogStream << PLAYERINFO(this) << "Draw prize num had run out!" << LogStream::eos;
		SendNotify(9881);
		return ITEM_DATA_INVALID;
	}

	UInt16 raffleTicketNum = raffleTicketItem->GetNum();

	if (raffleTicketNum == 0)
	{
		ErrorLogStream << PLAYERINFO(this) << "Draw prize num had run out!" << LogStream::eos;
		SendNotify(9881);
		return ITEM_DATA_INVALID;
	}

	InfoLogStream << PLAYERINFO(this) << "Request draw prize(" << drawPrizeId << "), now raffle ticket num(" << raffleTicketNum << ")." << LogStream::eos;

	RewardPoolDataEntry* rewardData = RewardPoolDataEntryMgr::Instance()->RandReward(drawPrizeId);
	if (!rewardData)
	{
		ErrorLogStream << PLAYERINFO(this) << "Rand reward failed by draw prize id=" << drawPrizeId << "!" << LogStream::eos;
		return ITEM_DATA_INVALID;
	}

	rewardId = rewardData->dataId;

	std::string reason = GetReason(SOURCE_TYPE_DRAW_PRIZE, drawPrizeId);

	//扣抽奖券
	if (ReduceItemNum(reason.c_str(), raffleTicketItem) != SUCCESS) 
	{
		ErrorLogStream << PLAYERINFO(this) << "Reduce item(" << raffleTicketId << ") failed." << LogStream::eos;
		return ITEM_DATA_INVALID;
	}

	//发奖励
	RewardGroup* grp = GetRewardGroup();
	grp->AddReward(rewardData->itemId, rewardData->itemNum);
	AddRewards(reason.c_str(), grp, false);

	InfoLogStream << PLAYERINFO(this) << "Get reward(id=" << rewardData->dataId << ", itemid=" << rewardData->itemId << ", num=" << rewardData->itemNum << ")!" << LogStream::eos;

	return SUCCESS;
}

void Player::SyncArtifactJarDiscountInfo(UInt32 opActId, UInt8 extractDiscountStatus, UInt32 discountRate, UInt32 discountEffectTimes)
{
	CLProtocol::SceneArtifactJarDiscountInfoSync protocol;
	protocol.opActId = opActId;
	protocol.extractDiscountStatus = extractDiscountStatus;
	protocol.discountRate = discountRate;
	protocol.discountEffectTimes = discountEffectTimes;
	SendProtocol(protocol);
}

void  Player::SyncResistMagicToWorld()
{
	CLProtocol::WorldSyncResistMagic protocol;
	protocol.roleId = this->GetID();
	protocol.resist_magic = this->GetResistMagic();

	Router::SendToWorld(protocol);
}

bool Player::SecurityLockCheck(bool _isNotify)
{
	if (!SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_SECURITY_LOCK))
		return true;

	if (GetSecurityStatus() != SECURITY_STATE_UNLOCK)
	{
		//if (_isNotify)
		{
			CLProtocol::WorldSecurityLockForbid notify;
			notify.ret = ErrorCode::SECURITY_LOCK_FORBID_OP;
			SendProtocol(notify);
		}
		return false;
	}

	return true;
}

bool Player::SecurityLockCheckByColor(std::vector<ObjID_t>& uidList, UInt32 _color)
{
	bool isHasPurpleEquip = false;
	for (auto& itemId : uidList)
	{
		Item* item = FindItem(itemId);
		if (NULL == item)
		{
			isHasPurpleEquip = false;
			break;
		}

		if (item->GetDataEntry()->color >= _color)
		{
			isHasPurpleEquip = true;
			break;
		}
	}

	if (isHasPurpleEquip)
	{
		if (!SecurityLockCheck())
			return false;
	}

	return true;
}

UInt32 Player::HandlerItemOpLock(UInt32 opType, ObjID_t itemId)
{
	Item* item = m_ItemMgr.FindItem(itemId);
	if (NULL == item)
		return ErrorCode::ITEM_DATA_INVALID;

	// 解锁
	if (0 == opType)
	{
		// 安全锁状态
		if (GetSecurityStatus() != SECURITY_STATE_UNLOCK)
			return ErrorCode::SECURITY_LOCK_FORBID_OP;
		
		if (item->GetType() == ITEM_FASHION)
		{
			item->ClearItemLock(ILT_FASHION_LOCK);
		}
		else{
			item->ClearItemLock(ILT_ITEM_LOCK);
		}
	}
	else  // 加锁
	{
		if (item->GetType() == ITEM_FASHION)
		{
			item->SetItemLock(ILT_FASHION_LOCK);
		}
		else{
			item->SetItemLock(ILT_ITEM_LOCK);
		}

		// 设置装备锁日志
		SendUdpLog("security_lock", LOG_SECURITY_LOCK, SL_LOG_SET_ITEM_LOCK);
	}

	item->SyncProperty(this);

	return ErrorCode::SUCCESS;
}

UInt32 Player::SetShowFashionWeapon(UInt8 isShow)
{
	if (isShow != 0 && isShow != 1)
	{
		ErrorLogStream << PLAYERINFO(this) << "isShow != 0 && isShow != 1, isShow = " << (UInt32)isShow << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	if (m_ShowFashionWeapon == isShow)
	{
		ErrorLogStream << PLAYERINFO(this) << "m_ShowFashionWeapon == isShow, isShow = " << (UInt32)isShow << ", m_ShowFashionWeapon = " << m_ShowFashionWeapon << LogStream::eos;
	}

	m_ShowFashionWeapon = isShow;

	this->CountAvatar();
	this->SyncProperty();
	return ErrorCode::SUCCESS;
}

void Player::AddChangeFashionScore(const char* reason, UInt32 num)
{
	if (reason == NULL) return;
	OpActivityRegInfo* opAct = ActivityMgr::Instance()->GetOpenActivityRegInfoByTmplateType(this, OAT_CHANGE_FASHION_ACT);
	if (opAct == NULL)
	{
		ErrorLogStream << "curOpenActId = NULL" << LogStream::eos;
		return;
	}
	std::ostringstream actscoreCounter;
	actscoreCounter << ACTIVE_SCORE_PER << opAct->parm;
	const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(actscoreCounter.str());
	if (!counterConfig)
	{
		CounterCfg::Instance()->RegCounter(actscoreCounter.str().c_str(), COUNTER_CYCLE_NONE);
	}
	UInt32 oldScore = this->GetCounter(actscoreCounter.str().c_str());
	UInt32 newScore = 0;
	if (oldScore + num > MAX_MONEY) newScore = MAX_MONEY;
	else newScore = oldScore + num;

	if (newScore > oldScore)
	{
		DebugLogStream << PLAYERINFO(this) << " AddChangeFashionScore old:" << oldScore << " new:" << newScore << " add:" << num << " reason:" << reason << LogStream::eos;
		SendAssetUdpLog(AssetType::ASSET_CHANGEFASHIN_SCORE, reason, (UInt64)oldScore, (Int64)num, (Int64)newScore - (Int64)oldScore, (UInt64)newScore);

		this->SetCounter(actscoreCounter.str().c_str(), newScore);
	}
}

void Player::RemoveChangeFashionScore(const char* reason, UInt32 num)
{
	if (reason == NULL) return;
	OpActivityRegInfo* opAct = ActivityMgr::Instance()->GetOpenActivityRegInfoByTmplateType(this, OAT_CHANGE_FASHION_ACT);
	if (opAct == NULL)
	{
		ErrorLogStream << "curOpenActId = NULL" << LogStream::eos;
		return;
	}
	std::ostringstream actscoreCounter;
	actscoreCounter << ACTIVE_SCORE_PER << opAct->parm;
	const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(actscoreCounter.str());
	if (!counterConfig)
	{
		CounterCfg::Instance()->RegCounter(actscoreCounter.str().c_str(), COUNTER_CYCLE_NONE);
	}

	std::ostringstream actscoreConsumeCounter;
	actscoreConsumeCounter << ACTIVE_SCORE_CONSUME_PER << opAct->parm;
	const CounterConfig* counterConsumeConfig = CounterCfg::Instance()->GetCycleConfig(actscoreConsumeCounter.str());
	if (!counterConsumeConfig)
	{
		CounterCfg::Instance()->RegCounter(actscoreConsumeCounter.str().c_str(), COUNTER_CYCLE_NONE);
	}
	UInt32 oldConsumeScore = this->GetCounter(actscoreConsumeCounter.str().c_str());

	UInt32 oldScore = this->GetCounter(actscoreCounter.str().c_str());
	UInt32 newScore = oldScore;
	if (num >= oldScore) newScore = 0;
	else newScore = oldScore - num;

	if (oldScore > newScore)
	{
		GameInfoLogStream("Property") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") DecChangeFashionScore:"
			<< reason << "|" << oldScore - newScore << "|" << newScore << LogStream::eos;

		SendAssetUdpLog(AssetType::ASSET_CHANGEFASHIN_SCORE, reason, (UInt64)oldScore, (Int64)num * -1, (Int64)newScore - (Int64)oldScore, (Int64)newScore);

		oldConsumeScore += oldScore - newScore;
		this->SetCounter(actscoreConsumeCounter.str().c_str(), oldConsumeScore);
		this->SetCounter(actscoreCounter.str().c_str(), newScore);
	}
}

UInt32 Player::GetChangeFashionScore()
{
	OpActivityRegInfo* opAct = ActivityMgr::Instance()->GetOpenActivityRegInfoByTmplateType(this, OAT_CHANGE_FASHION_ACT);
	if (opAct == NULL)
	{
		ErrorLogStream << "curOpenActId = NULL" << LogStream::eos;
		return 0;
	}

	std::ostringstream actscoreCounter;
	actscoreCounter << ACTIVE_SCORE_PER << opAct->parm;
	const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(actscoreCounter.str());
	if (!counterConfig)
	{
		CounterCfg::Instance()->RegCounter(actscoreCounter.str().c_str(), COUNTER_CYCLE_NONE);
	}
	return this->GetCounter(actscoreCounter.str().c_str());
}

void Player::AddWeaponLeaseTickets(const char* reason, UInt32 num)
{
	if (reason == NULL) return;
	UInt32 oldNum = m_weaponLeaseTickets;
	UInt32 maxNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_WEAPON_LEASE_TICKETS_MAXBUN);
	if (m_weaponLeaseTickets + num > maxNum) m_weaponLeaseTickets = maxNum;
	else m_weaponLeaseTickets = oldNum + num;

	if (m_weaponLeaseTickets > oldNum)
	{
		SyncAccountInfo(ACCOUNT_WEAPON_LEASE_TICKETS, m_weaponLeaseTickets);

		DebugLogStream << PLAYERINFO(this) << " AddWeaponLeaseTickets old:" << oldNum << " new:" << m_weaponLeaseTickets << " add:" << m_weaponLeaseTickets - oldNum << " reason:" << reason << LogStream::eos;
		SendAssetUdpLog(AssetType::ASSET_WEAPON_LEASE_TICKETS, reason, (UInt64)oldNum, (Int64)num, (Int64)m_weaponLeaseTickets - (Int64)oldNum, (UInt64)m_weaponLeaseTickets);
	}
}

void Player::RemoveWeaponLeaseTickets(const char* reason, UInt32 num)
{
	if (reason == NULL) return;
	UInt32 oldNum = m_weaponLeaseTickets;
	if (num >= m_weaponLeaseTickets) m_weaponLeaseTickets = 0;
	else m_weaponLeaseTickets = m_weaponLeaseTickets - num;

	if (oldNum > m_weaponLeaseTickets)
	{
		SyncAccountInfo(ACCOUNT_WEAPON_LEASE_TICKETS, m_weaponLeaseTickets);
		GameInfoLogStream("Property") << "player(" << GetAccID() << "," << GetID() << "," << GetName() << ") RemoveWeaponLeaseTickets:"
			<< reason << "|" << oldNum - m_weaponLeaseTickets << "|" << m_weaponLeaseTickets << LogStream::eos;

		SendAssetUdpLog(AssetType::ASSET_WEAPON_LEASE_TICKETS, reason, (UInt64)oldNum, (Int64)num * -1, (Int64)m_weaponLeaseTickets - (Int64)oldNum, (Int64)m_weaponLeaseTickets);
	}
}

UInt32 Player::GetWeaponLeaseTickets()
{
	return this->m_weaponLeaseTickets;
}

bool Player::IsVeteranReturn()
{
	if (!SSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_NEW_RETURN))
	{
		return false;
	}

	if (!ActivityUtil::IsInVeteranReturnPeriod(GetRoleReturnTime()))
	{
		return false;
	}

	return true;
}

void Player::OnCheckVeteranReturn()
{
	auto activitySystemData = ActivitySystemConfigDataEntryMgr::Instance()->FindEntry((UInt32)ACTEM_VETERAN_RETURN);
	if (!activitySystemData)
	{
		ErrorLogStream << "Can not find activity system data(" << (UInt32)ACTEM_VETERAN_RETURN << ")!" << LogStream::eos;
		return;
	}

	if (activitySystemData->isClose) 
		return;

	if (!IsVeteranReturn())
		return;

	InfoLogStream << PLAYERINFO(this) << " veteran return, time=" << GetRoleReturnTime() << LogStream::eos;

	// 注册运营活动
	GetPlayerActivityInfo().RegisterVeteransReturnOpActivity();
}

UInt32 Player::GetRoleReturnExpireTime()
{
	UInt32 return_days = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_VETERANT_RETURN_DAYS);
	return GetRoleReturnTime() + return_days * DAY_TO_SEC;
}

void Player::SyncAccountCounterToWorld(UInt32 type)
{
	CLProtocol::WorldAccountCounterSync sycn;
	sycn.roleId = GetID();
	sycn.accCounter.type = type;

	if (type == ACC_COUNTER_ENERGY_COIN)
	{
		sycn.accCounter.num = GetEnergyCoin();
	}
	
	Router::SendToWorld(sycn);
}

void Player::RefreshEnergyCoin(UInt32 num)
{
	InfoLogStream << "Energy Coin OldNum:"<<GetEnergyCoin()<<" NewNum:"<<num << LogStream::eos;
	SetEnergyCoin(num);
	SyncAccountCounterToWorld(ACC_COUNTER_ENERGY_COIN);
}

void Player::SetGameSet(UInt32 setType, const std::string& setValue)
{
	if (setType == 0 || setType >= GST_MAX) return;

	// 检查长度,暂定不能大于10
	if (setValue.length() > 10)
		return;

	CLProtocol::SceneGameSetRes res;

	do 
	{
		// 检查字符串是否合法
		auto iter = setValue.find("_");
		if (iter != std::string::npos)
		{
			res.retCode = ErrorCode::ITEM_DATA_INVALID;
			break;
		}

		iter = setValue.find("|");
		if (iter != std::string::npos)
		{
			res.retCode = ErrorCode::ITEM_DATA_INVALID;
			break;
		}

		m_GameSetMap[setType] = setValue;

		UpdateGameSet();
		SyncGameSetWorld();
		SyncProperty();
	} while (0);

	res.retCode = ErrorCode::SUCCESS;
	SendProtocol(res);
}

void Player::ParseGameSet()
{
	std::vector<std::string> param_1;
	Avalon::StringUtil::Split(m_GameSet, param_1, "|");
	for (auto& iter : param_1)
	{
		if (iter.empty())
			continue;

		std::vector<std::string> param_2;
		Avalon::StringUtil::Split(iter, param_2, "_");
		if (param_2.size() != 2)
		{
			ErrorLogStream << "playerId:(" << GetID() << ") game set " << m_GameSet << "error" << LogStream::eos;
			continue;
		}

		UInt32 type = Avalon::StringUtil::ConvertToValue<UInt32>(param_2[0]);
		std::string val = param_2[1];

		m_GameSetMap[type] = val;
	}

	// 屏蔽好友设置检查初始值
	auto iter = m_GameSetMap.find(GST_FRIEND_INVATE);
	if (iter == m_GameSetMap.end())
	{
		m_GameSetMap[GST_FRIEND_INVATE] = "10";
		UpdateGameSet();
	}
}

void Player::UpdateGameSet()
{
	std::string dev_str;
	dev_str.reserve(1024);
	for (auto& iter : m_GameSetMap)
	{
		std::string type = std::to_string(iter.first);
		dev_str.append(type).append("_").append(iter.second).append("|");
	}

	m_GameSet = dev_str;
}

void Player::SyncGameSetWorld()
{
	CLProtocol::SceneGameSetSync sync;
	sync.playerId = GetID();
	sync.gameSet = m_GameSet;
	Router::SendToWorld(sync);
}

void Player::SetRoleValueScore(UInt32 score)
{
	m_RoleValueScore = score;
	SyncRoleValueScore();
}

void Player::SyncRoleValueScore()
{
	CLProtocol::SceneSyncRoleValueScore protocol;
	protocol.roleid = GetID();
	protocol.roleValueScore = GetRoleValueScore();
	Router::SendToWorld(protocol);
}

UInt32 Player::GetPlayerLevelScore()
{
	if (!SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_ADVENTURE_TEAM_SORTLIST))
	{
		return 0;
	}

	UInt32 playerLevelScore = 0;
	auto playerData = PlayerDataEntryMgr::Instance()->FindEntry((UInt32)GetLevel());
	if (playerData)
	{
		playerLevelScore = playerData->score;
	}
	return playerLevelScore;
}

UInt32 Player::CalcItemsScoreInPackage(PackType packType, bool reCalc)
{
	class MyItemVisitor : public ItemVisitor
	{
	public:
		MyItemVisitor(bool reCalc, Player* owner) :scoreSum(0), m_ReCalc(reCalc), m_Owner(owner) {}

		bool operator()(Item *item)
		{
			if (item)
			{
				if (m_ReCalc)
				{
					item->CalcItemScore(m_Owner);
				}
				scoreSum += item->GetItemScore();
			}
			return true;
		}

		UInt32 scoreSum;

	private:
		bool m_ReCalc;
		Player* m_Owner;
	};

	MyItemVisitor visitor(reCalc, this);
	GetItemMgr().VisitItems(packType, visitor);
	return visitor.scoreSum;
}

UInt32 Player::CalcPetsSocre(bool reCalc)
{
	UInt32 petScoreSum = 0;
	std::vector<UInt64> petIds = GetPetMgr().GetBattlePets();
	for (auto petId : petIds)
	{
		auto pet = GetPetMgr().FindPet(petId);
		if (!pet) continue;

		if (reCalc)
		{
			pet->CalcPetScore();
		}
		petScoreSum += pet->GetPetScore();
	}
	return petScoreSum;
}

void Player::CalcRoleValueScore(bool reCalc)
{
	if (SERVER_TYPE == ST_BSCENE)
	{
		return;
	}

	if (!SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_ADVENTURE_TEAM))
	{
		return;
	}

	if (!SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_ADVENTURE_TEAM_SORTLIST))
	{
		return;
	}

	// 装备、称号评分
	UInt32 warePackSocre = CalcItemsScoreInPackage(PACK_WEAR, reCalc);
	// 时装评分
	UInt32 fashionPackScore = CalcItemsScoreInPackage(PACK_FASHION_WEAR, reCalc);
	// 等级评分
	UInt32 playerLevelScore = GetPlayerLevelScore();
	// 宠物评分
	UInt32 petScoreSum = CalcPetsSocre(reCalc);

	UInt32 roleScore = warePackSocre + fashionPackScore + playerLevelScore + petScoreSum;
	SetRoleValueScore(roleScore);
	InfoLogStream << PLAYERINFO(this) << " calculate role value score(" << roleScore << "," << warePackSocre << "," 
		<< fashionPackScore << "," << playerLevelScore << "," << petScoreSum << ")" << LogStream::eos;
}

void Player::UpdatePackageTypeSize()
{
	std::string str;
	str.reserve(1024);
	for (auto& iter : m_PackageTypeSizeMap)
	{
		std::string type = std::to_string(iter.first);
		std::string val = std::to_string(iter.second);
		str.append(type).append(",").append(val).append("|");
	}

	str.pop_back();
	m_PackageTypeStr = str;
}

void Player::OnNewTitleModify(UInt8 opType, UInt8 ownerType, UInt64 titleGuid, std::string titleName)
{
	if (opType == NTOPP_UP || opType == NTOPP_SYNC)
	{
		m_NewTitleGuid = titleGuid;
		m_NewTitleName = titleName;
	}
	else if (opType == NTOPP_OFF)
	{
		m_NewTitleGuid = 0;
		m_NewTitleName = "";
	}
	else
	{
		return;
	}

	SyncProperty();
	CLProtocol::SceneNewTitleModifyWorld protocl;
	protocl.roleId = GetID();
	protocl.opType = opType;
	protocl.ownerType = ownerType;
	protocl.titleGuid = m_NewTitleGuid;
	protocl.titleName = m_NewTitleName;

	Router::SendToWorld(protocl);
}

void Player::AddNewTitle(UInt32 titleId, UInt32 dueTime)
{
	CLProtocol::SceneNewTitleAddWorld protocl;
	protocl.roleId = this->GetID();
	protocl.titleId = titleId;
	protocl.dueTime = dueTime;

	Router::SendToWorld(protocl);
}

RacePlayerInfo Player::GetLostBattleRacePlayerInfo() 
{ 
	//检查神器
	class Visitor : public ItemVisitor
	{
	public:
		Visitor()
		{}
		bool operator()(Item* item)
		{
			if (!item) return true;
			RaceItem raceItem;
			raceItem.id = item->GetDataID();
			raceItem.num = item->GetNum();
			raceItems.push_back(raceItem);
			return true;
		}
	public:
		ItemRewardVec rewards;
		std::vector<RaceItem> raceItems;
	};

	Visitor visitor;
	this->VisitItems(PACK_LOSTDUNGEON, visitor);
	RacePlayerInfo raceInfo = m_lostBattleRacePlayerInfo;
	raceInfo.lostDungArtifacts = visitor.raceItems;

	//检查buff
	GetRaceBuffs(raceInfo.buffs);

	//血量，蓝量
	raceInfo.remainHp = GetLostDungeonHp();
	raceInfo.remainMp = GetLostDungeonMp();
	return raceInfo;
}

Int32 Player::GetLostDungeonHp() const
{
	return m_LostDungeonHp;
}

void Player::SetLostDungeonHp(Int32 hp)
{
	m_LostDungeonHp = hp;

	if (m_LostDungeonHp > HP_MP_FULL_VALUE)
	{
		DevLogStream << "player(" << PLAYERINFO(this) << ") invalid hp(" << m_LostDungeonHp << ")." << LogStream::eos;
		m_LostDungeonHp = HP_MP_FULL_VALUE;
	}
}

void Player::DecLostDungeonHp(Int32 value)
{
	if (value < 0)
	{
		return;
	}

	m_LostDungeonHp = m_LostDungeonHp - value;
	m_LostDungeonHp = m_LostDungeonHp < 0 ? 0 : m_LostDungeonHp;
}

void Player::IncLostDungeonHp(Int32 value)
{
	if (value < 0)
	{
		return;
	}

	SetLostDungeonHp(GetLostDungeonHp() + value);
}

Int32 Player::GetLostDungeonMp() const
{
	return m_LostDungeonMp;
}

void Player::SetLostDungeonMp(Int32 mp)
{
	m_LostDungeonMp = mp;

	if (m_LostDungeonMp > HP_MP_FULL_VALUE)
	{
		DevLogStream << "player(" << PLAYERINFO(this) << ") invalid mp(" << m_LostDungeonMp << ")." << LogStream::eos;
		m_LostDungeonMp = HP_MP_FULL_VALUE;
	}
}

void Player::DecLostDungenMp(Int32 value)
{
	if (value < 0)
	{
		return;
	}

	m_LostDungeonMp = m_LostDungeonMp - value;
	m_LostDungeonMp = m_LostDungeonMp < 0 ? 0 : m_LostDungeonMp;
}

void Player::IncLostDungeonMp(Int32 value)
{
	if (value < 0)
	{
		return;
	}

	SetLostDungeonMp(GetLostDungeonMp() + value);
}

void Player::OnEnterLostDungeon(bool reconnect)
{
	SyncSelfData(SST_LOSTDUNG_ENTER_SYNC, false);
}

bool Player::IsCompLostDungeonTask()
{ 
	return GetLostDungeonTaskMgr().IsFinishTask();
}

Int32 Player::GetLostDungeonHpBack() const
{
	return m_LostDungHpBack;
}

void Player::SetLostDungeonHpBack(Int32 hp)
{
	m_LostDungHpBack = hp;

	if (m_LostDungHpBack > HP_MP_FULL_VALUE)
	{
		DevLogStream << "player(" << PLAYERINFO(this) << ") invalid mp(" << m_LostDungHpBack << ")." << LogStream::eos;
		m_LostDungHpBack = HP_MP_FULL_VALUE;
	}
}

Int32 Player::GetLostDungeonMpBack() const
{
	return m_LostDungMpBack;
}

void Player::SetLostDungeonMpBack(Int32 mp)
{
	m_LostDungMpBack = mp;

	if (m_LostDungMpBack > HP_MP_FULL_VALUE)
	{
		DevLogStream << "player(" << PLAYERINFO(this) << ") invalid mp(" << m_LostDungMpBack << ")." << LogStream::eos;
		m_LostDungMpBack = HP_MP_FULL_VALUE;
	}
}

void Player::SetLostDungeonSpeedBack(UInt32 speed)
{
	m_LostDungSpeedBack = speed;
}

UInt32 Player::GetLostDungeonSpeedBack()
{
	return m_LostDungSpeedBack;
}

UInt32 Player::OnPlayerSeeIntell(UInt64 targetId, std::vector<ItemReward>& artifacts)
{
	Player* target = PlayerMgr::Instance()->FindPlayer(targetId);
	if (!target) return  ErrorCode::ITEM_INVALID_INDEX;

	UInt32 itemId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_LOSTDUNG_DUNG_QIBAO_ITEM);

	if (GetItemNum(itemId) < 1)
	{
		return ErrorCode::LOSTDUNGEON_SEE_QIBAO_FAIK_NOITEM;
	}

	class Visitor : public ItemVisitor
	{
	public:
		Visitor(std::vector<ItemReward>& artifacts_) : m_artifacts(artifacts_){}
		~Visitor() {}

		bool operator()(Item *item)
		{
			if (!item)
			{
				return true;
			}
			ItemReward itemReaward(item->GetDataID(), item->GetNum(), 0, 0);
			m_artifacts.push_back(itemReaward);
			return true;
		}

	public:
		std::vector<ItemReward>& m_artifacts;
	};
	Visitor visitor(artifacts);
	target->GetItemMgr().VisitItems(PACK_LOSTDUNGEON, visitor);
	return ErrorCode::SUCCESS;
}

void Player::GetLostDunegonTaskKillTargetPos(std::vector<UInt64>& players, std::vector<LostDungeonPlayerPos>& poss)
{
	std::set<UInt64>& killRoles = GetLostDungeonTaskMgr().GetKillRoles();
	if (killRoles.size() == 0)
	{
		return;
	}
	for (auto playerId : players)
	{
		auto it = killRoles.find(playerId);
		if (it == killRoles.end())
		{
			continue;
		}
		auto player = PlayerMgr::Instance()->FindNormalPlayer(playerId);
		if(!player) continue;
		LostDungeonPlayerPos playerPos;
		playerPos.playerId = playerId;
		if (player->GetScene())
		{
			playerPos.sceneId = player->GetScene()->GetMapID();
			playerPos.posX = player->GetScenePos().GetX();
			playerPos.posY = player->GetScenePos().GetY();
		}
		poss.push_back(playerPos);
	}
}