#include "ChargeManager.h"
#include <json/writer.h>
#include <AvalonMd5.h>
#include <AvalonTimeUtility.h>
#include <CLRecordClient.h>
#include <CLBillingProtocol.h>
#include <CLChargeMallDataEntry.h>
#include <CLChargePacketDataEntry.h>
#include <CLMoneyManageDefine.h>

#include "WSApplication.h"
#include "WSPlayerMgr.h"
#include "AccountChargeRecord.h"
#include "WSConsoleAgent.h"
#include "AccountMgr.h"
#include "WSSceneMgr.h"
#include "WSRouter.h"
#include "WSNetwork.h"
#include "WorldSysNotify.h"
#include "MallMgr.h"
#include "AccountActivityTaskMgr.h"
#include "AdventurePassMgr.h"

class ReChargeQueryCallback : public CLRecordCallback
{
public:
	ReChargeQueryCallback(UInt32 connid, const std::string& orderId) : m_Connid(connid), m_OrderId(orderId) {}

	virtual void OnFinished()
	{
		if (NextRow())
		{
			// 检测订单是不是已经在处理中了
			auto context = ChargeManager::Instance()->_FindContext(m_OrderId);
			if (!context)
			{
				ErrorLogStream << "can't find order(" << m_OrderId << ")." << LogStream::eos;
				ChargeManager::Instance()->SendChargeResult(m_Connid, CHARGE_OTHER);
				return;
			}

			context->SetDBData(this);

			// 订单已经完成了
			if (context->status == ORDER_STATUS_FINISH)
			{
				ErrorLogStream << "order(" << context->orderId << ") player(" << context->roleId << ") order is finished." << LogStream::eos;
				ChargeManager::Instance()->SendChargeResult(context->connid, CHARGE_REPEAT_ORDER);
				delete context;
				return;
			}

			if (context->mallType == CHARGE_MALL_PACKET)
			{
				//auto chargePacketData = ChargeManager::Instance()->FindChargePacket(context->chargeGoodsId);
				auto chargePacketData = MallMgr::Instance()->GetMallItemInfo(context->chargeGoodsId);
				if (!chargePacketData)
				{
					ErrorLogStream << "order(" << context->orderId << ") player(" << context->roleId << ") invalid charge packet(" << context->chargeGoodsId << ")." << LogStream::eos;
					ChargeManager::Instance()->SendChargeResult(context->connid, CHARGE_OTHER);
					delete context;
					return;
				}

				context->rewards = chargePacketData->rewards;
				
			}

			ChargeManager::Instance()->StartSendGoods(context);
		}
		else
		{
			// 没有订单
			ErrorLogStream << "order(" << m_OrderId << ") don't exist." << LogStream::eos;
			ChargeManager::Instance()->SendChargeResult(m_Connid, CHARGE_OTHER);
		}
	}

	virtual void OnFailed(UInt32 errorcode)
	{
		ErrorLogStream << "order(" << m_OrderId << ") db error(" << errorcode << ")." << LogStream::eos;
		ChargeManager::Instance()->SendChargeResult(m_Connid, CHARGE_DB_ERROR);
	}
private:
	UInt32			m_Connid;
	std::string		m_OrderId;
};

class QueryChargeOrderCallback : public CLRecordCallback
{
public:
	QueryChargeOrderCallback(ChargeOrderContext* context) : m_Context(context) { if (m_Context) m_OrderId = m_Context->orderId; }

	virtual void OnFinished()
	{
		if (!m_Context)
		{
			ErrorLogStream << "order(" << m_OrderId << ") context is null" << LogStream::eos;
			return;
		}

		DebugLogStream << "query charge order(" << m_Context->ToString() << ") return..." << LogStream::eos;

		if (NextRow())
		{
			// 订单已经创建了
			UInt8 status = GetData("status");
			UInt32 accid = GetData("accid");
			ObjID_t roleId = GetData("roleid");
			UInt32 chargeGoodsId = GetData("charge_goods_id");
			UInt32 chargeMoney = GetData("charge_money");
			UInt8 mallType = GetData("mall_type");
			
			// 验证数据库中的信息和订单信息是否一样
			if (accid != m_Context->accid || roleId != m_Context->roleId || chargeGoodsId != m_Context->chargeGoodsId || chargeMoney != m_Context->chargeMoney || mallType != m_Context->mallType)
			{
				ErrorLogStream << "charge(" << m_Context->ToString() << ") is different from db." << LogStream::eos;
				ChargeManager::Instance()->SendChargeResult(m_Context->connid, CHARGE_INVALID_PARAM);
				ChargeManager::Instance()->_DelContext(m_Context->orderId);
				return;
			}

			m_Context->SetID(GetKey());
			ChargeManager::Instance()->VerifyOrderRet(m_Context, (ChargeOrderStatus)status);
		}
		else
		{
			// 订单还未创建
			ChargeManager::Instance()->VerifyOrderRet(m_Context, ORDER_STATUS_NOT_CREATE);
		}
	}

	virtual void OnFailed(UInt32 errorcode)
	{
		if (m_Context)
		{
			ErrorLogStream << "charge(" << m_Context->ToString() << ") db error(" << errorcode << ")." << LogStream::eos;
			ChargeManager::Instance()->SendChargeResult(m_Context->connid, CHARGE_DB_ERROR);
			ChargeManager::Instance()->_DelContext(m_Context->orderId);
		}
	}
private:
	std::string				m_OrderId;
	ChargeOrderContext*		m_Context;
};

class QueryAccountCallback : public AccountCallback
{
public:
	QueryAccountCallback(ChargeOrderContext* context) : AccountCallback(context->accid), m_Context(context) {}

