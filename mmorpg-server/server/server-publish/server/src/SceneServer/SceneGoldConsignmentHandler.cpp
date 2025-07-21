#include "SceneGoldConsignmentHandler.h"

#include <CLOfflineEventDefine.h>
#include <CLErrorCode.h>
#include <CLVipDataEntry.h>
#include <CLPlayerDataEntry.h>
#include <CLUnionServerProtocol.h>
#include <CLSyncObjectProtocol.h>
#include <CLGoldConsignmentValueDataEntry.h>
#include "ActiveRecordMgr.h"
#include "PlayerMgr.h"
#include "SSRouter.h"

float GetMinPrice(UInt32 unitPrice)
{
	if (unitPrice == 0) return 0;

	UInt32 unitPriceBase = GoldConsignmentValueDataEntryMgr::Instance()->GetValue(GC_VALUE_TRADE_UNIT_BASE);
	UInt32 minTradeNum = GoldConsignmentValueDataEntryMgr::Instance()->GetValue(GC_VALUE_MIN_GOLD_NUM);

	if (unitPriceBase == 0 || minTradeNum == 0 || minTradeNum > unitPriceBase)
		return 0;

	return float(unitPrice) / (unitPriceBase / minTradeNum);
}

void HandleProtocol(Player* player, CLProtocol::SceneGoldConsignmentSubmitOrderReq& req)
{
	if (NULL == player) return;

	if (req.orderType == GCOT_BUY)
	{
		// 买金需要安全锁检查
		if (!player->SecurityLockCheck())
		{
			return;
		}

		player->SendToUnion(USFT_GOLD_CONSIGNMENT, req);
		return;
	}

	CLProtocol::SceneGoldConsignmentSubmitOrderRes res;
	res.submitId = req.submitId;
	res.orderType = req.orderType;
	res.retCode = ErrorCode::SUCCESS;

	do 
	{
		float minUnitPrice = GetMinPrice(req.unitPrice);

		// 计算手续费
		UInt32 commissionCfg = GoldConsignmentValueDataEntryMgr::Instance()->GetValue(GC_VALUE_COMMISSIOM);
		UInt32 commission = (UInt32)ceil(minUnitPrice * req.tradeNum * commissionCfg / GOLD_CONSIGNMENT_RATE);

		// 预期收益
		UInt32 estimateProfit = (UInt32)minUnitPrice * req.tradeNum - commission;

		// 检查月获得上限
		UInt32 monthGetLimit = PlayerDataEntryMgr::Instance()->GetCreditPointMonthGetLimit(player->GetRoleMaxLevel());
		UInt32 vipMonthGetLimit = VipPrivilegeDataEntryMgr::Instance()->FindPrivilegeValue(VIP_PRIVILEGE_CREDIT_POINT_MONTH_GET_LIMIT, player->GetVipLvl());
		if (vipMonthGetLimit > monthGetLimit)
		{
			monthGetLimit = vipMonthGetLimit;
		}

		UInt32 monthGet = OpActivityRecordMgr::Instance()->GetRecordValue(OP_ACTIVITY_BEHAVIOR_CREDIT_POINT_MONTH_GET, OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, player->GetAccID(), 0);
		if (monthGet + estimateProfit > monthGetLimit)
		{
			res.retCode = ErrorCode::GOLD_CONSIGNMENT_OVER_ACC_MONTH_GET_LIMIT;
			break;
		}

		// 检查拥有上限
		UInt32 holdLimit = PlayerDataEntryMgr::Instance()->GetCreditPointHoldLimit(player->GetRoleMaxLevel());
		UInt32 vipHoldLimit = VipPrivilegeDataEntryMgr::Instance()->FindPrivilegeValue(VIP_PRIVILEGE_CREDIT_POINT_HOLD_LIMIT, player->GetVipLvl());
		if (vipHoldLimit > holdLimit)
		{
			holdLimit = vipHoldLimit;
		}

		if (player->GetCreditPoint() + estimateProfit > holdLimit)
		{
			res.retCode = ErrorCode::GOLD_CONSIGNMENT_OVER_ACC_HOLD_LIMIT;
			break;
		}

		// 上限赋值
		req.param = holdLimit;

		player->SendToUnion(USFT_GOLD_CONSIGNMENT, req);
		return;

	} while (0);

	player->SendProtocol(res);
}

void HandleProtocol(CLProtocol::SceneGoldConsignmentDealNotify& req)
{
	Player* player = PlayerMgr::Instance()->FindPlayer(req.roleId);
	if (player != NULL)
	{
		player->SendProtocol(req);
	}
	else
	{
		CLProtocol::SceneSyncOfflineEvent event;
		event.playerId = req.roleId;
		event.type = OFFLINE_GOLD_CONSIGNMENT_POINT;
		event.param1 = req.redPointTime;
		Router::SendToWorld(event);
	}
}

void HandleProtocol(Player* player, CLProtocol::UnionPlayerGetItem& req)
{
	std::ostringstream logStr;
	for (auto& it : req.rewards)
	{
		logStr << " itemId:" << it.id << " itemNum:" << it.num;
	}

	InfoLogStream << "union server playerId:" << req.roleId << " reason:" << req.reason
		<< " reward:" << logStr.str() << LogStream::eos;

	if (NULL == player)
	{
		ErrorLogStream << "union server not find playerId:" << req.roleId << LogStream::eos;
		return;
	}

	RewardGroup* group = player->GetRewardGroup();
	group->AddRewards(req.rewards);
	player->AddRewards(req.reason.c_str(), group, req.notice);
}
