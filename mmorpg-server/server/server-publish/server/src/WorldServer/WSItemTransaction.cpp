#include "WSItemTransaction.h"

#include <CLItemProtocol.h>
#include "WSPlayer.h"


ItemTransaction::ItemTransaction(WSPlayer* player)
	:m_pPlayer(player)
{
	m_Status = STATUS_INIT;
	m_ErrorCode = 0;
	static UInt16 SERIAL_SEED = 0;
	m_Serial = ++SERIAL_SEED;
	m_Silver = 0;
	m_BindSilver = 0;
	m_Gold = 0;
	m_BindGold = 0;
	m_ReviveCoin = 0;
	m_GuildContri = 0;
	m_MaxCount = 0;
	m_NeedCount = 0;
	m_CounterTimes = 0;
	m_isWeeklySetTask = 0;
}

ItemTransaction::~ItemTransaction()
{
}


void ItemTransaction::SetItem(UInt32 id, UInt32 num, UInt32 isEqualItem)
{ 
	CLProtocol::TransItemData transItem;
	transItem.itemNum = num;
	transItem.isEqualItem = isEqualItem;
	m_Items[id] = transItem;
}

UInt32 ItemTransaction::GetItemNum(UInt32 id)
{
	auto iter = m_Items.find(id);
	if (iter == m_Items.end())
		return 0;

	return iter->second.itemNum;
}

void ItemTransaction::CheckItem()
{
	if(m_Status != STATUS_INIT) return;

	CLProtocol::SceneCheckItemNumReq req;
	req.playerId = m_pPlayer->GetID();
	req.serial = m_Serial;
	req.bindSilver = m_BindSilver;
	req.silver = m_Silver;
	req.gold = m_Gold;
	req.bindGold = m_BindGold;
	req.reviveCoin = m_ReviveCoin;
	req.guildContri = m_GuildContri;
	req.items = m_Items;
	req.nobindItems = m_NoBindItems;
	req.counterName = m_CounterName;
	req.maxCount = m_MaxCount;
	req.needCount = m_NeedCount;
	req.detailItems = m_detailItems;
	req.isWeeklySetTask = m_isWeeklySetTask;
	m_pPlayer->SendToScene(req);

	m_Status = STATUS_CHECK;
}

void ItemTransaction::RemoveItem(const std::string& reason)
{
	if(m_Status != STATUS_CHECKED) return;

	CLProtocol::SceneRemoveItemReq req;
	req.playerId = m_pPlayer->GetID();
	req.reason = reason;
	req.bindSilver = m_BindSilver;
	req.silver = m_Silver;
	req.gold = m_Gold;
	req.bindGold = m_BindGold;
	req.reviveCoin = m_ReviveCoin;
	req.guildContri = m_GuildContri;
	req.items = m_Items;
	req.nobindItems = m_NoBindItems;
	req.counterName = m_CounterName;
	req.needCount = m_NeedCount;
	req.detailItems = m_detailItems;
	m_pPlayer->SendToScene(req);

	m_Status = STATUS_FINAL;
}

void ItemTransaction::UnLock()
{
	if(m_Status == STATUS_INIT || m_Status == STATUS_FINAL) return;

	m_pPlayer->UnLockAsserts();

	m_Status = STATUS_FINAL;
}
