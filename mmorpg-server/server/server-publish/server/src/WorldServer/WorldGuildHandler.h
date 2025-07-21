#ifndef __WORLD_GUILD_HANDLER_H__
#define __WORLD_GUILD_HANDLER_H__

#include <CLGuildProtocol.h>
#include <CLPacketExecutor.h>
#include <CLTeamCopyProtocol.h>
class WSPlayer;

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildCreateReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildLeaveReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildJoinReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildListReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildRequesterReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildProcessRequester& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildChangePostReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildKickReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildMemberListReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildModifyDeclaration& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildModifyName& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildModifyAnnouncement& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildSendMail& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildUpgradeBuilding& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildDonateReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildDonateLogReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildUpgradeSkill& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildDismissReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildCancelDismissReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildLeaderInfoReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildOrzReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildTableJoinReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildPayRedPacketReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildBattleReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildBattleInspireReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildBattleReceiveReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildBattleRecordReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildBattleTerritoryReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildBattleSelfSortListReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildChallengeReq req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildChallengeInfoReq req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildBattleInspireInfoReq req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildStorageSettingReq req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildBattleLotteryReq req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildStorageListReq req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildAddStorageReq req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildDelStorageReq req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldWatchGuildStorageItemReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildDungeonInfoReq req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildDungeonDamageRankReq req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildDungeonCopyReq req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildDungeonLotteryReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildDungeonStatueReq& req);

void HandleProtocol(CLProtocol::WorldGuildSyncSkill& sync);

void HandleProtocol(CLProtocol::WorldGuildSyncContribution& sync);

void HandleProtocol(CLProtocol::WorldGuildGiveItemRes& protocol);

class WorldRecvAddGuildStorageItems : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const;
};

class WorldRecvGetGuildStorageItem : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const;
};

class WorldRecvShopBuyAddGuildStorage : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const;
};

void HandleProtocol(CLProtocol::WorldCrossGuildBattleEnrollInfoReq& req);

void HandleProtocol(CLProtocol::CrossGuildBattleEnrollRes& req);

void HandleProtocol(CLProtocol::CrossGuildBattleRecordRes& res);

void HandleProtocol(CLProtocol::WorldCrossGuildBattleTerritoryListInfoSync& sync);

void HandleProtocol(CLProtocol::WorldCrossGuildBattleTerritoryInfoSync& sync);

void HandleProtocol(CLProtocol::WorldCrossGuildBattleRecordInfoSync& sync);

void HandleProtocol(CLProtocol::WorldCrossGuildBattleEndInfoSync& sync);

void HandleProtocol(CLProtocol::CrossGuildBattleSortListRes& res);

void HandleProtocol(CLProtocol::CrossGuildBattleSelfSortListRes& res);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildSetJoinLevelReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildEmblemUpReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildSetDungeonTypeReq &req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildGetTerrDayRewardReq &req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildAuctionItemReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildAuctionBidReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildAuctionFixReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildWatchCanMergerReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildMergerRequestOperatorReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildReceiveMergerRequestReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildWatchHavedMergerRequestReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildAcceptOrRefuseOrCancleMergerRequestReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildEventListReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildHonorInfoReq& req);

void HandleProtocol(CLProtocol::SyncGuildEventToWorld& sync);
void HandleProtocol(CLProtocol::TeamCopyTeamPlayerInfo& info);
#endif