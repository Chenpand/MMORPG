#include "WorldAuctionHandler.h"

#include <CLRecordClient.h>
#include <CLMailDefine.h>

#include "NewAuctionMgr.h"
#include "WSPlayerMgr.h"
#include "AuctionTransRecord.h"
#include "AuctionAttentRecord.h"

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAuctionListReq &req)
{
	//AuctionMgr::Instance()->SyncAuctionList(pPlayer, req);
    zjy_auction::AuctionMgr::Instance()->OnPlayerQueryAuctionList(player, req.cond);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAuctionItemNumReq &req)
{
	zjy_auction::AuctionMgr::Instance()->OnPlayerQueryAuctionItemNum(player, req.cond);
}

void HandleProtocol(WSPlayer* pPlayer, CLProtocol::WorldAuctionSelfListReq &req)
{
	//AuctionMgr::Instance()->SyncMineAuctionList(pPlayer);
    zjy_auction::AuctionMgr::Instance()->OnPlayerQuerySelfAuctionList(pPlayer, req.type);
}

void HandleProtocol(WSPlayer* pPlayer, CLProtocol::WorldAuctionRecommendPriceReq &req)
{
    //AuctionMgr::Instance()->SyncMineAuctionList(pPlayer);
    zjy_auction::AuctionMgr::Instance()->OnPlayerQueryRecommendPrice(pPlayer, (AuctionType)req.type, req.itemTypeId);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAuctionBuy &req)
{
	// 如果安全锁锁住，不允许在拍卖行购买物品
	if ( !player->SecurityLockCheck())
		return;

	//AuctionMgr::Instance()->BuyAuction(pPlayer, req.id);
    zjy_auction::AuctionMgr::Instance()->OnPlayerBuyAuctionItem(player, req.id, req.num);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAuctionRusyBuy &req)
{
	// 如果安全锁锁住，不允许在拍卖行购买物品
	if (!player->SecurityLockCheck())
		return;

	zjy_auction::AuctionMgr::Instance()->OnPlayerRusyBuyAuctionItem(player, req.id, req.num);
}

