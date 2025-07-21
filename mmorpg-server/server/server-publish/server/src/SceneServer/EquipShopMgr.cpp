#include "EquipShopMgr.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "PlayerScript.h"

#include <CLItemDataEntry.h>

EquipShopMgr::EquipShopMgr()
	: m_uLastRefreshTime(0)
{
}

EquipShopMgr::~EquipShopMgr()
{
}

void EquipShopMgr::Buy(UInt8 uType, UInt8 uIndex)
{
	std::list<CLProtocol::RandEquipEntry>::iterator	iter;
	if(0 == uType)
	{
		if(uIndex >= m_listNormal.size())
			return;

		iter = m_listNormal.begin();
		for(UInt8 i = 0; i < uIndex; i++)
			iter++;
	}
	else
	{
		if(uIndex >= m_listVip.size())
			return;

		iter = m_listNormal.begin();
		for(UInt8 i = 0; i < uIndex; i++)
			iter++;
	}

	if(m_pOwner->GetBindPoint() < iter->price)
		return;

	if(!m_pOwner->CheckAddItem(iter->id, 1))
		return;

	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_INVALID);
	m_pOwner->AddItem(reason.c_str(), iter->id, 1);
	m_pOwner->RemoveBindPoint(reason.c_str(), iter->price);

	if(0 == uType)
		m_listNormal.erase(iter);
	else
		m_listVip.erase(iter);

	CLProtocol::SceneRandEquipShopDel	notify;
	notify.type = uType;
	notify.index = uIndex;
	m_pOwner->SendProtocol(notify);

	SetDirty();
}

void EquipShopMgr::OnOnline()
{
	if(ScriptMgr::Instance()->GetPlayerScript()->NeedRefreshEquipShop(CURRENT_TIME, m_uLastRefreshTime))
		RefreshEquipList();

	SyncEquipList();
}

void EquipShopMgr::OnTick(const Avalon::Time &now, int tickType)
{
	if(LOGICTICK_FIVESEC == tickType)
	{
		if(m_pOwner->GetLevel() < 30)
			return;

		if(ScriptMgr::Instance()->GetPlayerScript()->NeedRefreshEquipShop(now, m_uLastRefreshTime))
		{
			RefreshEquipList();
			SyncEquipList();
		}
	}
}

void EquipShopMgr::OnLevelUp()
{
	if(m_pOwner->GetLevel() < 30)
		return;

	if(0 != m_uLastRefreshTime)
		return;
	
	RefreshEquipList();
	SyncEquipList();
}

void EquipShopMgr::RefreshEquipList()
{
// 	if(m_pOwner->GetLevel() < 30)
// 		return;
// 
// 	m_listNormal.clear();
// 	m_listVip.clear();
// 
// 	UInt16	uPlayerLevel = m_pOwner->GetLevel();
// 	UInt16	uEquipLevel = (uPlayerLevel + 4) / 10 * 10;
// 
// 	for(UInt32 i = 0; i < NORMAL_SHOP_ITEM_NUM; i++)
// 	{
// 		UInt32	uEquipId = ItemDataEntryMgr::Instance()->SelectRandEquip(m_pOwner->GetOccu(), uEquipLevel);
// 		if(0 == uEquipId)
// 			break;
// 		
// 		UInt8	uQuality = ITEM_QUALITY_BLUE;
// 		if(Avalon::Random::SelectInHundred(30))
// 			uQuality = ITEM_QUALITY_PURPLE;
// 		UInt32	uPrice = ScriptMgr::Instance()->GetPlayerScript()->GetEquipShopPrice(uEquipLevel, uQuality);
// 		if(uPrice)
// 			break;
// 
// 		CLProtocol::RandEquipEntry	entry;
// 		entry.id = uEquipId;
// 		entry.price = uPrice;
// 		entry.quality = uQuality;
// 		m_listNormal.push_back(entry);
// 	}
// 
// 	for(UInt32 i = 0; i < m_pOwner->GetVipLvl(); i++)
// 	{
// 		UInt32	uEquipId = ItemDataEntryMgr::Instance()->SelectRandEquip(m_pOwner->GetOccu(), uEquipLevel);
// 		if(0 == uEquipId)
// 			break;
// 		
// 		UInt8	uQuality = ITEM_QUALITY_BLUE;
// 		if(Avalon::Random::SelectInHundred(50))
// 			uQuality = ITEM_QUALITY_PURPLE;
// 		UInt32	uPrice = ScriptMgr::Instance()->GetPlayerScript()->GetEquipShopPrice(uEquipLevel, uQuality);
// 		if(uPrice)
// 			break;
// 
// 		CLProtocol::RandEquipEntry	entry;
// 		entry.id = uEquipId;
// 		entry.price = uPrice;
// 		entry.quality = uQuality;
// 		m_listVip.push_back(entry);
// 	}
// 
// 	m_uLastRefreshTime = UInt32(CURRENT_TIME.Sec());
// 
// 	SetDirty();
}