	virtual void OnFinish(AccountInfo* accountInfo)
	{
		DebugLogStream << "order(" << m_Context->ToString() << ") query account info return..." << LogStream::eos;

		auto goodsData = ChargeMallDataEntryMgr::Instance()->FindEntry(m_Context->chargeGoodsId);
		if (!goodsData)
		{
			ErrorLogStream << "charge(" << m_Context->ToString() << ") goods unexist." << LogStream::eos;

			// 货物找不到
			ChargeManager::Instance()->SendChargeResult(m_Context->connid, CHARGE_OTHER);
			ChargeManager::Instance()->_DelContext(m_Context->orderId);
			return;
		}

		auto mallType = (ChargeMallType)(UInt8)m_Context->mallType;
		// 判断限购
		if (goodsData->dailiyLimitNum > 0)
		{
			if (accountInfo->GetTodayChargeTimes(mallType, m_Context->chargeGoodsId, 0) >= goodsData->dailiyLimitNum)
			{
				ErrorLogStream << "charge(" << m_Context->ToString() << ") goods daily times limit." << LogStream::eos;
				ChargeManager::Instance()->SendChargeResult(m_Context->connid, CHARGE_OTHER);
				ChargeManager::Instance()->_DelContext(m_Context);
				return;
			}
		}

		
		if (accountInfo->GetChargeTimes(mallType, m_Context->chargeGoodsId, 0) == 0)
		{
			m_Context->chargeItemNum += goodsData->firstChargeAddNum;
		}
		else
		{
			m_Context->chargeItemNum += goodsData->unfirstChargeAddNum;
		}

		accountInfo->IncChargeTimes(m_Context->accid, m_Context->roleId, mallType, m_Context->chargeGoodsId, 1);

		// 验证玩家返回
		ChargeManager::Instance()->CreateOrderRecord(m_Context);
		// 开始发货
		ChargeManager::Instance()->StartSendGoods(m_Context);
	}

	virtual void OnFailure()
	{
		ErrorLogStream << "charge(" << m_Context->ToString() << ") db load account info failed." << LogStream::eos;
		ChargeManager::Instance()->SendChargeResult(m_Context->connid, CHARGE_DB_ERROR);
		ChargeManager::Instance()->_DelContext(m_Context->orderId);
	}

private:
	ChargeOrderContext*		m_Context;
};

class QueryChargePlayerCallback : public CLRecordCallback
{
public:
	QueryChargePlayerCallback(ChargeOrderContext* context) : m_Context(context) {}
	virtual void OnFinished()
	{
		DebugLogStream << "order(" << m_Context->ToString() << ") query player return..." << LogStream::eos;

		if (NextRow())
		{
			UInt32 deleteTime = GetData("deletetime");
			UInt32 accid = GetData("accid");
			if (deleteTime > 0)
			{
				ErrorLogStream << "charge(" << m_Context->ToString() << ") role is deleted." << LogStream::eos;

				// 号已经被删掉了
				ChargeManager::Instance()->SendChargeResult(m_Context->connid, CHARGE_PLAYER_UNEXIST);
				ChargeManager::Instance()->_DelContext(m_Context->orderId);
			}
			else
			{
				// 检测角色ID和账号ID是否一致
				if (WSApplication::Instance()->IsCheckChargeAccount() && m_Context->accid != accid)
				{
					ErrorLogStream << "charge(" << m_Context->ToString() << ") player(" << m_Context->roleId << ", " << accid << ") invalid account(" << m_Context->accid << ")." << LogStream::eos;

					// 账号信息不一致
					ChargeManager::Instance()->SendChargeResult(m_Context->connid, CHARGE_OTHER);
					ChargeManager::Instance()->_DelContext(m_Context);
					return;
				}

				// 查询账号信息
				auto accountInfo = AccountMgr::Instance()->FindAccount(m_Context->accid);
				if (accountInfo)
				{
					auto goodsData = ChargeManager::Instance()->FindChargeGoods(m_Context->chargeGoodsId);
					if (!goodsData)
					{
						ErrorLogStream << "charge(" << m_Context->ToString() << ") goods unexist." << LogStream::eos;

						// 货物找不到
						ChargeManager::Instance()->SendChargeResult(m_Context->connid, CHARGE_OTHER);
						ChargeManager::Instance()->_DelContext(m_Context->orderId);
						return;
					}

					// 月卡
					if (goodsData->itemSubType == ST_MONTH_CARD)
					{
						if (!accountInfo->CanBuyMonthCard())
						{
							ErrorLogStream << "context(" << m_Context->ToString() << ") can't buy month card." << LogStream::eos;
							// 玩家不存在
							ChargeManager::Instance()->SendChargeResult(m_Context->connid, CHARGE_OTHER);
							ChargeManager::Instance()->_DelContext(m_Context);
							return;
						}
					}

					// 理财卡
					if (goodsData->itemSubType == ST_MONEY_MANAGE_CARD)
					{
						if (!accountInfo->CanBuyMoneyManageCard())
						{
							ErrorLogStream << "context(" << m_Context->ToString() << ") can't buy money manage card." << LogStream::eos;
							// 玩家不存在
							ChargeManager::Instance()->SendChargeResult(m_Context->connid, CHARGE_OTHER);
							ChargeManager::Instance()->_DelContext(m_Context);
							return;
						}
					}

					//冒险通行证
					//if (goodsData->itemSubType == ST_ADVENTURE_KING)
					//{
					//	auto ret = AdventurePassMgr::Instance()->CanPlayerBuyKingPassByRMB(m_Context->accid);
					//	if (ret != ErrorCode::SUCCESS)
					//	{
					//		ErrorLogStream << "context(" << m_Context->ToString() << ") can't buy ST_ADVENTURE_KING." << LogStream::eos;
					//		ChargeManager::Instance()->SendChargeResult(m_Context->connid, CHARGE_OTHER);
					//		ChargeManager::Instance()->_DelContext(m_Context);
					//		return;
					//	}
					//}

					// 判断限购
					if (goodsData->dailyLimitNum > 0)
					{
						if (accountInfo->GetTodayChargeTimes(CHARGE_MALL_CHARGE, m_Context->chargeGoodsId, 0) >= goodsData->dailyLimitNum)
						{
							ErrorLogStream << "charge(" << m_Context->ToString() << ") goods daily times limit." << LogStream::eos;
							ChargeManager::Instance()->SendChargeResult(m_Context->connid, CHARGE_OTHER);
							ChargeManager::Instance()->_DelContext(m_Context);
							return;
						}
					}

					auto mallType = (ChargeMallType)(UInt8)m_Context->mallType;
					if (accountInfo->GetChargeTimes(mallType, m_Context->chargeGoodsId, 0) == 0)
					{
						m_Context->chargeItemNum += goodsData->firstAddNum;
					}
					else
					{
						m_Context->chargeItemNum += goodsData->unfirstAddNum;
					}

					accountInfo->IncChargeTimes(m_Context->accid, m_Context->roleId, mallType, m_Context->chargeGoodsId, 1);

					// 验证玩家返回
					ChargeManager::Instance()->CreateOrderRecord(m_Context);
					// 开始发货
					ChargeManager::Instance()->StartSendGoods(m_Context);
				}
				else
				{
					/*auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_account");
					cmd->PutCond("accid", m_Context->accid);
					CLRecordClient::Instance()->SendCommand(cmd, new QueryAccountCallback(m_Context));*/

					AccountMgr::Instance()->LoadAccountInfo(m_Context->accid, new QueryAccountCallback(m_Context));
				}
				
			}
		}
		else
		{
			ErrorLogStream << "charge(" << m_Context->ToString() << ") role unexist." << LogStream::eos;

			// 玩家不存在
			ChargeManager::Instance()->SendChargeResult(m_Context->connid, CHARGE_PLAYER_UNEXIST);
			ChargeManager::Instance()->_DelContext(m_Context->orderId);
		}
	}

