#include "SceneItemHandler.h"

#include <CLMailProtocol.h>
#include <CLBillingProtocol.h>
#include <CLSysNotify.h>
#include <CLDBLogger.h>
#include <CLErrorCode.h>
#include <CLDrawPrizeDataEntry.h>

#include "ScriptMgr.h"
#include "PlayerScript.h"
#include "Player.h"
#include "PlayerMgr.h"
#include "Scene.h"
#include "Team.h"
#include "SSRouter.h"
#include "SceneSysNotify.h"
#include "ActivityMgr.h"
#include "SSServiceSwitchMgr.h"
#include "CondActivateEvent.h"
#include "StrengthenTicketPlanMgr.h"
#include "LostBattleScene.h"
#include "SceneMgr.h"

void HandleProtocol(Player* player, CLProtocol::SceneMoveItem& protocol)
{
	
}

void HandleProtocol(Player* player, CLProtocol::SceneUseItem& protocol)
{
	//中断动作

	//if (SERVER_TYPE == ST_WSCENE)
	//{
	//	Router::SendToSelfScenePlayer(player->GetSrcSceneNodeID(), player->GetID(), protocol);
	//	return;
	//}
	if (player->GetLastLostBattleSceneId() > 0)
	{
		Item* item = player->FindItem(protocol.itemid);
		if (!item)
		{
			protocol.param2 = 2;
			Router::SendToSelfScenePlayer(player->GetSrcSceneNodeID(), player->GetID(), protocol);
			return;
		}
	}
	
	UInt32 code = player->UseItem(protocol.itemid, protocol.useAll, protocol.param1, protocol.param2);
	CLProtocol::SceneUseItemRet ret;
	ret.errcode = code;
	player->SendProtocol(ret);
}

void HandleProtocol(Player* player, CLProtocol::SceneSellItem& protocol)
{
	UInt32 code = player->SellItem(protocol.itemid, protocol.num);
	CLProtocol::SceneSellItemRet ret;
	ret.errcode = code;
	player->SendProtocol(ret);
}

void HandleProtocol(Player* player, CLProtocol::SceneEnlargePackage& protocol)
{
	UInt32 code = player->EnlargePackage();
	CLProtocol::SceneEnlargePackageRet ret;
	ret.code = code;
	player->SendProtocol(ret);
}

//废弃
void HandleProtocol(Player* player, CLProtocol::SceneRemakeItem& protocol)
{
	return;

	CLProtocol::SceneRemakeItemResult result;
	result.remakeType = protocol.remakeType;
	result.result = 0;

	switch(protocol.remakeType)
	{
	case EQUIP_REMAKE_INLAY:
		{
			
		}
		break;
	case EQUIP_REMAKE_STRENGTHEN:
		{
			
		}
		break;
	case EQUIP_REMAKE_XIYUAN:
		{
			
		}
		break;
	case EQUIP_REMAKE_COMPOSE_STONE:
		{
			
		}
		break;
	case EQUIP_REMAKE_UPGRADEQUALITY:
		{
			
		}
		break;
	case EQUIP_REMAKE_INLAY_RUNE:
		{
			
		}
		break;
	case EQUIP_REMAKE_TAKEAPART_RUNE:
		{
			
		}
		break;
	default:
		return;
	}

	player->SendProtocol(result);
}

void HandleProtocol(CLProtocol::SceneCheckItemNumReq& req)
{
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(req.playerId);
	if(player == NULL) return;

	UInt32 sceneId = player->GetLastLostBattleSceneId();
	LostBattleScene* battleScene = dynamic_cast<LostBattleScene*>(SceneMgr::Instance()->FindNormalScene(sceneId));
	if (battleScene)
	{
		req.requestServerType = ST_BATTLE;
		player->SendToSelfScene(req);
		return;
	}

	CLProtocol::SceneCheckItemNumRet ret;
	ret.playerId = req.playerId;
	ret.serial = req.serial;
	ret.result = ErrorCode::ITEM_TRANS_FAILED;

	if (!player->CheckRmMoney(ST_ITEM_GOLD, req.silver) ||
		!player->CheckRmMoney(ST_BIND_GOLD, req.bindSilver) ||
		!player->CheckRmMoney(ST_ITEM_POINT, req.gold) ||
		!player->CheckRmMoney(ST_BIND_POINT, req.bindGold) ||
		!player->CheckRmMoney(ST_GUILD_CONTRI, req.guildContri))
	{
		ret.result = ErrorCode::ITEM_TRANS_NOT_ENOUGH_MONEY;
		req.SendBack(ret);
		return;
	}

	if (req.reviveCoin > 0)
	{
		UInt32 vipRevive = 0;
		player->GetVipValue(VIP_PRIVILEGE_FREE_REVIVE, vipRevive);
		if (vipRevive <= player->GetCount(COUNTER_VIP_REVIVE_TIMES))
		{
			if (player->GetReviveCoin() < req.reviveCoin)
			{
				ret.result = ErrorCode::ITEM_TRANS_NOT_ENOUGH_REVIVE_COIN;
				req.SendBack(ret);
				return;
			}
		}
	}

	for (auto& itemIter : req.items)
	{
		UInt32 itemNum = 0;
		if (itemIter.second.isEqualItem)
		{
			itemNum = player->GetItemNumWithEqualItem(itemIter.first);
		}
		else
		{
			itemNum = player->GetItemNum(itemIter.first);
		}

		if (itemNum < itemIter.second.itemNum)
		{
			ret.result = ErrorCode::ITEM_TRANS_NOT_ENOUGH_ITEM;
			req.SendBack(ret);
			return;
		}
	}

	for(std::map<UInt32, UInt32>::iterator iter = req.nobindItems.begin();
		iter != req.nobindItems.end(); ++iter)
	{
		if(player->GetNoBindItemNum(iter->first) < iter->second)
		{
			ret.result = ErrorCode::ITEM_TRANS_NOT_ENOUGH_ITEM;
			req.SendBack(ret);
			return;
		}
	}

	if (!req.counterName.empty() && req.needCount > 0)
	{
		UInt32 count = player->GetCount(req.counterName.c_str());
		ret.counterTimes = count;
		if (count >= req.maxCount || req.maxCount - count < req.needCount)
		{
			ret.result = ErrorCode::ITEM_TRANS_NOT_ENOUGH_TIMES;
			req.SendBack(ret);
			return;
		}
	}

	for (auto& detailItem : req.detailItems)
	{
		auto item = player->FindItem(detailItem.guid);
		if (!item || item->GetDataID() != detailItem.itemTypeId || item->GetNum() < detailItem.num)
		{
			ret.result = ErrorCode::ITEM_TRANS_NOT_ENOUGH_ITEM;
			req.SendBack(ret);
			return;
		}
	}

	ret.result = ErrorCode::SUCCESS;
	req.SendBack(ret);
}

