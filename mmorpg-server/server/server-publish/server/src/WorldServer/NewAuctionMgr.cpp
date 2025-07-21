#include "NewAuctionMgr.h"

#include <CLRecordClient.h>
#include <CLAuctionProtocol.h>

#include "WSPlayer.h"
#include "WSPlayerMgr.h"
#include "WSItemTransaction.h"
#include "MailBox.h"
#include "WSSelectInitDataCallback.h"
#include "WorldSysNotify.h"
#include "WSPushNotify.h"
#include <CLSystemValueDataEntry.h>
#include "WSServiceSwitchMgr.h"
#include "AuctionTransRecord.h"
#include "CLItemProperty.h"
#include "CLMsgParser.h"
#include "CLAuctionPageStruct.h"
#include "AuctionAttentRecord.h"
#include "GameParamMgr.h"
#include <CLAuctionRecoveItemDataEntry.h>
#include <CLAuctionNoTreasAverPriceMinNumDataEntry.h>
#include <CLAuctionMagicStrengAdditDataEntry.h>
#include <CLAuctionMagicExpectUpDataEntry.h>

namespace zjy_auction {

UInt8 AuctionInfo::GetEnhanceType() const
{
	return item ? item->GetEnhancetype() : enhanceType;
}

AuctionMgr::AuctionMgr()
{
}

AuctionMgr::~AuctionMgr()
{

}

/**
*@brief 加载拍卖信息回调
*/
class SelectAuctionCallback : public SelectInitDataCallback
{
public:
    bool OnLoadFinished()
    {
        return AuctionMgr::Instance()->OnSelectAuctionRet(this);
    }
};

/**
*@brief 加载拍卖信息回调
*/
class SelectAuctionRecomendPriceCallback : public SelectInitDataCallback
{
public:
    bool OnLoadFinished()
    {
        return AuctionMgr::Instance()->OnSelectAuctionRecommendPriceRet(this);
    }
};

void AuctionMgr::Init()
{
	CLSelectCommand	*pRecommendCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_auction_recommend_price");
	CLRecordClient::Instance()->SendCommand(pRecommendCmd, new SelectAuctionRecomendPriceCallback());

    CLSelectCommand	*pCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_auction_new");
    CLRecordClient::Instance()->SendCommand(pCmd, new SelectAuctionCallback());

    m_checkTimer.SetInterval(60 * 1000);
	m_checkTreasTimer.SetInterval(1000);
	m_checkRecoveNoTreasTimer.SetInterval(5 * 60 * 1000);
	m_checkExpireAuctionTimer.SetInterval(10 * 60 * 1000);
	m_checkNoTreasTimer.SetInterval(3000);
	m_clacMagicMinAverPriceTimer.SetInterval(1 * 60 * 1000);

    m_priceTimer.SetLastTime(Avalon::Time::CurrentTime().MSec());
    //m_priceTimer.SetInterval(4 * Avalon::Time::SECS_OF_HOUR * Avalon::Time::MSECS_OF_SEC);
    m_priceTimer.SetInterval(1 * Avalon::Time::SECS_OF_MIN * Avalon::Time::MSECS_OF_SEC);
}

/**
*@brief 加载超时的拍卖物品信息回调
*/
class SelectTimeOutAuctionCallback : public CLRecordCallback
{
public:
	void OnFinished()
    {
        AuctionMgr::Instance()->OnSelectTimeoutAuctionRet(this);
    }
};

/**
*@brief 加载扫货非珍品拍卖物品信息回调
*/
class SelectCheckRecovedNoTreasAuctionCallback : public CLRecordCallback
{
public:
	void OnFinished()
	{
		AuctionMgr::Instance()->OnSelectCheckRecovedNoTreasAuctionRet(this);
	}
};

class SelectTimeOutOffsaleAuctionCallback : public CLRecordCallback
{
public:
	void OnFinished()
	{
		AuctionMgr::Instance()->OnSelectTimeoutOffsaleAuctionRet(this);
	}
};

void AuctionMgr::OnTick(const Avalon::Time &now)
{
	if (m_checkTreasTimer.IsTimeout(now))
	{
		this->UpdateTreasState(now);
	}

	if (m_checkNoTreasTimer.IsTimeout(now))
	{
		OnTickNoTreasAuctions();
	}

	if (m_checkRecoveNoTreasTimer.IsTimeout(now))
	{
		CLSelectCommand	*pCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_auction_new");
		pCmd->PutCond("recove_handled", 0);
		pCmd->PutCond("on_public_time", 0);
		pCmd->PutCond("on_sale_time", now.Sec(), "<=");
		pCmd->PutCond("duetime", now.Sec(), ">");
		CLRecordClient::Instance()->SendCommand(pCmd, new SelectCheckRecovedNoTreasAuctionCallback());
	}

    if (m_checkTimer.IsTimeout(now))
    {
        CLSelectCommand	*pCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_auction_new");
        pCmd->PutCond("duetime", now.Sec(), "<=");
		//pCmd->PutCond("on_public_time", 0, ">");
		pCmd->PutCond("offsale_handled",0);
        CLRecordClient::Instance()->SendCommand(pCmd, new SelectTimeOutAuctionCallback());
    }

    if (m_priceTimer.IsTimeout(now))
    {
        _UpdateRecommendPrice();
    }

	if (m_checkExpireAuctionTimer.IsTimeout(now))
	{
		CLSelectCommand	*pCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_auction_new");
		UInt32 expireDay = WSApplication::Instance()->GetAuctionDueOffSaleTime();
		expireDay = (expireDay == 0) ? 14 : expireDay;
		pCmd->PutCond("duetime", now.Sec() - expireDay * 24 * 3600, "<=");
		CLRecordClient::Instance()->SendCommand(pCmd, new SelectTimeOutOffsaleAuctionCallback());
	}

	if (m_clacMagicMinAverPriceTimer.IsTimeout(now))
	{
		_CalcMagicMinAverPriceByQuality();
	}
}

bool AuctionMgr::OnPlayerAddItemToAuction(WSPlayer* player, AuctionType type, ObjID_t id,
	UInt32 typeId, UInt32 num, UInt32 price, AuctionSellDuration duration, UInt8 strength, UInt32 buffId, UInt8 enhanceType)
{
    if (!player)
    {
        return false;
    }

    DebugLogStream << "player(" << player->GetID() << ") request add item to auction type(" << (UInt8)type << ") itemId(" << id
        << ") num(" << num << ") price(" << price << "duration(" << duration << ")." << LogStream::eos;

    if (type >= AT_NUM)
    {
        InfoLogStream << "player(" << player->GetID() << ") invalid auction type(" << (UInt8)type << ")." << LogStream::eos;
        return false;
    }

    if (type == AT_ITEM && player->GetAuctionNum(type) >= _GetAuctionMaxItemPerPlayer(type, player))
    {
        InfoLogStream << "player(" << player->GetID() << ") auction item num is full." << LogStream::eos;
		player->SendNotify(1088);
        return false;
    }

	if (num == 0)
	{
		ErrorLogStream << "player(" << player->GetID() << ") invalid auction item num(" << num << ")." << LogStream::eos;
		return false;
	}

    MoneyType pricetype = GetMoneyTypeByAuctionType(type);
    if (pricetype == MONEY_INVALID)
    {
        InfoLogStream << "player(" << player->GetID() << ") get invalid price type by auction type(" << (UInt8)type << ")." << LogStream::eos;
        return false;
    }

    if (duration >= ASD_NUM)
    {
        InfoLogStream << "player(" << player->GetID() << ") invalid duration(" << (UInt8)duration << ")." << LogStream::eos;
        return false;
    }

    if (type == AT_GOLD)
    {
        DebugLogStream << "player(" << player->GetID() << ") want to sell gold num(" << num << ")." << LogStream::eos;

        // 如果是金币拍卖行，需要检测要卖的物品是否有效
        id = (ObjID_t)GetGoldItemByNum(num);
        if (id == 0)
        {
            ErrorLogStream << "player(" << player->GetID() << ") want to sell invalid gold num(" << num << ")." << LogStream::eos;
            return false;
        }
		typeId = (UInt32)id;
    }

	if (!_IsValidPrice(typeId, strength, buffId, enhanceType, price / num))
	{
		InfoLogStream << "player(" << player->GetID() << ") sell auction item(" << typeId << ") invalid price(" << price / num << ")." << LogStream::eos;
		player->SendNotify(1101);
		return false;
	}

    CLProtocol::WorldAuctionCheck	protocol;
    protocol.playerId = player->GetID();
    protocol.type = (UInt8)type;
    protocol.id = id;
	protocol.typeId = typeId;
    protocol.num = num;
    protocol.price = price;
    protocol.pricetype = (UInt8)pricetype;
	protocol.deposit = GetDepositByAuctionPrice(price);
    protocol.counterFee = _GetCounterFeeByAuctionType(player, type, price);
    protocol.duration = (UInt8)duration;
	protocol.strength = strength;
	protocol.beadbuffId = buffId;
	protocol.isAgain = 0;
	protocol.auctionGuid = 0;
    player->SendToScene(protocol);
    
    return true;
}    

bool AuctionMgr::OnPlayerAddItemToAuctionAgain(WSPlayer* player, AuctionType type, 
	ObjID_t auctionGuid, UInt32 num, UInt32 price, UInt8 strength, UInt32 buffId, UInt8 enhanceType)
{
	if (!player)
	{
		return false;
	}

	AuctionInfo* auctionInfo = _FindAuction(auctionGuid);
	if (!auctionInfo || !auctionInfo->item)
	{
		InfoLogStream << "player(" << player->GetID() << ") invalid auction guid(" << auctionGuid << ")." << LogStream::eos;
		return false;
	}

	UInt32 curTime = (UInt32)CURRENT_TIME.Sec();

	if (auctionInfo->duetime > curTime)
	{
		InfoLogStream << "player(" << player->GetID() << ") auction is not overdue guid(" << auctionGuid << ")." << LogStream::eos;
		return false;
	}

	if (auctionInfo->auctionAgaining)
	{
		InfoLogStream << "player(" << player->GetID() << ") auction item is auctionAgaining." << LogStream::eos;
		return false;
	}

	DebugLogStream << "player(" << player->GetID() << ") request again add item to auction guid(" << auctionGuid << ") itemId(" << auctionInfo->item->GetDataID()
		<< ") num(" << num << ") price(" << price << LogStream::eos;

	if (type >= AT_NUM)
	{
		InfoLogStream << "player(" << player->GetID() << ") invalid auction type(" << (UInt8)type << ")." << LogStream::eos;
		return false;
	}

	if (type == AT_ITEM && player->GetAuctionNum(type) > _GetAuctionMaxItemPerPlayer(type, player))
	{
		InfoLogStream << "player(" << player->GetID() << ") auction item num is full." << LogStream::eos;
		player->SendNotify(1088);
		return false;
	}

	if (num == 0 || auctionInfo->num == 0)
	{
		ErrorLogStream << "player(" << player->GetID() << ") invalid auction item num(" << num << ")." << " auctionInfo->num = " << auctionInfo->num << LogStream::eos;
		return false;
	}

	if (num != auctionInfo->num)
	{
		ErrorLogStream << "player(" << player->GetID() << ") invalid auction item num(" << num << ")." << " auctionInfo->num = " << auctionInfo->num << LogStream::eos;
		return false;
	}

	MoneyType pricetype = GetMoneyTypeByAuctionType(type);
	if (pricetype == MONEY_INVALID)
	{
		InfoLogStream << "player(" << player->GetID() << ") get invalid price type by auction type(" << (UInt8)type << ")." << LogStream::eos;
		return false;
	}

	ItemDataEntry* itemData = ItemDataEntryMgr::Instance()->FindEntry(auctionInfo->itemTypeId);

	if (!itemData)
	{
		ErrorLogStream << "player(" << auctionInfo->owner << ") want again add auction(" << auctionGuid << ", item(" << auctionInfo->itemTypeId
			<< ") to auction failed, can't find item's data entry." << LogStream::eos;
		return false;
	}

	if (type == AT_ITEM && !CheckTimeLimitAuctionOnsaleAgain(player, *auctionInfo))
	{
		return false;
	}

	UInt32 typeId = auctionInfo->itemTypeId;

	if (!_IsValidPrice(typeId, strength, buffId, enhanceType, price / num))
	{
		InfoLogStream << "player(" << player->GetID() << ") sell auction item(" << typeId << ") invalid price(" << price / num << ")." << LogStream::eos;
		player->SendNotify(1101);
		return false;
	}

	auctionInfo->auctionAgaining = true;

	CLProtocol::WorldAuctionCheck	protocol;
	protocol.playerId = player->GetID();
	protocol.type = (UInt8)type;
	protocol.id = auctionInfo->item->GetID();
	protocol.typeId = typeId;
	protocol.num = num;
	protocol.price = price;
	protocol.pricetype = (UInt8)pricetype;
	protocol.deposit = GetDepositByAuctionPrice(price);
	protocol.counterFee = _GetCounterFeeByAuctionType(player, type, price);
	protocol.duration = 0;
	protocol.strength = strength;
	protocol.beadbuffId = buffId;
	protocol.isAgain = 1;
	protocol.auctionGuid = auctionGuid;
	player->SendToScene(protocol);
	return true;
}

class QueryAuctionListCountCallback : public CLRecordCallback
{
public:
    QueryAuctionListCountCallback(ObjID_t roleId, const CLProtocol::AuctionQueryCondition& cond, const std::vector<ObjID_t>& auctionList) : roleId_(roleId), cond_(cond), auctionIds_(auctionList) {}
    virtual void OnFinished()
    {
        UInt32 count = 0;
        if (NextRow())
        {
            count = GetData("count(*)");
        }
        
        UInt16 maxPage = count / cond_.itemNumPerPage + (count % cond_.itemNumPerPage > 0 ? 1 : 0);
		maxPage = (maxPage == 0) ? 1 : maxPage;
        AuctionMgr::Instance()->OnQueryAuctionRet(roleId_, cond_.type, cond_.page, maxPage, auctionIds_, cond_.goodState, cond_.attent);
    }

    virtual void OnFailed(UInt32 errorcode)
    {
        ErrorLogStream << "player(" << roleId_ << ") query auction list count failed, errorcode(" << errorcode << ")." << LogStream::eos;
    }

    ObjID_t     roleId_;
    CLProtocol::AuctionQueryCondition cond_;
    std::vector<ObjID_t> auctionIds_;
};

class QueryAuctionListCallback : public CLRecordCallback
{
public:
    QueryAuctionListCallback(ObjID_t roleId, const CLProtocol::AuctionQueryCondition& cond) : roleId_(roleId), cond_(cond) {}
    virtual void OnFinished() 
    {
        std::vector<ObjID_t> auctionIds;
        while (NextRow())
        {
            ObjID_t guid = GetKey();;
            auctionIds.push_back(guid);
        }

		auto player = WSPlayerMgr::Instance()->FindNormalPlayer(roleId_);
		if (!player)
		{
			return;
		}

        CLSelectCommand* command = CLRecordClient::Instance()->PrepareSelectCommand("t_auction_new");
        command->PutColumn("count(*)");
		if (!AuctionMgr::GenAuctionQuerySqlCondition(player, command, cond_))
        {
            ErrorLogStream << "player(" << roleId_ << ") invalid auction count query." << LogStream::eos;
            delete command;
            return;
        }

        CLRecordClient::Instance()->SendCommand(command, new QueryAuctionListCountCallback(roleId_, cond_, auctionIds));
    }

    virtual void OnFailed(UInt32 errorcode) 
    {
        ErrorLogStream << "player(" << roleId_ << ") query auction list failed, errorcode(" << errorcode << ")." << LogStream::eos;
    }

    ObjID_t     roleId_;
    CLProtocol::AuctionQueryCondition cond_;
};

class QueryAuctionItemNumCallback : public CLRecordCallback
{
public:
	QueryAuctionItemNumCallback(ObjID_t roleId, const CLProtocol::AuctionQueryCondition& cond) : roleId_(roleId), cond_(cond) {}
	virtual void OnFinished()
	{
		CLProtocol::WorldAuctionItemNumRes res;
		res.goodState = cond_.goodState;
		while (NextRow())
		{
			UInt32 itemTypeId = GetData("itemid");
			UInt32 itemNum = GetData("cast(sum(num) as unsigned)");

			ItemDataEntry* itemdata = ItemDataEntryMgr::Instance()->FindEntry(itemTypeId);
			if (!itemdata)
			{
				continue;
			}
			if (!AuctionMgr::CheckAuctionOccus(itemdata, cond_.occus))
			{
				continue;
			}

			AuctionItemBaseInfo info;
			info.itemTypeId = itemTypeId;
			info.num = itemNum;
			
			if (itemdata->isTreasure && WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_AUCTION_TREAS))
			{
				info.isTreas = true;
			}
			res.items.push_back(info);
		}

		auto player = WSPlayerMgr::Instance()->FindNormalPlayer(roleId_);
		if (player)
		{
			player->SendProtocol(res);
		}

		AuctionMgr::Instance()->UpdateAuctionNumCache(cond_, res);
	}

	virtual void OnFailed(UInt32 errorcode)
	{
		ErrorLogStream << "player(" << roleId_ << ") query auction num failed, errorcode(" << errorcode << ")." << LogStream::eos;
	}

	ObjID_t     roleId_;
	CLProtocol::AuctionQueryCondition cond_;
};

bool AuctionMgr::OnPlayerQueryAuctionList(WSPlayer* player, const CLProtocol::AuctionQueryCondition& cond)
{
    DebugLogStream << "player(" << player->GetID() << ") request to query auction list type(" << cond.type << ") itemTypeId(" << cond.itemTypeID
                    << ") page(" << cond.page << ") sortType(" << cond.sortType << ")." << LogStream::eos;

    if (!player)
    {
        return false;
    }
	UInt64 prohibitTimeStamp = player->GetAuctionProhibitTimeStamp();
	UInt64 curTimeStamp = CURRENT_TIME.MSec();
	if (curTimeStamp < prohibitTimeStamp)
	{
		player->SendNotify(9952);
		return false;
	}
	UInt32 time_interval = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_QUERY_TIME_INTERVAL);
	if (time_interval == 0) time_interval = 200;
	player->SetAuctionProhibitTimeStamp(curTimeStamp + time_interval);

	if (cond.itemNumPerPage == 0)
	{
		return false;
	}

	if (cond.itemNumPerPage != SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_QUERY_PAGE_MAXNUM)) return false;

	//关注
	if (cond.attent == AuctionAttentType::ATT_ATTENT)
	{
		std::vector<UInt64> ids;
		AuctionAttentMgr::Instance()->GetAttentedAuctions(player->GetID(), ids);
		if (ids.size() == 0)
		{
			InfoLogStream << "player(" << player->GetID() << "), auction attent list query, attent list is empty" << LogStream::eos;
			return false;
		}
		UInt32 count = ids.size();
		UInt16 maxPage = count / cond.itemNumPerPage + (count % cond.itemNumPerPage > 0 ? 1 : 0);
		AuctionMgr::Instance()->OnQueryAuctionRet(player->GetID(), cond.type, cond.page, maxPage, ids, cond.goodState, cond.attent);
		return true;
	}
	else //非关注
	{
		if (cond.itemTypeID > 0)
		{
			ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(cond.itemTypeID);
			if (dataEntry == NULL) return false;
			//非珍品且非装备
			if (dataEntry->type != ITEM_EQUIP && dataEntry->isTreasure != 1 && WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_AUCTION_PAGE))
			{
				if (cond.sortType != AST_PRICE_ASC)	return false;
				if (cond.page != 1) return false;
			}
		}
		else
		{
			ErrorLogStream << "player(" << player->GetID() << ") invalid auction list query. cond.itemTypeID = " << cond.itemTypeID << LogStream::eos;
			return false;
		}
	}

	if (WSApplication::Instance()->IsAuctionOpUseMem())
	{
		QueryAuctionItemListFromMem(player, cond);
	}
	else
	{
		CLSelectCommand* command = CLRecordClient::Instance()->PrepareSelectCommand("t_auction_new");
		if (!GenAuctionQuerySqlCondition(player, command, cond))
		{
			ErrorLogStream << "player(" << player->GetID() << ") invalid auction list query." << LogStream::eos;
			delete command;
			return false;
		}

		if (cond.attent != AuctionAttentType::ATT_ATTENT)
		{
			std::stringstream ss;
			UInt32 limitStart = cond.page <= 1 ? 0 : ((cond.page - 1) * cond.itemNumPerPage);
			ss << "limit " << limitStart << ", " << (UInt32)cond.itemNumPerPage;
			command->SetLimit(ss.str());
		}

		CLRecordClient::Instance()->SendCommand(command, new QueryAuctionListCallback(player->GetID(), cond));
	}

    return true;
}

bool AuctionMgr::OnPlayerQueryAuctionItemNum(WSPlayer* player, const CLProtocol::AuctionQueryCondition& cond)
{
	DebugLogStream << "player(" << player->GetID() << ") request to query auction item num type(" << cond.type << ") itemTypeId(" << cond.itemTypeID
		<< ") page(" << cond.page << ") sortType(" << cond.sortType << ")." << LogStream::eos;

	if (!player)
	{
		return false;
	}

	UInt64 prohibitTimeStamp = player->GetAuctionProhibitTimeStamp();
	UInt64 curTimeStamp = CURRENT_TIME.MSec();
	if (curTimeStamp < prohibitTimeStamp)
	{
		player->SendNotify(9952);
		return false;
	}
	UInt32 time_interval = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_QUERY_TIME_INTERVAL);
	if (time_interval == 0) time_interval = 200;
	player->SetAuctionProhibitTimeStamp(curTimeStamp + time_interval);

	if (WSApplication::Instance()->IsUseAuctionNumCache())
	{
		AuctionNumCache* auctionNumCache = GetAuctionNumCache(cond);
		if (auctionNumCache && auctionNumCache->expireTime >= (UInt32)CURRENT_TIME.Sec())
		{
			player->SendProtocol(auctionNumCache->res);
			std::ostringstream logss;
			logss << "player(" << player->GetID() << ") query auction item num, use cache. cache expireTime : " << auctionNumCache->expireTime
				<< ", queryCond goodState : " << (UInt32)auctionNumCache->cond.goodState << ", item_sub_type : ";
			for (auto itemSubType : auctionNumCache->cond.itemSubTypes)
			{
				logss << "[" << itemSubType << "]";
			}
			if (auctionNumCache->cond.excludeItemSubTypes.size() > 0)
			{
				logss << ", exclude_item_sub_type : ";
				for (auto itemSubType : auctionNumCache->cond.excludeItemSubTypes)
				{
					logss << " [" << itemSubType << "] ";
				}
			}
			
			logss << ", quality : " << (UInt32)auctionNumCache->cond.quality << ", minLevel : " << (UInt32)auctionNumCache->cond.minLevel << ", maxLevel : " << (UInt32)auctionNumCache->cond.maxLevel
				<< ", couponStrengthToLev : " << auctionNumCache->cond.couponStrengthToLev;
			if (auctionNumCache->cond.occus.size() > 0)
			{
				logss << ", occus : ";
				for (auto occu : auctionNumCache->cond.occus)
				{
					logss << " [" << (UInt32)occu << "] ";
				}
			}

			InfoLogStream << logss.str() << LogStream::eos;
			return true;
		}
	}

	if (WSApplication::Instance()->IsAuctionOpUseMem())
	{
		QueryAuctionItemNumFromMem(player, cond);
	}
	else
	{
		CLSelectCommand* command = CLRecordClient::Instance()->PrepareSelectCommand("t_auction_new");
		if (!GenAuctionItemNumSql(player, command, cond))
		{
			ErrorLogStream << "player(" << player->GetID() << ") invalid auction num query." << LogStream::eos;
			delete command;
			return false;
		}

		CLRecordClient::Instance()->SendCommand(command, new QueryAuctionItemNumCallback(player->GetID(), cond));
	}
	
	return true;
}

bool AuctionMgr::OnPlayerQueryAuctionItem(WSPlayer* player, ObjID_t itemGuid)
{
    if (!player)
    {
        return false;
    }

    DebugLogStream << "player(" << player->GetID() << ") request to query item(" << itemGuid << ")." << LogStream::eos;

    WSItem* item = _FindAuctionItem(itemGuid);
    if (!item)
    {
        DebugLogStream << "can't find item(" << itemGuid << ") in auction." << LogStream::eos;
        
        // 发送结果给客户端
        // ToDo...

        return false;
    }

    Avalon::Packet	*packet = Avalon::Packet::Create();
    packet->SetID(CLProtocol::WORLD_AUCTION_QUERY_ITEM_RET);
    Avalon::NetOutputStream	stream(packet->GetBuffer(), packet->GetMaxSize());
    stream & itemGuid & item->m_DataId;
    item->Encode(stream, SST_SELF);
    packet->SetSize(stream.GetPos());
    player->SendPacket(packet);
    Avalon::Packet::Destroy(packet);
    return true;
}

/**
*@brief 购买拍卖品事务
*/
class BuyAuctionTransaction : public ItemTransaction
{
public:
	BuyAuctionTransaction(WSPlayer *player, ObjID_t auctionId, UInt32 num, bool isRushBuy = false)
		: ItemTransaction(player), m_auctionId(auctionId), m_num(num), m_isRushBuy(isRushBuy){
		if (player)
		{
			m_buyPlayerId = player->GetID();
		}
	}
    ~BuyAuctionTransaction() {
		if (this->GetStatus() == ItemTransaction::STATUS_CHECK && m_isRushBuy)
		{
			AuctionMgr::Instance()->OnDeliverOneRushBuyResult(m_buyPlayerId, m_auctionId, m_num, false, ARBFT_TRANSOBJ_DES);
		}
	}

    void OnCheckSucced(WSPlayer *pPlayer)
    {
		bool succ = AuctionMgr::Instance()->OnBuyAuctionCheckRet(pPlayer, m_auctionId, m_num);
		if (succ)
        {
            RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_ACTION_BUY, m_auctionId));
        }
        else
        {
            UnLock();
			if (pPlayer)
			{
				pPlayer->SendNotify(9945);//抢购失败通知
			}
        }
		if (m_isRushBuy)
		{
			AuctionMgr::Instance()->OnDeliverOneRushBuyResult(m_buyPlayerId, m_auctionId, m_num, succ, succ ? 0 : ARBFT_DELIVER_FAIL);
		}
    }

    void OnCheckFailed(WSPlayer *pPlayer)
    {
		if (m_isRushBuy)
		{
			AuctionMgr::Instance()->OnDeliverOneRushBuyResult(m_buyPlayerId, m_auctionId, m_num, false, ARBFT_CHECK_FAIL);
		}
        UnLock();
    }

	void OnTimeOut(WSPlayer* player)
	{
		if (this->GetStatus() == ItemTransaction::STATUS_CHECK && m_isRushBuy)
		{
			AuctionMgr::Instance()->OnDeliverOneRushBuyResult(m_buyPlayerId, m_auctionId, m_num, false, ARBFT_OVERTIME);
			this->SetStatus(ItemTransaction::STATUS_FINAL);
		}
	}

private:
    // 拍卖品id
	ObjID_t		m_buyPlayerId;
    ObjID_t		m_auctionId;
	UInt32		m_num;
	bool		m_isRushBuy;
};

bool AuctionMgr::OnPlayerBuyAuctionItem(WSPlayer* player, ObjID_t guid, UInt32 num, bool rushBuy)
{
	if (!player)
	{
		return false;
	};

	DebugLogStream << "player(" << player->GetID() << ") request to buy auction(" << guid << ") item." << LogStream::eos;

	auto auctionInfo = _FindAuction(guid);
	if (!auctionInfo || auctionInfo->duetime <= UInt32(CURRENT_TIME.Sec()))
	{
		DebugLogStream << "can't find auction(" << guid << ") item." << LogStream::eos;
		player->SendNotify(1039);
		return false;
	}

	if (!player->CheckCanBuyAuction(guid))
	{
		DebugLogStream << "CheckCanBuyAuction fail can't buy auction(" << guid << ") item." << LogStream::eos;
		return false;
	}

	if (auctionInfo->owner == player->GetID())
	{
		DebugLogStream << "player(" << player->GetID() << ") want to buy self auction(" << guid << ") item." << LogStream::eos;
		player->SendNotify(1037);
		return false;
	}

	if (num == 0 || auctionInfo->num < num || auctionInfo->num == 0)
	{
		ErrorLogStream << "player(" << player->GetID() << ") want to buy auction(" << guid << ") item invalid num(" << num << ")." << LogStream::eos;
		return false;
	}

	if (auctionInfo->isTreas && WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_AUCTION_TREAS))
	{
		bool earse;
		bool recoved = false;
		UInt64 auction_id = auctionInfo->guid;
		Avalon::Time curr_time = CURRENT_TIME;
		if (!this->UpdateOneTreasState(auctionInfo, curr_time, earse, recoved, true))
		{
			InfoLogStream << "update treas update state fail guid=" << auction_id << LogStream::eos;
			player->SendNotify(9946);
			return false;
		}
		if (recoved)
		{
			InfoLogStream << "auction is recovery by sys when player buy  guid=" << auction_id << LogStream::eos;
			player->SendNotify(9946);
			return false;
		}
		if (auctionInfo->goodState != AGS_ONSALE)
		{
			ErrorLogStream << "normal buy auction id(" << auctionInfo->guid << "), goodState(" << auctionInfo->goodState << ") buyState(" << auctionInfo->buyState << ")."
				<< "onPublicTime(" << auctionInfo->onPublicTime << "), onSaleTime(" << auctionInfo->onSaleTime << "), curr_time(" << (UInt32)CURRENT_TIME.Sec() << ")." << LogStream::eos;
			player->SendNotify(9946);//该物品当前不能购买
			return false;
		}
		if (auctionInfo->buyState != AGBS_NORMAL_BUY && !rushBuy)
		{
			ErrorLogStream << "normal buy auction id(" << auctionInfo->guid << "), goodState(" << auctionInfo->goodState << ") buyState(" << auctionInfo->buyState << ")."
				<< "onPublicTime(" << auctionInfo->onPublicTime << "), onSaleTime(" << auctionInfo->onSaleTime << "), curr_time(" << (UInt32)CURRENT_TIME.Sec() << ")." << LogStream::eos;
			player->SendNotify(9946);//该物品当前不能购买
			return false;
		}
	}

	UInt32 unitPrice = auctionInfo->price / auctionInfo->num;
	UInt32 price = unitPrice * num;
	BuyAuctionTransaction	*pTransaction = new BuyAuctionTransaction(player, guid, num, rushBuy);
    if (MONEY_GOLD == auctionInfo->pricetype)
    {
        pTransaction->SetSilver(price);
    }
    else if (MONEY_POINT == auctionInfo->pricetype)
    {
        pTransaction->SetGold(price);
    }
    else
    {
        delete pTransaction;
        return false;
    }

    return player->BeginItemTransaction(pTransaction);
}

bool AuctionMgr::OnPlayerRusyBuyAuctionItem(WSPlayer* player, ObjID_t guid, UInt32 num)
{
	if (!player)
	{
		return false;
	};

	if (!WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_AUCTION_TREAS))
	{
		DebugLogStream << "auction trans is close, player(" << player->GetID() << ") request to rush buy auction(" << guid << ") item." << LogStream::eos;
		return false;
	}

	DebugLogStream << "player(" << player->GetID() << ") request to rush buy auction(" << guid << ") item." << LogStream::eos;

	auto auctionInfo = _FindAuction(guid);
	if (!auctionInfo)
	{
		DebugLogStream << "can't find auction(" << guid << ") item." << LogStream::eos;
		player->SendNotify(1039);
		return false;
	}

	if (!player->CheckCanBuyAuction(guid))
	{
		DebugLogStream << "CheckCanBuyAuction fail can't rush buy auction(" << guid << ") item." << LogStream::eos;
		return false;
	}

	if (auctionInfo->owner == player->GetID())
	{
		DebugLogStream << "player(" << player->GetID() << ") want to rush buy self auction(" << guid << ") item." << LogStream::eos;
		player->SendNotify(1037);
		return false;
	}

	if (num == 0 || auctionInfo->num < num || auctionInfo->num == 0)
	{
		ErrorLogStream << "player(" << player->GetID() << ") want to rush buy auction(" << guid << ") item invalid num(" << num << ")." << LogStream::eos;
		return false;
	}

	if (!auctionInfo->isTreas)
	{
		ErrorLogStream << "player(" << player->GetID() << ") want to rush buy not treas auction(" << guid << LogStream::eos;
		return false;
	}

	bool earse;
	bool recoved = false;
	UInt64 auction_id = auctionInfo->guid;
	Avalon::Time curr_time = CURRENT_TIME;
	if (!this->UpdateOneTreasState(auctionInfo, curr_time, earse, recoved, true))
	{
		InfoLogStream << "update treas update state fail guid=" << auction_id << LogStream::eos;
		player->SendNotify(9946);
		return false;
	}
	if (recoved)
	{
		InfoLogStream << "auction is recovery by sys when  player rusy buy  guid=" << auction_id << LogStream::eos;
		player->SendNotify(9946);
		return false;
	}
	if (auctionInfo->buyState != AGBS_RUSH_BUY)
	{
		ErrorLogStream << "player(" << player->GetID() << ") want to rush buy not treas auction(" << guid << LogStream::eos;
		player->SendNotify(9945); //抢购物品失败
		return false;
	}

	return this->AddOneRushBuyPlayer(auctionInfo, player, num);
}

bool AuctionMgr::OnPlayerCancelAuction(WSPlayer *player, ObjID_t id)
{
    if (!player)
    {
        return false;
    }

    auto auctionInfo = _FindAuction(id);
    if (!auctionInfo)
    {
        player->SendNotify(1041);
        InfoLogStream << "player(" << player->GetID() << ") want to cancel auction(" << id << ") but it's not exist." << LogStream::eos;
        return false;
    }

    if (player->GetID() != auctionInfo->owner)
    {
        ErrorLogStream << "player(" << player->GetID() << ") want to cancel auction(" << id << ") but it's owner is " << auctionInfo->owner << ")." << LogStream::eos;
        return false;
    }

	//检查包裹
	CLProtocol::SceneAuctionCheckPackageReq req;
	req.playerId = player->GetID();
	req.auctionGuid = id;
	req.items.push_back(ItemReward(auctionInfo->item->m_DataId, auctionInfo->item->m_Num, 
		auctionInfo->item->m_Bind, auctionInfo->item->m_Quality, 0, 0, auctionInfo->item->m_AuctionTransNum));

	player->SendToScene(req);
    return true;
}

bool AuctionMgr::OnSceneAuctionCheckPackageRet(WSPlayer *player, ObjID_t id)
{
	if (!player)
	{
		return false;
	}

	auto auctionInfo = _FindAuction(id);
	if (!auctionInfo)
	{
		player->SendNotify(1041);
		InfoLogStream << "player(" << player->GetID() << ") want to cancel auction(" << id << ") but it's not exist." << LogStream::eos;
		return false;
	}

	if (player->GetID() != auctionInfo->owner)
	{
		ErrorLogStream << "player(" << player->GetID() << ") want to cancel auction(" << id << ") but it's owner is " << auctionInfo->owner << ")." << LogStream::eos;
		return false;
	}

	player->SendNotify(1036);
	if (auctionInfo->duetime > UInt32(CURRENT_TIME.Sec()))
	{
		_SendAuctionToPlayer(auctionInfo, auctionInfo->num, player->GetID(), ASIT_CANCEL);
	}
	else
	{
		_SendAuctionToPlayer(auctionInfo, auctionInfo->num, player->GetID(), ASIT_TIMEOUT);
	}

	CLProtocol::WorldAuctionNotifyRefresh refresh;
	refresh.type = auctionInfo->type;
	refresh.reason = SRR_CANCEL;
	refresh.auctGuid = auctionInfo->guid;
	player->SendProtocol(refresh);

	return true;
}

class QueryPlayerAuctionListCallback : public CLRecordCallback
{
public:
    QueryPlayerAuctionListCallback(ObjID_t roleId) : roleId_(roleId) {}
    virtual void OnFinished()
    {
        std::vector<std::pair<UInt8, ObjID_t>> auctionList;
        while (NextRow())
        {
            ObjID_t guid = GetKey();;
            UInt8 type = GetData("type");
            auctionList.push_back(std::make_pair(type, guid));
        }

        AuctionMgr::Instance()->OnQueryPlayerAuctionListRet(roleId_, auctionList);
    }

    virtual void OnFailed(UInt32 errorcode)
    {
        ErrorLogStream << "player(" << roleId_ << ") query player auction list failed, errorcode(" << errorcode << ")." << LogStream::eos;
    }

    ObjID_t     roleId_;
};

bool AuctionMgr::OnPlayerLoadSelfAuctionList(WSPlayer *player)
{
    if (!player)
    {
        return false;
    }

    CLSelectCommand* command = CLRecordClient::Instance()->PrepareSelectCommand("t_auction_new");
    command->PutCond("owner", player->GetID());
    command->PutColumn("type");

    CLRecordClient::Instance()->SendCommand(command, new QueryPlayerAuctionListCallback(player->GetID()));
    return true;
}

bool AuctionMgr::OnPlayerQuerySelfAuctionList(WSPlayer *player, UInt8 type)
{
    if (!player)
    {
        return false;
    }

    DebugLogStream << "player(" << player->GetID() << ") query self auction(" << type << ") list." << LogStream::eos;

    CLProtocol::WorldAuctionSelfListRes sync;
    sync.type = type;

    for (auto id : player->GetAuctionList((AuctionType)type))
    {
        AuctionInfo* info = _FindAuction(id);
        if (!info)
        {
            ErrorLogStream << "can't find player(" << player->GetID() << ")'s auction(" << id << ")." << LogStream::eos;
            continue;
        }

        sync.data.push_back(*((AuctionBaseInfo*)info));
    }

    player->SendProtocol(sync);
    return true;
}