	virtual void OnFailed(UInt32 errorcode)
	{
		ErrorLogStream << "charge(" << m_Context->ToString() << ") db error(" << errorcode << ")." << LogStream::eos;
		ChargeManager::Instance()->SendChargeResult(m_Context->connid, CHARGE_DB_ERROR);
		ChargeManager::Instance()->_DelContext(m_Context->orderId);
	}
private:
	ChargeOrderContext*		m_Context;
};

bool ChargeManager::Init()
{
	return LoadChargeGoods();
}

void ChargeManager::OnTick(const Avalon::Time& now)
{
	if (!m_ChargeCheckTimer.IsTimeout(now))
	{
		return;
	}

	std::vector<ChargeOrderContext*> needDelContext;
	for (auto itr : m_ChargingOrders)
	{
		auto context = itr.second;
		if (!context)
		{
			continue;
		}

		if (context->status == ORDER_STATUS_SENDING_ITEM && context->sendGoodTimeoutTime < (UInt32)now.Sec())
		{
			needDelContext.push_back(context);
		}
	}

	for (auto context : needDelContext)
	{
		_DelContext(context);
	}
}

bool ChargeManager::LoadChargeGoods()
{
	class ChargeMallVistor : public Avalon::DataEntryVisitor<ChargeMallDataEntry>
	{
	public:
		bool operator()(ChargeMallDataEntry* data)
		{
			datas.push_back(data);
			return true;
		}

		std::vector<ChargeMallDataEntry*> datas;
	};
	ChargeMallVistor vistor;
	ChargeMallDataEntryMgr::Instance()->Visit(vistor);

	std::vector<ChargeGoods> allGoods;
	for (auto data : vistor.datas)
	{
		if (!data)
		{
			continue;
		}

		ChargeGoods goods;
		goods.sort = data->sort;
		goods.id = data->id;
		goods.desc = data->desc;
		goods.tags = data->tags;
		goods.money = data->chargeMoney;
		goods.dailyLimitNum = data->dailiyLimitNum;
		goods.vipScore = data->vipScore;
		goods.itemId = data->itemId;
		goods.num = data->num;
		goods.firstAddNum = data->firstChargeAddNum;
		goods.unfirstAddNum = data->unfirstChargeAddNum;
		goods.isFirstCharge = 0;
		goods.remainDays = 0;
		goods.icon = data->icon;

		auto item = ItemDataEntryMgr::Instance()->FindEntry(goods.itemId);
		if (!item)
		{
			ErrorLogStream << "charge mall(" << goods.id << ") unexist item(" << goods.itemId << ")." << LogStream::eos;
			return false;
		}
		goods.itemSubType = (ItemSubType)item->subType;

		allGoods.push_back(goods);
	}

	std::sort(allGoods.begin(), allGoods.end());
	m_ChargeGoods = allGoods;


	class ChargePacketVistor : public Avalon::DataEntryVisitor<ChargePacketDataEntry>
	{
	public:
		bool operator()(ChargePacketDataEntry* data)
		{
			ChargePacket packet;
			packet.id = data->id;
			packet.sort = data->sort;
			packet.name = data->name;
			packet.oldPrice = data->oldPrice;
			packet.money = data->money;
			packet.vipScore = data->vipScore;
			packet.startTime = data->startTime;
			packet.endTime = data->endTime;
			packet.limitDailyNum = data->limitDailyNum;
			packet.limitTotalNum = data->limitTotalNum;
			packet.rewards = data->rewards;
			packet.icon = data->icon;

			ChargeManager::Instance()->AddChargePacket(packet);
			return true;
		}

	};
	ChargePacketVistor vistor2;
	ChargePacketDataEntryMgr::Instance()->Visit(vistor2);

	return true;
}

bool ChargeManager::AddChargePacket(const ChargePacket& packet)
{
	auto oldPacket = FindChargePacket(packet.id);
	if (oldPacket)
	{
		*oldPacket = packet;
	}
	else
	{
		m_ChargePackets.push_back(packet);
	}

	std::sort(m_ChargePackets.begin(), m_ChargePackets.end());

	return true;
}

void ChargeManager::OnPlayerQueryChargeGoods(WSPlayer* player)
{
	if (!player)
	{
		return;
	}

	CLProtocol::WorldBillingGoodsRes res;
	for (auto goods : m_ChargeGoods)
	{
		//理财卡和通行证不下发
		if (goods.itemSubType == ST_MONEY_MANAGE_CARD || goods.itemSubType == ST_ADVENTURE_KING || goods.itemSubType == ST_GIFT_RIGHT_CARD || goods.itemSubType == ST_DUNGEON_TICKET_RIGHT_CARD)
		{
			continue;
		}

		if ((goods.tags & (1 << (UInt32)CHARGE_MALL_TAG_WELFARE)) != 0)
		{
			continue;
		}

		goods.isFirstCharge = player->GetChargeGoodsTimes(CHARGE_MALL_CHARGE, goods.id, 0) == 0 ? 1 : 0;
		if (goods.itemSubType == ST_MONTH_CARD)
		{
			goods.remainDays = Avalon::TimeUtil::CountIntervalDays((UInt32)CURRENT_TIME.Sec(), player->GetMonthCardExpireTime(), 6);
		}

		if (goods.dailyLimitNum > 0)
		{
			auto todayNum = player->GetTodayChargeTimes(CHARGE_MALL_CHARGE, goods.id, 0);
			if (todayNum >= (UInt32)goods.dailyLimitNum)
			{
				goods.dailyLimitNum = 0;
			}
			else
			{
				goods.dailyLimitNum -= (UInt8)todayNum;
			}
		}
		else
		{
			goods.dailyLimitNum = (UInt8)-1;
		}

		res.goodsList.push_back(goods);
	}

	player->SendProtocol(res);
}

