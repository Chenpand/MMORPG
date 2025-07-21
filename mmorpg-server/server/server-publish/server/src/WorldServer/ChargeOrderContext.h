#ifndef __CHARGE_ORDER_CONTEXT_H__
#define __CHARGE_ORDER_CONTEXT_H__

#include <sstream>
#include <CLDefine.h>
#include <AvalonObjectPool.h>
#include <CLSceneObject.h>
#include <CLItemDefine.h>

// 订单状态
enum ChargeOrderStatus
{
	// 初始状态
	ORDER_STATUS_INIT,
	// 未创建
	ORDER_STATUS_NOT_CREATE = 0,
	// 验证中
	ORDER_STATUS_VERIFYING = 1,
	// 发货中
	ORDER_STATUS_SENDING_ITEM = 2,
	// 订单异常
	ORDER_STATUS_ERROR = 3,
	// 订单完成
	ORDER_STATUS_FINISH = 4,
};

class ChargeMallDataEntry;
// 充值订单上下文
struct ChargeOrderContext : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(ChargeOrderContext, Avalon::NullMutex)

	ChargeOrderContext();

	/**
	*@brief 插入订单到数据库
	*/
	void InsertToDB();

	/**
	*@brief 插入订单到数据库
	*/
	void SetDBData(CLRecordCallback* callback);

	/**
	*@brief 更新到数据库
	*/
	void SaveToDB();

	/**
	*@brief ToString
	*/
	std::string ToString() const;

	// 订单状态
	ChargeOrderStatus	status;
	// 连接ID
	UInt32				connid;
	// 渠道
	std::string			channel;
	// 订单号
	std::string			orderId;
	// 账号ID
	UInt32				accid;
	// 角色ID
	ObjID_t				roleId;
	// 商城类型
	ObjUInt8			mallType;
	// 充值商品ID
	UInt32				chargeGoodsId;
	// 充值道具ID
	UInt32				chargeItemId;
	// 道具数量
	UInt32				chargeItemNum;
	// VIP积分
	UInt32				vipScore;
	// 充值金额
	UInt32				chargeMoney;
	// 时间戳
	UInt32				time;
	// 发货超时时间
	UInt32				sendGoodTimeoutTime;
	// 所有道具
	ItemRewardVec		rewards;
};


#endif