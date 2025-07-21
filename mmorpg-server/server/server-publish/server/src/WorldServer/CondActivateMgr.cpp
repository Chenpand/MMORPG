#include "CondActivateMgr.h"

#include <CLRecordClient.h>
#include <CLMallProtocol.h>
#include <CLErrorCode.h>

#include "WSPlayer.h"
#include "MallMgr.h"


CondActivateMgr::CondActivateMgr()
{
	m_PersonalTailorPushState = PTAS_INIT;
}

CondActivateMgr::~CondActivateMgr()
{
	for (auto info : m_AllCondActivateInfo)
	{
		if (info)
		{
			CondActivateInfo::Destroy(info);
		}
	}

	m_AllCondActivateInfo.clear();
}

void CondActivateMgr::ActivateCond(UInt8 activateCond, UInt8 activateType)
{
	switch ((ConditionActivateType)activateType)
	{
	case CAT_MALL_LIMIT_TIME_GIFT_PACK :
		break;

	case CAT_MALL_PERSONAL_TAILOR:
		_ActivatePersonalTailor(activateCond);
		break;

	default:
		ErrorLogStream << PLAYERINFO(GetOwner()) << " Error cond type(condType: " << activateType << ")!" << LogStream::eos;
	}
}

bool CondActivateMgr::AddCondActivateInfo(CondActivateInfo* info)
{
	if (!info)
	{
		ErrorLogStream << "CondActivateInfo ptr is null!" << LogStream::eos;
		return false;
	}

	if (FindCondActivateInfoByCond(info->GetActivateType(), info->GetActivateCond()))
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " add the same cond activate(type: " << info->GetActivateType() << ", cond: " << info->GetActivateCond() << ")!" << LogStream::eos;
		return false;
	}

	m_AllCondActivateInfo.push_back(info);
	return true;
}

void CondActivateMgr::OnOnline()
{
	m_PersonalTailorPushState = PTAS_ONLINE;
}

void CondActivateMgr::OnTick(const Avalon::Time& now)
{
	UInt32 curTime = (UInt32)Avalon::Time::CurrentTime().Sec();

	PersonalTailorActivateState state = PTAS_CLOSED;
	
	bool hasPersonalTailor = false;

	for (auto info : m_AllCondActivateInfo)
	{
		if (!info)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "CondActivateInfo ptr is null!" << LogStream::eos;
			continue;
		}

		if (info->GetActivateType() == (UInt8)CAT_MALL_PERSONAL_TAILOR && hasPersonalTailor != true)
		{
			hasPersonalTailor = true;
		}

		if (curTime >= info->GetActivateStartTime() && curTime <= info->GetActivateEndTime())
		{
			if (state != PTAS_OPEN)
			{
				state = PTAS_OPEN;
			}

			continue;
		}

		if (info->GetActivateState() != PTAS_CLOSED)
		{
			info->SetActivateState((UInt8)PTAS_CLOSED);
			info->UpdateToDB(GetOwner());
		}
	}

	if (hasPersonalTailor && m_PersonalTailorPushState != state)
	{
		// 玩家触发私人订制后再次上线不显示图标
		if (m_PersonalTailorPushState == PTAS_ONLINE && state == PTAS_OPEN)
		{
			_SyncMallPersonalTailorState(m_PersonalTailorPushState);
			m_PersonalTailorPushState = state;
			return;
		}

		// 有新的私人订制再同步一次
		m_PersonalTailorPushState = state;
		_SyncMallPersonalTailorState(m_PersonalTailorPushState);
	}
}

//void CondActivateMgr::VisitCondActivateInfo(CondActivateInfoVisitor& visitor)
//{
//	for (auto info : m_AllCondActivateInfo)
//	{
//		if (!info)
//		{
//			ErrorLogStream << PLAYERINFO(GetOwner()) << " CondActivateInfo ptr is null!" << LogStream::eos;
//			continue;
//		}
//
//		if (!visitor(info))
//		{
//			return;
//		}
//	}
//}

CondActivateInfo* CondActivateMgr::FindOpenCondActivateInfoByGoodsId(UInt8 activateType, UInt32 mallGoodsId)
{
	for (auto info : m_AllCondActivateInfo)
	{
		if (!info)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " CondActivateInfo ptr is null!" << LogStream::eos;
			continue;
		}

		if (info->GetActivateType() == activateType && info->GetMallGoodsID() == mallGoodsId && info->GetActivateState() == (UInt8)PTAS_OPEN)
		{
			return info;
		}
	}

	return NULL;
}

CondActivateInfo* CondActivateMgr::FindOpenCondActivateInfoByCond(UInt8 activateType, UInt8 activateCond)
{
	for (auto info : m_AllCondActivateInfo)
	{
		if (!info)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " CondActivateInfo ptr is null!" << LogStream::eos;
			continue;
		}

		if (info->GetActivateType() == activateType && info->GetActivateCond() == activateCond && info->GetActivateState() == (UInt8)PTAS_OPEN)
		{
			return info;
		}
	}

	return NULL;
}