void ChargeManager::OnPlayerQueryChargePacketGoods(WSPlayer* player)
{
	if (!player)
	{
		return;
	}

	UInt32 curTime = (UInt32)CURRENT_TIME.Sec();

	CLProtocol::WorldBillingChargePacketRes res;
	for (auto goods : m_ChargePackets)
	{
		if ((goods.startTime > 0 && curTime < goods.startTime) ||
			(goods.endTime > 0 && curTime > goods.endTime))
		{
			continue;
		}

		if (goods.limitDailyNum > 0)
		{
			auto todayNum = player->GetTodayChargeTimes(CHARGE_MALL_PACKET, goods.id, player->GetID());
			if (todayNum >= (UInt32)goods.limitDailyNum)
			{
				goods.limitDailyNum = 0;
			}
			else
			{
				goods.limitDailyNum -= (UInt16)todayNum;
			}
		}
		else
		{
			goods.limitDailyNum = (UInt16)-1;
		}

		if (goods.limitTotalNum > 0)
		{
			auto totalNum = player->GetChargeGoodsTimes(CHARGE_MALL_PACKET, goods.id, player->GetID());
			if (totalNum >= (UInt32)goods.limitTotalNum)
			{
				goods.limitTotalNum = 0;
			}
			else
			{
				goods.limitTotalNum -= (UInt16)totalNum;
			}
		}
		else
		{
			goods.limitTotalNum = (UInt16)-1;
		}

		res.goodsList.push_back(goods);
	}

	player->SendProtocol(res);
}

UInt32 ChargeManager::OnPlayerCanBuyChargeGoods(WSPlayer* player, UInt32 goodsId)
{
	if (!player)
	{
		return ErrorCode::BILLING_PLAYER_OFFLINE;
	}

	auto goods = FindChargeGoods(goodsId);
	if (!goods)
	{
		return ErrorCode::BILLING_GOODS_UNEXIST;
	}

	// 月卡
	if (goods->itemSubType == ST_MONTH_CARD)
	{
		if (!player->CanBuyMonthCard())
		{
			return ErrorCode::BILLING_GOODS_MONTH_CARD_CANT_BUY;
		}
	}

	// 理财卡
	if (goods->itemSubType == ST_MONEY_MANAGE_CARD)
	{
		if (!player->CanBuyMoneyManageCard())
		{
			return ErrorCode::BILLING_GOODS_MONEY_MANAGE_CARD_BUY;
		}
	}

	//冒险通行证
	if (goods->itemSubType == ST_ADVENTURE_KING)
	{
		auto ret = AdventurePassMgr::Instance()->CanPlayerBuyKingPassByRMB(player->GetAccId());
		if (ret != ErrorCode::SUCCESS)
		{
			return ret;
		}
	}	
	
	//礼遇特权卡
	if (goods->itemSubType == ST_GIFT_RIGHT_CARD)
	{
		if (!player->CanBuyGiftRightCard())
		{
			return ErrorCode::BILLING_GOODS_ALREADY_HAVE_GIFT_RIGHT_CAARD;
		}
	}
	
	//深渊特权卡
	if (goods->itemSubType == ST_DUNGEON_TICKET_RIGHT_CARD)
	{
		if (!player->CanBuyDungeonRightCard())
		{
			return ErrorCode::BILLING_GOODS_ALREADY_HAVE_GIFT_RIGHT_CAARD;
		}
	}

	if (goods->dailyLimitNum == 0)
	{
		return ErrorCode::SUCCESS;
	}

	auto todayNum = player->GetTodayChargeTimes(CHARGE_MALL_CHARGE, goodsId, 0);
	if (todayNum >= goods->dailyLimitNum)
	{
		return ErrorCode::BILLING_GOODS_NUM_LIMIT;
	}

	return ErrorCode::SUCCESS;
}

UInt32 ChargeManager::OnPlayerCanBuyChargePacket(WSPlayer* player, UInt32 goodsId)
{
	if (!player)
	{
		return ErrorCode::BILLING_PLAYER_OFFLINE;
	}

	//auto goods = FindChargePacket(goodsId);
	auto goods = MallMgr::Instance()->GetMallItemInfo(goodsId);
	if (!goods)
	{
		return ErrorCode::BILLING_GOODS_UNEXIST;
	}

	//if (goods->limitDailyNum > 0)
	if (goods->limitnum > 0)
	{
		auto todayNum = player->GetTodayChargeTimes(CHARGE_MALL_PACKET, goodsId, player->GetID());
		//if (todayNum >= goods->limitDailyNum)
		if (todayNum >= goods->limitnum)
		{
			return ErrorCode::BILLING_GOODS_NUM_LIMIT;
		}
	}

	if (goods->limitTotalNum > 0)
	{
		auto totalNum = player->GetChargeGoodsTimes(CHARGE_MALL_PACKET, goodsId, player->GetID());
		if (totalNum >= goods->limitTotalNum)
		{
			return ErrorCode::BILLING_GOODS_NUM_LIMIT;
		}
	}

	return ErrorCode::SUCCESS;
}

void ChargeManager::ReCharge(UInt32 connid, const std::string& orderId)
{
	InfoLogStream << "start to recharge order(" << orderId << ")..." << LogStream::eos;

	if (_FindContext(orderId))
	{
		ErrorLogStream << "order(" << orderId << ") is now processing." << LogStream::eos;
		ChargeManager::Instance()->SendChargeResult(connid, CHARGE_OTHER);
		return;
	}

	auto context = new ChargeOrderContext();
	context->orderId = orderId;
	context->connid = connid;
	_AddContext(context);

	// 加载订单
	auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_charge_order");
	cmd->PutCond("orderid", orderId);
	CLRecordClient::Instance()->SendCommand(cmd, new ReChargeQueryCallback(connid, orderId));
}

