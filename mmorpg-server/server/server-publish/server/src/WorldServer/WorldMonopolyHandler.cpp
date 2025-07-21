#include "CLWorldMonopolyProtocol.h"

#include "WSPlayerMgr.h"
#include "Monopoly.h"
#include "WorldMonopolyHandler.h"


void HandleProtocol(WSPlayer* player, CLProtocol::WorldMonopolyCoinReq &req)
{
	player->GetMonopolyInfo().SendCoin();
}

void HandleProtocol(WSPlayer * player, CLProtocol::WorldMonopolyStatusReq & req)
{
	player->GetMonopolyInfo().SendStatus();
}

void HandleProtocol(WSPlayer * player, CLProtocol::WorldMonopolyRollReq & req)
{
	CLProtocol::WorldMonopolyRollRes res;
	auto& info = player->GetMonopolyInfo();
	res.errorCode = info.OnOwnerRollOnce(res.step);
	info.SendStatus();
	player->SendProtocol(res);
}

void HandleProtocol(WSPlayer * player, CLProtocol::WorldMonopolyCardExchangeReq & req)
{
	CLProtocol::WorldMonopolyCardExchangeRes res;
	res.errorCode = ErrorCode::ITEM_DATA_INVALID;
	auto& info = player->GetMonopolyInfo();
	auto data = MonopolyCardRewardDataEntryMgr::Instance()->FindEntry(req.id);
	if (data == nullptr)
	{
		player->SendProtocol(res);
		return;
	}

	if (info.GetCardExchange(req.id) >= data->limit)
	{
		player->SendProtocol(res);
		return;
	}

	if (data->type == MCRT_NORMAL)
	{
		for (auto& card : data->costCard)
		{
			if (info.GetCardNum(card.id) < card.num)
			{
				player->SendProtocol(res);
				return;
			}
		}

		for (auto& card : data->costCard)
		{
			info.RemoveCard(card.id, card.num);
		}
	}
	else
	{
		for (auto& card : data->costCard)
		{
			auto tempData = MonopolyCardRewardDataEntryMgr::Instance()->FindEntry(card.id);
			if (tempData == nullptr)
			{
				player->SendProtocol(res);
				return;
			}
			if (info.GetCardExchange(tempData->id) < tempData->limit && tempData->type == MCRT_NORMAL)
			{
				player->SendProtocol(res);
				return;
			}
		}
	}

	info.AddCardExchange(req.id, info.GetCardExchange(req.id) + 1);

	player->AddItem(GetReason(LogSourceType::SOURCE_MONOPOLY), data->reward);

	res.errorCode = ErrorCode::SUCCESS;
	player->SendProtocol(res);
	info.SendCardList();
}

void HandleProtocol(WSPlayer * player, CLProtocol::WorldMonopolyCardListReq & req)
{
	player->GetMonopolyInfo().SendCardList();
}

void HandleProtocol(WSPlayer * player, CLProtocol::WorldMonopolySellCardReq & req)
{
	CLProtocol::WorldMonopolySellCardRes res;
	res.errorCode = ErrorCode::SUCCESS;
	auto cardData = MonopolyCardDataEntryMgr::Instance()->FindEntry(req.id);
	if (cardData == nullptr)
	{
		res.errorCode = ErrorCode::ITEM_DATA_INVALID;
		player->SendProtocol(res);
		return;
	}
	auto& info = player->GetMonopolyInfo();
	if (info.GetCardNum(req.id) < req.num)
	{
		res.errorCode = ErrorCode::ITEM_DATA_INVALID;
	}
	else
	{
		info.RemoveCard(req.id, req.num);
		info.SetCoin(info.GetCoin() + cardData->coinNum * req.num, GetReason(SOURCE_MONOPOLY_SELL_CARD, req.id, req.num));
	}
	player->SendProtocol(res);
	info.SendCardList();
}

void HandleProtocol(WSPlayer * player, CLProtocol::WorldMonopolyCardExchangedListReq & req)
{
	CLProtocol::WorldMonopolyCardExchangedListRes res;
	auto& cardExchanged = player->GetMonopolyInfo().GetCardExchangeList();
	for (auto& cardIter : cardExchanged)
	{
		CLProtocol::MonpolyCard mc;
		mc.id = cardIter.first;
		mc.num = cardIter.second;
		res.cardList.push_back(mc);
	}
	player->SendProtocol(res);
}

void HandleProtocol(CLProtocol::WorldMonopolyGMJoinDungeon & req)
{
	auto player = WSPlayerMgr::Instance()->FindNormalPlayer(req.roleId);
	if (player == nullptr)
	{
		return;
	}
	player->GetMonopolyInfo().AddMissionLimit(req.dungeonId);
}