void HandleProtocol(CLProtocol::SceneRemoveItemReq& req)
{
	Player* player = PlayerMgr::Instance()->FindPlayer(req.playerId);
	if(player == NULL || player->GetGameStatus() == PS_INIT) return;

	UInt32 sceneId = player->GetLastLostBattleSceneId();
	LostBattleScene* battleScene = dynamic_cast<LostBattleScene*>(SceneMgr::Instance()->FindNormalScene(sceneId));
	if (battleScene)
	{
		player->SendToSelfScene(req);
		//return;
	}


	player->RemoveGold(req.reason.c_str(), req.silver);
	player->RemoveGoldBoth(req.reason.c_str(), req.bindSilver);
	player->RemovePoint(req.reason.c_str(), req.gold);
	player->RemovePointBoth(req.reason.c_str(), req.bindGold);
	player->RemoveGuildContribution(req.reason.c_str(), req.guildContri);

	// 优先用vip免费次数
	if (req.reviveCoin > 0)
	{
		UInt32 vipRevive = 0;
		player->GetVipValue(VIP_PRIVILEGE_FREE_REVIVE, vipRevive);
		if (vipRevive <= player->GetCount(COUNTER_VIP_REVIVE_TIMES))
		{
			player->RemoveReviveCoin(req.reason.c_str(), req.reviveCoin);
		}
		else
		{
			player->IncCounter(COUNTER_VIP_REVIVE_TIMES, 1);
		}
	}

	// 次数
	if (!req.counterName.empty() && req.needCount > 0)
	{
		player->IncCounter(req.counterName.c_str(), req.needCount);
	}
	
	for (auto& iterItem : req.items)
	{
		if (iterItem.second.isEqualItem)
		{
			player->RemoveItemWithEqualItem(req.reason.c_str(), iterItem.first, iterItem.second.itemNum);
		}
		else
		{
			player->RemoveItem(req.reason.c_str(), iterItem.first, iterItem.second.itemNum);
		}
	}

	for(std::map<UInt32, UInt32>::iterator iter = req.nobindItems.begin();
		iter != req.nobindItems.end(); ++iter)
	{
		player->RemoveNoBindItem(req.reason.c_str(), iter->first, iter->second);
	}

	for (auto& detailItem : req.detailItems)
	{
		auto item = player->FindItem(detailItem.guid);
		if (item)
		{
			player->ReduceItemNum(req.reason.c_str(), item, detailItem.num);
		}
		else
		{
			ErrorLogStream << "player(" << player->GetID() << ") can't find item(" << detailItem.guid << ")." << LogStream::eos;
		}
	}

	player->SyncToRecord();
}

void HandleProtocol(CLProtocol::SceneUnlockItemReq& req)
{
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(req.playerId);
	if(player == NULL) return;

}

void HandleProtocol(CLProtocol::SceneRemoveItemObjReq& req)
{
// 	Player* player = PlayerMgr::Instance()->FindPlayer(req.playerId);
// 	if(player == NULL || player->GetGameStatus() == PS_INIT) return;
// 
// 	player->UnlockAssets();
// 
// 	Item* item = player->FindCarryOnItem(req.itemId);
// 	if(item != NULL)
// 	{
// 		ItemDataEntry* dataEntry = item->GetDataEntry();
// 		player->ReduceItemNum(req.reason.c_str(), item, 1);
// 
// 		player->OnUseItem(dataEntry->id);
// 
// 		if(dataEntry->cdgroup != 0)
// 		{
// 			UInt32 cdoffset = UInt32(dataEntry->cd * 0.1f);
// 			if(cdoffset > 2000) cdoffset = 2000;
// 			player->SetItemUseTime(dataEntry->cdgroup, CURRENT_TIME.MSec() + dataEntry->cd - cdoffset);
// 		}
// 	}
}

void HandleProtocol(CLProtocol::SceneAddMoneyItemReq& protocol)
{
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if(player == NULL) return;

	player->AddMoneyItem(protocol.reason.c_str(), protocol.type, protocol.num);
}