void ChargeManager::StartCharge(UInt32 connid, const std::string& channel, const std::string& orderId, const std::string& token, UInt32 accid,
									ObjID_t roleId, UInt32 goodsId, UInt32 chargeMoney, UInt32 orderTime, ChargeMallType mallType)
{
	/***********************************************************************************************
	**	充值流程：																				  **
	**		验证签名 --> 查找玩家是否在线 --> 插入订单 --> 购买充值道具 --> 返回结果			  **
	**				 |					  |														  **
	**			  签名错误			  玩家不在线												  **
	**				 |					  |														  **
	**				 --> 返回		      --> 离线流程											  **
	**									  														  **
	***********************************************************************************************/

	DebugLogStream << "order(" << orderId << ") accid(" << accid << ") roleId(" << roleId << ") chargeMoney(" << chargeMoney << ") goodsId("
		<< goodsId << ") start charge..." << LogStream::eos;

	// 先看看是不是正在处理中的订单
	if (_FindContext(orderId))
	{
		ErrorLogStream << "order(" << orderId << ") is processing now, accid(" << accid << ") roleId("
			<< roleId << ") chargeMoney(" << chargeMoney << ") goodsid("
			<< goodsId << ")." << LogStream::eos;
		SendChargeResult(connid, CHARGE_OTHER);
		return;
	}

	// 验证签名(orderId + accid + roleId + malltype + goodsId + chargeMoney + sigKey)
	if (WSApplication::Instance()->IsCheckChargeOrder() && !_VerifyOrderSign(orderId, token, accid, roleId, mallType, goodsId, chargeMoney, orderTime))
	{
		ErrorLogStream << "order(" << orderId << ") accid(" << accid << ") roleId(" << roleId << ") chargeMoney(" << chargeMoney << ") goodsId("
			<< goodsId << "), invalid sign." << LogStream::eos;
		SendChargeResult(connid, CHARGE_OTHER);
		return;
	}

	//理财类型的强行指定ID
	if (goodsId == MONEY_MANAGE_OLD_CHARGE_ID && mallType == ChargeMallType::CHARGE_MALL_MONEY_MANAGE)
	{
		goodsId = MONEY_MANAGE_NOW_CHARGE_ID;
	}

	auto chargeMallData = ChargeMallDataEntryMgr::Instance()->FindEntry(goodsId);
	if (!chargeMallData)
	{
		ErrorLogStream << "order(" << orderId << ") accid(" << accid << ") roleId(" << roleId << ") chargeMoney(" << chargeMoney << ") goodsId("
			<< goodsId << "), invalid goods." << LogStream::eos;
		SendChargeResult(connid, CHARGE_INVALID_EXTRA_DATA);
		return;
	}

	if (WSApplication::Instance()->IsCheckChargeOrder() && chargeMallData->chargeMoney != chargeMoney)
	{
		ErrorLogStream << "order(" << orderId << ") accid(" << accid << ") roleId(" << roleId << ") chargeMoney(" << chargeMoney << ") goodsId("
			<< goodsId << "), invalid money correct money(" << chargeMallData->chargeMoney << ")." << LogStream::eos;
		SendChargeResult(connid, CHARGE_OTHER);
		return;
	}

	ChargeOrderContext *context = new ChargeOrderContext();
	context->status = ORDER_STATUS_INIT;
	context->channel = channel;
	context->connid = connid;
	context->orderId = orderId;
	context->accid = accid;
	context->roleId = roleId;
	context->mallType = mallType;
	context->chargeGoodsId = goodsId;
	context->chargeItemId = chargeMallData->itemId;
	context->chargeItemNum = chargeMallData->num;
	context->vipScore = chargeMallData->vipScore;
	context->chargeMoney = chargeMoney;
	context->time = orderTime;
	
	_AddContext(context);

	//UDP pay_start
	UInt32 tempMallType = context->mallType;
	Sys::SendUdpLog("pay_start", LOG_PAY_START_FORMAT, GAME_VERSION.c_str(), ZONE_ID, context->channel.c_str(), context->time, context->roleId, context->accid, 1, 0,
		context->orderId.c_str(), tempMallType, context->chargeGoodsId, context->chargeItemId, context->chargeItemNum, context->vipScore,
		context->chargeMoney, context->sendGoodTimeoutTime);

	// 验证订单状态（是否已经存在...）
	auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_charge_order");
	cmd->PutCond("orderid", orderId);
	CLRecordClient::Instance()->SendCommand(cmd, new QueryChargeOrderCallback(context));
}

void ChargeManager::BuyChargePacket(UInt32 connid, const std::string& channel, const std::string& orderId, const std::string& token, UInt32 accid, ObjID_t roleId, UInt32 goodsId, UInt32 chargeMoney, UInt32 orderTime)
{
	// 先看看是不是正在处理中的订单
	if (_FindContext(orderId))
	{
		ErrorLogStream << "order(" << orderId << ") is processing now, accid(" << accid << ") roleId("
			<< roleId << ") chargeMoney(" << chargeMoney << ") goodsid("
			<< goodsId << ")." << LogStream::eos;
		SendChargeResult(connid, CHARGE_OTHER);
		return;
	}

	// 验证签名
	if (WSApplication::Instance()->IsCheckChargeOrder() && !_VerifyOrderSign(orderId, token, accid, roleId, CHARGE_MALL_PACKET, goodsId, chargeMoney, orderTime))
	{
		ErrorLogStream << "order(" << orderId << ") accid(" << accid << ") roleId(" << roleId << ") chargeMoney(" << chargeMoney << ") goodsId("
			<< goodsId << "), invalid sign." << LogStream::eos;
		SendChargeResult(connid, CHARGE_OTHER);
		return;
	}

	//auto chargePacketData = FindChargePacket(goodsId);
	auto chargePacketData = MallMgr::Instance()->GetMallItemInfo(goodsId);
	if (!chargePacketData)
	{
		ErrorLogStream << "order(" << orderId << ") accid(" << accid << ") roleId(" << roleId << ") chargeMoney(" << chargeMoney << ") goodsId("
			<< goodsId << "), invalid goods." << LogStream::eos;
		SendChargeResult(connid, CHARGE_INVALID_EXTRA_DATA);
		return;
	}

	if (WSApplication::Instance()->IsCheckChargeOrder() && chargePacketData->discountprice != chargeMoney)
	{
		ErrorLogStream << "order(" << orderId << ") accid(" << accid << ") roleId(" << roleId << ") chargeMoney(" << chargeMoney << ") goodsId("
			<< goodsId << "), invalid money correct money(" << chargePacketData->discountprice << ")." << LogStream::eos;
		SendChargeResult(connid, CHARGE_OTHER);
		return;
	}

	// 判断是不是在开放时间
	if ((chargePacketData->startTime > 0 && orderTime < chargePacketData->startTime) ||
		(chargePacketData->endTime > 0 && orderTime > chargePacketData->endTime))
	{
		ErrorLogStream << "order(" << orderId << ") accid(" << accid << ") roleId(" << roleId << ") chargeMoney(" << chargeMoney << ") goodsId("
			<< goodsId << "), not in open time(" << chargePacketData->startTime << ", " << chargePacketData->endTime << ")." << LogStream::eos;
		SendChargeResult(connid, CHARGE_OTHER);
		return;
	}

	ChargeOrderContext *context = new ChargeOrderContext();
	context->status = ORDER_STATUS_INIT;
	context->connid = connid;
	context->channel = channel;
	context->orderId = orderId;
	context->accid = accid;
	context->roleId = roleId;
	context->mallType = CHARGE_MALL_PACKET;
	context->chargeGoodsId = goodsId;
	context->rewards = chargePacketData->rewards;
	context->vipScore = chargePacketData->vipScore;
	context->chargeMoney = chargeMoney;
	context->time = orderTime;

	_AddContext(context);

	//UDP pay_start
	UInt32 mallType = context->mallType;
	Sys::SendUdpLog("pay_start", LOG_PAY_START_FORMAT, GAME_VERSION.c_str(), ZONE_ID, context->channel.c_str(), context->time, context->roleId, context->accid, 1, 0,
		context->orderId.c_str(), mallType, context->chargeGoodsId, context->chargeItemId, context->chargeItemNum, context->vipScore,
		context->chargeMoney, context->sendGoodTimeoutTime);

	// 验证订单状态（是否已经存在...）
	auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_charge_order");
	cmd->PutCond("orderid", orderId);
	CLRecordClient::Instance()->SendCommand(cmd, new QueryChargeOrderCallback(context));
}