void HandleProtocol(WSPlayer* pPlayer, CLProtocol::WorldAuctionRequest &req)
{
	// 如果安全锁锁住，不允许拍卖物品
	if ( !pPlayer->SecurityLockCheck())
		return;
	if (!req.isAgain)
	{
		zjy_auction::AuctionMgr::Instance()->OnPlayerAddItemToAuction(pPlayer, (AuctionType)req.type, req.id, req.typeId, req.num,
			req.price, (zjy_auction::AuctionSellDuration)req.duration, req.strength, req.buffId, req.enhanceType);
	}
	else{
		zjy_auction::AuctionMgr::Instance()->OnPlayerAddItemToAuctionAgain(pPlayer, (AuctionType)req.type, req.auctionGuid, req.num, req.price, req.strength, req.buffId, req.enhanceType);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAuctionCancel &req)
{
	//AuctionMgr::Instance()->CancelAuction(pPlayer, req.id);
    zjy_auction::AuctionMgr::Instance()->OnPlayerCancelAuction(player, req.id);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAuctionQueryItemReq &req)
{
    zjy_auction::AuctionMgr::Instance()->OnPlayerQueryAuctionItem(player, req.id);
}

void HandleProtocol(WSPlayer* pPlayer, CLProtocol::WorldAuctionFindByID &req)
{
	//AuctionMgr::Instance()->GetAuctionInfo(pPlayer, req.id);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAuctionQueryItemPricesReq &req)
{
	zjy_auction::AuctionMgr::Instance()->OnPlayerQueryItemRecentPrices(player, (AuctionType)req.type, req.itemTypeId, req.strengthen, req.beadbuffid, req.enhanceType);
}

void AuctionCheckRetFunction::operator()(Avalon::Packet *pPacket) const
{
	Avalon::NetInputStream	stream(pPacket->GetBuffer(), pPacket->GetSize());

    zjy_auction::AuctionMgr::Instance()->OnAuctionCheckRet(stream);
}

void HandleProtocol(CLProtocol::SceneAbnormalTransRemoveMailItemReq& req)
{
	CLProtocol::SceneAbnormalTransRemoveMailItemRes resProtocol;
	resProtocol.abnormalTransId = req.abnormalTransId;
	resProtocol.preAbnormalTransId = req.preAbnormalTransId;
	resProtocol.owner = req.owner;
	resProtocol.itemDataId = req.itemDataId;
	resProtocol.needRmNum = req.needRmNum;
	resProtocol.successRmNum = 0;

	WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(req.owner);
	if (!player)
	{
		req.SendBack(resProtocol);
		ErrorLogStream << "Can not find player(" << req.owner << ")!" << LogStream::eos;
		return;
	}

	if (req.abnormalTransId == 0 || req.owner == 0 || req.itemDataId == 0 || req.needRmNum == 0)
	{
		req.SendBack(resProtocol);
		ErrorLogStream << PLAYERINFO(player) << "Error data(abnormalTransId=" << req.abnormalTransId << ", owner=" << req.owner 
			<< ", itemDataId=" << req.itemDataId << ", needRmNum=" << req.needRmNum << ")!" << LogStream::eos;
		return;
	}

	UInt32 needRemoveItemNum = resProtocol.needRmNum;

	player->GetMailBox().VisitMails([&](Mail* pMail)->bool{
		if (!pMail) return true;

		bool bRmSuccess = false;

		// 将邮件中的道具扣除
		for (auto& reward : pMail->items)
		{
			if (reward.id != resProtocol.itemDataId || reward.num == 0)
			{
				continue;
			}

			// 扣到剩余一个
			UInt32 canRmMailItemNum = reward.num - 1;

			if (canRmMailItemNum == 0)
			{
				continue;
			}

			if (needRemoveItemNum >= canRmMailItemNum)
			{
				needRemoveItemNum -= canRmMailItemNum;
				resProtocol.successRmNum += canRmMailItemNum;
				reward.num -= canRmMailItemNum;
			}
			else
			{
				reward.num -= needRemoveItemNum;
				resProtocol.successRmNum += needRemoveItemNum;
				needRemoveItemNum = 0;
			}

			bRmSuccess = true;
			player->SendNotify(9851);

			ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(resProtocol.itemDataId);
			if (dataEntry != NULL)
			{
				player->SendItemUdpLog(GetReason(LogSourceType::SOURCE_TYPE_MAIL_SYSTEM_TAKE).c_str(), pMail->titleInfo.id, dataEntry->id, (ItemType)dataEntry->type, (ItemSubType)dataEntry->subType, dataEntry->name.c_str(), (ItemQuality)dataEntry->color, reward.num, reward.num + canRmMailItemNum);
			}

			// 扣完了
			if (needRemoveItemNum == 0)
			{
				break;
			}
		}

		if (bRmSuccess)
		{
			CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_mail", pMail->titleInfo.id, true, player->GetID(), WSApplication::Instance()->GetTableCount());
			std::ostringstream stream;
			UInt32 itemnum = 0;
			for (auto reward : pMail->items)
			{
				if (reward.num == 0) continue;
				if (++itemnum > MAX_MAILITEM_NUM) break;
				reward.EncodeString(stream);
				stream << ';';
			}
			updateCmd->PutData("items", stream.str());
			CLRecordClient::Instance()->SendCommand(updateCmd, NULL, player->GetID());
		}

		// 全部扣完了
		if (needRemoveItemNum == 0) return false;

		return true;
	});

	req.SendBack(resProtocol);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAuctionGetTreasTransactionReq& req)
{
	CLProtocol::WorldAuctionGetTreasTransactionRes res;
	AuctionTransRecordMgr::Instance()->GetPlayerTrans(player, res.sales, res.buys);
	player->SendProtocol(res);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAuctionAttentReq &req)
{
	CLProtocol::WorldActionAttentRes res;
	res.code = AuctionAttentMgr::Instance()->HandleAttentReq(player, req.autionGuid);
	if (res.code == ErrorCode::SUCCESS)
	{
		zjy_auction::AuctionInfo* auctionInfo = zjy_auction::AuctionMgr::Instance()->GetAuctionInfo(player, req.autionGuid);
		if (auctionInfo)
		{
			res.auction = *(AuctionBaseInfo*)auctionInfo;
		}
	}
	player->SendProtocol(res);
}

void HandleProtocol(CLProtocol::SceneAuctionCheckPackageRet& ret)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(ret.playerId);
	if (player == NULL) return;

	zjy_auction::AuctionMgr::Instance()->OnSceneAuctionCheckPackageRet(player, ret.auctionGuid);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAuctionQueryItemPriceListReq& req)
{
	zjy_auction::AuctionMgr::Instance()->OnPlayerQueryItemRecentPricesList(player, (AuctionType)req.type, req.auctionState, req.itemTypeId, req.strengthen, req.enhanceType);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAuctionQueryMagicOnsalesReq& req)
{
	zjy_auction::AuctionMgr::Instance()->QueryMagicOnsales(player, req.itemTypeId);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAuctionQueryItemTransListReq& req)
{
	CLProtocol::WorldAuctionQueryItemTransListRes res;
	res.itemTypeId = req.itemTypeId;
	res.strengthen = req.strengthen;
	res.enhanceType = req.enhanceType;
	res.beadBuffId = req.beadBuffId;
	res.code = zjy_auction::AuctionMgr::Instance()->GetItemTransedRecords(req.itemTypeId, req.strengthen, req.beadBuffId, req.enhanceType, res.transList);
	player->SendProtocol(res);
}