#include "WSPlayerMgr.h"

#include "WSChatHandler.h"
#include "WorldMailHandler.h"
#include "WSTeamHandler.h"
#include "WSRelationHandler.h"
#include "WorldSyncObjectHandler.h"
#include "WorldSortListHandler.h"
#include "WorldMallHandler.h"
#include "WSMoveHandler.h"
#include "WorldAuctionHandler.h"
#include "WSBillingHandler.h"
#include "WSWorldChallengeHandler.h"
#include "WorldMatchHandler.h"
#include "WorldDungeonHandler.h"
#include "WSActivityHandler.h"
#include "WorldGuildHandler.h"
#include "WorldRedPacketHandler.h"
#include "WorldOperateHandler.h"
#include "CLItemProtocol.h"
#include "WSItemHandler.h"
#include "WSCenterHandler.h"
#include "WorldPremiumLeagueHandler.h"
#include "WorldRoomHandler.h"
#include "WorldDigHandler.h"
#include "WorldSecurityLockHandler.h"
#include "WorldRoleHandler.h"
#include "WorldShopHandler.h"
#include "CLBlackMarketProtocol.h"
#include "WSBlackMarketHandler.h"
#include "WorldAdventureTeamHandler.h"
#include "WorldTaskHandler.h"
#include "WorldNewTitleHandler.h"
#include "WorldDailyTodoHandler.h"
#include "WorldAdventurePassHandler.h"
#include "WorldHireHandler.h"
#include "WorldMonopolyHandler.h"

template<typename ProtocolT>
class WSPlayerPacketFunction : public CLParamPacketFunction<WSPlayer*>
{
public:
	void operator()(Avalon::Packet* packet,WSPlayer* player) const
	{
		ProtocolT protocol;
		if(protocol.Decode(packet))
		{
			HandleProtocol(player,protocol);
		}
	}
};

#define WS_REGISTER_PROTOCOL(ProtocolT) \
	m_Executor.Bind(ProtocolT::__ID,new WSPlayerPacketFunction<ProtocolT>);