void ChargeManager::OnSceneSendGoodsRes(const std::string& orderId, UInt32 result)
{
	DebugLogStream << "order(" << orderId << ") send goods result(" << result << ")." << LogStream::eos;

	auto context = _FindContext(orderId);
	if (!context)
	{
		ErrorLogStream << "order(" << orderId << ") send goods result(" << result << "), can't find this order." << LogStream::eos;

		char cond[128];
		avalon_snprintf(cond, sizeof(cond)-1, "orderid = '%s'", orderId.c_str());
		ChargeOrderStatus status = ORDER_STATUS_FINISH;
		if (result == ErrorCode::SUCCESS)
		{
			// 修改订单状态为已完成
			auto cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_charge_order", cond);
			cmd->PutData("status", (UInt8)status);
			CLRecordClient::Instance()->SendCommand(cmd);
		}
		else
		{
			// 修改订单状态为错误
			status = ORDER_STATUS_ERROR;
			auto cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_charge_order", cond);
			cmd->PutData("status", (UInt8)status);
			CLRecordClient::Instance()->SendCommand(cmd);
			
		}

		// pay_end
		//UInt32 mallType = context->mallType;
		//Sys::SendUdpLog("pay_end", LOG_PAY_END_FORMAT, GAME_VERSION.c_str(), ZONE_ID, context->time, context->roleId, context->accid, 1, 0,
		//	context->orderId.c_str(), mallType, context->chargeGoodsId, context->chargeItemId, context->chargeItemNum, context->vipScore,
		//	context->chargeMoney, context->sendGoodTimeoutTime, status);

		return;
	}

	if (result == ErrorCode::SUCCESS)
	{
		context->status = ORDER_STATUS_FINISH;
		SendChargeResult(context->connid, CHARGE_SUCCESS);

		auto goods = FindChargeGoods(context->chargeGoodsId);
		if (goods != NULL)
		{
			DebugLogStream << "order(" << context->orderId << ") tag(" << goods->tags << ") player(" << context->roleId << ", " << context->accid << ") add account total charge." << LogStream::eos;
			if ((goods->tags & (1 << 2)) == 0)
			{
				AccountActivityTaskMgr::Instance()->OnAccountTotalCharge(context->accid, context->roleId, context->chargeMoney);
			}
		}
	}
	else
	{
		context->status = ORDER_STATUS_ERROR;
		SendChargeResult(context->connid, CHARGE_OTHER);
	}

	context->SaveToDB();
	// pay_end
	//UInt32 mallType = context->mallType;
	//Sys::SendUdpLog("pay_end", LOG_PAY_END_FORMAT, GAME_VERSION.c_str(), ZONE_ID, context->time, context->roleId, context->accid, 1, 0,
	//	context->orderId.c_str(), mallType, context->chargeGoodsId, context->chargeItemId, context->chargeItemNum, context->vipScore,
	//	context->chargeMoney, context->sendGoodTimeoutTime, context->status);

	_DelContext(context);
}

