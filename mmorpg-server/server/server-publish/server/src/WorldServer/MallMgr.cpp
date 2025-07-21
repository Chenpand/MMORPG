#include "MallMgr.h"

#include <AvalonStringUtil.h>
#include <CLMallDataEntry.h>
#include <CLBillingProtocol.h>
#include <AvalonDataTable.h>
#include <TimeUtil.h>
#include <ActivityUtil.h>
#include <CLRecordCallback.h>
#include <CLMallShopMultiIDataEntry.h>

#include "WSRouter.h"
#include "WSRelationMgr.h"
#include "WSPlayer.h"
#include "WSPlayerMgr.h"
#include "GameParamMgr.h"
#include "ScriptMgr.h"
#include "WorldSysNotify.h"
#include "WSActivityMgr.h"
#include "CLRecordClient.h"
#include "WSSelectInitDataCallback.h"
#include "WSApplication.h"
#include "MallGiftPack.h"
#include "CondActivateMgr.h"
#include "LoginPushMgr.h"
#include "MallItem.h"
#include "SysRecordMgr.h"
#include <CLRechargePushDataEntry.h>

#define MALL_PARAM_NAME "mall_"

MallMgr::MallMgr()
{
	m_minTimer.SetInterval(60 * 1000);
	m_secTimer.SetInterval(1000);
	m_GiftActivityState = MGPAS_CLOSED;
}

MallMgr::~MallMgr()
{
	Destroy();
}

bool MallMgr::Init()
{
// 	MallScript	*pScript = ScriptMgr::Instance()->GetMallScript();
// 		if(NULL == pScript)
// 			return;
	
	//pScript->Init();

	if (!LoadData())
		return false;

	//LoadDBConfig();

	return true;
}

bool MallMgr::LoadData(bool isReload)
{
	class MallItemDataVistor : public Avalon::DataEntryVisitor<MallItemDataEntry>
	{
	public:
		bool operator()(MallItemDataEntry* dataEntry)
		{
			if (!dataEntry)
			{
				ErrorLogStream << "MallItemDataEntry ptr is null!" << LogStream::eos;
				return true;
			}

			mallItemDataEntrys.push_back(dataEntry);
			return true;
		}

		std::vector<MallItemDataEntry*> mallItemDataEntrys;
	};

	MallItemDataVistor vistor;
	MallItemDataEntryMgr::Instance()->Visit(vistor);

	UInt32 curTime = (UInt32)CURRENT_TIME.Sec();

	for (auto dataEntry : vistor.mallItemDataEntrys)
	{
		if (!dataEntry) continue;

		MallItemInfo* info = new MallItemInfo();
		if (!info)
		{
			ErrorLogStream << "MallItemInfo ptr is null!" << LogStream::eos;
			continue;
		}
		
		info->id = dataEntry->id;
		info->type = dataEntry->type;
		info->subType = dataEntry->subType;
		info->jobType = dataEntry->jobType;
		info->itemid = dataEntry->itemId;
		info->itemnum = dataEntry->groupNum;
		info->price = dataEntry->price;
		info->discountprice = dataEntry->disPrice;
		info->moneytype = dataEntry->moneyType;
		info->bLimit = dataEntry->limitType;
		info->limitnum = dataEntry->limitNum;
		info->rewards = dataEntry->giftPackItems;
		info->icon = dataEntry->giftPackIcon;
		info->timeCalcType = dataEntry->timeCalcType;
		info->startTime = dataEntry->limitStartTime;
		info->endTime = dataEntry->limitEndTime;
		info->multiple = dataEntry->multiple;
		info->multipleEndTime = 0;

		UInt32 gameStartTime = 0;
		std::string startTimeStr;
		std::string endTimeStr;

		if (!dataEntry->startTimeFromService.empty())
		{
			gameStartTime = GAMESTART_TIME;
			startTimeStr = dataEntry->startTimeFromService;
			endTimeStr = dataEntry->endTimeFromService;
		}

		if (!dataEntry->startTimeFromFirstService.empty())
		{
			gameStartTime = GAME_FIRST_START_TIME;
			startTimeStr = dataEntry->startTimeFromFirstService;
			endTimeStr = dataEntry->endTimeFromFirstService;
		}

		if (gameStartTime > 0)
		{
			char split = 0;

			UInt16 fromDay = 0;
			UInt16 fromHour = 0;
			UInt16 fromMin = 0;
			UInt16 fromSec = 0;
			std::istringstream fromIs(startTimeStr);
			fromIs >> fromDay >> split >> fromHour >> split >> fromMin >> split >> fromSec;

			UInt16 toDay = 0;
			UInt16 toHour = 0;
			UInt16 toMin = 0;
			UInt16 toSec = 0;
			std::istringstream toIs(endTimeStr);
			toIs >> toDay >> split >> toHour >> split >> toMin >> split >> toSec;

			if (fromDay == 0 || toDay == 0 || toDay < fromDay)
			{
				ErrorLogStream << "Mall item(" << info->id << ") error time format(framDay: " << fromDay << ", toDay: " << toDay << ")!" << LogStream::eos;
				return false;
			}

			if (fromHour >= 24 || toHour >= 24)
			{
				ErrorLogStream << "Mall item(" << info->id << ") error time format(fromHour: " << fromHour << ", toHour: " << toHour << ")!" << LogStream::eos;
				return false;
			}

			if (fromMin >= 60 || toMin >= 60)
			{
				ErrorLogStream << "Mall item(" << info->id << ") error time format(fromMin: " << fromMin << ", toMin: " << toMin << ")!" << LogStream::eos;
				return false;
			}

			if (fromSec >= 60 || toSec >= 60)
			{
				ErrorLogStream << "Mall item(" << info->id << ") error time format(fromSec: " << fromSec << ", toSec: " << toSec << ")!" << LogStream::eos;
				return false;
			}

			UInt32 startTemp = UInt32(GetDayBeginTime(Avalon::Time(UInt64(gameStartTime) * SEC_TO_MSEC)) / SEC_TO_MSEC);

			info->startTime = startTemp + (fromDay - 1) * DAY_TO_SEC + fromHour * HOUR_TO_SEC + fromMin * MIN_TO_SEC + fromSec;
			info->endTime = startTemp + (toDay - 1) * DAY_TO_SEC + toHour * HOUR_TO_SEC + toMin * MIN_TO_SEC + toSec;
		}

		if (info->endTime > 0 && info->endTime < curTime)
		{
			CL_SAFE_DELETE(info);
			continue;
		}

		info->openInterval = dataEntry->openInterval;
		info->closeInterval = dataEntry->closeInterval;
		info->limitTotalNum = dataEntry->limitTotalNum;
		info->vipScore = dataEntry->vipScore;
		info->giftName = dataEntry->giftPackName;
		info->tagType = dataEntry->tagType;
		info->sortIdx = dataEntry->sort;
		info->hotSortIdx = dataEntry->hotSort;
		info->goodsType = dataEntry->goodsType;
		info->goodsSubType = dataEntry->goodsSubType;
		info->isRecommend = dataEntry->isRecommend;
		info->personalTailorActivateCond = dataEntry->personalTailorActivateCond;
		info->discountRate = dataEntry->discountRate;
		info->loginPushId = dataEntry->loginPushId;
		info->fashionImagePath = dataEntry->fashionImagePath;
		info->desc = dataEntry->desc;
		info->buyGotInfos = dataEntry->buyGotInfos;
		info->params = dataEntry->params;
		info->playerLevelLimit = dataEntry->playerLevelLimit;
		info->mallPushCond = dataEntry->mallPushCond;
		info->accountRefreshType = dataEntry->accountRefreshType;
		info->accountRefreshDate = dataEntry->accountRefreshDate;
		info->accountLimitBuyNum = dataEntry->accountLimitBuyNum;
		info->discountCouponId = dataEntry->discountCouponId;
		info->multiple = dataEntry->multiple;
		info->multipleEndTime = 0;
		info->deductionCouponId = dataEntry->deductionCouponId;
		info->creditPointDeduction = dataEntry->creditPointDeduction;

		if (isReload)
		{
			m_TempItems.insert(std::make_pair(info->id, info));
			continue;
		}

		if (dataEntry->goodsState == MALL_GOODS_WAIT_ON_SALE)
		{
			if (!AddMallWaitGoods(info))
			{
				return false;
			}
			continue;
		}
		
		if (!AddMallItem(info))
		{
			return false;
		}
	}
	return true;
}

void MallMgr::ReloadDataTable()
{
	InfoLogStream << "Start reloading..." << LogStream::eos;

	LoadData(true);

	if (m_TempItems.empty())
	{
		ErrorLogStream << "Reload datas are empty!" << LogStream::eos;
		return;
	}

	for (auto elem : m_allItems)
	{
		MallItemInfo* itemInfo = elem.second;
		if (!itemInfo) continue;

		auto iter = m_TempItems.find(elem.first);
		if (iter != m_TempItems.end())
		{
			*itemInfo = *iter->second;
			CL_SAFE_DELETE(iter->second);
			m_TempItems.erase(iter);
		}
		else
		{
			// 现有内存中的数据在重载后的表格中找不到，则将之关闭
			itemInfo->endTime = itemInfo->startTime + 1;
			InfoLogStream << "Mall item(" << itemInfo->id << ") will close!" << LogStream::eos;
		}
	}

	// 说明有新加的
	if (!m_TempItems.empty())
	{
		for (auto elem : m_TempItems)
		{
			MallItemInfo* newItem = elem.second;
			if (!newItem) continue;

			MallItemDataEntry* dataEntry = MallItemDataEntryMgr::Instance()->FindEntry(newItem->id);
			if (!dataEntry)
			{
				CL_SAFE_DELETE(newItem);
				continue;
			}

			if (dataEntry->goodsState == MALL_GOODS_WAIT_ON_SALE)
			{
				if (!AddMallWaitGoods(newItem))
				{
					CL_SAFE_DELETE(newItem);
				}
				continue;
			}

			if (!AddMallItem(newItem))
			{
				CL_SAFE_DELETE(newItem);
				continue;
			}

			InfoLogStream << "Add the new mall item(" << newItem->id << ")!" << LogStream::eos;
		}
	}

	m_TempItems.clear();
}

//查询商城道具回调
class SelectMallConfigCb : public SelectInitDataCallback
{
public:
	bool OnLoadFinished()
	{
		while (NextRow())
		{
			UInt32 id = (UInt32)GetKey();

			MallItemInfo* mallItemInfo = MallMgr::Instance()->GetMallItemInfo(id);

			if (mallItemInfo != NULL)
			{
				if (!Read(mallItemInfo, false))
				{
					return false;
				}
			}
			else
			{
				mallItemInfo = new MallItemInfo();
				if (!Read(mallItemInfo, true))
				{
					return false;
				}
				if (!MallMgr::Instance()->AddMallItem(mallItemInfo))
				{
					return false;
				}
			}
		}

		return true;
	}

	bool Read(MallItemInfo* info, bool newMallItemInfoPtr)
	{
		if (!newMallItemInfoPtr)
		{
			ItemRewardVec vec;
			info->rewards.clear();
			info->rewards.swap(vec);
		}

		info->id = (UInt32)GetKey();
		info->type = GetData("type");
		info->subType = GetData("subtype");
		info->jobType = GetData("jobtype");
		info->itemid = GetData("itemid");
		info->itemnum = GetData("groupnum");
		info->price = GetData("price");
		info->discountprice = GetData("disprice");
		info->moneytype = GetData("moneytype");
		info->bLimit = GetData("limittype");
		info->limitnum = (UInt16)GetData("limitnum");

		if (!MallMgr::Instance()->DecodeRewardItems(info->rewards, GetData("giftpackitems").GetString()))
		{
			ErrorLogStream << "load malldata failed!" << " decode gift pack items err!" << LogStream::eos;
			WSApplication::Instance()->Exit();
			return false;
		}

		info->icon = GetData("giftpackicon").GetString();
		info->startTime = GetData("limtstarttime");
		info->endTime = GetData("limitendtime");
		info->limitTotalNum = (UInt16)GetData("limitetotlenum");
		info->vipScore = (UInt16)GetData("vipscore");
		info->giftName = GetData("giftpackname").GetString();
		info->tagType = GetData("tagtype");
		info->sortIdx = GetData("sort");
		info->hotSortIdx = GetData("hotsort");
		info->goodsType = GetData("goods_type");
		info->goodsSubType = GetData("goods_subtype");
		info->isRecommend = GetData("is_recommend");

		return true;
	}
};

void MallMgr::LoadDBConfig()
{
	CLSelectCommand* selectMallItemCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_mall");
	CLRecordClient::Instance()->SendCommand(selectMallItemCmd, new SelectMallConfigCb());
}

void MallMgr::Destroy()
{
	auto itemsIter = m_allItems.begin();
	while (itemsIter != m_allItems.end())
	{
		auto itemInfo = itemsIter->second;

		if (m_waitGoods.find(itemInfo->id) == m_waitGoods.end())
		{
			if (itemInfo)
			{
				CL_SAFE_DELETE(itemInfo);
			}
		}
		
		itemsIter = m_allItems.erase(itemsIter);
	}

	auto itemIter = m_idxItems.begin();
	while (itemIter != m_idxItems.end())
	{
		VecItem& tmpItems = itemIter->second;
		for (size_t idx = 0; idx != tmpItems.size(); ++idx)
		{
			tmpItems.erase(tmpItems.begin() + idx);
		}

		itemIter = m_idxItems.erase(itemIter);
	}
}

