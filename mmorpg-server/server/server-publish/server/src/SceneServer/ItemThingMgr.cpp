#include "ItemThingMgr.h"

#include <CLRecordClient.h>
#include <CLErrorCode.h>
#include "Player.h"

ItemThingMgr::ItemThingMgr()
{

}

ItemThingMgr::~ItemThingMgr()
{
	m_ItemThingMap.clear();
	m_ItemThingSerialMap.clear();
}

void ItemThingMgr::OnTick(const Avalon::Time& now)
{
	std::list<SceneItemThingPtr> expireThingList;
	for (auto& iter : m_ItemThingMap)
	{
		if (now.Sec() > iter.second->timeOutStamp)
		{
			ErrorLogStream << "item thing time out:" << iter.first << " (" << iter.second->LogStr() << ")" << LogStream::eos;
			expireThingList.push_back(iter.second);
		}
	}

	for (auto& thing : expireThingList)
	{
		InfoLogStream << "remove expire item thing id:" << thing->LogStr() << LogStream::eos;
		m_ItemThingMap.erase(thing->thingId);
		m_ItemThingSerialMap.erase(thing->serialId);
	}
}

UInt32 ItemThingMgr::StartThing(Player* player, UInt64 serialId, ItemThingCost& itemCost, const std::string& reason, UInt64& thingId)
{
	if (NULL == player)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}

	// 先检查消耗
	if (itemCost.point > 0 && !player->CheckRmMoney(ST_ITEM_POINT, itemCost.point))
	{
		return ErrorCode::ITEM_THING_POINT_NOT_ENOUGH;
	}

	if (itemCost.gold > 0 && !player->CheckRmMoney(ST_ITEM_GOLD, itemCost.gold))
	{
		return ErrorCode::ITEM_THING_GOLD_NOT_ENOUGH;
	}

	// 扣除掉消耗
	if (itemCost.point > 0)
	{
		player->RemoveMoney(reason.c_str(), ST_ITEM_POINT, itemCost.point);
	}

	if (itemCost.gold > 0)
	{
		player->RemoveMoney(reason.c_str(), ST_ITEM_GOLD, itemCost.gold);
	}

	thingId = CLRecordClient::Instance()->GenGuid();

	SceneItemThingPtr itemThingPtr = std::make_shared<SceneItemThing>();
	itemThingPtr->thingId = thingId;
	itemThingPtr->roleId = player->GetID();
	itemThingPtr->serialId = serialId;
	itemThingPtr->timeOutStamp = CURRENT_TIME.Sec() + ITEM_THING_TIME_OUT;
	itemThingPtr->reason = reason;
	itemThingPtr->itemCost = itemCost;

	player->SendUdpLog("item_thing", LOG_ITEM_THING, ITEM_THING_START, thingId, serialId, reason.c_str(), itemCost.Serialize().c_str());

	InfoLogStream << PLAYERINFO(player) << " start item thing id:" << thingId << " reason:"
		<< reason << " itemThing:" << itemThingPtr->LogStr() << LogStream::eos;

	m_ItemThingMap[thingId] = itemThingPtr;
	m_ItemThingSerialMap[serialId] = itemThingPtr;

	return ErrorCode::SUCCESS;
}

UInt32 ItemThingMgr::CommitThing(Player* player, UInt64 thingId)
{
	if (NULL == player)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}
	
	SceneItemThingPtr itemThingPtr = _FindThingByThingId(thingId);
	if (NULL == itemThingPtr)
	{
		ErrorLogStream << PLAYERINFO(player) << "not find item thing id:" << thingId << LogStream::eos;
		return ErrorCode::ITEM_THING_NOT_FIND;
	}

	InfoLogStream << PLAYERINFO(player) << "commit item itemThing:" << itemThingPtr->LogStr() << LogStream::eos;

	player->SendUdpLog("item_thing", LOG_ITEM_THING, ITEM_THING_CONMIT, thingId, itemThingPtr->serialId, itemThingPtr->reason.c_str(), 
		itemThingPtr->itemCost.Serialize().c_str());

	m_ItemThingMap.erase(thingId);
	m_ItemThingSerialMap.erase(itemThingPtr->serialId);

	return ErrorCode::SUCCESS;
}

UInt32 ItemThingMgr::RollBackThing(Player* player, UInt64 thingId)
{
	if (NULL == player)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}

	SceneItemThingPtr itemThingPtr = _FindThingByThingId(thingId);
	if (NULL == itemThingPtr)
	{
		ErrorLogStream << PLAYERINFO(player) << "not find item thing id:" << thingId << LogStream::eos;
		return ErrorCode::ITEM_THING_NOT_FIND;
	}

	// 返还消耗
	ItemThingCost& itemCost = itemThingPtr->itemCost;
	if (itemCost.point > 0)
	{
		player->AddMoneyItem(itemThingPtr->reason.c_str(), ST_ITEM_POINT, itemCost.point);
	}

	if (itemCost.gold > 0)
	{
		player->AddMoneyItem(itemThingPtr->reason.c_str(), ST_ITEM_GOLD, itemCost.gold);
	}

	InfoLogStream << PLAYERINFO(player) << "roll back itemThing:" << itemThingPtr->LogStr() << LogStream::eos;

	player->SendUdpLog("item_thing", LOG_ITEM_THING, ITEM_THING_ROLL_BACK, thingId, itemThingPtr->serialId, itemThingPtr->reason.c_str(),
		itemThingPtr->itemCost.Serialize().c_str());

	m_ItemThingMap.erase(thingId);
	m_ItemThingSerialMap.erase(itemThingPtr->serialId);

	return ErrorCode::SUCCESS;
}

void ItemThingMgr::CancelThing(Player* player, UInt64 serialId)
{
	if (NULL == player)
	{
		return;
	}

	SceneItemThingPtr itemThingPtr = _FindThingBySerialId(serialId);
	if (NULL == itemThingPtr)
	{
		ErrorLogStream << PLAYERINFO(player) << "not find item serial id:" << serialId << LogStream::eos;
		return;
	}

	player->SendUdpLog("item_thing", LOG_ITEM_THING, ITEM_THING_CANCEL, itemThingPtr->thingId, serialId, itemThingPtr->reason.c_str(),
		itemThingPtr->itemCost.Serialize().c_str());

	RollBackThing(player, itemThingPtr->thingId);
}

SceneItemThingPtr ItemThingMgr::_FindThingByThingId(UInt64 thingId)
{
	auto iter = m_ItemThingMap.find(thingId);
	if (iter == m_ItemThingMap.end())
	{
		return NULL;
	}

	SceneItemThingPtr thing = iter->second;
	if (NULL == thing)
	{
		return NULL;
	}

	auto it = m_ItemThingSerialMap.find(thing->serialId);
	return it != m_ItemThingSerialMap.end() ? it->second : NULL;
}

SceneItemThingPtr ItemThingMgr::_FindThingBySerialId(UInt64 serialId)
{
	auto iter = m_ItemThingSerialMap.find(serialId);
	if (iter == m_ItemThingSerialMap.end())
	{
		return NULL;
	}

	SceneItemThingPtr thing = iter->second;
	if (NULL == thing)
	{
		return NULL;
	}

	auto it = m_ItemThingMap.find(thing->thingId);
	return it != m_ItemThingMap.end() ? it->second : NULL;
}