void HandleProtocol(CLProtocol::WSRefreshLimiteBuyNum &req)
{
	CLProtocol::SWRefreshLimiteBuyNum ret;
	ret.mallType = req.mallType;
	ret.isQuerySingleGoods = req.isQuerySingleGoods;
	
	Player	*pPlayer = PlayerMgr::Instance()->FindNormalPlayer(req.playerid);
	if (NULL == pPlayer)
	{
		GameErrorLogStream("MallItem") << "player(," << req.playerid << ",) is offline when query items!" << LogStream::eos;
		return;
	}

	//最终的打折率
	Int32 ultimateRate = -1;
	for (size_t i = 0; i < req.qgs.size(); ++i)
	{
		//有限购的商品更新次数
		do 
		{
			MallLimitBuyType mallLimitBuyType = (MallLimitBuyType)req.qgs[i].bLimit;
			bool needCheck = false;
			switch (mallLimitBuyType)
			{
			case MLBT_DAILY_REFRESH:
			case MLBT_WEEKLY_REFRESH:
				needCheck = true;
				break;
			default:
				break;
			}

			if (!needCheck)
			{
				break;
			}

			std::ostringstream oss;
			oss << req.qgs[i].id;

			if (CounterCfg::Instance()->GetCycleType(oss.str()) == COUNTER_CYCLE_INVALID)
			{
				switch (mallLimitBuyType)
				{
				case MLBT_DAILY_REFRESH:
					CounterCfg::Instance()->RegCounter(oss.str().c_str(), COUNTER_CYCLE_DAY, 6);
					break;
				case MLBT_WEEKLY_REFRESH:
					CounterCfg::Instance()->RegCounter(oss.str().c_str(), COUNTER_CYCLE_MONDAY_HOUR, 6);
					break;

				default:
					break;
				}
			}

			pPlayer->GetCounter(oss.str().c_str());

		} while (0);

		// 0不打折
		if (req.qgs[i].discountRate == 0)
		{
			continue;
		}

		//只生成一次，生成后所有打折商品共用一个打折率
		if (ultimateRate >= 0)
		{
			req.qgs[i].discountRate = ultimateRate;
			continue;
		}

		OpActivityRegInfo* newServerDiscountInfo = ActivityMgr::Instance()->GetOpActData(OAID_MALL_DISCOUNT_FOR_NEW_SERVER);
		if (newServerDiscountInfo != NULL && newServerDiscountInfo->state == (UInt8)AS_IN)
		{
			UInt32 everBuyFashionNum = pPlayer->GetCounter(PLAYER_BUY_MALL_FASHION_NUM);
			//只有前n件时装打折
			if (everBuyFashionNum >= SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_MALL_VALID_DISCOUNT_NUM))
			{
				ultimateRate = 0;
			}
			else
			{
				ultimateRate = newServerDiscountInfo->parm;
			}
		}

		if (ultimateRate == -1)
		{
			ultimateRate = 0;
		}

		req.qgs[i].discountRate = ultimateRate;
	}
	
	ret.code = ErrorCode::SUCCESS;
	ret.playerid = req.playerid;
	ret.qgs = req.qgs;
	ret.isPersonalTailor = req.isPersonalTailor;
	req.SendBack(ret);
}

void HandleProtocol(CLProtocol::WorldMallCheckBuy &req)
{
	Player	*pPlayer = PlayerMgr::Instance()->FindNormalPlayer(req.m_uPlayerId);
	if(NULL == pPlayer)
	{
		GameErrorLogStream("MallItem") << "player(," << req.m_uPlayerId << ",) is offline when give items!" << LogStream::eos;
		return;
	}

	pPlayer->OnBuyMallGoods(req);
}

void HandleProtocol(CLProtocol::WolrdLoadConditionActivateDBNotify& ret)
{
	Player* player = PlayerMgr::Instance()->FindPlayer(ret.m_RoleId);
	if (!player)
	{
		ErrorLogStream << "Failed to find player(roleid: " << ret.m_RoleId << ")!" << LogStream::eos;
		return;
	}

	if (!ret.m_IsLoaded)
	{
		ErrorLogStream << "Player(roleid: " << ret.m_RoleId << ", load condition activate DB failed!" << LogStream::eos;
		return;
	}

	player->SetConditionActivateDBLoadState((UInt8)CADBLS_DONE);
}

void HandleProtocol(Player* player, CLProtocol::SceneComposeItemReq& protocol)
{
	
}

void HandleProtocol(Player* player, CLProtocol::SceneTrimItem& protocol)
{
	//player->CancelTrade();
	player->TrimItems(protocol.pack);

	CLProtocol::SceneTrimItemRet ret;
	ret.code = ErrorCode::SUCCESS;
	player->SendProtocol(ret);
}

// 废弃
void HandleProtocol(Player* player, CLProtocol::SceneExchangeItemsReq& protocol)
{
	return;

	CLProtocol::SceneExchangeItemsRet ret;
	ret.pos = protocol.pos;
	ret.items = ScriptMgr::Instance()->GetPlayerScript()->GetExchangeItems(protocol.pos);
	player->SendProtocol(ret);
}

// 废弃
void HandleProtocol(Player* player, CLProtocol::SceneExchangeItem& protocol)
{

}

// 废弃
void HandleProtocol( Player* pPlayer, CLProtocol::SceneItemLockReq& protocol )
{
	return;

	CLProtocol::SceneItemLockRet itemLockRet;
	itemLockRet.itemLockType = protocol.itemLockType;
	itemLockRet.result = 0;

	switch(protocol.itemLockType)
	{
	case ILT_NEW_PASSWD:
		{
		}
		break;
	case ILT_VERIFY_PASSWD:
		{
			
		}
		break;
	case ILT_FORGET_PASSWD:
		{
			
		}
		break;	
	case ILT_RESET_PASSWD:
		{
			
		}
		break;
	case ILT_CACEL_LOCK:
		{
			
		}
		break;
	case ILT_LOCK_RANGE:
		{
			
		}
		break;
	default:
		{

		}
		break;
	}

	pPlayer->SendProtocol(itemLockRet);
}

//废弃
void HandleProtocol(Player* pPlayer, CLProtocol::SceneRandEquipShopBuy& protocol)
{
	return;
	pPlayer->GetEquipShopMgr().Buy(protocol.type, protocol.index);
}

void HandleProtocol(Player* player, CLProtocol::ScenePushStorage& protocol)
{
	CLProtocol::ScenePushStorageRet ret;
	ret.code = player->PushStorage(protocol.itemid, protocol.num, protocol.storageType);
	player->SendProtocol(ret);
}

void HandleProtocol(Player* player, CLProtocol::ScenePullStorage& protocol)
{
	CLProtocol::ScenePullStorageRet ret;
	ret.code = player->PullStorage(protocol.itemid, protocol.num);
	player->SendProtocol(ret);
}

void HandleProtocol(Player* player, CLProtocol::SceneOneKeyPushStorage& protocol)
{
	CLProtocol::SceneOneKeyPushStorageRet ret;
	ret.code = player->OneKeyPushStorage();
	player->SendProtocol(ret);
}