void MallMgr::Buy(WSPlayer* pPlayer, UInt32 uId, UInt16 uNum, ObjID_t uReceiver)
{
// 	if(uNum > 10000) return;
// 	Buy(pPlayer, uId, uNum, uReceiver, 0 , 0, "", "", "", "");
}

void MallMgr::Buy(WSPlayer *pPlayer, UInt32 uId, UInt16 uNum, ObjID_t uReceiver, UInt32 uPrice, UInt32 billingId, const std::string& orderId, const std::string& coins,
		const std::string& moneyType, const std::string& taocanId)
{
// 	if(uNum > 10000) return;
// 	if(uReceiver == 0) uReceiver = pPlayer->GetID();
// 
// 	if(0 == uNum)
// 	{
// 		GameErrorLogStream("MallItem") << "player(" << pPlayer->GetAccId() << "," << pPlayer->GetID() << ",) buy mall item :" << uId << " num = 0" << LogStream::eos;
// 		//pPlayer->SendBillingResult(billingId, 4);
// 		return;
// 	}
// 
// 	if(uId >= BASE_LIMITED_ITEM_ID)
// 	{
// 		if(pPlayer->GetID() != uReceiver)
// 		{
// 			GameErrorLogStream("MallItem") << "player(" << pPlayer->GetAccId() <<  "," << pPlayer->GetID() << ",) send limited mall item to friend:" << uReceiver << " forbidden" << LogStream::eos;
// 			pPlayer->SendBillingResult(billingId, 4);
// 			return;
// 		}
// 
// 		BuyLimitedItem(pPlayer, uId, uNum, uReceiver, uPrice, billingId, orderId, coins, moneyType, taocanId);
// 		return;
// 	}
// 
// 	MallItemDataEntry	*pDataEntry = MallItemDataEntryMgr::Instance()->FindEntry(uId);
// 	if(NULL == pDataEntry || pDataEntry->num == 0)
// 	{
// 		GameErrorLogStream("MallItem") << "player(" << pPlayer->GetAccId() << "," << pPlayer->GetID() << ",) buy mall item :" << uId << " unexisted" << LogStream::eos;
// 		pPlayer->SendBillingResult(billingId, 4);
// 		return;
// 	}
// 
// 	if(pPlayer->GetID() != uReceiver)
// 	{
// 		WSRelation	*pRelation = pPlayer->GetRelationMgr().FindRelation(RELATION_FRIEND, uReceiver);
// 		if(NULL == pRelation)
// 		{
// 			GameErrorLogStream("MallItem") << "player(" << pPlayer->GetAccId() << "," << pPlayer->GetID() << ",) send mall item to friend:" << uReceiver << " unexisted" << LogStream::eos;
// 			pPlayer->SendBillingResult(billingId, 4);
// 			return;
// 		}
// 
// 		if(MONEY_POINT != pDataEntry->moneytype 
// 			&& MONEY_GOLD != pDataEntry->moneytype 
// 			&& MONEY_POINT != pDataEntry->moneytype)
// 			return;
// 	}
// 
// 	CLProtocol::WorldMallCheckBuy	protocol;
// 	protocol.m_uPlayerId = pPlayer->GetID();
// 	protocol.m_uAccId = pPlayer->GetAccId();
// 	UInt32 price = (0 == pDataEntry->discountprice) ? pDataEntry->price : pDataEntry->discountprice;
// 	protocol.m_uCost = price * uNum;
// 
// 	//金贝购买判断
// 	if(!orderId.empty() || !coins.empty())
// 	{
// 		if(pDataEntry->moneytype != MONEY_POINT)
// 		{
// 			pPlayer->SendBillingResult(billingId, 4);
// 			return;
// 		}
// 
// 		if(price != uPrice)
// 		{
// 			GameErrorLogStream("Billing") << "player(" << pPlayer->GetAccId() << "," << pPlayer->GetID() << "," << pPlayer->GetName() << ")"
// 				" send error request with wrong money!" << LogStream::eos;
// 			pPlayer->SendBillingResult(billingId, 4);
// 			return;
// 		}
// 	}
// 
// 	protocol.m_uMoneyType = pDataEntry->moneytype;
// 	protocol.m_uItemDataId = pDataEntry->itemid;
// 	protocol.m_uNum = pDataEntry->num * uNum;
// 	protocol.m_uMallItemId = uId;
// 	protocol.m_uMallItemNum = uNum;
// 	protocol.m_uReceiver = uReceiver;
// 	protocol.m_uItemQuality = pDataEntry->quality;
// 	if(MONEY_POINT == pDataEntry->moneytype || MONEY_GOLD == pDataEntry->moneytype 
// 		|| MONEY_POINT == pDataEntry->moneytype)
// 	{
// 		protocol.m_uBind = 0;
// 	}
// 	else
// 	{
// 		protocol.m_uBind = 1;
// 	}
// 	protocol.billingId = billingId;
// 	protocol.orderId = orderId;
// 	protocol.coins = coins;
// 	protocol.moneyType = moneyType;
// 	protocol.taocanId = taocanId;
// 
// 	pPlayer->SendToScene(protocol);
}

UInt32 MallMgr::Buy(WSPlayer* player, UInt32 itemId, UInt16 num)
{
	if (!player)
	{
		ErrorLogStream << "WSPlayer ptr is null!" << LogStream::eos;
		return ErrorCode::MALL_QUERY_FAIL;
	}

	if (0 == num)
	{
		ErrorLogStream << PLAYERINFO(player) << "Mall goods(" << itemId << ")'s num is empty!" << LogStream::eos;
		return ErrorCode::MALL_BUYNUM_ERR;
	}

	auto itr = m_allItems.find(itemId);
	if (itr == m_allItems.end())
	{
		ErrorLogStream << PLAYERINFO(player) << "Can not find mall goods(" << itemId << ")!" << LogStream::eos;
		return ErrorCode::MALL_CANNOT_FIND_MALL_GOODS;
	}

	MallItemInfo* item = itr->second;
	if (!item)
	{
		ErrorLogStream << "Can not find mall item(" << itemId << ")!" << LogStream::eos;
		return ErrorCode::MALL_CANNOT_FIND_MALL_GOODS;
	}

	if (item->subType == MST_FASHION_LIMIT)
	{
		if (!player->GetSuit())
		{
			return ErrorCode::MALL_BUYNUM_ERR;
		}
	}

	// 安全锁开启，不允许购买消耗点券
	if (item->moneytype == ST_ITEM_POINT && !player->SecurityLockCheck())
		return ErrorCode::SECURITY_LOCK_FORBID_OP;

	UInt32 curTime = (UInt32)CURRENT_TIME.Sec();

	if (!IsOnSale(player, item, curTime))
	{
		ErrorLogStream << PLAYERINFO(player) << " mall item(" << itemId << ") is not on sale!" << LogStream::eos;
		return ErrorCode::MALL_QUERY_FAIL;
	}

	if (IsLimitBuyOnAccount(player->GetAccId(), item, num))
	{
		ErrorLogStream << "Mall time(" << itemId << ") is limit buy on account!" << LogStream::eos;
		return ErrorCode::MALL_BUYNUM_ERR;
	}

	// 飞升礼包需要判断是否已经有满级号
	if (item->type == MT_FLYUP_GIFT && WSApplication::Instance()->IsLoadOtherRoleInfo() && !player->HasRoleLevelLimit(MAX_LEVEL))
	{
		ErrorLogStream << "player(" << player->GetID() << ") want buy mall time(" << itemId << ") no role level above 60!" << LogStream::eos;
		player->SendNotify(1008);
		return 1008;
	}

	CLProtocol::WorldMallCheckBuy	protocol;
	protocol.m_uPlayerId = player->GetID();
	protocol.m_uAccId = player->GetAccId();

	UInt32 price = (0 == item->discountprice) ? item->price : item->discountprice;

	protocol.m_uCost = price * num;
	protocol.m_uMoneyType = item->moneytype;
	protocol.m_uMallItemId = item->id;

	// 总购买数量=一组数量*组数
	UInt32 totalNum = item->itemnum * num;

	if (item->itemid > 0)
	{
		protocol.m_uItemDataId.push_back(item->itemid);
		protocol.m_uItemData[item->itemid] = totalNum;
	}
	else
	{
		for (auto itemReward : item->rewards)
		{
			protocol.m_uItemDataId.push_back(itemReward.id);
			protocol.m_uItemData[itemReward.id] = totalNum * itemReward.num;
		}
	}
	protocol.m_uReceiver = player->GetID();
	protocol.mallLimitBuyType = item->bLimit;
	protocol.m_uLimitNum = item->limitnum;
	protocol.m_uTotalLimitNum = item->limitTotalNum;
	protocol.m_uMallItemNum = num;
	protocol.m_uGoodsType = item->goodsType;
	protocol.m_uVipScore = item->vipScore;
	protocol.m_DiscountRate = GenDiscountRate(item, curTime);
	protocol.m_discountCouponId = item->discountCouponId;
	protocol.multiple = item->multiple;
	protocol.mallType = item->type;
	protocol.deductionCouponId = item->deductionCouponId;
	protocol.tagType = item->tagType;
	protocol.creditPointDeduction = item->creditPointDeduction;

	player->SendToScene(protocol);

	return ErrorCode::SUCCESS;
}

void MallMgr::OnCheckBuyRet(ObjID_t uPlayerId, UInt32 retCode, UInt32 uId, const std::vector<UInt64>& itemUids, UInt16 uNum, ObjID_t uReceiver, Int32 restLimitNum)
{
	WSPlayer	*pPlayer = WSPlayerMgr::Instance()->FindNormalPlayer(uPlayerId);

	if (!pPlayer)
	{
		ErrorLogStream << "Normal WSPlayer ptr is null!" << LogStream::eos;
		return;
	}

	InfoLogStream << PLAYERINFO(pPlayer) << " on check buy mall item(" << uId << ") num(" << uNum << "), retCode(" << retCode << ")." << LogStream::eos;

	if (retCode == ErrorCode::SUCCESS && restLimitNum > -1)
	{
		SetMallGiftPackNum(pPlayer, uId, restLimitNum);
	}
	
	auto itr = m_PlayersBatchBuyCtx.find(pPlayer->GetID());
	if (itr == m_PlayersBatchBuyCtx.end())
	{
		MallItemInfo* mallItem = GetMallItemInfo(uId);

		CLProtocol::WorldMallBuyRet	protocol;
		protocol.m_code = retCode;
		protocol.m_mallItemId = uId;
		protocol.m_restLimitNum = restLimitNum;

		if (mallItem != NULL && retCode == ErrorCode::SUCCESS)
		{
			InfoLogStream << PLAYERINFO(pPlayer) << " success buy mall item(" << uId << ") num(" << uNum << ") rest limit num(" << restLimitNum << ")." << LogStream::eos;

			if (mallItem->accountLimitBuyNum > 0)
			{
				AddMallItemAccountBuyRecord(pPlayer->GetAccId(), uId, uNum);
			}

			protocol.m_accountRestBuyNum = GetAccountRestBuyNum(pPlayer->GetAccId(), mallItem);

			pPlayer->OnMallItemBuy(uId, (UInt32)uNum);
		}

		pPlayer->SendProtocol(protocol);
		return;
	}
	else
	{
		MallBatchBuyCtx& ctx = itr->second;

		for (auto itemUid : itemUids)
		{
			ctx.m_itemUids.push_back(itemUid);
		}

		if (itemUids.empty())
		{
			ctx.m_itemUids.push_back(0);
		}

		if (ctx.m_itemUids.size() >= ctx.m_totleNum)
		{
			//全部收到返回
			CLProtocol::SCMallBatchBuyRes	protocol;
			protocol._code = retCode;
			protocol._itemUids = ctx.m_itemUids;
			pPlayer->SendProtocol(protocol);

			m_PlayersBatchBuyCtx.erase(itr);
		}
	}


// 	if(NULL != pPlayer)
// 	{
// 		CLProtocol::WorldMallBuyRet	protocol;
// 		//protocol.m_uRet = uSuc;
// 		pPlayer->SendProtocol(protocol);
// 	}
// 
// 	if(uPlayerId != uReceiver && uSuc == 2)
// 	{
// 		if(NULL != pPlayer)
// 		{
// 			pPlayer->AddIntimacy(uReceiver, INTIMACY_INC_BY_SEND_GIFT);
// 		}
// 
// 		IncIntimacyEvent	*pEvent = new IncIntimacyEvent(uPlayerId, INTIMACY_INC_BY_SEND_GIFT);
// 		WSPlayer::PutOfflineEvent(uReceiver, pEvent);
// 	}
// 
// 	if(uId >= BASE_LIMITED_ITEM_ID && uSuc > 0)
// 	{
// 		MallItemInfo	*pItem = NULL;
// 		if(uId >= SILVERSTONE_ITEM_ID)
// 		{
// 			std::map<UInt32, MallItemInfo>::iterator	iter = m_mapSilverStoneItem.find(uId);
// 			if(iter != m_mapSilverStoneItem.end())
// 				pItem = &(iter->second);
// 		}
// 		else if(uId >= GOLDSTONE_ITEM_ID)
// 		{
// 			std::map<UInt32, MallItemInfo>::iterator	iter = m_mapGoldStoneItem.find(uId);
// 			if(iter != m_mapGoldStoneItem.end())
// 				pItem = &(iter->second);
// 		}
// 		else if(uId >= FESTIVAL_LIMITED_ITEM_ID)
// 		{
// 			std::map<UInt32, MallItemInfo>::iterator	iter = m_mapFestivalItem.find(uId);
// 			if(iter != m_mapFestivalItem.end())
// 				pItem = &(iter->second);
// 		}
// 		else if(uId >= NORMAL_LIMITED_ITEM_ID)
// 		{
// 			std::map<UInt32, MallItemInfo>::iterator	iter = m_mapNormalItem.find(uId);
// 			if(iter != m_mapNormalItem.end())
// 				pItem = &(iter->second);
// 		}
// 		else
// 		{
// 			std::map<UInt32, MallItemInfo>::iterator iter = m_mapGSItem.find(uId);
// 			if(iter != m_mapGSItem.end())
// 				pItem = &(iter->second);
// 		}
// 
// 		if(NULL != pItem && 0 != pItem->bLimit)
// 		{
// // 			if(pItem->totalnum > uNum)
// // 				pItem->totalnum -= uNum;
// // 			else
// // 				pItem->totalnum = 0;
// 			
// 			std::ostringstream	os;
// 			os << MALL_PARAM_NAME << uId;
// 			UInt32	uSelledNum = GameParamMgr::Instance()->GetValue(os.str().c_str());
// 			GameParamMgr::Instance()->SetValue(os.str().c_str(), uSelledNum + uNum);
// 
// 			if(NULL != pPlayer)
// 			{
// 				UInt16	uBuyedNum = pPlayer->GetMallItemBuyedNum(uId);
// 				pPlayer->SetMallItemBuyedNum(uId, uBuyedNum + uNum);
// 
// 				if(uId < NORMAL_LIMITED_ITEM_ID)
// 				{
// 					MallScript	*pMallScript = ScriptMgr::Instance()->GetMallScript();
// 					if(NULL != pMallScript)
// 					{
// 						UInt32	uIndex = pMallScript->GetGSItemIndex(uId);
// 						pPlayer->SetGSMallBuyInfo(pPlayer->GetGSMallBuyInfo() | (1 << uIndex));
// 					}
// 					SyncLimitedItem(pPlayer);
// 
// 					// 广播
// 					//Sys::BroadcastNotify(1507, pPlayer->GetName().c_str(), pItem->itemid, pItem->quality);
// 					//Sys::BroadcastNotify(1508, pPlayer->GetName().c_str(), pItem->itemid, pItem->quality);
// 				}
// 				else
// 				{
// 					SyncLimitedItemNum(pPlayer, pItem);
// 				}
// 			}
// 			else
// 			{
// 				AddMallItemBuyCount	*pEvent = new AddMallItemBuyCount(uId, uNum);
// 				WSPlayer::PutOfflineEvent(uPlayerId, pEvent);
// 			}
// 		}
// 	}
}

