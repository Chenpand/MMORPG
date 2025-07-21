#include "WSNetwork.h"

#include "WSAdminHandler.h"
#include "WSSysHandler.h"
#include "WSSceneHandler.h"
#include "WSGateHandler.h"
#include "WSEnterGameHandler.h"
#include "WSLeaveGameHandler.h"
#include "WSMoveHandler.h"
#include "WSChatHandler.h"
#include "WorldMailHandler.h"
#include "WSChatHandler.h"
#include "WorldSyncObjectHandler.h"
#include "WSRelationHandler.h"
#include "WSItemHandler.h"
#include "WorldSortListHandler.h"
#include "WorldMallHandler.h"
#include "WSConsoleHandler.h"
#include "WSTeamHandler.h"
#include "WorldAuctionHandler.h"
#include "WSBillingHandler.h"
#include "WSCenterHandler.h"
#include "WorldSettingHandler.h"
#include "WSActivityHandler.h"
#include "WSWorldChallengeHandler.h"
#include "WSChangeZoneHandler.h"
#include "WorldMatchHandler.h"
#include "WorldDungeonHandler.h"
#include "WSAsyncMsgHandler.h"
#include "WorldGuildHandler.h"
#include "AnnouncementMgr.h"
#include "WorldRedPacketHandler.h"
#include "CLOpActiveProtocol.h"
#include "WorldOperateHandler.h"
#include "WorldVerifyHandler.h"
#include "WorldPremiumLeagueHandler.h"
#include "WorldRoomHandler.h"
#include "WorldGlobalActivityHandler.h"
#include "WorldSysRecordHandler.h"
#include "CLScoreWarProtocol.h"
#include "WorldSecurityLockHandler.h"
#include "WorldAdventureTeamHandler.h"
#include "WorldTaskHandler.h"
#include "WorldNewTitleHandler.h"
#include "WorldDailyTodoHandler.h"
#include "CL2V2ScoreWarProtocol.h"
#include "WorldHireHandler.h"
#include "WorldMonopolyHandler.h"

template<typename ProtocolT>
class WSPacketFunction : public CLPacketFunction
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

#define WS_REGISTER_PROTOCOL(ProtocolT) \
	RegisterProtocol(ProtocolT::__ID,new WSPacketFunction<ProtocolT>);