bool AuctionMgr::OnPlayerQueryRecommendPrice(WSPlayer *player, AuctionType type, UInt32 itemTypeId)
{
    if (!player)
    {
        return false;
    }

    DebugLogStream << "player(" << player->GetID() << ") query type(" << (UInt8)type << ") item(" << itemTypeId << ") recommend price." << LogStream::eos;

    CLProtocol::WorldAuctionRecommendPriceRes res;
    res.type = (UInt8)type;
    res.itemTypeId = itemTypeId;
    if (type == AT_GOLD)
    {
		//res.itemTypeId = GetGoldItemByNum(itemTypeId);
		res.price = _GetRecommendPrice(GetGoldItemByNum(itemTypeId), 0, 0);
    }
    else
    {
		
		res.price = _GetRecommendPrice(res.itemTypeId, 0, 0);
    }
    

    player->SendProtocol(res);
    return true;
}

class SelectAuctionSaleListCallback : public CLRecordCallback
{
public:
	SelectAuctionSaleListCallback(ObjID_t roleId_, AuctionType type_, UInt32 itemTypeId_, UInt32 strengthen_, UInt32  beadbuffid_, UInt8 enhanceType_)
		: roleId(roleId_), type(type_), itemTypeId(itemTypeId_), strengthen(strengthen_), beadbuffid(beadbuffid_), enhanceType(enhanceType_){}
	virtual void OnFinished()
	{
		std::vector<ObjID_t> auctionIds;
		while (NextRow())
		{
			ObjID_t guid = GetKey();;
			auctionIds.push_back(guid);
		}
		AuctionMgr::Instance()->OnQueryAuctionSaleLisRet(roleId, auctionIds, type, itemTypeId, strengthen, beadbuffid, enhanceType);
	}

	virtual void OnFailed(UInt32 errorcode)
	{
		ErrorLogStream << "player(" << roleId << ") query" << " itemTypeId(" << itemTypeId << ") strengthen(" << strengthen << ") auction price list count failed, errorcode(" << errorcode << ")." << LogStream::eos;
	}
	ObjID_t     roleId;
	AuctionType type;
	UInt32		itemTypeId;
	UInt32		strengthen;
	UInt32      beadbuffid;
	UInt8		enhanceType;
};

class SelectAuctionSaleListCallbackForExplaceHzEquip : public CLRecordCallback
{
public:
	SelectAuctionSaleListCallbackForExplaceHzEquip(ObjID_t roleId_, AuctionType type_, UInt32 itemTypeId_, UInt32 strengthen_, 
		UInt32  beadbuffid_, UInt8 enhanceType_, UInt8	goodState_, UInt8 opType_, UInt8 realEnhanceType_)
		: roleId(roleId_), type(type_), itemTypeId(itemTypeId_), strengthen(strengthen_), beadbuffid(beadbuffid_), 
		enhanceType(enhanceType_), goodState(goodState_), opType(opType_), realEnhanceType(realEnhanceType_){}
	virtual void OnFinished()
	{
		std::vector<ObjID_t> auctionIds;
		while (NextRow())
		{
			ObjID_t guid = GetKey();;
			auctionIds.push_back(guid);
		}
		AuctionMgr::Instance()->OnQueryAuctionSaleLisForExplaceHzEquipRet(roleId, auctionIds, type, itemTypeId, strengthen, beadbuffid, enhanceType, goodState, opType, realEnhanceType);
	}

	virtual void OnFailed(UInt32 errorcode)
	{
		ErrorLogStream << "player(" << roleId << ") query" << " itemTypeId(" << itemTypeId << ") strengthen(" << strengthen << ") auction price list count failed, errorcode(" << errorcode << ")." << LogStream::eos;
	}
	ObjID_t     roleId;
	AuctionType type;
	UInt32		itemTypeId;
	UInt32		strengthen;
	UInt32      beadbuffid;
	UInt8		enhanceType;
	UInt8		goodState;
	UInt8		opType;
	UInt8	    realEnhanceType;
};

class SelectAuctionSalePriceListCallback : public CLRecordCallback
{
public:
	SelectAuctionSalePriceListCallback(ObjID_t roleId_, AuctionType type_, UInt8 auctionState_, UInt32 itemTypeId_, UInt32 strengthen_, UInt8 enhanceType_)
		: roleId(roleId_), type(type_), auctionState(auctionState_), itemTypeId(itemTypeId_), strengthen(strengthen_), enhanceType(enhanceType_){}
	virtual void OnFinished()
	{
		std::vector<ObjID_t> auctionIds;
		while (NextRow())
		{
			ObjID_t guid = GetKey();;
			auctionIds.push_back(guid);
		}
		AuctionMgr::Instance()->OnQueryAuctionSalePriceListRet(roleId, auctionIds, type, auctionState, itemTypeId, strengthen, enhanceType);
	}

	virtual void OnFailed(UInt32 errorcode)
	{
		ErrorLogStream << "player(" << roleId << ") query" << " itemTypeId(" << itemTypeId << ") strengthen(" << strengthen << ") auction price list count failed, errorcode(" << errorcode << ")." << LogStream::eos;
	}

	ObjID_t     roleId;
	AuctionType type;
	UInt8		auctionState;
	UInt32		itemTypeId;
	UInt32		strengthen;
	UInt8		enhanceType;
};

bool AuctionMgr::OnPlayerQueryItemRecentPrices(WSPlayer *player, AuctionType type, UInt32 itemTypeId, UInt32 strengthen, UInt32  beadbuffid, UInt8 enhanceType)
{
	if (!player)
	{
		return false;
	}

	ItemDataEntry* itemData = ItemDataEntryMgr::Instance()->FindEntry(itemTypeId);
	if (!itemData)
	{
		ErrorLogStream << "itemData is null itemTypeId : " << itemTypeId << LogStream::eos;
		return false;
	}

	InfoLogStream << "player(" << player->GetID() << ") query type(" << (UInt8)type << ") item(" << itemTypeId << ") strengthen (" << strengthen << ") recent price." << LogStream::eos;

	AuctionPricesSt* priceCanch = GetAuctionPriceListCache(itemTypeId, strengthen, beadbuffid, enhanceType, *itemData);
	if (priceCanch)
	{
		CLProtocol::WorldAuctionQueryItemPricesRes res = priceCanch->res;
		res.strengthen = strengthen;
		//红字装备物品列表空的话,查看对应的普通装备的缓存
		if (enhanceType > ENHANCE_INVALID &&  res.actionItems.size() == 0)
		{
			AuctionPricesSt* priceCanch_ = GetAuctionPriceListCache(itemTypeId, strengthen, beadbuffid, ENHANCE_INVALID, *itemData);
			if (priceCanch_ && priceCanch_->res.actionItems.size() > 0)
			{
				res.actionItems = priceCanch_->res.actionItems;
	}
			else if (priceCanch_ == NULL) //去拉普通装备价格列表
			{
				if (WSApplication::Instance()->IsAuctionOpUseMem())
				{
					UpdateAuctionPriceCacheForExplaceHzEquipFromMem(player, type, itemTypeId, strengthen, beadbuffid, ENHANCE_INVALID, AGS_ONSALE, APLOP_ON_SALE, enhanceType);
				}
				else
				{
					UpdateAuctionPriceCacheForExplaceHzEquip(player, type, itemTypeId, strengthen, beadbuffid, ENHANCE_INVALID, AGS_ONSALE, APLOP_ON_SALE, enhanceType);
				}
				return true;
			}
		}
		player->SendProtocol(res);
		return true;
	}

	if (WSApplication::Instance()->IsAuctionOpUseMem())
	{
		QueryItemRecentPricesFormMem(player, type, itemTypeId, strengthen, beadbuffid, enhanceType);
	}
	else
	{
		CLProtocol::AuctionQueryCondition cond;
		cond.itemTypeID = itemTypeId;
		cond.goodState = AGS_ONSALE;
		CLSelectCommand* command = CLRecordClient::Instance()->PrepareSelectCommand("t_auction_new");
		command->PutColumn("itemid");
		GenSqlByCondition(player, command, cond);

		if (itemData->subType == ST_MAGICCARD)
		{
			command->PutCond("strengthen", strengthen, "=");
		}
		else
		{
			if (strengthen > AUCTION_SPECIAL_AVERAGE_PRICE_STRENGTHEN_LEVEL)
			{
				command->PutCond("strengthen", strengthen, "=");
			}
			else
			{
				if (!WSApplication::Instance()->GetAuctionSqlInModify())
				{
					if (enhanceType > ENHANCE_INVALID) //红字装备
					{
						UInt8 min = 0;
						UInt8 max = 0;
						GetStrengthenRangeForHzEquip(strengthen, min, max);
						command->PutCond("strengthen", min, ">=");
						command->PutCond("strengthen", max, "<=");
						command->PutCond("equiptype", EQUIP_RED, "=");
					}
					else //普通和带气息的装备
					{
						command->PutCond("strengthen", 0, ">=");
						command->PutCond("strengthen", 10, "<=");
						command->PutCond("equiptype", EQUIP_RED, "!=");
					}
				}
				else
				{
					if (enhanceType > ENHANCE_INVALID) //红字装备
					{
						UInt8 min = 0;
						UInt8 max = 0;
						GetStrengthenRangeForHzEquip(strengthen, min, max);
						std::stringstream stream;
						stream << "(";
						for (UInt32 i = min; i <= max; ++i)
						{
							stream << i;
							if (i != max)
							{
								stream << ",";
							}
						}
						stream << ")";

						command->PutCond("strengthen", stream.str(), "in");
						command->PutCond("equiptype", EQUIP_RED, "=");
					}
					else
					{
						command->PutCond("strengthen", "(0,1,2,3,4,5,6,7,8,9,10)", "in");
						command->PutCond("equiptype", EQUIP_RED, "!=");
					}
				}
			}
		}

		std::stringstream orderStream;
		orderStream << "order by price / num asc";
		command->SetOrder(orderStream.str());

		UInt32 limit = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_PRICESLIST_NUM);
		limit = (limit == 0) ? 10 : limit;
		std::ostringstream limitoss;
		limitoss << "limit " << limit;
		command->SetLimit(limitoss.str());

		CLRecordClient::Instance()->SendCommand(command, new SelectAuctionSaleListCallback(player->GetID(), type, itemTypeId, strengthen, beadbuffid, enhanceType));
	}

	return true;
}

bool AuctionMgr::OnQueryAuctionSaleLisRet(ObjID_t roleId, std::vector<ObjID_t>& auctionIds, AuctionType type, UInt32 itemTypeId, UInt32 strengthen, UInt32 beadbuffid, UInt8 enhanceType)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(roleId);
	if (!player)
	{
		InfoLogStream << "query auction sale list ret, player(" << roleId << ") offline." << LogStream::eos;
		return false;
	}

	CLProtocol::WorldAuctionQueryItemPricesRes res;
	res.type = type;
	res.itemTypeId = itemTypeId;
	res.strengthen = strengthen;
	res.recommendPrice = 0;

	for (auto auctionId : auctionIds)
	{
		AuctionInfo* auctionInfo = _FindAuction(auctionId);
		if (!auctionInfo)
		{
			continue;
		}

		res.actionItems.push_back(*(AuctionBaseInfo*)auctionInfo);
	}
	ItemDataEntry * itemdata = ItemDataEntryMgr::Instance()->FindEntry(itemTypeId);
	if (!itemdata)
	{
		InfoLogStream << "query auction sale list ret itemdata null player(" << roleId << ") itemTypeId(" << itemTypeId <<")" << LogStream::eos;
		return false;
	}
	res.averagePrice = 0;

	if (!_GetLimitPrice(itemTypeId, strengthen, beadbuffid, enhanceType, res.minPrice, res.maxPrice))
	{
		return false;
	}

	if (itemdata->isTreasure && WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_AUCTION_TREAS))
	{
		res.visAverPrice = 0;
		//res.averagePrice = _GetTreasAverPrice(itemTypeId, strengthen, beadbuffid);
	}
	else
	{
		UInt8 strengthen_ = GetStrengthenForAverPrice(*itemdata, strengthen, enhanceType);
		AuctionRecommendPrice* recommend = GetRecommendPrice(itemTypeId, strengthen_, beadbuffid, enhanceType);
		if (recommend && recommend->UpdateVisPrice())
		{
			CLUpdateCommand *pCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_auction_recommend_price", recommend->guid, false);
			pCmd->PutData("visibility_price", recommend->visibility_price);
			pCmd->PutData("vis_price_update_time", recommend->vis_price_update_time);
			CLRecordClient::Instance()->SendCommand(pCmd);
		}
		_GetVisAverPrice(itemTypeId, strengthen, beadbuffid, enhanceType, res.visAverPrice); //非珍品宝珠buffid必定是0
		res.recommendPrice = _GetRecommendPrice(itemTypeId, strengthen, enhanceType);
		//res.averagePrice = _GetAverPrice(itemTypeId, strengthen, 0); //非珍品宝珠buffid必定是0
	}
	//res.averagePrice = _GetAverPrice(itemTypeId, strengthen, 0);

	if (!_GetLimitPrice(itemTypeId, strengthen, beadbuffid, enhanceType, res.minPrice, res.maxPrice))
	{
		InfoLogStream << "_GetLimitPrice fail player(" << player->GetID() << ") query type(" << (UInt8)type << ") item(" << res.itemTypeId << ") strengthen (" << res.strengthen << ") recent price. " << LogStream::eos;
		return false;
	}

	UpdateAuctionPriceListCache(itemTypeId, strengthen, beadbuffid, enhanceType, res, *itemdata);

	if (enhanceType > ENHANCE_INVALID && res.actionItems.size() == 0)
	{
		AuctionPricesSt* priceCanch_ = GetAuctionPriceListCache(itemTypeId, strengthen, beadbuffid, ENHANCE_INVALID, *itemdata);
		if (priceCanch_ && priceCanch_->res.actionItems.size() > 0)
		{
			res.actionItems = priceCanch_->res.actionItems;
		}
		else if (priceCanch_ == NULL)
		{
			UpdateAuctionPriceCacheForExplaceHzEquip(player, type, itemTypeId, strengthen, beadbuffid, ENHANCE_INVALID, AGS_ONSALE, APLOP_ON_SALE, enhanceType);
			return true;
		}
	}

	player->SendProtocol(res);
	return true;
}

bool AuctionMgr::OnSelectAuctionRet(CLRecordCallback *pCallback)
{
    while (pCallback->NextRow())
    {
        AuctionInfo	*pAuctionInfo = new AuctionInfo;
        pAuctionInfo->guid = pCallback->GetKey();
        pAuctionInfo->owner = pCallback->GetData("owner");
		pAuctionInfo->account = pCallback->GetData("account");
        pAuctionInfo->type = pCallback->GetData("type");
        pAuctionInfo->price = pCallback->GetData("price");
        pAuctionInfo->counterFee = pCallback->GetData("counter_fee");
        pAuctionInfo->pricetype = pCallback->GetData("pricetype");
        pAuctionInfo->duetime = pCallback->GetData("duetime");
        pAuctionInfo->itemTypeId = pCallback->GetData("item_type_id");
        pAuctionInfo->num = pCallback->GetData("num");
		pAuctionInfo->onSaleTime = pCallback->GetData("on_sale_time");
		pAuctionInfo->mainType = (AuctionMainItemType)(UInt8)pCallback->GetData("item_main_type");
		pAuctionInfo->itemSrcGuid = pCallback->GetData("item_src_guid");
		pAuctionInfo->ip = pCallback->GetData("ip").GetString();
		pAuctionInfo->deviceId = pCallback->GetData("device_id").GetString();
		pAuctionInfo->onPublicTime = pCallback->GetData("on_public_time");
		pAuctionInfo->couponStrengthenTo = pCallback->GetData("coupon_strengthen_to");
		pAuctionInfo->ownerVipLev = pCallback->GetData("ownerVipLev");
		pAuctionInfo->isRecoveHandled = pCallback->GetData("recove_handled");
		pAuctionInfo->auctionSubType = pCallback->GetData("item_sub_type");
		pAuctionInfo->quality = pCallback->GetData("quality");
		pAuctionInfo->itemLevel = pCallback->GetData("item_level");

        // 拍卖的是道具
        if (AT_ITEM == pAuctionInfo->type)
        {
			pAuctionInfo->item = new WSItem();
			pAuctionInfo->item->m_DataId = pCallback->GetData("itemid");
			pAuctionInfo->item->SetDBData(pCallback);
			pAuctionInfo->item->SetID(pAuctionInfo->guid);
			pAuctionInfo->itemTypeId = pAuctionInfo->item->m_DataId;
			pAuctionInfo->strengthen = pAuctionInfo->item->m_Strengthen;
			pAuctionInfo->itemScore = pAuctionInfo->item->m_ValuedScore;
			pAuctionInfo->equipType = pAuctionInfo->item->m_EquipType;
			pAuctionInfo->enhanceType = pAuctionInfo->item->m_EnhanceType;
			pAuctionInfo->itemDueTime = pAuctionInfo->item->m_uDeadLine;
			pAuctionInfo->itemTransNum = pAuctionInfo->item->m_AuctionTransNum;

			ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(pAuctionInfo->item->m_DataId);
			if (dataEntry)
			{
				pAuctionInfo->item->SetDataEntry(dataEntry);
				pAuctionInfo->item->SetAttribute();
				pAuctionInfo->itemtype = dataEntry->type;
				pAuctionInfo->subType = dataEntry->subType;
				pAuctionInfo->item->m_isTreas = dataEntry->isTreasure && WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_AUCTION_TREAS);
				if (pAuctionInfo->subType == ST_COUPON)
				{
					pAuctionInfo->strengthen = 0;
					pAuctionInfo->item->m_Strengthen = 0;
				}
				else if (pAuctionInfo->subType == ST_BEAD)
				{
					pAuctionInfo->beadBuffId = pAuctionInfo->item->m_Param2;
				}
				else{

				}
			}

			//珍品设置相关状态
			UInt32 curtime = (UInt32)CURRENT_TIME.Sec();
			pAuctionInfo->isTreas = false;
			if (dataEntry && dataEntry->isTreasure == 1 && WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_AUCTION_TREAS))
			{
				pAuctionInfo->isTreas = true;
				pAuctionInfo->publicEndTime = pAuctionInfo->onSaleTime;
				if (pAuctionInfo->onPublicTime == 0)
				{
					ErrorLogStream << "Load Auction Treasure item onPublicTime err , adjust onPublicTime to currtime, pAuctionInfo->guid:" << pAuctionInfo->guid << ", curtime:" << curtime <<
						", onPublicTime:" << pAuctionInfo->onPublicTime << ", onSaleTime:" << pAuctionInfo->onSaleTime << LogStream::eos;
					pAuctionInfo->onPublicTime = pAuctionInfo->onSaleTime - SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_TREAS_PUBLIC_TIME) * 60;
				}
				if (pAuctionInfo->onPublicTime > 0
					&& pAuctionInfo->onSaleTime > 0
					&& pAuctionInfo->onPublicTime <= curtime
					&& curtime < pAuctionInfo->onSaleTime)
				{
					pAuctionInfo->goodState = AGS_PUBLIC;
					pAuctionInfo->buyState = AGBS_INVALID;
				}
				else if ( pAuctionInfo->onSaleTime > 0
					&& curtime >= pAuctionInfo->onSaleTime)
				{
					pAuctionInfo->goodState = AGS_ONSALE;
				}
				else
				{
					ErrorLogStream << "Load Auction Treasure item err1 pAuctionInfo->guid:" << pAuctionInfo->guid << ", curtime:" << curtime << 
						", onPublicTime:" << pAuctionInfo->onPublicTime << ", onSaleTime:" << pAuctionInfo->onSaleTime << LogStream::eos;
				}

				if (pAuctionInfo->goodState == AGS_ONSALE) //上架状态设置购买状态
				{
					UInt32 rushTime = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_RUSH_BUY_TIME);
					if (curtime >= pAuctionInfo->onSaleTime && curtime < pAuctionInfo->onSaleTime + rushTime)
					{
						pAuctionInfo->buyState = AGBS_RUSH_BUY;
					}
					else if (curtime >= pAuctionInfo->onSaleTime + rushTime)
					{
						pAuctionInfo->buyState = AGBS_NORMAL_BUY;
					}
					else
					{
						ErrorLogStream << "Load Auction Treasure item err2 pAuctionInfo->guid:" << pAuctionInfo->guid << ", curtime:" << curtime <<
							", onPublicTime:" << pAuctionInfo->onPublicTime << ", onSaleTime:" << pAuctionInfo->onSaleTime << LogStream::eos;
					}
				}
				InfoLogStream << "load auction treasure item guid:" << pAuctionInfo->guid << ", curr_time: " << curtime << ", onPublicTime:" << pAuctionInfo->onPublicTime
					<< ", onSaleTime:" << pAuctionInfo->onSaleTime << ", goodState: " << (UInt32)pAuctionInfo->goodState << ", buyState: " << (UInt32)pAuctionInfo->buyState << LogStream::eos;

				//上架珍品需要记录在售价格
				OnTreasOnSale(*pAuctionInfo);
			}

			//附魔卡
			if (dataEntry && dataEntry->subType == ST_MAGICCARD)
			{
				UInt32 curtime = (UInt32)CURRENT_TIME.Sec();
				if (curtime >= pAuctionInfo->onSaleTime && curtime < pAuctionInfo->duetime)
				{
					OnMagicOnsale(*pAuctionInfo);
				}
				else if (curtime < pAuctionInfo->onSaleTime)
				{
					AddWaiteOnsaleMagic(pAuctionInfo->guid);
				}
			}

			if (dataEntry && pAuctionInfo->item && pAuctionInfo->item->m_EnhanceType > 0)
			{
				auto data = EquipEnhanceAttributeDataEntryMgr::Instance()->FindEntry(dataEntry->needLevel, pAuctionInfo->item->m_Strengthen, pAuctionInfo->item->m_Quality);
				if (data)
				{
					pAuctionInfo->item->m_EnhanceNum = data->enhanceAttribute;
					pAuctionInfo->enhanceNum = data->enhanceAttribute;
				}
				else
				{
					ErrorLogStream << "load item zenfu set m_EnhanceNum fail not find EquipEnhanceAttributeDataEntry needLevel : " << dataEntry->needLevel << ", strengthen : " << pAuctionInfo->item->m_Strengthen
						<< ", qulity : " << pAuctionInfo->item->m_Quality << LogStream::eos;
				}
			}
        }
		else if (AT_GOLD == pAuctionInfo->type)
		{
			pAuctionInfo->itemTypeId = pCallback->GetData("itemid");
		}

		
		this->AddOneAution(pAuctionInfo);
        /*m_auctiones[pAuctionInfo->guid] = pAuctionInfo;
		if (pAuctionInfo->isTreas && pAuctionInfo->buyState != AGBS_NORMAL_BUY)
		{
			this->AddOneTreas(pAuctionInfo);
		}*/
    }

    return true;
}

bool AuctionMgr::OnSelectAuctionRecommendPriceRet(CLRecordCallback *pCallback)
{
    while (pCallback->NextRow())
    {
        AuctionRecommendPrice *recommend = new AuctionRecommendPrice;
        recommend->guid = pCallback->GetKey();
        recommend->itemTypeID = pCallback->GetData("item_type_id");
		recommend->strengthen = pCallback->GetData("strengthen");
		recommend->bead_buff_id = pCallback->GetData("bead_buff_id");
		recommend->enhanceType = pCallback->GetData("enhance_type");
		recommend->price = pCallback->GetData("price");
		recommend->price_update_time = pCallback->GetData("price_update_time");
		recommend->visibility_price = pCallback->GetData("visibility_price");
		recommend->vis_price_update_time = pCallback->GetData("vis_price_update_time");
		std::string recentPriceStr = pCallback->GetData("recent_price").GetString();
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(recentPriceStr, strs, "|");
		for (auto str : strs)
		{
			std::vector<std::string> ss;
			Avalon::StringUtil::Split(str, ss, ",");
			if (ss.size() == 6)
			{
				UInt32 price = Avalon::StringUtil::ConvertToValue<UInt32>(ss[0]);
				UInt32 num = Avalon::StringUtil::ConvertToValue<UInt32>(ss[1]);
				UInt32 timestamp = Avalon::StringUtil::ConvertToValue<UInt32>(ss[2]);
				UInt32 quality = Avalon::StringUtil::ConvertToValue<UInt32>(ss[3]);
				UInt32 beadAverPrice = Avalon::StringUtil::ConvertToValue<UInt32>(ss[4]);
				UInt32 magicCardAverPrice = Avalon::StringUtil::ConvertToValue<UInt32>(ss[5]);
				recommend->AddRecentPrice(price, num, timestamp, quality, beadAverPrice, magicCardAverPrice);
			}
		}
       
		AuctionRecommendPriceKey key(recommend->itemTypeID, recommend->strengthen, recommend->bead_buff_id, recommend->enhanceType);
        m_recommendPrice[key] = recommend;

		UInt32 minTimes = AuctionSystemValueDataEntryMgr::Instance()->GetSystemValue(ASVT_CALC_AVERPRICE_MINTIMES);
		if (minTimes == 0) minTimes = 10;

		if (recommend->price > 0 && recommend->recentPrice.size() < minTimes)
		{
			ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(recommend->itemTypeID);
			if (dataEntry && (!dataEntry->isTreasure || !WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_AUCTION_TREAS)))
			{
				recommend->price = 0;
				recommend->visibility_price = 0;
				recommend->price_update_time = (UInt32)CURRENT_TIME.Sec();
				recommend->vis_price_update_time = recommend->price_update_time;

				CLUpdateCommand *pCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_auction_recommend_price", recommend->guid, false);
				pCmd->PutData("price", recommend->price);
				pCmd->PutData("price_update_time", recommend->price_update_time);
				pCmd->PutData("visibility_price", recommend->visibility_price);
				pCmd->PutData("vis_price_update_time", recommend->vis_price_update_time);
				CLRecordClient::Instance()->SendCommand(pCmd);
			}
		}
    }

	//计算附魔卡最低实时平均交易价格
	_CalcMagicMinAverPriceByQuality();
    return true;
}

bool AuctionMgr::OnSelectTimeoutAuctionRet(CLRecordCallback *pCallback)
{
    while (pCallback->NextRow())
    {
        ObjID_t id = pCallback->GetKey();
        AuctionInfo* auctionInfo = _FindAuction(id);
        if (!auctionInfo)
        {
            ErrorLogStream << "auction(" << id << ") timeout but can't find it." << LogStream::eos;
            continue;
        }

       // DebugLogStream << "auction(" << id << ") is timeout sendback to player(" << auctionInfo->owner << ")." << LogStream::eos;
       //_SendAuctionToPlayer(auctionInfo, auctionInfo->num, auctionInfo->owner, ASIT_TIMEOUT);
		DebugLogStream << "auction(" << id << ") is timeout handled for offsaled" << LogStream::eos;
		
		if (!auctionInfo->offsaleHanled && auctionInfo->isTreas && WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_AUCTION_TREAS))
		{
			OnTreasOffSale(*auctionInfo);
			this->RemoveOneTreas(auctionInfo);
		}

		OnMagicTimeOut(*auctionInfo);
		if (WSApplication::Instance()->IsAuctionOpUseMem())
		{
			RemoveAuctionForMemQuery(auctionInfo);
		}
		
		auctionInfo->offsaleHanled = 1;
		// 保存到数据库
		CLUpdateCommand *pCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_auction_new", auctionInfo->guid, true);
		pCmd->PutData("offsale_handled", 1);
		CLRecordClient::Instance()->SendCommand(pCmd);
    }

    return true;
}

bool AuctionMgr::OnQueryPlayerAuctionListRet(ObjID_t roleId, std::vector<std::pair<UInt8, ObjID_t> >& auctionList)
{
    WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(roleId);
    if (!player)
    {
        InfoLogStream << "query player auction list ret, player(" << roleId << ") offline." << LogStream::eos;
        return false;
    }

	player->ClearAuction();
    for (auto info : auctionList)
    {
        player->AddAuction((AuctionType)info.first, info.second);
    }

    return true;
}

void AuctionMgr::OnQueryAuctionRet(ObjID_t roleId, UInt8 type, UInt16 curPage, UInt16 maxPage, std::vector<ObjID_t>& auctionIds, UInt8 goodState, UInt8 attent)
{
    WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(roleId);
    if (!player)
    {
        InfoLogStream << "player(" << roleId << ") query auction list return, but player is offline." << LogStream::eos;
        return;
    }

    CLProtocol::WorldAuctionListQueryRet ret;
    ret.type = type;
    ret.curPage = curPage;
    ret.maxPage = maxPage;
	ret.goodState = goodState;
	ret.attent = attent;

	std::vector<ObjID_t> guids;
    for (auto auctionId : auctionIds)
    {
        AuctionInfo* auctionInfo = _FindAuction(auctionId);
		if (!auctionInfo)
        {
            continue;
        }

		if (attent == ATT_ATTENT && auctionInfo->goodState != goodState)
		{
			continue;
		}

		guids.push_back(auctionInfo->guid);
		AuctionAttentMgr::Instance()->GetAuctionAttentInfo(*auctionInfo, player->GetID());
        ret.data.push_back(*(AuctionBaseInfo*)auctionInfo);
    }

	if (attent == ATT_ATTENT && ret.data.size() > 1)
	{
		if (goodState == AGS_PUBLIC) //关注公示期物品, 排序：最新关注的排在最上面，最早关注的排在最下面
		{
			std::sort(ret.data.begin(), ret.data.end(), [](AuctionBaseInfo a, AuctionBaseInfo b){
				return a.attentTime > b.attentTime;
			}
			);
		}
		else if (goodState == AGS_ONSALE) //关注上架物品, 排序: 最新上架的排在最上面，最早上架的排在最下面
		{
			std::sort(ret.data.begin(), ret.data.end(), [](AuctionBaseInfo a, AuctionBaseInfo b){
				return a.onSaleTime > b.onSaleTime;
			}
			);
		}
	}

	player->SetCanBuyAuctionGuids(guids);
    player->SendProtocol(ret);
}