CondActivateInfo* CondActivateMgr::FindCondActivateInfoByCond(UInt8 activateType, UInt8 activateCond)
{
	for (auto info : m_AllCondActivateInfo)
	{
		if (!info)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " CondActivateInfo ptr is null!" << LogStream::eos;
			continue;
		}

		if (info->GetActivateType() == activateType && info->GetActivateCond() == activateCond)
		{
			return info;
		}
	}

	return NULL;
}

//void CondActivateMgr::_ActivateLimitTimeGiftPack(UInt8 activateCond)
//{
//	InfoLogStream << PLAYERINFO(GetOwner()) << " start to activate limit time gift pack(cond: " << activateCond << ")." << LogStream::eos;
//
//	CLProtocol::WorldMallGiftPackActivateRet ret;
//	//ret.m_Code = ErrorCode::FAILED_TO_ACTIAVATE_MALL_LIMIT_TIME_GIFT_PACK;
//
//	auto dataEntry = MallGiftPackDataEntryMgr::Instance()->FindEntry(activateCond);
//	if (!dataEntry)
//	{
//		ErrorLogStream << "Can not find MallGiftPackDataEntry(activateCond: " << activateCond << ")!" << LogStream::eos;
//		GetOwner()->SendProtocol(ret);
//		return;
//	}
//
//	if (_IsLimitCreateLimitTimeGiftPack(dataEntry))
//	{
//		GetOwner()->SendProtocol(ret);
//		return;
//	}
//
//	
//
//	UInt32 retCode;
//	//if (itemInfo->goodsType == (UInt8)MGT_GIFT_CHOOSE_ONE)
//	//{
//	//	retCode = _ActivateChooseOneGiftPack(activateCond, ret.m_Items);
//	//	_SendToClient(ret.m_Items, retCode);
//	//	return;
//	//}
//
//	retCode = _CreateGiftPack(itemInfo, ret.m_Items, activateCond);
//	_SendToClient(ret.m_Items, retCode);
//
//	if (retCode == ErrorCode::SUCCESS)
//	{
//		GetOwner()->SendFunctionUdpLog(FunctionLogType::FUNCTION_ACTIVATE_GIFT, giftPackId, 0);
//	}
//}
//
//bool CondActivateMgr::_IsLimitCreateLimitTimeGiftPack(MallGiftPackDataEntry* dataEntry)
//{
//	if (!dataEntry)
//	{
//		ErrorLogStream << "MallGiftPackDataEntry ptr is null!" << LogStream::eos;
//		return true;
//	}
//
//	MallItemInfo* itemInfo = MallMgr::Instance()->GetMallItemInfo(dataEntry->mallGoodsId);
//	if (!itemInfo)
//	{
//		ErrorLogStream << "Can not activate limit time gift pack whitch is not mall goods(" << dataEntry->mallGoodsId << ")!" << LogStream::eos;
//		return true;
//	}
//
//	// 没有触发过的条件
//	CondActivateInfo* giftPack = FindCondActivateInfoByCond((UInt8)CAT_MALL_LIMIT_TIME_GIFT_PACK, dataEntry->activateCond);
//	if (!giftPack)
//	{
//		return false;
//	}
//
//	// 当前触发条件已经开启
//	if (giftPack->GetActivateState() == (UInt8)PTAS_OPEN)
//	{
//		return true;
//	}
//
//	// 角色次数限制
//	if (dataEntry->roleActivateTimesLimit != 0 && dataEntry->roleActivateTimesLimit <= giftPack->GetActivateCount())
//	{
//		return true;
//	}
//
//	// 账号次数限制
//	if (dataEntry->accountActivateTimesLimit != 0 && dataEntry->accountActivateTimesLimt <= giftPack->)
//	{
//		return true;
//	}
//
//	return false;
//}

void CondActivateMgr::_ActivatePersonalTailor(UInt8 activateCond)
{
	InfoLogStream << PLAYERINFO(GetOwner()) << " start to activate personal tailor(cond: " << activateCond << ")." << LogStream::eos;

	auto dataEntry = MallPersonalTailorDataEntryMgr::Instance()->FindEntry(activateCond);
	if (!dataEntry)
	{
		ErrorLogStream << "Can not find MallPersonalTailorDataEntry(activateCond: " << activateCond << ")!" << LogStream::eos;
		return;
	}

	if (_IsLimitCreatePersonalTailor(dataEntry))
	{
		return;
	}

	for (auto goodsId : dataEntry->mallGoodsIds)
	{
		if (dataEntry->mallGoodsIds.size() > 1)
		{
			MallItemInfo* info = MallMgr::Instance()->GetMallItemInfo(goodsId);
			if (!info)
			{
				ErrorLogStream << "MallItemInfo(id: " << goodsId << ") ptr is null!" << LogStream::eos;
				continue;
			}

			// 判断时装职业
			if (GetOwner()->GetOccu() == info->jobType)
			{
				_CreateCondActivate(dataEntry->activateCond, goodsId, dataEntry->limitInterval, CAT_MALL_PERSONAL_TAILOR, dataEntry->activateSubCond);
			}
		}
		else
		{
			_CreateCondActivate(dataEntry->activateCond, goodsId, dataEntry->limitInterval, CAT_MALL_PERSONAL_TAILOR, dataEntry->activateSubCond);
		}
	}
}

