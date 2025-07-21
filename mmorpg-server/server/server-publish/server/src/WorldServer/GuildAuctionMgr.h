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

	// ����������Ʒ��Ϣ
	void HandleAuctionItemReq(WSPlayer* player, UInt32 type);
	// ���۾���
	void HandleAuctionBidReq(WSPlayer* player, UInt64 guid, UInt32 bidPrice);
	// һ�ڼ�
	void HandleAuctionFixReq(WSPlayer* player, UInt64 guid);


	// ������������
	bool LoadAuctionItem(CLRecordCallback* callback);
	// �����������
	void ObtainAuctionReward(UInt64 guildId, UInt32 rewardGroup, UInt32 initPrice, UInt32 fixPrice, UInt32 addPrice, const ItemRewardVec& reward);
	// ���ۿۿ�
	void AuctionRemoveItem(WSPlayer* player, GuildAuctionTransaction* auctionTran);
	// ������ߣ�֪ͨ�Ƿ�����Ʒ
	void OnPlayerOnline(WSPlayer* player);
	// �������
	void OnLevelUp(WSPlayer* player);
	// ����Ƿ��й������Ʒ��û����
	bool CheckGuildIsAuction(UInt64 guildId);

private:
	// ����Ʒ�������ݿ�
	void _InsertAuctionItem(const GuildAuctionItem& auctionItem);
	// ����Ʒ����
	void _UpdateAuctionItem(const GuildAuctionItem& auctionItem);
	// ��ȡ����map
	GuildAuctionItemMap* _GetGuildAuctionItemMap(UInt32 type);
	// ��������Ʒ
	void _AddAuctionItem(const GuildAuctionItem& auctionItem);
	// �ҵ�����Ʒ
	GuildAuctionItem* _GetGuildAuctionItem(UInt64 guid);
	// ����������˿�
	void _PreBidPlayerRefund(UInt64 playerId, UInt64 guildId, UInt32 num, UInt32 auctionType, const std::string& rewardName);
	// ���������ɽ�
	void _AuctionDeal(const GuildAuctionItem& auctionItem);
	// ��ӹ�������Ʒ����
	void _AddGuildAuctionIndex(UInt64 guildId, UInt64 guid);
	// ��ȡ������������Ʒ
	const std::set<UInt64>* _GetGuildAuctionIndex(UInt64 guildId);
	// ��鹫���Ƿ��п��ĵ���Ʒ
	bool _CheckGuildAuctionIsHasItem(UInt64 guildId);
	// ֪ͨ�������
	void _NotifyAuctionEntrance(UInt32 type, bool _isOpen, UInt64 guildId = 0);
	// ���������������
	void _ClearAuctionData();
	// ��ý�������
	std::string _GetRewardStr(const ItemRewardVec& rewardVec);
	// �쳣���ؿͻ���
	void _NotifyClientResult(WSPlayer* player, UInt32 sourceType, UInt32 retCode);
	// ��������״̬��ô�����
	UInt32 _GetErrorCodeByAucState(UInt32 aucState);
	// �Ƿ��������
	bool _IsCanAuction(UInt32 state);
	// ����Ƿ���Էֳ�
	void _CheckAuctionBonus(UInt64 guildId);
	// ��ȡ��Ҫ�ֳɵĽ��������·ֳ���Ϣ
	UInt32 _GetBonusUpdateBonusState(UInt64 guildId);
	// udp����������
	void _UdpGuildAuction(UInt32 opType, const GuildAuctionItem& auctionItem);
	// udp�ɽ�
	void _UdpAuctionDeal(const GuildAuctionItem& auctionItem, UInt32 param = 0);
	// udp����
	void _UdpAuctionBid(WSPlayer* player, const GuildAuctionItem& auctionItem);
	
public:
	// GMֹͣ����
	void StopAuction(UInt64 guildId);

private:
	// ������Ʒ������(key->����id, val->����Ʒguid)
	std::map<UInt64, std::set<UInt64>> m_GuildAuctionIndex;
	// ���й�������Ʒ(key->guid, val->����Ʒ)
	GuildAuctionItemMap m_AuctionWorldMap;
	// ��������(key->guid, val->����Ʒ)
	GuildAuctionItemMap m_AuctionGuildMap;

	// �Ƿ����
	bool m_IsDayChange;
};

#endif