void HandleProtocol(Player* player, CLProtocol::SceneEnlargeStorage& protocol)
{
	UInt32 code = player->EnlargeStorage();
	if (code != ErrorCode::SUCCESS)
	{
		CLProtocol::SceneEnlargeStorageRet ret;
		ret.code = code;
		player->SendProtocol(ret);
	}
	
}

void HandleProtocol(Player* player, CLProtocol::SceneEquipDecompose& protocol)
{
	CLProtocol::SceneEquipDecomposeRet ret;
	ret.code = player->DecomposeEquip(GetReason(SOURCE_TYPE_EQUIP_DECOMPOSE).c_str(), protocol.uidList, ret.getItems);
	player->SendProtocol(ret);
}

void HandleProtocol(Player* player, CLProtocol::SceneEquipStrengthen& protocol)
{
	CLProtocol::SceneEquipStrengthenRet ret;
	player->ClearRewardGroup();

	ret.code = player->StrengthenEquip(protocol.equipUid, (protocol.useUnbreak != 0), protocol.strTicket);

	const std::vector<ItemReward> rewards = player->GetRewardGroupNoClear()->GetRewards();
	std::vector<ItemReward>& mergeRewards = ret.matNums;

	//合并奖励
	for (std::vector<ItemReward>::const_iterator iter = rewards.begin();
		iter != rewards.end(); ++iter)
	{
		bool bFind = false;
		ItemDataEntry* data = ItemDataEntryMgr::Instance()->FindEntry(iter->id);
		if (!data)
			continue;

		DebugLogStream << PLAYERINFO(player) << "Rewards id:" << iter->id << ",rewards num:" << iter->num << LogStream::eos;

		for (int i = 0; i < (int)mergeRewards.size(); ++i)
		{
			if (iter->id == mergeRewards[i].id)
			{
				bFind = true;

				if (data->type == ITEM_INCOME)
				{
					mergeRewards[i].num += iter->num;
					break;
				}

				if (data->maxNum == mergeRewards[i].num)
				{
					bFind = false;
					continue;
				}

				if (data->maxNum >= iter->num + mergeRewards[i].num)
				{
					mergeRewards[i].num += iter->num;
				}
				else
				{
					UInt8 restNum = (iter->num + mergeRewards[i].num) % data->maxNum;
					UInt8 grid = (iter->num + mergeRewards[i].num) / data->maxNum;
					mergeRewards[i].num = data->maxNum;
					ItemReward newReward = *iter;
					newReward.num = restNum;
					mergeRewards.push_back(newReward);
					while (grid > 1)
					{
						ItemReward newReward = *iter;
						newReward.num = data->maxNum;

						grid -= 1;
						mergeRewards.push_back(newReward);
					}

				}

				break;
			}
		}

		if (!bFind)
			mergeRewards.push_back(*iter);
	}

	player->ClearRewardGroup();

	for (std::vector<ItemReward>::const_iterator iter = mergeRewards.begin();
		iter != mergeRewards.end(); ++iter)
	{
		DebugLogStream << PLAYERINFO(player) << "MergeRewards id:" << iter->id << ",MergeRewards num:" << iter->num << LogStream::eos;
	}

	player->SendProtocol(ret);
}

void HandleProtocol(Player* player, CLProtocol::SceneShopQuery& protocol)
{
	CLProtocol::SceneShopQueryRet ret;
	ret.code = player->GetShopMgr().OnQueryItem(protocol.shopId, protocol.cache);
	player->SendProtocol(ret);
}

void HandleProtocol(Player* player, CLProtocol::SceneShopRefreshNumReq& protocol)
{
	CLProtocol::SceneShopRefreshNumRes res;
	res.shopId = protocol.shopId;
	player->GetShopMgr().OnQueryRefreshNum(protocol.shopId, res.restRefreshNum, res.maxRefreshNum);
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneShopBuy& protocol)
{
	CLProtocol::SceneShopBuyRet ret;
	ret.shopItemId = protocol.shopItemId;
	ret.code = player->GetShopMgr().OnBuy(protocol, ret.newNum, ret.leaseEndTimeStamp);
	if (ErrorCode::SHOP_WAIT_WORLD_RESPONSE != ret.code)
	{
		if (ErrorCode::SECURITY_LOCK_FORBID_OP != ret.code)
		{
			player->SendProtocol(ret);
		}
	}
	
}

void HandleProtocol(Player* player, CLProtocol::SceneUpdateNewItem& protocol)
{
	player->UpdateNewItem(protocol.packageType);
}

void HandleProtocol(Player* player, CLProtocol::SceneShopRefresh& protocol)
{
	CLProtocol::SceneShopRefreshRet ret;
	ret.code = player->GetShopMgr().OnRefreshShop(protocol.shopId);
	player->SendProtocol(ret);
}

void HandleProtocol(Player* player, CLProtocol::SceneDungeonUseItem& protocol)
{
	player->RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_USE_ITEM).c_str(), protocol.itemId, protocol.num);
}

void HandleProtocol(Player* player, CLProtocol::SceneOneKeyDecompose& protocol)
{
	
}

void HandleProtocol(Player* player, CLProtocol::SceneSealEquipReq& protocol)
{
	CLProtocol::SceneSealEquipRet ret;
	ret.code = player->SealEquip(protocol.uid);
	player->SendProtocol(ret);
}

void HandleProtocol(Player* player, CLProtocol::SceneCheckSealEquipReq& protocol)
{
	CLProtocol::SceneCheckSealEquipRet ret;
	ret.code = player->CheckSealEquip(protocol.uid, ret.needNum, ret.needMatId);
	player->SendProtocol(ret);
}

void HandleProtocol(Player* player, CLProtocol::SceneRandEquipQlvReq& protocol)
{
	CLProtocol::SceneRandEquipQlvRet ret;

	if (!SSServiceSwitchMgr::Instance()->IsOpen(EQUIP_SET_QUALITY_LV))
	{
		ret.code = ErrorCode::SYS_NOT_OPEN;
		player->SendProtocol(ret);
		return;
	}

	ret.code = player->ReMakeEquipQlv(protocol.uid, protocol.autoUsePont, protocol.usePerfect); 
	if (ret.code != ErrorCode::SECURITY_LOCK_FORBID_OP)
	{
		player->SendProtocol(ret);
	}
}

