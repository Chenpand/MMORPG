#include "SceneAuctionHandler.h"
#include "PlayerMgr.h"
#include "SSRouter.h"
#include <CLAuctionDefine.h>
#include <CLSystemValueDataEntry.h>

#include "AbnormalTransactionMgr.h"
#include "SSServiceSwitchMgr.h"

// 是否是一个合法的拍卖价格
bool IsValidAuctionPrice(UInt32 typeId, UInt8 strengthen, UInt32 price)
{
	auto item = ItemDataEntryMgr::Instance()->FindEntry(typeId);
	if (!item)
	{
		ErrorLogStream << "can't find item(" << typeId << ")..." << LogStream::eos;
		return false;
	}

	if (item->isTreasure == 1)
	{
		if (price < 1000 || price > 999999999)
		{
			return false;
		}
	}
	else
	{
		UInt32 addition = GetAuctionStrengthAddition(strengthen);

		UInt32 minPrice = (UInt32)((UInt64)item->recommendPrice * item->auctionMinPriceRate / 100 * (100 + addition) / 100);
		UInt32 maxPrice = (UInt32)((UInt64)item->recommendPrice * item->auctionMaxPriceRate / 100 * (100 + addition) / 100);

		if (price < minPrice || price > maxPrice)
		{
			return false;
		}
	}

	return true;
}