void AuctionMgr::OnAuctionCheckRet(Avalon::NetInputStream &stream)
{
	UInt8	isAgain = 0;
	UInt8   depositEnough = 0;
	ObjID_t auctionGuid = 0;
	try
	{
		stream & isAgain & depositEnough & auctionGuid;
	}
	catch (Avalon::NetException&)
	{
		ErrorLogStream << "decode isAgain err" << LogStream::eos;
		return;
	}

	if (isAgain)
	{
		ObjID_t playerId = 0;
		UInt8   type = 0;
		UInt32 price = 0;
		UInt8  pricetype = 0;
		UInt8  duration = 0;
		UInt32 counterFee = 0;

		AuctionInfo* auctionInfo = _FindAuction(auctionGuid);
		if (!auctionInfo)
		{
			InfoLogStream << "player(" << playerId << ") invalid auction guid(" << auctionGuid << ")." << LogStream::eos;
			return;
		}

		if (!auctionInfo->auctionAgaining)
		{
			ErrorLogStream << "player(" << auctionInfo->owner << ") want again add guid(" << auctionGuid << ", " << auctionInfo->itemTypeId
				<< ") to auction failed, is not in auctionAgaining." << LogStream::eos;
			return;
		}

		auctionInfo->auctionAgaining = false;
		if (!depositEnough)
		{
			ErrorLogStream << "player(" << auctionInfo->owner << ") want again add guid(" << auctionGuid << ", " << auctionInfo->itemTypeId
				<< ") to auction failed, deposit not enough." << LogStream::eos;
			return;
		}

		try
		{
			stream & playerId & type & price & pricetype & duration & counterFee;
		}
		catch (Avalon::NetException&)
		{
			ErrorLogStream << "decode playerId err" << LogStream::eos;
			return;
		}

		ItemDataEntry* itemData = ItemDataEntryMgr::Instance()->FindEntry(auctionInfo->itemTypeId);

		if (!itemData)
		{
			ErrorLogStream << "player(" << auctionInfo->owner << ") want again add auction(" << auctionGuid << ", item(" << auctionInfo->itemTypeId
				<< ") to auction failed, can't find item's data entry." << LogStream::eos;
			return;
		}

		UInt32 num = auctionInfo->num; //重新上架数量不能改变
		if (num == 0)
		{
			return;
		}
		
		bool handleSysRecove = false; //是否做扫货处理
		UInt32 oldDeposit = GetDepositByAuctionPrice(auctionInfo->price);
		UInt32 curTime = (UInt32)CURRENT_TIME.Sec();
		WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(auctionInfo->owner);

		if (auctionInfo->isTreas && WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_AUCTION_TREAS)) //珍品
		{
			UInt32 uintPrice = price / num;
			UInt32 oldUintPrice = auctionInfo->price / auctionInfo->num;
			if (uintPrice == oldUintPrice) //单价与之前单价相同,直接上架
			{
				DebugLogStream << "auction on sale again uintPrice == oldUintPrice guid : " << auctionInfo->guid << ", " << LogStream::eos;
				auctionInfo->isAgainOnsale = 1;
				auctionInfo->goodState = AGS_ONSALE;
				auctionInfo->buyState = AGBS_NORMAL_BUY;
				auctionInfo->onSaleTime = curTime;
				auctionInfo->onPublicTime = auctionInfo->onSaleTime - SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_TREAS_PUBLIC_TIME) * 60;
				auctionInfo->publicEndTime = auctionInfo->onSaleTime;
				auctionInfo->duetime = _GetAuctionDuetime(itemData);
				auctionInfo->price = price;
				OnTreasOnSale(*auctionInfo);
				handleSysRecove = true;
			}
			else //单价与之前单价不同,走正常流程，先公示
			{
				auctionInfo->goodState = AGS_PUBLIC;
				auctionInfo->buyState = AGBS_INVALID;
				auctionInfo->onPublicTime = curTime;
				auctionInfo->onSaleTime = auctionInfo->onPublicTime + SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_TREAS_PUBLIC_TIME) * 60; //物品公示结束,上架时间戳
				auctionInfo->publicEndTime = auctionInfo->onSaleTime;
				auctionInfo->duetime = _GetAuctionDuetime(itemData);
				auctionInfo->price = price;
				this->AddOneTreas(auctionInfo);
			}
		}
		else //非珍品
		{
			UInt32 lower = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_ON_SALE_WAIT_SEC_LOWER);
			UInt32 upper = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_ON_SALE_WAIT_SEC_UPPER);
			UInt32 onSaleWaitSec = Avalon::Random::RandBetween(lower, upper);
			auctionInfo->onSaleTime = (UInt32)CURRENT_TIME.Sec() + onSaleWaitSec;
			auctionInfo->duetime = _GetAuctionDuetime(itemData);
			auctionInfo->price = price;
			auctionInfo->publicEndTime = 0;
		}

		auctionInfo->counterFee = counterFee;
		auctionInfo->ownerVipLev = player ? player->GetVipLvl() : auctionInfo->ownerVipLev;
		auctionInfo->isRecoveHandled = 0;
		auctionInfo->offsaleHanled = 0;
		CLUpdateCommand *pCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_auction_new", auctionInfo->guid, true);
		pCmd->PutData("price", auctionInfo->price);
		pCmd->PutData("duetime", auctionInfo->duetime);
		pCmd->PutData("on_sale_time", auctionInfo->onSaleTime);
		pCmd->PutData("on_public_time", auctionInfo->onPublicTime);
		pCmd->PutData("counter_fee", auctionInfo->counterFee);
		pCmd->PutData("ownerVipLev", auctionInfo->ownerVipLev);
		pCmd->PutData("recove_handled", auctionInfo->isRecoveHandled);
		pCmd->PutData("offsale_handled", auctionInfo->offsaleHanled);

		//非珍品上架检查是否扫货,是否附魔卡
		if (auctionInfo->isTreas == 0)
		{
			if (IsShouldRecovery(*auctionInfo))
			{
				AddOneShouldRcoAuction(auctionInfo->guid);
			}
			else
			{
				auctionInfo->isRecoveHandled = 1;
				pCmd->PutData("recove_handled", auctionInfo->isRecoveHandled);
			}

			if (itemData->subType == ST_MAGICCARD)
			{
				AddWaiteOnsaleMagic(auctionInfo->guid);
			}
		}

		CLRecordClient::Instance()->SendCommand(pCmd);

		if (player)
		{
			CLProtocol::WorldAuctionNotifyRefresh refresh;
			refresh.type = auctionInfo->type;
			refresh.reason = SRR_SELL;
			refresh.auctGuid = auctionInfo->guid;
			player->SendProtocol(refresh);
		}

		std::vector<ItemReward> depositItems;
		std::vector<WSItem*>	vecitems;
		UInt32 depositItemTypeId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_DEPOSIT_ITEM_ID);
		depositItems.push_back(ItemReward(depositItemTypeId, oldDeposit, true, 0));

		char buffer[1024];
		memset(buffer, 0, sizeof(buffer));
		std::string	strTitle;
		std::string	strContent;
		std::string strReason;
		// 发送者
		SysNotifyMgr::Instance()->MakeNotify(1047, buffer, sizeof(buffer));
		std::string strSenderName(buffer);

		std::string strengthenStr = "";
		if (auctionInfo->strengthen != 0)
		{
			strengthenStr = Avalon::StringUtil::ConvertToString<UInt32>(auctionInfo->strengthen);
		}

		std::string itemName = "";
		ItemDataEntry	*pDataEntry = ItemDataEntryMgr::Instance()->FindEntry(auctionInfo->itemTypeId);
		if (pDataEntry != NULL)
		{
			itemName = pDataEntry->name;
		}

		SysNotifyMgr::Instance()->MakeNotify(2213, buffer, sizeof(buffer));
		strTitle = buffer;
		UInt32 notifyId = auctionInfo->strengthen != 0 ? 2226 : 2214;
		SysNotifyMgr::Instance()->MakeNotify(notifyId, buffer, sizeof(buffer), strengthenStr.c_str(), itemName.c_str(), auctionInfo->num);
		strContent = buffer;
		strReason = GetReason(SOURCE_TYPE_ACTION_TIMEOUT, auctionInfo->item ? auctionInfo->item->m_DataId : 0, auctionInfo->type);

		if (MailBox::SendSysMail(playerId, strSenderName, strTitle, strContent, strReason, depositItems, vecitems) != ErrorCode::SUCCESS)
		{
			// 发送邮件失败
			ErrorLogStream << "send auction(" << auctionInfo->guid << ") mail to role(" << playerId << ") title("
				<< strTitle << ") failed." << LogStream::eos;
			return;
		}

		if (WSApplication::Instance()->IsAuctionOpUseMem())
		{
			AddOneAuctionForMemQuery(auctionInfo);
		}
		
		//扫货处理
		if (handleSysRecove)
		{
			bool handled = false;
			HandleForRecovery(*auctionInfo, true, handled);
		}
		return;
	}

	AuctionInfo	*auctionInfo = new AuctionInfo;
    WSItem* item = NULL;
    UInt8 duration = 0;
	ObjID_t srcItemGuid = 0;

    try
    {
		stream & auctionInfo->owner & auctionInfo->type & auctionInfo->price & auctionInfo->pricetype & duration & auctionInfo->counterFee;

		if (!auctionInfo->auctionAgaining)
		{
			if (AT_ITEM == auctionInfo->type)
			{
				item = new WSItem;
				stream & srcItemGuid & item->m_DataId;
				item->Decode(stream);

				auctionInfo->itemTypeId = item->m_DataId;
				auctionInfo->strengthen = item->m_Strengthen;
				auctionInfo->itemScore = item->m_ValuedScore;
				auctionInfo->num = item->m_Num;
				auctionInfo->item = item;
				auctionInfo->itemSrcGuid = srcItemGuid;
				auctionInfo->itemDueTime = item->m_uDeadLine;
				auctionInfo->enhanceNum = item->m_EnhanceNum;
				auctionInfo->itemTransNum = item->m_AuctionTransNum;
				
			}
			else if (AT_GOLD == auctionInfo->type)
			{
				stream & auctionInfo->itemTypeId;
				auctionInfo->num = 1;
			}
		}
        
    }
    catch (Avalon::NetException&)
    {
        if (NULL != item)
            delete item;
        delete auctionInfo;
        return;
    }

    ItemDataEntry* itemData = ItemDataEntryMgr::Instance()->FindEntry(auctionInfo->itemTypeId);
    if (!itemData)
    {
        ObjID_t id = item ? item->GetID() : 0;
        ErrorLogStream << "player(" << auctionInfo->owner << ") want add item(" << id << ", " << auctionInfo->itemTypeId
                        << ") to auction failed, can't find item's data entry." << LogStream::eos;
        return;
    }

	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(auctionInfo->owner);
	
    auctionInfo->guid = CLRecordClient::Instance()->GenGuid();
	auctionInfo->duetime = _GetAuctionDuetime(itemData);
	auctionInfo->subType = itemData->subType;
	auctionInfo->itemtype = itemData->type;
	auctionInfo->quality = itemData->color;
	//UInt32 onSaleWaitSec = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_ON_SALE_WAIT_SEC);
	//auctionInfo->onSaleTime = (UInt32)CURRENT_TIME.Sec() + onSaleWaitSec;
	
	if (itemData->isTreasure == 1 && WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_AUCTION_TREAS)) //珍品
	{
		auctionInfo->isTreas = true;
		auctionInfo->onPublicTime = (UInt32)CURRENT_TIME.Sec(); //物品公示开始时间戳
		auctionInfo->onSaleTime = auctionInfo->onPublicTime + SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_TREAS_PUBLIC_TIME) * 60; //物品公示结束,上架时间戳
		auctionInfo->goodState = AGS_PUBLIC;
		auctionInfo->buyState = AGBS_INVALID;
		auctionInfo->publicEndTime = auctionInfo->onSaleTime;
	}
	else   //非珍品
	{
		auctionInfo->isTreas = false;
		UInt32 lower = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_ON_SALE_WAIT_SEC_LOWER);
		UInt32 upper = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_ON_SALE_WAIT_SEC_UPPER);
		UInt32 onSaleWaitSec = Avalon::Random::RandBetween(lower, upper);
		auctionInfo->onSaleTime = (UInt32)CURRENT_TIME.Sec() + onSaleWaitSec;
		auctionInfo->publicEndTime = 0;
	}

	if (player)
	{
		auctionInfo->account = player->GetAccId();
		auctionInfo->ip = player->GetIp();
		//auctionInfo->deviceId = "fuck";
		auctionInfo->deviceId = player->GetDeviceId().c_str();
		auctionInfo->ownerVipLev = player->GetVipLvl();
	}

    CLInsertCommand *pCmd = CLRecordClient::Instance()->PrepareInsertCommand("t_auction_new", auctionInfo->guid);
    pCmd->PutData("owner", auctionInfo->owner);
	pCmd->PutData("account", auctionInfo->account);
    pCmd->PutData("type", auctionInfo->type);
    pCmd->PutData("price", auctionInfo->price);
    pCmd->PutData("pricetype", auctionInfo->pricetype);
    pCmd->PutData("duetime", auctionInfo->duetime);
	pCmd->PutData("on_sale_time", auctionInfo->onSaleTime);
    pCmd->PutData("counter_fee", auctionInfo->counterFee);
	pCmd->PutData("ip", auctionInfo->ip);
	pCmd->PutData("device_id", auctionInfo->deviceId);
	pCmd->PutData("on_public_time", auctionInfo->onPublicTime);
	pCmd->PutData("ownerVipLev", auctionInfo->ownerVipLev);
    if (NULL != item)
    {
		auctionInfo->item->SetDataEntry(itemData);
		auctionInfo->item->m_isTreas = itemData->isTreasure && WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_AUCTION_TREAS);
		auctionInfo->mainType = GetAuctionMainType((ItemType)itemData->type, (ItemSubType)itemData->subType);
		auctionInfo->equipType = item->m_EquipType;
		auctionInfo->enhanceType = item->m_EnhanceType;

		pCmd->PutData("item_src_guid", auctionInfo->itemSrcGuid);
        pCmd->PutData("item_main_type", (UInt8)auctionInfo->mainType);
        pCmd->PutData("item_sub_type", GetAuctionSubType(itemData));
		auctionInfo->auctionSubType = GetAuctionSubType(itemData);

		if (itemData->subType == ST_COUPON || itemData->subType == ST_ZENGFU_AMPLIFICATION)
		{
			StrTicketData* entry = StrTicketDataMgr::Instance()->FindEntry(itemData->strenTicketDataIndex);
			if (entry)
			{
				//item->m_Strengthen = entry->effectLv;
				auctionInfo->couponStrengthenTo = entry->effectLv;
				auctionInfo->strengthen = 0;
				item->m_Strengthen = 0;
				pCmd->PutData("coupon_strengthen_to", entry->effectLv);
				pCmd->PutData("item_level", entry->rate / 10);
				auctionInfo->itemLevel = entry->rate / 10;
			}
			else
			{
				item->m_Strengthen = 0;
				auctionInfo->strengthen = 0;
				auctionInfo->couponStrengthenTo = 0;
				pCmd->PutData("coupon_strengthen_to", 0);
				pCmd->PutData("item_level", 0);
				auctionInfo->itemLevel = 0;
				ObjID_t id = item ? item->GetID() : 0;
				ErrorLogStream << "player(" << auctionInfo->owner << ") want add item(" << id << ", " << auctionInfo->itemTypeId
					<< ") to auction failed, can't find item's StrTicketData entry. strenTicketDataIndex : " << itemData->strenTicketDataIndex << LogStream::eos;
			}
		}
		else if (itemData->subType == ST_BEAD)
		{
			PreciousBeadDataEntry* entry = PreciousBeadDataEntryMgr::Instance()->FindEntry(auctionInfo->itemTypeId);
			if (entry)
			{
				pCmd->PutData("item_level", entry->level);
				auctionInfo->itemLevel = entry->level;
				auctionInfo->beadBuffId = item->m_Param2;
			}
		}
		else
		{
			pCmd->PutData("item_level", itemData->needLevel);
			auctionInfo->itemLevel = itemData->needLevel;
		}
		auctionInfo->item->SetQuality(itemData->color);
        auctionInfo->item->GetDBData(pCmd, false);
		item->SetID(auctionInfo->guid);
    }

    if (auctionInfo->type == AT_GOLD)
    {
        pCmd->PutData("itemid", auctionInfo->itemTypeId);
        pCmd->PutData("num", auctionInfo->num);
    }

	//非珍品上架检查是否扫货,是否附魔卡
	if (itemData->isTreasure == 0)
	{
		if (IsShouldRecovery(*auctionInfo))
		{
			AddOneShouldRcoAuction(auctionInfo->guid);
		}
		else
		{
			auctionInfo->isRecoveHandled = 1;
			pCmd->PutData("recove_handled", auctionInfo->isRecoveHandled);
		}

		if (itemData->subType == ST_MAGICCARD)
		{
			AddWaiteOnsaleMagic(auctionInfo->guid);
		}
	}

    CLRecordClient::Instance()->SendCommand(pCmd);

    UInt32 uAccId = 0;
    if (player)
    {
        uAccId = player->GetAccId();
        // WSPlayer中保存玩家的拍卖物品信息
        player->AddAuction((AuctionType)auctionInfo->type, auctionInfo->guid);

        // 发送提示
        player->SendNotify(1035);
    }

    if (AT_ITEM == auctionInfo->type)
    {
        GameInfoLogStream("Auction") << "player(" << uAccId << "," << auctionInfo->owner << ") add item(" <<  "|"
            << auctionInfo->itemTypeId << "|" << auctionInfo->num << ") to auction, price:" << auctionInfo->price << "|" 
            << auctionInfo->pricetype << LogStream::eos;
    }
    else if (AT_GOLD == auctionInfo->type)
    {
        GameInfoLogStream("Auction") << "player(" << uAccId << "," << auctionInfo->owner << ") add gold(" << "|"
            << auctionInfo->num << ") to auction, price:" << auctionInfo->price << "|"
            << auctionInfo->pricetype << LogStream::eos;
    }

	this->AddOneAution(auctionInfo);
	
    if (player)
    {
        CLProtocol::WorldAuctionNotifyRefresh refresh;
        refresh.type = auctionInfo->type;
        refresh.reason = SRR_SELL;
		refresh.auctGuid = auctionInfo->guid;
        player->SendProtocol(refresh);

		// by huchenhui 任务需要的拍卖行上架事件
		player->OnAddAuction(auctionInfo);
	}
}

bool AuctionMgr::OnBuyAuctionCheckRet(WSPlayer *player, ObjID_t id, UInt32 num)
{
    if (!player)
    {
        ErrorLogStream << "buy item(" << id << ") but can't find player." << LogStream::eos;
        return false;
    }

    auto auctionInfo = _FindAuction(id);
    
    if (!auctionInfo)
    {
        CLProtocol::WorldAuctionNotifyRefresh	notify;
        player->SendProtocol(notify);
        
		player->SendNotify(1079);
        return false;
    }

	if (auctionInfo->num < num || auctionInfo->num == 0)
	{
		ErrorLogStream << "player(" << player->GetID() << ") want to buy auction(" << auctionInfo->guid << ") invalid num(" << num << "),"
			<< "auctionInfo->num (" << auctionInfo->num << ")." << LogStream::eos;
		return false;
	}

	bool abnormalDeal = false;
			
	UInt32 unitPrice = auctionInfo->price / auctionInfo->num;
	UInt32 price = unitPrice * num;

	if (auctionInfo->type == AT_ITEM && auctionInfo->num > 0 && auctionInfo->isTreas == false)
	{
		UInt32 buffId = 0;

		if (auctionInfo->item && auctionInfo->subType == ST_BEAD)
		{
			buffId = auctionInfo->item->m_Param2;
		}
		_RecordDealPrice(auctionInfo, buffId, price, num);
	}
	else if (auctionInfo->type == AT_GOLD)
	{
		// 记录对应的价格
		_RecordDealPrice(auctionInfo, 0, price, num);
	}
    
	UInt8 auctionType = auctionInfo->type;
	if (!_SendAuctionToPlayer(auctionInfo, num, player->GetID(), ASIT_BUY, abnormalDeal))
	{
		return false;
	}

	player->SendNotify(1034);

	CLProtocol::WorldAuctionNotifyRefresh refresh;
	refresh.type = auctionType;
	refresh.reason = SRR_BUY;
	refresh.auctGuid = auctionInfo->guid;

	player->SendProtocol(refresh);

	return true;
}

/*std::vector<AuctionInfo*> AuctionMgr::GetPlayerAuctionInfoes(ObjID_t roleid)
{
	std::vector<AuctionInfo*> all;
	for (auto itr : m_auctiones)
	{
		auto info = itr.second;
		if (!info)
		{
			continue;
		}

		all.push_back(info);
	}

	return all;
}*/

UInt8 AuctionMgr::_GetAuctionMaxItemPerPlayer(AuctionType type, WSPlayer* player)
{
	UInt32 baseNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_ITEMAUCTION_MAX_ONSALENUM);
	return baseNum + (player ? player->GetAuctionAdditionBooth() : 0);
}

bool AuctionMgr::_SendAuctionToPlayer(AuctionInfo *auctionInfo, UInt32 num, ObjID_t roleId, AuctionSendItemType sendType, bool abnormalDeal)
{
	if (auctionInfo->num == 0)
	{
		ErrorLogStream << "auction(" << auctionInfo->guid << ") invalid num(" << auctionInfo->num << ")." << LogStream::eos;
		return false;
	}
	ObjID_t srcItemId = 0;

	std::string itemName = "";
	ItemQuality itemQuality = ItemQuality::ITEM_QUALITY_INVLID;
	ItemDataEntry	*pDataEntry = ItemDataEntryMgr::Instance()->FindEntry(auctionInfo->itemTypeId);
	if (pDataEntry == NULL)
	{
		ErrorLogStream << "auction(" << auctionInfo->guid << ") invalid itemTypeId(" << auctionInfo->itemTypeId << ")." << LogStream::eos;
		return false;
	}
	UInt64 auctionTransId = 0;
	
	itemQuality = (ItemQuality)pDataEntry->color;
	itemName = pDataEntry->name;

	std::string strengthenStr = "";
	if (auctionInfo->strengthen != 0)
	{
		strengthenStr = Avalon::StringUtil::ConvertToString<UInt32>(auctionInfo->strengthen);
	}

	bool copyItem = false;
	if (WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_AUCTION_COPY) && abnormalDeal && auctionInfo->strengthen < AUCTION_ABNORMAL_COPY_ITEM_MAX_STRENGTHEN && pDataEntry && !pDataEntry->isForbidAuctionCopy)
	{
		// 异常交易的情况下有几率给玩家的道具是一个服务器拷贝出来的道具
		copyItem = Avalon::Random::SelectInHundred(50);
	}

	UInt32 unitPrice = auctionInfo->price / auctionInfo->num;
	UInt32 price = unitPrice * num;
	//UInt32 counterFee = auctionInfo->counterFee / auctionInfo->num * num;
	UInt32 counterFee = GetCounterFeeNew(auctionInfo->ownerVipLev, AT_ITEM, price);

    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    // 发送者
    SysNotifyMgr::Instance()->MakeNotify(1047, buffer, sizeof(buffer));
    std::string strSenderName(buffer);

	std::string	strTitle;
    std::string	strContent;
	std::string strReason;
	//std::string itemlinkStr = "";

    WSPlayer	*pPlayer = WSPlayerMgr::Instance()->FindNormalPlayer(roleId);
    UInt32		uAccId = 0;
    if (NULL != pPlayer)
    {
        uAccId = pPlayer->GetAccId();
    }

	UInt64 mailId = 0;

	UInt32 auctionNum = 0;

    if (AUCTION_ITEM == auctionInfo->type)
    {
		std::vector<WSItem*>	vecitems;
        std::vector<ItemReward> depositItems;
		UInt32 deposit = 0;

		auctionInfo->item->SetID(auctionInfo->guid);
		srcItemId = auctionInfo->itemSrcGuid;


		if (copyItem)
		{
			try
			{
				auto item = new WSItem(*auctionInfo->item);
				item->SetID(CLRecordClient::Instance()->GenGuid());
				item->m_Num = num;
				vecitems.push_back(item);
				// 复制出来的道具，没有原始ID，相当于新的道具
				srcItemId = 0;

				InfoLogStream << "[auction_copy] copy auction(" << auctionInfo->guid << ") srcItem(" << auctionInfo->itemSrcGuid << ") num(" << ") new id(" << item->GetID() << ")." << LogStream::eos;
			}
			catch (Avalon::NetException& e)
			{
				ErrorLogStream << "copy item(" << auctionInfo->item->GetID() << ") failed, " << e.what() << LogStream::eos;
				return false;
			}

		}
		else if (num != auctionInfo->num)
		{
			// 如果不是全买，不能push物品对象
			auctionNum = 0;
			if (sendType == ASIT_BUY && pDataEntry->auctionTransNum > 0 && auctionInfo->item->m_AuctionTransNum < pDataEntry->auctionTransNum)
			{
				auctionNum = auctionInfo->item->m_AuctionTransNum + 1;
			}
			depositItems.push_back(ItemReward(auctionInfo->itemTypeId, num, 1, auctionInfo->item->GetStrengthen(), 0, 0, auctionNum));
		}
		else
		{
			/*if (sendType == ASIT_BUY)
			{
				this->SetAuctionItemCoolTime(auctionInfo->item);
			}*/
			if (sendType == ASIT_BUY && pDataEntry->auctionTransNum > 0 && auctionInfo->item->m_AuctionTransNum < pDataEntry->auctionTransNum)
			{
				auctionInfo->item->m_AuctionTransNum = auctionInfo->item->m_AuctionTransNum + 1;
				auctionNum = auctionInfo->item->m_AuctionTransNum;
			}
			vecitems.push_back(auctionInfo->item);
		}

		if (sendType == ASIT_TIMEOUT)
		{
			// 把押金退回去
			deposit = GetDepositByAuctionPrice(price);
			if (deposit > 0)
			{
				UInt32 depositItemTypeId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_DEPOSIT_ITEM_ID);
				depositItems.push_back(ItemReward(depositItemTypeId, deposit, true, 0));
			}
		}

		//MsgPackage::GetItemMsgTag(itemlinkStr, 0, auctionInfo->itemTypeId, auctionInfo->strengthen);

		if (sendType == ASIT_BUY)
		{
			SysNotifyMgr::Instance()->MakeNotify(2218, buffer, sizeof(buffer));
			strTitle = buffer;
			//SysNotifyMgr::Instance()->MakeNotify(2212, buffer, sizeof(buffer), strengthenStr.c_str(), itemName.c_str(), num, price);
			UInt32 notifyId = (auctionInfo->strengthen != 0) ? 2225 : 2212;
			SysNotifyMgr::Instance()->MakeNotify(notifyId, buffer, sizeof(buffer), strengthenStr.c_str(), itemName.c_str(), num, price);
			strContent = buffer;
			strReason = GetReason(SOURCE_TYPE_ACTION_BUY, auctionInfo->item ? auctionInfo->item->m_DataId : 0, auctionInfo->type);
		}
		else if (sendType == ASIT_CANCEL)
		{
			SysNotifyMgr::Instance()->MakeNotify(2215, buffer, sizeof(buffer));
			strTitle = buffer;
			//SysNotifyMgr::Instance()->MakeNotify(2216, buffer, sizeof(buffer), strengthenStr.c_str(), itemName.c_str(), auctionInfo->num);
			UInt32 notifyId = (auctionInfo->strengthen != 0) ? 2227 : 2216;
			SysNotifyMgr::Instance()->MakeNotify(notifyId, buffer, sizeof(buffer), strengthenStr.c_str(), itemName.c_str(), num, price);
			strContent = buffer;
			strReason = GetReason(SOURCE_TYPE_ACTION_CANCEL, auctionInfo->item ? auctionInfo->item->m_DataId : 0, auctionInfo->type);
		}
		else
		{
			SysNotifyMgr::Instance()->MakeNotify(2213, buffer, sizeof(buffer));
			strTitle = buffer;
			//SysNotifyMgr::Instance()->MakeNotify(2214, buffer, sizeof(buffer), strengthenStr.c_str(), itemName.c_str(), auctionInfo->num);
			UInt32 notifyId = (auctionInfo->strengthen != 0) ? 2226 : 2214;
			SysNotifyMgr::Instance()->MakeNotify(notifyId, buffer, sizeof(buffer), strengthenStr.c_str(), itemName.c_str(), num, price);
			strContent =  buffer;
			strReason = GetReason(SOURCE_TYPE_ACTION_TIMEOUT, auctionInfo->item ? auctionInfo->item->m_DataId : 0, auctionInfo->type);
		}

		if (sendType == ASIT_BUY)
		{
			GameInfoLogStream("Auction") << "player(" << uAccId << "," << roleId << ") buy item(" << auctionInfo->item->m_DataId << "|"
					<< num << ") from auction, price:" << price << "|" << auctionInfo->pricetype << LogStream::eos;
		}
		else
		{
			GameInfoLogStream("Auction") << "player(" << uAccId << "," << roleId << ") get returned item(" << auctionInfo->item->m_DataId << "|"
					<< num << ") from auction" << LogStream::eos;
		}

		// 直接取回背包
		if (sendType == ASIT_CANCEL || sendType == ASIT_TIMEOUT)
		{
			std::vector<MailItemReward> rewards;
			UInt32 silver = 0;

			if (sendType == ASIT_CANCEL)
			{
				for (std::vector<ItemReward>::iterator iter = depositItems.begin(); iter != depositItems.end(); ++iter)
				{
					rewards.push_back(MailItemReward(iter->id, iter->num, iter->strenth, strReason, iter->auctionCoolTimeStamp, iter->auctionTransTimes));
				}
			}

			if (auctionInfo->itemDueTime == 0 || auctionInfo->itemDueTime > (UInt32)CURRENT_TIME.Sec())
			{
			Avalon::Packet* packet = Avalon::Packet::Create();
			packet->SetID(CLProtocol::SCENE_MAIL_GIVEITEMS);
			Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
			stream & pPlayer->GetID() & silver & rewards;

			for (std::vector<WSItem*>::iterator iter = vecitems.begin(); iter != vecitems.end(); ++iter)
			{
				WSItem* item = *iter;

				try
				{
					stream & item->GetID() & item->m_DataId & strReason & item->m_SealState & item->m_SealCount;
					item->Encode(stream, SST_SCENE, false);
				}
				catch (const Avalon::NetException& exception)
				{
					ErrorLogStream << PLAYERINFO(pPlayer) << " encode items failed:" << exception.what() << "!" << LogStream::eos;
					Avalon::Packet::Destroy(packet);
					return false;
				}
			}

			stream & ObjID_t(0);
			packet->SetSize(stream.GetPos());
			pPlayer->SendToScene(packet);
			Avalon::Packet::Destroy(packet);
			}
			else //道具过期
			{
				pPlayer->SendNotify(10040);
			}

			//过期,押金邮件退回
			if (sendType == ASIT_TIMEOUT && depositItems.size() > 0)
			{
				std::vector<WSItem*>	vecitemsTemp;
				if (MailBox::SendSysMailGetMailID(roleId, strSenderName, strTitle, strContent, strReason, depositItems, vecitemsTemp, mailId) != ErrorCode::SUCCESS)
				{
					// 发送邮件失败
					ErrorLogStream << "send auction(" << auctionInfo->guid << ") mail to role(" << roleId << ") title("
						<< strTitle << ") failed." << LogStream::eos;
					return false;
				}
			}
		}
		else
		{
			if (MailBox::SendSysMailGetMailID(roleId, strSenderName, strTitle, strContent, strReason, depositItems, vecitems, mailId) != ErrorCode::SUCCESS)
			{
				// 发送邮件失败
				ErrorLogStream << "send auction(" << auctionInfo->guid << ") mail to role(" << roleId << ") title("
					<< strTitle << ") failed." << LogStream::eos;
				return false;
			}
		}

		//auctionInfo->item = NULL;
    }
    else if (AUCTION_GOLD == auctionInfo->type)
    {
        if (sendType == ASIT_BUY)
        {
            SysNotifyMgr::Instance()->MakeNotify(1042, buffer, sizeof(buffer));
            strContent = buffer;
			strReason = GetReason(SOURCE_TYPE_ACTION_BUY, auctionInfo->item ? auctionInfo->item->m_DataId : 0, auctionInfo->type);
        }
        else if (sendType == ASIT_CANCEL)
        {
            SysNotifyMgr::Instance()->MakeNotify(1045, buffer, sizeof(buffer));
            strContent = buffer;
			strReason = GetReason(SOURCE_TYPE_ACTION_CANCEL, auctionInfo->item ? auctionInfo->item->m_DataId : 0, auctionInfo->type);
        }
        else
        {
            SysNotifyMgr::Instance()->MakeNotify(1044, buffer, sizeof(buffer));
            strContent = buffer;
			strReason = GetReason(SOURCE_TYPE_ACTION_TIMEOUT, auctionInfo->item ? auctionInfo->item->m_DataId : 0, auctionInfo->type);
        }

        std::vector<ItemReward>	vecitems;
        vecitems.push_back(ItemReward(auctionInfo->itemTypeId, 1, false, 0));

		if (MailBox::SendSysMail(roleId, strSenderName, strTitle, strContent, strReason, vecitems) != ErrorCode::SUCCESS)
        {
            // 发送邮件失败
            ErrorLogStream << "send auction(" << auctionInfo->guid << ") mail to role(" << roleId << ") title("
                            << strTitle << ") failed." << LogStream::eos;
            return false;
        }
    }

    // 把拍卖的钱寄给卖家
	if (sendType == ASIT_BUY && !copyItem && !auctionInfo->isTreas)
    {
        UInt32 uGetMoneyItemId = _GetItemTypeIDByPriceType((MoneyType)auctionInfo->pricetype);
		/*
		2018.08.23 押金不退还
		by zjy
		*/
        UInt32 deposit = GetDepositByAuctionPrice(auctionInfo->price);
        std::vector<ItemReward>	vecitems;
		UInt32 money = price;
		if (money >= counterFee)
        {
			money -= counterFee;
        }
		else
		{
			money = 0;
		}

		RecordCounterFee(counterFee);

		// 东西被买光的时候把押金还给卖方
		if (num == auctionInfo->num)
		{
			//SysNotifyMgr::Instance()->MakeNotify(2211, buffer, sizeof(buffer), strengthenStr.c_str(), itemName.c_str(), num, price, counterFee, money);
			UInt32 notifyId = auctionInfo->strengthen != 0 ? 2224 : 2211;
			SysNotifyMgr::Instance()->MakeNotify(notifyId, buffer, sizeof(buffer), strengthenStr.c_str(), itemName.c_str(), num, price, counterFee, money);
			strContent = buffer;
		}
		else
		{
			//SysNotifyMgr::Instance()->MakeNotify(2219, buffer, sizeof(buffer), strengthenStr.c_str(), itemName.c_str(), num, price, counterFee, money);
			UInt32 notifyId = auctionInfo->strengthen != 0 ? 2228 : 2219;
			SysNotifyMgr::Instance()->MakeNotify(notifyId, buffer, sizeof(buffer), strengthenStr.c_str(), itemName.c_str(), num, price, counterFee, money);
			strContent = buffer;
		}

		if (money > 0)
		{
			vecitems.push_back(ItemReward(uGetMoneyItemId, money, false, 0));
		}

		SysNotifyMgr::Instance()->MakeNotify(2210, buffer, sizeof(buffer));
		strTitle = buffer;
		strReason = GetReason(SOURCE_TYPE_ACTION_SELL, auctionInfo->item ? auctionInfo->item->m_DataId : 0);

		if (MailBox::SendSysMail(auctionInfo->owner, strSenderName, strTitle, strContent, strReason, vecitems) != ErrorCode::SUCCESS)
        {
            // 发送邮件失败，这里出错就不return了
            ErrorLogStream << "send auction(" << auctionInfo->guid << ") mail to role(" << roleId << ") title("
							<< strTitle << ") money(" << money << "deposit(" << deposit << ") failed." << LogStream::eos;
        }

		// 卖家离线的时候发送推送消息
		if(WSPlayerMgr::Instance()->FindNormalPlayer(auctionInfo->owner) == NULL)
		{
			auto title = SysNotifyMgr::Instance()->MakeString(8514);
			auto content = SysNotifyMgr::Instance()->MakeString(8515);
			pushNotifyToCenterByRoleID(auctionInfo->owner, title, content);
		}
		
    }

	ObjID_t owner = auctionInfo->owner;
	//UInt32 ownerAccid = auctionInfo->account;
	ObjID_t itemId = auctionInfo->item ? auctionInfo->item->GetID() : 0;
	UInt32 itemTypeId = auctionInfo->itemTypeId;
	UInt8 strengthen = auctionInfo->strengthen;
	UInt32 beadbuffId = auctionInfo->beadBuffId;
	UInt8  enhanceType = auctionInfo->enhanceType;
	//UInt32 timeSaleToBuy = (UInt32)CURRENT_TIME.Sec() - auctionInfo->onSaleTime;
	//UInt8 oc = auctionInfo->ownerOccu;
	//bool  isFitOccu = pDataEntry && (pDataEntry->occu.test(oc) || pDataEntry->occu.test(oc - oc % 10));
	
	UInt32 averPrice = 0;
	UInt32 dealPrice = price; //成交价
	//UInt32 mPrice = 0;
	UInt32 minPriceOnSale = 0;
	UInt8 isAttented = 0;
	UInt32 itemAttentNum = 0;

	UInt32 mountMagicCardId = 0;
	UInt32 mountMagicCardAverPrice = 0;
	UInt32 mountBeadId = 0;
	UInt32 mountBeadAverPrice = 0;
	UInt32 eqQualityLv = 0;
	UInt32 remainSealCount = 0; //装备剩余封装次数

	UInt32 auctionPrice = unitPrice; //拍卖上架价格

	//在售最低物品相关信息
	UInt64 minPriceGuid = 0;				// 拍卖guid
	UInt32 minPriceOnsalePrice = 0;			// 在售价格
	UInt32 minPriceQualityLv = 0;			// 品级
	UInt32 minPriceMountBeadId = 0;			// 镶嵌宝珠的ID
	UInt32 minPriceMountBeadAverPrice = 0;	// 镶嵌宝珠的平均交易价格
	UInt32 minPriceMountMagicId = 0;		// 镶嵌的附魔卡ID
	UInt32 minPriceMountMagicAverPrice = 0; // 镶嵌附魔卡的平均交易价格

	if (auctionInfo->itemtype == ITEM_EQUIP && auctionInfo->item)
	{
		eqQualityLv = auctionInfo->item->m_QualityLv;
		remainSealCount = pDataEntry->sealMax - auctionInfo->item->m_SealCount;
		mountMagicCardId = auctionInfo->item->m_AddMagic.GetMagicCard();
		if (mountMagicCardId > 0)
		{
			UInt8 mountMagicCardLv = auctionInfo->item->m_AddMagic.GetMagicCardLev();
			mountMagicCardAverPrice = zjy_auction::AuctionMgr::Instance()->_GetAverPrice(mountMagicCardId, mountMagicCardLv, 0, 0);
			if (mountMagicCardAverPrice == 0)
			{
				mountMagicCardAverPrice = zjy_auction::AuctionMgr::Instance()->_GetRecommendPrice(mountMagicCardId, mountMagicCardLv, 0);
			}
		}

		PreciousBeadMountHole* hole = auctionInfo->item->m_PreciousBeadHoles.GetFirstMountedHole();
		if (hole) //镶嵌宝珠
		{
			mountBeadId = hole->preciousBeadId;
			mountBeadAverPrice = zjy_auction::AuctionMgr::Instance()->_GetAverPrice(hole->preciousBeadId, 0, hole->buffId, 0);
			if (mountBeadAverPrice == 0)
			{
				mountBeadAverPrice = zjy_auction::AuctionMgr::Instance()->_GetRecommendPrice(hole->preciousBeadId, 0, 0);
			}
		}
	}

	if (auctionInfo->isTreas && WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_AUCTION_TREAS)) //珍品
	{
		if (auctionInfo->itemtype == ITEM_EQUIP) //珍品装备
		{
			dealPrice = CalcTransEqMPrice(*auctionInfo);
			minPriceOnSale = _GetTreasEqMinPriceOnSale(*auctionInfo, minPriceGuid, minPriceOnsalePrice, minPriceQualityLv, 
				minPriceMountBeadId, minPriceMountBeadAverPrice, minPriceMountMagicId, minPriceMountMagicAverPrice);
		}
		else //其他珍品
		{
			minPriceOnSale = _GetMinPriceOnsale(*auctionInfo, dealPrice);
		}
		averPrice = _GetTreasAverPrice(itemTypeId, strengthen, beadbuffId, enhanceType);
		//minPriceOnSale = _GetMinPriceOnsale(itemTypeId, strengthen, beadbuffId, dealPrice);
		AuctionAttentMgr::Instance()->GetAuctionAttentInfo(*auctionInfo, roleId);
		isAttented = auctionInfo->attent;
		itemAttentNum = auctionInfo->attentNum;
	}
	else
	{
		averPrice = _GetAverPrice(itemTypeId, strengthen, beadbuffId, enhanceType);
	}

	//UInt32 averPrice = _GetAverPrice(itemTypeId, strengthen, beadbuffId);
	UInt32 sysRecomPrice = _GetRecommendPrice(itemTypeId, strengthen, enhanceType);

	//购买珍品,生成交易记录
	if (sendType == ASIT_BUY && auctionInfo->isTreas && WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_AUCTION_TREAS))
	{
		UInt32 deposit = GetDepositByAuctionPrice(price);
		auctionTransId = AuctionTransRecordMgr::Instance()->AddNewTransaction(roleId, auctionInfo, num, itemName, deposit, 
			mailId, minPriceOnSale, 0, dealPrice);

		char buffer[1024];
		memset(buffer, 0, sizeof(buffer));

		std::string strSenderName;
		std::string	strTitle;
		std::string	strContent;
		std::string strReason;
		std::vector<ItemReward> depositItems;
		std::vector<WSItem*> vecitems;

		// 发送者
		SysNotifyMgr::Instance()->MakeNotify(9960, buffer, sizeof(buffer));
		strSenderName = buffer;

		SysNotifyMgr::Instance()->MakeNotify(9959, buffer, sizeof(buffer));
		strTitle = buffer;

		//std::string itemlinkStr;
		//MsgPackage::GetItemMsgTag(itemlinkStr, 0, auctionInfo->item ? auctionInfo->item->GetDataID() : 0, auctionInfo->strengthen);

		SysNotifyMgr::Instance()->MakeNotify(9958, buffer, sizeof(buffer), itemName.c_str(),
			TimeUtil::TimestampToStandarFormatNoSpace((UInt32)CURRENT_TIME.Sec()), price);
		strContent = buffer;

		strReason = GetReason(SOURCE_TYPE_ACTION_BUY, auctionInfo->guid);

		if (MailBox::SendSysMail(auctionInfo->owner, strSenderName, strTitle, strContent, strReason, depositItems, vecitems) != ErrorCode::SUCCESS)
		{
			// 发送邮件失败
			ErrorLogStream << "send aution treas transction fail (" << auctionInfo->guid << ") mail to role(" << roleId << ") title("
				<< strTitle << ") failed." << LogStream::eos;
		}
	}

	_OnSendAuctionToPlayer(pPlayer, sendType, (AuctionType)auctionInfo->type, abnormalDeal,
		owner, auctionInfo->ownerVipLev, srcItemId, itemId, auctionInfo->mainType, itemTypeId, num,
		(MoneyType)auctionInfo->pricetype, dealPrice, counterFee, strengthen, averPrice,
		auctionInfo->onSaleTime, itemQuality, auctionInfo->ip, auctionInfo->deviceId,
		auctionInfo->account, auctionInfo->isTreas, sysRecomPrice, minPriceOnSale, auctionTransId,
		isAttented, itemAttentNum, mountMagicCardId, mountMagicCardAverPrice,
		mountBeadId, mountBeadAverPrice, eqQualityLv, remainSealCount,
		minPriceGuid, minPriceOnsalePrice, minPriceQualityLv, minPriceMountBeadId,
		minPriceMountBeadAverPrice, minPriceMountMagicId, minPriceMountMagicAverPrice, auctionPrice,
		auctionNum, pDataEntry->auctionTransNum);

	InfoLogStream << "player id : " << pPlayer->GetID() << ", dealPrice : " << dealPrice << ", isTreas : " << auctionInfo->isTreas << ", trans id : " << auctionTransId << LogStream::eos;

	if (!copyItem)
	{
		if (sendType == ASIT_CANCEL)
		{
			OnMagicOffsale(*auctionInfo);
		}
		else if (sendType == ASIT_BUY)
		{
			OnMagicBuy(*auctionInfo, num);
		}

		if (num == auctionInfo->num)
		{
			if (sendType != ASIT_CANCEL)
			{
				auctionInfo->item = NULL;
			}
			if (auctionInfo->isTreas && auctionInfo->buyState != AGBS_NORMAL_BUY)
			{
				if ((auctionInfo->buyState == AGBS_RUSH_BUY || auctionInfo->buyState == AGBS_RUSH_BUY_DELIVER) && sendType == ASIT_CANCEL)
				{
					for (UInt32 i = 0; i < auctionInfo->rushBuyPlayers.size(); ++i)
					{
						WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(auctionInfo->rushBuyPlayers[i].playerId);
						if (player)
						{
							player->SendNotify(9947); //卖家下架,抢购失败
							/*CLProtocol::WorldAuctionNotifyRefresh refresh;
							refresh.type = auctionInfo->type;
							refresh.reason = SRR_CANCEL;
							player->SendProtocol(refresh);*/
						}
					}
				}
				else if (auctionInfo->buyState == AGBS_RUSH_BUY_DELIVER && sendType == ASIT_BUY)
				{
					for (UInt32 i = 0; i < auctionInfo->rushBuyPlayers.size(); ++i)
					{
						WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(auctionInfo->rushBuyPlayers[i].playerId);
						if (player && roleId != player->GetID())
						{
							player->SendNotify(9945); //
							CLProtocol::WorldAuctionNotifyRefresh refresh;
							refresh.type = auctionInfo->type;
							refresh.reason = SRR_RUSY_BUY;
							refresh.auctGuid = auctionInfo->guid;
							player->SendProtocol(refresh);
						}
					}
				}
				//this->RemoveOneTreas(auctionInfo);
			}

			_DeleteAuctionInfo(auctionInfo);
		}
		else
		{
			// 修改道具数量和价格
			UInt32 remainNum = auctionInfo->num - num;
			auctionInfo->num = remainNum;
			auctionInfo->price -= price;
			auctionInfo->counterFee -= counterFee;
			auctionInfo->item->m_Num = remainNum;

			// 保存到数据库
			CLUpdateCommand *pCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_auction_new", auctionInfo->guid, true);
			pCmd->PutData("num", auctionInfo->num);
			pCmd->PutData("price", auctionInfo->price);
			pCmd->PutData("counter_fee", auctionInfo->counterFee);
			CLRecordClient::Instance()->SendCommand(pCmd);
		}
	}
	
    return true;
}

