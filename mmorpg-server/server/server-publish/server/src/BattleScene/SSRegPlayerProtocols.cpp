#include "PlayerMgr.h"

#include "SSMoveHandler.h"
#include "SceneSkillHandler.h"
#include "SceneChatHandler.h"
#include "SceneItemHandler.h"
#include "SceneTaskHandler.h"
#include "SettingHandler.h"
#include "SceneTradeHandler.h"
#include "SceneRoutineHandler.h"
#include "SceneGiftBagHandler.h"
#include "SceneVipHandler.h"
#include "SceneDataCenterHandler.h"
#include "SceneRelationHandler.h"
#include "SceneHandler.h"
#include "SSEnterGameHandler.h"
#include "SceneInviteHandler.h"
#include "SceneSyncObjectHandler.h"
#include "SceneStoryHandler.h"
#include "SceneChangeZoneHandler.h"
#include "SceneDungeonHandler.h"
#include "SceneMatchHandler.h"
#include "SceneRetinueHandler.h"
#include "SceneQuickBuyHandler.h"
#include "SceneGiftBagHandler.h"
#include "SceneGuildHandler.h"
#include "SceneTeamHandler.h"
#include "SceneAuctionHandler.h"
#include "SceneOperateHandler.h"
#include "ScenePetHandler.h"
#include "SceneReplayHandler.h"
#include "SceneBattleHandler.h"
#include "SceneHeadFrameHandler.h"
#include "SceneLostDungeonHandler.h"

template<typename ProtocolT>
class SSPlayerPacketFunction : public CLParamPacketFunction<Player*>
{
public:
	void operator()(Avalon::Packet* packet,Player* player) const 
	{
		ProtocolT protocol;
		if(protocol.Decode(packet))
		{
			HandleProtocol(player,protocol); //noted by aprilliu, HandleProtocol是全局函数，很多重载形式
		}
	}
};

#define SS_REGISTER_PROTOCOL(ProtocolT) \
	m_Executor.Bind(ProtocolT::__ID,new SSPlayerPacketFunction<ProtocolT>);