void WSPlayerMgr::RegisterProtocols()
{
	WS_REGISTER_PROTOCOL(CLProtocol::WorldChatLinkDataReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldOfflineNotifyReq)

	WS_REGISTER_PROTOCOL(CLProtocol::WorldMailListReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldReadMailReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGetMailItems)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldSendMail)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldDeleteMail)

	WS_REGISTER_PROTOCOL(CLProtocol::WorldCreateTeam)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldLeaveTeamReq)
    WS_REGISTER_PROTOCOL(CLProtocol::WorldSetTeamOption)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldTransferTeammaster)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldDismissTeam)
    WS_REGISTER_PROTOCOL(CLProtocol::WorldQueryTeamList)
    WS_REGISTER_PROTOCOL(CLProtocol::WorldTeamMasterOperSync)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldTeamRequesterListReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldTeamProcessRequesterReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldTeamReportVoteChoice)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldTeamMatchCancelReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldChangeAssistModeReq)

	WS_REGISTER_PROTOCOL(CLProtocol::WorldQueryPlayerReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldQueryPlayerDetailsReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldAddToBlackList)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldRemoveRelation)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldChaseKill)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldSetOnlineNotify)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldRelationFindPlayersReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldRelationQuickMakeFriends)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldRelationSetClostFriend)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldRelationPresentGiveReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldUpdateRelation)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldSetDiscipleQuestionnaireReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldSetMasterQuestionnaireReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldUpdateMasterSectRelationReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldDiscipleFinishSchoolReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldSetPlayerRemarkReq)
	
	//师门任务
	WS_REGISTER_PROTOCOL(CLProtocol::WorldPublishMasterDialyTaskReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGetDiscipleMasterTasksReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldRelationAnnounceReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldAutomaticFinishSchoolReq)
	//WS_REGISTER_PROTOCOL(CLProtocol::WorldReportMasterDailyTaskReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldReceiveMasterDailyTaskRewardReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldReceiveMasterAcademicTaskRewardReq)

	WS_REGISTER_PROTOCOL(CLProtocol::WorldWatchPlayerReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldPlayerPowerReq)

	WS_REGISTER_PROTOCOL(CLProtocol::WorldSortListReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldSortListNearReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldSortListWatchDataReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldSortListSelfInfoReq)

	WS_REGISTER_PROTOCOL(CLProtocol::WorldMallBuy)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldMallQueryItemReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldMallQueryItemDetailReq)
	WS_REGISTER_PROTOCOL(CLProtocol::CWMallBatchBuyReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldMallGiftPackActivateReq)

	WS_REGISTER_PROTOCOL(CLProtocol::WorldAuctionListReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldAuctionItemNumReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldAuctionSelfListReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldAuctionRequest)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldAuctionBuy)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldAuctionCancel)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldAuctionFindByID)
    WS_REGISTER_PROTOCOL(CLProtocol::WorldAuctionQueryItemReq)
    WS_REGISTER_PROTOCOL(CLProtocol::WorldAuctionRecommendPriceReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldAuctionQueryItemPricesReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldAuctionQueryMagicOnsalesReq)

	WS_REGISTER_PROTOCOL(CLProtocol::WorldNotifyCounterWindow)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldActivityMonsterReq)

	/*
	WS_REGISTER_PROTOCOL(CLProtocol::WorldWorldChallengeSigninReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldWorldChallengeSignout)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldWorldChallengeInfoReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldWorldChallengeSortListReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldWorldChallengeGetAwardReq)
	*/

	WS_REGISTER_PROTOCOL(CLProtocol::WorldMatchQueryFriendStatusReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldMatchCancelRes)

	WS_REGISTER_PROTOCOL(CLProtocol::SceneDungeonRaceEndReq)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneDungeonEnterNextAreaReq)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneDungeonRewardReq)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneDungeonOpenChestReq)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneDungeonKillMonsterReq)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneDungeonClearAreaMonsters)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneDungeonReviveReq)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneDungeonEndDropReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldDungeonEnterRaceReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldDungeonReportFrameReq)

	WS_REGISTER_PROTOCOL(CLProtocol::WorldDungeonGetAreaIndexReq)

	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildCreateReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildLeaveReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildJoinReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildListReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildRequesterReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildProcessRequester)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildChangePostReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildKickReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildMemberListReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildModifyDeclaration)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildModifyName)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildModifyAnnouncement)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildSendMail)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildUpgradeBuilding)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildDonateReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildDonateLogReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildUpgradeSkill)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildDismissReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildCancelDismissReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildLeaderInfoReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildOrzReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildTableJoinReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildPayRedPacketReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildEventListReq)

	WS_REGISTER_PROTOCOL(CLProtocol::WorldSendRedPacketReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldOpenRedPacketReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldSendCustomRedPacketReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGetGuildRedPacketInfoReq)

	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildBattleInspireReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildBattleReceiveReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildBattleRecordReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildBattleReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildBattleTerritoryReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildBattleSelfSortListReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildChallengeReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildChallengeInfoReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildBattleInspireInfoReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildStorageSettingReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildBattleLotteryReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildStorageListReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildAddStorageReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildDelStorageReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildSetJoinLevelReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildEmblemUpReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildSetDungeonTypeReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildGetTerrDayRewardReq)

	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildWatchCanMergerReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildMergerRequestOperatorReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildReceiveMergerRequestReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildWatchHavedMergerRequestReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildAcceptOrRefuseOrCancleMergerRequestReq)


	WS_REGISTER_PROTOCOL(CLProtocol::WorldBillingGoodsReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldBillingChargePacketReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldBillingChargeReq)

	WS_REGISTER_PROTOCOL(CLProtocol::WorldUpdatePlayerOnlineReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldWatchGuildStorageItemReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldAdventureTeamRenameReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldExtensibleRoleFieldUnlockReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldBlessCrystalInfoReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldInheritBlessInfoReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldInheritExpReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldAccountShopItemQueryReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldAccountShopItemBuyReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldAdventureTeamExtraInfoReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldQueryExpeditionMapReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldQueryOptionalExpeditionRolesReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldDispatchExpeditionTeamReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldCancelExpeditionReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGetExpeditionRewardsReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldQueryAllExpeditionMapsReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldQueryOwnOccupationsReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldRemoveUnlockedNewOccupationsReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldSubmitAccountTask)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldSetTaskItemReq)

	//运营活动
	WS_REGISTER_PROTOCOL(CLProtocol::CWCDKVerifyReq)

	WS_REGISTER_PROTOCOL(CLProtocol::WorldOpenJarRecordReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldCustomServiceSignReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldEquipRecoOpenJarsRecordReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGetSysRecordReq)

	WS_REGISTER_PROTOCOL(CLProtocol::WorldSetMasterNoteReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldSetMasterIsRecvDiscipleReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldQueryMasterSettingReq)

	WS_REGISTER_PROTOCOL(CLProtocol::WorldAddonPayReply)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldRelationReportCheat)
	
	WS_REGISTER_PROTOCOL(CLProtocol::WorldPremiumLeagueEnrollReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldPremiumLeagueRewardPoolReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldPremiumLeagueBattleRecordReq)

	WS_REGISTER_PROTOCOL(CLProtocol::SceneActiveTaskSubmit)
	WS_REGISTER_PROTOCOL(CLProtocol::SceneSignInRp)
	//房间
	WS_REGISTER_PROTOCOL(CLProtocol::WorldRoomListReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldUpdateRoomReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldJoinRoomReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldQuitRoomReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldKickOutRoomReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldInviteJoinRoomReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldChangeRoomOwnerReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldBeInviteRoomReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldRoomCloseSlotReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldRoomSwapSlotReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldRoomResponseSwapSlotReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldRoomBattleStartReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldRoomBattleCancelReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldRoomBattleReadyReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldRoomSendInviteLinkReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGetMallItemByItemIdReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldMallQuerySingleItemReq)

	WS_REGISTER_PROTOCOL(CLProtocol::WorldGetRechargePushItemsReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldBuyRechargePushItemReq)

	//Dig
	WS_REGISTER_PROTOCOL(CLProtocol::WorldDigMapOpenReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldDigMapCloseReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldDigWatchReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldDigOpenReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldDigMapInfoReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldDigRecordsReq)

	// 安全锁
	WS_REGISTER_PROTOCOL(CLProtocol::WorldSecurityLockDataReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldSecurityLockOpReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldChangeSecurityPasswdReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldBindDeviceReq)

	// 公会地下城
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildDungeonInfoReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildDungeonDamageRankReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildDungeonCopyReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildDungeonLotteryReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildDungeonStatueReq)

	// 公会拍卖
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildAuctionItemReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildAuctionBidReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildAuctionFixReq)

	//拍卖行获取交易记录
	WS_REGISTER_PROTOCOL(CLProtocol::WorldAuctionGetTreasTransactionReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldAuctionRusyBuy)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldAuctionAttentReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldAuctionQueryItemPriceListReq)

	//黑市商人
	WS_REGISTER_PROTOCOL(CLProtocol::WorldBlackMarketAuctionListReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldBlackMarketAuctionReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldBlackMarketAuctionCancelReq)

	//头衔
	WS_REGISTER_PROTOCOL(CLProtocol::WorldNewTitleTakeUpReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldNewTitleTakeOffReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldNewTitleTakeUpGuildPostReq)

	//每日必做
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGetPlayerDailyTodosReq)


	//冒险通行证
	WS_REGISTER_PROTOCOL(CLProtocol::WorldAventurePassStatusReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldAventurePassBuyReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldAventurePassBuyLvReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldAventurePassExpPackReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldAventurePassRewardReq)
	

	//大富翁
	WS_REGISTER_PROTOCOL(CLProtocol::WorldMonopolyCoinReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldMonopolyStatusReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldMonopolyRollReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldMonopolyCardExchangeReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldMonopolyCardListReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldMonopolySellCardReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldMonopolyCardExchangedListReq)

	
	//roll请求
	WS_REGISTER_PROTOCOL(CLProtocol::WorldDungeonRollItemReq)

	//查询道具拍卖行交易记录请求
	WS_REGISTER_PROTOCOL(CLProtocol::WorldAuctionQueryItemTransListReq)

	//道具赠送
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGetItemFriendPresentInfosReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldItemFriendPresentReq)

	//招募
	WS_REGISTER_PROTOCOL(CLProtocol::WorldQueryHireInfoReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldUseHireCodeReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldQueryTaskStatusReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldQueryHireTaskAccidListReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldQueryHireListReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldSubmitHireTaskReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldQueryHirePushReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldQueryHireAlreadyBindReq)

	WS_REGISTER_PROTOCOL(CLProtocol::WorldActivityDetailReq)
	WS_REGISTER_PROTOCOL(CLProtocol::WorldGuildHonorInfoReq)
}