WSItem* AuctionMgr::_FindAuctionItem(ObjID_t guid)
{
    auto auction = _FindAuction(guid);
    if (!auction)
    {
        return NULL;
    }

    return auction->item;
}

AuctionInfo* AuctionMgr::_FindAuction(ObjID_t guid)
{
    auto itr = m_auctiones.find(guid);
    return itr == m_auctiones.end() ? NULL : itr->second;
}

void AuctionMgr::_DeleteAuctionInfo(AuctionInfo *auctionInfo)
{
	if (auctionInfo->isTreas && WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_AUCTION_TREAS))
	{
		OnTreasOffSale(*auctionInfo);
	}
	m_auctiones.erase(auctionInfo->guid);
	this->RemoveOneTreas(auctionInfo);
	if (WSApplication::Instance()->IsAuctionOpUseMem())
	{
		this->RemoveAuctionForMemQuery(auctionInfo);
	}
	
    WSPlayer	*pPlayer = WSPlayerMgr::Instance()->FindNormalPlayer(auctionInfo->owner);
    if (NULL != pPlayer)
    {
        pPlayer->DelAuction((AuctionType)auctionInfo->type, auctionInfo->guid);
        CLProtocol::WorldAuctionDel	protocol;
        protocol.id = auctionInfo->guid;
        pPlayer->SendProtocol(protocol);
    }

    CLDeleteCommand	*pCmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_auction_new", auctionInfo->guid);
    CLRecordClient::Instance()->SendCommand(pCmd);

    delete auctionInfo;
}

UInt32 AuctionMgr::_GetCounterFeeByAuctionType(WSPlayer* player, AuctionType type, UInt32 price)
{
	if (!player)
	{
		return 0;
	}

	auto rate = player->GetVipValue(VIP_AUCTION_COUNTER_FEE);
	return (UInt32)((UInt64)price * rate / 1000);
}

UInt32 AuctionMgr::GetCounterFeeNew(UInt8 vipLvl, AuctionType type, UInt32 price)
{
	auto rate = VipPrivilegeDataEntryMgr::Instance()->FindPrivilegeValue(VIP_AUCTION_COUNTER_FEE, vipLvl);
	return (UInt32)((UInt64)price * rate / 1000);
}

UInt32 AuctionMgr::_GetAuctionDuetime(ItemDataEntry* dataEntry)
{
    /*if (duration == ASD_24_HOUR)
    {
        return UInt32(CURRENT_TIME.Sec() + DAY_TO_SEC);
    }
    else if (duration == ASD_48_HOUR)
    {
        return UInt32(CURRENT_TIME.Sec() + 2 * DAY_TO_SEC);
    }*/

	if (!dataEntry) return 0;
	if (dataEntry->isTreasure == 1 && WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_AUCTION_TREAS))
	{
		return UInt32(CURRENT_TIME.Sec() + SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_TREAS_ONSALE_TIME) * HOUR_TO_SEC);
	}
	else 
	{
		return UInt32(CURRENT_TIME.Sec() + SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_NOT_TREAS_ONSALE_TIME) * HOUR_TO_SEC);
	}
}

UInt32 AuctionMgr::_GetItemTypeIDByPriceType(MoneyType type)
{
    if (type == MONEY_GOLD)
    {
		return GOLD_ITEM_ID;
    }
    else if (type == MONEY_POINT)
    {
		return BIND_POINT_ITEM_ID;
    }

    return 0;
}

UInt32 AuctionMgr::_GetRecommendPrice(UInt32 itemTypeID, UInt8 strengthen, UInt8 enhanceType)
{
	auto itemData = ItemDataEntryMgr::Instance()->FindEntry(itemTypeID);
	if (!itemData)
	{
		ErrorLogStream << "get recommend price not find data entry id : " << itemTypeID << LogStream::eos;
		return 0;
	}

	if (itemData->subType == ST_MAGICCARD)	//附魔卡
	{
		return _GetMagicRecommendPrice(itemTypeID, strengthen, itemData);
	}

	UInt32 addition = 0;
	if (enhanceType > ENHANCE_INVALID) //红字装备
	{
		addition = AuctionMagicStrengAdditDataEntryMgr::Instance()->GetAddition(itemData->color, strengthen, AMSTP_ZENFU);
	}
	//else if (itemData->subType == ST_MAGICCARD) //附魔卡
	//{
	//	addition = AuctionMagicStrengAdditDataEntryMgr::Instance()->GetAddition(itemData->color, strengthen, AMSTP_MAGIC);
	//}
	else
	{
		addition = GetAuctionStrengthAddition(strengthen);
	}

	return (UInt32)((UInt64)itemData->recommendPrice * (100 + addition) / 100);
}

UInt32 AuctionMgr::_GetMagicRecommendPrice(UInt32 itemTypeID, UInt8 strengthen, ItemDataEntry* itemData)
{
	if (!itemData)
	{
		ErrorLogStream << "get magic recommend price not find data entry id : " << itemTypeID << LogStream::eos;
		return 0;
	}
	if (strengthen == 0)
	{
		return itemData->recommendPrice;
	}

	UInt32 recommendPrice = 0;

	if (itemData->isSpeicMagic == 1)	//特殊附魔卡(升级消耗自身类型卡)
	{
		UInt32 expectUpTimes = AuctionMagicExpectUpDataEntryMgr::Instance()->GetExpectUpTimes(itemData->color, strengthen, MUCT_CONSELF);
		recommendPrice = itemData->recommendPrice + (UInt64)(itemData->recommendPrice) * expectUpTimes / 100;
	}
	else
	{
		UInt32 expectUpTimes = AuctionMagicExpectUpDataEntryMgr::Instance()->GetExpectUpTimes(itemData->color, strengthen, MUCT_NORMAL);
		UInt32 minAverPrice = _GetMagicMinAverPriceByQuality(itemData->color);
		recommendPrice = itemData->recommendPrice + (UInt64)minAverPrice * expectUpTimes / 100;
	}

	return recommendPrice;

	////附魔卡可见平均交易价格
	//UInt32 visAverPrice = 0;

	////strengthen = GetStrengthenForAverPrice(*itemData, strengthen, ENHANCE_INVALID);
	//AuctionRecommendPriceKey key(itemTypeID, 0, 0, ENHANCE_INVALID);
	//auto itr = m_recommendPrice.find(key);
	//if (itr == m_recommendPrice.end())
	//{
	//	visAverPrice = itemData->recommendPrice;
	//}
	//else 
	//{
	//	auto recommend = itr->second;
	//	visAverPrice = recommend != NULL ? recommend->visibility_price : itemData->recommendPrice;
	//}

	//if (itemData->isSpeicMagic == 1)	//特殊附魔卡
	//{
	//	UInt32 expectUpTimes = AuctionMagicExpectUpDataEntryMgr::Instance()->GetExpectUpTimes(itemData->color, strengthen);
	//	recommendPrice = visAverPrice + visAverPrice * (float)expectUpTimes / 100;
	//}
	//else
	//{
	//	UInt32 expectUpTimes = AuctionMagicExpectUpDataEntryMgr::Instance()->GetExpectUpTimes(itemData->color, strengthen);
	//	UInt32 minAverPrice = _GetMagicMinAverPriceByQuality(itemData->color);
	//	minAverPrice = (minAverPrice == 0) ? visAverPrice : minAverPrice;
	//	recommendPrice = visAverPrice + minAverPrice * (float)expectUpTimes / 100;
	//}

	//return recommendPrice;
}

UInt32 AuctionMgr::_GetMagicMinAverPriceByQuality(UInt8 quality)
{
	auto itr = m_magicMinAverPrice.find(quality);
	if (itr == m_magicMinAverPrice.end())
	{
		ErrorLogStream << "not find magic min aver price quality : " << quality << LogStream::eos;
		return 0;
	}

	return itr->second;
}

void AuctionMgr::_CalcMagicMinAverPriceByQuality()
{
	const std::map<UInt8, std::vector<UInt32>>& magicIdMap = ItemDataEntryMgr::Instance()->GetMagicIds();
	for (auto item : magicIdMap)
	{
		UInt8 quality = item.first;
		std::vector<UInt32>& magicIds = item.second;
		UInt32 minAverPrice = 0;

		for (auto magicId : magicIds)
		{
			UInt32 averPrice = _GetRealAverPrice(magicId, 0, 0, ENHANCE_INVALID);
			if (averPrice == 0)
			{
				auto itemData = ItemDataEntryMgr::Instance()->FindEntry(magicId);
				if (!itemData)
				{
					ErrorLogStream << "calc magic min aver price error, not find magic data entry  magicId: " << magicId << LogStream::eos;
					continue;
				}
				averPrice = itemData->recommendPrice;
			}
			if (minAverPrice == 0 || averPrice < minAverPrice)
			{
				minAverPrice = averPrice;
			}
		}
		m_magicMinAverPrice[quality] = minAverPrice;
	}
}

UInt32 AuctionMgr::_GetMinPriceOnsale(AuctionInfo& auctionInfo, UInt32 selfPrice)
{
	UInt8 strengthen = GetStrengthenForAverPrice(auctionInfo);
	AuctionRecommendPriceKey key(auctionInfo.itemTypeId, strengthen, auctionInfo.beadBuffId, auctionInfo.GetEnhanceType());
	std::map<AuctionRecommendPriceKey, MapPriceNum>::iterator iter = m_onSaleTreaPrices.find(key);
	if (iter == m_onSaleTreaPrices.end())
	{
		return 0;
	}
	else{
		MapPriceNum& prices = iter->second;
		for (auto priceItem : prices)
		{
			if (priceItem.first != selfPrice)
			{
				return priceItem.first;
			}
		}
		return 0;
	}
}

UInt32 AuctionMgr::_GetTreasEqMinPriceOnSale(AuctionInfo& auctionInfo, UInt64& minPriceGuid, UInt32& minPriceOnsalePrice, UInt32& minPriceQualityLv,
	UInt32& minPriceMountBeadId, UInt32& minPriceMountBeadAverPrice, UInt32& minPriceMountMagicId, UInt32& minPriceMountMagicAverPrice)
{
	InfoLogStream << "get treas eq min price auction guid : " << auctionInfo.guid << ", item type id : " << auctionInfo.itemTypeId << LogStream::eos;
	UInt8 strengthen = GetStrengthenForAverPrice(auctionInfo);
	AuctionRecommendPriceKey key(auctionInfo.itemTypeId, strengthen, auctionInfo.beadBuffId, auctionInfo.GetEnhanceType());
	MapAuction& auctions = m_onSaleTreaEqAuctions[key];
	UInt32 minMPrice = 0xFFFFFFFF;
	AuctionInfo* minPriceAuction = NULL;
	HashMap<ObjID_t, AuctionInfo*>::iterator iter = auctions.begin();
	for (; iter != auctions.end(); ++iter)
	{
		AuctionInfo* pAuctionInfo = iter->second;
		if (!pAuctionInfo)
		{
			continue;
		}
		if (pAuctionInfo->itemTypeId != auctionInfo.itemTypeId)
		{
			InfoLogStream << "calc trans eq m price, item type id is err, err guid : " << pAuctionInfo->guid << ", err item type id : " << pAuctionInfo->itemTypeId 
				<< ", address of err object : " << (UInt64)(pAuctionInfo) << ", auction item type id : " << auctionInfo.itemTypeId  << LogStream::eos;
			continue;
		}
		if (pAuctionInfo->guid == auctionInfo.guid)
		{
			continue;
		}

		UInt32 mPrice = CalcTransEqMPrice(*pAuctionInfo);
		if (mPrice < minMPrice)
		{
			minPriceAuction = pAuctionInfo;
			minMPrice = mPrice;
		}	
	}
	minMPrice = minMPrice < 0xFFFFFFFF ? minMPrice : 0;
	if (minPriceAuction)
	{
		minPriceGuid = minPriceAuction->guid;
		minPriceOnsalePrice = minPriceAuction->price;
		minPriceQualityLv = (minPriceAuction->item) ? minPriceAuction->item->m_QualityLv : 0;
		PreciousBeadMountHole* hole = (minPriceAuction->item) ? minPriceAuction->item->m_PreciousBeadHoles.GetFirstMountedHole() : NULL;
		if (hole)
		{
			minPriceMountBeadId = hole->preciousBeadId;
			minPriceMountBeadAverPrice = zjy_auction::AuctionMgr::Instance()->_GetAverPrice(hole->preciousBeadId, 0, hole->buffId, 0);
			if (minPriceMountBeadAverPrice == 0)
			{
				minPriceMountBeadAverPrice = zjy_auction::AuctionMgr::Instance()->_GetRecommendPrice(hole->preciousBeadId, 0, 0);
			}
		}

		minPriceMountMagicId = (minPriceAuction->item) ? minPriceAuction->item->m_AddMagic.GetMagicCard() : 0;
		if (minPriceMountMagicId > 0)
		{
			UInt8 mountMagicCardLv = minPriceAuction->item->m_AddMagic.GetMagicCardLev();
			minPriceMountMagicAverPrice = zjy_auction::AuctionMgr::Instance()->_GetAverPrice(minPriceMountMagicId, mountMagicCardLv, 0, 0);
			if (minPriceMountMagicAverPrice == 0)
			{
				minPriceMountMagicAverPrice = zjy_auction::AuctionMgr::Instance()->_GetRecommendPrice(minPriceMountMagicId, mountMagicCardLv, 0);
			}
		}
	}
	return minMPrice;
}

void AuctionMgr::OnTreasOnSale(AuctionInfo& auctionInfo)
{
	if (auctionInfo.itemtype == ITEM_EQUIP)	//珍品装备
	{
		OnTreasEqOnSale(auctionInfo);
	}
	else //珍品非装备
	{
		AddTreasOnsalePrice(auctionInfo);
	}
}

void AuctionMgr::OnTreasEqOnSale(AuctionInfo& auctionInfo)
{
	UInt8 strengthen = GetStrengthenForAverPrice(auctionInfo);

	DebugLogStream << "treas on sale guid : " << auctionInfo.guid << " , itemTypeId : " << auctionInfo.itemTypeId << " , strengthen : " << strengthen
		<< " , beadBuffId : " << auctionInfo.beadBuffId << " , price : " << auctionInfo.price << ", enhance type : " << auctionInfo.GetEnhanceType() << LogStream::eos;
	if (auctionInfo.goodState != AGS_ONSALE)
	{
		DebugLogStream << "auctionInfo.goodState err guid : " << auctionInfo.guid << ", goodState : " << auctionInfo.goodState << LogStream::eos;
		return;
	}

	if (auctionInfo.itemtype != ITEM_EQUIP)
	{
		DebugLogStream << "auctionInfo.itemtype err guid : " << auctionInfo.guid << ", itemtype : " << auctionInfo.itemtype << LogStream::eos;
		return;
	}

	if (auctionInfo.duetime <= (UInt32)CURRENT_TIME.Sec())
	{
		DebugLogStream << "duetime <= curtime, auctionInfo.guid : " << auctionInfo.guid << "auctionInfo.duetime : " << auctionInfo.duetime << LogStream::eos;
		return;
	}

	AuctionRecommendPriceKey key(auctionInfo.itemTypeId, strengthen, auctionInfo.beadBuffId, auctionInfo.GetEnhanceType());
	MapAuction& auctions = m_onSaleTreaEqAuctions[key];
	auctions[auctionInfo.guid] = &auctionInfo;
	InfoLogStream << "add treas eq on sale price : " << auctionInfo.guid << " , itemTypeId : " << auctionInfo.itemTypeId << ", object address : " << (UInt64)(&auctionInfo) << LogStream::eos;
}

void AuctionMgr::AddTreasOnsalePrice(AuctionRecommendPriceKey &key, UInt32 price)
{
	if (price == 0) return;
	std::map<AuctionRecommendPriceKey, MapPriceNum>::iterator iter = m_onSaleTreaPrices.find(key);
	if (iter == m_onSaleTreaPrices.end())
	{
		m_onSaleTreaPrices[key].insert(std::pair<UInt32, UInt32>(price, 1));
	}
	else
	{
		MapPriceNum& prices = iter->second;
		prices[price] = prices[price] + 1;
	}
}

void AuctionMgr::AddTreasOnsalePrice(AuctionInfo& auctionInfo)
{
	if (auctionInfo.goodState != AGS_ONSALE)
	{
		return;
	}

	if (auctionInfo.itemtype == ITEM_EQUIP) //珍品装备
	{
		return;
	}

	if (auctionInfo.duetime <= (UInt32)CURRENT_TIME.Sec())
	{
		return;
	}
	UInt8 strengthen = GetStrengthenForAverPrice(auctionInfo);

	AuctionRecommendPriceKey key(auctionInfo.itemTypeId, strengthen, auctionInfo.beadBuffId, auctionInfo.GetEnhanceType());
	UInt32 unitPrice = 0;
	if (auctionInfo.num != 0)
	{
		unitPrice = auctionInfo.price / auctionInfo.num;
	}
	AddTreasOnsalePrice(key, unitPrice);
}

void AuctionMgr::RemoveTreasOnsalePrice(AuctionRecommendPriceKey &key, UInt32 price)
{
	std::map<AuctionRecommendPriceKey, MapPriceNum>::iterator iter = m_onSaleTreaPrices.find(key);
	if (iter != m_onSaleTreaPrices.end())
	{
		MapPriceNum& prices = iter->second;
		if (prices[price] >= 1)
		{
			prices[price] = prices[price] - 1;
			if (prices[price] == 0)
			{
				prices.erase(price);
			}
		}
	}
}

void AuctionMgr::OnTreasOffSale(AuctionInfo& auctionInfo)
{
	DebugLogStream << "treas off sale guid : " << auctionInfo.guid << " , itemTypeId : " << auctionInfo.itemTypeId << " , strengthen : " << auctionInfo.strengthen
		<< " , beadBuffId : " << auctionInfo.beadBuffId << " , price : " << auctionInfo.price << LogStream::eos;
	if (!auctionInfo.isTreas)
	{
		ErrorLogStream << " !auctionInfo.isTreas " << LogStream::eos;
		return;
	}
	if (auctionInfo.itemtype == ITEM_EQUIP) //珍品装备
	{
		OnTreasEqOffSale(auctionInfo);
	}
	else //珍品非装备
	{
		OnTreasNoEqOffSale(auctionInfo);
	}
	AuctionAttentMgr::Instance()->OnAuctionOffSale(auctionInfo);
}

void AuctionMgr::OnTreasEqOffSale(AuctionInfo& auctionInfo)
{
	UInt8 strengthen = GetStrengthenForAverPrice(auctionInfo);

	DebugLogStream << "treas off sale guid : " << auctionInfo.guid << " , itemTypeId : " << auctionInfo.itemTypeId << " , strengthen : " << strengthen
		<< ", enhance type : " << (UInt32)auctionInfo.GetEnhanceType() << " , beadBuffId : " << auctionInfo.beadBuffId << " , price : " << auctionInfo.price 
		<< "address of object : " << (UInt64)&auctionInfo << LogStream::eos;
	if (auctionInfo.goodState != AGS_ONSALE)
	{
		ErrorLogStream << " auctionInfo.goodState != AGS_ONSALE, guid : " << auctionInfo.guid << ", goodState : " << (UInt32)auctionInfo.goodState 
			<< "address of object : " << (UInt64)&auctionInfo << LogStream::eos;
		return;
	}

	AuctionRecommendPriceKey key(auctionInfo.itemTypeId, strengthen, auctionInfo.beadBuffId, auctionInfo.GetEnhanceType());
	MapAuction& auctions = m_onSaleTreaEqAuctions[key];
	auto iter = auctions.find(auctionInfo.guid);
	if (iter != auctions.end())
	{
		DebugLogStream << "erase auction from onsales guid : " << auctionInfo.guid << "item type id : " << auctionInfo.itemTypeId 
			<< "address of object : " << (UInt64)iter->second <<  LogStream::eos;
		auctions.erase(auctionInfo.guid);
	}
	else
	{
		DebugLogStream << "erase auction from onsales err not find auction, guid : " << auctionInfo.guid << "item type id : " << auctionInfo.itemTypeId
			<< "address of object : " << (UInt64)&auctionInfo << LogStream::eos;
	}
}

void AuctionMgr::OnTreasNoEqOffSale(AuctionInfo& auctionInfo)
{
	if (auctionInfo.goodState == AGS_ONSALE && auctionInfo.num != 0)
	{
		UInt32 uintPrice = auctionInfo.price / auctionInfo.num;
		UInt8 strengthen = GetStrengthenForAverPrice(auctionInfo);
		AuctionRecommendPriceKey key(auctionInfo.itemTypeId, strengthen, auctionInfo.beadBuffId, auctionInfo.GetEnhanceType());
		this->RemoveTreasOnsalePrice(key, uintPrice);
	}
}

UInt32 AuctionMgr::_GetAverPrice(UInt32 itemTypeID, UInt8 strengthen, UInt32 buffId, UInt8 enhanceType)
{
	auto itemdata = ItemDataEntryMgr::Instance()->FindEntry(itemTypeID);
	if (!itemdata)
	{
		ErrorLogStream << "can't find item(" << itemTypeID << ")..." << LogStream::eos;
		return 0;
	}

	//强化+N的附魔卡
	if (itemdata->subType == ST_MAGICCARD && strengthen > 0)
	{
		UInt32 averPrice = 0;
		//对应+0的可见平均交易价格
		UInt32 averPrice_temp = 0;
		AuctionRecommendPriceKey key(itemTypeID, 0, buffId, enhanceType);
		auto it = m_recommendPrice.find(key);
		if (it == m_recommendPrice.end())
		{
			averPrice_temp = itemdata->recommendPrice;
		}
		else
		{
			auto recommend = it->second;
			averPrice_temp = recommend != NULL ? recommend->visibility_price : itemdata->recommendPrice;
		}

		if (itemdata->isSpeicMagic == 1)   //特殊附魔卡(升级只消耗自身类型)
		{
			//期望升级次数
			UInt32 expectUpTimes = AuctionMagicExpectUpDataEntryMgr::Instance()->GetExpectUpTimes(itemdata->color, strengthen, MUCT_CONSELF);
			averPrice = averPrice_temp + (UInt64)averPrice_temp * expectUpTimes / 100;
		}
		else
		{
			//同品质附魔卡中实时平均交易价格最低的价格
			UInt32 minAverPrice = _GetMagicMinAverPriceByQuality(itemdata->color);
			//期望升级次数
			UInt32 expectUpTimes = AuctionMagicExpectUpDataEntryMgr::Instance()->GetExpectUpTimes(itemdata->color, strengthen, MUCT_NORMAL);
			averPrice = averPrice_temp + (UInt64)minAverPrice * expectUpTimes / 100;
		}
		return averPrice;
	}
	else
	{
		strengthen = GetStrengthenForAverPrice(*itemdata, strengthen, enhanceType);

		AuctionRecommendPriceKey key(itemTypeID, strengthen, buffId, enhanceType);
		auto itr = m_recommendPrice.find(key);
		if (itr == m_recommendPrice.end())
		{
			return _GetRecommendPrice(itemTypeID, strengthen, enhanceType);
		}

		auto recommend = itr->second;
		return recommend != NULL ? recommend->price : _GetRecommendPrice(itemTypeID, strengthen, enhanceType);
	}
}


UInt32 AuctionMgr::_GetRealAverPrice(UInt32 itemTypeID, UInt8 strengthen, UInt32 buffId, UInt8 enhanceType)
{
	auto itemdata = ItemDataEntryMgr::Instance()->FindEntry(itemTypeID);
	if (!itemdata)
	{
		ErrorLogStream << "can't find item(" << itemTypeID << ")..." << LogStream::eos;
		return 0;
	}

	//强化+N的附魔卡
	if (itemdata->subType == ST_MAGICCARD && strengthen > 0)
	{
		UInt32 averPrice = 0;
		//对应+0的可见平均交易价格
		UInt32 averPrice_temp = 0;
		AuctionRecommendPriceKey key(itemTypeID, 0, buffId, enhanceType);
		auto it = m_recommendPrice.find(key);
		if (it == m_recommendPrice.end())
		{
			averPrice_temp = itemdata->recommendPrice;
		}
		else
		{
			auto recommend = it->second;
			averPrice_temp = recommend != NULL ? recommend->visibility_price : itemdata->recommendPrice;
		}

		if (itemdata->isSpeicMagic == 1)   //特殊附魔卡(升级只消耗自身类型)
		{
			//期望升级次数
			UInt32 expectUpTimes = AuctionMagicExpectUpDataEntryMgr::Instance()->GetExpectUpTimes(itemdata->color, strengthen, MUCT_CONSELF);
			averPrice = averPrice_temp + (UInt64)averPrice_temp * expectUpTimes / 100;
		}
		else
		{
			//同品质附魔卡中实时平均交易价格最低的价格
			UInt32 minAverPrice = _GetMagicMinAverPriceByQuality(itemdata->color);
			//期望升级次数
			UInt32 expectUpTimes = AuctionMagicExpectUpDataEntryMgr::Instance()->GetExpectUpTimes(itemdata->color, strengthen, MUCT_NORMAL);
			averPrice = averPrice_temp + (UInt64)minAverPrice * expectUpTimes / 100;
		}
		return averPrice;
	}
	else
	{
		strengthen = GetStrengthenForAverPrice(*itemdata, strengthen, enhanceType);

		AuctionRecommendPriceKey key(itemTypeID, strengthen, buffId, enhanceType);
		auto itr = m_recommendPrice.find(key);
		if (itr == m_recommendPrice.end())
		{
			return 0;
		}

		auto recommend = itr->second;
		return recommend != NULL ? recommend->price : 0;
	}
}

UInt32 AuctionMgr::_GetTreasAverPrice(UInt32 itemTypeID, UInt8 strengthen, UInt32 buffId, UInt8 enhanceType)
{
	auto itemdata = ItemDataEntryMgr::Instance()->FindEntry(itemTypeID);
	if (!itemdata)
	{
		ErrorLogStream << "can't find item(" << itemTypeID << ")..." << LogStream::eos;
		return 0;
	}

	strengthen = GetStrengthenForAverPrice(*itemdata, strengthen, enhanceType);

	AuctionRecommendPriceKey key(itemTypeID, strengthen, buffId, enhanceType);
	auto itr = m_recommendPrice.find(key);
	if (itr == m_recommendPrice.end())
	{
		return 0;
	}

	auto recommend = itr->second;
	return recommend != NULL ? recommend->price : 0;
}

bool AuctionMgr::_GetVisAverPrice(UInt32 itemTypeID, UInt8 strengthen, UInt32 buffId, UInt8 enhanceType,  UInt32& visAverPrice)
{
	auto itemdata = ItemDataEntryMgr::Instance()->FindEntry(itemTypeID);
	if (!itemdata)
	{
		ErrorLogStream << "can't find item(" << itemTypeID << ")..." << LogStream::eos;
		return 0;
	}
	
	//强化+N的附魔卡
	if (itemdata->subType == ST_MAGICCARD && strengthen > 0)
	{
		//对应+0的可见平均交易价格
		UInt32 averPrice_temp = 0;
		AuctionRecommendPriceKey key(itemTypeID, 0, buffId, enhanceType);
		auto it = m_recommendPrice.find(key);
		if (it == m_recommendPrice.end())
		{
			averPrice_temp = itemdata->recommendPrice;
		}
		else
		{
			auto recommend = it->second;
			averPrice_temp = recommend != NULL ? recommend->visibility_price : itemdata->recommendPrice;
			if (averPrice_temp == 0)
			{
				averPrice_temp = itemdata->recommendPrice;
			}
		}

		if (itemdata->isSpeicMagic == 1)   //特殊附魔卡(升级只消耗自身类型)
		{
			//期望升级次数
			UInt32 expectUpTimes = AuctionMagicExpectUpDataEntryMgr::Instance()->GetExpectUpTimes(itemdata->color, strengthen, MUCT_CONSELF);
			visAverPrice = averPrice_temp + (UInt64)averPrice_temp * expectUpTimes / 100;
		}
		else
		{
			//同品质附魔卡中实时平均交易价格最低的价格
			UInt32 minAverPrice = _GetMagicMinAverPriceByQuality(itemdata->color);
			//期望升级次数
			UInt32 expectUpTimes = AuctionMagicExpectUpDataEntryMgr::Instance()->GetExpectUpTimes(itemdata->color, strengthen, MUCT_NORMAL);
			visAverPrice = averPrice_temp + (UInt64)minAverPrice * expectUpTimes / 100;
		}
		return true;
	}
	else
	{
		strengthen = GetStrengthenForAverPrice(*itemdata, strengthen, enhanceType);

		AuctionRecommendPriceKey key(itemTypeID, strengthen, buffId, enhanceType);
		auto itr = m_recommendPrice.find(key);
		if (itr == m_recommendPrice.end())
		{
			visAverPrice = _GetRecommendPrice(itemTypeID, strengthen, enhanceType);
			return false;
		}

		auto recommend = itr->second;
		//return recommend != NULL ? recommend->visibility_price : _GetRecommendPrice(itemTypeID, strengthen);
		visAverPrice = recommend != NULL ? recommend->visibility_price : _GetRecommendPrice(itemTypeID, strengthen, enhanceType);
		if (visAverPrice == 0)
		{
			visAverPrice = _GetRecommendPrice(itemTypeID, strengthen, enhanceType);
			return false;
		}
		return true;
	}
}

UInt32 AuctionMgr::GetGoldItemByNum(UInt32 num)
{
    struct GoldItemInfo
    {
        UInt32 itemId;
        UInt32 millon;
    };

    static GoldItemInfo s_goldItemInfoes[] = 
        {
            { 800000011, 1 },
            { 800000012, 2 },
            { 800000013, 3 },
            { 800000014, 4 },
            { 800000015, 5 },
            { 800000016, 6 },
            { 800000017, 7 },
            { 800000018, 8 },
            { 800000019, 9 },
            { 800000020, 10 },
            { 800000021, 20 },
            { 800000022, 30 },
            { 800000023, 40 },
            { 800000024, 50 },
            { 800000025, 100 },
        };

    if (num % GOLD_BASE_NUM)
    {
        return 0;
    }

    UInt32 millon = num / GOLD_BASE_NUM;
    for (auto& info : s_goldItemInfoes)
    {
        if (millon == info.millon)
        {
            return info.itemId;
        }
    }

    return 0;
}

UInt32 AuctionMgr::GetDepositByAuctionPrice(UInt32 price)
{
	// 千分比
	UInt32 uplimit = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_DEPOSIT_COST_UPLIMIT);
	UInt32 rate = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_DEPOSIT_COST);
	UInt64 deposit = (UInt64)rate * price / 1000;
	if ((UInt32)deposit > uplimit)
	{
		return uplimit;
	}
	return deposit > 0 ? deposit : 1;
}

void AuctionMgr::_RecordDealPrice(AuctionInfo* auctionInfo, UInt32 beadBuffId, UInt32 price, UInt32 num)
{
	if (!auctionInfo) return;

	UInt32 itemTypeID = auctionInfo->itemTypeId;

	AuctionNoTreasAverPriceMinNumDataEntry* entry = AuctionNoTreasAverPriceMinNumDataEntryMgr::Instance()->FindEntry(itemTypeID);
	if (entry && num < entry->minNum)
	{
		return;
	}

	UInt8  strengthen = GetStrengthenForAverPrice(*auctionInfo);
	UInt32 qualitylv = 0;
	UInt32 beadAverPrice = 0;
	UInt32 magicCardAverPrice = 0;
	if (auctionInfo->isTreas)
	{
		GetBeadAndMagicCardAverPrice(auctionInfo, beadAverPrice, magicCardAverPrice);
		qualitylv = auctionInfo->item->m_QualityLv;
	}
	
	AuctionRecommendPrice* recommend = NULL;
	AuctionRecommendPriceKey key(itemTypeID, strengthen, beadBuffId, auctionInfo->GetEnhanceType());

	auto itr = m_recommendPrice.find(key);
	if (itr == m_recommendPrice.end() || !itr->second)
	{
		recommend = new AuctionRecommendPrice();
		recommend->guid = CLRecordClient::Instance()->GenGuid();
		recommend->itemTypeID = itemTypeID;
		recommend->strengthen = strengthen;
		recommend->bead_buff_id = beadBuffId;
		recommend->enhanceType = auctionInfo->GetEnhanceType();
		if (auctionInfo->isTreas) //珍品
		{
			recommend->price = 0;
		}
		else //非珍品
		{
			//recommend->price = _GetAverPrice(itemTypeID, strengthen, beadBuffId);
			recommend->price = 0;
		}
		//recommend->price = _GetAverPrice(itemTypeID, strengthen, beadBuffId);
		Avalon::Date curr_date = CURRENT_DATE;
		UInt32 curr_time = (UInt32)curr_date.ToTime().Sec();
		recommend->price_update_time = curr_time;
		recommend->AddRecentPrice(price, num, curr_time, qualitylv, beadAverPrice, magicCardAverPrice, auctionInfo->isTreas);
		if (!auctionInfo->isTreas)
		{
			recommend->visibility_price = 0;
			recommend->vis_price_update_time = curr_time;
		}
		
		CLInsertCommand *pCmd = CLRecordClient::Instance()->PrepareInsertCommand("t_auction_recommend_price", recommend->guid);
		pCmd->PutData("item_type_id", recommend->itemTypeID);
		pCmd->PutData("strengthen", recommend->strengthen);
		pCmd->PutData("bead_buff_id", recommend->bead_buff_id);
		pCmd->PutData("enhance_type", recommend->enhanceType);
		pCmd->PutData("price", recommend->price);
		pCmd->PutData("price_update_time", recommend->price_update_time);
		pCmd->PutData("recent_price", recommend->GetRecentPriceStr());
		if (!auctionInfo->isTreas)
		{
			pCmd->PutData("visibility_price", recommend->visibility_price);
			pCmd->PutData("vis_price_update_time", recommend->vis_price_update_time);
		}
		CLRecordClient::Instance()->SendCommand(pCmd);

		m_recommendPrice[key] = recommend;
	}
	else
	{
		recommend = itr->second;

		recommend->AddRecentPrice(price, num, (UInt32)Avalon::Time::CurrentTime().Sec(), qualitylv, beadAverPrice, magicCardAverPrice, auctionInfo->isTreas);

		recommend->UpdatePrice(auctionInfo->isTreas);
		bool update_vis_price = false;
		if (!auctionInfo->isTreas)
		{
			update_vis_price = recommend->UpdateVisPrice();
		}
		CLUpdateCommand *pCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_auction_recommend_price", recommend->guid, false);
		pCmd->PutData("price", recommend->price);
		pCmd->PutData("recent_price", recommend->GetRecentPriceStr());
		pCmd->PutData("price_update_time", recommend->price_update_time);
		if (update_vis_price)
		{
			pCmd->PutData("visibility_price", recommend->visibility_price);
			pCmd->PutData("vis_price_update_time", recommend->vis_price_update_time);
		}
		CLRecordClient::Instance()->SendCommand(pCmd);
	}
}

