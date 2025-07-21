#include "SceneItemHandler.h"

#include <CLMailProtocol.h>
#include <CLBillingProtocol.h>
#include <CLSysNotify.h>
#include <CLDBLogger.h>
#include <CLErrorCode.h>
#include <CLDrawPrizeDataEntry.h>
#include <CLMallShopMultiIDataEntry.h>
#include <CLQuickBuyDataEntry.h>

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
#include "CurrencyFrozenMgr.h"
#include "PlayerLoadCallback.h"
#include "SSApplication.h"
#include "CreditPointRecordMgr.h"

void HandleProtocol(Player* player, CLProtocol::SceneMoveItem& protocol)
{
	if (protocol.targetpos.pack != PACK_ROLE_STORAGE)
	{
		return;
	}
	
	CLProtocol::SceneMoveItemRes res;
	res.code = player->PushRoleStorage(protocol.itemid, protocol.num, protocol.targetpos.gridIndex);
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneUseItem& protocol)
{
	//中断动作

	if (SERVER_TYPE == ST_WSCENE)
	{
		Router::SendToSelfScenePlayer(player->GetSrcSceneNodeID(), player->GetID(), protocol);
		return;
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

	CLProtocol::SceneCheckItemNumRet ret;
	ret.playerId = req.playerId;
	ret.serial = req.serial;
	ret.param = req.param;
	ret.result = ErrorCode::ITEM_TRANS_FAILED;

	do 
	{
		if (!player->CheckRmMoney(ST_ITEM_GOLD, req.silver) ||
			!player->CheckRmMoney(ST_BIND_GOLD, req.bindSilver) ||
			!player->CheckRmMoney(ST_ITEM_POINT, req.gold) ||
			!player->CheckRmMoney(ST_BIND_POINT, req.bindGold) ||
			!player->CheckRmMoney(ST_GUILD_CONTRI, req.guildContri))
		{
			ret.result = ErrorCode::ITEM_TRANS_NOT_ENOUGH_MONEY;
			goto endReturn;
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
					goto endReturn;
				}
			}
		}

		for (auto& itemIter : req.items)
		{
			UInt32 itemNum = 0;
			if (itemIter.second.isEqualItem)
				itemNum = player->GetItemNumWithEqualItem(itemIter.first);
			else
				itemNum = player->GetItemNum(itemIter.first);

			if (itemNum < itemIter.second.itemNum)
			{
				ret.result = ErrorCode::ITEM_TRANS_NOT_ENOUGH_ITEM;
				goto endReturn;
			}
		}

		for (std::map<UInt32, UInt32>::iterator iter = req.nobindItems.begin();
			iter != req.nobindItems.end(); ++iter)
		{
			if (player->GetNoBindItemNum(iter->first) < iter->second)
			{
				ret.result = ErrorCode::ITEM_TRANS_NOT_ENOUGH_ITEM;
				goto endReturn;
			}
		}

		if (!req.counterName.empty() && req.needCount > 0)
		{
			UInt32 count = player->GetCount(req.counterName.c_str());
			ret.counterTimes = count;
			if (count >= req.maxCount || req.maxCount - count < req.needCount)
			{
				ret.result = ErrorCode::ITEM_TRANS_NOT_ENOUGH_TIMES;
				goto endReturn;
			}
		}

		for (auto& detailItem : req.detailItems)
		{
			auto item = player->FindItem(detailItem.guid);
			if (!item || item->GetDataID() != detailItem.itemTypeId || item->GetNum() < detailItem.num)
			{
				ret.result = ErrorCode::ITEM_TRANS_NOT_ENOUGH_ITEM;
				goto endReturn;
			}
			if (req.isWeeklySetTask) //账号周常任务上交物品
			{
				if (item->GetEquipType() == EQUIP_RED)
				{
					ret.result = ErrorCode::TASK_SET_ITEM_TASK_TYPE_ERROR;
					goto endReturn;
				}
				if (item->GetDataEntry()->subType == ItemSubType::ST_EQUIP_WEAPON && player->IsWeaponExist(item->GetID()))
				{
					ret.result = ErrorCode::TASK_SET_ITEM_TASK_TYPE_ERROR;
					goto endReturn;
				}
				if (SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_EQUIP_SCHEME) && item->IsCanPushScheme() && player->GetEqSchemeMgr().IsEquipInScheme(item->GetID()))
				{
					ret.result = ErrorCode::ITEM_IILEG_OPER_IN_SCHEME;
					goto endReturn;
				}
			}
		}

		ret.result = ErrorCode::SUCCESS;

	} while (0);

