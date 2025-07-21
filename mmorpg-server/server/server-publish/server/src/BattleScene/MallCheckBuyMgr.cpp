#include "MallCheckBuyMgr.h"

#include <CLMailDefine.h>
#include <CLErrorCode.h>

#include "SSRouter.h"
#include "SceneSysNotify.h"
#include "Player.h"
#include "ActivityMgr.h"


MallCheckBuyInfo::MallCheckBuyInfo()
{
	m_PlayerId = 0;
	m_AccId = 0;
	m_Cost = 0;
	m_MoneyType = 0;
	m_MallItemId = 0;
	m_MallItemNum = 0;
	m_Receiver = 0;
	m_MallLimitBuyType = MLBT_INVALID;
	m_LimitNum = 0;
	m_TotalLimitNum = 0;
	m_GoodsType = 0;
	m_VipScore = 0;
	m_DiscountRate = 0;
	m_DiscountCouponId = 0;
}

MallCheckBuyInfo::~MallCheckBuyInfo()
{

}

void MallCheckBuyInfo::Init(const CLProtocol::WorldMallCheckBuy &req)
{
	m_PlayerId = req.m_uPlayerId;
	m_AccId = req.m_uAccId;
	m_Cost = req.m_uCost;
	m_MoneyType = req.m_uMoneyType;
	m_MallItemId = req.m_uMallItemId;
	m_MallItemNum = req.m_uMallItemNum;
	m_ItemDataId = req.m_uItemDataId;
	m_ItemData = req.m_uItemData;
	m_Receiver = req.m_uReceiver;
	m_MallLimitBuyType = (MallLimitBuyType)req.mallLimitBuyType;
	m_LimitNum = req.m_uLimitNum;
	m_TotalLimitNum = req.m_uTotalLimitNum;
	m_GoodsType = req.m_uGoodsType;
	m_VipScore = req.m_uVipScore;
	m_DiscountRate = req.m_DiscountRate;
	m_DiscountCouponId = req.m_discountCouponId;
}