void AuctionMgr::RecordTreasDealPrice(const AuctionTransRecord& auctionTrans, bool fore)
{
	if (auctionTrans.m_isSysRecove)
	{
		return;
	}
	if (auctionTrans.m_isNotRecAveprice && !fore)
	{
		return;
	}
	UInt32 itemTypeID = auctionTrans.m_item_data_id;
	UInt32 strengthen = auctionTrans.m_strengthen;
	UInt32 beadBuffId = auctionTrans.m_beadbuffId;
	UInt8  enhanceType = auctionTrans.m_enhanceType;
	UInt32 beadAverPrice = auctionTrans.m_beadAverPrice;
	UInt32 magicCardAverPrice = auctionTrans.m_magicCardAverPrice;
	UInt32 qualitylv = auctionTrans.m_qualitylv;
	UInt32 price = auctionTrans.m_transaction_amount;
	UInt32 num = auctionTrans.m_buy_num;
	bool isTreas = auctionTrans.m_is_treas;
	if (!isTreas)
	{
		InfoLogStream << "not treas auction trans id="<< auctionTrans.GetID() << ", auction id=" << auctionTrans.m_auction_id << LogStream::eos;
		return;
	}
	
	/*if (auctionTrans.m_mountBeadId > 0 || auctionTrans.m_mountMagicCardId > 0)
	{
	InfoLogStream << "mounted bead or magic, trans id=" << auctionTrans.GetID() << ", auction id=" << auctionTrans.m_auction_id << LogStream::eos;
	return;
	}*/

	auto itemData = ItemDataEntryMgr::Instance()->FindEntry(itemTypeID);
	if (!itemData)
	{
		InfoLogStream << "not find item data entry id : " << itemTypeID << LogStream::eos;
		return;
	}

	if (itemData->type == ITEM_EQUIP && (beadAverPrice > 0 || magicCardAverPrice > 0) && auctionTrans.m_MPrice > 0)
	{
		price = auctionTrans.m_MPrice;
	}

	strengthen = GetStrengthenForAverPrice(*itemData, strengthen, enhanceType);

	AuctionRecommendPrice* recommend = NULL;
	AuctionRecommendPriceKey key(itemTypeID, strengthen, beadBuffId, enhanceType);

	auto itr = m_recommendPrice.find(key);
	if (itr == m_recommendPrice.end() || !itr->second)
	{
		recommend = new AuctionRecommendPrice();
		recommend->guid = CLRecordClient::Instance()->GenGuid();
		recommend->itemTypeID = itemTypeID;
		recommend->strengthen = strengthen;
		recommend->bead_buff_id = beadBuffId;
		recommend->enhanceType = enhanceType;

		if (isTreas) //珍品
		{
			recommend->price = 0;
		}
		else //非珍品
		{
			//recommend->price = _GetAverPrice(itemTypeID, strengthen, beadBuffId);
			recommend->price = 0;
		}
		
		//recommend->price = _GetAverPrice(itemTypeID, strengthen, beadBuffId);
		Avalon::Date curr_date = CURRENT_DATE;
		UInt32 curr_time = (UInt32)curr_date.ToTime().Sec();
		recommend->price_update_time = curr_time;
		recommend->AddRecentPrice(price, num, curr_time, qualitylv, beadAverPrice, magicCardAverPrice, isTreas);
		if (!isTreas)
		{
			recommend->visibility_price = 0;
			recommend->vis_price_update_time = curr_time;
		}
		
		CLInsertCommand *pCmd = CLRecordClient::Instance()->PrepareInsertCommand("t_auction_recommend_price", recommend->guid);
		pCmd->PutData("item_type_id", recommend->itemTypeID);
		pCmd->PutData("strengthen", recommend->strengthen);
		pCmd->PutData("bead_buff_id", recommend->bead_buff_id);
		pCmd->PutData("enhance_type", recommend->enhanceType);
		pCmd->PutData("price", recommend->price);
		pCmd->PutData("price_update_time", recommend->price_update_time);
		pCmd->PutData("recent_price", recommend->GetRecentPriceStr());
		if (!isTreas)
		{
			pCmd->PutData("visibility_price", recommend->visibility_price);
			pCmd->PutData("vis_price_update_time", recommend->vis_price_update_time);
		}

		CLRecordClient::Instance()->SendCommand(pCmd);

		m_recommendPrice[key] = recommend;
	}
	else
	{
		recommend = itr->second;

		recommend->AddRecentPrice(price, num, (UInt32)Avalon::Time::CurrentTime().Sec(), qualitylv, beadAverPrice, magicCardAverPrice, isTreas);

		recommend->UpdatePrice(isTreas);
		bool update_vis_price = false;
		if (!isTreas)
		{
			update_vis_price = recommend->UpdateVisPrice();
		}
		CLUpdateCommand *pCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_auction_recommend_price", recommend->guid, false);
		pCmd->PutData("price", recommend->price);
		pCmd->PutData("recent_price", recommend->GetRecentPriceStr());
		pCmd->PutData("price_update_time", recommend->price_update_time);
		if (update_vis_price)
		{
			pCmd->PutData("visibility_price", recommend->visibility_price);
			pCmd->PutData("vis_price_update_time", recommend->vis_price_update_time);
		}
		CLRecordClient::Instance()->SendCommand(pCmd);
	}
}

void AuctionMgr::_UpdateRecommendPrice()
{
    /*for (auto itr : m_recentPrice)
    {
        auto itemTypeId = itr.first;
        auto& recentPrice = itr.second;
        auto recommendItr = m_recommendPrice.find(itr.first);
        if (recommendItr == m_recommendPrice.end())
        {
            AuctionRecommendPrice* recommend = new AuctionRecommendPrice();
            recommend->guid = CLRecordClient::Instance()->GenGuid();
            recommend->itemTypeID = itemTypeId;
            recommend->price = recentPrice.averPrice;

            CLInsertCommand *pCmd = CLRecordClient::Instance()->PrepareInsertCommand("t_auction_recommend_price", recommend->guid);
            pCmd->PutData("item_type_id", recommend->itemTypeID);
            pCmd->PutData("price", recommend->price);
            CLRecordClient::Instance()->SendCommand(pCmd);

            m_recommendPrice[itemTypeId] = recommend;
        }
        else
        {
            auto recommend = recommendItr->second;
            recommend->price = recentPrice.averPrice;
            CLUpdateCommand *pCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_auction_recommend_price", recommend->guid, false);
            pCmd->PutData("price", recommend->price); 
            CLRecordClient::Instance()->SendCommand(pCmd);

            m_recommendPrice[itemTypeId] = recommend;
        }
    }*/

    m_recentPrice.clear();
}

void AuctionMgr::_OnSendAuctionToPlayer(WSPlayer* player, AuctionSendItemType sendType, AuctionType type, bool abnormalDeal, ObjID_t owner, UInt32 ownerVipLev, ObjID_t srcItemId, ObjID_t itemId,
											AuctionMainItemType mainType, UInt32 itemDataId, UInt32 num, MoneyType priceType, UInt32 price, UInt32 counterFee, UInt8 strengthen, 
											UInt32 averPrice, UInt32 onSealTime, ItemQuality itemQuality, std::string sellerIp, std::string sellerDeviceId, UInt32 ownerAccount,
											UInt8 isTreas, UInt32 sysRecomPrice, UInt32 minPriceOnSale, UInt64 transGuid, UInt8 isAttented, UInt32 itemAttentNum,
											UInt32 mountMagicCardId, UInt32 mountMagicCardAverPrice, UInt32 mountBeadId, UInt32 mountBeadAverPrice, UInt32 eqQualityLv, UInt32 remainSealCount,
											UInt64 minPriceGuid, UInt32 minPriceOnsalePrice, UInt32 minPriceQualityLv, UInt32 minPriceMountBeadId, UInt32 minPriceMountBeadAverPrice, 
											UInt32 minPriceMountMagicId, UInt32 minPriceMountMagicAverPrice, UInt32 auctionPrice, UInt32 auctionTransNum, UInt32 auctionTransMax)
{
	switch (sendType)
	{
	case zjy_auction::ASIT_BUY:
		if (NULL != player)
		{
			player->OnBuyAuction(type, abnormalDeal, owner, ownerVipLev, srcItemId, itemId, mainType, itemDataId, num, priceType,
				price, counterFee, strengthen, averPrice, onSealTime, itemQuality, sellerIp, sellerDeviceId, ownerAccount,
				isTreas, sysRecomPrice, minPriceOnSale, transGuid, isAttented, itemAttentNum, mountMagicCardId, mountMagicCardAverPrice,
				mountBeadId, mountBeadAverPrice, eqQualityLv, remainSealCount, minPriceGuid, minPriceOnsalePrice, minPriceQualityLv,
				minPriceMountBeadId, minPriceMountBeadAverPrice, minPriceMountMagicId, minPriceMountMagicAverPrice, auctionPrice, auctionTransNum, auctionTransMax);
		}
		break;
	case zjy_auction::ASIT_CANCEL:
		if (NULL != player)
		{
			player->OnDownAuction(ASIT_CANCEL, type, itemId, itemDataId, num);
		}
		break;
	case zjy_auction::ASIT_TIMEOUT:
		//TODO: 这里应该抛出一个拍卖超时的离线事件

		Sys::SendUdpLog("auction_down", LOG_AUCTION_DOWN_FORMAT, owner, ASIT_TIMEOUT, type, itemId, itemDataId, num);
		break;
	default:
		break;
	}
}

bool AuctionMgr::_IsValidPrice(UInt32 typeId, UInt8 strengthen, UInt32 beadbuffid, UInt8 enhanceType, UInt32 price)
{
	/*auto item = ItemDataEntryMgr::Instance()->FindEntry(id);
	if (!item)
	{
		return false;
	}

	UInt32 minPrice = (UInt32)((UInt64)item->recommendPrice * 40 / 100);
	UInt32 maxPrice = (UInt32)((UInt64)item->recommendPrice * 160 / 100);
	if (price < minPrice || price > maxPrice)
	{
		return false;
	}

	return true;*/
	UInt32 minPrice = 0;
	UInt32 maxPrice = 0;
	if (!_GetLimitPrice(typeId, strengthen, beadbuffid, enhanceType, minPrice, maxPrice))
	{
		return false;
	}

	if (price < minPrice || price > maxPrice)
	{
		return false;
	}
	return true;
}

bool AuctionMgr::_GetLimitPrice(UInt32 typeId, UInt8 strengthen, UInt32 beadbuffid, UInt8 enhanceType, UInt32& minPrice, UInt32& maxPrice)
{
	auto itemdata = ItemDataEntryMgr::Instance()->FindEntry(typeId);
	if (!itemdata)
	{
		ErrorLogStream << "can't find item(" << typeId << ")..." << LogStream::eos;
		return false;
	}

	if (itemdata->isTreasure == 1 && WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_AUCTION_TREAS))
	{
		//激化装备
		if (enhanceType > ENHANCE_INVALID)
		{
			UInt32 averPrice = _GetAverPrice(typeId, 0, 0, ENHANCE_INVALID);
			if (averPrice == 0)
			{
				averPrice = _GetRecommendPrice(typeId, 0, ENHANCE_INVALID);
			}
			UInt32 addition = AuctionMagicStrengAdditDataEntryMgr::Instance()->GetAddition(itemdata->color, strengthen, AMSTP_ZENFU);
			minPrice = (UInt64)averPrice * (100 + addition) / 100 * itemdata->auctionMinPriceRate / 100;
			maxPrice = (UInt64)averPrice * (100 + addition) / 100 * itemdata->auctionMaxPriceRate / 100;
		}
		else
		{
			UInt32 price_uplimit_rate = itemdata->auctionMaxPriceRate;
			UInt32 price_uplimit = ((UInt64)_GetRecommendPrice(typeId, strengthen, enhanceType)) * price_uplimit_rate / 100;
			minPrice = 1000;
			maxPrice = price_uplimit;
			if (minPrice > maxPrice)
			{
				minPrice = maxPrice;
			}
		}
	}
	else if (itemdata->subType == ST_MAGICCARD && strengthen > 0 )
	{
		//UInt32 recomPrice = _GetRecommendPrice(typeId, strengthen, enhanceType);
		//
		//UInt32 expectUpTimes = AuctionMagicExpectUpDataEntryMgr::Instance()->GetExpectUpTimes(itemdata->color, strengthen);
		//UInt32 compPrice = itemdata->recommendPrice + itemdata->recommendPrice * (float)expectUpTimes / 100;
		//UInt32 tempPrice = recomPrice < compPrice ? recomPrice : compPrice;
		//maxPrice = tempPrice * itemdata->auctionMaxPriceRate / 100;
		//minPrice = recomPrice * itemdata->auctionMinPriceRate / 100;

		//推荐价格
		UInt32 recomPrice = _GetRecommendPrice(typeId, strengthen, enhanceType);
		//平均交易价格
		UInt32 averPrice = 0;
		 _GetVisAverPrice(typeId, strengthen, beadbuffid, enhanceType, averPrice);

		 UInt32 tempPrice = recomPrice < averPrice ? recomPrice : averPrice;
		 maxPrice = tempPrice * itemdata->auctionMaxPriceRate / 100;
		 minPrice = recomPrice * itemdata->auctionMinPriceRate / 100;

		if (minPrice > maxPrice)
		{
			minPrice = maxPrice;
		}
	}
	else
	{
		UInt32 avePrice = 0;
		bool isAvgPrice = _GetVisAverPrice(typeId, strengthen, beadbuffid, enhanceType, avePrice);
		UInt32 addition = 0;
		
		if (enhanceType > ENHANCE_INVALID)
		{
			addition = AuctionMagicStrengAdditDataEntryMgr::Instance()->GetAddition(itemdata->color, strengthen, AMSTP_ZENFU);
		}
		//else if (itemdata->subType == ST_MAGICCARD)
		//{
		//	addition = AuctionMagicStrengAdditDataEntryMgr::Instance()->GetAddition(itemdata->color, strengthen, AMSTP_MAGIC);
		//}
		else
		{
			addition = GetAuctionStrengthAddition(strengthen);
		}

		if (isAvgPrice)
		{
			UInt32 minRecommendPrice = (UInt32)((UInt64)itemdata->recommendPrice * itemdata->auctionMinPriceRate / 100 * (100 + addition) / 100);
			UInt32 maxRecommendPrice = (UInt32)((UInt64)itemdata->recommendPrice * itemdata->auctionMaxPriceRate / 100 * (100 + addition) / 100);

			//UInt32 minVisPrice = (UInt32)((UInt64)avePrice * 50 / 100);
			UInt32 maxVisPrice = (UInt32)((UInt64)avePrice * 150 / 100);

			minPrice = minRecommendPrice;
			maxPrice = maxRecommendPrice < maxVisPrice ? maxRecommendPrice : maxVisPrice;
		}
		else
		{
			minPrice = (UInt32)((UInt64)itemdata->recommendPrice * itemdata->auctionMinPriceRate / 100 * (100 + addition) / 100);
			maxPrice = (UInt32)((UInt64)itemdata->recommendPrice * itemdata->auctionMaxPriceRate / 100 * (100 + addition) / 100);
		}
	}

#ifdef _DEBUG
	InfoLogStream << "AuctionMgr::_GetLimitPrice itemId=" << typeId << " strengthen = " << strengthen
		<< " beadbuffid = " << beadbuffid << " minPrice = " << minPrice << " maxPrice = " << maxPrice << LogStream::eos;
#endif

	return true;
}

MoneyType AuctionMgr::GetMoneyTypeByAuctionType(AuctionType type)
{
    if (type == AT_ITEM)
    {
        return MONEY_GOLD;
    }
    else if (type == AT_GOLD)
    {
		return MONEY_POINT;
    }

    return MONEY_INVALID;
}

AuctionMainItemType AuctionMgr::GetAuctionMainType(ItemType type, ItemSubType subType)
{
	if (type == ITEM_EXPENDABLE)
	{
		return AMIT_COST;
	}
	else if (type == ITEM_MATERIAL)
	{
		return AMIT_MATERIAL;
	}
	else if (type == ITEM_TITLE)
	{
		return AMIT_JEWELRY;
	}
	else if (subType == ST_EQUIP_WEAPON)
    {
        return AMIT_WEAPON;
    }
	else if (subType >= ST_EQUIP_HEAD && subType <= ST_EQUIP_BOOT)
    {
        return AMIT_ARMOR;
    }
	else if (subType >= ST_EQUIP_RING && subType <= ST_EQUIP_BRACELET)
    {
        return AMIT_JEWELRY;
    }
    else
    {
        return AMIT_OTHER;
    }
}

UInt32 AuctionMgr::GetAuctionSubType(ItemDataEntry* entry)
{
    if (!entry)
    {
        return 0;
    }
    return entry->subType * 1000 + entry->thirdType;
}

bool AuctionMgr::GenSqlByCondition(WSPlayer* player, CLSelectCommand* command, const CLProtocol::AuctionQueryCondition& cond)
{
	if (!command || !player)
	{
		return false;
	}

	if (!WSApplication::Instance()->GetAuctionSqlJointModify())
	{
	command->PutCond("duetime", (UInt32)(CURRENT_TIME.Sec()), ">");
	command->PutCond("type", cond.type);
	}
	
	if (cond.type == AT_GOLD)
	{
		if (cond.itemSubTypes.empty())
		{
			return false;
		}

		std::stringstream ss;
		ss << "(";
		for (UInt32 i = 0; i < cond.itemSubTypes.size(); i++)
		{
			if (i)
			{
				ss << ", ";
			}
			UInt32 num = (UInt32)cond.itemSubTypes[i] * GOLD_BASE_NUM;
			ss << GetGoldItemByNum(num);
		}
		ss << ")";

		command->PutCond("itemid", ss.str(), "in");
	}
	else if (cond.attent == AuctionAttentType::ATT_ATTENT)
	{
		std::vector<UInt64> ids;
		AuctionAttentMgr::Instance()->GetAttentedAuctions(player->GetID(), ids);
		if (ids.size() == 0)
		{
			InfoLogStream << "player(" << player->GetID() << "), auction attent list query, attent list is empty" << LogStream::eos;
			return false;
		}
		std::vector<UInt64> idv;
		AuctionMgr::Instance()->FilterAuctions(ids, idv, cond.goodState);
		if (idv.size() == 0)
		{
			InfoLogStream << "player(" << player->GetID() << "), auction attent list query, attent list is empty, goodState=" << cond.goodState << LogStream::eos;
			return false;
		}
		std::stringstream ss;
		ss << "(";
		for (UInt32 i = 0; i < idv.size(); i++)
		{
			if (i)
			{
				ss << ", ";
			}
			ss << idv[i];
		}
		ss << ")";

		command->PutCond("guid", ss.str(), "in");
	}
	else if (cond.itemTypeID)
	{
		command->PutCond("itemid", cond.itemTypeID);
	}
	else if (!cond.itemSubTypes.empty())
	{
		if (cond.itemSubTypes.size() > 50)
		{
			ErrorLogStream << "player(" << player->GetID() << "), GenSqlByCondition err cond.itemSubTypes.size() " << (int)cond.itemSubTypes.size() << LogStream::eos;
			return false;
		}
		std::stringstream ss;
		ss << "(";
		for (UInt32 i = 0; i < cond.itemSubTypes.size(); i++)
		{
			if (i)
			{
				ss << ", ";
			}
			ss << cond.itemSubTypes[i];
		}
		ss << ")";

		command->PutCond("item_sub_type", ss.str(), "in");
	}
	else if (cond.itemMainType > 0)
	{
		command->PutCond("item_main_type", cond.itemMainType);
		if (cond.excludeItemSubTypes.size() > 0)
		{
			if (cond.excludeItemSubTypes.size() > 50)
			{
				ErrorLogStream << "player(" << player->GetID() << "), GenSqlByCondition err cond.excludeItemSubTypes.size() " << (int)cond.excludeItemSubTypes.size() << LogStream::eos;
				return false;
			}
			/*std::stringstream ss;
			ss << "(";
			for (UInt32 i = 0; i < cond.excludeItemSubTypes.size(); i++)
			{
				if (i)
				{
					ss << ", ";
				}
				ss << cond.excludeItemSubTypes[i];
			}
			ss << ")";

			std::string limitStr = "AND `item_sub_type` not in ";
			limitStr += ss.str();
			command->SetLimit(limitStr);*/

			for (UInt32 i = 0; i < cond.excludeItemSubTypes.size(); i++)
			{
				command->PutCond("item_sub_type", cond.excludeItemSubTypes[i], "<>");
			}


		}
	}
	else
	{
		//return false;
	}

	if (cond.quality > 0)
	{
		command->PutCond("quality", cond.quality);
	}

	if (cond.minLevel != cond.maxLevel)
	{
		command->PutCond("item_level", cond.minLevel, ">=");
		command->PutCond("item_level", cond.maxLevel, "<=");
	}
	else if (cond.minLevel == cond.maxLevel && cond.minLevel != 0)
	{
		command->PutCond("item_level", cond.minLevel);
	}

	if (cond.minStrength != cond.maxStrength)
	{
		if (!WSApplication::Instance()->GetAuctionSqlInModify())
		{
			command->PutCond("strengthen", cond.minStrength, ">=");
			command->PutCond("strengthen", cond.maxStrength, "<=");
		}
		else
		{
			if (cond.minStrength > cond.maxStrength)
			{
				ErrorLogStream << "player(" << player->GetID() << "), invalid cond, strength(" << cond.minStrength << ", " << cond.maxStrength << ")" << LogStream::eos;
				return false;
			}

			if (cond.maxStrength >= 30)
			{
				ErrorLogStream << "player(" << player->GetID() << "), invalid cond, strength(" << cond.minStrength << ", " << cond.maxStrength << ")" << LogStream::eos;
				return false;
			}

			std::stringstream ss;
			ss << "(";
			for (UInt8 i = cond.minStrength; i < cond.maxStrength; i++)
			{
				if (i)
				{
					ss << ", ";
				}
				ss << (UInt32)i;
			}
			ss << ")";

			command->PutCond("strengthen", ss.str(), "in");
		}
	}
	else if (cond.minStrength == cond.maxStrength && cond.minStrength != 0)
	{
		if (cond.minStrength == 100) //附魔卡查询等级0的数量的时候传100特殊处理
		{
			command->PutCond("strengthen", 0);
		}
		else
		{
			command->PutCond("strengthen", cond.minStrength);
		}
	}

	if (cond.couponStrengthToLev > 0)
	{
		command->PutCond("coupon_strengthen_to", cond.couponStrengthToLev);
	}

	if (cond.goodState == AGS_PUBLIC)
	{
		UInt32 currTime = (UInt32)CURRENT_TIME.Sec();
		command->PutCond("on_public_time", 0, "<>");
		command->PutCond("on_public_time", currTime, "<=");
		command->PutCond("on_sale_time", currTime, ">");
	}
	else if (cond.goodState == AGS_ONSALE)
	{
		UInt32 currTime = (UInt32)CURRENT_TIME.Sec();
		command->PutCond("on_sale_time", currTime, "<=");
	}
	else
	{
		ErrorLogStream << "GenSqlByCondition err cond.goodState " << (int)cond.goodState << LogStream::eos;
	}

	//command->PutCond("on_sale_time", player->GetAuctionRefreshTime(), "<=");
	if (WSApplication::Instance()->GetAuctionSqlJointModify())
	{
		command->PutCond("duetime", (UInt32)(CURRENT_TIME.Sec()), ">");
		command->PutCond("type", cond.type);
	}
	return true;
}

bool AuctionMgr::GenAuctionItemNumSql(WSPlayer* player, CLSelectCommand* command, const CLProtocol::AuctionQueryCondition& cond)
{
	if (!command || !player)
	{
		return false;
	}

	GenSqlByCondition(player, command, cond);

	command->PutColumn("itemid");
	command->PutColumn("quality");
	command->PutColumn("item_level");
	command->PutColumn("cast(sum(num) as unsigned)");
	//command->SetLimit("group by itemid");
	std::stringstream limitStream;
	limitStream << "group by itemid order by quality desc, item_level asc";
	command->SetLimit(limitStream.str());
	return true;
}

bool AuctionMgr::GenAuctionQuerySqlCondition(WSPlayer* player, CLSelectCommand* command, const CLProtocol::AuctionQueryCondition& cond)
{
	if (!command || !player)
    {
        return false;
    }

	if (cond.itemNumPerPage == 0)
	{
		return false;
	}

	if (!GenSqlByCondition(player, command, cond))
	{
		return false;
	}
    
	std::stringstream orderStream;
    orderStream << "order by price / num " << (((AuctionSortType)cond.sortType == AST_PRICE_ASC) ? "asc" : "desc");
    command->SetOrder(orderStream.str());

    return true;
}

void AuctionMgr::UpdateTreasState(const Avalon::Time &now)
{
	std::vector<ObjID_t> ids;
	std::vector<AuctionInfo*> recoverAuctions; //被系统扫货的id

	for (auto it = m_needUpdateTreasAuctions.begin(); it != m_needUpdateTreasAuctions.end(); ++it)
	{
		AuctionInfo* pAuctionInfo = it->second;
		bool earse = false;
		bool recoved = false;
		if (!this->UpdateOneTreasState(pAuctionInfo, now, earse, recoved, false) || !earse)
		{
			continue;
		}

		InfoLogStream << "earse one treas state guid=" << pAuctionInfo->guid << ", goodState=" << pAuctionInfo->goodState
			<< ", BuyState=" << pAuctionInfo->buyState << ", curr_time=" << (UInt32)CURRENT_TIME.Sec() << ", onPublicTime=" << pAuctionInfo->onPublicTime
			<< ", onSaleTime=" << pAuctionInfo->onSaleTime << LogStream::eos;
		ids.push_back(it->first);
		if (recoved)
		{
			recoverAuctions.push_back(pAuctionInfo);
		}
	}

	for (auto id : ids)
	{
		m_needUpdateTreasAuctions.erase(id);
	}

	for (auto auction : recoverAuctions)
	{
		if (auction->num == 0)
		{
			UInt8 type = auction->type;
			ObjID_t playerId = auction->owner;
			ObjID_t auctionGuid = auction->guid;
			_DeleteAuctionInfo(auction);
			WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(playerId);
			if (player)
			{
				CLProtocol::WorldAuctionNotifyRefresh refresh;
				refresh.type = type;
				refresh.reason = SRR_SYS_RECOVERY;
				refresh.auctGuid = auctionGuid;
				player->SendProtocol(refresh);
			}
		}
	}
}

bool AuctionMgr::UpdateOneTreasState(AuctionInfo* pAuctionInfo, const Avalon::Time &now, bool& earse, bool& recoved, bool canDelete)
{
	earse = false;
	if (pAuctionInfo == NULL || !pAuctionInfo->isTreas) return false;
	UInt32 curr_time = (UInt32)now.Sec();
	UInt32 rush_buy_time = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_RUSH_BUY_TIME);
	if (rush_buy_time == 0) rush_buy_time = 5;

	//UInt8 OldGoodState = pAuctionInfo->goodState;
	UInt8 OldBuyState = pAuctionInfo->buyState;
	if (pAuctionInfo->goodState == AGS_PUBLIC) //公示状态
	{
		if (curr_time >= pAuctionInfo->onSaleTime)
		{
			SetAuctionGoodState(*pAuctionInfo, AGS_ONSALE); //上架

			OnTreasOnSale(*pAuctionInfo);

			if (curr_time < pAuctionInfo->onSaleTime + rush_buy_time)
			{
				SetAuctionBuyState(*pAuctionInfo, AGBS_RUSH_BUY); //抢购
			}
			else
			{
				SetAuctionBuyState(*pAuctionInfo, AGBS_NORMAL_BUY); //正常购买
			}

			bool handled = false;
			if (HandleForRecovery(*pAuctionInfo, canDelete, handled))
			{
				earse = true;
				recoved = true;
				return true;
			}
		}
		else
		{

		}
	}
	else if (pAuctionInfo->goodState == AGS_ONSALE) //上架状态
	{
		if (pAuctionInfo->buyState == AGBS_NORMAL_BUY)  ///正常购买
		{
			earse = true;
		}
		else if (pAuctionInfo->buyState == AGBS_RUSH_BUY_DELIVER) //抢购发货中
		{
			if (curr_time > pAuctionInfo->onSaleTime + rush_buy_time + max_rush_buy_time)
			{
				InfoLogStream << "state of auction in AGBS_RUSH_BUY_DELIVER timeout guid=" << pAuctionInfo->guid << LogStream::eos;
				SetAuctionBuyState(*pAuctionInfo, AGBS_NORMAL_BUY); //正常购买
				earse = true;
			}
		}
		else if (AGBS_RUSH_BUY == pAuctionInfo->buyState) //抢购中
		{
			if (curr_time >= pAuctionInfo->onSaleTime + rush_buy_time) //抢购转抢购发货
			{
				if (pAuctionInfo->rushBuyPlayers.size() > 0)
				{
					InfoLogStream << "buyState of auction change to AGBS_RUSH_BUY_DELIVER, guid=" << pAuctionInfo->guid << LogStream::eos;
					SetAuctionBuyState(*pAuctionInfo, AGBS_RUSH_BUY_DELIVER);
					if (!this->DeliverOneRushBuyPlayerNew(pAuctionInfo))
					{
						InfoLogStream << "DeliverOneRushBuyPlayerNew fail!  buyState of auction change to AGBS_NORMAL_BUY auction guid=" << pAuctionInfo->guid << LogStream::eos;
						SetAuctionBuyState(*pAuctionInfo, AGBS_NORMAL_BUY);
						earse = true;
					}
				}
				else
				{
					InfoLogStream << "size of rushBuyPlayers is 0, buyState of auction change to AGBS_NORMAL_BUY, auction guid=" << pAuctionInfo->guid << LogStream::eos;
					SetAuctionBuyState(*pAuctionInfo, AGBS_NORMAL_BUY);
					earse = true;
				}
			}
		}
	}
	else
	{
		ErrorLogStream << "UpdateTreasState err state of auction is err guid:" << pAuctionInfo->guid << ", state:" << (int)pAuctionInfo->goodState << LogStream::eos;
		return false;
	}

	if (OldBuyState != pAuctionInfo->buyState && pAuctionInfo->buyState == AGBS_NORMAL_BUY)
	{
		//this->RemoveOneTreas(pAuctionInfo);
		earse = true;
	}
	return true;
}

void AuctionMgr::AddShieldItem(UInt32 itemId)
{
	m_ShieldItemVec.push_back(itemId);
}

void AuctionMgr::ClearShieldItem()
{
	m_ShieldItemVec.clear();
}

bool AuctionMgr::AddOneRushBuyPlayer(AuctionInfo* pAuctionInfo, WSPlayer* player, UInt32 num)
{
	if (pAuctionInfo == NULL || player == NULL) return false;
	for (UInt32 i = 0; i < pAuctionInfo->rushBuyPlayers.size(); ++i)
	{
		if (pAuctionInfo->rushBuyPlayers[i].playerId == player->GetID())
		{
			player->SendNotify(9948); //你已经抢购过该物品
			return false;
		}
	}
	pAuctionInfo->rushBuyPlayers.push_back(AuctionRushBuyPlayer(player->GetID(), num));
	player->SendNotify(9949);//抢购物品成功
	return true;
}

/*void AuctionMgr::DeliverOneRushBuyPlayer(AuctionInfo* pAuctionInfo)
{
	if (pAuctionInfo == NULL
		|| pAuctionInfo->goodState != AGS_ONSALE
		|| pAuctionInfo->buyState != AGBS_RUSH_BUY_DELIVER)
		return;

	if (pAuctionInfo->rushBuyPlayers.empty()) //抢购发货结束
	{
		this->HandleTreaseRushDeliverEnd(pAuctionInfo);
		return;
	}

	//选择一个玩家发货
	int index = Avalon::Random::RandBetween(0, pAuctionInfo->rushBuyPlayers.size() - 1);
	if (index < 0 || index >= pAuctionInfo->rushBuyPlayers.size())
	{
		ErrorLogStream << "DeliverOneRushBuyPlayer Random::RandBetween err index = " << index << LogStream::eos;
		pAuctionInfo->buyState = AGBS_NORMAL_BUY;
		return;
	}
	//pAuctionInfo->rushBuyPlayers.erase(pAuctionInfo->rushBuyPlayers.begin() + i);
	//std::random_shuffle(pAuctionInfo->rushBuyPlayers.begin(), pAuctionInfo->rushBuyPlayers.end());
	//AuctionRushBuyPlayer& rushBuyPlayer = pAuctionInfo->rushBuyPlayers.back();
	AuctionRushBuyPlayer& rushBuyPlayer = pAuctionInfo->rushBuyPlayers[index];

	if (rushBuyPlayer.buyNum > pAuctionInfo->num)
	{
		//pAuctionInfo->rushBuyPlayers.pop_back();
		pAuctionInfo->rushBuyPlayers.erase(pAuctionInfo->rushBuyPlayers.begin() + index);
		if (pAuctionInfo->rushBuyPlayers.empty())
		{
			this->HandleTreaseRushDeliverEnd(pAuctionInfo);
			return;
		}
		else
		{
			return this->DeliverOneRushBuyPlayer(pAuctionInfo);
		}
	}

	WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(rushBuyPlayer.playerId);
	if (player == NULL)
	{
		//pAuctionInfo->rushBuyPlayers.pop_back();
		pAuctionInfo->rushBuyPlayers.erase(pAuctionInfo->rushBuyPlayers.begin() + index);
		if (pAuctionInfo->rushBuyPlayers.empty())
		{
			this->HandleTreaseRushDeliverEnd(pAuctionInfo);
			return;
		}
		else
		{
			return this->DeliverOneRushBuyPlayer(pAuctionInfo);
		}
	}
	else
	{
		UInt32 unitPrice = pAuctionInfo->price / pAuctionInfo->num;
		UInt32 price = unitPrice * rushBuyPlayer.buyNum;
		BuyAuctionTransaction* pTransaction = new BuyAuctionTransaction(player, pAuctionInfo->guid, rushBuyPlayer.buyNum);
		if (MONEY_GOLD == pAuctionInfo->pricetype)
		{
			pTransaction->SetSilver(price);
		}
		else if (MONEY_POINT == pAuctionInfo->pricetype)
		{
			pTransaction->SetGold(price);
		}
		else
		{
			delete pTransaction;
			//pAuctionInfo->rushBuyPlayers.pop_back();
			pAuctionInfo->rushBuyPlayers.erase(pAuctionInfo->rushBuyPlayers.begin() + index);
			if (pAuctionInfo->rushBuyPlayers.empty())  //
			{
				this->HandleTreaseRushDeliverEnd(pAuctionInfo);
				return;
			}
			else
			{
				return this->DeliverOneRushBuyPlayer(pAuctionInfo);
			}
		}
		if (!player->BeginItemTransaction(pTransaction))
		{
			//pAuctionInfo->rushBuyPlayers.pop_back();
			pAuctionInfo->rushBuyPlayers.erase(pAuctionInfo->rushBuyPlayers.begin() + index);
			if (pAuctionInfo->rushBuyPlayers.empty())
			{
				this->HandleTreaseRushDeliverEnd(pAuctionInfo);
				return;
			}
			else
			{
				return this->DeliverOneRushBuyPlayer(pAuctionInfo);
			}
		}
	}
}*/

bool AuctionMgr::SelectOneRushBuyer(const AuctionInfo& auctionInfo, AuctionRushBuyPlayer& rush_buyer)
{
	//选择一个玩家发货
	if (auctionInfo.rushBuyPlayers.size() == 0)
	{
		InfoLogStream << "size of rushBuyPlayers is 0, auction guid=" << auctionInfo.guid << LogStream::eos;
		return false;
	}

	std::vector<const AuctionRushBuyPlayer*> onlineRushBuyer;
	for (auto& rushbuyer : auctionInfo.rushBuyPlayers)
	{
		WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(rushbuyer.playerId);
		if (player)
		{
			onlineRushBuyer.push_back(&rushbuyer);
		}
	}

	if (onlineRushBuyer.size() == 0)
	{
		InfoLogStream << "size of onlineRushBuyer is 0, auction guid=" << auctionInfo.guid << LogStream::eos;
		return false;
	}

	UInt32 index = Avalon::Random::RandBetween(0, onlineRushBuyer.size() - 1);
	if (index < 0 || (size_t)index >= onlineRushBuyer.size())
	{
		InfoLogStream << "random index of onlineRushBuyer err auction guid=" << auctionInfo.guid 
			<< ", index=" << index << LogStream::eos;
		return false;
	}
	rush_buyer.playerId = onlineRushBuyer[index]->playerId;
	rush_buyer.buyNum = onlineRushBuyer[index]->buyNum;

	return true;
}

bool AuctionMgr::DeliverOneRushBuyPlayerNew(AuctionInfo* pAuctionInfo)
{
	if (pAuctionInfo == NULL)
	{
		InfoLogStream << "pAuctionInfo is NULL" << LogStream::eos;
		return false;
	}
	if (pAuctionInfo->goodState != AGS_ONSALE
		|| pAuctionInfo->buyState != AGBS_RUSH_BUY_DELIVER)
	{
		InfoLogStream << "state of auction is err goodState=" << pAuctionInfo->goodState 
			<< ", buyState=" << pAuctionInfo->buyState << ", guid=" << pAuctionInfo->guid << LogStream::eos;
		return false;
	}
	
	AuctionRushBuyPlayer selected_rush_buyer;
	if (!SelectOneRushBuyer(*pAuctionInfo, selected_rush_buyer))
	{
		InfoLogStream << "select one rush buyer fail, guid=" << pAuctionInfo->guid << LogStream::eos;
		return false;
	}
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(selected_rush_buyer.playerId);
	if (player == NULL)
	{
		InfoLogStream << "selected player is not online guid=" << pAuctionInfo->guid << LogStream::eos;
		return false;
	}

	if (!this->OnPlayerBuyAuctionItem(player, pAuctionInfo->guid, selected_rush_buyer.buyNum, true))
	{
		HandleTreaseRushDeliverEnd(pAuctionInfo);
		return false;
	}

	//HandleTreaseRushDeliverEnd(pAuctionInfo);

	return true;
}

void AuctionMgr::HandleTreaseRushDeliverEnd(AuctionInfo* pAuctionInfo)
{
	if (pAuctionInfo == NULL || !pAuctionInfo->isTreas || pAuctionInfo->buyState != AGBS_RUSH_BUY_DELIVER)
	{
		ErrorLogStream << "err 1" << LogStream::eos;
		return;
	}
	
	std::vector<AuctionRushBuyPlayer> temp;
	pAuctionInfo->rushBuyPlayers.swap(temp);
	InfoLogStream << "end treas auction's rush deliver guid=" << pAuctionInfo->guid << LogStream::eos;
	SetAuctionBuyState(*pAuctionInfo, AGBS_NORMAL_BUY);
}

/**
*@brief 处理一次抢购发货结果
*/
void AuctionMgr::OnDeliverOneRushBuyResult(ObjID_t playerId, ObjID_t id, UInt32 num, bool succ, UInt8 reason)
{

	InfoLogStream << "OnDeliverOneRushBuyResult rest = " << succ << ", reason = " << (UInt32)reason
		<< ", playerId = " << playerId << ", autionId = " << id << LogStream::eos;
	WSPlayer *player = WSPlayerMgr::Instance()->FindPlayer(playerId);

	auto auctionInfo = _FindAuction(id);
	if (!auctionInfo)
	{
		ErrorLogStream << "OnDeliverOneRushBuyResult auction id(" << id << ") can't find Auction" << LogStream::eos;
		return;
	}

	if (auctionInfo->buyState != AGBS_RUSH_BUY_DELIVER) return;

	if (player)
	{
		if (!succ)
		{
			player->SendNotify(9945);//抢购物品失败
		}
		else
		{
			player->SendNotify(9949);//抢购物品成功
		}
	}
	this->HandleTreaseRushDeliverEnd(auctionInfo);

	/*for (UInt32 i = 0; i < auctionInfo->rushBuyPlayers.size(); ++i)
	{
		if (auctionInfo->rushBuyPlayers[i].playerId == playerId)
		{
			auctionInfo->rushBuyPlayers.erase(auctionInfo->rushBuyPlayers.begin() + i);
			break;
		}
	}
	if (auctionInfo->rushBuyPlayers.empty())
	{
		this->HandleTreaseRushDeliverEnd(auctionInfo);
	}
	else
	{
		this->DeliverOneRushBuyPlayer(auctionInfo);
	}*/
}