void PlayerMgr::RegisterProtocols()
{
    SS_REGISTER_PROTOCOL(CLProtocol::SceneTestNetDelay)

    SS_REGISTER_PROTOCOL(CLProtocol::SceneSyncWallowFactor)

    SS_REGISTER_PROTOCOL(CLProtocol::SceneMoveRequire)
    SS_REGISTER_PROTOCOL(CLProtocol::SceneLeaveDynScene)
    SS_REGISTER_PROTOCOL(CLProtocol::SceneReturnToTown)
    SS_REGISTER_PROTOCOL(CLProtocol::ScenePlayerChangeSceneReq)
	
	SS_REGISTER_PROTOCOL(CLProtocol::SceneChangeSkillsReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneUseSkillsReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneReviveReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneAddBuff)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneInitSkillsReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneRecommendSkillsReq)

	//SS_REGISTER_PROTOCOL(CLProtocol::SceneSwitchPkMode)

	SS_REGISTER_PROTOCOL(CLProtocol::SceneObjectDataReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneClearRedPoint)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneSetCustomData)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneSelectObject)

	SS_REGISTER_PROTOCOL(CLProtocol::SceneChat)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneRequest)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneReply)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneChatSendPos)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneChatHornReq)

	SS_REGISTER_PROTOCOL(CLProtocol::SceneMoveItem)

	SS_REGISTER_PROTOCOL(CLProtocol::SceneUseItem)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneSellItem)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneEnlargePackage)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneEnlargeStorage)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneEquipStrengthen)
 	SS_REGISTER_PROTOCOL(CLProtocol::SceneEquipDecompose)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneEquieUpdateReq)

	SS_REGISTER_PROTOCOL(CLProtocol::SceneShopRefreshNumReq)
 	SS_REGISTER_PROTOCOL(CLProtocol::SceneShopQuery)
 	SS_REGISTER_PROTOCOL(CLProtocol::SceneShopBuy)	
	SS_REGISTER_PROTOCOL(CLProtocol::SceneShopRefresh)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneDungeonUseItem)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneOneKeyDecompose)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneSealEquipReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneCheckSealEquipReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneRandEquipQlvReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneAddMagicReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneMagicCardCompReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneUseMagicJarReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneQuickUseItemReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneFashionComposeReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneEquipMakeReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneFashionAttrSelReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneJarPointReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneAddPreciousBeadReq)
		
	SS_REGISTER_PROTOCOL(CLProtocol::ScenePushStorage)
	SS_REGISTER_PROTOCOL(CLProtocol::ScenePullStorage)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneOneKeyPushStorage)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneUpdateNewItem)
		
		
	SS_REGISTER_PROTOCOL(CLProtocol::SceneRemakeItem)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneComposeItemReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneTrimItem)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneExchangeItemsReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneExchangeItem)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneRandEquipShopBuy)

	SS_REGISTER_PROTOCOL(CLProtocol::SceneVisitTask)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneAcceptTask)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneSubmitTask)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneAbandonTask)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneLegendTaskListReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneSubmitLegendTask)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneSubmitDailyTask)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneSubmitAllDailyTask)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneSubmitAchievementTask)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneActiveTaskSubmit)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneSignInRp)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneSetTaskItemReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneRefreshCycleTask)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneDailyScoreRewardReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneAchievementScoreRewardReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneRestTimeReq)
	

	//SS_REGISTER_PROTOCOL(CLProtocol::SceneSetQuickBar)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneExchangeSkillBarReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneSetWeaponBarReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneSetPvpSkillConfigReq)
	//SS_REGISTER_PROTOCOL(CLProtocol::SceneExchangeItemBarReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneSaveOptionsReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneChangeNameReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneChangeSexReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneNotifyAppOnPanel)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneChangeOccu)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneCheckChangeNameReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneNotifyLoadingInfo)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneGameSetReq)

	SS_REGISTER_PROTOCOL(CLProtocol::SceneTradeUpdateItemsReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneTradeUpdateSilverReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneTradeSetStatus)

	SS_REGISTER_PROTOCOL(CLProtocol::SceneRelationPresentInfoReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneRelationPresentDraw)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneRelationPresentGive)

	SS_REGISTER_PROTOCOL(CLProtocol::SceneActiveDegreeReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneRoutineGetBoxReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneRoutineTodaySigninAwardReq)

	SS_REGISTER_PROTOCOL(CLProtocol::SceneGetGiftbag)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneGetActiveGiftbag)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneFestivalCounterReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneGSActivityStoreGold)

	SS_REGISTER_PROTOCOL(CLProtocol::SceneVipBuyItemReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneGiveMoneyManageRewardReq)

	SS_REGISTER_PROTOCOL(CLProtocol::SceneDataCenterUdpLog)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneCustomLogReport)

	SS_REGISTER_PROTOCOL(CLProtocol::SceneInviteDailyInfoReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneInviteInviteeListReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneInviteNotifyInvited)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneInviteGetGiftbag)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneInviteGetRecallAwardReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneInviteGetConsumeAwardReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneInviteBindKLPlayer)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneInviteExchangeCDKey)

	SS_REGISTER_PROTOCOL(CLProtocol::SceneUpdateBulletinRecord)

	SS_REGISTER_PROTOCOL(CLProtocol::SceneNotifyStopStory)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneNotifyNewBoot)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneNotifyBootFlag)
	
	SS_REGISTER_PROTOCOL(CLProtocol::SceneItemLockReq)

	SS_REGISTER_PROTOCOL(CLProtocol::SceneBuyGiftBag)

	SS_REGISTER_PROTOCOL(CLProtocol::SceneChangeZoneReturn)

    SS_REGISTER_PROTOCOL(CLProtocol::SceneDungeonStartReq)
    SS_REGISTER_PROTOCOL(CLProtocol::SceneDungeonRaceEndReq)
    SS_REGISTER_PROTOCOL(CLProtocol::SceneDungeonEnterNextAreaReq)
    SS_REGISTER_PROTOCOL(CLProtocol::SceneDungeonRewardReq)
    SS_REGISTER_PROTOCOL(CLProtocol::SceneDungeonOpenChestReq)
    SS_REGISTER_PROTOCOL(CLProtocol::SceneDungeonKillMonsterReq)
    SS_REGISTER_PROTOCOL(CLProtocol::SceneDungeonClearAreaMonsters)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldDungeonEnterRaceReq)
	SS_REGISTER_PROTOCOL(CLProtocol::WorldDungeonReportFrameReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneDungeonReviveReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneDungeonEndDropReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneTowerWipeoutReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneTowerWipeoutResultReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneTowerWipeoutQueryResultReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneTowerWipeoutQuickFinishReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneTowerResetReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneTowerFloorAwardReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneDungeonBuyTimesReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneDungeonResetAreaIndexReq)

    SS_REGISTER_PROTOCOL(CLProtocol::SceneMatchStartReq)
    SS_REGISTER_PROTOCOL(CLProtocol::SceneMatchCancelReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneWudaoJoinReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneWudaoRewardReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneSeasonPlayStatus)

	//Retinue
	SS_REGISTER_PROTOCOL(CLProtocol::SceneChanageRetinueReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneRetinueChangeSkillReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneRetinueUnlockReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneRetinueUpLevelReq)

	//Pet
	SS_REGISTER_PROTOCOL(CLProtocol::SceneSetPetSoltReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneFeedPetReq)
	//SS_REGISTER_PROTOCOL(CLProtocol::SceneSellPetReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneChangePetSkillReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneSetPetFollowReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneDevourPetReq)

	//QuickBuy
	SS_REGISTER_PROTOCOL(CLProtocol::SceneQuickBuyReq)

	SS_REGISTER_PROTOCOL(CLProtocol::SceneGuildExchangeReq)

	SS_REGISTER_PROTOCOL(CLProtocol::SceneTeamMatchStartReq)

	SS_REGISTER_PROTOCOL(CLProtocol::SceneAuctionRefreshReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneAuctionBuyBoothReq)
	SS_REGISTER_PROTOCOL(CLProtocol::CSOpActTakeRewardReq)

	SS_REGISTER_PROTOCOL(CLProtocol::SceneReplayListReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneReplayView)
	SS_REGISTER_PROTOCOL(CLProtocol::CSDeleteNotifyList)
	SS_REGISTER_PROTOCOL(CLProtocol::CSSetDungeonPotionReq)
	SS_REGISTER_PROTOCOL(CLProtocol::CSRenewTimeItemReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneMasterGiveEquip)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneAddonPayReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneAddonPayReq)
	SS_REGISTER_PROTOCOL(CLProtocol::CSOpenMagicBoxReq)
	
	SS_REGISTER_PROTOCOL(CLProtocol::SceneSellItemBatReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneDrawPrizeReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneOpActivityTaskInfoReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneGiftPackInfoReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneAbnormalTransactionRecordReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneStrengthenTicketSynthesisReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneStrengthenTicketFuseReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneArtifactJarDiscountInfoReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneArtifactJarExtractDiscountReq)

	//Battle
	if (SERVER_TYPE == ST_BSCENE)
	{
		SS_REGISTER_PROTOCOL(CLProtocol::SceneBattlePickUpItemReq)
		SS_REGISTER_PROTOCOL(CLProtocol::SceneBattleChangeSkillsReq)
		SS_REGISTER_PROTOCOL(CLProtocol::SceneItemSync)
		SS_REGISTER_PROTOCOL(CLProtocol::SceneBattlePickUpSpoilsReq)
		SS_REGISTER_PROTOCOL(CLProtocol::SceneBattleSelectOccuReq)
		SS_REGISTER_PROTOCOL(CLProtocol::SceneBattleBirthTransfer)
		SS_REGISTER_PROTOCOL(CLProtocol::SceneBattleThrowSomeoneItemReq)
		SS_REGISTER_PROTOCOL(CLProtocol::SceneBattleDelItemReq)
		SS_REGISTER_PROTOCOL(CLProtocol::SceneBattleEnterBattleReq)
		SS_REGISTER_PROTOCOL(CLProtocol::SceneBattleNpcTradeReq)
		SS_REGISTER_PROTOCOL(CLProtocol::SceneBattlePlaceTrapsReq)
		SS_REGISTER_PROTOCOL(CLProtocol::SceneBattleOpenBoxReq)
		SS_REGISTER_PROTOCOL(CLProtocol::BattleChoiceSkillReq)
		SS_REGISTER_PROTOCOL(CLProtocol::SceneBattleNoWarChoiceReq)
		SS_REGISTER_PROTOCOL(CLProtocol::SceneBattleOccuListReq)
		SS_REGISTER_PROTOCOL(CLProtocol::BattleChoiceEquipReq)
	}

	//EquipRec
	SS_REGISTER_PROTOCOL(CLProtocol::SceneEquipRecSubcmtReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneEquipRecRedeemReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneEquipRecUpscoreReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneEquipRecRedeemTmReq)

	//EquipHeirloom
	SS_REGISTER_PROTOCOL(CLProtocol::SceneEquipTransferReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneFashionMergeRecordReq)

	//ResistMagic
	SS_REGISTER_PROTOCOL(CLProtocol::SceneSyncResistMagicReq)

	// 道具锁
	SS_REGISTER_PROTOCOL(CLProtocol::SceneItemOpLockReq)

	//时装武器
	SS_REGISTER_PROTOCOL(CLProtocol::SceneSetFashionWeaponShowReq)

	//时装节时装合成
	SS_REGISTER_PROTOCOL(CLProtocol::SceneFashionChangeActiveMergeReq)

	//装备宝珠孔镶嵌
	SS_REGISTER_PROTOCOL(CLProtocol::SceneMountPreciousBeadReq)
	//装备宝珠孔摘除
	//SS_REGISTER_PROTOCOL(CLProtocol::SceneExtirpePreciousBeadReq)
	//宝珠升级
	//SS_REGISTER_PROTOCOL(CLProtocol::SceneUpgradePreciousbeadReq)
	//宝珠替换
	//SS_REGISTER_PROTOCOL(CLProtocol::SceneReplacePreciousBeadReq)

	SS_REGISTER_PROTOCOL(CLProtocol::SceneArtifactJarBuyCntReq)


	// 头像框
	SS_REGISTER_PROTOCOL(CLProtocol::SceneHeadFrameReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneHeadFrameUseReq)

	// 迷失战场
	SS_REGISTER_PROTOCOL(CLProtocol::SceneLostDungeonSwitchChageModeReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneLostDungeonPkReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneLostDungeonTaskListReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneLostDungeonChoiceTaskReq)
	SS_REGISTER_PROTOCOL(CLProtocol::BattleLostDungSeeIntellReq)
	SS_REGISTER_PROTOCOL(CLProtocol::SceneQueryKillTargetPosReq)	
}
