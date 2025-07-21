#ifndef _CL_BILLING_PROTOCOL_H_
#define _CL_BILLING_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLItemDefine.h"
#include "CLBillingDefine.h"

namespace CLProtocol
{
	/**
	 *@brief billing->admin billing请求发货
	 */
	class AdminBillingGiveItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ADMIN_BILLING_GIVEITEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accId & friendId & itemId & num & price & orderId & token & amt & payamt_coins & moneyType & taocanId;
		}

		//玩家id
		UInt32	accId;
		//好友id
		std::string	friendId;
		//道具id
		UInt32	itemId;
		//道具数量
		UInt32	num;
		//价格
		UInt32	price;
		//订单号
		std::string	orderId;
		//口令
		std::string	token;
		//本次消费总额
		std::string	amt;
		//本次消费二级货币的量
		std::string	payamt_coins;
		//货币类型
		std::string	moneyType;
		//套餐id
		std::string	taocanId;
	};

	/**
	 *@brief admin->billing 发货返回给billing
	 */
	class AdminBillingGiveItemRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ADMIN_BILLING_GIVEITEM_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & level & isqqvip & qqviplvl;
		}

		//结果
		UInt8	result;
		//玩家等级
		UInt16	level;
		//是否黄钻
		UInt16	isqqvip;
		//qq vip等级
		UInt16	qqviplvl;
	};

	/**
	 *@brief admin->world billing请求发货
	 */
	class WorldBillingGiveItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BILLING_GIVEITEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeId & accId & friendId & itemId & num & price & orderId & token & amt & payamt_coins & moneyType & taocanId;
		}

		//连接id
		UInt32	nodeId;
		//玩家id
		UInt32	accId;
		//好友id
		ObjID_t	friendId;
		//道具id
		UInt32	itemId;
		//道具数量
		UInt32	num;
		//价格
		UInt32	price;
		//订单号
		std::string	orderId;
		//口令
		std::string	token;
		//本次消费总额
		std::string	amt;
		//本次消费二级货币量
		std::string	payamt_coins;
		//货币类型
		std::string	moneyType;
		//套餐id
		std::string	taocanId;
	};

	/**
	 *@brief billing->admin 请求角色信息                                                                     
	 */
	class AdminBillingRoleInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ADMIN_BILLING_ROLEINFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & zoneId & accId;
		}

	public:
		//区号
		UInt32	zoneId;
		//账号id
		UInt32	accId;
	};

	/**
	 *@brief admin->billing 角色信息返回                                                                     
	 */
	class AdminBillingRoleInfoRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ADMIN_BILLING_ROLEINFO_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & name & level & sex & gold;
		}

	public:
		//玩家名
		std::string	name;
		//等级
		UInt16	level;
		//性别
		UInt8	sex;
		//元宝数
		UInt32	gold;
	};

	/**
	 *@brief server->billing 发货返回
	 */
	class WorldBillingGiveItemRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BILLING_GIVEITEM_RET)
	public:
		WorldBillingGiveItemRet():nodeId(0), accId(0), result(0), level(0), isqqvip(0), qqviplvl(0){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeId & accId & result & level & isqqvip & qqviplvl;
		}

		//连接id
		UInt32	nodeId;
		//玩家账号
		UInt32	accId;
		//结果
		UInt8	result;
		//玩家等级
		UInt16	level;
		//是否黄钻
		UInt16	isqqvip;
		//qq vip等级
		UInt16	qqviplvl;
	};

	/**
	 *@brief client->server 请求签名
	 */
	class WorldBillingSigReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BILLING_SIG_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & openId & friendId & pid & num & price & zoneId;
		}

		//openid
		std::string	openId;
		//好友id
		ObjID_t		friendId;
		//道具id
		UInt32		pid;
		//数量
		UInt32		num;
		//价格
		UInt32		price;
		//区号
		UInt32		zoneId;
	};

	/**
	 *@brief server->client 返回签名
	 */
	class WorldBillingSigRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BILLING_SIG_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & friendId & pid & num & price & time & sig;
		}

		//好友id
		std::string	friendId;
		//道具id
		UInt32		pid;
		//数量
		UInt32		num;
		//价格
		UInt32		price;
		//时间戳
		UInt32		time;
		//签名
		std::string	sig;
	};

	/**
	 *@brief 支付角色信息请求                                                                     
	 */
	class WorldBillingRoleInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BILLING_ROLEINFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & zoneId & accId;
		}

	public:
		UInt32	zoneId;
		UInt32	accId;
	};

	/**
	 *@brief 支付角色信息返回                                                                     
	 */
	class WorldBillingRoleInfoRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BILLING_ROLEINFO_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeId & name & level & sex & gold;
		}

	public:
		//节点id
		UInt32	nodeId;
		//玩家名
		std::string	name;
		//等级
		UInt16	level;
		//性别
		UInt8	sex;
		//元宝数
		UInt32	gold;
	};

	/**
	*@brief 支付角色信息返回
	*/
	class WorldBillingGoodsReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BILLING_GOODS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			
		}
	};

	/**
	*@brief 支付角色信息返回
	*/
	class WorldBillingGoodsRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BILLING_GOODS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & goodsList;
		}

		std::vector<ChargeGoods>	goodsList;
	};

	/**
	*@brief 获取充值礼包商品
	*/
	class WorldBillingChargePacketReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BILLING_CHARGE_PACKET_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	};

	/**
	*@brief 返回充值礼包商品
	*/
	class WorldBillingChargePacketRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BILLING_CHARGE_PACKET_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & goodsList;
		}

		std::vector<ChargePacket>	goodsList;
	};

	/**
	*@brief world -> scene 通知scene开始发货
	*/
	class SceneBillingSendGoods : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BILLING_SEND_GOODS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & orderId & accid & roleId & mallType & chargeGoodsId & chargeItemId & chargeItemNum & vipScore & chargeMoney & 
				time & sendGoodTimeoutTime & rewards & onlinePlayerID & vipLevel & vipExp & totalChargeNum & point;
		}
		// 订单号
		std::string		orderId;
		// 账号ID
		UInt32			accid;
		// 角色ID
		ObjID_t			roleId;
		//商城类型
		UInt8			mallType;
		//充值商品ID
		UInt32			chargeGoodsId;
		//充值道具ID
		UInt32			chargeItemId;
		//道具数量
		UInt32			chargeItemNum;
		// VIP积分
		UInt32			vipScore;
		// 充值金额
		UInt32			chargeMoney;
		// 时间戳
		UInt32			time;
		// 发货超时时间
		UInt32			sendGoodTimeoutTime;
		// 商品
		ItemRewardVec	rewards;
		// 同账户下当前在线的玩家
		ObjID_t			onlinePlayerID;
		// vip等级
		UInt8			vipLevel;
		// vip经验
		UInt32			vipExp;
		// 总充值数
		UInt32			totalChargeNum;
		// 点劵
		UInt32			point;
	};
	
	/**
	*@brief scene -> world 通知world发货结果
	*/
	class SceneBillingSendGoodsRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BILLING_SEND_GOODS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & orderId & result;
		}

		// 订单号
		std::string		orderId;
		// 结果
		UInt32			result;
	};

	/**
	*@brief scene->client 通知客户端发货了
	*/
	class SceneBillingSendGoodsNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BILLING_SEND_GOODS_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};
	

	/**
	*@brief client -> world 请求购买商品(这里只判断能不能买)
	*/
	class WorldBillingChargeReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BILLING_CHARGE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mallType & goodsId;
		}

		// 商城类型
		UInt8			mallType;
		// 商品ID
		UInt32			goodsId;
	};
	
	/**
	*@brief world->client 返回能否购买商品
	*/
	class WorldBillingChargeRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BILLING_CHARGE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		// 结果
		UInt32			result;
	};
};

#endif