void MallMgr::SetMallGiftPackNum(WSPlayer* pPlayer, UInt32 giftPackId, Int32 restLimitNum)
{
	MallGiftPack* giftPack = pPlayer->GetMallGiftPackMgr().GetMallGiftPack(giftPackId);

	if (!giftPack)
	{
		return;
	}

	giftPack->SetRestNum((UInt16)restLimitNum);
	giftPack->UpdateToDB(pPlayer);
}

void MallMgr::ClearLimitedItem()
{
	m_mapNormalItem.clear();
	m_mapGSItem.clear();
	m_mapFestivalItem.clear();
	m_mapGoldStoneItem.clear();
	m_mapSilverStoneItem.clear();
}

void MallMgr::ResetLimitedItemSelledInfo()
{
	for(std::map<UInt32, MallItemInfo>::iterator iter = m_mapNormalItem.begin();
		iter != m_mapNormalItem.end(); iter++)
	{
		if(0 == iter->second.bLimit)
			continue;

		std::ostringstream	os;
		os << MALL_PARAM_NAME << iter->first;
		GameParamMgr::Instance()->SetValue(os.str().c_str(), 0);
	}

	for(std::map<UInt32, MallItemInfo>::iterator iter = m_mapFestivalItem.begin();
		iter != m_mapFestivalItem.end(); iter++)
	{
		if(0 == iter->second.bLimit)
			continue;

		std::ostringstream	os;
		os << MALL_PARAM_NAME << iter->first;
		GameParamMgr::Instance()->SetValue(os.str().c_str(), 0);
	}

	for(std::map<UInt32, MallItemInfo>::iterator iter = m_mapGoldStoneItem.begin();
		iter != m_mapGoldStoneItem.end(); iter++)
	{
		if(0 == iter->second.bLimit)
			continue;

		std::ostringstream	os;
		os << MALL_PARAM_NAME << iter->first;
		GameParamMgr::Instance()->SetValue(os.str().c_str(), 0);
	}

	for(std::map<UInt32, MallItemInfo>::iterator iter = m_mapSilverStoneItem.begin();
		iter != m_mapSilverStoneItem.end(); iter++)
	{
		if(0 == iter->second.bLimit)
			continue;

		std::ostringstream	os;
		os << MALL_PARAM_NAME << iter->first;
		GameParamMgr::Instance()->SetValue(os.str().c_str(), 0);
	}
}

void MallMgr::AddNormalLimitedItem(const MallItemInfo &item)
{
// 	m_mapNormalItem[item.id] = item;
// 	
// 	if(0 != item.bLimit)
// 	{
// 		UInt32	uRestNum = item.totalnum;
// 
// 		std::ostringstream	os;
// 		os << MALL_PARAM_NAME << item.id;
// 		UInt32	uSelledNum = GameParamMgr::Instance()->GetValue(os.str().c_str());
// 
// 		if(uRestNum <= uSelledNum)
// 			m_mapNormalItem[item.id].totalnum = 0;
// 		else
// 			m_mapNormalItem[item.id].totalnum = UInt16(uRestNum - uSelledNum);
// 	}
}

void MallMgr::AddGSLimitedItem(const MallItemInfo &item)
{
// 	m_mapGSItem[item.id] = item;
// 	
// 	if(0 != item.bLimit)
// 	{
// 		UInt32	uRestNum = item.totalnum;
// 
// 		std::ostringstream	os;
// 		os << MALL_PARAM_NAME << item.id;
// 		UInt32	uSelledNum = GameParamMgr::Instance()->GetValue(os.str().c_str());
// 
// 		if(uRestNum <= uSelledNum)
// 			m_mapGSItem[item.id].totalnum = 0;
// 		else
// 			m_mapGSItem[item.id].totalnum = UInt16(uRestNum - uSelledNum);
// 	}
}

void MallMgr::AddFestivalLimitedItem(const MallItemInfo &item)
{
// 	if(m_mapFestivalItem.find(item.id) != m_mapFestivalItem.end())
// 		return;
// 
// 	m_mapFestivalItem[item.id] = item;
// 	
// 	if(0 != item.bLimit)
// 	{
// 		UInt32	uRestNum = item.totalnum;
// 
// 		std::ostringstream	os;
// 		os << MALL_PARAM_NAME << item.id;
// 		UInt32	uSelledNum = GameParamMgr::Instance()->GetValue(os.str().c_str());
// 
// 		if(uRestNum <= uSelledNum)
// 			m_mapFestivalItem[item.id].totalnum = 0;
// 		else
// 			m_mapFestivalItem[item.id].totalnum = UInt16(uRestNum - uSelledNum);
// 	}
}

void MallMgr::AddGoldStoneItem(const MallItemInfo &item)
{
// 	m_mapGoldStoneItem[item.id] = item;
// 
// 	if(0 != item.bLimit)
// 	{
// 		UInt32	uRestNum = item.totalnum;
// 
// 		std::ostringstream	os;
// 		os << MALL_PARAM_NAME << item.id;
// 		UInt32	uSelledNum = GameParamMgr::Instance()->GetValue(os.str().c_str());
// 
// 		if(uRestNum <= uSelledNum)
// 			m_mapGoldStoneItem[item.id].totalnum = 0;
// 		else
// 			m_mapGoldStoneItem[item.id].totalnum = UInt16(uRestNum - uSelledNum);
// 	}
}

void MallMgr::AddSilverStoneItem(const MallItemInfo &item)
{
// 	m_mapSilverStoneItem[item.id] = item;
// 
// 	if(0 != item.bLimit)
// 	{
// 		UInt32	uRestNum = item.totalnum;
// 
// 		std::ostringstream	os;
// 		os << MALL_PARAM_NAME << item.id;
// 		UInt32	uSelledNum = GameParamMgr::Instance()->GetValue(os.str().c_str());
// 
// 		if(uRestNum <= uSelledNum)
// 			m_mapSilverStoneItem[item.id].totalnum = 0;
// 		else
// 			m_mapSilverStoneItem[item.id].totalnum = UInt16(uRestNum - uSelledNum);
// 	}
}

void MallMgr::SetNotifyInfo(const std::string &strNotify)
{
	m_strNotify = strNotify;
	Sys::BroadcastNotify(1, 3, m_strNotify.c_str());
}

void MallMgr::OnTick(const Avalon::Time &now)
{
	if (m_secTimer.IsTimeout(now))
	{
		UInt32 curTime = (UInt32)CURRENT_TIME.Sec();

		for (auto &i : m_allItems)
		{
			if (i.second->multipleEndTime < curTime)
			{
				auto data = MallItemDataEntryMgr::Instance()->FindEntry(i.second->id);
				if (data == nullptr)
				{
					continue;
				}
				i.second->multiple = data->multiple;
				i.second->multipleEndTime = 0;
				MallShopMultiIDataEntryMgr::Instance()->GetMallItemMultiple(i.second->id, i.second->multiple, i.second->multipleEndTime);
			}
		}

		for (auto &i : m_waitGoods)
		{
			if (i.second->multipleEndTime < curTime)
			{
				auto data = MallItemDataEntryMgr::Instance()->FindEntry(i.second->id);
				if (data == nullptr)
				{
					continue;
				}
				i.second->multiple = data->multiple;
				i.second->multipleEndTime = 0;
				MallShopMultiIDataEntryMgr::Instance()->GetMallItemMultiple(i.second->id, i.second->multiple, i.second->multipleEndTime);
			}
		}

		ItemKey key((UInt8)MT_GIFT_PACK_ACTIVITY, (UInt8)MST_INVALID, (UInt8)OCCU_NONE);
		VecItem items;

		GetIdxItems(key, items);
		if (items.empty())
		{
			return;
		}

		UInt32 id = 0, startTime = 0, endTime = 0;

		MallGiftPackActivityState open = MGPAS_CLOSED;
		for (auto item : items)
		{
			if (!item)
			{
				continue;
			}

			if (curTime >= item->startTime && curTime <= item->endTime)
			{
				open = MGPAS_OPEN;

				id = item->id;
				startTime = item->startTime;
				endTime = item->endTime;

				break;
			}
		}
		
		if (m_GiftActivityState != open)
		{
			m_GiftActivityState = open;

			CLProtocol::SyncWorldMallGiftPackActivityState protocol;
			protocol.m_State = m_GiftActivityState;
			Router::BroadcastToPlayer(protocol);

			InfoLogStream << "Mall gift pack activity (gift pack: " << id << ", state: " << m_GiftActivityState << ", curTime(" << curTime
				<< ", startTime(" << startTime << "), endTime(" << endTime << ")!" << LogStream::eos;
		}
	}

	
	//if(m_minTimer.IsTimeout(now))
	//{
	//	//Avalon::Date	today(now);
	//	//if(today.Hour() != 0 && today.Min() == 0)
	//	//{
	//	//	Sys::BroadcastNotify(1, 3, m_strNotify.c_str());
	//	//}

	//	SetCurJQLBActivity();
	//	
	//	MallScript	*pScript = ScriptMgr::Instance()->GetMallScript();
	//	if(NULL != pScript)
	//	{
	//		pScript->SetCurFestivalItems();
	//	}
	//}
}

void MallMgr::OnDayChanged()
{
	ResetLimitedItemSelledInfo();

	MallScript	*pScript = ScriptMgr::Instance()->GetMallScript();
	if(NULL != pScript)
		pScript->SetCurLimitedItems();
}

void MallMgr::OnPlayerLevelChanged(WSPlayer* player, UInt16 oldLv)
{
	if (!player) return;

	MallItemInfosVec mallItems;

	for (auto elem : m_allItems)
	{
		MallItemInfo* mallItem = elem.second;
		if (!mallItem) continue;

		if (mallItem->mallPushCond != MALL_PUSH_COND_PLAYER_LEVEL_UP)
		{
			continue;
		}

		if (mallItem->playerLevelLimit == 0)
		{
			continue;
		}

		if (mallItem->playerLevelLimit == player->GetLevel())
		{
			MallItemInfo tmpVar = *mallItem;
			auto customerMallItem = CreateCustomerMallItem(player, mallItem);
			if (!customerMallItem)
			{
				continue;
			}

			customerMallItem->TransformMallItem(tmpVar);
			mallItems.push_back(tmpVar);
		}
	}

	if (!mallItems.empty())
	{
		PushMallItems(player, mallItems);
	}
}

void MallMgr::SetLoginPushBindGoodsInfo(LoginPushInfo* pushInfo)
{
	if (!pushInfo)
	{
		ErrorLogStream << "LoginPushInfo ptr is null!" << LogStream::eos;
		return;
	}

	ItemKey key((UInt8)MT_GIFT_PACK_ACTIVITY, (UInt8)MST_INVALID, (UInt8)OCCU_NONE);
	VecItem items;

	GetIdxItems(key, items);
	if (items.empty())
	{
		ErrorLogStream << "Query mall items failed, key(" << key.m_type << ", " << key.m_subType << ", " << key.m_job << ")." << LogStream::eos;
		return;
	}

	for (auto goods : items)
	{
		if (!goods)
		{
			ErrorLogStream << "MallItemInfo ptr is null!" << LogStream::eos;
			continue;
		}

		if (goods->loginPushId == pushInfo->id)
		{
			goods->startTime = pushInfo->startTime;
			goods->endTime = pushInfo->endTime;

			InfoLogStream << "Set goods(" << goods->id << ") start time=" << TimeUtil::TimestampToStandardFormat(goods->startTime) << ", end time=" << TimeUtil::TimestampToStandardFormat(goods->endTime) << "." << LogStream::eos;
		}
	}
}