void MallCheckBuyInfo::OnBuy()
{
	m_Owner->ClearMallGetItem();
	REGIST_PLAYER_EVT_HANDLER(m_Owner->GetID(), Player, ET_NOTIFY_NEWITEM, m_Owner, &Player::OnGetMallItem);

	CLProtocol::WorldMallCheckBuyRet ret;
	ret.m_uPlayerId = m_PlayerId;
	ret.m_uRet = ErrorCode::SUCCESS;
	ret.m_uMallItemId = m_MallItemId;
	ret.m_uMallItemNum = m_MallItemNum;
	ret.m_uReceiver = m_Receiver;
	ret.m_restLimitNum = -1;

	std::string counterName = _GetCounterName();
	if (_IsLimitBuy(counterName))
	{
		return;
	}

	//是否时装
	bool isFashion = false;

	for (auto itemDataId : m_ItemDataId)
	{
		ItemDataEntry *pDataEntry = ItemDataEntryMgr::Instance()->FindEntry(itemDataId);
		if (NULL == pDataEntry)
		{
			ErrorLogStream << PLAYERINFO(m_Owner) << " unknow item " << itemDataId << "!" << LogStream::eos;

			m_Owner->SendNotify("ItemDataEntry is null! key = {0}", itemDataId);
			ret.m_uRet = ErrorCode::ITEM_DATA_INVALID;

			Router::SendToWorld(ret);
			return;
		}

		UInt32 itemDataNum = _GetItemNum(itemDataId);
		if (pDataEntry->getLimitNum > 0 && m_Owner->GetItemNum(itemDataId) + itemDataNum > pDataEntry->getLimitNum)
		{
			m_Owner->SendNotify(9103);
			ret.m_uRet = ErrorCode::ITEM_DATA_INVALID;

			Router::SendToWorld(ret);
			return;
		}

		if (isFashion != true && _IsFashion(pDataEntry->subType))
		{
			isFashion = true;
		}
	}

	UInt32 cost = m_Cost;
	UInt32 discountRate = 0;
	if (isFashion)
	{
		// 打折率
		discountRate = _GenMallUtilDiscountRate(m_Owner, m_DiscountRate);
		if (discountRate > 0)
		{
			// 打折后的价格
			cost = (UInt32)(cost * ((float)discountRate / DISCOUNT_RATE_DENOMINATOR));
		}
	}
	
	bool removeDiscountCoupon = false;
	UInt32 removeDiscountCouponNum = 0;
	UInt32 discountCouponNum = m_Owner->GetItemNum(m_DiscountCouponId);
	if (m_DiscountCouponId != 0 && discountCouponNum > 0 && m_MallItemNum > 0)
	{
		UInt32 discount_rate = 0;
		ItemDataEntry* entry = ItemDataEntryMgr::Instance()->FindEntry(m_DiscountCouponId);
		if (entry)
		{
			discount_rate = entry->discountGiftCouponPro;
		}
		if (discount_rate > 0)
		{
			if (discountCouponNum >= m_MallItemNum)
			{
				// 打折后的价格
				cost = (UInt32)(cost * ((float)discount_rate / DISCOUNT_RATE_DENOMINATOR));
				removeDiscountCouponNum = m_MallItemNum;
			}
			else 
			{
				UInt32 uintPrice = m_Cost / m_MallItemNum;
				cost = (UInt32)(uintPrice * discountCouponNum * ((float)discount_rate / DISCOUNT_RATE_DENOMINATOR))
					+ uintPrice * (m_MallItemNum - discountCouponNum);
				removeDiscountCouponNum = discountCouponNum;
			}
			removeDiscountCoupon = true;
		}
	}

	std::string itemReason = GetReason(SOURCE_TYPE_SHOP_MALL, m_MallItemId, m_MallItemNum);

	switch (m_MoneyType)
	{
	case ST_BIND_POINT:
		if (m_Owner->GetPointBoth() < cost)
		{
			ret.m_uRet = ErrorCode::ITEM_NOT_ENOUGH_POINT;
			Router::SendToWorld(ret);
			return;
		}
		else
		{
			m_Owner->RemovePointBoth(itemReason.c_str(), cost);
		}
		break;

	case ST_ITEM_POINT:
		if (m_Owner->GetPoint() < cost)
		{
			ret.m_uRet = ErrorCode::ITEM_NOT_ENOUGH_POINT;
			Router::SendToWorld(ret);
			return;
		}
		else
		{
			m_Owner->RemovePoint(itemReason.c_str(), cost);
		}
		break;

	case ST_ITEM_GOLD:
		if (m_Owner->GetGold() < cost)
		{
			ret.m_uRet = ErrorCode::ITEM_NOT_ENOUGH_GOLD;
			Router::SendToWorld(ret);
			return;
		}
		else
		{
			m_Owner->RemoveGold(itemReason.c_str(), cost);
		}
		break;

	default:
		ret.m_uRet = ErrorCode::ITEM_DATA_INVALID;
		ErrorLogStream << PLAYERINFO(m_Owner) << " moneyType err mallItem " << m_MallItemId << "!" << LogStream::eos;

		m_Owner->SendNotify("moneyType err type = {0}", m_MoneyType);
		Router::SendToWorld(ret);
		return;
	}

	if (removeDiscountCoupon)
	{
		m_Owner->RemoveItem(itemReason.c_str(), m_DiscountCouponId, removeDiscountCouponNum);
	}

	// 给自己买
	if (m_Owner->GetID() == m_Receiver)
	{
		_AddItemsToPlayer(itemReason, ret.m_getItemUid);

		m_Owner->IncCounter(counterName.c_str(), m_MallItemNum);
		m_Owner->SyncProperty();

		if (m_LimitNum > 0)
		{
			ret.m_restLimitNum = m_LimitNum - m_Owner->GetCounter(counterName.c_str());
		}
		if (m_TotalLimitNum > 0)
		{
			ret.m_restLimitNum = m_TotalLimitNum - m_Owner->GetCounter(counterName.c_str());
		}
	}
	else // 给其他玩家买商城道具
	{
		if (m_ItemDataId.size() == 0)
		{
			ret.m_uRet = ErrorCode::ITEM_DATA_INVALID;
			Router::SendToWorld(ret);
			return;
		}

		_SendMailToAddonPayPlayer(m_Receiver, m_ItemDataId[0]);
	}

	Router::SendToWorld(ret);
	m_Owner->SyncToRecord();

	//log以及其他系统关联逻辑
	if (ret.m_uRet == ErrorCode::SUCCESS)
	{
		m_Owner->OnBuyMallItem(m_MallItemId, m_MallItemNum);

		if (ActivityMgr::Instance()->IsInOpActivity(OAID_MALL_DISCOUNT_FOR_NEW_SERVER) && 
			isFashion && discountRate > 0)
		{
			// 当是在购买打折时装时才计数（并且打折活动没有开启，新服打折开启）
			m_Owner->IncCounter(PLAYER_BUY_MALL_FASHION_NUM, m_MallItemNum);
		}

		//百变时装活动获得换装积分
		if (isFashion && m_MoneyType == ST_ITEM_POINT)
		{
			UInt32 actScoreNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_CHFASH_ACTSCORE_SHOPMALL);
			OpActivityRegInfo* opAct = ActivityMgr::Instance()->GetOpenActivityRegInfoByTmplateType(m_Owner, OAT_CHANGE_FASHION_MERGE);
			if (opAct)
			{
				std::string reason = GetReason(LogSourceType::SOURCE_TYPE_SHOP_MALL);
				UInt32 score = actScoreNum * cost / 100;
				if (score > 0)
				{
					m_Owner->AddItem(reason.c_str(), opAct->parm, score);
				}
			}
		}
	}

	CLEventMgr::Instance()->UnRegistPlayerEventHandler(m_Owner->GetID(), CLEventMgr::ET_NOTIFY_NEWITEM, ET_NOTIFY_NEWITEMevt);
}