void HandleProtocol(Player* player, CLProtocol::SceneAddMagicReq& protocol)
{
	// 安全锁开启，不允许装备附魔
	if (!player->SecurityLockCheck())
		return;

	CLProtocol::SceneAddMagicRet ret;

	UInt32 code = 0;
	Item* card = player->FindItem(protocol.magicUid);
	Item* item = player->FindItem(protocol.itemUid);
	if (!card)
	{
		ErrorLogStream << "accid(" << player->GetAccID() << ") playerid(" << player->GetID() << ")card is null, uid=" << protocol.magicUid << LogStream::eos;
		player->SendNotify("card is null uid = {0}", protocol.magicUid);

		ret.code = ErrorCode::ITEM_DATA_INVALID;
		ret.itemUid = protocol.itemUid;
		ret.cardId = 0;

		player->SendProtocol(ret);
		return;
	}

	if (!item)
	{
		ErrorLogStream << "accid(" << player->GetAccID() << ") playerid(" << player->GetID() << ")item is null, uid=" << protocol.itemUid << LogStream::eos;
		player->SendNotify("item is null uid = {0}", protocol.itemUid);

		ret.code = ErrorCode::ITEM_DATA_INVALID;
		ret.itemUid = protocol.itemUid;
		ret.cardId = 0;

		player->SendProtocol(ret);
		return;
	}

	ret.cardId = card->GetDataEntry()->id;
	code = player->AddMagic(card, item);

	if (code != ErrorCode::SUCCESS)
	{
		InfoLogStream << "player(" << player->GetAccID() << "," << player->GetID() << "," << player->GetName() << ") AddMagic Carduid:"
			<< protocol.magicUid << "|itemUid:" << protocol.itemUid << "|code:" << code << LogStream::eos;
	}
	
	ret.code = code;
	ret.itemUid = protocol.itemUid;
	player->SendProtocol(ret);
}

void HandleProtocol(Player* player, CLProtocol::SceneMagicCardCompReq& protocol)
{
	CLProtocol::SceneMagicCardCompRet ret;
	UInt32 cardId = 0;
	ret.code = player->ComposeMagicCard(protocol.card_1, protocol.card_2, cardId);
	ret.cardId = cardId;
	player->SendProtocol(ret);
}

void HandleProtocol(Player* player, CLProtocol::SceneAddPreciousBeadReq& protocol)
{
	// 安全锁开启，不允许镶嵌宝珠
	if (!player->SecurityLockCheck())
		return;

	CLProtocol::SceneAddPreciousBeadRet ret;
	ret.preciousBeadId = 0;
	ret.itemUid = protocol.itemUid;

	if (!player)
	{
		ret.code = ErrorCode::ITEM_DATA_INVALID;
		player->SendProtocol(ret);

		ErrorLogStream << "Player ptr is null!" << LogStream::eos;
		return;
	}

	Item* preciousBead = player->FindItem(protocol.preciousBeadUid);
	if (!preciousBead)
	{
		ret.code = ErrorCode::ITEM_DATA_INVALID;
		player->SendProtocol(ret);

		ErrorLogStream << PLAYERINFO(player) << "Can not find item(guid: " << protocol.preciousBeadUid << ")." << LogStream::eos;
		return;
	}

	ret.preciousBeadId = preciousBead->GetDataID();

	Item* item = player->FindItem(protocol.itemUid);
	if (!item)
	{
		ret.code = ErrorCode::ITEM_DATA_INVALID;
		player->SendProtocol(ret);

		ErrorLogStream << PLAYERINFO(player) << "Can not find item(guid: " << protocol.itemUid << ")." << LogStream::eos;
		return;
	}

	//检查时间保护
	UInt64 addPreciousBeadTimeStamp = item->GetPreciosBeadAddedTimeStamp();
	if (addPreciousBeadTimeStamp > 0 && Avalon::Time::CurrentTime().Sec() - addPreciousBeadTimeStamp <= 60)
	{
		player->SendNotify(9870);
		return;
	}


	ret.code = player->AddPreciousBeadToItem(preciousBead, item);
	player->SendProtocol(ret);

	InfoLogStream << PLAYERINFO(player) << "Add precious bead(" << preciousBead->GetDataID() << ") to item(" << item->GetDataID() << "), code(" << ret.code << ")." << LogStream::eos;
}

void HandleProtocol(Player* player, CLProtocol::SceneUseMagicJarReq& protocol)
{
	player->OpenJar(protocol.id, protocol.combo, protocol.opActId);
}

void HandleProtocol(Player* player, CLProtocol::SceneQuickUseItemReq& protocol)
{
	CLProtocol::SceneQuickUseItemRet ret;
	ret.code = player->QuickUseItem(protocol.idx, protocol.dungenId);
	player->SendProtocol(ret);
}

void HandleProtocol(Player* player, CLProtocol::SceneFashionComposeReq& protocol)
{
	UInt32 code = ErrorCode::SUCCESS;
	if (!player->SecurityLockCheck())
		code = ErrorCode::SECURITY_LOCK_FORBID_OP;
	else
		code = player->GetItemMgr().FashionComposeNew(protocol.skySuitID, protocol.selComFashionID, protocol.fashionA, protocol.fashionB, protocol.composer);

	//UInt32 code = player->FashionCompose(protocol.fashionA, protocol.fashionB);

	if (code != ErrorCode::SUCCESS)
	{
		player->SendFashionComposeBack(code, 0, 0, 0, 0);
	}
}

void HandleProtocol(Player* player, CLProtocol::SceneFashionMergeRecordReq& protocol)
{
	if (player->GetCount(PLAYER_FASHION_MERGE_RECORD) == protocol.handleType) return;

	player->SetCounter(PLAYER_FASHION_MERGE_RECORD, protocol.handleType);
	player->SyncSelfData(true);
}