void MallMgr::LoadMallItemAccountBuyRecordFromDB(CLRecordCallback* callback)
{
	if (!callback) return;

	if (!callback->NextRow())
	{
		return;
	}

	do
	{
		UInt32 accid = callback->GetData("owner");
		UInt32 mallItemId = callback->GetData("mall_item_id");

		MallItemAccountBuyIndex index(accid, mallItemId);
		MallItemAccountBuyRecord* record = FindMallItemAccountBuyRecord(index);
		if (!record)
		{
			record = new MallItemAccountBuyRecord();
			record->SetID(callback->GetKey());
			record->SetDBData(callback);
			AddMallItemAccountBuyRecord(index, record, false);
		}

	} while (callback->NextRow());
}

MallItemAccountBuyRecord* MallMgr::FindMallItemAccountBuyRecord(UInt32 accid, UInt32 mallItemId)
{
	MallItemAccountBuyIndex index(accid, mallItemId);
	return FindMallItemAccountBuyRecord(index);
}

MallItemAccountBuyRecord* MallMgr::FindMallItemAccountBuyRecord(const MallItemAccountBuyIndex& index)
{
	auto itr = m_AccountMallItemBuyRecords.find(index);
	if (itr == m_AccountMallItemBuyRecords.end())
	{
		return NULL;
	}
	return itr->second;
}

void MallMgr::AddMallItemAccountBuyRecord(const MallItemAccountBuyIndex& index, MallItemAccountBuyRecord* record, bool saveDB)
{
	m_AccountMallItemBuyRecords.insert(std::make_pair(index, record));
	if (saveDB) record->SaveToDB();
}

void MallMgr::AddMallItemAccountBuyRecord(UInt32 accid, UInt32 mallItemId, UInt32 buyNum)
{
	MallItemAccountBuyIndex index(accid, mallItemId);
	auto record = FindMallItemAccountBuyRecord(index);
	if (!record)
	{
		record = new MallItemAccountBuyRecord();
		record->SetMallItemId(mallItemId);
		record->SetOwner(accid);
		record->SetBuyedNum(buyNum);
		AddMallItemAccountBuyRecord(index, record);
	}
	else
	{
		record->IncBuyedNum(buyNum);
	}
}

UInt32 MallMgr::GetAccountRestBuyNum(UInt32 accid, const MallItemInfo* mallItem)
{
	if (!mallItem) return 0;

	// 不限购
	if (mallItem->accountLimitBuyNum == 0)
	{
		return (UInt32)INT32_MAX;
	}

	auto record = FindMallItemAccountBuyRecord(accid, mallItem->id);
	if (!record)
	{
		return mallItem->accountLimitBuyNum;
	}

	record->TryRefresh((TimeUtil::RefreshType)mallItem->accountRefreshType, mallItem->accountRefreshDate);

	// 账号下已购买数量
	UInt32 buyedNum = record->GetBuyedNum();
	if (buyedNum >= mallItem->accountLimitBuyNum)
	{
		return 0;
	}

	return mallItem->accountLimitBuyNum - buyedNum;
}

bool MallMgr::IsLimitBuyOnAccount(UInt32 accid, const MallItemInfo* mallItem, UInt32 buyNum)
{
	if (!mallItem) return true;

	if (mallItem->accountLimitBuyNum == 0)
	{
		return false;
	}

	UInt32 accountRestBuyNum = GetAccountRestBuyNum(accid, mallItem);

	if (buyNum > accountRestBuyNum)
	{
		return true;
	}

	return false;
}

void MallMgr::LoadCustomerMallItemFromDB(WSPlayer* player, CLRecordCallback* callback)
{
	if (!player || !callback) return;

	if (!callback->NextRow())
	{
		return;
	}

	do
	{
		UInt32 mallItemId = callback->GetData("mall_itemid");

		// 找到上架的商品
		auto mallItem = GetMallItemInfo(mallItemId);
		if (!mallItem)
		{
			ErrorLogStream << "Can't find mall item(" << mallItemId << ")!" << LogStream::eos;
			continue;
		}

		auto customerMallItem = FindCustomerMallItem(player->GetID(), mallItemId);
		if (!customerMallItem)
		{
			customerMallItem = new MallItem();
			customerMallItem->SetID(callback->GetKey());
			customerMallItem->SetDBData(callback);
			AddCustomerMallItem(customerMallItem);
		}

	} while (callback->NextRow());
}

MallItem* MallMgr::CreateCustomerMallItem(WSPlayer* player, const MallItemInfo* mallItem)
{
	if (!player || !mallItem) return NULL;

	UInt32 mallItemId = mallItem->id;

	auto customerMallItem = FindCustomerMallItem(player->GetID(), mallItemId);
	if (customerMallItem != NULL)
	{
		return customerMallItem;
	}

	customerMallItem = new MallItem();
	customerMallItem->SetMallItemId(mallItemId);
	customerMallItem->SetRoleId(player->GetID());
	customerMallItem->SetStartTime(CURRENT_TIME.MSec());
	
	switch ((TimeUtil::TimestampCalcType)mallItem->timeCalcType)
	{
	case TimeUtil::TIMESTAMP_CALC_TYPE_PLAYER_LEVEL_UP:
		customerMallItem->CalcEndTime(mallItem->closeInterval);
		break;

	default:
		ErrorLogStream << PLAYERINFO(player) << " try to create error mall item(" << mallItemId << "), timesstamp calc type(" << mallItem->timeCalcType << ") error!" << LogStream::eos;
		delete customerMallItem;
		return NULL;
	}

	AddCustomerMallItem(customerMallItem);
	customerMallItem->SaveToDB();

	return customerMallItem;
}

MallItem* MallMgr::FindCustomerMallItem(ObjID_t roleid, UInt32 mallItemId)
{
	auto itr = m_MallItemsOnPlayer.find(roleid);
	if (itr == m_MallItemsOnPlayer.end())
	{
		return NULL;
	}

	for (auto elem : itr->second)
	{
		if (!elem) continue;

		if (elem->GetMallItemId() == mallItemId)
		{
			return elem;
		}
	}

	return NULL;
}

void MallMgr::AddCustomerMallItem(MallItem* mallItem)
{
	m_MallItemsOnPlayer[mallItem->GetRoleId()].push_back(mallItem);
}

void MallMgr::GetMallItemByItemId(WSPlayer *pPlayer, UInt32 itemId)
{
	if (!pPlayer) return;

	UInt32 curTime = (UInt32)CURRENT_TIME.Sec();

	CLProtocol::WorldGetMallItemByItemIdRes protocol;
	protocol.retCode = ErrorCode::ITEM_DATA_INVALID;
	protocol.m_ItemId = itemId;

	for (auto elem : m_allItems)
	{
		MallItemInfo* mallItem = elem.second;
		if (!mallItem) continue;

		if (mallItem->itemid != itemId)
		{
			continue;
		}

		if (!IsOnSale(pPlayer, mallItem, curTime))
		{
			continue;
		}

		protocol.retCode = ErrorCode::SUCCESS;
		protocol.m_MallItem = *mallItem;
		break;
	}

	pPlayer->SendProtocol(protocol);
}

void MallMgr::PushMallItems(WSPlayer *player, const MallItemInfosVec& mallItems)
{
	if (!player || mallItems.empty())
	{
		return;
	}

	CLProtocol::WorldPushMallItems protocol;
	protocol.mallItems = mallItems;
	player->SendProtocol(protocol);
}

void MallMgr::SyncLimitedItem(WSPlayer *pPlayer)
{
// 	Avalon::Packet	*pPacket = Avalon::Packet::Create();
// 	pPacket->SetID(CLProtocol::WORLD_MALL_SYNC_LIMITED_ITEM);
// 
// 	Avalon::NetOutputStream	stream(pPacket->GetBuffer(), pPacket->GetMaxSize());
// 	try
// 	{
// 		stream & UInt8(m_mapNormalItem.size());
// 		if(!m_mapNormalItem.empty())
// 		{
// 			Avalon::Date	today(CURRENT_TIME);
// 			today.Hour(23);
// 			today.Min(59);
// 			today.Sec(59);
// 			stream & UInt32(today.ToTime().Sec());
// 			for(std::map<UInt32, MallItemInfo>::iterator iter = m_mapNormalItem.begin();
// 				iter != m_mapNormalItem.end(); iter++)
// 			{
// 				const MallItemInfo	&item = iter->second;
// 				item.Encode(stream);
// 
// 				if(0 != item.bLimit)
// 				{
// 					UInt16	uBuyedNum = pPlayer->GetMallItemBuyedNum(item.id);
// 					if(uBuyedNum >= item.limitnum)
// 						stream & UInt16(0);
// 					else
// 						stream & UInt16(item.limitnum - uBuyedNum);
// 				}
// 				else
// 				{
// 					stream & UInt16(0);
// 				}
// 			}
// 		}
// 
// 		stream & UInt8(m_mapFestivalItem.size());
// 		if(!m_mapFestivalItem.empty())
// 		{
// 			stream & UInt32(0);
// 			for(std::map<UInt32, MallItemInfo>::iterator iter = m_mapFestivalItem.begin();
// 				iter != m_mapFestivalItem.end(); iter++)
// 			{
// 				const MallItemInfo	&item = iter->second;
// 				item.Encode(stream);
// 
// 				if(0 != item.bLimit)
// 				{
// 					UInt16	uBuyedNum = pPlayer->GetMallItemBuyedNum(item.id);
// 					if(uBuyedNum >= item.limitnum)
// 						stream & UInt16(0);
// 					else
// 						stream & UInt16(item.limitnum - uBuyedNum);
// 				}
// 				else
// 				{
// 					stream & UInt16(0);
// 				}
// 			}
// 		}
// 
// 		stream & UInt8(m_mapGoldStoneItem.size()) & UInt32(0);
// 		for(std::map<UInt32, MallItemInfo>::iterator iter = m_mapGoldStoneItem.begin();
// 			iter != m_mapGoldStoneItem.end(); iter++)
// 		{
// 			const MallItemInfo	&item = iter->second;
// 			item.Encode(stream);
// 
// 			if(0 != item.bLimit)
// 			{
// 				UInt16	uBuyedNum = pPlayer->GetMallItemBuyedNum(item.id);
// 				if(uBuyedNum >= item.limitnum)
// 					stream & UInt16(0);
// 				else
// 					stream & UInt16(item.limitnum - uBuyedNum);
// 			}
// 			else
// 			{
// 				stream & UInt16(0);
// 			}
// 		}
// 
// 		stream & UInt8(m_mapSilverStoneItem.size()) & UInt32(0);
// 		for(std::map<UInt32, MallItemInfo>::iterator iter = m_mapSilverStoneItem.begin();
// 			iter != m_mapSilverStoneItem.end(); iter++)
// 		{
// 			const MallItemInfo	&item = iter->second;
// 			item.Encode(stream);
// 
// 			if(0 != item.bLimit)
// 			{
// 				UInt16	uBuyedNum = pPlayer->GetMallItemBuyedNum(item.id);
// 				if(uBuyedNum >= item.limitnum)
// 					stream & UInt16(0);
// 				else
// 					stream & UInt16(item.limitnum - uBuyedNum);
// 			}
// 			else
// 			{
// 				stream & UInt16(0);
// 			}
// 		}
// 		
// 		if(!m_mapGSItem.empty())
// 		{
// 			std::list<UInt32>	listGSItem;
// 			MallScript	*pScript = ScriptMgr::Instance()->GetMallScript();
// 			if(NULL != pScript)
// 				pScript->GetGSItems(pPlayer->GetGSMallBuyInfo(), listGSItem);
// 
// 			stream & UInt8(listGSItem.size());
// 			if(!listGSItem.empty())
// 			{
// 				Avalon::Date	gsday(Avalon::Time(UInt64(GAMESTART_TIME) * 1000));
// 				gsday.Hour(0);
// 				gsday.Min(0);
// 				gsday.Sec(0);
// 				stream & (UInt32(gsday.ToTime().Sec()) + 5 * DAY_TO_SEC);
// 
// 				for(std::list<UInt32>::iterator iter = listGSItem.begin();
// 					iter != listGSItem.end(); iter++)
// 				{
// 					std::map<UInt32, MallItemInfo>::iterator	itermap = m_mapGSItem.find(*iter);
// 					if(itermap == m_mapGSItem.end())
// 						continue;
// 			
// 					const MallItemInfo	&item = itermap->second;
// 					item.Encode(stream);
// 
// 					if(0 != item.bLimit)
// 					{
// 						UInt16	uBuyedNum = pPlayer->GetMallItemBuyedNum(item.id);
// 						if(uBuyedNum >= item.limitnum)
// 							stream & UInt16(0);
// 						else
// 							stream & UInt16(item.limitnum - uBuyedNum);
// 					}
// 					else
// 					{
// 						stream & UInt16(0);
// 					}
// 				}
// 			}
// 		}
// 		else
// 		{
// 			stream & UInt8(0);
// 		}
// 	}
// 	catch(Avalon::NetException&)
// 	{
// 		Avalon::Packet::Destroy(pPacket);
// 		return;
// 	}
// 
// 	pPacket->SetSize(stream.GetPos());
// 	pPlayer->SendPacket(pPacket);
// 	Avalon::Packet::Destroy(pPacket);
}