void AuctionMgr::SetAuctionItemCoolTime(WSItem* item)
{
	if (!item || !item->GetDataEntry()) return;

	UInt32 cooltime = 0;
	if (item->GetDataEntry()->isTreasure == 1 && WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_AUCTION_TREAS))
	{
		cooltime = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_TREAS_COOLTIME);
	}
	else 
	{
		cooltime = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_NOT_TREAS_COOLTIME);
	}

	Avalon::Date date(Avalon::Time(CURRENT_TIME.MSec() + cooltime * Avalon::Time::MSECS_OF_DAY));
	if (item->GetDataEntry()->maxNum > 0)
	{
		date.Hour(0);
		date.Min(0);
		date.Sec(0);
		date.MSec(0);
	}
	item->SetAuctionCoolTimeStamp(date.Sec());
}

UInt32 AuctionMgr::CalcAuctionItemCoolTime(const std::string& pf, UInt32 itemId, UInt32 beginTime)
{
	ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(itemId);
	if (!dataEntry) return 0;

	return AuctionMgr::CalcAuctionItemCoolTime(pf, dataEntry, beginTime);
}

UInt32 AuctionMgr::CalcAuctionItemCoolTime(const std::string& pf, ItemDataEntry* dataEntry, UInt32 beginTime)
{
	if (!dataEntry) return 0;
	/*UInt32 cooltime = dataEntry->auctionTransCoolTimeMl;*/
	/*if (dataEntry->isTreasure == 1)
	{
		cooltime = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_TREAS_COOLTIME);
	}
	else
	{
		cooltime = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_NOT_TREAS_COOLTIME);
	}*/
	
	UInt32 cooltime = 0;
	if (pf == "7977" || pf == "mg" || pf == "none" || pf == "test" || pf == "")
	{
		cooltime = dataEntry->auctionTransCoolTimeMl;
	}
	else
	{
		cooltime = dataEntry->auctionTransCoolTimeOther;
	}
	//cooltime = dataEntry->auctionTransCoolTimeMl;
	UInt64 time = (UInt64)beginTime * 1000 + (UInt64)cooltime * Avalon::Time::SECS_OF_HOUR * Avalon::Time::MSECS_OF_SEC;
	Avalon::Date date(time);
	if (dataEntry->maxNum > 1)
	{
		date.Hour(0);
		date.Min(0);
		date.Sec(0);
		date.MSec(0);
	}

	return (UInt32)date.ToTime().Sec();
}

void  AuctionMgr::AddOneAution(AuctionInfo* auctionInfo)
{
	if (!auctionInfo) return;
	InfoLogStream << "add auction object guid : " << auctionInfo->guid << ", item data id : " 
		<< auctionInfo->itemTypeId << ", address : " << (UInt64)auctionInfo << LogStream::eos;
	m_auctiones[auctionInfo->guid] = auctionInfo;
	if (auctionInfo->isTreas && auctionInfo->buyState != AGBS_NORMAL_BUY)
	{
		this->AddOneTreas(auctionInfo);
	}

	if (WSApplication::Instance()->IsAuctionOpUseMem())
	{
		AddOneAuctionForMemQuery(auctionInfo);
	}
}

void  AuctionMgr::AddOneTreas(AuctionInfo* auctionInfo)
{
	if (!auctionInfo) return;
	m_needUpdateTreasAuctions[auctionInfo->guid] = auctionInfo;
}

void AuctionMgr::RemoveOneTreas(AuctionInfo* auctionInfo)
{
	if (!auctionInfo) return;
	MapAuction::iterator iter = m_needUpdateTreasAuctions.find(auctionInfo->guid);
	if (iter != m_needUpdateTreasAuctions.end())
	{
		m_needUpdateTreasAuctions.erase(iter);
	}
}

void AuctionMgr::GetBeadAndMagicCardAverPrice(AuctionInfo* auctionInfo, UInt32& beadAverPrice, UInt32& magicCardAverPrice)
{
	if (!auctionInfo || !auctionInfo->item || auctionInfo->itemtype != ITEM_EQUIP)
	{
		beadAverPrice = 0;
		magicCardAverPrice = 0;
		return;
	}

	WSItem* item = auctionInfo->item;
	//宝珠
	PreciousBeadMountHole* hole = item->m_PreciousBeadHoles.GetFirstMountedHole();
	if (hole) //镶嵌宝珠
	{
		beadAverPrice = this->_GetAverPrice(hole->preciousBeadId, 0, hole->buffId, 0);
		if (beadAverPrice == 0)
		{
			beadAverPrice = _GetRecommendPrice(hole->preciousBeadId, 0, 0);
		}
	}
	else //没镶嵌
	{
		beadAverPrice = 0;
	}

	//附魔卡
	UInt32 magicCardId = item->m_AddMagic.GetMagicCard();
	UInt8  magicStrengthen = item->m_AddMagic.GetMagicCardLev();
	if (magicCardId > 0)
	{
		ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(magicCardId);
		if (!dataEntry || dataEntry->color < ItemQuality::ITEM_QUALITY_PINK)
		{
			magicCardAverPrice = 0;
			return;
		}
		magicCardAverPrice = this->_GetAverPrice(magicCardId, magicStrengthen, 0, 0);
		if (magicCardAverPrice == 0)
		{
			magicCardAverPrice = _GetRecommendPrice(magicCardId, magicStrengthen, 0);
		}
	}
	else //没附魔卡
	{
		magicCardAverPrice = 0;
	}
}

void AuctionMgr::GetBeadAndMagicAverPriceForCalcTransEqMPrice(AuctionInfo* auctionInfo, UInt32& beadAverPrice, UInt32& magicCardAverPrice, UInt32& mountedBeadId, UInt32& mountedMagicId, 
	UInt32& mountedBeadBuffId, UInt8& mountedMagicLv)
{
	if (!auctionInfo || !auctionInfo->item || auctionInfo->itemtype != ITEM_EQUIP)
	{
		beadAverPrice = 0;
		magicCardAverPrice = 0;
		mountedBeadId = 0;
		mountedMagicId = 0;
		return;
	}

	WSItem* item = auctionInfo->item;
	//宝珠
	PreciousBeadMountHole* hole = item->m_PreciousBeadHoles.GetFirstMountedHole();
	if (hole) //镶嵌宝珠
	{
		mountedBeadId = hole->preciousBeadId;
		mountedBeadBuffId = hole->buffId;
		beadAverPrice = this->_GetRealAverPrice(hole->preciousBeadId, 0, hole->buffId, 0);
		if (beadAverPrice == 0)
		{
			ItemDataEntry* itemData = ItemDataEntryMgr::Instance()->FindEntry(hole->preciousBeadId);
			if (!itemData)
			{
				beadAverPrice = 0;
			}
			else
			{
				if (itemData->owner == IBTP_NOT) // 非绑定
				{
					beadAverPrice = itemData->recommendPrice;
				}
				else //绑定
				{
					beadAverPrice = itemData->recommendPrice * itemData->auctionMinPriceRate / 100;
				}
				InfoLogStream << "bead id : " << itemData->id << "itemData->owner : " << itemData->owner << ", itemData->recommendPrice " << itemData->recommendPrice << ", itemData->auctionMinPriceRate " << itemData->auctionMinPriceRate
					<< ", beadAverPrice : " << beadAverPrice << LogStream::eos;
			}
			//beadAverPrice = _GetRecommendPrice(hole->preciousBeadId, 0);
		}
	}
	else //没镶嵌
	{
		beadAverPrice = 0;
		mountedBeadId = 0;
		mountedBeadBuffId = 0;
	}

	//附魔卡
	UInt32 magicCardId = item->m_AddMagic.GetMagicCard();
	UInt8  magicStrengthen = item->m_AddMagic.GetMagicCardLev();
	mountedMagicLv = magicStrengthen;
	if (magicCardId > 0)
	{
		mountedMagicId = magicCardId;
		ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(magicCardId);
		if (!dataEntry || dataEntry->color < ItemQuality::ITEM_QUALITY_PINK)
		{
			magicCardAverPrice = 0;
			return;
		}
		magicCardAverPrice = this->_GetRealAverPrice(magicCardId, magicStrengthen, 0, 0);
		if (magicCardAverPrice == 0)
		{
			UInt32 addtion = 0;
			if (magicStrengthen > 0)
			{
				addtion = AuctionMagicStrengAdditDataEntryMgr::Instance()->GetAddition(dataEntry->color, magicStrengthen, 0);
			}
			if (dataEntry->owner == IBTP_NOT) // 非绑定
			{
				magicCardAverPrice = dataEntry->recommendPrice * (100 + addtion) / 100;
			}
			else //绑定
			{
				magicCardAverPrice = dataEntry->recommendPrice * dataEntry->auctionMinPriceRate / 100 * (100 + addtion) / 100;
			}
			//magicCardAverPrice = _GetRecommendPrice(magicCardId, 0);
			InfoLogStream << "magic id : " << dataEntry->id << "dataEntry->owner : " << dataEntry->owner << ", dataEntry->recommendPrice " << dataEntry->recommendPrice << ", dataEntry->auctionMinPriceRate " << dataEntry->auctionMinPriceRate
				<< ", magicCardAverPrice : " << magicCardAverPrice << LogStream::eos;
		}
	}
	else //没附魔卡
	{
		magicCardAverPrice = 0;
		mountedMagicLv = 0;
	}
}

bool AuctionMgr::IsMountedBeadOrMagic(const AuctionInfo& auctionInfo)
{
	WSItem* item = auctionInfo.item;
	if (!item) return false;

	UInt32 magicCardId = item->m_AddMagic.GetMagicCard();
	if (magicCardId > 0) //镶嵌附魔卡
	{
		return true;
	}

	PreciousBeadMountHole* hole = item->m_PreciousBeadHoles.GetFirstMountedHole();
	if (hole) //镶嵌宝珠
	{
		return true;
	}

	return false;
}

AuctionRecommendPrice* AuctionMgr::GetRecommendPrice(UInt32 itemTypeID, UInt8 strengthen, UInt32 buffId, UInt8 enhanceType)
{
	AuctionRecommendPriceKey key(itemTypeID, strengthen, buffId, enhanceType);
	auto itr = m_recommendPrice.find(key);
	if (itr == m_recommendPrice.end()) return NULL;
	return itr->second;
}

bool AuctionMgr::SetAuctionGoodState(AuctionInfo& auctionInfo, UInt8 st)
{
	if (st < AGS_INVALID || st >= AGS_NUM)
	{
		ErrorLogStream << "set auction goodState err st = " << st << LogStream::eos;
		return false;
	}
	UInt8 oldGoodState = auctionInfo.goodState;
	if (auctionInfo.goodState != st)
	{
		UInt32 curr_time = (UInt32)CURRENT_TIME.Sec();
		auctionInfo.goodState = st;
		InfoLogStream << "set auction goodState guid=" << auctionInfo.guid << ", oldGoodState=" << oldGoodState
			<< ", newGoodState=" << auctionInfo.goodState << ", buyState=" << auctionInfo.buyState <<", curr_time=" << curr_time << ", onPublicTime=" << auctionInfo.onPublicTime 
			<< ", onSaleTime=" << auctionInfo.onSaleTime << LogStream::eos;
	}
	else
	{
		InfoLogStream << "set auction state fail st == auctionInfo.goodState, st =" << (UInt32)st << LogStream::eos;
	}
	return true;
}

bool AuctionMgr::SetAuctionBuyState(AuctionInfo& auctionInfo, UInt8 st)
{
	if (st < AGBS_INVALID || st >= AGBS_NUM)
	{
		ErrorLogStream << "set auction buyState err st = " << st << LogStream::eos;
		return false;
	}
	UInt8 oldBuyState = auctionInfo.buyState;
	if (auctionInfo.buyState != st)
	{
		UInt32 curr_time = (UInt32)CURRENT_TIME.Sec();
		auctionInfo.buyState = st;
		InfoLogStream << "set auction buyState guid=" << auctionInfo.guid << ", oldBuyState=" << oldBuyState
			<< ", newBuyState=" << auctionInfo.buyState << ", goodState=" << auctionInfo.goodState << ", curr_time=" << curr_time << ", onPublicTime=" << auctionInfo.onPublicTime
			<< ", onSaleTime=" << auctionInfo.onSaleTime << LogStream::eos;
	}
	else
	{
		InfoLogStream << "set auction state fail st == auctionInfo.goodState, st =" << (UInt32)st << LogStream::eos;
	}
	return true;
}

void AuctionMgr::PrintAuctionState(AuctionInfo& auctionInfo)
{
	InfoLogStream << "auction  guid=" << auctionInfo.guid  << ", isTreas=" << auctionInfo.isTreas << ", goodState=" << auctionInfo.goodState << ", buyState=" << auctionInfo.buyState
		<< ", curr_time=" << (UInt32)CURRENT_TIME.Sec() << ", onPublicTime=" << auctionInfo.onPublicTime << ", onSaleTime=" << auctionInfo.onSaleTime 
		<< LogStream::eos;
}

UInt32 AuctionMgr::CalcTransEqMPrice(AuctionInfo& auctionInfo)
{
	DebugLogStream << "guid : " << auctionInfo.guid << LogStream::eos;
	UInt32 bead_averprice_rate = 0;
	UInt32 magiccard_averprice_rate = 0;
	UInt32 guarantee_rate = 0;
	UInt32 grade_uplimint = 0;
	UInt32 cmp_bead_rate = 0;
	UInt32 cmp_magic_rate = 0;
	AuctionRecommendPrice::GetCalcTransMPriceRateNew(auctionInfo, bead_averprice_rate, magiccard_averprice_rate,
		guarantee_rate, grade_uplimint, cmp_bead_rate, cmp_magic_rate);

	if (!auctionInfo.item)
	{
		ErrorLogStream << "item of auction is null id:" << auctionInfo.guid << LogStream::eos;
		return 0;
	}

	if (IsMountedBeadOrMagic(auctionInfo)) //镶嵌了物品
	{
		DebugLogStream << "MountedBeadOrMagic guid : " << auctionInfo.guid << LogStream::eos;
		UInt32 beadAverPrice = 0;
		UInt32 magicCardAverPrice = 0;
		UInt32 mountedBeadId = 0;
		UInt32 mountedMagicId = 0;
		UInt32 mountedBeadBuffId = 0;
		UInt8 mountedMagicLv = 0;
		GetBeadAndMagicAverPriceForCalcTransEqMPrice(&auctionInfo, beadAverPrice, magicCardAverPrice, 
			mountedBeadId, mountedMagicId, mountedBeadBuffId, mountedMagicLv);
		return AuctionRecommendPrice::CalcTransEqMountedMPrice(auctionInfo, beadAverPrice, magicCardAverPrice,
			bead_averprice_rate, magiccard_averprice_rate, guarantee_rate, grade_uplimint, cmp_bead_rate, cmp_magic_rate);
	}
	else //裸装
	{
		DebugLogStream << "CalcTransEqBareMPrice guid : " << auctionInfo.guid << LogStream::eos;
		return AuctionRecommendPrice::CalcTransEqBareMPrice(auctionInfo.price, auctionInfo.item->m_QualityLv, guarantee_rate, grade_uplimint);
	}
}

void AuctionMgr::OnPlayerOnline(WSPlayer* player)
{
	if (!player) return;
	CLProtocol::WorldAuctionSyncPubPageIds protocol;
	protocol.pageIds = AuctionPageStructDataEntryMgr::Instance()->GetPubDisplayIds();
	protocol.shieldItemList = m_ShieldItemVec;
	player->SendProtocol(protocol);
	return;
}
AuctionInfo* AuctionMgr::GetAuctionInfo(WSPlayer* player, UInt64 auctionId)
{
	if (!player) return NULL;

	auto auctionInfo = _FindAuction(auctionId);
	if (!auctionInfo) return NULL;
	AuctionAttentMgr::Instance()->GetAuctionAttentInfo(*auctionInfo, player->GetID());
	return auctionInfo;
}

void AuctionMgr::FilterAuctions(std::vector<UInt64>& srcIds, std::vector<UInt64>& desIds, UInt8 goodState)
{
	for (auto id : srcIds)
	{
		auto auction = _FindAuction(id);
		if (auction && auction->goodState == goodState)
		{
			desIds.push_back(id);
		}
	}
}

UInt32 AuctionMgr::GetItemLatestDealTime(UInt32 itemId)
{
	UInt32 timestamp = 0;
	UInt32 strengths[] = { 0, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
	UInt8  enhanceTypes[] = { ENHANCE_INVALID, ENHANCE_STRENTH, ENHANCE_INTELLECT, ENHANCE_STAMINA, ENHANCE_SPIRIT };
	UInt32 size = sizeof(strengths) / sizeof(UInt32);
	for (UInt32 i = 0; i < size; ++i)
	{
		for (UInt32 j = 0; j < sizeof(enhanceTypes) / sizeof(UInt8); ++j)
		{
			AuctionRecommendPriceKey key(itemId, strengths[i], 0, enhanceTypes[j]);
		auto itr = m_recommendPrice.find(key);
		if (itr == m_recommendPrice.end() || !itr->second)
		{
			continue;
		}
		AuctionRecommendPrice* rcommedPrice = itr->second;
		if (rcommedPrice->recentPrice.empty())
		{
			continue;
		}
		AuctionItemRecentPrice& recentPrice = rcommedPrice->recentPrice.back();
		if (recentPrice.timestamp > timestamp)
		{
			timestamp = recentPrice.timestamp;
		}
	}
	}
	return timestamp;
}

bool AuctionMgr::SetAverPriceGM(UInt32 itemTypeID, UInt32 strengthen, UInt32 beadBuffId, UInt8 enhanceType,
	bool bSetAverPrice, UInt32 averPrice, bool bSetAverVisPrice, UInt32 averVisPrice, UInt32 transNum)
{
	ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(itemTypeID);
	if (!dataEntry)
	{
		ErrorLogStream << "dataEntry null itemTypeID : " << itemTypeID << LogStream::eos;
		return false;
	}

	bool isTreasure = false;
	if (dataEntry->isTreasure && WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_AUCTION_TREAS))
	{
		isTreasure = true;
	}

	if (!bSetAverPrice && !bSetAverVisPrice)
	{
		return false;
	}

	UInt32 addition = 0;
	UInt32 minRecommendPrice = 0;
	UInt32 minPrice = 0;
	UInt32 maxPrice = 0;
	if (isTreasure)
	{
		//UInt32 price_uplimit_rate = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_TUIJIANJIAGE_BEISHU);
		//if (price_uplimit_rate == 0) price_uplimit_rate = 5;
		UInt32 price_uplimit_rate = dataEntry->auctionMaxPriceRate;
		UInt32 price_uplimit = _GetRecommendPrice(itemTypeID, strengthen, enhanceType) * price_uplimit_rate;
		minPrice = 1000;
		maxPrice = price_uplimit;
		if (minPrice > maxPrice)
		{
			minPrice = maxPrice;
		}
		if (bSetAverPrice && averPrice != 0
			&& (averPrice < minPrice || averPrice > maxPrice))
		{
			ErrorLogStream << "averPrice < minPrice or averPrice > maxPrice, itemTypeID : " << itemTypeID << ", strengthen : " << strengthen << ", beadBuffId : " << beadBuffId
				<< ", isTreasure : " << isTreasure << ", averPrice : " << averPrice << ", minPrice : " << minPrice << ", maxPrice" << maxPrice << LogStream::eos;
			return false;
		}
	}
	else
	{
		UInt32 addition = GetAuctionStrengthAddition(strengthen);
		UInt32 minRecommendPrice = (UInt32)((UInt64)dataEntry->recommendPrice * dataEntry->auctionMinPriceRate / 100 * (100 + addition) / 100);
		if (bSetAverPrice && averPrice < minRecommendPrice && averPrice != 0)
		{
			ErrorLogStream << "averPrice < minRecommendPrice, itemTypeID : " << itemTypeID << ", strengthen : " << strengthen << ", addition : " << addition
				<< ", averPrice : " << averPrice << ", minRecommendPrice : " << minRecommendPrice << LogStream::eos;
			return false;
		}

		if (bSetAverVisPrice && averVisPrice < minRecommendPrice && averVisPrice != 0)
		{
			ErrorLogStream << "averVisPrice < minRecommendPrice, itemTypeID : " << itemTypeID << ", strengthen : " << strengthen << ", addition : " << addition
				<< ", averVisPrice : " << averVisPrice << ", minRecommendPrice : " << minRecommendPrice << LogStream::eos;
			return false;
		}
	}

	AuctionRecommendPrice* recommend = NULL;
	CLSaveCommand *pCmd = NULL;

	AuctionRecommendPriceKey key(itemTypeID, strengthen, beadBuffId, enhanceType);
	auto itr = m_recommendPrice.find(key);
	if (itr == m_recommendPrice.end() || !itr->second)
	{
		recommend = new AuctionRecommendPrice();
		recommend->guid = CLRecordClient::Instance()->GenGuid();
		recommend->itemTypeID = itemTypeID;
		recommend->strengthen = strengthen;
		recommend->bead_buff_id = beadBuffId;
		recommend->enhanceType = enhanceType;
		m_recommendPrice[key] = recommend;

		pCmd = CLRecordClient::Instance()->PrepareInsertCommand("t_auction_recommend_price", recommend->guid);
		pCmd->PutData("item_type_id", recommend->itemTypeID);
		pCmd->PutData("strengthen", recommend->strengthen);
		pCmd->PutData("bead_buff_id", recommend->bead_buff_id);
		pCmd->PutData("enhance_type", recommend->enhanceType);
	}
	else
	{
		recommend = itr->second;
		pCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_auction_recommend_price", recommend->guid, false);
	}

	if (recommend == NULL || pCmd == NULL)
	{
		ErrorLogStream << "recommend or pCmd is NULL" << LogStream::eos;
		if (pCmd)
		{
			CL_SAFE_DELETE(pCmd);
		}
		return false;
	}

	if (bSetAverPrice)
	{
		recommend->price = averPrice;
		recommend->price_update_time = (UInt32)CURRENT_TIME.Sec();
		pCmd->PutData("price", recommend->price);
		pCmd->PutData("price_update_time", recommend->price_update_time);
		recommend->recentPrice.clear();
		if (recommend->price != 0)
		{
			//UInt32 max_times = AuctionSystemValueDataEntryMgr::Instance()->GetSystemValue(ASVT_NOTTREAN_CALC_AVERPRICE_MAXTIMES);
			//if (max_times == 0) max_times = 100;
			UInt32 max_times = 0;
			if (isTreasure)
			{
				max_times = AuctionSystemValueDataEntryMgr::Instance()->GetSystemValue(ASVT_TREAN_CALC_AVERPRICE_MAXTIMES);
				max_times = max_times == 0 ? 20 : max_times;
			}
			else
			{
				max_times = AuctionSystemValueDataEntryMgr::Instance()->GetSystemValue(ASVT_NOTTREAN_CALC_AVERPRICE_MAXTIMES);
				max_times = max_times == 0 ? 100 : max_times;
			}
			if (transNum > max_times)
			{
				transNum = max_times;
			}
			transNum = transNum == 0 ? max_times : transNum;

			for (UInt32 i = 0; i < transNum; ++i)
			{
				recommend->AddRecentPrice(averPrice, 1, (UInt32)Avalon::Time::CurrentTime().Sec(), 0, 0, 0, isTreasure);
			}
		}
		pCmd->PutData("recent_price", recommend->GetRecentPriceStr());
	}
	if (bSetAverVisPrice)
	{
		recommend->visibility_price = averVisPrice;
		recommend->vis_price_update_time = (UInt32)CURRENT_TIME.Sec();
		pCmd->PutData("visibility_price", recommend->visibility_price);
		pCmd->PutData("vis_price_update_time", recommend->vis_price_update_time);
	}

	CLRecordClient::Instance()->SendCommand(pCmd);
	if (isTreasure)
	{
		InfoLogStream << "SetAverPriceGM success itemTypeID : " << itemTypeID << ", strengthen : " << strengthen << ", beadBuffId : " << beadBuffId
			<< ", isTreasure : " << isTreasure << ", bSetAverPrice : " << bSetAverPrice << ", averPrice : " << averPrice
			<< ", minPrice : " << minPrice << ", maxPrice" << maxPrice << LogStream::eos;
	}
	else
	{
		InfoLogStream << "SetAverPriceGM success itemTypeID : " << itemTypeID << ", strengthen : " << strengthen << ", beadBuffId : " << beadBuffId
			<< ", isTreasure : " << isTreasure << ", addition : " << addition << ", bSetAverPrice : " << bSetAverPrice << ", averPrice : " << averPrice
			<< ", bSetAverVisPrice : " << bSetAverVisPrice << ", averVisPrice : " << averVisPrice << ", minRecommendPrice : " << minRecommendPrice << LogStream::eos;
	}
	return true;
}

void AuctionMgr::OnDayChanged()
{
	UInt32 counterfee = GameParamMgr::Instance()->GetValue(GAME_PARAM_AUCTION_CURRDAY_COUNTERFEE);
	InfoLogStream << "counterfee : " << counterfee << LogStream::eos;
	GameParamMgr::Instance()->SetValue(GAME_PARAM_AUCTION_YESTERDAY_COUNTERFEE, counterfee);
	GameParamMgr::Instance()->SetValue(GAME_PARAM_AUCTION_CURRDAY_COUNTERFEE, 0);
}

void AuctionMgr::RecordCounterFee(UInt32 counterFee)
{
	UInt32 curvalue = GameParamMgr::Instance()->GetValue(GAME_PARAM_AUCTION_CURRDAY_COUNTERFEE);
	GameParamMgr::Instance()->SetValue(GAME_PARAM_AUCTION_CURRDAY_COUNTERFEE, curvalue + counterFee);
}

bool AuctionMgr::OnPlayerQueryItemRecentPricesList(WSPlayer *player, AuctionType type, UInt8 auctionState, UInt32 itemTypeId, UInt32 strengthen, UInt8 enhanceType)
{
	if (!player)
	{
		return false;
	}

	if (auctionState >= AGS_NUM || auctionState <= AGS_INVALID)
	{
		return false;
	}

	ItemDataEntry * itemdata = ItemDataEntryMgr::Instance()->FindEntry(itemTypeId);
	if (!itemdata)
	{
		InfoLogStream << "query auction price list ret itemdata null player(" << player->GetID() << ") itemTypeId(" << itemTypeId << ")" << LogStream::eos;
		return false;
	}

	if ((!itemdata->isTreasure || !WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_AUCTION_TREAS)) && auctionState == AGS_PUBLIC)
	{
		InfoLogStream << "query auction price list fail not treasure auctionState : " << auctionState << LogStream::eos;
		return false;
	}

	AuctionPricesStForSwtich* priceCanch = GetAuctionPriceListCacheForSwitch(itemTypeId, strengthen, 0, enhanceType, *itemdata, auctionState);
	if (priceCanch)
	{
		CLProtocol::WorldAuctionQueryItemPriceListRes res = priceCanch->res;
		res.strengthen = strengthen;
		if (enhanceType > ENHANCE_INVALID && res.actionItems.size() == 0)
		{
			AuctionPricesStForSwtich* priceCanch_ = GetAuctionPriceListCacheForSwitch(itemTypeId, strengthen, 0, ENHANCE_INVALID, *itemdata, auctionState);
			if (priceCanch_ && priceCanch->res.actionItems.size() > 0)
			{
				res.actionItems = priceCanch->res.actionItems;
	}
			else if (priceCanch_ == NULL)
			{
				if (WSApplication::Instance()->IsAuctionOpUseMem())
				{
					UpdateAuctionPriceCacheForExplaceHzEquipFromMem(player, type, itemTypeId, strengthen, 0, ENHANCE_INVALID, auctionState, APLOP_SWITCH, enhanceType);
				}
				else
				{
					UpdateAuctionPriceCacheForExplaceHzEquip(player, type, itemTypeId, strengthen, 0, ENHANCE_INVALID, auctionState, APLOP_SWITCH, enhanceType);
				}
				return true;
			}
		}
		player->SendProtocol(res);
		return true;
	}

	if (WSApplication::Instance()->IsAuctionOpUseMem())
	{
		QueryItemRecentPricesFormMemForSwitch(player, type, auctionState, itemTypeId, strengthen, enhanceType);
	}
	else
	{
		CLProtocol::AuctionQueryCondition cond;
		cond.itemTypeID = itemTypeId;
		cond.goodState = auctionState;
		CLSelectCommand* command = CLRecordClient::Instance()->PrepareSelectCommand("t_auction_new");
		command->PutColumn("itemid");
		GenSqlByCondition(player, command, cond);

		if (itemdata->subType == ST_MAGICCARD)
		{
			command->PutCond("strengthen", strengthen, "=");
		}
		else
		{
			if (strengthen > AUCTION_SPECIAL_AVERAGE_PRICE_STRENGTHEN_LEVEL)
			{
				command->PutCond("strengthen", strengthen, "=");
			}
			else
			{
				if (!WSApplication::Instance()->GetAuctionSqlInModify())
				{
					if (enhanceType > ENHANCE_INVALID) //红字装备
					{
						UInt8 min = 0;
						UInt8 max = 0;
						GetStrengthenRangeForHzEquip(strengthen, min, max);
						command->PutCond("strengthen", min, ">=");
						command->PutCond("strengthen", max, "<=");
					}
					else
					{
						command->PutCond("strengthen", 0, ">=");
						command->PutCond("strengthen", 10, "<=");
					}
				}
				else
				{
					if (enhanceType > ENHANCE_INVALID) //红字装备
					{
						UInt8 min = 0;
						UInt8 max = 0;
						GetStrengthenRangeForHzEquip(strengthen, min, max);
						std::stringstream stream;
						stream << "(";
						for (UInt32 i = min; i <= max; ++i)
						{
							stream << i;
							if (i != max)
							{
								stream << ",";
							}
						}
						stream << ")";

						command->PutCond("strengthen", stream.str(), "in");
					}
					else
					{
						command->PutCond("strengthen", "(0,1,2,3,4,5,6,7,8,9,10)", "in");
					}
				}
			}
		}

		std::stringstream orderStream;
		orderStream << "order by price / num asc";
		command->SetOrder(orderStream.str());

		UInt32 limit = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_PRICESLIST_NUM);
		limit = (limit == 0) ? 10 : limit;
		std::ostringstream limitoss;
		limitoss << "limit " << limit;
		command->SetLimit(limitoss.str());

		CLRecordClient::Instance()->SendCommand(command, new SelectAuctionSalePriceListCallback(player->GetID(), type, auctionState, itemTypeId, strengthen, enhanceType));
	}

	return true;
}

bool AuctionMgr::OnQueryAuctionSalePriceListRet(ObjID_t roleId, std::vector<ObjID_t>& auctionIds, AuctionType type, UInt8 auctionState, UInt32 itemTypeId, UInt32 strengthen, UInt8 enhanceType)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(roleId);
	if (!player)
	{
		InfoLogStream << "query auction prices list ret, player(" << roleId << ") offline." << LogStream::eos;
		return false;
	}
	
	CLProtocol::WorldAuctionQueryItemPriceListRes res;
	res.type = type;
	res.itemTypeId = itemTypeId;
	res.strengthen = strengthen;
	res.auctionState = auctionState;
	
	for (auto auctionId : auctionIds)
	{
		AuctionInfo* auctionInfo = _FindAuction(auctionId);
		if (!auctionInfo)
		{
			continue;
		}

		res.actionItems.push_back(*(AuctionBaseInfo*)auctionInfo);
	}

	ItemDataEntry * itemdata = ItemDataEntryMgr::Instance()->FindEntry(itemTypeId);
	if (!itemdata)
	{
		InfoLogStream << "query auction prices list ret itemdata null player(" << roleId << ") itemTypeId(" << itemTypeId << ")" << LogStream::eos;
		return false;
	}

	UpdateAuctionPriceListCacheForSwitch(itemTypeId, strengthen, 0, enhanceType, res, *itemdata, auctionState);

	if (enhanceType > ENHANCE_INVALID && res.actionItems.size() == 0)
	{
		AuctionPricesStForSwtich* priceCanch_ = GetAuctionPriceListCacheForSwitch(itemTypeId, strengthen, 0, ENHANCE_INVALID, *itemdata, auctionState);
		if (priceCanch_ && priceCanch_->res.actionItems.size() > 0)
		{
			res.actionItems = priceCanch_->res.actionItems;
		}
		else if (priceCanch_ == NULL)
		{
			UpdateAuctionPriceCacheForExplaceHzEquip(player, type, itemTypeId, strengthen, 0, ENHANCE_INVALID, auctionState, APLOP_SWITCH, enhanceType);
			return true;
		}
	}

	player->SendProtocol(res);
	return true;
}

UInt32 AuctionMgr::GetSysRecoveSurplus(UInt8 isTreas)
{
	UInt32 recoveLimit = 0;
	UInt32 currRecoveNum = 0;
	UInt32 currTm = (UInt32)CURRENT_TIME.Sec();
	if (isTreas)
	{
		recoveLimit = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_RECOVE_TREANS_NUM);
		recoveLimit = (recoveLimit == 0) ? 1000 : recoveLimit;
		UInt32 last_timestamp = GameParamMgr::Instance()->GetValue(PARAM_NAME_AUCTION_RECOVE_TREAS_TIMESTAMP);
		if (!IsSameDay(Avalon::Time(UInt64(last_timestamp) * 1000), Avalon::Time(UInt64(currTm) * 1000)))
		{
			GameParamMgr::Instance()->SetValue(PARAM_NAME_AUCTION_RECOVE_TREAS_NUM, 0);
			GameParamMgr::Instance()->SetValue(PARAM_NAME_AUCTION_RECOVE_TREAS_TIMESTAMP, currTm);
		}
		currRecoveNum = GameParamMgr::Instance()->GetValue(PARAM_NAME_AUCTION_RECOVE_TREAS_NUM);
	}
	else
	{
		recoveLimit = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_RECOVE_NOTTREANS_NUM);
		recoveLimit = (recoveLimit == 0) ? 1000 : recoveLimit;
		UInt32 last_timestamp = GameParamMgr::Instance()->GetValue(PARAM_NAME_AUCTION_RECOVE_NOT_TREAS_TIMESTAMP);
		if (!IsSameDay(Avalon::Time(UInt64(last_timestamp) * 1000), Avalon::Time(UInt64(currTm) * 1000)))
		{
			GameParamMgr::Instance()->SetValue(PARAM_NAME_AUCTION_RECOVE_NOT_TREAS_NUM, 0);
			GameParamMgr::Instance()->SetValue(PARAM_NAME_AUCTION_RECOVE_NOT_TREAS_TIMESTAMP, currTm);
		}
		currRecoveNum = GameParamMgr::Instance()->GetValue(PARAM_NAME_AUCTION_RECOVE_NOT_TREAS_NUM);
	}

	if (currRecoveNum < recoveLimit)
	{
		return recoveLimit - currRecoveNum;
	}
	return 0;
}

void AuctionMgr::IncRecoveryNum(UInt8 IsTreas, UInt32 num)
{
	if (IsTreas)
	{
		UInt32 currRecoveNum = GameParamMgr::Instance()->GetValue(PARAM_NAME_AUCTION_RECOVE_TREAS_NUM);
		GameParamMgr::Instance()->SetValue(PARAM_NAME_AUCTION_RECOVE_TREAS_NUM, currRecoveNum + num);
		GameParamMgr::Instance()->SetValue(PARAM_NAME_AUCTION_RECOVE_TREAS_TIMESTAMP, (UInt32)CURRENT_TIME.Sec());
	}
	else
	{
		UInt32 currRecoveNum = GameParamMgr::Instance()->GetValue(PARAM_NAME_AUCTION_RECOVE_NOT_TREAS_NUM);
		GameParamMgr::Instance()->SetValue(PARAM_NAME_AUCTION_RECOVE_NOT_TREAS_NUM, currRecoveNum + num);
		GameParamMgr::Instance()->SetValue(PARAM_NAME_AUCTION_RECOVE_NOT_TREAS_TIMESTAMP, (UInt32)CURRENT_TIME.Sec());
	}
}

bool AuctionMgr::IsShouldRecovery(AuctionInfo& auctionInfo)
{
	if (auctionInfo.isRecoveHandled || auctionInfo.num <= 0)
	{
		return false;
	}

	//UInt32 sysRecovSurplus = GetSysRecoveSurplus(auctionInfo.isTreas);
	//if (sysRecovSurplus <= 0)
	//{
	//	return false;
	//}

	AuctionRecoveItemDataEntry* recoveEntry = AuctionRecoveItemDataEntryMgr::Instance()->FindEntry(auctionInfo.itemTypeId);
	if (!recoveEntry)
	{
		return false;
	}

	auto pDataEntry = ItemDataEntryMgr::Instance()->FindEntry(auctionInfo.itemTypeId);
	if (!pDataEntry)
	{
		return false;
	}

	UInt32 unitPrice = auctionInfo.price / auctionInfo.num;
	UInt32 priceLimit = pDataEntry->recommendPrice * recoveEntry->sysRecoPriceRate / 100;

	if (unitPrice > priceLimit)
	{
		return false;
	}

	return true;
}

void AuctionMgr::AddOneShouldRcoAuction(ObjID_t auctionId)
{
	m_shouldRecoveryAuctions.insert(auctionId);
}

void AuctionMgr::RecoveryAuctions()
{
	if (m_shouldRecoveryAuctions.empty())
	{
		return;
	}

	std::vector<ObjID_t> removeIds;
	for (auto id : m_shouldRecoveryAuctions)
	{
		AuctionInfo* auctionInfo = _FindAuction(id);
		if (!auctionInfo || auctionInfo->isRecoveHandled)
		{
			removeIds.push_back(id);
			continue;
		}

		bool handled = false;
		HandleForRecovery(*auctionInfo, true, handled);
		if (handled)
		{
			removeIds.push_back(id);
		}
	}

	for (auto id : removeIds)
	{
		m_shouldRecoveryAuctions.erase(id);
	}
}

