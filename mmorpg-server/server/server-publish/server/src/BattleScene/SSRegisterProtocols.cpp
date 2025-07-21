#include "SSNetwork.h"
#include "PlayerMgr.h"

#include "SceneHandler.h"
#include "SSGateHandler.h"
#include "SSEnterGameHandler.h"
#include "SSLeaveGameHandler.h"
#include "SSMoveHandler.h"
#include "SceneMailHandler.h"
#include "SceneChatHandler.h"
#include "SceneTeamHandler.h"
#include "SceneSyncObjectHandler.h"
#include "SceneRelationHandler.h"
#include "SceneItemHandler.h"
#include "SceneSortListHandler.h"
#include "SceneRoutineHandler.h"
#include "SceneSysHandler.h"
#include "SceneAuctionHandler.h"
#include "SceneGiftBagHandler.h"
#include "SceneConsoleHandler.h"
#include "SceneInviteHandler.h"
#include "SceneCrossHandler.h"
#include "SceneBattleHandler.h"
#include "SceneChangeZoneHandler.h"
#include "SceneRecordHandler.h"
#include "SceneDungeonHandler.h"
#include "SceneMatchHandler.h"
#include "SceneQuickBuyHandler.h"
#include "SceneGuildHandler.h"
#include "SceneBillingHandler.h"
#include "SceneOperateHandler.h"
#include "SceneReplayHandler.h"
#include "SceneRoomHandler.h"
#include "SceneSysRecordHandler.h"
#include "SceneTaskHandler.h"
#include "SceneSecurityLockHandler.h"
#include "SceneAdventureTeamHandler.h"
#include "SceneNewTitleHandler.h"
#include "SceneLostDungeonHandler.h"

template<typename ProtocolT>
class SSPacketFunction : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const
	{
		ProtocolT protocol;
		if(protocol.Decode(packet))
		{
			HandleProtocol(protocol);
		}
	}
};

#define SS_REGISTER_PROTOCOL(ProtocolT) \
	RegisterProtocol(ProtocolT::__ID,new SSPacketFunction<ProtocolT>);