void MallMgr::BuyLimitedItem(WSPlayer *pPlayer, UInt32 uId, UInt16 uNum, ObjID_t uReceiver, UInt32 uPrice, UInt32 billingId, const std::string& orderId, const std::string& coins,
		const std::string& moneyType, const std::string& taocanId)
{
// 	MallItemInfo	*pItem = NULL;
// 	if(uId >= SILVERSTONE_ITEM_ID)
// 	{
// 		std::map<UInt32, MallItemInfo>::iterator	iter = m_mapSilverStoneItem.find(uId);
// 		if(iter != m_mapSilverStoneItem.end())
// 			pItem = &(iter->second);
// 	}
// 	else if(uId >= GOLDSTONE_ITEM_ID)
// 	{
// 		std::map<UInt32, MallItemInfo>::iterator	iter = m_mapGoldStoneItem.find(uId);
// 		if(iter != m_mapGoldStoneItem.end())
// 			pItem = &(iter->second);
// 	}
// 	else if(uId >= FESTIVAL_LIMITED_ITEM_ID)
// 	{
// 		std::map<UInt32, MallItemInfo>::iterator	iter = m_mapFestivalItem.find(uId);
// 		if(iter != m_mapFestivalItem.end())
// 			pItem = &(iter->second);
// 	}
// 	else if(uId >= NORMAL_LIMITED_ITEM_ID)
// 	{
// 		std::map<UInt32, MallItemInfo>::iterator	iter = m_mapNormalItem.find(uId);
// 		if(iter != m_mapNormalItem.end())
// 			pItem = &(iter->second);
// 	}
// 	else
// 	{
// 		std::map<UInt32, MallItemInfo>::iterator iter = m_mapGSItem.find(uId);
// 		if(iter != m_mapGSItem.end())
// 			pItem = &(iter->second);
// 	}
// 
// 	if(NULL == pItem)
// 	{
// 		GameInfoLogStream("MallItem") << "player(," << pPlayer->GetID() << ",) buy limited mall item :" << uId << " unexisted" << LogStream::eos;
// 		pPlayer->SendBillingResult(billingId, 4);
// 		pPlayer->SendNotify(1506);
// 		return;
// 	}
// 
// 	if(0 != pItem->bLimit)
// 	{
// 		if(pItem->totalnum < uNum)
// 		{
// 			GameInfoLogStream("MallItem") << "player(," << pPlayer->GetID() << ",) buy limited mall item :" << uId << " is'nt enough" << LogStream::eos;
// 			pPlayer->SendBillingResult(billingId, 4);
// 			pPlayer->SendNotify(1504);
// 			SyncLimitedItemNum(pPlayer, pItem);
// 			return;
// 		}
// 
// 		if((pPlayer->GetMallItemBuyedNum(uId) + uNum) > pItem->limitnum)
// 		{
// 			GameInfoLogStream("MallItem") << "player(," << pPlayer->GetID() << ",) buy limited mall item :" << uId << " too much " << LogStream::eos;
// 			pPlayer->SendBillingResult(billingId, 4);
// 			pPlayer->SendNotify(1505);
// 			return;
// 		}
// 	}
// 
// 	CLProtocol::WorldMallCheckBuy	protocol;
// 	protocol.m_uPlayerId = pPlayer->GetID();
// 	protocol.m_uAccId = pPlayer->GetAccId();
// 	UInt32 price = (0 == pItem->discountprice) ? pItem->price : pItem->discountprice;
// 	protocol.m_uCost = price * uNum;
// 
// 	//金贝购买判断
// 	if(!orderId.empty() || !coins.empty())
// 	{
// 		if(pItem->moneytype != MONEY_POINT)
// 		{
// 			pPlayer->SendBillingResult(billingId, 4);
// 			return;
// 		}
// 
// 		if(price != uPrice)
// 		{
// 			GameErrorLogStream("Billing") << "player(" << pPlayer->GetAccId() << "," << pPlayer->GetID() << "," << pPlayer->GetName() << ")"
// 				" send error request with wrong money!" << LogStream::eos;
// 			pPlayer->SendBillingResult(billingId, 4);
// 			return;
// 		}
// 	}
// 
// 	protocol.m_uMoneyType = pItem->moneytype;
// 	protocol.m_uItemDataId = pItem->itemid;
// 	protocol.m_uNum = pItem->itemnum * uNum;
// 	protocol.m_uMallItemId = uId;
// 	protocol.m_uMallItemNum = uNum;
// 	protocol.m_uReceiver = uReceiver;
// 	protocol.m_uItemQuality = pItem->quality;
// 	protocol.m_uBind = pItem->bBind;
// 	protocol.billingId = billingId;
// 	protocol.orderId = orderId;
// 	protocol.coins = coins;
// 	protocol.moneyType = moneyType;
// 	protocol.taocanId = taocanId;
// 
// 	pPlayer->SendToScene(protocol);
}

void MallMgr::SyncLimitedItemNum(WSPlayer *pPlayer, MallItemInfo *pItem)
{
// 	CLProtocol::WorldMallLimitedItemChange	protocol;
// 	protocol.id = pItem->id;
// 	protocol.totalnum = pItem->totalnum;
// 
// 	UInt16	uBuyedNum = pPlayer->GetMallItemBuyedNum(pItem->id);
// 	if(uBuyedNum >= pItem->limitnum)
// 		protocol.limitnum = 0;
// 	else
// 		protocol.limitnum = pItem->limitnum - uBuyedNum;
// 
// 	pPlayer->SendProtocol(protocol);
}

void MallMgr::SetCurJQLBActivity()
{
// 	Avalon::Date	today(CURRENT_TIME);
// 	if(1 == today.WDay() || 5 == today.WDay())
// 	{
// 		Avalon::Date	gsDay(Avalon::Time(UInt64(GAMESTART_TIME) * 1000));
// 		gsDay.Hour(0);
// 		gsDay.Min(0);
// 		gsDay.Sec(0);
// 
// 		UInt64	uGSWeekTime = gsDay.ToTime().Sec() - (gsDay.WDay() - 1) * DAY_TO_SEC;
// 		UInt64	uIndex = ((CURRENT_TIME.Sec() - uGSWeekTime) / (DAY_TO_SEC * 7)) % 4 + 1;
// 
// 		std::ostringstream	os;
// 		os << "pubacctBuy@" << uIndex << "_" << today.WDay();
// 
// 		today.Hour(23);
// 		today.Min(59);
// 		today.Sec(59);
// 		
// 		WSActivityMgr::Instance()->OpenActivity(0, os.str(), 0, 1, UInt32(CURRENT_TIME.Sec()), UInt32(today.ToTime().Sec()));
// 	}
}

bool MallMgr::IsOnSale(WSPlayer* player, MallItemInfo* mallItem, UInt32 limitTime)
{
	if (!player || !mallItem) return false;

	if (mallItem->playerLevelLimit > 0 && mallItem->playerLevelLimit > player->GetLevel())
	{
		return false;
	}

	switch ((TimeUtil::TimestampCalcType)mallItem->timeCalcType)
	{
	case TimeUtil::TIMESTAMP_CALC_TYPE_COMMON:
		if ((mallItem->startTime > 0 && limitTime < mallItem->startTime) ||
			(mallItem->endTime > 0 && limitTime > mallItem->endTime))
		{
			return false;
		}
		break;

	case TimeUtil::TIMESTAMP_CALC_TYPE_PLAYER_LEVEL_UP:
	{
		auto customerMallItem = FindCustomerMallItem(player->GetID(), mallItem->id);
		if (!customerMallItem)
		{
			return false;
		}

		if (!customerMallItem->IsOnSale(limitTime))
		{
			return false;
		}

		break;
	}

	default:
		break;
	}

	return true;
}

void MallMgr::AddMallItem(UInt8 type, UInt8 subType, UInt8 job, UInt8 moneyType, const char *szItem)
{
	ItemKey key(type, subType, job);
	MallItemInfo* item = new MallItemInfo();
	DecodeItemInfo(*item, szItem);

	VecItem& items = m_idxItems[key];
	items.push_back(item);

	m_allItems[item->id] = item;
}

bool MallMgr::AddMallItem(MallItemInfo* info)
{
	if (!info)
	{
		ErrorLogStream << "Failed to add the mall goods, MallItemInfoPtr is null!" << LogStream::eos;
		return false;
	}

	auto good = m_allItems.find(info->id);
	if (good != m_allItems.end())
	{
		ErrorLogStream << "The same mall goods id(" << info->id << ")." << LogStream::eos;
		return false;
	}

	m_allItems[info->id] = info;

	ItemKey key(info->type, info->subType, info->jobType);
	VecItem& items = m_idxItems[key];

	items.push_back(info);

	return true;
}

bool MallMgr::AddMallWaitGoods(MallItemInfo* info)
{
	if (!info)
	{
		ErrorLogStream << "MallItemInfo ptr is null!" << LogStream::eos;
		return false;
	}

	auto iter = m_waitGoods.find(info->id);
	if (iter != m_waitGoods.end())
	{
		ErrorLogStream << "The same mall wait goods(id: " << info->id << ")." << LogStream::eos;
		return false;
	}

	m_waitGoods[info->id] = info;

	InfoLogStream << "Add mall wait goods(id: " << info->id << ")." << LogStream::eos;
	return true;
}

void MallMgr::DecodeItemInfo(MallItemInfo &item, const char *szItem)
{
	UInt32	uVal = 0;
	char	split;
	std::istringstream	is(szItem);

	is >> item.id >> split;
	is >> item.itemid >> split;
	is >> item.itemnum >> split;
	//is >> uVal >> split;
	//item.quality = UInt8(uVal);
	is >> item.price >> split;
	is >> item.discountprice >> split;
	is >> uVal >> split;
	item.moneytype = UInt8(uVal);
	//is >> item.totalnum >> split;
	//is >> uVal >> split;
	//item.bBind = UInt8(uVal);
	is >> uVal >> split;
	item.bLimit = UInt8(uVal);
	is >> item.limitnum >> split;
	is >> uVal;
	item.goodsType = UInt8(uVal);
	//uVal = 0;
	//is >> uVal;
	//item.hide = UInt8(uVal);
}

void MallMgr::OnQueryItem(WSPlayer* player, UInt32 mallItemId)
{
	if (!player) return;

	auto mallItem = GetMallItemInfo(mallItemId);
	if (!mallItem) return;

	UInt32 curTime = (UInt32)CURRENT_TIME.Sec();

	if (!IsOnSale(player, mallItem, curTime))
	{
		return;
	}

	std::vector<CLProtocol::QueryGood> queryGoods;

	CLProtocol::QueryGood qg;
	qg.bLimit = mallItem->bLimit;
	qg.id = mallItem->id;
	qg.discountRate = GenDiscountRate(mallItem, curTime);
	queryGoods.push_back(qg);

	CLProtocol::WSRefreshLimiteBuyNum req;
	req.playerid = player->GetID();
	req.qgs = queryGoods;
	req.isPersonalTailor = mallItem->personalTailorActivateCond;
	req.mallType = mallItem->type;
	req.isQuerySingleGoods = 1;
	player->SendToScene(req);
}

UInt32 MallMgr::OnQueryItems(WSPlayer* player, UInt8 tagType, UInt8 type, UInt8 subType, UInt8 job, UInt8 moneyType, UInt8 isPersonalTailor)
{
	if (!player)
	{
		ErrorLogStream << "WSPlayer ptr is null!" << LogStream::eos;
		return ErrorCode::MALL_QUERY_FAIL;
	}

	std::vector<CLProtocol::QueryGood> queryGoods;

	//if (isPersonalTailor > 0)
	//{
	//	GetPersonalTailorGoods(queryGoods);
	//}
	//else if (tagType > 0)
	//{
	//	GetHotGoods(moneyType, queryGoods);
	//}
	//else
	{
		ItemKey key(type, subType, job);

		GetCommonGoods(player, key, queryGoods);
	}

	if (queryGoods.empty())
	{
		DebugLogStream << PLAYERINFO(player) << " query goods is empty, error key(key: " << type << ", " << subType << ", " << job << ", " << moneyType << ")!" << LogStream::eos;
	}

	CLProtocol::WSRefreshLimiteBuyNum req;
	req.playerid = player->GetID();
	req.qgs = queryGoods;
	req.isPersonalTailor = isPersonalTailor;
	req.mallType = type;
	player->SendToScene(req);

	return ErrorCode::SUCCESS;
// 	预留页码查询
// 		int queryPage = 1;
// 		int curItemNum = 1;
// 		int curPage = 1;
// 		for (auto item : items)
// 		{
// 			if(queryPage == curPage)
// 				queryItems.push_back(item);
// 			else
// 				continue;
// 	
// 			if (curItemNum > ITEM_NUM_PER_PAGE)
// 				curPage++;
// 	
// 			curItemNum++;
// 		}
}