void EquipShopMgr::SyncEquipList()
{
	CLProtocol::SceneRandEquipShopSync	protocol;
	protocol.uNextRefreshTime = ScriptMgr::Instance()->GetPlayerScript()->GetNextRefreshEquipTime(m_uLastRefreshTime);
	protocol.vecNormal.insert(protocol.vecNormal.end(), m_listNormal.begin(), m_listNormal.end());
	protocol.vecVip.insert(protocol.vecVip.end(), m_listVip.begin(), m_listVip.end());
	m_pOwner->SendProtocol(protocol);
}

void EquipShopMgr::Input(Avalon::NetInputStream& stream)
{
	stream & m_uLastRefreshTime;

	UInt16	uNum = 0;
	stream & uNum;
	for(UInt16 i = 0; i < uNum; i++)
	{
		CLProtocol::RandEquipEntry	entry;
		stream & entry;
		m_listNormal.push_back(entry);
	}
	
	stream & uNum;
	for(UInt16 i = 0; i < uNum; i++)
	{
		CLProtocol::RandEquipEntry	entry;
		stream & entry;
		m_listVip.push_back(entry);
	}
}

void EquipShopMgr::Output(Avalon::NetOutputStream& stream)
{
	stream & m_uLastRefreshTime;

	stream & UInt16(m_listNormal.size());
	for(std::list<CLProtocol::RandEquipEntry>::iterator iter = m_listNormal.begin();
		iter != m_listNormal.end(); iter++)
	{
		stream & *iter;
	}

	stream & UInt16(m_listVip.size());
	for(std::list<CLProtocol::RandEquipEntry>::iterator iter = m_listVip.begin();
		iter != m_listVip.end(); iter++)
	{
		stream & *iter;
	}
}

void EquipShopMgr::DecodeString(std::istringstream& is)
{
	char	split;
	is >> m_uLastRefreshTime >> split;

	UInt32	uNum = 0;
	is >> uNum >> split;
	for(UInt32 i = 0; i < uNum; i++)
	{
		CLProtocol::RandEquipEntry	entry;
		is >> entry.id >> split >> entry.price >> split;

		UInt32	uVal = 0;
		is >> uVal >> split;
		entry.quality = UInt8(uVal);
		
		m_listNormal.push_back(entry);
	}
	
	uNum = 0;
	is >> uNum >> split;
	for(UInt32 i = 0; i < uNum; i++)
	{
		CLProtocol::RandEquipEntry	entry;
		is >> entry.id >> split >> entry.price >> split;

		UInt32	uVal = 0;
		is >> uVal >> split;
		entry.quality = UInt8(uVal);
		
		m_listVip.push_back(entry);
	}
}

void EquipShopMgr::EncodeString(std::ostringstream& os)
{
	os << m_uLastRefreshTime << ";";

	os << m_listNormal.size() << ";";
	for(std::list<CLProtocol::RandEquipEntry>::iterator iter = m_listNormal.begin();
		iter != m_listNormal.end(); iter++)
	{
		os << iter->id << "," << iter->price << "," << UInt32(iter->quality) << ";";
	}

	os << m_listVip.size() << ";";
	for(std::list<CLProtocol::RandEquipEntry>::iterator iter = m_listVip.begin();
		iter != m_listVip.end(); iter++)
	{
		os << iter->id << "," << iter->price << "," << UInt32(iter->quality) << ";";
	}
}