bool AuctionMgr::HandleForRecovery(AuctionInfo& auctionInfo, bool canDelete, bool& handled)
{
	if (auctionInfo.onSaleTime > (UInt32)CURRENT_TIME.Sec())
	{
		return false;
	}

	if (auctionInfo.isRecoveHandled || auctionInfo.num <= 0)
	{
		return false;
	}


	handled = true;
	auctionInfo.isRecoveHandled = 1;
	// 保存到数据库
	CLUpdateCommand *pCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_auction_new", auctionInfo.guid, true);
	pCmd->PutData("recove_handled", 1);
	CLRecordClient::Instance()->SendCommand(pCmd);

	if (auctionInfo.num == 0)
	{
		return false;
	}

	UInt32 sysRecovSurplus = GetSysRecoveSurplus(auctionInfo.isTreas);
	if (sysRecovSurplus <= 0)
	{
		return false;
	}
	
	AuctionRecoveItemDataEntry* recoveEntry = AuctionRecoveItemDataEntryMgr::Instance()->FindEntry(auctionInfo.itemTypeId);
	if (!recoveEntry)
	{
		return false;
	}

	auto pDataEntry = ItemDataEntryMgr::Instance()->FindEntry(auctionInfo.itemTypeId);
	if (!pDataEntry)
	{
		return false;
	}

	UInt32 unitPrice = auctionInfo.price / auctionInfo.num;
	UInt32 priceLimit = pDataEntry->recommendPrice * recoveEntry->sysRecoPriceRate / 100;

	if (unitPrice > priceLimit)
	{
		return false;
	}

	//回收数量
	UInt32 recoveNum = (auctionInfo.num <= sysRecovSurplus) ? auctionInfo.num : sysRecovSurplus;
	IncRecoveryNum(auctionInfo.isTreas, recoveNum);

	UInt32 price = unitPrice * recoveNum;

	UInt32 uGetMoneyItemId = _GetItemTypeIDByPriceType((MoneyType)auctionInfo.pricetype);
	//UInt32 counterFee = auctionInfo.counterFee / auctionInfo.num * recoveNum; //手续费
	UInt32 counterFee = GetCounterFeeNew(auctionInfo.ownerVipLev, AT_ITEM, price);
	RecordCounterFee(counterFee);

	std::vector<ItemReward>	vecitems;
	UInt32 money = price;
	if (money >= counterFee)
	{
		money -= counterFee;
	}
	else
	{
		money = 0;
	}

	if (auctionInfo.isTreas && WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_AUCTION_TREAS)) //珍品
	{
		std::string itemName = "";
		ItemDataEntry	*pDataEntry = ItemDataEntryMgr::Instance()->FindEntry(auctionInfo.itemTypeId);
		if (pDataEntry != NULL)
		{
			itemName = pDataEntry->name;
		}
		UInt64 auctionTransId = 0;
		UInt32 deposit = GetDepositByAuctionPrice(price);

		char buffer[1024];
		memset(buffer, 0, sizeof(buffer));

		std::string strSenderName;
		std::string	strTitle;
		std::string	strContent;
		std::string strReason;
		std::vector<ItemReward> depositItems;
		std::vector<WSItem*> vecitems;

		// 发送者
		SysNotifyMgr::Instance()->MakeNotify(9960, buffer, sizeof(buffer));
		strSenderName = buffer;

		SysNotifyMgr::Instance()->MakeNotify(9959, buffer, sizeof(buffer));
		strTitle = buffer;

		//std::string itemlinkStr;
		//MsgPackage::GetItemMsgTag(itemlinkStr, 0, auctionInfo->item ? auctionInfo->item->GetDataID() : 0, auctionInfo->strengthen);

		SysNotifyMgr::Instance()->MakeNotify(9958, buffer, sizeof(buffer), itemName.c_str(),
			TimeUtil::TimestampToStandarFormatNoSpace((UInt32)CURRENT_TIME.Sec()), price);
		strContent = buffer;

		strReason = GetReason(SOURCE_TYPE_ACTION_BUY, auctionInfo.guid);

		if (MailBox::SendSysMail(auctionInfo.owner, strSenderName, strTitle, strContent, strReason, depositItems, vecitems) != ErrorCode::SUCCESS)
		{
			// 发送邮件失败
			ErrorLogStream << "send aution treas transction fail (" << auctionInfo.guid << ") mail to role(" << auctionInfo.owner << ") title("
				<< strTitle << ") failed." << LogStream::eos;
		}

		auctionTransId = AuctionTransRecordMgr::Instance()->AddNewTransaction(0, &auctionInfo, recoveNum, itemName, deposit, 0, 0, 1, 0);
		InfoLogStream << "sys recovery treas auction guid : " << auctionInfo.guid << ", transId : " << auctionTransId << ", isTreas : " << auctionInfo.isTreas << ", owner : "
			<< auctionInfo.owner << ", itemId : " << auctionInfo.itemTypeId << ", price : " << auctionInfo.price << ", num : "
			<< auctionInfo.num << ", recoveNum : " << recoveNum << ",  priceLimit : " << priceLimit << LogStream::eos;
	}
	else
	{
		char buffer[1024];
		memset(buffer, 0, sizeof(buffer));

		SysNotifyMgr::Instance()->MakeNotify(1047, buffer, sizeof(buffer));
		std::string strSenderName(buffer);

		std::string	strTitle;
		std::string	strContent;
		std::string strReason;

		std::string strengthenStr = "";
		if (auctionInfo.strengthen != 0)
		{
			strengthenStr = Avalon::StringUtil::ConvertToString<UInt32>(auctionInfo.strengthen);
		}

		std::string itemName = "";
		itemName = pDataEntry->name;

		if (auctionInfo.num == recoveNum)
		{
			UInt32 notifyId = auctionInfo.strengthen != 0 ? 2224 : 2211;
			SysNotifyMgr::Instance()->MakeNotify(notifyId, buffer, sizeof(buffer), strengthenStr.c_str(), itemName.c_str(), recoveNum, price, counterFee, money);
			strContent = buffer;
		}
		else
		{
			UInt32 notifyId = auctionInfo.strengthen != 0 ? 2228 : 2219;
			SysNotifyMgr::Instance()->MakeNotify(notifyId, buffer, sizeof(buffer), strengthenStr.c_str(), itemName.c_str(), recoveNum, price, counterFee, money);
			strContent = buffer;
		}

		if (money > 0)
		{
			vecitems.push_back(ItemReward(uGetMoneyItemId, money, false, 0));
		}

		SysNotifyMgr::Instance()->MakeNotify(2210, buffer, sizeof(buffer));
		strTitle = buffer;
		strReason = GetReason(SOURCE_TYPE_ACTION_SYS_RECOVERY, auctionInfo.guid, auctionInfo.item ? auctionInfo.item->m_DataId : 0);

		if (MailBox::SendSysMail(auctionInfo.owner, strSenderName, strTitle, strContent, strReason, vecitems) != ErrorCode::SUCCESS)
		{
			// 发送邮件失败，这里出错就不return了
			ErrorLogStream << "send auction(" << auctionInfo.guid << ") mail to role(" << auctionInfo.owner << ") title("
				<< strTitle << ") money(" << money << " failed." << LogStream::eos;
		}

		InfoLogStream << "sys recovery auction guid : " << auctionInfo.guid << ", isTreas : " << auctionInfo.isTreas << ", owner : "
			<< auctionInfo.owner << ", itemId : " << auctionInfo.itemTypeId << ", price : " << auctionInfo.price << ", num : "
			<< auctionInfo.num << ", recoveNum : " << recoveNum << ",  priceLimit : " << priceLimit << LogStream::eos;
	}

	auctionInfo.num -= recoveNum;

	Sys::SendUdpLog("auction_sys_recover", LOG_AUCTION_SYSRECOVER, GAME_VERSION.c_str(), ZONE_ID, UInt32(CURRENT_TIME.Sec()), PLATFORM.c_str(),
		auctionInfo.isTreas, auctionInfo.itemTypeId, recoveNum, unitPrice);

	if (auctionInfo.num == 0)
	{
		if (canDelete)
		{
			UInt8 type = auctionInfo.type;
			ObjID_t playerId = auctionInfo.owner;
			ObjID_t auctionGuid = auctionInfo.guid;
			_DeleteAuctionInfo(&auctionInfo);
			WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(playerId);
			if (player)
			{
				CLProtocol::WorldAuctionNotifyRefresh refresh;
				refresh.type = type;
				refresh.reason = SRR_SYS_RECOVERY;
				refresh.auctGuid = auctionGuid;
				player->SendProtocol(refresh);
			}
		}
	}
	else
	{
		auctionInfo.item->m_Num = auctionInfo.num;
		auctionInfo.price -= price;
		auctionInfo.counterFee -= counterFee;

		CLUpdateCommand *pCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_auction_new", auctionInfo.guid, true);
		pCmd->PutData("num", auctionInfo.num);
		pCmd->PutData("price", auctionInfo.price);
		pCmd->PutData("counter_fee", auctionInfo.counterFee);
		CLRecordClient::Instance()->SendCommand(pCmd);
	}
 	return true;
}

bool AuctionMgr::OnSelectCheckRecovedNoTreasAuctionRet(CLRecordCallback *pCallback)
{
	while (pCallback->NextRow())
	{
		ObjID_t id = pCallback->GetKey();
		AuctionInfo* auctionInfo = _FindAuction(id);
		if (!auctionInfo)
		{
			ErrorLogStream << "auction(" << id << ") should check sys recovery but can't find it." << LogStream::eos;
			continue;
		}

		DebugLogStream << "auction(" << id << ") is  handled for check sys recovery" << LogStream::eos;
		if (!auctionInfo->isRecoveHandled && !auctionInfo->isTreas)
		{
			bool handled = false;
			HandleForRecovery(*auctionInfo, true, handled);
		}

		OnMagicOnsale(*auctionInfo);
	}

	return true;
}

AuctionPricesSt* AuctionMgr::GetAuctionPriceListCache(UInt32 itemTypeId, UInt32 strengthen, UInt32 beadBuffId, UInt8 enhanceType, ItemDataEntry& itemData, UInt8 goodState)
{
	strengthen = GetStrengthenForAverPrice(itemData, strengthen, enhanceType);

	UInt8 isReadEquip = (enhanceType > ENHANCE_INVALID) ? 1 : 0;

	AuctionPricesKey key(itemTypeId, strengthen, beadBuffId, isReadEquip, goodState);
	MapAuctionPrices::iterator iter = m_pircesCacheOfOnSale.find(key);
	if (iter == m_pircesCacheOfOnSale.end())
	{
		DebugLogStream << "itemTypeId(" << itemTypeId << "), strengthen( " << strengthen << "), beadBuffId("
			<< beadBuffId << "), goodState( " << goodState << "), not finded" << LogStream::eos;
		return NULL;
	}

	UInt32 now = (UInt32)Avalon::Time::CurrentTime().Sec();
	if (iter->second.timeStmap > now)
	{
		DebugLogStream << "itemTypeId(" << itemTypeId << "), strengthen( " << strengthen << "), beadBuffId("
			<< beadBuffId << "), goodState( " << goodState << "), finded" << LogStream::eos;
		return &(iter->second);
	}

	DebugLogStream << "itemTypeId(" << itemTypeId << "), strengthen( " << strengthen << "), beadBuffId("
		<< beadBuffId << "), goodState( " << goodState << "), not finded" << LogStream::eos;
	return NULL;
}

void AuctionMgr::UpdateAuctionPriceListCache(UInt32 itemTypeId, UInt32 strengthen, UInt32 beadBuffId, UInt8 enhanceType, CLProtocol::WorldAuctionQueryItemPricesRes& res, ItemDataEntry& itemData, UInt8 goodState)
{
	strengthen = GetStrengthenForAverPrice(itemData, strengthen, enhanceType);

	UInt8 isReadEquip = (enhanceType > ENHANCE_INVALID) ? 1 : 0;

	AuctionPricesKey key(itemTypeId, strengthen, beadBuffId, isReadEquip, goodState);
	AuctionPricesSt st;
	
	UInt32 update_interal = WSApplication::Instance()->GetAuctionPriceListCacheTime();
	if (update_interal == 0)
	{
		update_interal = 10;
	}
	st.timeStmap = (UInt32)Avalon::Time::CurrentTime().Sec() + update_interal;
	st.res = res;

	m_pircesCacheOfOnSale[key] = st;
	DebugLogStream << "itemTypeId(" << itemTypeId << "), strengthen( " << strengthen << "), beadBuffId("
		<< beadBuffId << "), goodState( " << goodState << "), update price list cache" << LogStream::eos;
}

AuctionPricesStForSwtich* AuctionMgr::GetAuctionPriceListCacheForSwitch(UInt32 itemTypeId, UInt32 strengthen, UInt32 beadBuffId, UInt8 enhanceType, ItemDataEntry& itemData, UInt8 goodState)
{
	strengthen = GetStrengthenForAverPrice(itemData, strengthen, enhanceType);

	UInt8 isReadEquip = (enhanceType > ENHANCE_INVALID) ? 1 : 0;

	AuctionPricesKey key(itemTypeId, strengthen, beadBuffId, isReadEquip, goodState);
	MapAuctionPricesForSwtich::iterator iter = m_pircesCacheOfSwitch.find(key);
	if (iter == m_pircesCacheOfSwitch.end())
	{
		DebugLogStream << "itemTypeId(" << itemTypeId << "), strengthen( " << strengthen << "), beadBuffId("
			<< beadBuffId << "), goodState( " << goodState << "), not finded" << LogStream::eos;
		return NULL;
	}

	UInt32 now = (UInt32)Avalon::Time::CurrentTime().Sec();
	if (iter->second.timeStmap > now)
	{
		DebugLogStream << "itemTypeId(" << itemTypeId << "), strengthen( " << strengthen << "), beadBuffId("
			<< beadBuffId << "), goodState( " << goodState << "), finded" << LogStream::eos;
		return &(iter->second);
	}

	DebugLogStream << "itemTypeId(" << itemTypeId << "), strengthen( " << strengthen << "), beadBuffId("
		<< beadBuffId << "), goodState( " << goodState << "), not finded" << LogStream::eos;
	return NULL;
}

void AuctionMgr::UpdateAuctionPriceListCacheForSwitch(UInt32 itemTypeId, UInt32 strengthen, UInt32 beadBuffId, UInt8 enhanceType, CLProtocol::WorldAuctionQueryItemPriceListRes& res, ItemDataEntry& itemData, UInt8 goodState)
{
	strengthen = GetStrengthenForAverPrice(itemData, strengthen, enhanceType);

	UInt8 isReadEquip = (enhanceType > ENHANCE_INVALID) ? 1 : 0;
	AuctionPricesKey key(itemTypeId, strengthen, beadBuffId, isReadEquip, goodState);
	AuctionPricesStForSwtich st;

	UInt32 update_interal = WSApplication::Instance()->GetAuctionPriceListCacheTime();
	if (update_interal == 0)
	{
		update_interal = 10;
	}
	st.timeStmap = (UInt32)Avalon::Time::CurrentTime().Sec() + update_interal;
	st.res = res;

	m_pircesCacheOfSwitch[key] = st;
	DebugLogStream << "itemTypeId(" << itemTypeId << "), strengthen( " << strengthen << "), beadBuffId("
		<< beadBuffId << "), goodState( " << goodState << "), update price list cache" << LogStream::eos;
}

void AuctionMgr::AddWaiteOnsaleMagic(ObjID_t auctionId)
{
	m_waiteOnsaleMagics.insert(auctionId);
}

void AuctionMgr::HandleWaiteOnsaleMagics()
{
	if (m_waiteOnsaleMagics.empty())
	{
		return;
	}

	UInt32 curtime = (UInt32)CURRENT_TIME.Sec();
	std::vector<ObjID_t> removeIds;
	for (auto id : m_waiteOnsaleMagics)
	{
		AuctionInfo* auctionInfo = _FindAuction(id);
		if (!auctionInfo || auctionInfo->isRecordInOnsaleNum)
		{
			removeIds.push_back(id);
			continue;
		}

		if (auctionInfo->onSaleTime > curtime)
		{
			continue;
		}

		OnMagicOnsale(*auctionInfo);
		removeIds.push_back(id);

	}

	for (auto id : removeIds)
	{
		m_waiteOnsaleMagics.erase(id);
	}
}

void AuctionMgr::OnMagicOnsale(AuctionInfo& auctionInfo)
{
	if (auctionInfo.isRecordInOnsaleNum)
	{
		return;
	}
	auctionInfo.isRecordInOnsaleNum = 1;
	IncMagicOnsaleNum(auctionInfo.itemTypeId, auctionInfo.strengthen, auctionInfo.num);
}

void AuctionMgr::IncMagicOnsaleNum(UInt32 itemTypeId, UInt8 strengthen, UInt32 num)
{
	MagicOnsaleKey key(itemTypeId, strengthen);
	auto iter = m_magicOnsaleNum.find(key);
	if (iter == m_magicOnsaleNum.end())
	{
		m_magicOnsaleNum[key] = num;
	}
	else
	{
		m_magicOnsaleNum[key] += num;
	}
}

void AuctionMgr::DecMagicOnsaleNum(UInt32 itemTypeId, UInt8 strengthen, UInt32 num)
{
	MagicOnsaleKey key(itemTypeId, strengthen);
	auto iter = m_magicOnsaleNum.find(key);
	if (iter == m_magicOnsaleNum.end())
	{
		return;
	}
	else
	{
		if (m_magicOnsaleNum[key] < num)
		{
			m_magicOnsaleNum[key] = 0;
		}
		else
		{
			m_magicOnsaleNum[key] -= num;
		}
	}
}

UInt32 AuctionMgr::GetMagicOnsaleNum(UInt32 itemTypeId, UInt8 strengthen)
{
	MagicOnsaleKey key(itemTypeId, strengthen);
	auto iter = m_magicOnsaleNum.find(key);
	if (iter == m_magicOnsaleNum.end())
	{
		return 0;
	}
	return m_magicOnsaleNum[key];
}

void AuctionMgr::RecordMagicOnsaleNum(AuctionInfo& auctionInfo)
{
	if (auctionInfo.isRecordInOnsaleNum)
	{
		return;
	}
	ItemDataEntry* itemData = ItemDataEntryMgr::Instance()->FindEntry(auctionInfo.itemTypeId);
	if (!itemData || itemData->subType != ST_MAGICCARD)
	{
		return;
	}

	auctionInfo.isRecordInOnsaleNum = 1;

	IncMagicOnsaleNum(auctionInfo.itemTypeId, auctionInfo.strengthen, auctionInfo.num);
	return;
}

void AuctionMgr::OnMagicOffsale(AuctionInfo& auctionInfo)
{
	if (!auctionInfo.isRecordInOnsaleNum)
	{
		return;
	}

	auctionInfo.isRecordInOnsaleNum = 0;

	DecMagicOnsaleNum(auctionInfo.itemTypeId, auctionInfo.strengthen, auctionInfo.num);
	return;
}

void AuctionMgr::OnMagicTimeOut(AuctionInfo& auctionInfo)
{
	if (!auctionInfo.isRecordInOnsaleNum)
	{
		return;
	}

	auctionInfo.isRecordInOnsaleNum = 0;

	DecMagicOnsaleNum(auctionInfo.itemTypeId, auctionInfo.strengthen, auctionInfo.num);
	return;
}

void AuctionMgr::OnMagicBuy(AuctionInfo& auctionInfo, UInt32 num)
{
	if (!auctionInfo.isRecordInOnsaleNum)
	{
		return;
	}
	ItemDataEntry* itemData = ItemDataEntryMgr::Instance()->FindEntry(auctionInfo.itemTypeId);
	if (!itemData || itemData->subType != ST_MAGICCARD)
	{
		return;
	}
	DecMagicOnsaleNum(auctionInfo.itemTypeId, auctionInfo.strengthen, num);
	return;
}

bool AuctionMgr::QueryMagicOnsales(WSPlayer *player, UInt32 itemTypeId)
{
	if (!player)
	{
		return false;
	}

	CLProtocol::WorldAuctionQueryMagicOnsalesRes res;
	MagicCardData* upgradeCardData = MagicCardDataMgr::Instance()->FindEntry(itemTypeId);
	if (!upgradeCardData)
	{
		ErrorLogStream << "cant find magic card data, itemTypeId : " << itemTypeId << LogStream::eos;
		return false;
	}

	for (UInt8 i = 0; i <= upgradeCardData->maxLevel; ++i)
	{
		UInt32 magicCardNum = GetMagicOnsaleNum(itemTypeId, i);
		res.magicOnsales.push_back(AuctionMagicOnsale(i, magicCardNum));
	}
	res.code = ErrorCode::SUCCESS;
	player->SendProtocol(res);
	return true;
}

bool AuctionMgr::OnSelectTimeoutOffsaleAuctionRet(CLRecordCallback *pCallback)
{
	if (!pCallback)
	{
		return false;
	}

	UInt32 expireDay = AuctionSystemValueDataEntryMgr::Instance()->GetSystemValue(ASVT_EXPIRE_TIME_AUTO_OFFSALE);
	expireDay = (expireDay == 0) ? 14 : expireDay;

	while (pCallback->NextRow())
	{
		ObjID_t id = pCallback->GetKey();
		AuctionInfo* auctionInfo = _FindAuction(id);
		if (!auctionInfo)
		{
			ErrorLogStream << "auction(" << id << ") expire should auto offsale but can't find it." << LogStream::eos;
			continue;
		}

		//if (auctionInfo->duetime > currTime - expireDay * 24 * 3600)
		//{
		//	ErrorLogStream << "auction(" << id << ") is not should off sale duetime : " << auctionInfo->duetime << ", currTime : " << currTime
		//		<< ", expireDay : " << expireDay << LogStream::eos;
		//	continue;
		//}

		DebugLogStream << "auction(" << id << ") is  offsale for expire" << LogStream::eos;
		
		std::vector<WSItem*>	vecitems;
		vecitems.push_back(auctionInfo->item);
		auctionInfo->item = NULL;

		std::vector<ItemReward> depositItems;
		UInt32 deposit = 0;
		// 把押金退回去
		deposit = GetDepositByAuctionPrice(auctionInfo->price);
		if (deposit > 0)
		{
			UInt32 depositItemTypeId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_DEPOSIT_ITEM_ID);
			depositItems.push_back(ItemReward(depositItemTypeId, deposit, true, 0));
		}

		UInt64 mailId = 0;
		ObjID_t roleId = auctionInfo->owner;
		UInt32 num = auctionInfo->num;
		
		char buffer[1024];
		memset(buffer, 0, sizeof(buffer));

		// 发送者
		SysNotifyMgr::Instance()->MakeNotify(1047, buffer, sizeof(buffer));
		std::string strSenderName(buffer);
		SysNotifyMgr::Instance()->MakeNotify(2213, buffer, sizeof(buffer));
		std::string strTitle = buffer;
		SysNotifyMgr::Instance()->MakeNotify(10036, buffer, sizeof(buffer));
		std::string strContent = buffer;

		std::string strReason = GetReason(SOURCE_TYPE_ACTION_TIMEOUT, auctionInfo->item ? auctionInfo->item->m_DataId : 0, auctionInfo->type);
		if (MailBox::SendSysMailGetMailID(roleId, strSenderName, strTitle, strContent, strReason, depositItems, vecitems, mailId) != ErrorCode::SUCCESS)
		{
			// 发送邮件失败
			ErrorLogStream << "offsale for expire send auction(" << auctionInfo->guid << ") mail to role(" << roleId << ") title("
				<< strTitle << ") failed." << LogStream::eos;
			return false;
		}

		ObjID_t itemId = auctionInfo->item ? auctionInfo->item->GetID() : 0;
		UInt32 itemDataId = auctionInfo->itemTypeId;
		Sys::SendUdpLog("auction_down", LOG_AUCTION_DOWN_FORMAT, roleId, ASIT_TIMEOUT, auctionInfo->type, itemId, itemDataId, num);
		_DeleteAuctionInfo(auctionInfo);
	}

	return true;
}

void AuctionMgr::OnTickNoTreasAuctions()
{
	RecoveryAuctions();
	HandleWaiteOnsaleMagics();
}

UInt8 AuctionMgr::GetStrengthenForAverPrice(const AuctionInfo& auctionInfo)
{
	UInt8 strengthen = auctionInfo.strengthen;

	if (auctionInfo.itemtype != ITEM_EQUIP) //非装备
	{
		return strengthen;
	}

	if (strengthen > AUCTION_SPECIAL_AVERAGE_PRICE_STRENGTHEN_LEVEL)
	{
		return strengthen;
	}

	if (auctionInfo.equipType == EQUIP_RED) //红字装备
	{
		//strengthen = (strengthen / AUCTION_ZENFU_AVERAGE_SPAN) * AUCTION_ZENFU_AVERAGE_SPAN;
		strengthen = strengthen > AUCTION_ZENFU_AVERAGE_STR_LEV ? strengthen : 0;
	}
	else
	{
		strengthen = 0;
	}
	return strengthen;
}

UInt8 AuctionMgr::GetStrengthenForAverPrice(ItemDataEntry& itemData, UInt32 strengthen, UInt8 enhanceType)
{
	UInt8 strengthen_ = strengthen;

	if (itemData.type != ITEM_EQUIP)
	{
		return strengthen_;
	}

	if (strengthen_ > AUCTION_SPECIAL_AVERAGE_PRICE_STRENGTHEN_LEVEL)
	{
		return strengthen_;
	}

	if (enhanceType > ENHANCE_INVALID) //红字装备
	{
		//strengthen_ = (strengthen_ / AUCTION_ZENFU_AVERAGE_SPAN) * AUCTION_ZENFU_AVERAGE_SPAN;
		strengthen_ = strengthen_ > AUCTION_ZENFU_AVERAGE_STR_LEV ? strengthen_ : 0;
	}
	else
	{
		strengthen_ = 0;
	}
	return strengthen_;

}

void AuctionMgr::GetStrengthenRangeForHzEquip(UInt8 strengthen, UInt8& min, UInt8& max)
{
	if (strengthen > AUCTION_ZENFU_AVERAGE_STR_LEV)
	{
		min = strengthen;
		max = strengthen;
	}
	else
	{
		min = 0;
		max = AUCTION_ZENFU_AVERAGE_STR_LEV;
	}
}

void AuctionMgr::UpdateAuctionPriceCacheForExplaceHzEquip(WSPlayer *player, AuctionType type, UInt32 itemTypeId, UInt32 strengthen, UInt32  beadbuffid, 
	UInt8 enhanceType, UInt8 goodState, UInt8 opType, UInt8 realEnhanceType)
{
	if (!player)
	{
		return;
	}

	ItemDataEntry* itemData = ItemDataEntryMgr::Instance()->FindEntry(itemTypeId);
	if (!itemData)
	{
		ErrorLogStream << "itemData is null itemTypeId : " << itemTypeId << LogStream::eos;
		return;
	}

	DebugLogStream << "player(" << player->GetID() << ") query type(" << (UInt8)type << ") item(" << itemTypeId << ") strengthen (" << strengthen << ") recent price." 
		<< ", beadbuufid : " << beadbuffid << ", enhanceType : " << enhanceType << LogStream::eos;


	CLProtocol::AuctionQueryCondition cond;
	cond.itemTypeID = itemTypeId;
	cond.goodState = goodState;
	CLSelectCommand* command = CLRecordClient::Instance()->PrepareSelectCommand("t_auction_new");
	command->PutColumn("itemid");
	GenSqlByCondition(player, command, cond);

	if (itemData->subType == ST_MAGICCARD)
	{
		command->PutCond("strengthen", strengthen, "=");
	}
	else
	{
		if (strengthen > AUCTION_SPECIAL_AVERAGE_PRICE_STRENGTHEN_LEVEL)
		{
			command->PutCond("strengthen", strengthen, "=");
		}
		else
		{
			if (!WSApplication::Instance()->GetAuctionSqlInModify())
			{
				if (enhanceType > ENHANCE_INVALID) //红字装备
				{
					UInt8 min = 0;
					UInt8 max = 0;
					GetStrengthenRangeForHzEquip(strengthen, min, max);
					command->PutCond("strengthen", min, ">=");
					command->PutCond("strengthen", max, "<=");
					command->PutCond("equiptype", EQUIP_RED, "=");
				}
				else //普通和带气息的装备
				{
					command->PutCond("strengthen", 0, ">=");
					command->PutCond("strengthen", 10, "<=");
					command->PutCond("equiptype", EQUIP_RED, "!=");
				}
			}
			else
			{
				if (enhanceType > ENHANCE_INVALID) //红字装备
				{
					UInt8 min = 0;
					UInt8 max = 0;
					GetStrengthenRangeForHzEquip(strengthen, min, max);
					std::stringstream stream;
					stream << "(";
					for (UInt32 i = min; i <= max; ++i)
					{
						stream << i;
						if (i != max)
						{
							stream << ",";
						}
					}
					stream << ")";

					command->PutCond("strengthen", stream.str(), "in");
					command->PutCond("equiptype", EQUIP_RED, "=");
				}
				else
				{
					command->PutCond("strengthen", "(0,1,2,3,4,5,6,7,8,9,10)", "in");
					command->PutCond("equiptype", EQUIP_RED, "!=");
				}
			}
		}
	}

	std::stringstream orderStream;
	orderStream << "order by price / num asc";
	command->SetOrder(orderStream.str());

	UInt32 limit = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_PRICESLIST_NUM);
	limit = (limit == 0) ? 10 : limit;
	std::ostringstream limitoss;
	limitoss << "limit " << limit;
	command->SetLimit(limitoss.str());

	CLRecordClient::Instance()->SendCommand(command, new SelectAuctionSaleListCallbackForExplaceHzEquip(player->GetID(), type, itemTypeId, strengthen, beadbuffid, enhanceType, goodState, opType, realEnhanceType));

	return;
}

void AuctionMgr::OnQueryAuctionSaleLisForExplaceHzEquipRet(ObjID_t roleId, std::vector<ObjID_t>& auctionIds, AuctionType type, UInt32 itemTypeId,
	UInt32 strengthen, UInt32 beadbuffid, UInt8 enhanceType, UInt8 goodState, UInt8 opType, UInt8	realEnhanceType)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(roleId);
	if (!player)
	{
		InfoLogStream << "query auction sale list ret, player(" << roleId << ") offline." << LogStream::eos;
		return;
	}

	ItemDataEntry * itemdata = ItemDataEntryMgr::Instance()->FindEntry(itemTypeId);
	if (!itemdata)
	{
		InfoLogStream << "query auction sale list ret itemdata null player(" << roleId << ") itemTypeId(" << itemTypeId << ")" << LogStream::eos;
		return;
	}

	//上架
	if (opType == APLOP_ON_SALE)
	{
		CLProtocol::WorldAuctionQueryItemPricesRes res;
		res.type = type;
		res.itemTypeId = itemTypeId;
		res.strengthen = strengthen;

		for (auto auctionId : auctionIds)
		{
			AuctionInfo* auctionInfo = _FindAuction(auctionId);
			if (!auctionInfo)
			{
				continue;
			}

			res.actionItems.push_back(*(AuctionBaseInfo*)auctionInfo);
		}
		ItemDataEntry * itemdata = ItemDataEntryMgr::Instance()->FindEntry(itemTypeId);
		if (!itemdata)
		{
			InfoLogStream << "query auction sale list ret itemdata null player(" << roleId << ") itemTypeId(" << itemTypeId << ")" << LogStream::eos;
			return;
		}
		res.averagePrice = 0;

		if (!_GetLimitPrice(itemTypeId, strengthen, beadbuffid, enhanceType, res.minPrice, res.maxPrice))
		{
			return;
		}

		if (itemdata->isTreasure && WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_AUCTION_TREAS))
		{
			res.visAverPrice = 0;
		}
		else
		{
			UInt8 strengthen_ = GetStrengthenForAverPrice(*itemdata, strengthen, enhanceType);
			AuctionRecommendPrice* recommend = GetRecommendPrice(itemTypeId, strengthen_, beadbuffid, enhanceType);
			if (recommend && recommend->UpdateVisPrice())
			{
				CLUpdateCommand *pCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_auction_recommend_price", recommend->guid, false);
				pCmd->PutData("visibility_price", recommend->visibility_price);
				pCmd->PutData("vis_price_update_time", recommend->vis_price_update_time);
				CLRecordClient::Instance()->SendCommand(pCmd);
			}
			_GetVisAverPrice(itemTypeId, strengthen, beadbuffid, enhanceType, res.visAverPrice); //非珍品宝珠buffid必定是0
		}

		UpdateAuctionPriceListCache(itemTypeId, strengthen, beadbuffid, enhanceType, res, *itemdata);
		

		CLProtocol::WorldAuctionQueryItemPricesRes real_res;
		real_res.type = type;
		real_res.itemTypeId = itemTypeId;
		real_res.strengthen = strengthen;
		real_res.actionItems = res.actionItems;
		if (!_GetLimitPrice(itemTypeId, strengthen, beadbuffid, realEnhanceType, real_res.minPrice, real_res.maxPrice))
		{
			return;
		}

		if (itemdata->isTreasure && WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_AUCTION_TREAS))
		{
			real_res.visAverPrice = 0;
		}
		else
		{
			UInt8 strengthen_ = GetStrengthenForAverPrice(*itemdata, strengthen, realEnhanceType);
			AuctionRecommendPrice* recommend = GetRecommendPrice(itemTypeId, strengthen_, beadbuffid, realEnhanceType);
			if (recommend && recommend->UpdateVisPrice())
			{
				CLUpdateCommand *pCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_auction_recommend_price", recommend->guid, false);
				pCmd->PutData("visibility_price", recommend->visibility_price);
				pCmd->PutData("vis_price_update_time", recommend->vis_price_update_time);
				CLRecordClient::Instance()->SendCommand(pCmd);
			}
			_GetVisAverPrice(itemTypeId, strengthen, beadbuffid, realEnhanceType, real_res.visAverPrice);
		}

		player->SendProtocol(real_res);
	}
	else if (opType == APLOP_SWITCH) //在售列表切换
	{
		CLProtocol::WorldAuctionQueryItemPriceListRes res;
		res.type = type;
		res.itemTypeId = itemTypeId;
		res.strengthen = strengthen;
		res.auctionState = goodState;

		for (auto auctionId : auctionIds)
		{
			AuctionInfo* auctionInfo = _FindAuction(auctionId);
			if (!auctionInfo)
			{
				continue;
			}

			res.actionItems.push_back(*(AuctionBaseInfo*)auctionInfo);
		}

		UpdateAuctionPriceListCacheForSwitch(itemTypeId, strengthen, 0, enhanceType, res, *itemdata, goodState);
		player->SendProtocol(res);
	}
	else
	{
		return;
	}
}

bool AuctionMgr::CheckAuctionOccus(const ItemDataEntry* itemData, std::vector<UInt8>& occus)
{
	if (!itemData)
	{
		return false;
	}
	if (occus.size() == 0)
	{
		return true;
	}

	for (auto occu : occus)
	{
		if (itemData->occu.test(occu))
		{
			return true;
		}
	}
	return false;
}

bool AuctionMgr::CheckTimeLimitAuctionOnsaleAgain(WSPlayer* player, const AuctionInfo& auctionInfo)
{
	if (!auctionInfo.item)
	{
		return false;
	}
	if (auctionInfo.item->m_uDeadLine == 0)
	{
		return true;
	}
	UInt32 dueTimeRemain = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_ONSALE_DUETIME_REMAIN);
	if (auctionInfo.item->m_uDeadLine <= (UInt32)CURRENT_TIME.Sec())
	{
		player->SendNotify(10039);
		return false;
	}

	if (auctionInfo.item->m_uDeadLine < ((UInt32)CURRENT_TIME.Sec() + dueTimeRemain * HOUR_TO_SEC))
	{
		player->SendNotify(10038);
		return false;
	}

	return true;
}

UInt32  AuctionMgr::GetItemTransedRecords(UInt32 itemTypeId, UInt32 strengthen, UInt32 beadBuffId, UInt8 enhanceType, std::vector<AuctionTransaction>& itemTrans)
{
	return AuctionTransRecordMgr::Instance()->GetItemTransedRecords(itemTypeId, strengthen, beadBuffId, enhanceType, itemTrans);
}

AuctionNumCache* AuctionMgr::GetAuctionNumCache(const CLProtocol::AuctionQueryCondition& cond)
{
	for (auto& cache : m_auctionNumCaches)
	{
		if (cache.cond == cond)
		{
			return &cache;
		}
	}
	return NULL;
}

void AuctionMgr::UpdateAuctionNumCache(const CLProtocol::AuctionQueryCondition& cond, CLProtocol::WorldAuctionItemNumRes& res)
{
	UInt32 time = WSApplication::Instance()->GetAuctionNumCacheTime();
	time = (time == 0) ? 5 : time;

	AuctionNumCache* cache = GetAuctionNumCache(cond);
	if (cache)
	{
		cache->res = res;
		cache->expireTime = (UInt32)CURRENT_TIME.Sec() + time;
	}
	else
	{
		AuctionNumCache tempCache;
		tempCache.cond = cond;
		tempCache.res = res;
		tempCache.expireTime = (UInt32)CURRENT_TIME.Sec() + time;
		m_auctionNumCaches.push_back(tempCache);
	}
}

void AuctionMgr::AddOneAuctionForMemQuery(AuctionInfo* auctionInfo)
{
	if (!auctionInfo)	return;

	if (auctionInfo->duetime < (UInt32)CURRENT_TIME.Sec()) return;

	UInt32 auctionMainType = GetAuctionMainType((ItemType)auctionInfo->itemtype, (ItemSubType)auctionInfo->subType);
	auto it = m_AuctionsMainType[auctionMainType].find(auctionInfo->guid);
	if (it == m_AuctionsMainType[auctionMainType].end())
	{
		m_AuctionsMainType[auctionMainType].insert(std::make_pair(auctionInfo->guid, auctionInfo));
	}

	auto it1 = m_AuctionsSubType[auctionInfo->auctionSubType].find(auctionInfo->guid);
	if (it1 == m_AuctionsSubType[auctionInfo->auctionSubType].end())
	{
		m_AuctionsSubType[auctionInfo->auctionSubType].insert(std::make_pair(auctionInfo->guid, auctionInfo));
	}

	m_auctionesItemId[auctionInfo->itemTypeId].insert(auctionInfo);
}