UInt32 MallMgr::GetCommonGoods(WSPlayer* player, ItemKey& key, std::vector<CLProtocol::QueryGood>& queryGoods)
{
	if (!player)
	{
		ErrorLogStream << "WSPlayer ptr is null!" << LogStream::eos;
		return ErrorCode::MALL_QUERY_FAIL;
	}

	VecItem items;

	GetIdxItems(key, items);

	UInt32 curTime = (UInt32)CURRENT_TIME.Sec();

	for (auto item : items)
	{
		if (!item)
		{
			ErrorLogStream << "MallItemInfo ptr is null!" << LogStream::eos;
			continue;
		}

		if (!IsOnSale(player, item, curTime))
		{
			continue;
		}

		if (item->type == MT_FLYUP_GIFT && WSApplication::Instance()->IsCheckLevelWhenFlyupGift() && !player->HasRoleLevelLimit(MAX_LEVEL))
		{
			continue;
		}

		CLProtocol::QueryGood qg;
		qg.bLimit = item->bLimit;
		qg.id = item->id;
		qg.discountRate = GenDiscountRate(item, curTime);
		queryGoods.push_back(qg);
	}

	return ErrorCode::SUCCESS;
}

void MallMgr::GetIdxItems(ItemKey& key, VecItem& items)
{
	auto itemIter = m_idxItems.find(key);
	if (itemIter != m_idxItems.end())
	{
		items = itemIter->second;
	}
}

void MallMgr::GetRechargePushItems(WSPlayer* pPlayer)
{
	//DebugLogStream << "player: " << pPlayer->GetID() << LogStream::eos;

	CLProtocol::WorldGetRechargePushItemsRes res;
	res.retCode = ErrorCode::SUCCESS;
	
	class RechargePushVisitor : public Avalon::DataEntryVisitor<RechargePushDataEntry>
	{
	public:
		RechargePushVisitor(WSPlayer* player) :m_pPlayer(player){}

		bool operator()(RechargePushDataEntry* dataEntry)
		{
			UInt32 timeNow = UInt32(CURRENT_TIME.Sec());

			UInt32 pushId = dataEntry->type;
			for (auto& item : m_itemVec)
			{
				if (pushId == item.pushId)
				{
					return true;
				}
			}

			RechargePushItem tmpItem;
			tmpItem.pushId = pushId;
			tmpItem.itemId = dataEntry->pushItemDataID;
			tmpItem.itemCount = dataEntry->pushItemCount;
			tmpItem.maxTimes = dataEntry->buyTimes;
			tmpItem.price = dataEntry->pushBeforePrice;
			tmpItem.discountPrice = dataEntry->pushPrice;
			tmpItem.validTimestamp = SysRecordMgr::Instance()->GetRecordValue(SYS_BEHAVIOR_TOTAL_RECHARGE_PUSH_TIMESTAMP, SYS_BEHAVIOR_OWNER_ROLE, m_pPlayer->GetID(), pushId);
			tmpItem.buyTimes = SysRecordMgr::Instance()->GetRecordValue(SYS_BEHAVIOR_TOTAL_RECHARGE_PUSH_BUYCOUNT, SYS_BEHAVIOR_OWNER_ROLE, m_pPlayer->GetID(), pushId);
			tmpItem.lastTimestamp = SysRecordMgr::Instance()->GetRecordValue(SYS_BEHAVIOR_TOTAL_RECHARGE_PUSH_LASTTIME, SYS_BEHAVIOR_OWNER_ROLE, m_pPlayer->GetID(), pushId);

			if (tmpItem.validTimestamp > timeNow)
			{
				DebugLogStream << "player: " << m_pPlayer->GetID() << " push id : " << pushId << ",get " << timeNow << "," << tmpItem.validTimestamp << "," << tmpItem.buyTimes << "," << tmpItem.pushId << LogStream::eos;

				if ((tmpItem.validTimestamp - timeNow) <= dataEntry->pushCD*86400)
				{
					m_pPlayer->SendRechargePushUdpLog(pushId, tmpItem.discountPrice);
					m_itemVec.push_back(tmpItem);

					SysRecordMgr::Instance()->SetRecord(SYS_BEHAVIOR_TOTAL_RECHARGE_PUSH_LASTTIME, SYS_BEHAVIOR_OWNER_ROLE, m_pPlayer->GetID(), pushId, timeNow);
				}
			}
			else if (tmpItem.validTimestamp > 0 || tmpItem.buyTimes > 0)
			{
				SysRecordMgr::Instance()->SetRecord(SYS_BEHAVIOR_TOTAL_RECHARGE_PUSH_TIMESTAMP, SYS_BEHAVIOR_OWNER_ROLE, m_pPlayer->GetID(), pushId, 0);
				SysRecordMgr::Instance()->SetRecord(SYS_BEHAVIOR_TOTAL_RECHARGE_PUSH_BUYCOUNT, SYS_BEHAVIOR_OWNER_ROLE, m_pPlayer->GetID(), pushId, 0);
				SysRecordMgr::Instance()->SetRecord(SYS_BEHAVIOR_TOTAL_RECHARGE_PUSH_LASTTIME, SYS_BEHAVIOR_OWNER_ROLE, m_pPlayer->GetID(), pushId, 0);
			}

			UInt32 coolTimestamp = SysRecordMgr::Instance()->GetRecordValue(SYS_BEHAVIOR_TOTAL_RECHARGE_COOL_TIMESTAMP, SYS_BEHAVIOR_OWNER_ROLE, m_pPlayer->GetID(), pushId);
			if (coolTimestamp <=  timeNow && tmpItem.validTimestamp <= timeNow)
			{
				CLProtocol::WorldCheckRechargePushReq req;
				req.zoneId = ZONE_ID;
				req.roleId = m_pPlayer->GetID();
				req.pushId = pushId;
				req.lvLower = dataEntry->lvLower;
				req.lvToplimit = dataEntry->lvToplimit;
				req.vipLvLower = dataEntry->vipLvLower;
				req.judgePropIDs = dataEntry->judgePropIDs;
				req.daysBeforMin = dataEntry->daysBeforMin;
				req.daysBeforeMax = dataEntry->daysBeforeMax;
				req.recent = dataEntry->recent;
				req.parameterType = dataEntry->parameterType;
				req.parameter1 = dataEntry->parameter1;
				req.parameter2 = dataEntry->parameter2;

				//DebugLogStream << "player: " << m_pPlayer->GetID() << " push id : " << pushId << LogStream::eos;

				if (3 == req.parameterType)
				{
					m_pPlayer->SendToScene(req);
				}
				else
				{
					//Router::SendToCenter(req);
				}
			}
			
			return true;
		}

	public:
		WSPlayer*	m_pPlayer;
		RechargePushItemVec m_itemVec;
	};

	RechargePushVisitor visitor(pPlayer);
	RechargePushDataEntryMgr::Instance()->Visit(visitor);
	res.itemVec = visitor.m_itemVec;

	pPlayer->SendProtocol(res);
}

void MallMgr::BuyRechargePushItem(WSPlayer* pPlayer, UInt32 pushId)
{
	CLProtocol::WorldBuyRechargePushItemRes res;
	res.retCode = ErrorCode::ITEM_DATA_INVALID;

	UInt32 timeNow = UInt32(CURRENT_TIME.Sec());

	UInt32 validTimestamp = SysRecordMgr::Instance()->GetRecordValue(SYS_BEHAVIOR_TOTAL_RECHARGE_PUSH_TIMESTAMP, SYS_BEHAVIOR_OWNER_ROLE, pPlayer->GetID(), pushId);
	UInt32 buyTimes = SysRecordMgr::Instance()->GetRecordValue(SYS_BEHAVIOR_TOTAL_RECHARGE_PUSH_BUYCOUNT, SYS_BEHAVIOR_OWNER_ROLE, pPlayer->GetID(), pushId);

	//配置缺失
	const RechargePushDataEntry* dataEntry = RechargePushDataEntryMgr::Instance()->GetDataEntryByPushId(pushId);
	if (NULL == dataEntry)
	{
		pPlayer->SendProtocol(res);
		return;
	}

	//不在时间范围内
	if (timeNow >= validTimestamp || (validTimestamp - timeNow) > dataEntry->pushCD*86400)
	{
		pPlayer->SendProtocol(res);
		return;
	}
	
	//超出购买次数
	if (buyTimes >= dataEntry->buyTimes)
	{
		pPlayer->SendProtocol(res);
		return;
	}

	//先减次数，后买东西
	++buyTimes;
	SysRecordMgr::Instance()->SetRecord(SYS_BEHAVIOR_TOTAL_RECHARGE_PUSH_BUYCOUNT, SYS_BEHAVIOR_OWNER_ROLE, pPlayer->GetID(), pushId, buyTimes);

	//发送给场景
	CLProtocol::WorldSceneBuyRechargePushItemReq req;
	req.roleId = pPlayer->GetID();
	req.pushId = dataEntry->type;
	req.price = dataEntry->pushPrice;
	ItemReward tmpItem(dataEntry->pushItemDataID, dataEntry->pushItemCount, 0, 0);
	req.item = tmpItem;

	pPlayer->SendToScene(req);
}

void MallMgr::OnSceneBuyRechargePushItemRes(WSPlayer* pPlayer, UInt32 retCode, UInt32 pushId)
{
	DebugLogStream << "player: " << pPlayer->GetID() << " push id : " << pushId << " return value : " << retCode << LogStream::eos;

	CLProtocol::WorldBuyRechargePushItemRes res;
	res.retCode = retCode;

	if (ErrorCode::SUCCESS != retCode)
	{
		UInt32 validTimestamp = SysRecordMgr::Instance()->GetRecordValue(SYS_BEHAVIOR_TOTAL_RECHARGE_PUSH_TIMESTAMP, SYS_BEHAVIOR_OWNER_ROLE, pPlayer->GetID(), pushId);
		UInt32 buyTimes = SysRecordMgr::Instance()->GetRecordValue(SYS_BEHAVIOR_TOTAL_RECHARGE_PUSH_BUYCOUNT, SYS_BEHAVIOR_OWNER_ROLE, pPlayer->GetID(), pushId);
	
		UInt32 timeNow = UInt32(CURRENT_TIME.Sec());
		if (validTimestamp > timeNow)
		{
			if (buyTimes > 0)
			{
				//补回去
				--buyTimes;
				SysRecordMgr::Instance()->SetRecord(SYS_BEHAVIOR_TOTAL_RECHARGE_PUSH_BUYCOUNT, SYS_BEHAVIOR_OWNER_ROLE, pPlayer->GetID(), pushId, buyTimes);
			}
		}
		else
		{
			SysRecordMgr::Instance()->SetRecord(SYS_BEHAVIOR_TOTAL_RECHARGE_PUSH_TIMESTAMP, SYS_BEHAVIOR_OWNER_ROLE, pPlayer->GetID(), pushId, 0);
			SysRecordMgr::Instance()->SetRecord(SYS_BEHAVIOR_TOTAL_RECHARGE_PUSH_BUYCOUNT, SYS_BEHAVIOR_OWNER_ROLE, pPlayer->GetID(), pushId, 0);
			SysRecordMgr::Instance()->SetRecord(SYS_BEHAVIOR_TOTAL_RECHARGE_PUSH_LASTTIME, SYS_BEHAVIOR_OWNER_ROLE, pPlayer->GetID(), pushId, 0);
		}
	}

	class RechargePushVisitor : public Avalon::DataEntryVisitor<RechargePushDataEntry>
	{
	public:
		RechargePushVisitor(WSPlayer* player) :m_pPlayer(player){}

		bool operator()(RechargePushDataEntry* dataEntry)
		{
			UInt32 timeNow = UInt32(CURRENT_TIME.Sec());

			UInt32 pushId1 = dataEntry->type;
			for (auto& item : m_itemVec)
			{
				if (pushId1 == item.pushId)
				{
					return true;
				}
			}

			RechargePushItem tmpItem;
			tmpItem.pushId = pushId1;
			tmpItem.itemId = dataEntry->pushItemDataID;
			tmpItem.itemCount = dataEntry->pushItemCount;
			tmpItem.maxTimes = dataEntry->buyTimes;
			tmpItem.price = dataEntry->pushBeforePrice;
			tmpItem.discountPrice = dataEntry->pushPrice;
			tmpItem.validTimestamp = SysRecordMgr::Instance()->GetRecordValue(SYS_BEHAVIOR_TOTAL_RECHARGE_PUSH_TIMESTAMP, SYS_BEHAVIOR_OWNER_ROLE, m_pPlayer->GetID(), pushId1);
			tmpItem.buyTimes = SysRecordMgr::Instance()->GetRecordValue(SYS_BEHAVIOR_TOTAL_RECHARGE_PUSH_BUYCOUNT, SYS_BEHAVIOR_OWNER_ROLE, m_pPlayer->GetID(), pushId1);
			tmpItem.lastTimestamp = SysRecordMgr::Instance()->GetRecordValue(SYS_BEHAVIOR_TOTAL_RECHARGE_PUSH_LASTTIME, SYS_BEHAVIOR_OWNER_ROLE, m_pPlayer->GetID(), pushId1);

			DebugLogStream << "player: " << m_pPlayer->GetID() << ",get " << tmpItem.validTimestamp << "," << tmpItem.buyTimes << "," << tmpItem.pushId << LogStream::eos;

			if (tmpItem.validTimestamp > timeNow && (tmpItem.validTimestamp - timeNow) <= dataEntry->pushCD*86400)
			{
				m_itemVec.push_back(tmpItem);
			}

			return true;
		}

	public:
		WSPlayer*	m_pPlayer;
		RechargePushItemVec m_itemVec;
	};

	RechargePushVisitor visitor(pPlayer);
	RechargePushDataEntryMgr::Instance()->Visit(visitor);
	res.itemVec = visitor.m_itemVec;

	pPlayer->SendProtocol(res);
	return;
}

