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

	// ������Ʒ
	void HandleDepositItem();
	// ��ȡ��Ʒ
	void HandleGetDepositItem(UInt64 guid);
	// ����
	void LoadItemDeposit(CLRecordCallback* callback);
	// �����ݴ���Ʒ
	void AddItemDeposit(const ItemReward& itemReward, UInt32 dungeonId = 0);

private:

	// ����ʱ��֪ͨ
	void _ExpireTimeNotify(UInt32 oddTime);
	// udp��־
	void _UdpItemDeposit(UInt32 opType, const ItemReward& itemReward, UInt32 dungeonId);
	// �����ݴ�����Ʒ
	void _DepositItemRes();
	// �������ݿ�
	void _InsertDB(const CLProtocol::depositItem& dpitem);
	// ɾ�����ݿ�
	void _DelDB(UInt64 guid);
	// ɾ�����е�����Ʒ
	void _DelExpireItem();

private:

	Player* m_Owner;
	// �Ƿ���ʾ����
	bool m_IsTipsExpire;
	// �ݴ�����(key->guid)
	std::map<UInt64, CLProtocol::depositItem> m_DepositItemMap;
};

#endif
