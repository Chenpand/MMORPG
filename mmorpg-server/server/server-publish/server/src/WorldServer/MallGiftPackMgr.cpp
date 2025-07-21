#include "MallGiftPackMgr.h"

#include <CLRecordClient.h>
#include <CLMallProtocol.h>
#include <CLErrorCode.h>
#include <CLMallGiftPackDataEntry.h>

#include "WSPlayerLoading.h"
#include "MallMgr.h"


MallGiftPackMgr::MallGiftPackMgr()
{
	
}

MallGiftPackMgr::~MallGiftPackMgr()
{
	_DestroyGiftPacks();
}

void MallGiftPackMgr::ActivateLimitGiftPack(UInt8 activateCond)
{
	MallItemInfosVec items;

	std::vector<UInt32> giftPackIds;
	MallGiftPackDataEntryMgr::Instance()->GetGiftsByCondition((MallGiftPackActivateCond)activateCond, giftPackIds);

	if (giftPackIds.empty())//商城礼包表未配置条件
	{
		_SendToClient(items, ErrorCode::MALL_CANNOT_FIND_GIFT_PACK);
		return;
	}

	UInt32 giftPackId = *giftPackIds.begin();

	MallItemInfo* itemInfo = MallMgr::Instance()->GetMallItemInfo(giftPackId);
	if (NULL == itemInfo)
	{
		ErrorLogStream << "In MallItemTable, there has no such gift pack(" << giftPackId << ")!" << LogStream::eos;
		_SendToClient(items, ErrorCode::MALL_CANNOT_FIND_GIFT_PACK);
		return;
	}

	InfoLogStream << PLAYERINFO(GetOwner()) << " activate limit gift pack(" << giftPackId << "," << (UInt32)itemInfo->goodsType << "), by condition(" << (UInt32)activateCond << ")." << LogStream::eos;

	switch ((MallGoodsType)itemInfo->goodsType)
	{
	case MGT_GIFT_ACTIVATE_ONCE:
		if (_IsAccLimitActivateGiftPack(activateCond, (UInt16)MGPLN_ONCE))
		{
			_SendToClient(items, ErrorCode::MALL_GIFT_PACK_ACTIVATED);
			return;
		}
		break;

	case MGT_GIFT_ACTIVATE_THREE_TIMES:
		if (_IsRoleLimitActivateGiftPack(activateCond, (UInt16)MGPLN_ONCE) || _IsAccLimitActivateGiftPack(activateCond, (UInt16)MGPLN_THREE_TIMES))
		{
			_SendToClient(items, ErrorCode::MALL_GIFT_PACK_ACTIVATED);
			return;
		}
		break;

	default:
		ErrorLogStream << "The gift pack(" << giftPackId << ") is not gift or is daily refresh gift!" << LogStream::eos;
		_SendToClient(items, ErrorCode::MALL_CANNOT_FIND_GIFT_PACK);
		return;
	}

	UInt32 retCode;
	//if (itemInfo->goodsType == (UInt8)MGT_GIFT_CHOOSE_ONE)
	//{
	//	retCode = _ActivateChooseOneGiftPack(activateCond, items);
	//	_SendToClient(items, retCode);
	//	return;
	//}

	retCode = _CreateGiftPack(itemInfo, items, activateCond);
	_SendToClient(items, retCode);

	if (retCode == ErrorCode::SUCCESS)
	{
		GetOwner()->SendFunctionUdpLog(FunctionLogType::FUNCTION_ACTIVATE_GIFT, giftPackId, 0);
	}
}

bool MallGiftPackMgr::SetRoleMallGiftPack(MallGiftPack* giftPack)
{
	if (giftPack == NULL)
	{
		ErrorLogStream << "player(" << m_Owner->GetID() << ", " << m_Owner->GetAccId() << ") MallItemInfo ptr is null!" << LogStream::eos;
		return false;
	}

	auto iter = m_RoleGiftPacks.find(giftPack->GetGiftPackID());
	if (iter != m_RoleGiftPacks.end())
	{
		ErrorLogStream << "player(" << m_Owner->GetID() << ", " << m_Owner->GetAccId() << ") had the giftPack(" << giftPack->GetGiftPackID() << ") already!" << LogStream::eos;
		return false;
	}

	m_RoleGiftPacks[giftPack->GetGiftPackID()] = giftPack;
	return true;
}

MallGiftPack* MallGiftPackMgr::GetMallGiftPack(UInt32 mallGoodsId)
{
	auto giftPackIter = m_RoleGiftPacks.find(mallGoodsId);
	if (giftPackIter != m_RoleGiftPacks.end())
	{
		return giftPackIter->second;
	}
	return NULL;
}

void MallGiftPackMgr::SetRoleCondCounter(UInt8 cond, UInt16 counter)
{
	m_RoleCondCounter[cond] = counter;
}

void MallGiftPackMgr::AddAccountCondCounter(UInt8 cond, UInt16 counter)
{
	m_AccCondCounter[cond] += counter;
	DebugLogStream << PLAYERINFO(GetOwner()) << " add account condition(" << (UInt32)cond << ")'s count(" << counter << "), now count=" << m_AccCondCounter[cond] << LogStream::eos;
}