void SSNetwork::RegisterProtocols()
{
	SS_REGISTER_PROTOCOL(CLProtocol::GateTransmitPacket)
	SS_REGISTER_PROTOCOL(CLProtocol::SysTransmitGmCommand)
    SS_REGISTER_PROTOCOL(CLProtocol::SysAsyncWaitMsgRet)

	SS_REGISTER_PROTOCOL(CLProtocol::SceneNotifyLoadmap)
    SS_REGISTER_PROTOCOL(CLProtocol::SceneNotifyLoadDungeonEntryMap)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldRegisterSceneRet)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldUnregisterSceneRet)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneNotifyLoadDynMap)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneNotifyLoadBattleScene)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneSetParam)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneSummonNpc)

	SS_REGISTER_PROTOCOL(CLProtocol::SceneEnterGameReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneLeaveGameReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneLogicLeaveGameReq)

    SS_REGISTER_PROTOCOL(CLProtocol::SceneGameReconnectReq)
    SS_REGISTER_PROTOCOL(CLProtocol::SceneGameReconnectRet)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldRegisterPlayerRet)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneBeginChangeSceneReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneBeginChangeSceneRet)
	RegisterProtocol(CLProtocol::SCENE_TRANS_OBJECTDATA, new TransObjectDataFunction()); //noted by aprilliu TransObjectDataFunction 并非继承与SSPacketFunction，所以不能用宏SS_REGISTER_PROTOCOL
	SS_REGISTER_PROTOCOL(CLProtocol::SceneChangeScene)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneEnterScenePullPlayer)
	SS_REGISTER_PROTOCOL(CLProtocol::ScenePlayerPosReq)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldMatchRaceStart);

	SS_REGISTER_PROTOCOL(CLProtocol::SceneMailCheckPackageReq)
	RegisterProtocol(CLProtocol::SCENE_MAIL_GIVEITEMS, new MailGiveItemsFunction());
	RegisterProtocol(CLProtocol::WS_GUILD_GETSTORAGEITEM_REQ, new GetGuildStorageItemFunction());
	SS_REGISTER_PROTOCOL(CLProtocol::WSShopBuyAddGuildStorageRes)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneMailRemoveCostReq)

	SS_REGISTER_PROTOCOL(CLProtocol::SceneAuctionCheckPackageReq)

	SS_REGISTER_PROTOCOL(CLProtocol::WorldSyncTeamMemberStatusToScene)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldSyncTeamInfoToScene)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldNotifyDelTeamToScene)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldSyncTeamBaseToScene)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldNotifyMemberInToScene)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldNotifyMemberOutToScene)
    SS_REGISTER_PROTOCOL(CLProtocol::WorldServerTeamCreateRaceRes)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldServerTeamStartDungeonRes)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldTeamQueryTargetCondReq)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldTeamDungeonRaceStart)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldTeamDungeonRaceOver)

	SS_REGISTER_PROTOCOL(CLProtocol::SceneSyncSocialData)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldAddExpReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneWatchPlayerReq)	//废弃
	SS_REGISTER_PROTOCOL(CLProtocol::SceneSyncPunishment)
	SS_REGISTER_PROTOCOL(CLProtocol::ScenePlayerPowerReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneSyncEvent)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldAccountCounterSync)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldAccountDataSync)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneGameParamSync)

	SS_REGISTER_PROTOCOL(CLProtocol::SceneRelationSyncList)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneRelationNotifyNew)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneRelationNotifyDel)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneRelationSyncIntimacy)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneRelationFindPlayerReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneRelationPresentToFriendReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneRelationPresentToFriendRet)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneQueryPlayerReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneQueryPlayerDetailsReq)

	SS_REGISTER_PROTOCOL(CLProtocol::SceneCheckItemNumReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneRemoveItemReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneUnlockItemReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneRemoveItemObjReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneAddMoneyItemReq)

	RegisterProtocol(CLProtocol::SCENE_SYNC_SORTLISTS, new SyncSortListFunction());
	SS_REGISTER_PROTOCOL(CLProtocol::SceneSyncSortListMinValue)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneSyncSortListEntry)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneSortListWatchDataReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneSortListDailyReward)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneSortListResetSeason)

	SS_REGISTER_PROTOCOL(CLProtocol::WorldMallCheckBuy)	
	SS_REGISTER_PROTOCOL(CLProtocol::WSRefreshLimiteBuyNum)
	SS_REGISTER_PROTOCOL(CLProtocol::WolrdLoadConditionActivateDBNotify)

	SS_REGISTER_PROTOCOL(CLProtocol::WorldNotifySceneActivity)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldSyncSceneActivities)

	SS_REGISTER_PROTOCOL(CLProtocol::WorldAuctionCheck)

	SS_REGISTER_PROTOCOL(CLProtocol::SceneBillingSendGoods)

	SS_REGISTER_PROTOCOL(CLProtocol::WorldNotifyGameStartTimeToS)

	SS_REGISTER_PROTOCOL(CLProtocol::SceneConsoleQueryPlayer)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneConsoleReloadScripts)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneConsoleControlPlayer)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneConsolePhoneBind)
	SS_REGISTER_PROTOCOL(CLProtocol::AdminConsoleRequest)

	SS_REGISTER_PROTOCOL(CLProtocol::WorldNotifyWorldLevelToS)
	SS_REGISTER_PROTOCOL(CLProtocol::SysServiceSwitchChangeStatus)
	SS_REGISTER_PROTOCOL(CLProtocol::SysDungeonConfigUpdate)
	SS_REGISTER_PROTOCOL(CLProtocol::SysDungeonOpenStatusChange)
	SS_REGISTER_PROTOCOL(CLProtocol::SysPlayerForeverDeleted)
	SS_REGISTER_PROTOCOL(CLProtocol::SysTransmitToPlayer)

	SS_REGISTER_PROTOCOL(CLProtocol::SocialInviteInviteeListRet)
	SS_REGISTER_PROTOCOL(CLProtocol::SocialInviteCheckGiftbagRet)
	SS_REGISTER_PROTOCOL(CLProtocol::SocialInviteCheckRecallAwardRet)
	SS_REGISTER_PROTOCOL(CLProtocol::SocialInviteCheckConsumeAwardRet)
	SS_REGISTER_PROTOCOL(CLProtocol::SocialInviteCheckKLActivityRet)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneInviteBeginExchangeCDKeyRet)

	/*
	SS_REGISTER_PROTOCOL(CLProtocol::SceneWorldChallengeSigninReq)
	RegisterProtocol(CLProtocol::SCENE_WORLDCHALLENGE_SEND_CHALLENGERS, new SendChallengersFunction());
	RegisterProtocol(CLProtocol::WorldChallengeEnterSceneReq::__ID, new WorldChallengeEnterSceneFunction());
	SS_REGISTER_PROTOCOL(CLProtocol::ChallengeWorldChallengeSyncResult)
	SS_REGISTER_PROTOCOL(CLProtocol::ChallengeWorldChallengeNotifyBattle)

	*/
	RegisterProtocol(CLProtocol::CrossEnterSceneReq::__ID, new WorldCrossEnterSceneFunction());
	SS_REGISTER_PROTOCOL(CLProtocol::CrossEnterSceneRes)
	SS_REGISTER_PROTOCOL(CLProtocol::CrossExitSceneReq)

	if (SERVER_TYPE == ST_WSCENE)
	{
		SS_REGISTER_PROTOCOL(CLProtocol::CrossExitSceneRes)
	}
	SS_REGISTER_PROTOCOL(CLProtocol::SysTransmitPlayerProtocolInNode)
	
	RegisterProtocol(CLProtocol::BattleEnterSceneReq::__ID, new BattleEnterSceneFunction());
	SS_REGISTER_PROTOCOL(CLProtocol::BattleEnterSceneRes)
	SS_REGISTER_PROTOCOL(CLProtocol::BattleExitSceneReq)
	if (SERVER_TYPE == ST_BSCENE)
	{
		SS_REGISTER_PROTOCOL(CLProtocol::BattleExitSceneRes)
		SS_REGISTER_PROTOCOL(CLProtocol::SceneBattleCreateBattleReq)
		SS_REGISTER_PROTOCOL(CLProtocol::SceneBattleDestroyBattle)
		SS_REGISTER_PROTOCOL(CLProtocol::SceneBattleKillSomeOne)
		SS_REGISTER_PROTOCOL(CLProtocol::SceneBattleNotifyWaveInfo)
		SS_REGISTER_PROTOCOL(CLProtocol::SceneBattleBalanceLog)
		SS_REGISTER_PROTOCOL(CLProtocol::BattleCheckNoWarReq)
		SS_REGISTER_PROTOCOL(CLProtocol::BattleShopNotify)
		SS_REGISTER_PROTOCOL(CLProtocol::BattleSynLostDungTeamSt)
	}

	SS_REGISTER_PROTOCOL(CLProtocol::GateChangeZoneCheckRet)
	SS_REGISTER_PROTOCOL(CLProtocol::RecordNotifyOtherDisconnected)

    SS_REGISTER_PROTOCOL(CLProtocol::SceneDungeonChestRes)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldDungeonStartRaceRes)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldDungeonNotifyRaceEnd)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldDungeonNotifyClearAreaMonster)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldDungeonNotifyConsumeFatigue)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldDungeonNotifyKillMonster)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldDungeonNotifyGotItems)

    SS_REGISTER_PROTOCOL(CLProtocol::SysTransmitRaceProtocolToScenePlayer)
    SS_REGISTER_PROTOCOL(CLProtocol::MatchServerQueryPlayerInfoReq)
    SS_REGISTER_PROTOCOL(CLProtocol::MatchServerRaceEnd)
    SS_REGISTER_PROTOCOL(CLProtocol::MatchServerDungeonRaceEnd)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldChangeMatchScore)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneNotifySeason)

	SS_REGISTER_PROTOCOL(CLProtocol::SceneQuickBuyTransactionRes)

	SS_REGISTER_PROTOCOL(CLProtocol::WorldGuildSyncToScene)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldGuildEmblemSync)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldGuildSyncContribution)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldGuildUpdateSkill)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldGuildGiveItemReq)
	SS_REGISTER_PROTOCOL(CLProtocol::WSEnlargeStorageRet)
	SS_REGISTER_PROTOCOL(CLProtocol::WSAddGuildStorageItemReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneAbnormalTransRemoveMailItemRes)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneSysRecordDataRes)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldNotifyQueryWaitHandleAbnormalTrans)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldAdventureTeamInfoSync)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldIncreasePlayerExpReq)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldSyncPlayerGMAuthority)
	

	//运营活动
	SS_REGISTER_PROTOCOL(CLProtocol::WSSyncOpActivityData)
	SS_REGISTER_PROTOCOL(CLProtocol::WSOpActivityChange)
	SS_REGISTER_PROTOCOL(CLProtocol::CSCDKVerifyRes)
	SS_REGISTER_PROTOCOL(CLProtocol::ReplayListRes)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldCheckMasterGiveRes)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldActivityTaskSendRewards)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldFirstReturnSync)

	//房间
	SS_REGISTER_PROTOCOL(CLProtocol::SceneRoomStartMatch)
	SS_REGISTER_PROTOCOL(CLProtocol::CrossRoomMatchInfoReq)
	SS_REGISTER_PROTOCOL(CLProtocol::MatchServerPkRoomRaceEnd)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneRoomSyncInfo)

	//师门任务
	SS_REGISTER_PROTOCOL(CLProtocol::WorldNotifyPublishMasterTaskToScene)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldGetDiscipleMasterTasksToSceneReq)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldCheckMasterDailyTaskReqToScene)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldNotifyGiveMasterSysGiftReqToScene)

	//挖宝
	SS_REGISTER_PROTOCOL(CLProtocol::SceneAddRewardReq)

	SS_REGISTER_PROTOCOL(CLProtocol::SceneSyncChat)

	// 安全锁
	SS_REGISTER_PROTOCOL(CLProtocol::WorldSecurityStatusSync);

	//item check world->client
	SS_REGISTER_PROTOCOL(CLProtocol::SceneCheckItemReq);

	SS_REGISTER_PROTOCOL(CLProtocol::WorldCheckRechargePushReq);
	SS_REGISTER_PROTOCOL(CLProtocol::WorldSceneBuyRechargePushItemReq);
	
	//头衔
	SS_REGISTER_PROTOCOL(CLProtocol::WorldNewTitleModifyScene);

	//查询战场玩家信息
	SS_REGISTER_PROTOCOL(CLProtocol::BattleQuerylayerDetailOne);

	//通知组队地下城比赛结束
	SS_REGISTER_PROTOCOL(CLProtocol::BattleNotifyTeamRaceEnd);

	//同步玩家战斗数据
	SS_REGISTER_PROTOCOL(CLProtocol::BattleSyncRacePlayerInfo);
}