void MallMgr::OnCheckRechargePushRes(WSPlayer* pPlayer, UInt32 retCode, UInt32 pushId)
{
	DebugLogStream << "player: " << pPlayer->GetID() << " push id : " << pushId << " return value : " << retCode << LogStream::eos;

	if (ErrorCode::SUCCESS != retCode)
	{
		return;
	}

	//配置缺失
	const RechargePushDataEntry* dataEntry = RechargePushDataEntryMgr::Instance()->GetDataEntryByPushId(pushId);
	if (NULL == dataEntry)
	{
		return;
	}

	UInt32 timeNow = UInt32(CURRENT_TIME.Sec());

	UInt32 coolTimestamp = SysRecordMgr::Instance()->GetRecordValue(SYS_BEHAVIOR_TOTAL_RECHARGE_COOL_TIMESTAMP, SYS_BEHAVIOR_OWNER_ROLE, pPlayer->GetID(), pushId);
	UInt32 buyTimes = SysRecordMgr::Instance()->GetRecordValue(SYS_BEHAVIOR_TOTAL_RECHARGE_PUSH_BUYCOUNT, SYS_BEHAVIOR_OWNER_ROLE, pPlayer->GetID(), pushId);
	
	DebugLogStream << "player: " << pPlayer->GetID() << " push id : " << pushId << " time : " << timeNow << "," << coolTimestamp << "," << buyTimes << LogStream::eos;

	if (coolTimestamp > timeNow)
	{
		//do nothing
	}
	else
	{
		UInt64 timeNowMsec = timeNow;
		timeNowMsec = timeNowMsec * 1000;
		Avalon::Time tmpTime(timeNowMsec);
		Avalon::Date tmpDate(tmpTime);
		tmpDate.Hour(0);
		tmpDate.Min(0);
		tmpDate.Sec(0);

		UInt32 validTimestamp = 0;
		if (dataEntry->parameterType >= 1 && dataEntry->parameterType <= 3)
		{
			//计算出下下周一的00:00:00，截至时间
			validTimestamp = tmpDate.ToTime().Sec() - ((tmpDate.WDay() - 1) * 86400) + (7 * 86400) + (dataEntry->pushCD * 86400);
			buyTimes = 0;
			coolTimestamp = validTimestamp + dataEntry->coolCD * 86400;
		}
		else if (dataEntry->parameterType == 4)
		{
			//计算出本周一的00:00:00，并加上CD时间
			validTimestamp = tmpDate.ToTime().Sec() - ((tmpDate.WDay() - 1) * 86400) + (dataEntry->pushCD * 86400);
			buyTimes = 0;
			coolTimestamp = validTimestamp + dataEntry->coolCD * 86400;
		}

		DebugLogStream << "player: " << pPlayer->GetID() << " push id : " << pushId << " set " << timeNowMsec << "," << timeNow << "," << tmpDate.ToTime().Sec() << "," << tmpDate.WDay() << "," << validTimestamp << "," << buyTimes << LogStream::eos;

		SysRecordMgr::Instance()->SetRecord(SYS_BEHAVIOR_TOTAL_RECHARGE_PUSH_TIMESTAMP, SYS_BEHAVIOR_OWNER_ROLE, pPlayer->GetID(), pushId, validTimestamp);
		SysRecordMgr::Instance()->SetRecord(SYS_BEHAVIOR_TOTAL_RECHARGE_PUSH_BUYCOUNT, SYS_BEHAVIOR_OWNER_ROLE, pPlayer->GetID(), pushId, buyTimes);
		SysRecordMgr::Instance()->SetRecord(SYS_BEHAVIOR_TOTAL_RECHARGE_COOL_TIMESTAMP, SYS_BEHAVIOR_OWNER_ROLE, pPlayer->GetID(), pushId, coolTimestamp);
	}
}