bool CondActivateMgr::_IsLimitCreatePersonalTailor(MallPersonalTailorDataEntry* dataEntry)
{
	if (!dataEntry)
	{
		ErrorLogStream << "MallPersonalTailorDataEntry ptr is null!" << LogStream::eos;
		return true;
	}

	if (dataEntry->mallGoodsIds.empty())
	{
		ErrorLogStream << "In personal tailor table, mallGoodsId is empty!" << LogStream::eos;
		return true;
	}

	for (auto goodsId : dataEntry->mallGoodsIds)
	{
		// 不能触发非商城商品的私人订制
		MallItemInfo* info = MallMgr::Instance()->GetMallItemInfo(goodsId);
		if (!info)
		{
			ErrorLogStream << "Can not activate personal tailor whitch is not mall goods(" << goodsId << ")!" << LogStream::eos;
			return true;
		}
	}

	// 判断共用触发
	if (dataEntry->activateSubCond > 0)
	{
		for (auto tailor : m_AllCondActivateInfo)
		{
			if (tailor->GetActivateSubCond() != dataEntry->activateSubCond)
			{
				continue;
			}

			// 已经开启
			if (tailor->GetActivateState() == (UInt8)PTAS_OPEN)
			{
				return true;
			}

			// 角色次数限制
			if (dataEntry->roleActivateTimesLimit != 0 && dataEntry->roleActivateTimesLimit <= tailor->GetActivateCount())
			{
				return true;
			}
		}
	}

	// 没有触发过的条件
	CondActivateInfo* tailor = FindCondActivateInfoByCond((UInt8)CAT_MALL_PERSONAL_TAILOR, dataEntry->activateCond);
	if (!tailor)
	{
		return false;
	}

	// 当前触发条件已经开启
	if (tailor->GetActivateState() == (UInt8)PTAS_OPEN)
	{
		return true;
	}

	// 角色次数限制
	if (dataEntry->roleActivateTimesLimit != 0 && dataEntry->roleActivateTimesLimit <= tailor->GetActivateCount())
	{
		return true;
	}

	return false;
}

void CondActivateMgr::_CreateCondActivate(UInt8 activateCond, UInt32 mallGoodsId, UInt16 limitInterval, ConditionActivateType activateType, UInt8 activateSubCond)
{
	CondActivateInfo* info = FindCondActivateInfoByCond((UInt8)activateType, activateCond);

	bool isFirst = false;
	if (!info)
	{
		info = CondActivateInfo::Create();
		if (!info)
		{
			ErrorLogStream << "CondActivateInfo ptr is null!" << LogStream::eos;
			return;
		}

		info->SetID(CLRecordClient::Instance()->GenGuid());
		info->SetAccID(GetOwner()->GetAccId());
		info->SetRoleID(GetOwner()->GetID());
		info->SetActivateCond(activateCond);

		isFirst = true;
	}

	UInt32 curTime = (UInt32)Avalon::Time::CurrentTime().Sec();
	UInt32 endTime = curTime + (UInt32)limitInterval * Avalon::Time::SECS_OF_HOUR;

	info->SetMallGoodsID(mallGoodsId);
	info->SetActivateSubCond(activateSubCond);
	info->SetActivateState((UInt8)PTAS_OPEN);
	info->SetActivateStartTime(curTime);
	info->SetActivateEndTime(endTime);
	info->AddActivateCount();
	info->SetActivateType((UInt8)activateType);

	if (isFirst)
	{
		if (AddCondActivateInfo(info))
		{
			info->InsertToDB(GetOwner());
		}
	}
	else
	{
		info->UpdateToDB(GetOwner());
	}

	if (activateType == CAT_MALL_PERSONAL_TAILOR)
	{
		// 有新的私人订制再发一次协议
		m_PersonalTailorPushState = PTAS_INIT;
	}

	InfoLogStream << PLAYERINFO(GetOwner()) << " Create CondActivateInfo(type: " << info->GetActivateType() << ", cond: " << info->GetActivateCond() << ", goodsId: " << info->GetMallGoodsID()
		<< ", startTime: " << info->GetActivateStartTime() << ", endTime: " << info->GetActivateEndTime() << ")." << LogStream::eos;
}

void CondActivateMgr::_SyncMallPersonalTailorState(PersonalTailorActivateState state)
{
	return;

	CLProtocol::WorldSyncMallPersonalTailorState protocol;
	protocol.m_State = (UInt8)state;
	protocol.m_GoodsId = 0;

	GetOwner()->SendProtocol(protocol);
	InfoLogStream << PLAYERINFO(GetOwner()) << " Sync mall personal tailor(state: " << protocol.m_State << ")." << LogStream::eos;
}