void HandleProtocol(Player* player, CLProtocol::SceneEquipMakeReq& protocol)
{
	CLProtocol::SceneEquipMakeRet ret;

	if (!player)
	{
		ret.code = ErrorCode::ITEM_DATA_INVALID;
		player->SendProtocol(ret);

		ErrorLogStream << "Player ptr is null!" << LogStream::eos;
		return;
	}

	ret.code = player->EuqipMake(protocol.equipId);
	player->SendProtocol(ret);

	InfoLogStream << PLAYERINFO(player) << "Compose equip(id: " << protocol.equipId << "), code(" << ret.code << ")." << LogStream::eos;
}

void HandleProtocol(CLProtocol::WSEnlargeStorageRet& ret)
{
	CLProtocol::SceneEnlargeStorageRet clientRet;

	Player* player = PlayerMgr::Instance()->FindNormalPlayer(ret.playerId);
	if (player)
	{
		if (ret.code == ErrorCode::SUCCESS)
		{
			// by huchenhui 已经在EnlargeStorage里扣过了.
			//player->RemoveItem(GetReason(SOURCE_TYPE_ENLARGE_PACK).c_str(), ret.costItemId, ret.cost);
			player->SetStorageSize(ret.newSize);
			player->SyncProperty();
			player->OnEnlargeStorage(player->GetEnlargeStoragePackSize());
		}

		clientRet.code = ret.code;
		player->SendProtocol(clientRet);
		InfoLogStream << PLAYERINFO(player) << "newSize =  " << ret.newSize << " Enlarge storage result : " << ret.code << LogStream::eos;
	}
	else
	{
		ErrorLogStream << "player(" << ret.playerId << ") is not player  result:" << ret.code << LogStream::eos;
	}
}

void HandleProtocol(Player* player, CLProtocol::SceneFashionAttrSelReq& protocol)
{
	CLProtocol::SceneFashionAttrSelRes res;

	if (!SSServiceSwitchMgr::Instance()->IsOpen(FASHION_SEL_ATTR))
	{
		res.result = ErrorCode::SYS_NOT_OPEN;
		player->SendProtocol(res);
		return;
	}

	Item* item = player->FindItem(protocol.uid);
	res.result = player->GetItemMgr().FashionAttrSel(item, protocol.attrid);
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneJarPointReq& protocol)
{
	CLProtocol::SceneJarPointRes res;

	player->GetJarMgr().ResetPoint(false);

	res.goldJarPoint = player->GetGoldJarPoint();
	res.magJarPoint = player->GetMagJarPoint();

	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::CSSetDungeonPotionReq& req)
{
	CLProtocol::SCSetDungeonPotionRes res;

	//设置药水检查是否合法
	if (req.potionId > 0)
	{
		ItemDataEntry* data = ItemDataEntryMgr::Instance()->FindEntry(req.potionId);
		if (!data)
		{
			res.code = ErrorCode::ITEM_DATA_INVALID;
			player->SendProtocol(res);
			return;
		}

		if (data->thirdType != TT_DUNGEON_POTION)
		{
			res.code = ErrorCode::ITEM_DATA_INVALID;
			player->SendProtocol(res);
			return;
		}
	}
	
	//是否是已经配置的
	std::vector<UInt32> pos = player->GetPotionPos();
	int repeatPos = -1;
	for (size_t i = 0; i < pos.size(); ++i)
	{
		if (pos[i] == req.potionId)
		{
			repeatPos = i;
			break;
		}
	}

	if (player->SetPotionPos(req.pos, req.potionId) &&
		(repeatPos > 0 ? (player->SetPotionPos(repeatPos, 0)) : true))
	{
		res.code = ErrorCode::SUCCESS;
		player->SyncProperty();
	}
	else
	{
		res.code = ErrorCode::ITEM_SET_POTION_ERROR;
	}

	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::CSRenewTimeItemReq& req)
{
	CLProtocol::SCRenewTimeItemRes res;

	res.code = player->GetItemMgr().RenewTimeItem(req.itemUid, req.durationDay);
	if (res.code != ErrorCode::SECURITY_LOCK_FORBID_OP)
	{
		player->SendProtocol(res);
	}
}

void HandleProtocol(Player* player, CLProtocol::CSOpenMagicBoxReq& req)
{
	CLProtocol::SCOpenMagicBoxRes res;

	player->ClearRewardGroup();
	std::vector<Item*> getItems;

	player->GetJarMgr().StartOpenJar();
	res.retCode = player->GetJarMgr().OpenMagicBox(req.uid, req.isBatch);

	if (res.retCode != ErrorCode::SUCCESS)
	{
		player->GetJarMgr().ClearHitItems();
		player->GetJarMgr().FinishOpenJar();
		player->SendProtocol(res);
		return;
	}

	player->GetJarMgr().GetOpenResult(getItems, res.rewards);
	Avalon::Packet* packet = Avalon::Packet::Create();
	res.Encode(packet);

	for (size_t i = 0; i < getItems.size(); ++i)
	{
		Item* item = getItems[i];
		if (!item)	continue;

		Avalon::NetOutputStream stream(packet->GetBuffer() + packet->GetSize(), packet->GetMaxSize() - packet->GetSize());

		try
		{
			stream & item->GetID() & item->GetDataID();
			item->Encode(stream, SST_SELF, false);
		}
		catch (const Avalon::NetException& exception)
		{
			ErrorLogStream << "player(" << player->GetID() << "," << player->GetName()
				<< ") encode items failed:" << exception.what() << "!" << LogStream::eos;
			Avalon::Packet::Destroy(packet);
			return;
		}


		if (i == getItems.size() - 1)
		{
			stream & (UInt64)0;
		}

		packet->SetSize(packet->GetSize() + UInt16(stream.GetPos()));
	}

	player->SendPacket(packet);
	Avalon::Packet::Destroy(packet);

	player->GetJarMgr().FinishOpenJar();
	player->GetJarMgr().ClearHitItems();
}