endReturn:
	if (req.requestServerType == ST_BATTLE)
	{
		Router::SendToBattle(ret);
	}
	else if (req.requestServerType == ST_UNION)
	{
		player->SendToUnion(UnionServerFunctionType(req.param), ret);
	}
	else if (req.requestServerType == ST_TEAMCOPY)
	{
		Router::SendToTeamCopy(ret);
	}
	else
	{
		req.SendBack(ret);
	}
}

void HandleProtocol(CLProtocol::SceneRemoveItemReq& req)
{
	Player* player = PlayerMgr::Instance()->FindPlayer(req.playerId);
	if(player == NULL || player->GetGameStatus() == PS_INIT) return;


	if (req.gold > 0 && req.reason == GetReason(LogSourceType::SOURCE_TYPE_GUILD_CHANGE_NAME))
	{
		auto data = QuickBuyDataEntryMgr::Instance()->FindEntry(230000303);
		UInt32 rate = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_TICKET_CONVERT_MALL_INTEGRAL_RATE);
		UInt8 multiple = 0;
		UInt32 endTime = 0;
		MallShopMultiIDataEntryMgr::Instance()->GetMallItemMultiple(230000303, multiple, endTime);
		player->AddMallPoint(GetReason(LogSourceType::SOURCE_TYPE_GUILD_CHANGE_NAME).c_str(), req.gold * (data->multiple + multiple) * rate);
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

class PlayerLoadingForAddMoneyItem : public PlayerLoadingCallback
{
public:
	PlayerLoadingForAddMoneyItem(const std::string& _reason, UInt32 _type, UInt32 _num) : reason(_reason), type(_type), num(_num) {}
	virtual void OnFinish(Player* player)
	{
		InfoLogStream << "player(" << player->GetID() << ") loading for add money item finish." << LogStream::eos;

		player->AddMoneyItem(reason.c_str(), type, num);

		if (player->GetStatus() == PS_INIT)
		{
			player->SetGameStatus(PS_FINAL);
			player->SetRefTime(CURRENT_TIME.MSec());
		}
	}

	virtual void OnFailure(Player* player)
	{
		ErrorLogStream << "player(" << player->GetID() << "offline add money item failed! reason:" << reason
			<< " type:" << type << " num:" << num << LogStream::eos;

		if (player->GetGameStatus() == PS_INIT)
		{
			ErrorLogStream << PLAYERINFO(player) << "loading failed wait to destory..." << LogStream::eos;
			player->SetGameStatus(PS_DESTORY);
		}
	}

	virtual void OnTimeout(Player* player)
	{
		OnFailure(player);
	}

private:
	std::string reason;
	UInt32 type;
	UInt32 num;
};

void HandleProtocol(CLProtocol::SceneAddMoneyItemReq& protocol)
{
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if (player != NULL)
	{
		player->AddMoneyItem(protocol.reason.c_str(), protocol.type, protocol.num);
	}
	else
	{
		PlayerMgr::Instance()->LoadOfflinePlayerCb(protocol.playerId, 0, 
			new PlayerLoadingForAddMoneyItem(protocol.reason, protocol.type, protocol.num));
	}
}

class PlayerLoadingForCreditPoint : public PlayerLoadingCallback
{
public:
	PlayerLoadingForCreditPoint(UInt32 num, UInt64 orderId, const std::string& reason)
		: m_CreditPoint(num), m_OrderId(orderId), m_Reason(reason) {}
	virtual void OnFinish(Player* player)
	{
		if (NULL == player) return;

		InfoLogStream << PLAYERINFO(player) << " loading finish, add credit point:" << m_CreditPoint
			<< " reason:" << m_Reason << " orderId:" << m_OrderId << LogStream::eos;

		player->AddCreditPoint(m_Reason.c_str(), m_CreditPoint);
		CreditPointRecordMgr::Instance()->ObtainCreditPoint(player->GetAccID(), m_CreditPoint, m_OrderId);

		if (player->GetStatus() == PS_INIT)
		{
			player->SetGameStatus(PS_FINAL);
			player->SetRefTime(CURRENT_TIME.MSec());
		}
	}

	virtual void OnFailure(Player* player)
	{
		if (NULL == player) return;

		ErrorLogStream << PLAYERINFO(player) << "offline add credit point failed! reason:" << m_Reason
			<< " creditPoint:" << m_CreditPoint << " orderId:" << m_OrderId << LogStream::eos;

		if (player->GetGameStatus() == PS_INIT)
		{
			ErrorLogStream << PLAYERINFO(player) << "loading failed wait to destory..." << LogStream::eos;
			player->SetGameStatus(PS_DESTORY);
		}
	}

	virtual void OnTimeout(Player* player)
	{
		OnFailure(player);
	}

private:
	UInt32 m_CreditPoint;
	UInt64 m_OrderId;
	std::string m_Reason;
};

class OpActivityRecordCallback : public CLRecordCallback
{
public:
	OpActivityRecordCallback(UInt32 accid, UInt32 num) : m_AccId(accid), m_Num(num) {}
	void OnFinished()
	{
		while (NextRow())
		{
			OpActivityRecord* record = new OpActivityRecord();
			record->guid = GetKey();
			record->behavior = (OpActivityBehavior)(UInt32)GetData("behavior");
			record->ownerType = (OpActivityBehaviorOwnerType)(UInt32)GetData("owner_type");
			record->owner = GetData("owner");
			record->param1 = GetData("param1");
			record->num = GetData("num");
			record->recordTime = GetData("record_time");

			OpActivityRecordMgr::Instance()->LoadRecord(record);
		}

		OpActivityRecordMgr::Instance()->UpdateRecord(OP_ACTIVITY_BEHAVIOR_CREDIT_POINT_MONTH_GET, OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, m_AccId, 0, m_Num);
	}

	void OnFailed(UInt32 errorcode)
	{

	}

private:
	UInt32 m_AccId;
	UInt32 m_Num;
};

void HandleProtocol(CLProtocol::SceneGetCreditPoint& req)
{
	InfoLogStream << "onlineRole:" << req.onlineId << "player:" << req.roleId << " get credit point:" << req.creditPoint 
		<< " reason:" << req.reason << " orderId:"<< req.orderId << LogStream::eos;

	if (req.onlineId != 0)
	{
		Player* onlinePlayer = PlayerMgr::Instance()->FindLoadingFinishPlayer(req.onlineId);
		if (NULL == onlinePlayer)
		{
			ErrorLogStream << "error credit point add !" << LogStream::eos;
			PlayerMgr::Instance()->LoadOfflinePlayerCb(req.roleId, req.accId, new PlayerLoadingForCreditPoint(req.creditPoint, req.orderId, req.reason));
		}
		else
		{
			onlinePlayer->AddCreditPoint(req.reason.c_str(), req.creditPoint);
			CreditPointRecordMgr::Instance()->ObtainCreditPoint(onlinePlayer->GetAccID(), req.creditPoint, req.orderId);
		}
	}
	else
	{
		// 当没有角色在线时，在World上已经加上了信用点券， 但记录在此处更新
		CreditPointRecordMgr::Instance()->ObtainCreditPoint(req.accId, req.creditPoint, req.orderId);

		auto record = OpActivityRecordMgr::Instance()->GetRecord(OP_ACTIVITY_BEHAVIOR_CREDIT_POINT_MONTH_GET, OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, req.accId, 0);
		if (NULL == record)
		{
			CLSelectCommand	*cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_activity_op_record");
			cmd->PutCond("owner_type", (UInt32)OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT);
			cmd->PutCond("owner", req.accId);
			CLRecordClient::Instance()->SendCommand(cmd, new OpActivityRecordCallback(req.accId, req.creditPoint));
		}
		else
		{
			OpActivityRecordMgr::Instance()->UpdateRecord(OP_ACTIVITY_BEHAVIOR_CREDIT_POINT_MONTH_GET, OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, req.accId,
				0, req.creditPoint);
		}
	}
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
	//角色仓库不整理道具
	if (protocol.pack == PACK_ROLE_STORAGE)
	{
		CLProtocol::SceneTrimItemRet ret;
		ret.code = ErrorCode::SUCCESS;
		player->SendProtocol(ret);

		return;
	}

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
	// 唯一性检查
	std::set<UInt64> tmpSet;
	for (auto uid : protocol.uidList)
	{
		tmpSet.insert(uid);
	}

	if (tmpSet.size() != protocol.uidList.size())
	{
		return;
	}

	CLProtocol::SceneEquipDecomposeRet ret;
	ret.code = player->DecomposeEquip(GetReason(SOURCE_TYPE_EQUIP_DECOMPOSE).c_str(), protocol.uidList, ret.getItems);
	player->SendProtocol(ret);
}

void HandleProtocol(Player* player, CLProtocol::SceneEquipStrengthen& protocol)
{
	CLProtocol::SceneEquipStrengthenRet ret;
	player->ClearRewardGroup();

	ret.code = player->StrengthenEquip(protocol.equipUid, (protocol.useUnbreak == 1 || protocol.useUnbreak == 3), protocol.strTicket, (protocol.useUnbreak == 2 || protocol.useUnbreak == 3));

	InfoLogStream << PLAYERINFO(player) << "|" << protocol.equipUid << "|" << protocol.strTicket << "|" << protocol.useUnbreak << "|" << ret.code << LogStream::eos;

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



void HandleProtocol(Player* player, CLProtocol::SceneEquipEnhanceUpgrade& protocol)
{
	CLProtocol::SceneEquipEnhanceUpgradeRet ret;
	player->ClearRewardGroup();

	ret.code = player->EnhanceEquip(protocol.equipUid, (protocol.useUnbreak == 1 || protocol.useUnbreak == 3), protocol.strTicket, (protocol.useUnbreak == 2 || protocol.useUnbreak == 3));

	InfoLogStream << PLAYERINFO(player) << "|" << protocol.equipUid << "|" << protocol.strTicket << "|" << protocol.useUnbreak << "|" << ret.code << LogStream::eos;

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

void HandleProtocol(Player* player, CLProtocol::SceneEquipEnhanceClear& protocol)
{
	CLProtocol::SceneEquipEnhanceClearRet ret;
	ret.code = player->ClearEquipSmell(protocol.equipUid, protocol.stuffID);
	player->SendProtocol(ret);
}

void HandleProtocol(Player* player, CLProtocol::SceneEquipEnhanceRed& protocol)
{
	CLProtocol::SceneEquipEnhanceRedRet ret;
	ret.code = player->RedEquip(protocol.equipUid, protocol.stuffID);
	player->SendProtocol(ret);
}

void HandleProtocol(Player* player, CLProtocol::SceneEquipEnhanceChg& protocol)
{
	CLProtocol::SceneEquipEnhanceChgRet ret;
	ret.code = player->EquipEnhanceTypeChg(protocol.equipUid, static_cast<EnhanceType>(protocol.enhanceType), protocol.stuffID);
	player->SendProtocol(ret);
}

void HandleProtocol(Player* player, CLProtocol::SceneEnhanceMaterialCombo& protocol)
{
	CLProtocol::SceneEnhanceMaterialComboRet ret;
	ret.code = player->ComboEnhanceMaterial(protocol.goalId, protocol.goalNum);
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
	ret.code = player->SealEquip(protocol.uid, ret.inscriptionIds);
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
	ret.cardLev = item->GetMagicCardLv();
	player->SendProtocol(ret);
}

void HandleProtocol(Player* player, CLProtocol::SceneMagicCardCompReq& protocol)
{
	CLProtocol::SceneMagicCardCompRet ret;
	UInt32 cardId = 0;
	ret.code = player->ComposeMagicCard(protocol.card_1, protocol.card_2, cardId, ret.cardLev);
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
	player->OpenJar(protocol.id, protocol.combo, protocol.opActId, protocol.param);
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

		auto vec = player->GetItemMgr().GetFitPotionType(req.pos);
		if (std::find(vec.begin(), vec.end(), data->subType) == vec.end())
		{
			res.code = ErrorCode::ITEM_SET_POTION_ERROR;
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

	if (SSApplication::Instance()->OpenFastSellBat())
	{
		ret = player->SellItemBatFast(req.itemUids);
	}
	else
	{
		ret = player->SellItemBat(req.itemUids);
	}

	

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
		else if (drawPrizeData->opActivityType == OAT_HALLOWEEN_PUMPKIN_HELMET)
		{
			res.retCode = player->DrawPrizeForHalloweenPumpkin(drawPrizeData->dataId, res.rewardId);
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
	if (player == NULL)
	{
		InfoLogStream << "World req add item but player not online,roleid is: " << req.roleId << " reason is: " << req.reason << LogStream::eos;
		return;
	}
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

void HandleProtocol(Player* player, CLProtocol::SceneBeadConvertReq& protocol)
{
	player->GetItemMgr().BeadConvert(protocol.beadGuid, protocol.materialGuid);
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

void HandleProtocol(CLProtocol::SceneCheckItemReq& protocol)
{
	ItemMgr::CheckItem(protocol.serialId, protocol.type, protocol.owerId, protocol.itemId, protocol.param);
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

void HandleProtocol(Player* player, CLProtocol::SceneItemDepositReq& req)
{
	player->GetItemDepositMgr().HandleDepositItem();
}

void HandleProtocol(Player* player, CLProtocol::SceneItemDepositGetReq& req)
{
	player->GetItemDepositMgr().HandleGetDepositItem(req.guid);
}

void HandleProtocol(CLProtocol::SceneItemDepositSync& protocol)
{
	auto player = PlayerMgr::Instance()->FindPlayer(protocol.roleId);
	if (!player)
	{
		for (auto& iter : protocol.rewardVec)
		{
			std::ostringstream os;
			iter.EncodeString(os);
			ErrorLogStream << "Not Find Player Id:" << protocol.roleId << " reward:" << os.str() << LogStream::eos;
		}

		return;
	}

	for (auto& iter : protocol.rewardVec)
	{
		player->GetItemDepositMgr().AddItemDeposit(iter, protocol.dungeonId);
	}
}

void HandleProtocol(CLProtocol::WorldCheckRechargePushReq& protocol)
{
	CLProtocol::WorldCheckRechargePushRes res;
	res.retCode = 1;
	res.roleId = protocol.roleId;
	res.pushId = protocol.pushId;

	//DebugLogStream << "WorldCheckRechargePushReq, " << protocol.zoneId << "," << protocol.pushId << "," << protocol.lvLower << "," << protocol.lvToplimit << "," << protocol.vipLvLower << "," << protocol.parameter1 << "," << protocol.parameter2 << "," << LogStream::eos;

	UInt32 propCount = 0;
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(protocol.roleId);
	if (player && player->GetLevel() >= protocol.lvLower && player->GetLevel() <= protocol.lvToplimit && player->GetVipLvl() >= protocol.vipLvLower)
	{
		for (auto iter : protocol.judgePropIDs)
		{
			propCount += player->GetItemNum(iter);
			//DebugLogStream << "WorldCheckRechargePushReq, " << propCount << "," << iter << LogStream::eos;
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

void HandleProtocol(Player* player, CLProtocol::SceneMagicCardUpgradeReq& protocol)
{
	CLProtocol::SceneMagicCardUpgradeRes res;
	res.cardTypeId = protocol.upgradeUid;
	res.cardLev = 0;
	res.equipUid = 0;
	res.cardGuid = 0;
	if (protocol.upgradeUid && protocol.materialUid)
	{
		res.code = player->GetItemMgr().UpgradeMagicCard(protocol.upgradeUid, protocol.materialUid, res.cardTypeId, res.cardLev, res.cardGuid);
	}
	else if (protocol.equipUid && protocol.cardId && protocol.materialUid)
	{
		res.equipUid = protocol.equipUid;
		res.code = player->GetItemMgr().UpgradeMountMagicCard(protocol.equipUid, protocol.cardId, protocol.materialUid, res.cardTypeId, res.cardLev);
	}
	else{
		res.code = ErrorCode::ITEM_DATA_INVALID;
	}
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneMagicCardCompOneKeyReq& protocol)
{
	CLProtocol::SceneMagicCardCompOneKeyRes res;
	res.code = player->GetItemMgr().ComposeMagicCardOneKey(protocol.isCompWhite, protocol.isCompBlue, protocol.autoUseGold, protocol.compNotBind,
		res.endReason, res.compTimes, res.consumeBindGolds, res.comsumeGolds, res.items);
	player->SendProtocol(res);
}

void HandleProtocol(CLProtocol::WorldAccountShopItemBuyedNotify& protocol)
{
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if (!player)
	{
		return;
	}
	player->OnShopBuy(protocol.shopId, protocol.shopItemId, protocol.shopItemNum, 0, 0);
}

void HandleProtocol(CLProtocol::SceneCurrencyFrozenApplyReq& protocol)
{
	CLProtocol::SceneCurrencyFrozenApplyRes res;
	res.guid = 0;
	res.roleId = protocol.roleId;

	do
	{
		// 冻结前，先从玩家身上扣除
		Player* player = PlayerMgr::Instance()->FindNormalPlayer(protocol.roleId);
		if (NULL == player)
		{
			res.retCode = ErrorCode::ITEM_DATA_INVALID;
			break;
		}

		if (player->GetItemNum(protocol.itemId) < protocol.num)
		{
			res.retCode = ErrorCode::ITEM_FROZEN_CURRENCY_NOT_ENOUGH;
			break;
		}

		player->RemoveItem(protocol.reason.c_str(), protocol.itemId, protocol.num);

		res.guid = CurrencyFrozenMgr::Instance()->ApplyFrozen(protocol.roleId, protocol.itemId, protocol.num, protocol.reason);
		res.retCode = ErrorCode::SUCCESS;

	} while (0);

	protocol.SendBack(res);
}

void HandleProtocol(CLProtocol::SceneCurrencyFrozenOpReq& protocol)
{
	CLProtocol::SceneCurrencyFrozenOpRes res;
	if (protocol.opType != 0)
	{
		res.retCode = CurrencyFrozenMgr::Instance()->DeductFrozen(protocol.guid);
	}
	else
	{
		res.retCode = CurrencyFrozenMgr::Instance()->RemoveFrozen(protocol.guid);
	}

	protocol.SendBack(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneEquipInscriptionOpenReq& protocol)
{
	CLProtocol::SceneEquipInscriptionOpenRes res;
	res.guid = protocol.guid;
	res.index = protocol.index;
	res.retCode = player->EquipInscriptionOpen(protocol.guid, protocol.index);
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneEquipInscriptionMountReq& protocol)
{
	CLProtocol::SceneEquipInscriptionMountRes res;
	res.guid = protocol.guid;
	res.index = protocol.index;
	res.inscriptionGuid = protocol.inscriptionGuid;
	res.inscriptionId = protocol.inscriptionId;
	res.retCode = player->EquipInscriptionMount(protocol.guid, protocol.index, protocol.inscriptionGuid);
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneEquipInscriptionExtractReq& protocol)
{
	CLProtocol::SceneEquipInscriptionExtractRes res;
	res.guid = protocol.guid;
	res.index = protocol.index;
	res.inscriptionId = protocol.inscriptionId;
	res.retCode = player->EquipInscriptionExtract(protocol.guid, protocol.index);
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneEquipInscriptionSynthesisReq& protocol)
{
	CLProtocol::SceneEquipInscriptionSynthesisRes res;
	res.retCode = player->EquipInscriptionSynthesis(protocol.itemIDVec,res.items);
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneEquipInscriptionDestroyReq& protocol)
{
	CLProtocol::SceneEquipInscriptionDestroyRes res;
	res.guid = protocol.guid;
	res.index = protocol.index;
	res.inscriptionId = protocol.inscriptionId;
	res.retCode = player->EquipInscriptionDestroy(protocol.guid, protocol.index);
	player->SendProtocol(res);
}

void HandleProtocol(Player * player, CLProtocol::SceneMallFashionLimitedSuitStatusReq & req)
{
	CLProtocol::SceneMallFashionLimitedSuitStatusRes res;
	std::vector<UInt32> tempFlagVec;
	tempFlagVec = player->GetSuitFlag();

	//ST_EQUIP_HEAD = 2			//头部
	//ST_EQUIP_CHEST = 3			//胸
	//ST_EQUIP_BELT = 4			//腰带
	//ST_EQUIP_LEG = 5		//护腿
	//ST_EQUIP_BOOT = 6			//鞋子
	std::vector<UInt32> itemIDVec = MallFashionLimitedSuitDataEntryMgr::Instance()->GetAllSuitItemID(player->GetOccu());
	for (auto &i : tempFlagVec)
	{
		for (UInt32 j = ST_EQUIP_HEAD; j <= ST_EQUIP_BOOT; ++j)
		{
			if ((1 << j) & i)
			{
				for (auto &k : itemIDVec)
				{
					auto data = ItemDataEntryMgr::Instance()->FindEntry(k);
					if (data == nullptr)
					{
						continue;
					}
					else
					{
						if (data->subType == j)
						{
							res.result.emplace_back(k);
							break;
						}
					}
				}
			}
		}
	}
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneEquipSchemeWearReq& protocol)
{
	CLProtocol::SceneEquipSchemeWearRes res;
	res.type = protocol.type;
	res.id = protocol.id;
	res.isSync = protocol.isSync;

	res.code = player->GetEqSchemeMgr().OnPlayerWearReq(protocol.type, protocol.id, protocol.isSync, res.overdueIds);
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneEquipConvertReq& protocol)
{
	CLProtocol::SceneEquipConvertRes res;
	res.retCode = player->GetItemMgr().HandleEquipConvert(protocol.type, protocol.sourceEqGuid, protocol.targetEqTypeId, protocol.convertorGuid, res.eqGuid);
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneActivePlantReq& protocol)
{
	CLProtocol::SceneActivePlantRes res;
	res.retCode = player->HandlePlantReq();
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneActivePlantAppraReq& protocol)
{
	CLProtocol::SceneActivePlantAppraRes res;
	res.retCode = player->HandlePlantAppraReq();
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneUnlockRoleStorageReq& protocol)
{
	CLProtocol::SceneUnlockRoleStorageRes res;
	res.retCode = player->UnlockRoleStorage();
	res.curOpenNum = player->GetCounter(COUNTER_ROLE_STORAGE_OPEN_NUM);
	player->SendProtocol(res);
}

void HandleProtocol(Player * player, CLProtocol::SceneWeaponBuildReq& protocol)
{
	CLProtocol::SceneWeaponBuildRes res;
	res.errorCode = ErrorCode::ITEM_DATA_INVALID;
	res.guid = 0;

	// 检查背包格子
	Package* pkg = player->GetItemMgr().GetPackage(PACK_EQUIP);
	if (!pkg)
	{
		player->SendProtocol(res);
		return;
	}
	if (pkg->GetCapacity() < 1)
	{
		res.errorCode = ErrorCode::ITEM_PACKSIZE_MAX;
		player->SendProtocol(res);
		return;
	}

	auto weapon = player->GetItemMgr().BuildWeapon(protocol.id, protocol.extraMaterial1 != 0 ? true : false, protocol.extraMaterial2 != 0 ? true : false);
	if (weapon != nullptr)
	{
		if (player->AddItemDirect(GetReason(SOURCE_WEAPON_BUILD).c_str(), weapon, false) == 1)
		{
			weapon->CalculateValuedScore(player);
			res.guid = weapon->GetID();
			res.errorCode = ErrorCode::SUCCESS;
		}
		else
		{
			ErrorLogStream << "Weapon Build Add Failed " << PLAYERINFO(player) << LogStream::eos;
		}
	}
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneCreditPointRecordReq& protocol)
{
	if (NULL == player) return;
	CreditPointRecordMgr::Instance()->HandlerCreditPointRecordReq(player);
}

void HandleProtocol(Player * player, CLProtocol::SceneSecretCoinReq& protocol)
{
	if (player == nullptr)
	{
		return;
	}
	player->SendSecretCoin();
}