void ChargeManager::VerifyOrderRet(ChargeOrderContext* context, ChargeOrderStatus status)
{
	if (!context)
	{
		ErrorLogStream << "order context is null!!!" << LogStream::eos;
		return;
	}

	InfoLogStream << "order(" << context->ToString() << ") status(" << (UInt8)status << ")." << LogStream::eos;

	if (context->mallType == CHARGE_MALL_PACKET)
	{
		VerifyChargePacketOrderRet(context, status);
		return;
	}

	if (status == ORDER_STATUS_NOT_CREATE)
	{
		InfoLogStream << "start to create order(" << context->ToString() << ")." << LogStream::eos;

		// 查询账号信息
		WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(context->roleId);
		if (player)
		{
			// 检测角色ID和账号ID是否一致
			if (WSApplication::Instance()->IsCheckChargeAccount() && context->accid != player->GetAccId())
			{
				ErrorLogStream << "charge(" << context->ToString() << ") player(" << player->GetID() << ", " << player->GetAccId() << ") invalid account(" << context->accid << ")." << LogStream::eos;
				
				// 账号信息不一致
				SendChargeResult(context->connid, CHARGE_OTHER);
				_DelContext(context);
				return;
			}

			auto goodsData = FindChargeGoods(context->chargeGoodsId);
			if (!goodsData)
			{
				ErrorLogStream << "charge(" << context->ToString() << ") goods unexist." << LogStream::eos;

				// 玩家不存在
				SendChargeResult(context->connid, CHARGE_OTHER);
				_DelContext(context);
				return;
			}

			// 月卡
			if (goodsData->itemSubType == ST_MONTH_CARD)
			{
				if (!player->CanBuyMonthCard())
				{
					ErrorLogStream << "context(" << context->ToString() << ") can't buy month card." << LogStream::eos;
					// 玩家不存在
					SendChargeResult(context->connid, CHARGE_OTHER);
					_DelContext(context);
					return;
				}
			}

			// 理财卡
			if (goodsData->itemSubType == ST_MONEY_MANAGE_CARD)
			{
				if (!player->CanBuyMoneyManageCard())
				{
					ErrorLogStream << "context(" << context->ToString() << ") can't buy money manage card." << LogStream::eos;
					// 玩家不存在
					SendChargeResult(context->connid, CHARGE_OTHER);
					_DelContext(context);
					return;
				}
			}

			// 冒险通行证
			//if (goodsData->itemSubType == ST_ADVENTURE_KING)
			//{
			//	auto ret = AdventurePassMgr::Instance()->CanPlayerBuyKingPassByRMB(player->GetAccId());
			//	if (ret != ErrorCode::SUCCESS)
			//	{
			//		ErrorLogStream << "context(" << context->ToString() << ") can't buy ST_ADVENTURE_KING." << LogStream::eos;
			//		SendChargeResult(context->connid, CHARGE_OTHER);
			//		_DelContext(context);
			//		return;
			//	}
			//}


			// 判断限购
			if (goodsData->dailyLimitNum > 0)
			{
				if (player->GetTodayChargeTimes(CHARGE_MALL_CHARGE, context->chargeGoodsId, 0) >= goodsData->dailyLimitNum)
				{
					ErrorLogStream << "charge(" << context->ToString() << ") goods daily times limit." << LogStream::eos;
					SendChargeResult(context->connid, CHARGE_OTHER);
					_DelContext(context);
					return;
				}
			}
			
			ChargeMallType mallType = (ChargeMallType)(UInt8)context->mallType;
			if (player->GetChargeGoodsTimes(mallType, context->chargeGoodsId, 0) == 0)
			{
				context->chargeItemNum += goodsData->firstAddNum;
			}
			else
			{
				context->chargeItemNum += goodsData->unfirstAddNum;
			}
			
			player->AddChargeGoodsTimes(context->accid, context->roleId, mallType, context->chargeGoodsId, 1);
			CreateOrderRecord(context);
		}
		else
		{
			auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info");
			cmd->PutCond("guid", context->roleId);
			cmd->PutColumn("deletetime");
			cmd->PutColumn("accid");
			CLRecordClient::Instance()->SendCommand(cmd, new QueryChargePlayerCallback(context));
			return;
		}
	}
	else if (status == ORDER_STATUS_FINISH)
	{
		SendChargeResult(context->connid, CHARGE_REPEAT_ORDER);
		_DelContext(context->orderId);
		return;
	}
	
	InfoLogStream << "order(" << context->orderId << ") status verify success." << LogStream::eos;
	
	// 开始发货
	StartSendGoods(context);
}

void ChargeManager::VerifyChargePacketOrderRet(ChargeOrderContext* context, ChargeOrderStatus status)
{
	if (!context)
	{
		return;
	}

	auto player = WSPlayerMgr::Instance()->FindNormalPlayer(context->roleId);
	if (!player)
	{
		ErrorLogStream << "order(" << context->orderId << ") player(" << context->roleId << ") offline." << LogStream::eos;
		SendChargeResult(context->connid, CHARGE_OTHER);
		_DelContext(context->orderId);
		return;
	}

	//auto chargePacketData = FindChargePacket(context->chargeGoodsId);
	auto chargePacketData = MallMgr::Instance()->GetMallItemInfo(context->chargeGoodsId);
	if (!chargePacketData)
	{
		ErrorLogStream << "order(" << context->orderId << ") player(" << context->roleId << ") invalid charge packet(" << context->chargeGoodsId << ")." << LogStream::eos;
		SendChargeResult(context->connid, CHARGE_OTHER);
		_DelContext(context->orderId);
		return;
	}

	// 验证次数，验证成功后直接添加次数
	//if (chargePacketData->limitDailyNum > 0)
	if (chargePacketData->limitnum > 0)
	{
		UInt32 num = player->GetTodayChargeTimes(CHARGE_MALL_PACKET, context->chargeGoodsId, context->roleId);
		//if (num >= chargePacketData->limitDailyNum)
		if (num >= chargePacketData->limitnum)
		{
			ErrorLogStream << "order(" << context->orderId << ") player(" << context->roleId << ") charge packet(" << context->chargeGoodsId << ") limited." << LogStream::eos;
			SendChargeResult(context->connid, CHARGE_OTHER);
			_DelContext(context->orderId);
			return;
		}
	}

	if (chargePacketData->limitTotalNum > 0)
	{
		UInt32 num = player->GetChargeGoodsTimes(CHARGE_MALL_PACKET, context->chargeGoodsId, context->roleId);
		if (num >= chargePacketData->limitTotalNum)
		{
			ErrorLogStream << "order(" << context->orderId << ") player(" << context->roleId << ") charge packet(" << context->chargeGoodsId << ") limited." << LogStream::eos;
			SendChargeResult(context->connid, CHARGE_OTHER);
			_DelContext(context->orderId);
			return;
		}
	}

	player->AddChargeGoodsTimes(context->accid, context->roleId, CHARGE_MALL_PACKET, context->chargeGoodsId, 1);

	if (status == ORDER_STATUS_NOT_CREATE)
	{
		// 创建订单
		CreateOrderRecord(context);
	}
	else if (status == ORDER_STATUS_FINISH)
	{
		SendChargeResult(context->connid, CHARGE_REPEAT_ORDER);
		_DelContext(context->orderId);
		return;
	}

	// 发货
	StartSendGoods(context);
}

void ChargeManager::CreateOrderRecord(ChargeOrderContext* context)
{
	if (!context)
	{
		return;
	}

	context->SetID(CLRecordClient::Instance()->GenGuid());
	context->status = ORDER_STATUS_VERIFYING;
	context->InsertToDB();

	InfoLogStream << "insert order(" << context->ToString() << ") to db..." << LogStream::eos;
}