UInt32 MallMgr::OnQueryItemDetail(UInt32 mallItemId, std::vector<MallGiftDetail>& detail)
{
	MallScript* script = ScriptMgr::Instance()->GetMallScript();
	if (!script)
	{
		ErrorLogStream << "MallScript is null!" << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}
		

	const char* items = script->GetGiftDetail(mallItemId);
	if (!items)
	{
		ErrorLogStream << "GetGiftDetail is null! mallItemId = " << mallItemId  << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	std::string str(items);
	std::vector<std::string> strGift;
	Avalon::StringUtil::Split(str, strGift, "|");

	for (size_t i = 0; i < strGift.size(); ++i)
	{
		std::istringstream	is(strGift[i].c_str());
		MallGiftDetail giftDetail;
		char	split;
		is >> giftDetail.itemId >> split >> giftDetail.num;
		detail.push_back(giftDetail);
	}

	return ErrorCode::SUCCESS;
}

void MallMgr::OnGetItemPrice(UInt32 itemId, UInt32& outPrice, UInt8& outMoneyT)
{
// 	std::map<UInt32, MallItemInfo*>::iterator itr = m_allItems.begin();
// 	for (; itr != m_allItems.end(); ++itr)
// 	{
// 		MallItemInfo* item = itr->second;
// 		if (!item)
// 			continue;
// 
// 		if (item->itemid == itemId)
// 		{
// 			outPrice = 
// 		}
// 	}
}

//UInt32 MallMgr::GetOpActivityDiscountRate()
//{
//	UInt32 rate = WSActivityMgr::Instance()->GetMallSale(OAT_MALL_SALE);
//	UInt32 rate2 = WSActivityMgr::Instance()->GetMallSale(OAT_MALL_DISCOUNT_FOR_NEW_SERVER);
//
//	if (rate > 100) rate = 0;
//	if (rate2 > 100) rate2 = 0;
//
//	OpActivityType source = OAT_MALL_SALE;
//	if (rate == 0)
//	{
//		rate = rate2;
//		source = OAT_MALL_DISCOUNT_FOR_NEW_SERVER;
//	}
//
//	InfoLogStream << "Mall discount rate(" << rate << "), source(" << (UInt8)source << ")." << LogStream::eos;
//
//	return rate;
//}

UInt32 MallMgr::GenDiscountRate(MallItemInfo* goods, UInt32 curTime)
{
	if (!goods) return 0;

	// 时装打折
	if (goods->type == (UInt8)MT_FASHION && goods->discountRate > 0)
	{
		if (goods->startTime > 0)
		{
			OpActivityRegInfo info;
			if (!WSActivityMgr::Instance()->GetOpActivityById(OAID_MALL_DISCOUNT_FOR_NEW_SERVER, info))
			{
				return 0;
			}

			// 上架30天时装逻辑
			UInt32 everTime = goods->startTime + 30 * DAY_TO_SEC;
			if (info.endTime > 0 && everTime <= info.endTime)
			{
				return goods->discountRate;
			}
			else
			{
				return 0;
			}
		}
	}

	return goods->discountRate;
}

float MallMgr::GenDiscountRate(UInt32 rate)
{
	return (float)rate / DISCOUNT_RATE_DENOMINATOR;
}

void MallMgr::GetQueryItems(WSPlayer* player, const CLProtocol::SWRefreshLimiteBuyNum& protocol, std::vector<MallItemInfo>& outGoodsInfos)
{
	if (!player)
	{
		ErrorLogStream << "WSPlayer ptr is null!" << LogStream::eos;
		return;
	}
	
	UInt32 curTime = (UInt32)Avalon::Time::CurrentTime().Sec();

	auto actSystemData = ActivitySystemConfigDataEntryMgr::Instance()->FindEntry((UInt32)ACTEM_VETERAN_RETURN);
	if (!actSystemData)
	{
		ErrorLogStream << PLAYERINFO(player) << " can't find activity ststem config data(" << (UInt32)ACTEM_VETERAN_RETURN << ")!" << LogStream::eos;
	}

	for (auto queryGoods : protocol.qgs)
	{
		auto pGoods = GetMallItemInfo(queryGoods.id);
		if (!pGoods)
		{
			ErrorLogStream << PLAYERINFO(player) << "Can not find mall goods(id: " << queryGoods.id << ")!" << LogStream::eos;
			continue;
		}
		
		MallItemInfo good = *pGoods;

		good.accountRestBuyNum = GetAccountRestBuyNum(player->GetAccId(), pGoods);

		// 私人订制
		//if (protocol.isPersonalTailor > 0 && good.personalTailorActivateCond > 0)
		//{
		//	CondActivateInfo* tailor = player->GetCondActivateMgr().FindOpenCondActivateInfoByCond((UInt8)CAT_MALL_PERSONAL_TAILOR, good.personalTailorActivateCond);
		//	if (!tailor)
		//	{
		//		// 共用触发通过商品id找
		//		tailor = player->GetCondActivateMgr().FindOpenCondActivateInfoByGoodsId((UInt8)CAT_MALL_PERSONAL_TAILOR, good.id);
		//	}

		//	if (!tailor)
		//	{
		//		continue;
		//	}

		//	// 处理时装
		//	if (good.jobType != 0 && good.jobType != player->GetOccu())
		//	{
		//		continue;
		//	}

		//	good.startTime = tailor->GetActivateStartTime();
		//	good.endTime = tailor->GetActivateEndTime();
		//}
		//else
		//{
		//	good.personalTailorActivateCond = 0;
		//}

		if (actSystemData != NULL)
		{
			//if (std::find(actSystemData->mallItemList.begin(), actSystemData->mallItemList.end(), good.id) != actSystemData->mallItemList.end())
			//{
			//	// 老兵回归
			//	if (!player->IsVeteranReturn())
			//	{
			//		continue;
			//	}
			//	else
			//	{
			//		Int32 ret = TimeUtil::CalculateEndPointByStartPointAndInterval(player->GetRoleVeteranReturnTime(), good.openInterval, good.startTime);
			//		if (ret != 0)
			//		{
			//			ErrorLogStream << PLAYERINFO(player) << " calculte time failed, open interval=" << LogStream::eos;
			//			continue;
			//		}
			//		ret = TimeUtil::CalculateEndPointByStartPointAndInterval(player->GetRoleVeteranReturnTime(), good.closeInterval, good.endTime);
			//		if (ret != 0)
			//		{
			//			ErrorLogStream << PLAYERINFO(player) << " calculte time failed, close interval=" << LogStream::eos;
			//			continue;
			//		}

			//		if ((curTime < good.startTime) || (curTime > good.endTime))
			//		{
			//			continue;
			//		}
			//	}
			//}
		}

		// 限时礼包
		if (good.goodsType == (UInt8)MGT_GIFT_ACTIVATE_ONCE || good.goodsType == (UInt8)MGT_GIFT_ACTIVATE_THREE_TIMES)
		{
			MallGiftPack* giftPack = player->GetMallGiftPackMgr().GetMallGiftPack(good.id);
			if (!giftPack)
			{
				continue;
			}

			if (giftPack->GetActivateState() == (UInt8)MGPAT_CLOSED)
			{
				continue;
			}

			if (giftPack->IsExpired(curTime))
			{
				giftPack->SetActivateState(MGPAT_CLOSED);
				giftPack->UpdateToDB(player);
				InfoLogStream << PLAYERINFO(player) << " mall time limit gift pack(" << giftPack->GetGiftPackID() << ") is expired!" << LogStream::eos;

				continue;
			}

			good.startTime = curTime;
			good.endTime = giftPack->GetActivateEndTime();

			switch ((ItemSubType)good.moneytype)
			{
			case ST_ITEM_INVALID: // 人民币
				UpdateGiftGood(good, player);
				break;

			case ST_ITEM_POINT: // 点券
				good.limitnum = giftPack->GetRestNum();
				break;

			default:
				ErrorLogStream << "unknown moneytype(" << good.moneytype << ")!" << LogStream::eos;
				break;
			}
		}

		// 每日刷新礼包(人民币礼包)
		if (good.goodsType == (UInt8)MGT_GIFT_DAILY_REFRESH)
		{
			UpdateGiftGood(good, player);
		}

		// 打折
		if (good.type == (UInt8)MT_FASHION && good.discountRate > 0)
		{
			good.discountRate = queryGoods.discountRate;
			if (good.discountRate > 0)
			{
				good.discountprice = (UInt32)(good.discountprice * GenDiscountRate(queryGoods.discountRate));
			}
		}

		// 自定义商品
		auto customerMallItem = FindCustomerMallItem(player->GetID(), good.id);
		if (customerMallItem != NULL)
		{
			if (!customerMallItem->IsOnSale(curTime))
			{
				continue;
			}
			customerMallItem->TransformMallItem(good);
		}
		
		outGoodsInfos.push_back(good);
	}
}

MallItemInfo* MallMgr::GetMallItemInfo(UInt32 uid)
{
	auto good = m_allItems.find(uid);
	if (good == m_allItems.end())
	{
		return NULL;
	}

	return good->second;
}

void MallMgr::QueryAllItems(VecItem& items)
{
	auto itr = m_idxItems.begin();
	for (; itr != m_idxItems.end(); ++itr)
	{
		VecItem& tmpItems = itr->second;
		for (size_t i = 0; i < tmpItems.size(); ++i)
		{
			items.push_back(tmpItems[i]);
		}
	}
}

bool MallMgr::DecodeRewardItems(std::vector<ItemReward>& items, std::string strItems)
{
	if (strItems.empty())
	{
		return true;
	}


	std::vector<std::string> rewardsStr;
	std::vector<std::string> paramStr;
	Avalon::StringUtil::Split(strItems, rewardsStr, "|");
	for (std::vector<std::string>::iterator iter = rewardsStr.begin();
		iter != rewardsStr.end(); ++iter)
	{
		Avalon::StringUtil::Split(*iter, paramStr, ":");
		if (paramStr.size() != 2)
			return false;

		ItemReward reward;
		reward.id = Avalon::StringUtil::ConvertToValue<UInt32>(paramStr[0].c_str());
		reward.num = Avalon::StringUtil::ConvertToValue<UInt32>(paramStr[1].c_str());

		items.push_back(reward);
	}

	return true;
}

std::string MallMgr::EncodRewardItems(std::vector<ItemReward>& items)
{
	std::ostringstream os;
	
	for (size_t i = 0; i < items.size(); ++i)
	{
		if (i > 0)
		{
			os << "|";
		}

		os << items[i].id << ":";
		os << items[i].num;
	}

	return os.str();
}

UInt32 MallMgr::TransUidGMToLocal(UInt32 gmUid)
{
	if (gmUid >= GM_GEN_UID)
	{
		return 0;
	}

	return (GM_GEN_UID + gmUid);
}

void MallMgr::GmSetGoods(MallItemInfo* good)
{
	if (!good)
	{
		return;
	}

	auto oldGood = GetMallItemInfo(good->id);
	if (oldGood != NULL)
	{
		*oldGood = *good;

		InfoLogStream << "Update " << good->LogStr() << LogStream::eos;
		return;
	}

	m_allItems[good->id] = good;
	m_idxItems[ItemKey(good->type, good->subType, good->jobType)].push_back(good);

	InfoLogStream << "Add " << good->LogStr() << LogStream::eos;
}

bool MallMgr::GmDelGoods(UInt32 goodsId)
{
	auto mallItem = GetMallItemInfo(goodsId);
	if (!mallItem)
	{
		return false;
	}

	mallItem->endTime = mallItem->startTime + 1;
	InfoLogStream << "Delete " << mallItem->LogStr() << "!" << LogStream::eos;
	return true;
}

void MallMgr::UpdateItem(VecItem& tmpItems, MallItemInfo* good)
{
	for (size_t i = 0; i < tmpItems.size(); ++i)
	{
		MallItemInfo* tmpGood = tmpItems[i];
		if (!tmpGood)
		{
			continue;
		}

		if (tmpGood->id == good->id)
		{
			tmpItems[i] = good;
			m_allItems[good->id] = good;

			UpdateDB(good, tmpGood);
			CL_SAFE_DELETE(tmpGood);
			return;
		}
	}
}

void MallMgr::DeleteItem(VecItem& tmpItems, UInt32 goodsId)
{
	for (size_t i = 0; i < tmpItems.size(); ++i)
	{
		MallItemInfo* tmpInfo = tmpItems[i];
		if (!tmpInfo)
		{
			continue;
		}

		if (tmpInfo->id == goodsId)
		{
			tmpItems.erase(tmpItems.begin() + i);
			break;
		}
	}
}

void MallMgr::InsertDB(MallItemInfo* good)
{
	//if (!good)
	//{
	//	return;
	//}

	//CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_mall", good->id);

	//cmd->PutData("type", good->type);
	//cmd->PutData("subtype", good->subType);
	//cmd->PutData("jobtype", good->jobType);
	//cmd->PutData("itemid", good->itemid);
	//cmd->PutData("groupnum", good->itemnum);

	//cmd->PutData("price", good->price);
	//cmd->PutData("disprice", good->discountprice);
	//cmd->PutData("moneytype", good->moneytype);
	//cmd->PutData("limittype", good->bLimit);
	//cmd->PutData("limitnum", good->limitnum);
	//cmd->PutData("giftpackitems", EncodRewardItems(good->rewards));
	//cmd->PutData("giftpackicon", good->icon);
	//cmd->PutData("limtstarttime", good->startTime);
	//cmd->PutData("limitendtime", good->endTime);
	//cmd->PutData("limitetotlenum", good->limitTotalNum);
	//cmd->PutData("vipscore", good->vipScore);
	//cmd->PutData("giftpackname", good->giftName);
	//cmd->PutData("tagtype", good->tagType);
	//cmd->PutData("sort", good->sortIdx);
	//cmd->PutData("hotsort", good->hotSortIdx);
	//cmd->PutData("goods_type", good->goodsType);
	//cmd->PutData("goods_subtype", good->goodsSubType);
	//cmd->PutData("is_recommend", good->isRecommend);

	//CLRecordClient::Instance()->SendCommand(cmd);
}

void MallMgr::DeleteDB(MallItemInfo* good)
{
	//if (!good)
	//{
	//	return;
	//}

	//CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_mall", good->id);
	//CLRecordClient::Instance()->SendCommand(cmd);
}

void MallMgr::UpdateDB(MallItemInfo* newGood, MallItemInfo* oldGood)
{
	//if (!newGood || !oldGood)
	//{
	//	return;
	//}

	//CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_mall", newGood->id, true);
	//if (newGood->type != oldGood->type)						cmd->PutData("type", newGood->type);
	//if (newGood->subType != oldGood->subType)				cmd->PutData("subtype", newGood->subType);
	//if (newGood->jobType != oldGood->jobType)				cmd->PutData("jobtype", newGood->jobType);
	//if (newGood->itemid != oldGood->itemid)					cmd->PutData("itemid", newGood->itemid);
	//if (newGood->itemnum != oldGood->itemnum)				cmd->PutData("groupnum", newGood->itemnum);
	//if (newGood->price != oldGood->price)					cmd->PutData("price", newGood->price);
	//if (newGood->discountprice != oldGood->discountprice)	cmd->PutData("disprice", newGood->discountprice);
	//if (newGood->moneytype != oldGood->moneytype)			cmd->PutData("moneytype", newGood->moneytype);
	//if (newGood->bLimit != oldGood->bLimit)					cmd->PutData("limittype", newGood->bLimit);
	//if (newGood->limitnum != oldGood->limitnum)				cmd->PutData("limitnum", newGood->limitnum);
	//cmd->PutData("giftpackitems", EncodRewardItems(newGood->rewards));
	//if (newGood->icon != oldGood->icon)						cmd->PutData("giftpackicon", newGood->icon);
	//if (newGood->startTime != oldGood->startTime)			cmd->PutData("limtstarttime", newGood->startTime);
	//if (newGood->endTime != oldGood->endTime)				cmd->PutData("limitendtime", newGood->endTime);
	//if (newGood->limitTotalNum != oldGood->limitTotalNum)	cmd->PutData("limitetotlenum", newGood->limitTotalNum);
	//if (newGood->vipScore != oldGood->vipScore)				cmd->PutData("vipscore", newGood->vipScore);
	//if (newGood->giftName != oldGood->giftName)				cmd->PutData("giftpackname", newGood->giftName);
	//if (newGood->tagType != oldGood->tagType)				cmd->PutData("tagtype", newGood->tagType);
	//if (newGood->sortIdx != oldGood->sortIdx)				cmd->PutData("sort", newGood->sortIdx);
	//if (newGood->hotSortIdx != oldGood->hotSortIdx)			cmd->PutData("hotsort", newGood->hotSortIdx);
	//if (newGood->goodsType != oldGood->goodsType)			cmd->PutData("goods_type", newGood->goodsType);
	//if (newGood->goodsSubType != oldGood->goodsSubType)		cmd->PutData("goods_subtype", newGood->goodsSubType);
	//if (newGood->isRecommend != oldGood->isRecommend)		cmd->PutData("is_recommend", newGood->isRecommend);

	//CLRecordClient::Instance()->SendCommand(cmd);
}

UInt32 MallMgr::TransUidLocalToGm(UInt32 uid)
{
	if (uid < GM_GEN_UID)
	{
		return uid;
	}

	return (uid - GM_GEN_UID);
}

void MallMgr::GetHotGoods(UInt8 moneyType, std::vector<CLProtocol::QueryGood>& queryGoods)
{
	UInt32 curTime = (UInt32)CURRENT_TIME.Sec();

	auto itr = m_allItems.begin();
	for (; itr != m_allItems.end(); ++itr)
	{
		MallItemInfo* good = itr->second;
		if (!good)
		{
			ErrorLogStream << "MallItemInfo ptr is null!" << LogStream::eos;
			continue;
		}

		if (good->tagType == TT_HOT && moneyType == good->moneytype)
		{
			if ((good->startTime > 0 && curTime < good->startTime) ||
				(good->endTime > 0 && curTime > good->endTime))
			{
				continue;
			}

			CLProtocol::QueryGood qg;
			qg.bLimit = good->bLimit;
			qg.id = good->id;
			qg.discountRate = GenDiscountRate(good, curTime);
			queryGoods.push_back(qg);
		}
	}
}

void MallMgr::GetPersonalTailorGoods(std::vector<CLProtocol::QueryGood>& queryGoods)
{
	UInt32 curTime = (UInt32)Avalon::Time::CurrentTime().Sec();

	for (auto elem : m_allItems)
	{
		MallItemInfo* goods = elem.second;
		if (!goods)
		{
			ErrorLogStream << "MallItemInfo ptr is null!" << LogStream::eos;
			continue;
		}

		if (goods->personalTailorActivateCond > 0)
		{
			CLProtocol::QueryGood qg;
			qg.bLimit = goods->bLimit;
			qg.id = goods->id;
			qg.discountRate = GenDiscountRate(goods, curTime);
			queryGoods.push_back(qg);
		}
	}
}

void MallMgr::UpdateGiftGood(MallItemInfo& good, WSPlayer* player)
{
	// 人民币礼包数量
	if (good.limitnum > 0)
	{
		auto todayNum = player->GetTodayChargeTimes(CHARGE_MALL_PACKET, good.id, player->GetID());
		if (todayNum >= (UInt32)good.limitnum)
		{
			good.limitnum = 0;
		}
		else
		{
			good.limitnum -= (UInt16)todayNum;
		}
	}
	else
	{
		good.limitnum = (UInt16)-1;
	}

	if (good.limitTotalNum > 0)
	{
		auto totalNum = player->GetChargeGoodsTimes(CHARGE_MALL_PACKET, good.id, player->GetID());
		if (totalNum >= (UInt32)good.limitTotalNum)
		{
			good.limitTotalNum = 0;
		}
		else
		{
			good.limitTotalNum -= (UInt16)totalNum;
		}
		SetMallGiftPackNum(player, good.id, good.limitTotalNum);
	}
	else
	{
		good.limitTotalNum = (UInt16)-1;
	}
}

UInt32 MallMgr::BatchBuy(WSPlayer* player, std::vector<ItemReward>& items)
{
	MallBatchBuyCtx& ctx = m_PlayersBatchBuyCtx[player->GetID()];
	ctx.m_itemUids.clear();
	ctx.m_totleNum = items.size();

	for (size_t i = 0; i < items.size(); ++i)
	{
		Buy(player, items[i].id, items[i].num);
	}

	return ErrorCode::SUCCESS;
}

bool MallMgr::OnSaleMallWaitGoods(UInt32 goodsId)
{
	auto itemsIter = m_allItems.find(goodsId);
	if (itemsIter != m_allItems.end())
	{
		ErrorLogStream << "Can not add the same mall goods(" << goodsId << ") again!" << LogStream::eos;
		return false;
	}

	auto goodsIter = m_waitGoods.find(goodsId);
	if (goodsIter == m_waitGoods.end())
	{
		ErrorLogStream << "Can not find the mall wait goods(" << goodsId << ")!" << LogStream::eos;
		return false;
	}

	MallItemInfo* itemInfo = goodsIter->second;
	if (!itemInfo)
	{
		ErrorLogStream << "Failed to onSale the mall goods(" << goodsId << "), MallItemInfoPtr is null!" << LogStream::eos;
		return false;
	}

	if (!AddMallItem(itemInfo))
	{
		return false;
	}

	InfoLogStream << "On sale mall goods(" << goodsId << ")!" << LogStream::eos;
	return true;
}

bool MallMgr::OffSaleMallWaitGoods(UInt32 goodsId)
{
	auto goodsIter = m_waitGoods.find(goodsId);
	if (goodsIter == m_waitGoods.end())
	{
		ErrorLogStream << "Can not find the mall wait goods(" << goodsId << ")!" << LogStream::eos;
		return false;
	}

	auto itemsIter = m_allItems.find(goodsId);
	if (itemsIter == m_allItems.end())
	{
		ErrorLogStream << "Can not find the mall goods(" << goodsId << ")!" << LogStream::eos;
		return false;
	}

	MallItemInfo* itemInfo = itemsIter->second;
	if (!itemInfo)
	{
		ErrorLogStream << "Failed to offsale the mall goods(" << goodsId << "), MallItemInfoPtr is null!" << LogStream::eos;
		return false;
	}

	ItemKey key(itemInfo->type, itemInfo->subType, itemInfo->jobType);
	auto idxIter = m_idxItems.find(key);
	if (idxIter == m_idxItems.end())
	{
		return false;
	}

	VecItem& tmpItems = idxIter->second;
	DeleteItem(tmpItems, itemInfo->id);
	m_allItems.erase(itemsIter);

	InfoLogStream << "Off sale mall goods(" << goodsId << ")!" << LogStream::eos;
	return true;
}

void MallMgr::SyncMallGiftPackActivityState(WSPlayer *pPlayer)
{
	if (!pPlayer)
	{
		ErrorLogStream << "WSPlayer ptr is null!" << LogStream::eos;
		return;
	}

	CLProtocol::SyncWorldMallGiftPackActivityState protocol;
	protocol.m_State = m_GiftActivityState;
	pPlayer->SendProtocol(protocol);
}

UInt32 MallMgr::GetTimeOnZero(Avalon::Time& time)
{
	Avalon::Date date = Avalon::Date(time);
	date.Hour(0);
	date.Min(0);
	date.Sec(0);

	return (UInt32)date.ToTime().Sec();
}