void HandleProtocol(Player* player, CLProtocol::SceneSellItemBatReq& req)
{
	UInt32 ret = ErrorCode::SUCCESS;

	/*for (size_t i = 0; i < req.itemUids.size(); ++i)
	{
		ret = player->SellItem(req.itemUids[i]);
		if (ret != ErrorCode::SUCCESS)
		{
			break;
		}
	}*/
	ret = player->SellItemBat(req.itemUids);

	CLProtocol::SceneSellItemBatRes res;
	res.code = ret;
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneDrawPrizeReq& req)
{
	if (!player) return;

	CLProtocol::SceneDrawPrizeRes res;
	res.drawPrizeId = req.id;
	res.rewardId = 0;
	res.retCode = ErrorCode::ITEM_DATA_INVALID;

	DrawPrizeDataEntry* drawPrizeData = DrawPrizeDataEntryMgr::Instance()->FindEntry(req.id);
	if (!drawPrizeData)
	{
		ErrorLogStream << PLAYERINFO(player) << "Can not find draw prize data by type(" << req.id << ")!" << LogStream::eos;
		player->SendProtocol(res);
		return;
	}

	if (drawPrizeData->activityId > 0)
	{
		if (drawPrizeData->activityId == AID_CHARGE_DAY_NEW)
		{
			res.retCode = player->DrawPrizeForRaffleTicket(drawPrizeData->dataId, res.rewardId);
		}
	}
	else if (drawPrizeData->opActivityType > 0)
	{
		if (drawPrizeData->opActivityType == OAT_HELL_TICKET_FOR_DRAW_PRIZE)
		{
			res.retCode = player->DrawPrizeForHellTicket(drawPrizeData->dataId, res.rewardId);
		}
	}
	else
	{
		ErrorLogStream << PLAYERINFO(player) << " draw prize id(" << req.id << ") is error!" << LogStream::eos;
	}

	player->SendProtocol(res);
}

//装备回收提交请求
void HandleProtocol(Player* player, CLProtocol::SceneEquipRecSubcmtReq& req)
{
	CLProtocol::SceneEquipRecSubcmtRes res;
	res.code = player->handleEqRecSubcmt(req.itemUids, res.scoreItems, res.score);
	player->SendProtocol(res);
}

//装备赎回请求
void HandleProtocol(Player* player, CLProtocol::SceneEquipRecRedeemReq& req)
{
	CLProtocol::SceneEquipRecRedeemRes res;
	UInt32 consScore = 0;
	res.code = player->handleEqRecRedeem(req.equid, consScore);
	res.consScore = consScore;
	player->SendProtocol(res);
}

//装备回收积分提升请求
void HandleProtocol(Player* player, CLProtocol::SceneEquipRecUpscoreReq& req)
{
	CLProtocol::SceneEquipRecUpscoreRes res;
	res.code = player->handleEqRecUpscore(req.equid, res.upscore);
	player->SendProtocol(res);
}
//装备赎回刷新时间戳请求
void HandleProtocol(Player* player, CLProtocol::SceneEquipRecRedeemTmReq& req)
{
	CLProtocol::SceneEquipRecRedeemTmRes res;
	res.timestmap = player->GetItemMgr().getEqRecUpdateTimeStamp();
	player->SendProtocol(res);
}

//装备转移
void HandleProtocol(Player* player, CLProtocol::SceneEquipTransferReq& req)
{
	CLProtocol::SceneEquipTransferRes ret;

	ret.code = player->TransferEquip(req.equid, req.transferId);
	player->SendProtocol(ret);
}

void HandleProtocol(Player* player, CLProtocol::SceneSyncResistMagicReq& req)
{
	if (!player) return;

	player->SetResistMagic(req.resist_magic);

	player->SyncResistMagicToWorld();
}

void HandleProtocol(CLProtocol::SceneAddRewardReq& req)
{
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(req.roleId);
	if (player == NULL) return;
	RewardGroup* group = player->GetRewardGroup();
	group->AddRewards(req.rewards);

	player->AddRewards(req.reason.c_str(), group, req.notice);

	//CLProtocol::SceneAddRewradRes res;

}

void HandleProtocol(Player* player, CLProtocol::SceneItemOpLockReq& req)
{
	UInt32 ret = player->HandlerItemOpLock(req.opType, req.itemUid);
	if (ret != ErrorCode::SUCCESS)
	{
		CLProtocol::SceneItemOpLockRes res;
		res.itemUid = req.itemUid;
		res.ret = ret;
		player->SendProtocol(res);
	}
}

void HandleProtocol(Player* player, CLProtocol::SceneSetFashionWeaponShowReq& req)
{
	CLProtocol::SceneSetFashionWeaponShowRes res;
	res.ret = player->SetShowFashionWeapon(req.isShow);
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneFashionChangeActiveMergeReq& req)
{
	UInt32 code = player->GetItemMgr().FashionComposeChangeActive(req.fashionId, req.ticketId, req.choiceComFashionId);
	if (code != ErrorCode::SUCCESS)
	{
		player->SendFashionCompBackChangeActive(code, 0, 0, 0, false);
	}
}

void HandleProtocol(Player* player, CLProtocol::SceneStrengthenTicketSynthesisReq& req)
{
	if (!player) return;

	UInt64 beginTime = TimeUtil::GetMicroSecondNow();
	CLProtocol::SceneStrengthenTicketSynthesisRes res;
	res.retCode = StrengthenTicketPlanMgr::Instance()->SynthesisStrengthenTicket(player, req.synthesisPlan);
	player->SendProtocol(res);
	UInt64 endTime = TimeUtil::GetMicroSecondNow();
	UInt64 useTime = endTime - beginTime;
	if (useTime >= 100000)
	{
		ErrorLogStream << PLAYERINFO(player) << " syhthesis strengthen ticket use time(" << useTime << ") with plan(" << req.synthesisPlan << LogStream::eos;
	}
}