void ChargeManager::StartSendGoods(ChargeOrderContext* context)
{
	if (!context)
	{
		return;
	}

	DebugLogStream << "order(" << context->orderId << ") start sending goods." << LogStream::eos;

	context->status = ORDER_STATUS_SENDING_ITEM;
	context->sendGoodTimeoutTime = (UInt32)CURRENT_TIME.Sec() + 60;
	context->SaveToDB();

	// 正式开始发货
	CLProtocol::SceneBillingSendGoods req;
	req.orderId = context->orderId;
	req.accid = context->accid;
	req.roleId = context->roleId;
	req.mallType = context->mallType;
	req.chargeGoodsId = context->chargeGoodsId;
	req.chargeItemId = context->chargeItemId;
	req.chargeItemNum = context->chargeItemNum;
	req.vipScore = context->vipScore;
	req.chargeMoney = context->chargeMoney;
	req.time = context->time;
	req.sendGoodTimeoutTime = context->sendGoodTimeoutTime;
	if (context->mallType == CHARGE_MALL_CHARGE || context->mallType == CHARGE_MALL_MONEY_MANAGE ||
		context->mallType == CHARGE_ADVENTURE_PASS_KING || context->mallType == CHARGE_GIGT_RIGHT_CARD ||
		context->mallType == CHARGE_DUNGEON_RIGHT_CARD)
	{
		req.rewards.push_back(ItemReward(context->chargeItemId, context->chargeItemNum, 1, 0));
	}
	else
	{
		req.rewards = context->rewards;
	}

	auto accountInfo = AccountMgr::Instance()->FindAccount(context->accid);
	if (!accountInfo)
	{
		ErrorLogStream << "player(" << context->roleId << ") order(" << req.orderId << ") can't find account(" << context->accid << ")'s info." << LogStream::eos;

		context->status = ORDER_STATUS_ERROR;
		SendChargeResult(context->connid, CHARGE_OTHER);
		context->SaveToDB();
		_DelContext(context);
		return;
	}

	req.vipLevel = accountInfo->GetVipLevel();
	req.vipExp = accountInfo->GetVipExp();
	req.totalChargeNum = accountInfo->GetTotalChargeNum();
	req.point = accountInfo->GetPoint();
	
	auto onlinePlayer = WSPlayerMgr::Instance()->FindPlayerByAccid(context->accid);
	if (onlinePlayer)
	{
		req.onlinePlayerID = onlinePlayer->GetID();
	}
	else
	{
		req.onlinePlayerID = 0;
	}

	auto itemData = ItemDataEntryMgr::Instance()->FindEntry(context->chargeItemId);
	if (itemData != NULL)
	{
		if (itemData->subType == ST_ITEM_POINT)
		{
			AccountActivityTaskMgr::Instance()->OnCharge(context->accid, context->roleId, context->chargeMoney);
		}
	}

	auto player = WSPlayerMgr::Instance()->FindNormalPlayer(context->roleId);
	if (player)
	{
		
		player->SendToScene(req);
		return;
	}

	// 这里先广播到scene
	// 正常情况应该是选择一个scene
	auto sceneServerID = WSPlayerMgr::Instance()->GetPlayerSceneServerID(context->roleId);
	if (sceneServerID == 0)
	{
		sceneServerID = WSNetwork::Instance()->SelectRandSceneServer();
	}
	
	auto sceneConn = WSNetwork::Instance()->FindConnection(sceneServerID);
	if (sceneConn != NULL)
	{
		CL_SEND_PROTOCOL(sceneConn, req);
	}
	else
	{
		ErrorLogStream << "player(" << context->roleId << ") order(" << req.orderId << ") no sceneserver to send goods." << LogStream::eos;

		context->status = ORDER_STATUS_ERROR;
		SendChargeResult(context->connid, CHARGE_OTHER);
		context->SaveToDB();
		_DelContext(context);
	}
}

ChargePacket* ChargeManager::FindChargePacket(UInt32 goodsId)
{
	for (auto& packet : m_ChargePackets)
	{
		if (packet.id == goodsId)
		{
			return &packet;
		}
	}

	return NULL;
}

ChargeGoods* ChargeManager::FindChargeGoods(UInt32 goodsId)
{
	for (auto& goods : m_ChargeGoods)
	{
		if (goods.id == goodsId)
		{
			return &goods;
		}
	}

	return NULL;
}

void ChargeManager::SendChargeResult(UInt32 connid, ChargeResult result)
{
	InfoLogStream << "send charge result(" << (UInt8)result << ") to conn(" << connid << ")." << LogStream::eos;

	json::Object body;
	body["result"] = json::Number((UInt32)result);
	WSConsoleAgent::Instance()->SendResponse(connid, "chargeMange", "charge", body);
}

bool ChargeManager::_VerifyOrderSign(const std::string& orderId, const std::string& token, UInt32 accid, ObjID_t roleId, UInt8 mallType, UInt32 goodsId, UInt32 chargeMoney, UInt32 orderTime)
{
	// 验证签名(orderId + accid + roleId + goodsId + chargeMoney + sigKey)
	char signStr[33];
	std::string signSrc = orderId + Avalon::StringUtil::ConvertToString(accid) + Avalon::StringUtil::ConvertToString(roleId) + Avalon::StringUtil::ConvertToString(mallType) +
		Avalon::StringUtil::ConvertToString(goodsId) + Avalon::StringUtil::ConvertToString(chargeMoney) + Avalon::StringUtil::ConvertToString(orderTime) +
							WSApplication::Instance()->GetBillingSigKey();
	Avalon::md5_encode32(signStr, (void*)signSrc.c_str(), signSrc.length());
	signStr[32] = 0;

	if (token.compare(signStr) != 0)
	{
		ErrorLogStream << "order(" << orderId << ") accid(" << accid << ") roleId(" << roleId << ") chargeMoney(" << chargeMoney << ") goodsId(" 
						<< goodsId << "), token error." << LogStream::eos;
		return false;
	}

	return true;
}

void ChargeManager::_AddContext(ChargeOrderContext* context)
{
	if (!context)
	{
		return;
	}

	m_ChargingOrders[context->orderId] = context;
}

ChargeOrderContext* ChargeManager::_FindContext(const std::string& orderId)
{
	auto itr = m_ChargingOrders.find(orderId);
	return itr == m_ChargingOrders.end() ? NULL : itr->second;
}

void ChargeManager::_DelContext(const std::string& orderId)
{
	auto itr = m_ChargingOrders.find(orderId);
	if (itr == m_ChargingOrders.end())
	{
		return;
	}

	auto context = itr->second;
	m_ChargingOrders.erase(itr);
	if (context)
	{
		delete context;
	}
}

void ChargeManager::_DelContext(ChargeOrderContext* context)
{
	if (!context)
	{
		return;
	}

	_DelContext(context->orderId);
}