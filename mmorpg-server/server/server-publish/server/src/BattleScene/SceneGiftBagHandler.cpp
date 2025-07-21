#include "SceneGiftBagHandler.h"

#include "Player.h"
#include "ScriptMgr.h"

void HandleProtocol(Player* player, CLProtocol::SceneGetGiftbag& protocol)
{
	
}

void HandleProtocol(Player *pPlayer, CLProtocol::SceneGetActiveGiftbag &protocol)
{
	
}

void HandleProtocol(CLProtocol::WorldNotifyGameStartTimeToS &protocol)
{
	GAMESTART_TIME = protocol.time;
	GAME_FIRST_START_TIME = protocol.firstTime;

	InfoLogStream << "SceneServer receive sync, GAMESTART_TIME=" << GAMESTART_TIME << ", GAME_FIRST_START_TIME=" << GAME_FIRST_START_TIME << LogStream::eos;
}

void HandleProtocol(Player* pPlayer, CLProtocol::SceneFestivalCounterReq& protocol)
{
	CLProtocol::SceneFestivalCounterRet ret;
	ret.name = protocol.name;
	ret.value = pPlayer->GetCounter(protocol.name.c_str());
	pPlayer->SendProtocol(ret);
}

void HandleProtocol(Player *pPlayer, CLProtocol::SceneGSActivityStoreGold &protocol)
{
	
}

void HandleProtocol( Player* pPlayer, CLProtocol::SceneBuyGiftBag& protocol )
{
	
}

void HandleProtocol(Player* player, CLProtocol::SceneGiftPackInfoReq& req)
{
	if (!player) return;

	CLProtocol::SceneGiftPackInfoRes res;

	for (auto giftPackId : req.giftPackIds)
	{
		GiftPackData* giftPackData = GiftPackDataMgr::Instance()->FindEntry(giftPackId);
		if (!giftPackData)
		{
			ErrorLogStream << PLAYERINFO(player) << "Can not find gift pack data(" << giftPackId << ")!" << LogStream::eos;
			continue;
		}

		CLProtocol::GiftPackSyncInfo giftPack;
		giftPack.id = giftPackData->giftPackId;
		giftPack.filterType = giftPackData->filterType;
		giftPack.filterCount = giftPackData->filterCount;
		giftPack.uiType = giftPackData->uiType;
		giftPack.description = giftPackData->description;

		for (auto giftId : giftPackData->giftItems)
		{
			GiftItemData* giftData = GiftItemDataMgr::Instance()->FindEntry(giftId);
			if (!giftData)
			{
				ErrorLogStream << PLAYERINFO(player) << "Can not find gift(" << giftId << ")!" << LogStream::eos;
				continue;
			}

			CLProtocol::GiftSyncInfo gift;
			gift.id = giftData->giftId;
			gift.itemId = giftData->itemId;
			gift.itemNum = giftData->num;
			gift.recommendJobs = giftData->occus;
			gift.weight = giftData->weight;
			gift.validLevels = giftData->levels;

			giftPack.gifts.push_back(gift);
		}

		res.giftPacks.push_back(giftPack);
	}

	player->SendProtocol(res);
}