void HandleProtocol(CLProtocol::WorldAuctionCheck &req)
{
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(req.playerId);
    if (player == NULL)
    {
        ErrorLogStream << "auction item chest, can't find player(" << req.playerId << ")." << LogStream::eos;
        return;
    }

	if (req.num == 0)
	{
		ErrorLogStream << "auction item chest, player(" << req.playerId << ") invalid num." << LogStream::eos;
		return;
	}

	UInt32 itemTypeId = req.typeId;
	//UInt8 strengthen = 0;
	Item	*pItem = NULL;
	
	if (req.isAgain) //重新上架
	{
		UInt32 depositItemTypeId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_DEPOSIT_ITEM_ID);

		Avalon::Packet* pPacket = Avalon::Packet::Create();
		pPacket->SetID(CLProtocol::WORLD_AUCTION_CHECK_RET);
		Avalon::NetOutputStream stream(pPacket->GetBuffer(), pPacket->GetMaxSize());
		UInt8 depositEnough = 0;
		if (player->GetItemNum(depositItemTypeId) < req.deposit)
		{
			ErrorLogStream << "again auction gold check, player" << req.playerId << ") don't have enough gold to pay deposit, need (" << req.num << ")." << LogStream::eos;
			player->SendNotify(1040);
			depositEnough = 0;
			try
			{
				stream & req.isAgain & depositEnough & req.auctionGuid;
			}
			catch (Avalon::NetException&)
			{
				Avalon::Packet::Destroy(pPacket);
				return;
			}

			pPacket->SetSize(stream.GetPos());
			Router::SendToWorld(pPacket);
			Avalon::Packet::Destroy(pPacket);
			return;
		}

		depositEnough = 1;
		std::string reason = GetReason(SOURCE_TYPE_ACTION_UP, req.id, req.num);

		try
		{
			stream & req.isAgain & depositEnough & req.auctionGuid & req.playerId & req.type & req.price & req.pricetype & req.duration & req.counterFee;
		}
		catch (Avalon::NetException&)
		{
			Avalon::Packet::Destroy(pPacket);
			return;
		}

		pPacket->SetSize(stream.GetPos());
		Router::SendToWorld(pPacket);
		Avalon::Packet::Destroy(pPacket);

		player->RemoveItem(reason.c_str(), depositItemTypeId, req.deposit);
		return;
	}

	if(req.type == AT_ITEM)
	{
		pItem = player->FindCarryOnItem(req.id);
		if (pItem == NULL)
		{
            ErrorLogStream << "auction item check, player" << req.playerId << ") can't find item(" << req.id << ")." << LogStream::eos;
			return;
		}

		if (pItem->GetDataID() != req.typeId)
		{
			ErrorLogStream << "auction item check, player" << req.playerId << ") item(" << req.id << ", " << player->GetDataID() << ") invalid item typeid(" << req.typeId << ")." << LogStream::eos;
			return;
		}

		UInt8 strength = pItem->GetStrengthen();
		if (strength != req.strength)
		{
			ErrorLogStream << "auction item check, player" << req.playerId << ") item(" << req.id << ") can't sell. strength err strength = "
				<< strength << ", req.strength = " << req.strength << LogStream::eos;
			return;
		}

		if (pItem->GetDataEntry()->subType == ST_BEAD)
		{
			UInt32 beadBuffid = pItem->GetParam2();
			if (beadBuffid != req.beadbuffId)
			{
				ErrorLogStream << "auction item check, player" << req.playerId << ") item(" << req.id << ") can't sell. bendbuffid err beadBuffid = "
					<< beadBuffid << ", req.beadbuffId = " << req.beadbuffId << LogStream::eos;
				return;
			}
		}

		if (pItem->IsInAuctionCoolTimeStamp())
		{
			ErrorLogStream << "auction item check, player" << req.playerId << ") item(" << req.id << ") can't sell is in auction transaction cool time." << LogStream::eos;
			player->SendNotify(9950);
			return;
		}

		if (!SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_AUCTION_TREAS) && pItem->IsTreas())
		{
			player->SendNotify(9951);
			return;
		}

		if (pItem->IsItemLock(ILT_LEASE_LOCK | ILT_ITEM_LOCK))
		{
			ErrorLogStream << "auction item check is lock, player" << req.playerId << ") item(" << req.id << ") can't sell." << LogStream::eos;
			return;
		}

        if (!pItem->CanAuctionSell(req.num))
        {
            ErrorLogStream << "auction item check, player" << req.playerId << ") item(" << req.id << ") can't sell." << LogStream::eos;
            return;
        }

		if (!pItem->CheckDueTimeForAuction(player))
		{
			ErrorLogStream << "auction item check duetime, player" << req.playerId << ") item(" << req.id << ") can't sell." << LogStream::eos;
			player->SendNotify(10038);
			return;
		}
		if (!pItem->CheckAuctionTransNum())
		{
			ErrorLogStream << "auction item check trans times re, player" << req.playerId << ") item(" << req.id << ") can't sell." << LogStream::eos;
			player->SendNotify(10043);
			return;
		}

		std::vector<UInt32> inscriptionIds;
		pItem->GetInscriptionHoleProperty().GetRaceInfo(inscriptionIds);
		if (inscriptionIds.size() > 0)
		{
			ErrorLogStream << "auction item check inscription, player" << req.playerId << ") item(" << req.id << ") can't sell." << LogStream::eos;
			return;
		}

		if (SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_EQUIP_SCHEME) && pItem->IsCanPushScheme() && player->GetEqSchemeMgr().IsEquipInScheme(pItem->GetID()))
		{
			ErrorLogStream << "auction item check, player" << req.playerId << ") item(" << req.id << ", " << player->GetDataID() << ") item is in scheme" << LogStream::eos;
			player->SendNotify(1000133);
			return;
		}
	}
    else if (req.type == AT_GOLD)
    {
		auto maxNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GOLDAUCTION_DAILY_MAX_ONSALENUM);
		if (player->GetCounter(COUNTER_SELL_GOLD_NUM) >= maxNum)
        {
            ErrorLogStream << "auction item check, player" << req.playerId << ") can't sell gold anymore." << LogStream::eos;
            return;
        }

        // 金币拍卖行传过来的是对应的物品，如100W金币的物品等，这里需要扣去相应的金币
		itemTypeId = req.id;
        ItemDataEntry* itemData = ItemDataEntryMgr::Instance()->FindEntry(itemTypeId);
        if (itemData == NULL)
        {
            ErrorLogStream << "auction gold check, player" << req.playerId << ") gold item(" << req.id << ")." << LogStream::eos;
            return;
        }

        if (player->GetGold() < req.num)
        {
            ErrorLogStream << "auction gold check, player" << req.playerId << ") don't have enough gold to sell, need gold(" << req.num << ")." << LogStream::eos;
            return;
        }
    }

	// 检查价格是否合法
	/*if (!IsValidAuctionPrice(itemTypeId, strengthen, req.price / req.num))
	{
	InfoLogStream << "player(" << player->GetID() << ") sell auction item(" << itemTypeId << ") invalid price(" << req.price / req.num << ")." << LogStream::eos;
	player->SendNotify(1101);
	return;
	}*/

	UInt32 depositItemTypeId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_DEPOSIT_ITEM_ID);
    if (player->GetItemNum(depositItemTypeId) < req.deposit)
    {
        ErrorLogStream << "auction gold check, player" << req.playerId << ") don't have enough gold to pay deposit, need (" << req.num << ")." << LogStream::eos;
        player->SendNotify(1040);
        return;
    }
	
	std::string reason = GetReason(SOURCE_TYPE_ACTION_UP, req.id, req.num);

	Avalon::Packet* pPacket = Avalon::Packet::Create();
	pPacket->SetID(CLProtocol::WORLD_AUCTION_CHECK_RET);
	Avalon::NetOutputStream stream(pPacket->GetBuffer(), pPacket->GetMaxSize());
	try
	{
		UInt8   depositEnough = 0;
		stream & req.isAgain & depositEnough & req.auctionGuid & req.playerId & req.type & req.price & req.pricetype & req.duration & req.counterFee;

		if(AT_ITEM == req.type)
		{
			UInt32 oldNum = pItem->GetNum();
			pItem->SetNum(req.num);
			stream & pItem->GetID() & pItem->GetDataID();
			pItem->Encode(stream, SST_WORLD, false);
			pItem->SetNum(oldNum);

			player->ReduceItemNum(reason.c_str(), pItem, req.num);
		}
        else if (AT_GOLD == req.type)
        {
            stream & (UInt32)req.id;
			player->RemoveGold(reason.c_str(), req.num);
            player->IncCounter(COUNTER_SELL_GOLD_NUM, 1);
        }
        else
        {
			Avalon::Packet::Destroy(pPacket);
            return;
        }
	}
	catch(Avalon::NetException&)
	{
		Avalon::Packet::Destroy(pPacket);
		return;
	}

	pPacket->SetSize(stream.GetPos());
	Router::SendToWorld(pPacket);
	Avalon::Packet::Destroy(pPacket);

	player->RemoveItem(reason.c_str(), depositItemTypeId, req.deposit);
}

