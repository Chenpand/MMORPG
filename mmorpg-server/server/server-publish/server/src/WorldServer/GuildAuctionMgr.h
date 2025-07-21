#ifndef _GUILD_AUCTION_MGR_H_
#define _GUILD_AUCTION_MGR_H_

#include <CLDefine.h>
#include <AvalonSingleton.h>
#include <CLGuildDefine.h>

class CLRecordCallback;
class WSPlayer;
class GuildAuctionTransaction;

typedef std::map<UInt64, GuildAuctionItem> GuildAuctionItemMap;

class GuildAuctionMgr : public Avalon::Singleton<GuildAuctionMgr>
{
public:
	GuildAuctionMgr();
	~GuildAuctionMgr();

	void Init();
	void OnTick(const Avalon::Time &now);

	// 请求拍卖物品信息
	void HandleAuctionItemReq(WSPlayer* player, UInt32 type);
	// 出价竞拍
	void HandleAuctionBidReq(WSPlayer* player, UInt64 guid, UInt32 bidPrice);
	// 一口价
	void HandleAuctionFixReq(WSPlayer* player, UInt64 guid);


	// 加载拍卖道具
	bool LoadAuctionItem(CLRecordCallback* callback);
	// 获得拍卖奖励
	void ObtainAuctionReward(UInt64 guildId, UInt32 rewardGroup, UInt32 initPrice, UInt32 fixPrice, UInt32 addPrice, const ItemRewardVec& reward);
	// 出价扣款
	void AuctionRemoveItem(WSPlayer* player, GuildAuctionTransaction* auctionTran);
	// 玩家上线，通知是否有拍品
	void OnPlayerOnline(WSPlayer* player);
	// 玩家升级
	void OnLevelUp(WSPlayer* player);
	// 检查是否有公会的物品还没拍卖
	bool CheckGuildIsAuction(UInt64 guildId);

private:
	// 拍卖品插入数据库
	void _InsertAuctionItem(const GuildAuctionItem& auctionItem);
	// 拍卖品更新
	void _UpdateAuctionItem(const GuildAuctionItem& auctionItem);
	// 获取拍卖map
	GuildAuctionItemMap* _GetGuildAuctionItemMap(UInt32 type);
	// 增加拍卖品
	void _AddAuctionItem(const GuildAuctionItem& auctionItem);
	// 找到拍卖品
	GuildAuctionItem* _GetGuildAuctionItem(UInt64 guid);
	// 给出价玩家退款
	void _PreBidPlayerRefund(UInt64 playerId, UInt64 guildId, UInt32 num, UInt32 auctionType, const std::string& rewardName);
	// 处理拍卖成交
	void _AuctionDeal(const GuildAuctionItem& auctionItem);
	// 添加公会拍卖品索引
	void _AddGuildAuctionIndex(UInt64 guildId, UInt64 guid);
	// 获取公会所有拍卖品
	const std::set<UInt64>* _GetGuildAuctionIndex(UInt64 guildId);
	// 检查公会是否有可拍的物品
	bool _CheckGuildAuctionIsHasItem(UInt64 guildId);
	// 通知拍卖入口
	void _NotifyAuctionEntrance(UInt32 type, bool _isOpen, UInt64 guildId = 0);
	// 清除所有拍卖数据
	void _ClearAuctionData();
	// 获得奖励名称
	std::string _GetRewardStr(const ItemRewardVec& rewardVec);
	// 异常返回客户端
	void _NotifyClientResult(WSPlayer* player, UInt32 sourceType, UInt32 retCode);
	// 根据拍卖状态获得错误码
	UInt32 _GetErrorCodeByAucState(UInt32 aucState);
	// 是否可以拍卖
	bool _IsCanAuction(UInt32 state);
	// 检查是否可以分成
	void _CheckAuctionBonus(UInt64 guildId);
	// 获取需要分成的奖励，更新分成信息
	UInt32 _GetBonusUpdateBonusState(UInt64 guildId);
	// udp创建和流拍
	void _UdpGuildAuction(UInt32 opType, const GuildAuctionItem& auctionItem);
	// udp成交
	void _UdpAuctionDeal(const GuildAuctionItem& auctionItem, UInt32 param = 0);
	// udp竞价
	void _UdpAuctionBid(WSPlayer* player, const GuildAuctionItem& auctionItem);
	
public:
	// GM停止拍卖
	void StopAuction(UInt64 guildId);

private:
	// 公会拍品的索引(key->公会id, val->拍卖品guid)
	std::map<UInt64, std::set<UInt64>> m_GuildAuctionIndex;
	// 所有公会拍卖品(key->guid, val->拍卖品)
	GuildAuctionItemMap m_AuctionWorldMap;
	// 世界拍卖(key->guid, val->拍卖品)
	GuildAuctionItemMap m_AuctionGuildMap;

	// 是否跨天
	bool m_IsDayChange;
};

#endif
