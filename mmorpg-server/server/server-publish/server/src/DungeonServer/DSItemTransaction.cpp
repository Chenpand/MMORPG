#include "DSItemTransaction.h"

#include <CLItemProtocol.h>
#include "DSPlayer.h"


DSItemTransaction::DSItemTransaction(DSPlayer* player)
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
	m_DsRaceId = 0;
}

DSItemTransaction::~DSItemTransaction()
{
}

void DSItemTransaction::SetItem(UInt32 id, UInt32 num, UInt32 isEqualItem)
{
	CLProtocol::TransItemData transItem;
	transItem.itemNum = num;
	transItem.isEqualItem = isEqualItem;
	m_Items[id] = transItem;
}

UInt32 DSItemTransaction::GetItemNum(UInt32 id)
{
	auto iter = m_Items.find(id);
	if (iter == m_Items.end())
		return 0;

	return iter->second.itemNum;
}

void DSItemTransaction::CheckItem()
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
	req.requestServerType = ST_TEAMCOPY;
	req.param = m_DsRaceId;
	m_pPlayer->SendToScene(req);

	m_Status = STATUS_CHECK;
}

void DSItemTransaction::RemoveItem(const std::string& reason)
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

void DSItemTransaction::UnLock()
{
	if(m_Status == STATUS_INIT || m_Status == STATUS_FINAL) return;

	m_pPlayer->UnLockAsserts();

	m_Status = STATUS_FINAL;
}