void HandleProtocol(Player* player, CLProtocol::SceneStrengthenTicketFuseReq& req)
{
	if (!player) return;

	CLProtocol::SceneStrengthenTicketFuseRes res;
	res.retCode = StrengthenTicketPlanMgr::Instance()->FuseStrengthenTickets(player, req.pickTicketA, req.pickTicketB);
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneMountPreciousBeadReq& req)
{
	CLProtocol::SceneMountPreciousBeadRet ret;
	ret.itemUid = req.itemUid;
	ret.holeIndex = req.holeIndex;
	ret.code = player->GetItemMgr().MountPreciousBead(req.preciousBeadUid, req.itemUid, req.holeIndex, ret.preciousBeadId);
	player->SendProtocol(ret);
}

void HandleProtocol(Player* player, CLProtocol::SceneExtirpePreciousBeadReq& req)
{
	CLProtocol::SceneExtirpePreciousBeadRet ret;
	ret.code = player->GetItemMgr().ExtirpePreciousBead(req.itemUid, req.holeIndex, req.pestleId);
	player->SendProtocol(ret);
}

void HandleProtocol(Player* player, CLProtocol::SceneUpgradePreciousbeadReq& req)
{
	if (!player) return;

	CLProtocol::SceneUpgradePreciousbeadRes res;
	res.mountedType = req.mountedType;
	res.equipGuid = req.equipGuid;
	res.retCode = player->GetItemMgr().UpgradePreciousBead(req.mountedType, req.precGuid, req.equipGuid,
		req.eqPrecHoleIndex, req.precId, req.consumePrecId,
		res.precId, res.addBuffId, res.newPrebeadUid);
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneReplacePreciousBeadReq& req)
{
	if (!player) return;

	CLProtocol::SceneReplacePreciousBeadRet ret;
	ret.itemUid = req.itemUid;
	ret.holeIndex = req.holeIndex;
	ret.retCode = player->GetItemMgr().ReplacePreciousBead(req.preciousBeadUid, req.itemUid, req.holeIndex, ret.preciousBeadId);
	player->SendProtocol(ret);
}

void HandleProtocol(CLProtocol::WorldIncreasePlayerExpReq& protocol)
{
	Player* player = PlayerMgr::Instance()->FindPlayer(protocol.playerId);
	if (!player)
	{
		ErrorLogStream << "Can't find player(" << protocol.playerId << ") while increase player exp(" << protocol.incExp << ")!" << LogStream::eos;
		return;
	}

	player->IncExp(protocol.reason.c_str(), protocol.incExp);
}

void HandleProtocol(CLProtocol::SceneCheckItemReq& protcol)
{
	ItemMgr::CheckItem(protcol.serialId, protcol.type, protcol.owerId, protcol.itemId, protcol.param);
}

void HandleProtocol(Player* player, CLProtocol::SceneArtifactJarBuyCntReq& protocol)
{
	if (NULL == player) return;
	player->GetJarMgr().OpenActifactJarData();
}

void HandleProtocol(Player* player, CLProtocol::SceneEquieUpdateReq& protocol)
{
	CLProtocol::SceneEquieUpdateRes ret;
	ret.equipUid = protocol.equipUid;
	ret.code = player->GetItemMgr().EuqieUpdate(protocol.equipUid);
	player->SendProtocol(ret);
}

void HandleProtocol(CLProtocol::WorldCheckRechargePushReq& protocol)
{
	CLProtocol::WorldCheckRechargePushRes res;
	res.retCode = 1;
	res.roleId = protocol.roleId;
	res.pushId = protocol.pushId;

	InfoLogStream << "WorldCheckRechargePushReq, " << protocol.zoneId << "," << protocol.pushId << "," << protocol.lvLower << "," << protocol.lvToplimit << "," << protocol.vipLvLower << "," << protocol.parameter1 << "," << protocol.parameter2 << "," << LogStream::eos;

	UInt32 propCount = 0;
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(protocol.roleId);
	if (player && player->GetLevel() >= protocol.lvLower && player->GetLevel() <= protocol.lvToplimit && player->GetVipLvl() >= protocol.vipLvLower)
	{
		for (auto iter : protocol.judgePropIDs)
		{
			propCount += player->GetItemNum(iter);
			InfoLogStream << "WorldCheckRechargePushReq, " << propCount << "," << iter << LogStream::eos;
		}

		if (propCount >= protocol.parameter1 && propCount < protocol.parameter2)
		{
			res.retCode = ErrorCode::SUCCESS;
		}
	}

	protocol.SendBack(res);
}

void HandleProtocol(CLProtocol::WorldSceneBuyRechargePushItemReq& protocol)
{
	CLProtocol::WorldSceneBuyRechargePushItemRes res;
	res.retCode = ErrorCode::SUCCESS;
	res.roleId = protocol.roleId;
	res.pushId = protocol.pushId;

	Player* player = PlayerMgr::Instance()->FindNormalPlayer(protocol.roleId);
	if (!player)
	{
		res.retCode = ErrorCode::ITEM_DATA_INVALID;
		protocol.SendBack(res);
		return;
	}

	if (player->GetPoint() < protocol.price)
	{
		res.retCode = ErrorCode::ITEM_NOT_ENOUGH_MONEY;
		protocol.SendBack(res);
		return;
	}

	if (!player->CheckAddItem(protocol.item.id, protocol.item.num))
	{
		res.retCode = ErrorCode::ITEM_ADD_PACK_FULL;
		protocol.SendBack(res);
		return;
	}

	auto reason = GetReason(SOURCE_TYPE_RECHARGE_PUSH_BUY, protocol.pushId, protocol.item.id);
	
	auto num = player->AddItem(reason.c_str(), protocol.item.id, protocol.item.num, protocol.item.qualityLv, protocol.item.strenth);
	if (num != protocol.item.num)
	{
		res.retCode = ErrorCode::ITEM_DATA_INVALID;
		protocol.SendBack(res);
		return;
	}

	player->RemovePoint(reason.c_str(), protocol.price);

	protocol.SendBack(res);
}