void HandleProtocol(Player* player, CLProtocol::SceneAuctionRefreshReq& req)
{
	if (!player)
	{
		return;
	}

	CLProtocol::SceneAuctionRefreshRes res;
	res.result = player->OnPlayerRefreshAuction();
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneAuctionBuyBoothReq& req)
{
	if (!player)
	{
		return;
	}

	CLProtocol::SceneAuctionBuyBoothRes res;
	res.result = player->OnPlayerBuyAuctionBooth();
	res.boothNum = player->GetAuctionAdditionBooth();
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneAbnormalTransactionRecordReq& req)
{
	if (!player) return;

	player->GetAbnormalTransactionMgr().OnQueryAbnormalTransactionRecordReq();
}

void HandleProtocol(CLProtocol::SceneAbnormalTransRemoveMailItemRes& res)
{
	Player* player = PlayerMgr::Instance()->FindPlayer(res.owner);
	if (!player)
	{
		ErrorLogStream << "Can not find player(" << res.owner << ") while remove mail item from world for abnormal transaction!" << LogStream::eos;
		return;
	}

	player->GetAbnormalTransactionMgr().OnRemoveMailItemRes(res);
}

void HandleProtocol(CLProtocol::WorldNotifyQueryWaitHandleAbnormalTrans& protocol)
{
	auto player = PlayerMgr::Instance()->FindNormalPlayer(protocol.roleid);
	if (!player) return;

	//player->GetAbnormalTransactionMgr().QueryWaitFreezeAbnormalTrans();
	AbnormalTransactionMgr::QueryWaitFreezeAbnormalTransNew();
}

void HandleProtocol(CLProtocol::SceneAuctionCheckPackageReq& req)
{
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(req.playerId);
	if (player == NULL) return;

	CLProtocol::SceneAuctionCheckPackageRet ret;
	ret.playerId = req.playerId;

	RewardGroup* rewardGroup = player->GetRewardGroup();

	ItemRewardVec getItems;
	for (size_t i = 0; i < req.items.size(); ++i)
	{
		ItemDataEntry* data = ItemDataEntryMgr::Instance()->FindEntry(req.items[i].id);
		if (!data)
		{
			continue;
		}

		if (data->getLimitNum > 0)
		{
			if (player->GetItemNum(data->id) + req.items[i].num > data->getLimitNum)
			{
				player->SendNotify(9102, data->name.c_str());
				return;
			}
		}

		ItemReward item(req.items[i].id, req.items[i].num, req.items[i].qualityLv, req.items[i].strenth, req.items[i].auctionCoolTimeStamp, req.items[i].equipType, req.items[i].auctionTransTimes);
		getItems.push_back(item);
	}

	rewardGroup->AddRewards(getItems);
	UInt32 nRes = player->CheckAddRewards(rewardGroup);
	if (ErrorCode::SUCCESS != nRes)
	{
		player->SendNotify(nRes);
		return;
	}
	else
	{
		ret.auctionGuid = req.auctionGuid;
	}

	Router::SendToWorld(ret);
}
