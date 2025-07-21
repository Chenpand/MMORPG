#include "USItemTransaction.h"

#include <CLItemProtocol.h>
#include "USPlayer.h"

USItemTransaction::USItemTransaction(USPlayer* player)
	:m_Player(player)
{
	static UInt32 SERIAL_SEED = 0;
	m_Serial = ++SERIAL_SEED;
	m_TimeOut = 0;
	m_ServiceType = 0;
}

USItemTransaction::~USItemTransaction()
{

}

void USItemTransaction::SetItem(UInt32 id, UInt32 num, UInt32 isEqualItem)
{
	CLProtocol::TransItemData transItem;
	transItem.itemNum = num;
	transItem.isEqualItem = isEqualItem;
	m_Items[id] = transItem;
}

UInt32 USItemTransaction::GetItemNum(UInt32 id)
{
	auto iter = m_Items.find(id);
	if (iter == m_Items.end())
		return 0;

	return iter->second.itemNum;
}

void USItemTransaction::SetNoBindItem(UInt32 id, UInt32 num)
{
	m_NoBindItems[id] = num; 
}

UInt32 USItemTransaction::GetNoBindItemNum(UInt32 id)
{
	std::map<UInt32, UInt32>::iterator iter = m_NoBindItems.find(id);
	return iter != m_NoBindItems.end() ? iter->second : 0;
}

void USItemTransaction::CheckItem()
{
	if(m_Status != STATUS_INIT) 
		return;

	CLProtocol::SceneCheckItemNumReq req;
	req.playerId = m_Player->GetID();
	req.serial = m_Serial;
	req.bindSilver = m_BindGold;
	req.silver = m_Gold;
	req.gold = m_Point;
	req.bindGold = m_BindPoint;
	req.reviveCoin = 0;
	req.guildContri = 0;
	req.items = m_Items;
	req.nobindItems = m_NoBindItems;
	req.maxCount = 0;
	req.needCount = 0;
	req.requestServerType = ST_UNION;
	req.param = m_ServiceType;
	m_Player->SendToScene(req);

	m_TimeOut = UInt32(CURRENT_TIME.Sec());

	m_Status = STATUS_CHECK;
}

void USItemTransaction::RemoveItem(const std::string& reason)
{
	if(m_Status != STATUS_CHECKED)
		return;

	CLProtocol::SceneRemoveItemReq req;
	req.playerId = m_Player->GetID();
	req.reason = reason;
	req.bindSilver = m_BindGold;
	req.silver = m_Gold;
	req.gold = m_Point;
	req.bindGold = m_BindPoint;
	req.reviveCoin = 0;
	req.guildContri = 0;
	req.items = m_Items;
	req.nobindItems = m_NoBindItems;
	req.needCount = 0;
	m_Player->SendToScene(req);

	m_TimeOut = 0;
}

void USItemTransaction::UnLock()
{
	if(m_Status == STATUS_INIT || m_Status == STATUS_FINAL) 
		return;

	m_TimeOut = 0;
	m_Status = STATUS_FINAL;

	CLProtocol::SceneUnlockItemReq req;
	req.playerId = m_Player->GetID();
	m_Player->SendToScene(req);
}

bool USItemTransaction::CheckTimeOut(const Avalon::Time& now)
{
	// ÊÂÎñ5Ãë³¬Ê±
	if (UInt32(now.Sec()) > m_TimeOut + 5)
	{
		OnTimeOut(m_Player);
		return true;
	}

	return false;
}