void WSNetwork::RegisterProtocols()
{
	WS_REGISTER_PROTOCOL(CLProtocol::SysTransmitInNodes)
	WS_REGISTER_PROTOCOL(CLProtocol::SysTransmitInNodesType)
	WS_REGISTER_PROTOCOL(CLProtocol::SysTransmitToPlayer)
	WS_REGISTER_PROTOCOL(CLProtocol::SysTransmitGmCommand)
	WS_REGISTER_PROTOCOL(CLProtocol::SysTransferProtocol)

    WS_REGISTER_PROTOCOL(CLProtocol::SysAsyncWaitMsg)

	WS_REGISTER_PROTOCOL(CLProtocol::AdminPlayerGateInfoReq)

	WS_REGISTER_PROTOCOL(CLProtocol::GateTransmitPacket)
	WS_REGISTER_PROTOCOL(CLProtocol::GateNotifyDisconnect)
	WS_REGISTER_PROTOCOL(CLProtocol::GateNotifyReconnectSuccess)

	WS_REGISTER_PROTOCOL(CLProtocol::WorldRegisterSceneReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldUnregisterSceneReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldEnterDynSceneReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldRegisterNpcPos)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneSyncIdSeed)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneSyncPlayerNum)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneNotifyWorldEnterScene)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneNotifyWorldLoadStart)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneNotifyWorldLoadFinish)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneNotifyWorldLeave)
	WS_REGISTER_PROTOCOL(CLProtocol::ScenePushNotifyToWorld)
	
	WS_REGISTER_PROTOCOL(CLProtocol::WorldVerifyUniqueReq)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneEnterGameReq)
    WS_REGISTER_PROTOCOL(CLProtocol::SceneGameReconnectRet)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldRegisterPlayerReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldLeaveGameReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldUnregisterPlayerReq)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneSyncShareDataToWorld)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldPlayerLeaveLoginQueue)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGetLoginPushInfoReq)

	WS_REGISTER_PROTOCOL(CLProtocol::SceneBeginChangeSceneReq)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneChangeScene)

	RegisterProtocol(CLProtocol::WORLD_CACHE_CHATDATA_REQ, new CacheChatDataFunction());

	WS_REGISTER_PROTOCOL(CLProtocol::SceneMailCheckPackageRet)
	RegisterProtocol(CLProtocol::SCENE_MAIL_REMOVECOST_RET, new MailRemoveCostRetFunction());
	RegisterProtocol(CLProtocol::WORLD_SEND_SYSMAIL, new WorldSendSysMailFunction());
	//WS_REGISTER_PROTOCOL(CLProtocol::WorldSendSysMail)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldSendSysMailToTribeLeader)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldTeamCopySendMail)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldUnionSendMail)
	RegisterProtocol(CLProtocol::WORLD_TEST_ITEM_MAIL, new TestItemMailFunction());

	WS_REGISTER_PROTOCOL(CLProtocol::SceneAuctionCheckPackageRet)

	RegisterProtocol(CLProtocol::WORLD_TRANSMIT_CHAT, new TransChatFunction());
	WS_REGISTER_PROTOCOL(CLProtocol::WorldTransmitRequest)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldTransmitReply)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldSendOfflineNotify)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldSendAnnouncement)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldChatHorn)

	WS_REGISTER_PROTOCOL(CLProtocol::WorldSyncPlayerBaseInfo)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldNotifyKillPlayer)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneSyncEvent)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneSyncOfflineEvent)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneSyncPlayerAvatar)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneSyncAccountInfo)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneSyncRoleValueScore)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneGameParamSync)

	WS_REGISTER_PROTOCOL(CLProtocol::SceneQueryPlayerRet)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneQueryPlayerDetailsRet)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneRelationFindPlayerRet)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldRelationAddIntimacy)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneRelationPresentToFriendReq)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneRelationPresentToFriendRet)

	WS_REGISTER_PROTOCOL(CLProtocol::SceneCheckItemNumRet)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneQuickBuyTransactionReq)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneGetCreditPoint);

	WS_REGISTER_PROTOCOL(CLProtocol::SceneNotifyChangeNameToWorld)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneNotifyChangeSexToWorld)

	WS_REGISTER_PROTOCOL(CLProtocol::WorldSortListUpdateReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldSortListOwnerChangedReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldSortListDeleteEntryReq)
	RegisterProtocol(CLProtocol::SCENE_SORTLIST_WATCH_DATA_RET, new SortListWatchDataFunction());
	WS_REGISTER_PROTOCOL(CLProtocol::WorldSortListVisitRet)

	WS_REGISTER_PROTOCOL(CLProtocol::WorldMallCheckBuyRet)
	WS_REGISTER_PROTOCOL(CLProtocol::SWRefreshLimiteBuyNum)
	WS_REGISTER_PROTOCOL(CLProtocol::MallGiftPackActivateReq)
	WS_REGISTER_PROTOCOL(CLProtocol::MallActivateCondReq)

	WS_REGISTER_PROTOCOL(CLProtocol::WorldCheckRechargePushRes)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldSceneBuyRechargePushItemRes)

	WS_REGISTER_PROTOCOL(CLProtocol::AdminConsoleRequest)

	WS_REGISTER_PROTOCOL(CLProtocol::WorldTeamBroadcastNotify)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldSceneCreateTeamReq)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneSyncTeamMemberAvatar)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldServerTeamStartDungeonReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldTeamMatchStartReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldTeamQueryTargetCondRes)
	
	RegisterProtocol(CLProtocol::WORLD_AUCTION_CHECK_RET, new AuctionCheckRetFunction());

	WS_REGISTER_PROTOCOL(CLProtocol::SceneBillingSendGoodsRes)

	WS_REGISTER_PROTOCOL(CLProtocol::CenterForbidTalkListRet)
	WS_REGISTER_PROTOCOL(CLProtocol::CenterNotifyNewForbidTalk)
	WS_REGISTER_PROTOCOL(CLProtocol::CenterNotifyDelForbidTalk)

	WS_REGISTER_PROTOCOL(CLProtocol::WorldSyncActivesCounter)

	/*
	WS_REGISTER_PROTOCOL(CLProtocol::ChallengeWorldChallengeSigninRet)
	WS_REGISTER_PROTOCOL(CLProtocol::ChallengeWorldChallengeInfoRet)
	WS_REGISTER_PROTOCOL(CLProtocol::ChallengeWorldChallengeSyncResult)
	WS_REGISTER_PROTOCOL(CLProtocol::ChallengeWorldChallengeSortListRet)
	WS_REGISTER_PROTOCOL(CLProtocol::ChallengeWorldChallengeGetAwardRet)
	WS_REGISTER_PROTOCOL(CLProtocol::ChallengeWorldChallengeNotifyBattle)
	*/

	WS_REGISTER_PROTOCOL(CLProtocol::GateChangeZoneCheckRet)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldChangeZoneNotify)

    WS_REGISTER_PROTOCOL(CLProtocol::SysTransmitRaceProtocolInNode)
    WS_REGISTER_PROTOCOL(CLProtocol::SysTransmitRaceProtocolToScenePlayer)
	WS_REGISTER_PROTOCOL(CLProtocol::MatchServerMatchStartReq)
	WS_REGISTER_PROTOCOL(CLProtocol::MatchServerMatchStartRes)
    WS_REGISTER_PROTOCOL(CLProtocol::MatchServerMatchCancelReq)
    WS_REGISTER_PROTOCOL(CLProtocol::MatchServerQueryPlayerInfoReq)
    WS_REGISTER_PROTOCOL(CLProtocol::MatchServerQueryPlayerInfoRes)
    WS_REGISTER_PROTOCOL(CLProtocol::WorldNotifyRaceStart)
    WS_REGISTER_PROTOCOL(CLProtocol::MatchServerRaceEnd)
	WS_REGISTER_PROTOCOL(CLProtocol::MatchServerPkRoomRaceEnd)
    WS_REGISTER_PROTOCOL(CLProtocol::MatchServerDungeonRaceEnd)
    WS_REGISTER_PROTOCOL(CLProtocol::WorldServerTeamCreateRaceReq)
    WS_REGISTER_PROTOCOL(CLProtocol::WorldServerTeamCreateRaceRes)
	WS_REGISTER_PROTOCOL(CLProtocol::MatchServerMatchDungeonRes)
	WS_REGISTER_PROTOCOL(CLProtocol::MatchServerGuildBattleRaceEnd)
	WS_REGISTER_PROTOCOL(CLProtocol::MatchServerPremiumLeagueRaceEnd)
	WS_REGISTER_PROTOCOL(CLProtocol::RelayServerNotifyRaceStart)
	WS_REGISTER_PROTOCOL(CLProtocol::MatchServerPlayerMatchSuccessNotify)
	WS_REGISTER_PROTOCOL(CLProtocol::MatchServerPlayerMatchCancelNotify)
	WS_REGISTER_PROTOCOL(CLProtocol::MatchServerPkRoomReq)

	WS_REGISTER_PROTOCOL(CLProtocol::SceneGotRaceItem)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldDungeonHardUpdate)
    WS_REGISTER_PROTOCOL(CLProtocol::SceneDungeonChestReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldDungeonStartRaceReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldDungeonLeaveRace)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldDungeonNotifyResetAreaIndex)

	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildSyncSkill)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildSyncContribution)

	WS_REGISTER_PROTOCOL(CLProtocol::SceneAddRedPacketReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildGiveItemRes)
	WS_REGISTER_PROTOCOL(CLProtocol::SyncGuildEventToWorld)

	WS_REGISTER_PROTOCOL(CLProtocol::SWEnlargeStorageReq)

	WS_REGISTER_PROTOCOL(CLProtocol::WorldAddJarRecordReq)
	WS_REGISTER_PROTOCOL(CLProtocol::CSCDKVerifyRes)
	WS_REGISTER_PROTOCOL(CLProtocol::CenterCustomServiceSignRes)
	WS_REGISTER_PROTOCOL(CLProtocol::CenterForbidAccountNotify)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldCheckMasterGive)
	
	RegisterProtocol(CLProtocol::SW_GUILD_ADDSTORAGEITEM_RES, new WorldRecvAddGuildStorageItems());
	RegisterProtocol(CLProtocol::SW_GUILD_GETSTORAGEITEM_RES, new WorldRecvGetGuildStorageItem());
	RegisterProtocol(CLProtocol::SW_GUILDSTORAGE_ADD_REQ, new WorldRecvShopBuyAddGuildStorage());
	

	WS_REGISTER_PROTOCOL(CLProtocol::VerifyCreateTaskRes)
	WS_REGISTER_PROTOCOL(CLProtocol::VerifyCenterNotifyVSInfo)
	WS_REGISTER_PROTOCOL(CLProtocol::VerifyCenterNotifyVSOnline)
	WS_REGISTER_PROTOCOL(CLProtocol::VerifyCenterNotifyVSOffline)
	WS_REGISTER_PROTOCOL(CLProtocol::VerifyCenterNotifyAbility)
	WS_REGISTER_PROTOCOL(CLProtocol::VerifyReportTaskResult)
		WS_REGISTER_PROTOCOL(CLProtocol::VerifyReportTaskRaceEnd)

	WS_REGISTER_PROTOCOL(CLProtocol::MatchServerPremiumLeagueBattleRes)

	WS_REGISTER_PROTOCOL(CLProtocol::MatchServerPkRoomRes)
	WS_REGISTER_PROTOCOL(CLProtocol::CenterGlobalServerInfoRet)
	WS_REGISTER_PROTOCOL(CLProtocol::CenterQueryConvertAccountRes)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneAbnormalTransRemoveMailItemReq)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneSysRecordDataReq)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneSysRecordDataSync)
		WS_REGISTER_PROTOCOL(CLProtocol::WorldAccountTaskCheckPakcageEnoughRes)
		WS_REGISTER_PROTOCOL(CLProtocol::GateNotifyReCalculateTotalRoleValueScore)


	//跨服工会战
	WS_REGISTER_PROTOCOL(CLProtocol::WorldCrossGuildBattleEnrollInfoReq)
	WS_REGISTER_PROTOCOL(CLProtocol::CrossGuildBattleEnrollRes)
	WS_REGISTER_PROTOCOL(CLProtocol::CrossGuildBattleRecordRes)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldCrossGuildBattleTerritoryListInfoSync)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldCrossGuildBattleTerritoryInfoSync)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldCrossGuildBattleRecordInfoSync)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldCrossGuildBattleEndInfoSync)
	WS_REGISTER_PROTOCOL(CLProtocol::CrossGuildBattleSortListRes)
	WS_REGISTER_PROTOCOL(CLProtocol::CrossGuildBattleSelfSortListRes)

	//全局活动服
	WS_REGISTER_PROTOCOL(CLProtocol::GASSendAnnounceReq)
	WS_REGISTER_PROTOCOL(CLProtocol::GASSendMailReq)
	WS_REGISTER_PROTOCOL(CLProtocol::GASActivitySync)
	WS_REGISTER_PROTOCOL(CLProtocol::GASWholeBargainDiscountSync)

	//
	WS_REGISTER_PROTOCOL(CLProtocol::CrossScoreWarSortListRes)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldScoreWarRankingReward)

	WS_REGISTER_PROTOCOL(CLProtocol::Cross2V2ScoreWarSortListRes)
	WS_REGISTER_PROTOCOL(CLProtocol::World2V2ScoreWarRankingReward)

	//房间
	WS_REGISTER_PROTOCOL(CLProtocol::CrossSyncRoomPlayerInfo)
	WS_REGISTER_PROTOCOL(CLProtocol::CrossSyncRoomLogInfo)
	WS_REGISTER_PROTOCOL(CLProtocol::CrossSyncRoomNotifyInfo)
	WS_REGISTER_PROTOCOL(CLProtocol::CrossWorldRoomListRes)
	WS_REGISTER_PROTOCOL(CLProtocol::CrossUpdateRoomRes)
	WS_REGISTER_PROTOCOL(CLProtocol::CrossJoinRoomRes)
	WS_REGISTER_PROTOCOL(CLProtocol::CrossQuitRoomRes)
	WS_REGISTER_PROTOCOL(CLProtocol::CrossKickOutRoomRes)
	WS_REGISTER_PROTOCOL(CLProtocol::CrossDismissRoomRes)
	WS_REGISTER_PROTOCOL(CLProtocol::CrossInviteJoinRoomRes)
	WS_REGISTER_PROTOCOL(CLProtocol::CrossChangeRoomOwnerRes)
	WS_REGISTER_PROTOCOL(CLProtocol::CrossBeInviteRoomRes)
	WS_REGISTER_PROTOCOL(CLProtocol::CrossRoomCloseSlotRes)
	WS_REGISTER_PROTOCOL(CLProtocol::CrossRoomSwapSlotRes)
	WS_REGISTER_PROTOCOL(CLProtocol::CrossRoomResponseSwapSlotRes)
	WS_REGISTER_PROTOCOL(CLProtocol::CrossRoomBattleStartRes)
	WS_REGISTER_PROTOCOL(CLProtocol::CrossRoomBattleCancelRes)
	WS_REGISTER_PROTOCOL(CLProtocol::CrossRoomBattleReadyRes)
	WS_REGISTER_PROTOCOL(CLProtocol::CrossRoomSendInviteLinkRes)
	WS_REGISTER_PROTOCOL(CLProtocol::CrossWorldInviteRoomReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldScoreWarSyncBattleInfo)
	WS_REGISTER_PROTOCOL(CLProtocol::World2V2ScoreWarSyncBattleInfo)



    RegisterAsyncMsgHandlers();
	//师门任务
	WS_REGISTER_PROTOCOL(CLProtocol::SceneNotifyPublishMasterTaskResToWorld)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGetDiscipleMasterTasksToSceneRes)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneSyncMasterTaskShareDataToWorld)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneCheckMasterDailyTaskResToWorld)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneNotifyGiveMasterSysGiftResToWorld)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneSyncMasterSysDataToWorld)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneSyncMasterDailyTaskComp)

	//抗魔值
	WS_REGISTER_PROTOCOL(CLProtocol::WorldSyncResistMagic)    RegisterAsyncMsgHandlers();
	// 安全锁
	WS_REGISTER_PROTOCOL(CLProtocol::GateSecurityLockCheckReq)
	WS_REGISTER_PROTOCOL(CLProtocol::GateToWorldSecurityLockRemoveReq)

	RegisterProtocol(CLProtocol::SCENE_CHECK_ITEM_RET, new ItemCheckRetFunction());

	// 游戏设置
	WS_REGISTER_PROTOCOL(CLProtocol::SceneGameSetSync);
		WS_REGISTER_PROTOCOL(CLProtocol::WorldGotAccountMoneyReq)

	// 账号计数同步
	WS_REGISTER_PROTOCOL(CLProtocol::WorldAccountCounterSync);

	// 头衔
	WS_REGISTER_PROTOCOL(CLProtocol::SceneNewTitleModifyWorld)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneNewTitleAddWorld)

	WS_REGISTER_PROTOCOL(CLProtocol::GASCheckHireBindRet)

		//每日必做
		WS_REGISTER_PROTOCOL(CLProtocol::WorldDailyTodoSyncData)

	WS_REGISTER_PROTOCOL(CLProtocol::WorldOnSceneTrigeTask)

	WS_REGISTER_PROTOCOL(CLProtocol::UnionMatchServerQueryPlayerInfoReq)

	WS_REGISTER_PROTOCOL(CLProtocol::WorldQueryHireAlreadyBindRes)

	WS_REGISTER_PROTOCOL(CLProtocol::WorldMonopolyGMJoinDungeon)


	WS_REGISTER_PROTOCOL(CLProtocol::TeamCopyTeamPlayerInfo)
}
