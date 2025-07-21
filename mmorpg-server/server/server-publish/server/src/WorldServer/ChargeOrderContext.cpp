#include "ChargeOrderContext.h"
#include <CLRecordClient.h>

ChargeOrderContext::ChargeOrderContext()
{
	status = ORDER_STATUS_INIT;
	connid = 0;
	accid = 0;
	roleId = 0;
	chargeGoodsId = 0;
	chargeItemId = 0;
	chargeItemNum = 0;
	vipScore = 0;
	chargeMoney = 0;
	time = 0;
	sendGoodTimeoutTime = 0;
}

std::string ChargeOrderContext::ToString() const
{
	std::stringstream ss;
	ss << "order(" << orderId << ") accid(" << accid << ") roleId(" << roleId << ") chargeMoney(" << chargeMoney << ") chargeItemId("
		<< chargeItemId << ")";
	return ss.str();
}

void ChargeOrderContext::InsertToDB()
{
	auto cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_charge_order", GetID());
	cmd->PutData("channel", channel);
	cmd->PutData("orderid", orderId);
	cmd->PutData("status", (UInt8)status);
	cmd->PutData("accid", accid);
	cmd->PutData("roleid", roleId);
	cmd->PutData("mall_type", mallType);
	cmd->PutData("charge_goods_id", chargeGoodsId);
	cmd->PutData("charge_item_id", chargeItemId);
	cmd->PutData("charge_item_num", chargeItemNum);
	cmd->PutData("vip_score", vipScore);
	cmd->PutData("charge_money", chargeMoney);
	cmd->PutData("time", time);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void ChargeOrderContext::SetDBData(CLRecordCallback* callback)
{
	if (!callback)
	{
		return;
	}

	SetID(callback->GetKey());
	channel = callback->GetData("channel").GetString();
	orderId = callback->GetData("orderid").GetString();
	status = (ChargeOrderStatus)(UInt8)callback->GetData("status");
	accid = callback->GetData("accid");
	roleId = callback->GetData("roleid");
	mallType = callback->GetData("mall_type");
	chargeGoodsId = callback->GetData("charge_goods_id");
	chargeItemId = callback->GetData("charge_item_id");
	chargeItemNum = callback->GetData("charge_item_num");
	vipScore = callback->GetData("vip_score");
	chargeMoney = callback->GetData("charge_money");
	time = callback->GetData("time");
}

void ChargeOrderContext::SaveToDB()
{
	auto cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_charge_order", GetID(), true);
	cmd->PutData("status", (UInt8)status);
	CLRecordClient::Instance()->SendCommand(cmd);
}