void AuctionMgr::RemoveAuctionForMemQuery(AuctionInfo* auctionInfo)
{
	if (!auctionInfo)	return;
	UInt32 auctionMainType = GetAuctionMainType((ItemType)auctionInfo->itemtype, (ItemSubType)auctionInfo->subType);
	auto iter_main_type = m_AuctionsMainType.find(auctionMainType);
	if (iter_main_type != m_AuctionsMainType.end())
	{
		iter_main_type->second.erase(auctionInfo->guid);
	}

	auto iter_sub_type = m_AuctionsSubType.find(auctionInfo->auctionSubType);
	if (iter_sub_type != m_AuctionsSubType.end())
	{
		iter_sub_type->second.erase(auctionInfo->guid);
	}

	auto iter_itemId = m_auctionesItemId.find(auctionInfo->itemTypeId);
	if (iter_itemId != m_auctionesItemId.end())
	{
		auto it = iter_itemId->second.begin();
		for (; it != iter_itemId->second.end(); it++)
		{
			if (auctionInfo->guid == (*it)->guid)
			{
				iter_itemId->second.erase(it);
				break;
			}
		}
	}

}

void AuctionMgr::QueryAuctionItemNumFromMem(WSPlayer* player, const CLProtocol::AuctionQueryCondition& cond)
{
	if (!player)	return;

	CLProtocol::WorldAuctionItemNumRes res;
	res.goodState = cond.goodState;
	UInt32 currTime = (UInt32)CURRENT_TIME.Sec();

	std::set<QueryAuctionItemNumSt> auctionItemNums;

	if (!cond.itemSubTypes.empty())
	{
		for (auto itemSubType : cond.itemSubTypes)
		{
			auto it = m_AuctionsSubType.find(itemSubType);
			if (it == m_AuctionsSubType.end() || it->second.size() == 0)
			{
				continue;
			}

			for (auto e : it->second)
			{
				AuctionInfo* auction = e.second;
				if (!auction)	continue;
				//检查到期
				if (auction->duetime < currTime)
				{
					continue;
				}
				//检查类型
				if (auction->type != cond.type)
				{
					continue;
				}

				//检查公示，上架
				if (cond.goodState == AGS_PUBLIC)
				{
					if (auction->onPublicTime == 0 ||
						auction->publicEndTime < currTime)
					{
						continue;
					}
				}
				else if (cond.goodState == AGS_ONSALE)
				{
					if (auction->onSaleTime > currTime)
					{
						continue;
					}
				}
				//检查quality
				if (cond.quality > 0)
				{
					if (auction->quality != cond.quality)
					{
						continue;
					}
				}
				//检查itemLevel
				if (cond.minLevel != cond.maxLevel)
				{
					if (auction->itemLevel < cond.minLevel || auction->itemLevel > cond.maxLevel)
					{
						continue;
					}
				}
				else if (cond.minLevel == cond.maxLevel && cond.minLevel != 0)
				{
					if (auction->itemLevel != cond.minLevel)
					{
						continue;
					}
				}
				//检查strength
				if (cond.minStrength != cond.maxStrength)
				{
					if (auction->strengthen < cond.minStrength || auction->strengthen > cond.maxStrength)
					{
						continue;
					}
				}
				else if (cond.minStrength == cond.maxStrength && cond.minStrength != 0)
				{
					if (cond.minStrength == 100 && auction->strengthen != 0)
					{
						continue;
					}
					else if (auction->strengthen != cond.minStrength)
					{
						continue;
					}
				}
				//检查couponStrengthToLev
				if (cond.couponStrengthToLev > 0)
				{
					if (auction->couponStrengthenTo != cond.couponStrengthToLev)
					{
						continue;
					}
				}

				QueryAuctionItemNumSt auctionItemSt;
				auctionItemSt.itemTypeId = auction->itemTypeId;
				auctionItemSt.quality = auction->quality;
				auctionItemSt.itemLevel = auction->itemLevel;

				std::set<QueryAuctionItemNumSt>::iterator it = auctionItemNums.find(auctionItemSt);
				if (it == auctionItemNums.end())
				{
					auctionItemSt.isTreas = auction->isTreas;
					*auctionItemSt.num = auction->num;
					auctionItemNums.insert(auctionItemSt);
				}
				else
				{
					*(it->num) += auction->num;
				}
			}
		}
	}
	else if (cond.itemMainType > 0)
	{
		auto it = m_AuctionsMainType.find(cond.itemMainType);
		if (it != m_AuctionsMainType.end())
		{
			MapAuction& auctions = it->second;
			for (auto e : auctions)
			{
				AuctionInfo* auction = e.second;
				if (!auction)	continue;
				//检查到期
				if (auction->duetime < currTime)
				{
					continue;
				}
				//检查类型
				if (auction->type != cond.type)
				{
					continue;
				}

				//检查公示，上架
				if (cond.goodState == AGS_PUBLIC)
				{
					if (auction->onPublicTime == 0 ||
						auction->publicEndTime < currTime)
					{
						continue;
					}
				}
				else if (cond.goodState == AGS_ONSALE)
				{
					if (auction->onSaleTime > currTime)
					{
						continue;
					}
				}
				//检查排除subtype
				if (cond.excludeItemSubTypes.size() > 0)
				{
					if (FindInVector(auction->auctionSubType, cond.excludeItemSubTypes))
					{
						continue;
					}
				}
				//检查quality
				if (cond.quality > 0)
				{
					if (auction->quality != cond.quality)
					{
						continue;
					}
				}
				//检查itemLevel
				if (cond.minLevel != cond.maxLevel)
				{
					if (auction->itemLevel < cond.minLevel || auction->itemLevel > cond.maxLevel)
					{
						continue;
					}
				}
				else if (cond.minLevel == cond.maxLevel && cond.minLevel != 0)
				{
					if (auction->itemLevel != cond.minLevel)
					{
						continue;
					}
				}

				//检查strength
				if (cond.minStrength != cond.maxStrength)
				{
					if (auction->strengthen < cond.minStrength || auction->strengthen > cond.maxStrength)
					{
						continue;
					}
				}
				else if (cond.minStrength == cond.maxStrength && cond.minStrength != 0)
				{
					if (cond.minStrength == 100 && auction->strengthen != 0)
					{
						continue;
					}
					else if (auction->strengthen != cond.minStrength)
					{
						continue;
					}
				}
				//检查couponStrengthToLev
				if (cond.couponStrengthToLev > 0)
				{
					if (auction->couponStrengthenTo != cond.couponStrengthToLev)
					{
						continue;
					}
				}
				QueryAuctionItemNumSt auctionItemSt;
				auctionItemSt.itemTypeId = auction->itemTypeId;
				auctionItemSt.quality = auction->quality;
				auctionItemSt.itemLevel = auction->itemLevel;

				std::set<QueryAuctionItemNumSt>::iterator it = auctionItemNums.find(auctionItemSt);
				if (it == auctionItemNums.end())
				{
					auctionItemSt.isTreas = auction->isTreas;
					*auctionItemSt.num = auction->num;
					auctionItemNums.insert(auctionItemSt);
				}
				else
				{
					*(it->num) += auction->num;
				}

			}
		}
	}
	else
	{
	}

	for (auto it : auctionItemNums)
	{
		AuctionItemBaseInfo info;
		info.itemTypeId = it.itemTypeId;
		info.isTreas = it.isTreas;
		info.num = *(it.num);
		res.items.push_back(info);
	}

	player->SendProtocol(res);

	AuctionMgr::Instance()->UpdateAuctionNumCache(cond, res);
}

void AuctionMgr::QueryAuctionItemListFromMem(WSPlayer* player, const CLProtocol::AuctionQueryCondition& cond)
{
	if (!player)	return;
	if (cond.itemNumPerPage == 0)
	{
		return;
	}

	if (cond.itemTypeID == 0)
	{
		return;
	}

	auto it = m_auctionesItemId.find(cond.itemTypeID);
	if (it == m_auctionesItemId.end() || it->second.size() == 0)
	{
		return;
	}

	UInt32 currTime = (UInt32)CURRENT_TIME.Sec();
	CLProtocol::WorldAuctionListQueryRet ret;
	ret.type = cond.type;
	ret.curPage = cond.page;
	ret.maxPage = 0;
	ret.goodState = cond.goodState;
	ret.attent = cond.attent;

	std::vector<AuctionInfo*> auctionInfos;
	for (auto auction : it->second)
	{
		if (!auction)	continue;
		if (cond.goodState == AGS_PUBLIC)
		{
			if (auction->onPublicTime == 0 ||
				auction->publicEndTime < currTime)
			{
				continue;
			}
		}
		else if (cond.goodState == AGS_ONSALE)
		{
			if (auction->onSaleTime > currTime)
			{
				continue;
			}
		}
		auctionInfos.push_back(auction);
	}

	UInt32 count = auctionInfos.size();
	UInt16 maxPage = count / cond.itemNumPerPage + (count % cond.itemNumPerPage > 0 ? 1 : 0);
	maxPage = (maxPage == 0) ? 1 : maxPage;
	ret.maxPage = maxPage;

	std::vector<ObjID_t> guids;

	Int32 iStart = cond.page <= 1 ? 0 : ((cond.page - 1) * cond.itemNumPerPage);
	Int32 iEnd = iStart + cond.itemNumPerPage - 1;
	iEnd = (iEnd > auctionInfos.size() - 1) ? auctionInfos.size() - 1 : iEnd;

	if ((AuctionSortType)cond.sortType == AST_PRICE_ASC)
	{
		for (Int32 i = iStart; i <= iEnd; ++i)
		{
			ret.data.push_back(*(AuctionBaseInfo*)auctionInfos[i]);
			AuctionAttentMgr::Instance()->GetAuctionAttentInfo(*auctionInfos[i], player->GetID());
			guids.push_back(auctionInfos[i]->guid);
		}
	}
	else if ((AuctionSortType)cond.sortType == AST_PRICE_DESC)
	{
		Int32 iEnd_ = auctionInfos.size() - 1 - iStart;
		Int32 iStart_ = auctionInfos.size() - 1 - iEnd;
		if (iStart_ == iEnd_)
		{
			ret.data.push_back(*(AuctionBaseInfo*)auctionInfos[iStart_]);
			AuctionAttentMgr::Instance()->GetAuctionAttentInfo(*auctionInfos[iStart_], player->GetID());
			guids.push_back(auctionInfos[iStart_]->guid);
		}
		else
		{
			for (Int32 i = iEnd_; i >= iStart_; --i)
			{
				ret.data.push_back(*(AuctionBaseInfo*)auctionInfos[i]);
				AuctionAttentMgr::Instance()->GetAuctionAttentInfo(*auctionInfos[i], player->GetID());
				guids.push_back(auctionInfos[i]->guid);
			}
		}
	}

	if (cond.attent == ATT_ATTENT && ret.data.size() > 1)
	{
		if (cond.goodState == AGS_PUBLIC)
		{
			std::sort(ret.data.begin(), ret.data.end(), [](AuctionBaseInfo a, AuctionBaseInfo b) {
				return a.attentTime > b.attentTime;
			}
			);
		}
		else if (cond.goodState == AGS_ONSALE)
		{
			std::sort(ret.data.begin(), ret.data.end(), [](AuctionBaseInfo a, AuctionBaseInfo b) {
				return a.onSaleTime > b.onSaleTime;
			}
			);
		}
	}

	player->SetCanBuyAuctionGuids(guids);
	player->SendProtocol(ret);
}

void AuctionMgr::QueryItemRecentPricesFormMem(WSPlayer* player, AuctionType type, UInt32 itemTypeId, UInt32 strengthen, UInt32  beadbuffid, UInt8 enhanceType)
{
	if (!player)	return;

	ItemDataEntry* itemData = ItemDataEntryMgr::Instance()->FindEntry(itemTypeId);
	if (!itemData)
	{
		ErrorLogStream << "itemData is null itemTypeId : " << itemTypeId << LogStream::eos;
		return;
	}

	auto it = m_auctionesItemId.find(itemTypeId);
	if (it == m_auctionesItemId.end() || it->second.size() == 0)
	{
		//return;
	}
	UInt32 currTime = (UInt32)CURRENT_TIME.Sec();
	CLProtocol::WorldAuctionQueryItemPricesRes res;
	res.type = type;
	res.itemTypeId = itemTypeId;
	res.strengthen = strengthen;
	res.recommendPrice = 0;
	res.averagePrice = 0;

	UInt32 limit = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_PRICESLIST_NUM);
	limit = (limit == 0) ? 10 : limit;
	std::vector<AuctionInfo*> auctionInfos;
	if (it != m_auctionesItemId.end())
	{
		for (auto auction : it->second)
		{
			if (!auction)	continue;

			if (auction->onSaleTime > currTime)
			{
				continue;
			}

			if (itemData->subType == ST_MAGICCARD)
			{
				if (auction->strengthen != strengthen)
				{
					continue;
				}
			}
			else
			{
				if (strengthen > AUCTION_SPECIAL_AVERAGE_PRICE_STRENGTHEN_LEVEL)
				{
					if (auction->strengthen != strengthen)
					{
						continue;
					}
				}
				else
				{
					if (enhanceType > ENHANCE_INVALID) //红字装备
					{
						if (auction->equipType != EQUIP_RED)
						{
							continue;
						}
						UInt8 min = 0;
						UInt8 max = 0;
						GetStrengthenRangeForHzEquip(strengthen, min, max);
						if (auction->strengthen < min || auction->strengthen > max)
						{
							continue;
						}
					}
					else //普通和带气息的装备
					{
						if (auction->equipType == EQUIP_RED)
						{
							continue;
						}
						if (auction->strengthen > 10)
						{
							continue;
						}
					}
				}
			}
			res.actionItems.push_back(*(AuctionBaseInfo*)auction);
			if (res.actionItems.size() >= limit)
			{
				break;
			}
		}
	}

	if (!_GetLimitPrice(itemTypeId, strengthen, beadbuffid, enhanceType, res.minPrice, res.maxPrice))
	{
		return;
	}
	if (itemData->isTreasure && WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_AUCTION_TREAS))
	{
		res.visAverPrice = 0;
		//res.averagePrice = _GetTreasAverPrice(itemTypeId, strengthen, beadbuffid);
	}
	else
	{
		UInt8 strengthen_ = GetStrengthenForAverPrice(*itemData, strengthen, enhanceType);
		AuctionRecommendPrice* recommend = GetRecommendPrice(itemTypeId, strengthen_, beadbuffid, enhanceType);
		if (recommend && recommend->UpdateVisPrice())
		{
			CLUpdateCommand *pCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_auction_recommend_price", recommend->guid, false);
			pCmd->PutData("visibility_price", recommend->visibility_price);
			pCmd->PutData("vis_price_update_time", recommend->vis_price_update_time);
			CLRecordClient::Instance()->SendCommand(pCmd);
		}
		_GetVisAverPrice(itemTypeId, strengthen, beadbuffid, enhanceType, res.visAverPrice); //非珍品宝珠buffid必定是0
		res.recommendPrice = _GetRecommendPrice(itemTypeId, strengthen, enhanceType);
	}

	UpdateAuctionPriceListCache(itemTypeId, strengthen, beadbuffid, enhanceType, res, *itemData);
	if (enhanceType > ENHANCE_INVALID && res.actionItems.size() == 0)
	{
		AuctionPricesSt* priceCanch_ = GetAuctionPriceListCache(itemTypeId, strengthen, beadbuffid, ENHANCE_INVALID, *itemData);
		if (priceCanch_ && priceCanch_->res.actionItems.size() > 0)
		{
			res.actionItems = priceCanch_->res.actionItems;
		}
		else if (priceCanch_ == NULL)
		{
			UpdateAuctionPriceCacheForExplaceHzEquipFromMem(player, type, itemTypeId, strengthen, beadbuffid, ENHANCE_INVALID, AGS_ONSALE, APLOP_ON_SALE, enhanceType);
			return;
		}
	}
	player->SendProtocol(res);
	return;
}

void AuctionMgr::UpdateAuctionPriceCacheForExplaceHzEquipFromMem(WSPlayer *player, AuctionType type, UInt32 itemTypeId, UInt32 strengthen, UInt32  beadbuffid, UInt8 enhanceType, UInt8 goodState, UInt8 opType, UInt8 realEnhanceType)
{
	if (!player) return;
	ItemDataEntry* itemdata = ItemDataEntryMgr::Instance()->FindEntry(itemTypeId);
	if (!itemdata)
	{
		InfoLogStream << "query auction sale list ret itemdata null player(" << player->GetID() << ") itemTypeId(" << itemTypeId << ")" << LogStream::eos;
		return;
	}

	DebugLogStream << "player(" << player->GetID() << ") query type(" << (UInt8)type << ") item(" << itemTypeId << ") strengthen (" << strengthen << ") recent price."
		<< ", beadbuufid : " << beadbuffid << ", enhanceType : " << enhanceType << LogStream::eos;

	auto it = m_auctionesItemId.find(itemTypeId);
	if (it == m_auctionesItemId.end() || it->second.size() == 0)
	{
		//return;
	}

	UInt32 currTime = (UInt32)CURRENT_TIME.Sec();

	std::vector<AuctionBaseInfo>    actionItems;
	UInt32 limit = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_PRICESLIST_NUM);
	limit = (limit == 0) ? 10 : limit;

	if (it != m_auctionesItemId.end())
	{
		for (auto auction : it->second)
		{
			if (!auction)	continue;

			if (goodState == AGS_PUBLIC)
			{
				if (auction->onPublicTime == 0 ||
					auction->publicEndTime < currTime)
				{
					continue;
				}
			}
			else if (goodState == AGS_ONSALE)
			{
				if (auction->onSaleTime > currTime)
				{
					continue;
				}
			}

			if (itemdata->subType == ST_MAGICCARD)
			{
				if (auction->strengthen != strengthen)
				{
					continue;
				}
			}
			else
			{
				if (strengthen > AUCTION_SPECIAL_AVERAGE_PRICE_STRENGTHEN_LEVEL)
				{
					if (auction->strengthen != strengthen)
					{
						continue;
					}
				}
				else
				{
					if (enhanceType > ENHANCE_INVALID) //红字装备
					{
						if (auction->equipType != EQUIP_RED)
						{
							continue;
						}
						UInt8 min = 0;
						UInt8 max = 0;
						GetStrengthenRangeForHzEquip(strengthen, min, max);
						if (auction->strengthen < min || auction->strengthen > max)
						{
							continue;
						}
					}
					else //普通和带气息的装备
					{
						if (auction->equipType == EQUIP_RED)
						{
							continue;
						}
						if (auction->strengthen > 10)
						{
							continue;
						}
					}
				}
			}

			actionItems.push_back(*(AuctionBaseInfo*)auction);
			if (actionItems.size() >= limit)
			{
				break;
			}
		}
	}

	if (opType == APLOP_ON_SALE)	//上架
	{
		CLProtocol::WorldAuctionQueryItemPricesRes res;
		res.type = type;
		res.itemTypeId = itemTypeId;
		res.strengthen = strengthen;
		res.recommendPrice = 0;
		res.averagePrice = 0;

		res.actionItems = actionItems;

		if (!_GetLimitPrice(itemTypeId, strengthen, beadbuffid, enhanceType, res.minPrice, res.maxPrice))
		{
			return;
		}
		if (itemdata->isTreasure && WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_AUCTION_TREAS))
		{
			res.visAverPrice = 0;
		}
		else
		{
			UInt8 strengthen_ = GetStrengthenForAverPrice(*itemdata, strengthen, enhanceType);
			AuctionRecommendPrice* recommend = GetRecommendPrice(itemTypeId, strengthen_, beadbuffid, enhanceType);
			if (recommend && recommend->UpdateVisPrice())
			{
				CLUpdateCommand *pCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_auction_recommend_price", recommend->guid, false);
				pCmd->PutData("visibility_price", recommend->visibility_price);
				pCmd->PutData("vis_price_update_time", recommend->vis_price_update_time);
				CLRecordClient::Instance()->SendCommand(pCmd);
			}
			_GetVisAverPrice(itemTypeId, strengthen, beadbuffid, enhanceType, res.visAverPrice); //非珍品宝珠buffid必定是0

			UpdateAuctionPriceListCache(itemTypeId, strengthen, beadbuffid, enhanceType, res, *itemdata);

			CLProtocol::WorldAuctionQueryItemPricesRes real_res;
			real_res.type = type;
			real_res.itemTypeId = itemTypeId;
			real_res.strengthen = strengthen;
			real_res.actionItems = res.actionItems;
			if (!_GetLimitPrice(itemTypeId, strengthen, beadbuffid, realEnhanceType, real_res.minPrice, real_res.maxPrice))
			{
				return;
			}

			if (itemdata->isTreasure && WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_AUCTION_TREAS))
			{
				real_res.visAverPrice = 0;
			}
			else
			{
				UInt8 strengthen_ = GetStrengthenForAverPrice(*itemdata, strengthen, realEnhanceType);
				AuctionRecommendPrice* recommend = GetRecommendPrice(itemTypeId, strengthen_, beadbuffid, realEnhanceType);
				if (recommend && recommend->UpdateVisPrice())
				{
					CLUpdateCommand *pCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_auction_recommend_price", recommend->guid, false);
					pCmd->PutData("visibility_price", recommend->visibility_price);
					pCmd->PutData("vis_price_update_time", recommend->vis_price_update_time);
					CLRecordClient::Instance()->SendCommand(pCmd);
				}
				_GetVisAverPrice(itemTypeId, strengthen, beadbuffid, realEnhanceType, real_res.visAverPrice);
			}

			player->SendProtocol(real_res);
		}

	}
	else if (opType == APLOP_SWITCH)	//在售列表切换
	{
		CLProtocol::WorldAuctionQueryItemPriceListRes res;
		res.type = type;
		res.itemTypeId = itemTypeId;
		res.strengthen = strengthen;
		res.auctionState = goodState;

		res.actionItems = actionItems;

		UpdateAuctionPriceListCacheForSwitch(itemTypeId, strengthen, 0, enhanceType, res, *itemdata, goodState);
		player->SendProtocol(res);
	}
	else
	{
		return;
	}
}

void AuctionMgr::QueryItemRecentPricesFormMemForSwitch(WSPlayer *player, AuctionType type, UInt8 auctionState, UInt32 itemTypeId, UInt32 strengthen, UInt8 enhanceType)
{
	if (!player)	return;
	if (auctionState >= AGS_NUM || auctionState <= AGS_INVALID)	return;

	ItemDataEntry * itemdata = ItemDataEntryMgr::Instance()->FindEntry(itemTypeId);
	if (!itemdata)
	{
		InfoLogStream << "query auction price list ret itemdata null player(" << player->GetID() << ") itemTypeId(" << itemTypeId << ")" << LogStream::eos;
		return;
	}

	auto it = m_auctionesItemId.find(itemTypeId);
	if (it == m_auctionesItemId.end() || it->second.size() == 0)
	{
		//return;
	}

	UInt32 currTime = (UInt32)CURRENT_TIME.Sec();
	CLProtocol::WorldAuctionQueryItemPriceListRes res;
	res.type = type;
	res.itemTypeId = itemTypeId;
	res.strengthen = strengthen;
	res.auctionState = auctionState;

	std::vector<AuctionBaseInfo>    actionItems;
	UInt32 limit = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_PRICESLIST_NUM);
	limit = (limit == 0) ? 10 : limit;

	if (it != m_auctionesItemId.end())
	{
		for (auto auction : it->second)
		{
			if (!auction)	continue;

			if (auctionState == AGS_PUBLIC)
			{
				if (auction->onPublicTime == 0 ||
					auction->publicEndTime < currTime)
				{
					continue;
				}
			}
			else if (auctionState == AGS_ONSALE)
			{
				if (auction->onSaleTime > currTime)
				{
					continue;
				}
			}

			if (itemdata->subType == ST_MAGICCARD)
			{
				if (auction->strengthen != strengthen)
				{
					continue;
				}
			}
			else
			{
				if (strengthen > AUCTION_SPECIAL_AVERAGE_PRICE_STRENGTHEN_LEVEL)
				{
					if (auction->strengthen != strengthen)
					{
						continue;
					}
				}
				else
				{
					if (enhanceType > ENHANCE_INVALID) //红字装备
					{
						if (auction->equipType != EQUIP_RED)
						{
							continue;
						}
						UInt8 min = 0;
						UInt8 max = 0;
						GetStrengthenRangeForHzEquip(strengthen, min, max);
						if (auction->strengthen < min || auction->strengthen > max)
						{
							continue;
						}
					}
					else //普通和带气息的装备
					{
						if (auction->equipType == EQUIP_RED)
						{
							continue;
						}
						if (auction->strengthen > 10)
						{
							continue;
						}
					}
				}
			}

			res.actionItems.push_back(*(AuctionBaseInfo*)auction);
			if (res.actionItems.size() >= limit)
			{
				break;
			}
		}
	}

	UpdateAuctionPriceListCacheForSwitch(itemTypeId, strengthen, 0, enhanceType, res, *itemdata, auctionState);
	
	if (enhanceType > ENHANCE_INVALID && res.actionItems.size() == 0)
	{
		AuctionPricesStForSwtich* priceCanch_ = GetAuctionPriceListCacheForSwitch(itemTypeId, strengthen, 0, ENHANCE_INVALID, *itemdata, auctionState);
		if (priceCanch_ && priceCanch_->res.actionItems.size() > 0)
		{
			res.actionItems = priceCanch_->res.actionItems;
		}
		else if (priceCanch_ == NULL)
		{
			UpdateAuctionPriceCacheForExplaceHzEquipFromMem(player, type, itemTypeId, strengthen, 0, ENHANCE_INVALID, auctionState, APLOP_SWITCH, enhanceType);
			return;
		}
	}

	player->SendProtocol(res);
	return;
}

void AuctionRecommendPrice::UpdatePrice(bool isTreas)
{
	UInt32 minTimes = AuctionSystemValueDataEntryMgr::Instance()->GetSystemValue(ASVT_CALC_AVERPRICE_MINTIMES);
	if (minTimes == 0) minTimes = 10;
	UInt32 treas_calc_max_times = AuctionSystemValueDataEntryMgr::Instance()->GetSystemValue(ASVT_TREAN_CALC_AVERPRICE_MAXTIMES);
	if (treas_calc_max_times == 0) treas_calc_max_times = 20;
	UInt32 not_treas_calc_max_times = AuctionSystemValueDataEntryMgr::Instance()->GetSystemValue(ASVT_NOTTREAN_CALC_AVERPRICE_MAXTIMES);
	if (not_treas_calc_max_times == 0) not_treas_calc_max_times = 100;
	if (isTreas) //珍品
	{
		if (recentPrice.size() < minTimes) //交易次数不足10次，没有平均价格
		{
			this->price = 0;
		}
		else
		{ 
			//珍品取最近20笔交易计算平均交易价格
			bool is_equip = false;
			ItemDataEntry* entry = ItemDataEntryMgr::Instance()->FindEntry(itemTypeID);
			if (entry && entry->type == ITEM_EQUIP) is_equip = true;
			UInt32 bead_averprice_rate = AuctionSystemValueDataEntryMgr::Instance()->GetSystemValue(ASVT_BEAD_AVERPRICE_RATE); //宝珠的平均交易价格系数（百分比）
			if (bead_averprice_rate == 0) bead_averprice_rate = 50;
			UInt32 magiccard_averprice_rate = AuctionSystemValueDataEntryMgr::Instance()->GetSystemValue(ASVT_MAGICCARD_AVERPRICE_RATE); //附魔卡平均交易价格系数（百分比）
			if (magiccard_averprice_rate == 0) magiccard_averprice_rate = 25;
			UInt32 guarantee_rate = AuctionSystemValueDataEntryMgr::Instance()->GetSystemValue(ASVT_MINIMUM_GUARANTEE_RATIO); //品级100保底价值系数（百分比）
			if (guarantee_rate == 0) guarantee_rate = 10;
			UInt32 grade_uplimint = AuctionSystemValueDataEntryMgr::Instance()->GetSystemValue(ASVT_GRADE_TOPLIMIT); //品级100上限价值
			if (grade_uplimint == 0) grade_uplimint = 450000;
			UInt64 totalM = 0;
			UInt32 num = 0;
			UInt32 sum = 0;
			for (Int32 index = recentPrice.size()-1; index >= 0; index--)
			{
				if (recentPrice[index].num == 0)
				{
					continue;
				}
			
				AuctionItemRecentPrice& itemRecentPrice = recentPrice[index];
				++num;
				sum += itemRecentPrice.num;
				if (is_equip)
				{
					UInt32 m = CalcTransEqBareMPrice(itemRecentPrice.price, itemRecentPrice.qualitylv, guarantee_rate, grade_uplimint);
					totalM += m;
				}
				else
				{
					totalM += recentPrice[index].price;
				}
				if (num >= treas_calc_max_times) //达到最大次数
				{
					break;
				}
			}
			if (sum != 0)
			{
				this->price = totalM / sum;
			}
		}
	}
	else //非珍品
	{
		if (recentPrice.size() < minTimes) //非珍品不足10次，取推荐价格
		{
			//this->price = zjy_auction::AuctionMgr::Instance()->_GetRecommendPrice(itemTypeID, strengthen);
			this->price = 0;
		}
		else{	//10次以上,非珍品取最近100笔交易计算平均交易价格，不足100笔则有多少取多少
			UInt64 total = 0;
			UInt32 num = 0;
			UInt32 sum = 0;
			//std::deque<AuctionItemRecentPrice>::iterator recent = recentPrice.end() - 1;
			for (Int32 index = recentPrice.size() - 1; index >= 0; index--)
			{
				total += recentPrice[index].price;
				sum += recentPrice[index].num;
				++num;
				if (num >= not_treas_calc_max_times) //达到最大次数
				{
					break;
				}
			}
			if (sum != 0)
			{
				UInt32 oldPrice = this->price;
				this->price = (UInt32)(total / sum);
				if (oldPrice == 0)
				{
					this->ForceUpdateVisPrice(0);
				}
			}
		}
	}
	this->price_update_time = (UInt32)CURRENT_TIME.Sec();
}

UInt32 AuctionRecommendPrice::CalcTransEqBareMPrice(UInt32 price, UInt32 qualitylv, UInt32 guarantee_rate, UInt32 grade_uplimint)
{
	InfoLogStream << "price : " << price << ", qualitylv : " << qualitylv << ", guarantee_rate : " << guarantee_rate << ", grade_uplimint : " << grade_uplimint << LogStream::eos;
	UInt32 m = 0;				  //珍品装备交易价格M
	UInt32 quality_value_100 = 0; //装备品级100价值

	if (qualitylv == 100)
	{
		quality_value_100 = price * guarantee_rate / 100;
		quality_value_100 = quality_value_100 > grade_uplimint ? grade_uplimint : quality_value_100;
	}

	m = price - quality_value_100;
	InfoLogStream << "m :" << m << LogStream::eos;
	return m;
}

UInt32 AuctionRecommendPrice::CalcTransEqMountedMPrice(AuctionInfo& auctionInfo, UInt32 beadAverPrice, UInt32 magicCardAverPrice,
	UInt32 bead_averprice_rate, UInt32 magiccard_averprice_rate, UInt32 guarantee_rate, UInt32 grade_uplimint, UInt32 cmp_bead_rate, UInt32 cmp_magic_rate)
{
	UInt32 price = auctionInfo.price;
	UInt32 qualitylv = auctionInfo.item->m_QualityLv;
	UInt32 eq_aver_price = AuctionMgr::Instance()->_GetAverPrice(auctionInfo.itemTypeId, auctionInfo.strengthen, 0, auctionInfo.enhanceType);
	if (eq_aver_price == 0)
	{
		eq_aver_price = AuctionMgr::Instance()->_GetRecommendPrice(auctionInfo.itemTypeId, auctionInfo.strengthen, auctionInfo.enhanceType);
	}

	eq_aver_price = (eq_aver_price < price) ? eq_aver_price : price;

	InfoLogStream << "item_dataid : " << auctionInfo.itemTypeId << "price : " << price << ", qualitylv : " << qualitylv << ", beadAverPrice : " << beadAverPrice << ", magicCardAverPrice : " << magicCardAverPrice
		<< ", bead_averprice_rate : " << bead_averprice_rate << ", magiccard_averprice_rate : " << magiccard_averprice_rate << ", guarantee_rate : " 
		<< guarantee_rate << ", grade_uplimint : " << grade_uplimint << ", cmp_bead_rate : " << cmp_bead_rate << ", cmp_magic_rate : " << cmp_magic_rate << "eq_aver_price : " << eq_aver_price << LogStream::eos;

	UInt32 min_bead_price = ((beadAverPrice * bead_averprice_rate / 100) < (eq_aver_price * cmp_bead_rate / 100)) ? (beadAverPrice * bead_averprice_rate / 100) : (eq_aver_price * cmp_bead_rate / 100);
	UInt32 min_magic_price = ((magicCardAverPrice * magiccard_averprice_rate / 100) < (eq_aver_price * cmp_magic_rate / 100)) ? (magicCardAverPrice * magiccard_averprice_rate / 100) : (eq_aver_price * cmp_magic_rate / 100);

	if (price <= (min_bead_price + min_magic_price) + 1)
	{
		InfoLogStream << "m : " << 1 << LogStream::eos;
		return 1;
	}

	UInt32 m = 0;				  //珍品装备交易价格M
	UInt32 bare_m = 0;			  //裸装价格 
	UInt32 quality_value_100 = 0; //装备品级100价值

	bare_m = price - min_bead_price - min_magic_price;
	if (qualitylv == 100)
	{
		quality_value_100 = bare_m * guarantee_rate / 100;
		quality_value_100 = quality_value_100 > grade_uplimint ? grade_uplimint : quality_value_100;
	}

	m = bare_m - quality_value_100;
	InfoLogStream << "m : " << m << LogStream::eos;
	return m;
}

void AuctionRecommendPrice::GetCalcTransMPriceRate(UInt32& bead_averprice_rate, UInt32& magiccard_averprice_rate, UInt32& guarantee_rate, UInt32& grade_uplimint)
{
	bead_averprice_rate = AuctionSystemValueDataEntryMgr::Instance()->GetSystemValue(ASVT_BEAD_AVERPRICE_RATE); //宝珠的平均交易价格系数（百分比）
	if (bead_averprice_rate == 0) bead_averprice_rate = 50;
	magiccard_averprice_rate = AuctionSystemValueDataEntryMgr::Instance()->GetSystemValue(ASVT_MAGICCARD_AVERPRICE_RATE); //附魔卡平均交易价格系数（百分比）
	if (magiccard_averprice_rate == 0) magiccard_averprice_rate = 25;
	guarantee_rate = AuctionSystemValueDataEntryMgr::Instance()->GetSystemValue(ASVT_MINIMUM_GUARANTEE_RATIO); //品级100保底价值系数（百分比）
	if (guarantee_rate == 0) guarantee_rate = 10;
	grade_uplimint = AuctionSystemValueDataEntryMgr::Instance()->GetSystemValue(ASVT_GRADE_TOPLIMIT); //品级100上限价值
	if (grade_uplimint == 0) grade_uplimint = 450000;
}

void AuctionRecommendPrice::GetCalcTransMPriceRateNew(AuctionInfo& auctionInfo, UInt32& bead_averprice_rate, UInt32& magiccard_averprice_rate, UInt32& guarantee_rate,
	UInt32& grade_uplimint, UInt32& cmp_bead_rate, UInt32& cmp_magic_rate)
{
	bead_averprice_rate = AuctionSystemValueDataEntryMgr::Instance()->GetSystemValue(ASVT_BEAD_AVERPRICE_RATE); //宝珠的平均交易价格系数（百分比）
	if (bead_averprice_rate == 0) bead_averprice_rate = 30;

	//附魔卡
	WSItem* item = auctionInfo.item;
	if (!item)
	{
		magiccard_averprice_rate = 0;
	}
	else
	{
		UInt32 magicCardId = item->m_AddMagic.GetMagicCard();
		if (magicCardId > 0)
		{
			ItemDataEntry* magicItemEntry = ItemDataEntryMgr::Instance()->FindEntry(magicCardId);
			if (!magicItemEntry || magicItemEntry->color < ItemQuality::ITEM_QUALITY_PINK)
			{
				magiccard_averprice_rate = 0;
			}
			else
			{
				magiccard_averprice_rate = AuctionSystemValueDataEntryMgr::Instance()->GetSystemValue(ASVT_MAGICCARD_AVERPRICE_RATE); //附魔卡平均交易价格系数（百分比）
				if (magiccard_averprice_rate == 0) magiccard_averprice_rate = 15;
			}
		}
		else //没附魔卡
		{
			magiccard_averprice_rate = 0;
		}
	}
	
	guarantee_rate = AuctionSystemValueDataEntryMgr::Instance()->GetSystemValue(ASVT_MINIMUM_GUARANTEE_RATIO); //品级100保底价值系数（百分比）
	if (guarantee_rate == 0) guarantee_rate = 10;
	grade_uplimint = AuctionSystemValueDataEntryMgr::Instance()->GetSystemValue(ASVT_GRADE_TOPLIMIT); //品级100上限价值
	if (grade_uplimint == 0) grade_uplimint = 450000;

	cmp_bead_rate = AuctionSystemValueDataEntryMgr::Instance()->GetSystemValue(ASVT_AUCTION_CMPBEDS_RATIO); //和宝珠平均价格比较系数
	if (cmp_bead_rate == 0) cmp_bead_rate = 50;

	cmp_magic_rate = AuctionSystemValueDataEntryMgr::Instance()->GetSystemValue(ASVT_AUCTION_CMPMAGIC_RATIO); //和附魔卡平均价格比较系数
	if (cmp_magic_rate == 0) cmp_magic_rate = 10;
}

void AuctionRecommendPrice::ForceUpdateVisPrice(UInt32 price)
{
	Avalon::Date curr_date = CURRENT_DATE;
	curr_date.Hour(0);
	curr_date.Min(0);
	curr_date.Sec(0);
	curr_date.MSec(0);
	this->visibility_price = price;
	this->vis_price_update_time = (UInt32)curr_date.ToTime().Sec();
	CLUpdateCommand *pCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_auction_recommend_price", this->guid, false);
	pCmd->PutData("visibility_price", this->visibility_price);
	pCmd->PutData("vis_price_update_time", this->vis_price_update_time);
	CLRecordClient::Instance()->SendCommand(pCmd);
}

};