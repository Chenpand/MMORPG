#ifndef _ITEM_DEPOSIT_MGR_H_
#define _ITEM_DEPOSIT_MGR_H_

#include <CLDefine.h>

struct ItemReward;
class Player;
class CLRecordCallback;
namespace CLProtocol{
	struct depositItem;
}

class ItemDepositMgr
{
public:
	ItemDepositMgr();
	~ItemDepositMgr();

	void SetOwner(Player* player) { m_Owner = player; }
	Player* GetOwner() { return m_Owner; }

	// tick
	void OnTick(const Avalon::Time& now);

	// 请求物品
	void HandleDepositItem();
	// 领取物品
	void HandleGetDepositItem(UInt64 guid);
	// 加载
	void LoadItemDeposit(CLRecordCallback* callback);
	// 增加暂存物品
	void AddItemDeposit(const ItemReward& itemReward, UInt32 dungeonId = 0);

private:

	// 到期时间通知
	void _ExpireTimeNotify(UInt32 oddTime);
	// udp日志
	void _UdpItemDeposit(UInt32 opType, const ItemReward& itemReward, UInt32 dungeonId);
	// 返回暂存箱物品
	void _DepositItemRes();
	// 插入数据库
	void _InsertDB(const CLProtocol::depositItem& dpitem);
	// 删除数据库
	void _DelDB(UInt64 guid);
	// 删除所有到期物品
	void _DelExpireItem();

private:

	Player* m_Owner;
	// 是否提示过期
	bool m_IsTipsExpire;
	// 暂存数据(key->guid)
	std::map<UInt64, CLProtocol::depositItem> m_DepositItemMap;
};

#endif