bool MallCheckBuyInfo::_IsPlayerBindGiftPack()
{
	switch ((MallGoodsType)m_GoodsType)
	{
	case MGT_GIFT_ACTIVATE_ONCE: return true;
	case MGT_GIFT_ACTIVATE_THREE_TIMES: return true;
	//case MGT_GIFT_CHOOSE_ONE: return true;
	default: return false;
	}
}

bool MallCheckBuyInfo::_IsLimitBuy(const std::string& mallItemIdStr)
{
	//if (_IsPlayerBindGiftPack())
	//{
	//	if (m_TotalLimitNum == 0)
	//	{
	//		return false;
	//	}

	//	if (CounterCfg::Instance()->GetCycleType(mallItemIdStr) == COUNTER_CYCLE_INVALID)
	//	{
	//		CounterCfg::Instance()->RegCounter(mallItemIdStr.c_str(), COUNTER_CYCLE_NONE);
	//	}

	//	UInt32 totalNum = m_Owner->GetCounter(mallItemIdStr.c_str());
	//	if (m_TotalLimitNum < totalNum + m_MallItemNum)
	//	{
	//		return true;
	//	}
	//}
	//else
	//{
	if (m_TotalLimitNum > 0)
	{
		if (m_LimitNum > 0)
		{
			ErrorLogStream << PLAYERINFO(m_Owner) << "Mall item(" << m_MallItemId << ") info is error! Total limit num("
				<< m_TotalLimitNum << "), limit num(" << m_LimitNum << ")." << LogStream::eos;
			m_Owner->SendNotify(9306);
			return true;
		}

		if (CounterCfg::Instance()->GetCycleType(mallItemIdStr) == COUNTER_CYCLE_INVALID)
		{
			CounterCfg::Instance()->RegCounter(mallItemIdStr.c_str(), COUNTER_CYCLE_NONE);
		}

		UInt32 totalNum = m_Owner->GetCounter(mallItemIdStr.c_str());
		if (m_TotalLimitNum < totalNum + m_MallItemNum)
		{
			m_Owner->SendNotify(9306);
			return true;
		}
	}

	if (m_LimitNum > 0)
	{
		if (m_TotalLimitNum > 0)
		{
			ErrorLogStream << PLAYERINFO(m_Owner) << "Mall item(" << m_MallItemId << ") info is error! Total limit num("
				<< m_TotalLimitNum << "), limit num(" << m_LimitNum << ")." << LogStream::eos;
			m_Owner->SendNotify(1204);
			return true;
		}

		if (CounterCfg::Instance()->GetCycleType(mallItemIdStr) == COUNTER_CYCLE_INVALID)
		{
			switch (m_MallLimitBuyType)
			{
			case MLBT_DAILY_REFRESH:
				CounterCfg::Instance()->RegCounter(mallItemIdStr.c_str(), COUNTER_CYCLE_DAY, 6);
				break;
			case MLBT_WEEKLY_REFRESH:
				CounterCfg::Instance()->RegCounter(mallItemIdStr.c_str(), COUNTER_CYCLE_MONDAY_HOUR, 6);
				break;

			default:
				break;
			}
		}

		UInt32 buyNum = m_Owner->GetCounter(mallItemIdStr.c_str());
		if (m_LimitNum < buyNum + m_MallItemNum)
		{
			m_Owner->SendNotify(1204);
			return true;
		}
	}
	//}

	return false;
}

UInt32 MallCheckBuyInfo::_GetItemNum(UInt32 itemDataId)
{
	auto item = m_ItemData.find(itemDataId);
	if (item != m_ItemData.end())
	{
		return item->second;
	}
	ErrorLogStream << PLAYERINFO(m_Owner) << " MallGiftPack's item(" << itemDataId << ") is empty!" << LogStream::eos;
	return 0;
}