UInt32 MallGiftPackMgr::_CreateGiftPack(MallItemInfo* itemInfo, MallItemInfosVec& itemInfos, UInt8 activateCond)
{
	if (itemInfo == NULL)
	{
		ErrorLogStream << "MallItemInfo ptr is null!" << LogStream::eos;
		return ErrorCode::MALL_CANNOT_FIND_GIFT_PACK;
	}

	UInt32 giftPackId = itemInfo->id;
	UInt16 limitNum = itemInfo->limitTotalNum;

	bool isNewGiftPack = false;
	MallGiftPack* giftPack = GetMallGiftPack(giftPackId);
	if (!giftPack)
	{
		giftPack = MallGiftPack::Create();
		giftPack->SetID(CLRecordClient::Instance()->GenGuid());
		isNewGiftPack = true;
	}
	else
	{
		if (giftPack->GetActivateState() == (UInt8)MGPAT_OPEN)
		{
			return ErrorCode::MALL_GIFT_PACK_ACTIVATED;
		}
	}

	MallGiftPackDataEntry* giftData = MallGiftPackDataEntryMgr::Instance()->FindEntry(giftPackId);
	if (giftData == NULL)
	{
		ErrorLogStream << "MallGiftPackDataEntry ptr is null!" << LogStream::eos;
		return ErrorCode::MALL_CANNOT_FIND_GIFT_PACK;
	}

	UInt32 intervals = giftData->limitInterval * Avalon::Time::SECS_OF_HOUR;
	UInt32 startTime = (UInt32)CURRENT_TIME.Sec();
	UInt32 endTime = startTime + intervals;

	giftPack->SetAccId(m_Owner->GetAccId());
	giftPack->SetPlayerId(m_Owner->GetID());
	giftPack->SetGiftPackID(giftPackId);
	giftPack->SetActivateStartTime(startTime);
	giftPack->SetActivateEndTime(endTime);
	giftPack->SetRestNum(limitNum);
	giftPack->SetActivateCond(activateCond);
	giftPack->SetActivateState(MGPAT_OPEN);
	giftPack->SetActivateCounter(1);

	if (isNewGiftPack)
	{
		giftPack->InsertProperty(m_Owner);
		SetRoleMallGiftPack(giftPack);
	}
	else
	{
		giftPack->UpdateToDB(m_Owner);
	}

	SetRoleCondCounter(giftPack->GetActivateCond(), giftPack->GetActivateCounter());
	AddAccountCondCounter(giftPack->GetActivateCond(), giftPack->GetActivateCounter());

	MallItemInfo info = *itemInfo;
	info.startTime = startTime;
	info.endTime = endTime;
	itemInfos.push_back(info);

	DevLogStream << "player(" << m_Owner->GetID() << ", " << m_Owner->GetAccId() << ") activate mall gift pack(" << giftPackId << ")!" << LogStream::eos;
	return ErrorCode::SUCCESS;
}

bool MallGiftPackMgr::_IsRoleLimitActivateGiftPack(UInt8 activateCond, UInt16 limitTimes)
{
	auto condIter = m_RoleCondCounter.find(activateCond);
	if (condIter == m_RoleCondCounter.end())
	{
		ErrorLogStream << "player(" << m_Owner->GetID() << ", " << m_Owner->GetAccId() << ") has no such condition(" << activateCond << ") to activate one gift pack!" << LogStream::eos;
		return false;
	}

	UInt16 roleCounter = condIter->second;
	if (roleCounter < limitTimes)
	{
		return false;
	}

	return true;
}

bool MallGiftPackMgr::_IsAccLimitActivateGiftPack(UInt8 activateCond, UInt16 limitTimes)
{
	auto condIter = m_AccCondCounter.find(activateCond);
	if (condIter == m_AccCondCounter.end())
	{
		ErrorLogStream << "player(" << m_Owner->GetID() << ", " << m_Owner->GetAccId() << ") has no such condition(" << activateCond << ") to activate one gift pack!" << LogStream::eos;
		return false;
	}

	UInt16 accCounter = condIter->second;
	if (accCounter < limitTimes)
	{
		return false;
	}

	return true;
}

UInt32 MallGiftPackMgr::_ActivateChooseOneGiftPack(UInt8 activateCond, MallItemInfosVec& itemInfos)
{
	std::vector<UInt32> giftPackIds;
	MallGiftPackDataEntryMgr::Instance()->GetGiftsByCondition((MallGiftPackActivateCond)activateCond, giftPackIds);

	auto iter = giftPackIds.begin();
	for (; iter != giftPackIds.end(); ++iter)
	{
		UInt32 giftPackId = *iter;
		MallItemInfo* itemInfo = MallMgr::Instance()->GetMallItemInfo(giftPackId);
		if (NULL == itemInfo)
		{
			ErrorLogStream << "In MallItemTable, there has no such gift pack(" << giftPackId << ")!" << LogStream::eos;
			return ErrorCode::MALL_CANNOT_FIND_GIFT_PACK;
		}

		UInt32 retCode = _CreateGiftPack(itemInfo, itemInfos, activateCond);
		if (retCode == ErrorCode::MALL_GIFT_PACK_ACTIVATED)
		{
			return retCode;
		}
	}

	return ErrorCode::SUCCESS;
}

void MallGiftPackMgr::_SendToClient(MallItemInfosVec items, UInt32 errorCode)
{
	CLProtocol::WorldMallGiftPackActivateRet ret;
	ret.m_Items = items;
	ret.m_Code = errorCode;
	m_Owner->SendProtocol(ret);
}

void MallGiftPackMgr::_DestroyGiftPacks()
{
	auto iter = m_RoleGiftPacks.begin();
	while (iter != m_RoleGiftPacks.end())
	{
		auto giftPack = iter->second;
		if (giftPack)
		{
			CL_SAFE_DELETE(giftPack);
		}
		iter = m_RoleGiftPacks.erase(iter);
	}
}
