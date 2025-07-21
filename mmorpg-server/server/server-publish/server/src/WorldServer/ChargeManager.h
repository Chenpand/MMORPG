#ifndef __CHARGE_MANAGER_H__
#define __CHARGE_MANAGER_H__

#include <CLDefine.h>
#include <AvalonSingleton.h>
#include <CLBillingProtocol.h>
#include <AvalonSimpleTimer.h>
#include "ChargeOrderContext.h"

enum ChargeResult
{
	// 成功
	CHARGE_SUCCESS,
	// 参数错误
	CHARGE_INVALID_PARAM,
	// 玩家不存在
	CHARGE_PLAYER_UNEXIST,
	// 游戏服不存在
	CHARGE_INVALID_SERVER_ID,
	// 订单已存在
	CHARGE_REPEAT_ORDER,
	// 透传信息错误
	CHARGE_INVALID_EXTRA_DATA,
	// 签名校验错误
	CHARGE_INVALID_SIGN,
	// 数据库错误
	CHARGE_DB_ERROR,
	// 其它错误
	CHARGE_OTHER,
};

class WSPlayer;
class ChargeManager : public Avalon::Singleton<ChargeManager>
{
public:
	/**
	*@brief 初始化
	*/
	bool Init();

	/**
	*@brief 初始化
	*/
	void OnTick(const Avalon::Time& now);

	/**
	*@brief 加载充值货物
	*/
	bool LoadChargeGoods();

	/**
	*@brief 加载充值货物
	*/
	bool AddChargePacket(const ChargePacket& packet);

	/**
	*@brief 玩家请求商品列表
	*/
	void OnPlayerQueryChargeGoods(WSPlayer* player);

	/**
	*@brief 玩家请求商品列表
	*/
	void OnPlayerQueryChargePacketGoods(WSPlayer* player);

	/**
	*@brief 判断能否购买充值商品
	*/
	UInt32 OnPlayerCanBuyChargeGoods(WSPlayer* player, UInt32 goodsId);

	/**
	*@brief 判断能否购买充值礼包
	*/
	UInt32 OnPlayerCanBuyChargePacket(WSPlayer* player, UInt32 goodsId);

	/**
	*@brief 充值补单
	*/
	void ReCharge(UInt32 connid, const std::string& orderId);

	/**
	*@brief 开始充值
	*/
	void StartCharge(UInt32 connid, const std::string& channel, const std::string& orderId, const std::string& token, UInt32 accid, ObjID_t roleId, UInt32 goodsId, UInt32 chargeMoney, UInt32 orderTime, ChargeMallType mallType = ChargeMallType::CHARGE_MALL_MONEY_MANAGE );

	/**
	*@brief 购买人民币礼包
	*/
	void BuyChargePacket(UInt32 connid, const std::string& channel, const std::string& orderId, const std::string& token, UInt32 accid, ObjID_t roleId, UInt32 goodsId, UInt32 chargeMoney, UInt32 orderTime);

	/**
	*@brief 发货返回(scene -> world)
	*/
	void OnSceneSendGoodsRes(const std::string& orderId, UInt32 result);

public:
	// 充值验证流程

	/**
	*@brief 验证订单返回
	*/
	void VerifyOrderRet(ChargeOrderContext* context, ChargeOrderStatus status);

	/**
	*@brief 验证人民币礼包订单返回
	*/
	void VerifyChargePacketOrderRet(ChargeOrderContext* context, ChargeOrderStatus status);

	/**
	*@brief 验证玩家返回（玩家存在）
	*/
	void CreateOrderRecord(ChargeOrderContext* context);

	/**
	*@brief 开始发货
	*/
	void StartSendGoods(ChargeOrderContext* context);

public:
	/**
	*@brief 获取人民币礼包
	*/
	ChargePacket* FindChargePacket(UInt32 goodsId);
	
	/**
	*@brief 充值货物
	*/
	ChargeGoods* FindChargeGoods(UInt32 goodsId);

public:

	/**
	*@brief 返回充值结果
	*/
	void SendChargeResult(UInt32 connid, ChargeResult result);

protected:
	/**
	*@brief 验证订单签名
	*/
	bool _VerifyOrderSign(const std::string& orderId, const std::string& token, UInt32 accid, ObjID_t roleId, UInt8 mallType, UInt32 goodsId, UInt32 chargeMoney, UInt32 orderTime);

	/**
	*@brief 添加正在进行中的订单
	*/
	void _AddContext(ChargeOrderContext* context);

	/**
	*@brief 查询正在进行中的订单
	*/
	ChargeOrderContext* _FindContext(const std::string& orderId);

	/**
	*@brief 删除正在进行中的订单
	*/
	void _DelContext(const std::string& orderId);

	/**
	*@brief 删除正在进行中的订单
	*/
	void _DelContext(ChargeOrderContext* context);

private:
	// 正在进行中的充值订单
	HashMap<std::string, ChargeOrderContext*>		m_ChargingOrders;

	// 充值商品
	std::vector<ChargeGoods>						m_ChargeGoods;

	// 人民币礼包
	std::vector<ChargePacket>						m_ChargePackets;

	// 充值订单检测定时器
	Avalon::SimpleTimer								m_ChargeCheckTimer;

	friend class ReChargeQueryCallback;
	friend class QueryChargeOrderCallback;
	friend class QueryChargePlayerCallback;
	friend class QueryAccountCallback;
};

#endif