void MallCheckBuyInfo::_AddItemsToPlayer(const std::string& itemReason, std::vector<UInt64>& itemUids)
{
	if (m_ItemDataId.empty())
	{
		ErrorLogStream << PLAYERINFO(m_Owner) << " Mall items is empty on buy!" << LogStream::eos;
		return;
	}

	// 加入背包
	std::vector<UInt32> needSendToMailItems;
	for (auto itemDataId : m_ItemDataId)
	{
		UInt32 itemDataNum = _GetItemNum(itemDataId);

		if (m_Owner->CheckAddItem(itemDataId, itemDataNum))
		{
			m_Owner->AddItem(itemReason.c_str(), itemDataId, itemDataNum);

			auto mallItemId = m_Owner->GetMallGetItemID();
			if (mallItemId)
			{
				itemUids.push_back(mallItemId);
			}
		}
		else
		{
			m_Owner->SendNotify(1245);
			needSendToMailItems.push_back(itemDataId);
		}
	}

	if (needSendToMailItems.empty())
	{
		return;
	}

	// 发送邮件
	UInt32 mailRetCode;
	std::vector<UInt32> mailItems;

	for (UInt32 idx = 0; idx != needSendToMailItems.size(); ++idx)
	{
		mailItems.push_back(needSendToMailItems[idx]);
		if (mailItems.size() == MAX_MAILITEM_NUM)
		{
			mailRetCode = _AddItemsToMail(itemReason, mailItems);
			if (mailRetCode != ErrorCode::SUCCESS)
			{
				std::string itemStr = _GetItemStr(mailItems);
				ErrorLogStream << PLAYERINFO(m_Owner) << " fail to add items(" << itemStr << ") to mail(error code: " << mailRetCode << ")!" << LogStream::eos;
			}

			mailItems.clear();
		}
	}

	mailRetCode = _AddItemsToMail(itemReason, mailItems);
	if (mailRetCode != ErrorCode::SUCCESS)
	{
		std::string itemStr = _GetItemStr(mailItems);
		ErrorLogStream << PLAYERINFO(m_Owner) << " fail to add items(" << itemStr << ") to mail(error code: " << mailRetCode << ")!" << LogStream::eos;
	}
}

UInt32 MallCheckBuyInfo::_AddItemsToMail(const std::string& reason, const std::vector<UInt32>& mailItems)
{
	RewardGroup	*pGroup = m_Owner->GetRewardGroup();

	for (auto mailItemId : mailItems)
	{
		UInt32 mailItemNum = _GetItemNum(mailItemId);
		pGroup->AddReward(mailItemId, mailItemNum);
	}

	std::string strSenderName;
	std::string	strTitle;
	std::string	strContent;
	m_Owner->GetMailInfoByReason(reason.c_str(), strSenderName, strTitle, strContent);

	UInt32 mailRetCode = Sys::SendSysMail(m_Receiver, strSenderName.c_str(), strTitle.c_str(), strContent.c_str(), reason.c_str(), 30, pGroup);

	return mailRetCode;
}

void MallCheckBuyInfo::_SendMailToAddonPayPlayer(UInt64 tarId, UInt32 mailItem)
{
	RewardGroup	group;
	group.AddReward(mailItem, 1);
	
	std::string strSenderName;
	std::string	strTitle;
	std::string	strContent;
	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_MASTER_ADDON_PAY);

	m_Owner->GetMailInfoByReason(reason.c_str(), strSenderName, strTitle, strContent);

	Sys::SendSysMail(m_Receiver, strSenderName.c_str(), strTitle.c_str(), strContent.c_str(), reason.c_str(), 30, &group);

	return;
}

std::string MallCheckBuyInfo::_GetCounterName()
{
	std::ostringstream ossCounterName;
	ossCounterName << m_MallItemId;
	std::string counterName = ossCounterName.str();

	return counterName;
}

std::string MallCheckBuyInfo::_GetItemStr(const std::vector<UInt32>& mailItems)
{
	std::ostringstream oss;
	for (auto itemId : mailItems)
	{
		oss << itemId << ", ";
	}

	return oss.str();
}

bool MallCheckBuyInfo::_IsFashion(UInt8 itemSubType)
{
	switch ((ItemSubType)itemSubType)
	{
	case ST_FASHION_HEAD:
	case ST_FASHION_SASH:
	case ST_FASHION_CHEST:
	case ST_FASHION_LEG:
	case ST_FASHION_EPAULET:
		return true;

	default:
		break;
	}

	return false;
}

UInt32 MallCheckBuyInfo::_GenMallUtilDiscountRate(Player* player, UInt32 baseRate)
{
	//最终的打折率
	UInt32 utilRate = 0;

	if (!player) return utilRate;

	// 0不打折
	if (baseRate == 0)
	{
		return utilRate;
	}

	OpActivityRegInfo* newServerDiscountInfo = ActivityMgr::Instance()->GetOpActData(OAID_MALL_DISCOUNT_FOR_NEW_SERVER);
	if (newServerDiscountInfo != NULL && newServerDiscountInfo->state == (UInt8)AS_IN)
	{
		//只有前n件时装打折
		if (player->GetCounter(PLAYER_BUY_MALL_FASHION_NUM) >= 
			SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_MALL_VALID_DISCOUNT_NUM))
		{
			utilRate = 0;
		}
		else
		{
			utilRate = newServerDiscountInfo->parm;
		}
	}

	return utilRate;
}