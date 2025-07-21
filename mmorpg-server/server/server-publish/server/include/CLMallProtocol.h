#ifndef _CL_MALL_PROTOCOL_H_
#define _CL_MALL_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include <CLMallDefine.h>

namespace CLProtocol
{
	/**
	*@brief client->server 购买商城道具请求
	*/
	class WorldMallBuy : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MALL_BUY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & m_uId & m_uNum;
		}
	public:
		// 道具Id
		UInt32	m_uId;
		// 数量
		UInt16	m_uNum;
	};

	/**
	*@brief server->client 购买商城道具返回
	*/
	class WorldMallBuyRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MALL_BUY_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & m_code & m_mallItemId & m_restLimitNum & m_accountRestBuyNum;
		}
	public:
		UInt32	m_code;
		UInt32	m_mallItemId;
		Int32	m_restLimitNum;
		// 账号剩余购买次数
		UInt32  m_accountRestBuyNum;
	};

	class CWMallBatchBuyReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CW_MALL_BATCH_BUY_FASHION_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & _buyItems;
		}
	public:
		std::vector<ItemReward>	_buyItems;
	};

	class SCMallBatchBuyRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WC_MALL_BATCH_BUY_FASHION_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & _code & _itemUids;
		}
	public:
		UInt32				_code;
		std::vector<UInt64>	_itemUids;
	};

	/**
	*@brief world->scene 检查购买商城道具请求
	*/
	class WorldMallCheckBuy : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MALL_CHECK_BUY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & m_uPlayerId & m_uAccId & m_uCost & m_uMoneyType & m_uItemDataId & m_uItemData & m_uMallItemId & m_uMallItemNum & m_uReceiver & mallLimitBuyType & m_uLimitNum
				& m_uTotalLimitNum & m_uGoodsType & m_uVipScore & m_uItemQuality & m_uBind & billingId & orderId & coins & moneyType & taocanId & m_DiscountRate
				& m_discountCouponId & multiple & mallType & deductionCouponId & tagType & creditPointDeduction;
		}
	public:
		// 玩家id
		ObjID_t	m_uPlayerId;
		// 账号id
		UInt32	m_uAccId;
		// 购买费用
		UInt32	m_uCost;
		// 货币类型
		UInt8	m_uMoneyType;
		// 道具id
		std::vector<UInt32>	m_uItemDataId;
		// 道具id-->数量
		std::map<UInt32, UInt32>	m_uItemData;
		// 商城道具id
		UInt32	m_uMallItemId;
		// 组数
		UInt16	m_uMallItemNum;
		// 获得道具的玩家id
		ObjID_t	m_uReceiver;
		// 限购刷新类型
		UInt8	mallLimitBuyType;
		// 限购次数
		UInt32 m_uLimitNum;
		// 总限购次数
		UInt32 m_uTotalLimitNum;
		// 商品类型
		UInt8 m_uGoodsType;
		// 购买后获得的vip积分
		UInt16 m_uVipScore;
		// 道具品质
		UInt8	m_uItemQuality;
		// 绑定
		UInt8	m_uBind;
		// billing连接id
		UInt32	billingId;
		// 订单号
		std::string	orderId;
		// 额度
		std::string	coins;
		// 货币类型
		std::string	moneyType;
		// 套餐id
		std::string	taocanId;
		// 打折率
		UInt32 m_DiscountRate;
		// 折扣券id
		UInt32 m_discountCouponId;
		// 积分倍率
		UInt8 multiple;
		// 商城类别
		UInt8 mallType;
		// 抵扣券
		UInt32 deductionCouponId;
		// 标签
		UInt8 tagType;
		// 信用点券抵扣
		UInt32 creditPointDeduction;
	};

	/**
	*@brief scene->world 检查购买商城道具返回
	*/
	class WorldMallCheckBuyRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MALL_CHECK_BUY_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & m_uPlayerId & m_uRet & m_uMallItemId & m_getItemUid & m_uMallItemNum & m_uReceiver & m_restLimitNum;
		}
	public:
		// 玩家id
		ObjID_t	m_uPlayerId;
		// 返回结果
		UInt32	m_uRet;
		// 商城道具id
		UInt32	m_uMallItemId;
		// 获得道具uid
		std::vector<UInt64>	m_getItemUid;
		// 组数
		UInt16	m_uMallItemNum;
		// 获得道具的玩家id
		ObjID_t	m_uReceiver;
		// 剩余限购次数
		UInt32  m_restLimitNum;
	};
	
	/**
	*@brief client->server 请求商品
	*/
	class WorldMallQueryItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MALL_QUERY_ITEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & m_tagType & m_type & m_subType & m_moneyType & m_job & m_updateTime & m_IsPersonalTailor;
		}

		UInt8 m_tagType;			//热门索引,1-热门
		UInt8 m_type;				//商城类别
		UInt8 m_subType;			//商城子类别
		UInt8	m_moneyType;		//货币类型
		UInt8 m_job;				//职业
		UInt32	m_updateTime;		// 更新的时间(s)
		UInt8 m_IsPersonalTailor;	//是否私人订制
	};

	/**
	*@brief client->server 请求商品礼包详细
	*/
	class WorldMallQueryItemDetailReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MALL_QUERY_ITEM_DETAIL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & m_MallItemId;
		}

		UInt32	m_MallItemId;	//商城物品id
	};

	/**
	*@brief server->client 请求商品礼包详细返回
	*/
	class WorldMallQueryItemDetailRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MALL_QUERY_ITEM_DETAIL_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & details;
		}

		std::vector<MallGiftDetail>	details;	//商城礼包详细
	};

	
	
	// server->client 请求商品返回
	class WorldMallQueryItemRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MALL_QUERY_ITEM_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mallType & m_items;
		}

		//商城类别
		UInt8 mallType;
		std::vector<MallItemInfo>	m_items;
	};
	
	/**
	*@brief server->client 同步限量商品数量变化
	*/
	class WorldMallLimitedItemChange : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MALL_LIMITED_ITEM_CHANGE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & totalnum & limitnum;
		}

	public:
		// 商品id
		UInt32	id;
		// 剩余数量
		UInt16	totalnum;
		// 限购数量
		UInt16	limitnum;
	};	

	struct QueryGood
	{
		QueryGood() :id(0), bLimit(0), discountRate(0) {}
		~QueryGood(){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & bLimit & discountRate;
		}

		UInt32	id;
		UInt8	bLimit;
		UInt8	discountRate;
	};
	/**
	*@brief w->s 刷新商城限购次数
	*/
	class WSRefreshLimiteBuyNum : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WS_REFRESH_LIMITEBUYNUM)
	public:
		WSRefreshLimiteBuyNum()
		{
			playerid = 0;
			isPersonalTailor = 0;
			mallType = 0;
			isQuerySingleGoods = 0;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerid & qgs & isPersonalTailor & mallType & isQuerySingleGoods;
		}

	public:
		ObjID_t playerid;
		// 商品id
		std::vector<QueryGood>	qgs;
		// 是否私人订制
		UInt8 isPersonalTailor;
		//商城类别
		UInt8 mallType;
		// 是否查询单个商品
		UInt8 isQuerySingleGoods;
	};
	
	/**
	*@brief s->w 刷新商城限购次数返回
	*/
	class SWRefreshLimiteBuyNum : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SW_REFRESH_LIMITEBUYNUM)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerid & qgs & code & isPersonalTailor & mallType & isQuerySingleGoods;
		}

	public:
		ObjID_t playerid;
		
		// 商品id
		std::vector<QueryGood>	qgs;
		UInt32 code;
		UInt8 isPersonalTailor;
		//商城类别
		UInt8 mallType;
		// 是否查询单个商品
		UInt8 isQuerySingleGoods;
	};

	/**
	*@brief client ->server 激活商城限时礼包请求
	*/
	class WorldMallGiftPackActivateReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MALL_GIFT_PACK_ACTIVATE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & m_GiftPackActCond;
		}

	public:
		UInt8 m_GiftPackActCond;
	};

	/**
	*@brief scene ->server 激活商城限时礼包请求
	*/
	class MallGiftPackActivateReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(MALL_GIFT_PACK_ACTIVATE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & m_PlayerId & m_GiftPackActCond;
		}

	public:
		ObjID_t m_PlayerId;
		UInt8 m_GiftPackActCond;
	};

	/**
	*@brief server->client 激活商城限时礼包返回
	*/
	class WorldMallGiftPackActivateRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MALL_GIFT_PACK_ACTIVATE_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & m_Items & m_Code;
		}

	public:
		std::vector<MallItemInfo> m_Items;
		UInt32 m_Code;
	};

	/**
	*@brief world->client 同步商城礼包活动状态
	*/
	class SyncWorldMallGiftPackActivityState : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(MALL_GIFT_PACK_ACTIVITY_SYNC_STATE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & m_State;
		}

	public:
		UInt8 m_State;
	};

	/**
	*@brief scene->world 商城触发条件请求
	*/
	class MallActivateCondReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MALL_ACTIVATE_COND_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & m_RoleId & m_ActivateCond & m_ActivateType;
		}

	public:
		// 角色id
		UInt64		m_RoleId;
		// 触发条件
		UInt8		m_ActivateCond;
		// 触发类型
		UInt8		m_ActivateType;
	};

	/**
	*@brief world->scene 同步商城私人订制状态
	*/
	class WorldSyncMallPersonalTailorState : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MALL_PERSONAL_TAILOR_SYNC_STATE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & m_State & m_GoodsId;
		}

	public:
		// 同步状态
		UInt8		m_State;
		// 最新触发的商品id
		UInt32		m_GoodsId;
	};

	/**
	*@brief world->scene 加载商城触发条件数据库通知
	*/
	class WolrdLoadConditionActivateDBNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_LOAD_CONDITION_ACTIVATE_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & m_RoleId & m_IsLoaded;
		}

	public:
		// 角色id
		UInt64		m_RoleId;
		// 是否加载完毕
		UInt8		m_IsLoaded;
	};	

	/**
	*@brief client->world 根据道具id获取商城道具请求
	*/
	class WorldGetMallItemByItemIdReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GET_MALL_ITEMS_BY_ITEM_ID_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & m_ItemId;
		}

		// 道具id(不是商城道具的id)
		UInt32 m_ItemId;
	};

	/**
	*@brief world->client 根据道具id获取商城道具返回(仅作为展示用)
	*/
	class WorldGetMallItemByItemIdRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GET_MALL_ITEMS_BY_ITEM_ID_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & m_ItemId & m_MallItem;
		}

		// 错误码
		UInt32 retCode;
		// 道具id
		UInt32 m_ItemId;
		// 映射的商城道具
		MallItemInfo m_MallItem;
	};

	/**
	*@brief client->world 商城查询单个商品请求
	*/
	class WorldMallQuerySingleItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MALL_QUERY_SINGLE_ITEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mallItemId;
		}

		// 商城道具id
		UInt32 mallItemId;
	};

	/**
	*@brief world->client 商城查询单个商品返回
	*/
	class WorldMallQuerySingleItemRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MALL_QUERY_SINGLE_ITEM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & mallItemInfo;
		}

		// 错误码
		UInt32 retCode;
		// 商城道具
		MallItemInfo mallItemInfo;
	};

	/**
	*@brief world->client 玩家拥有的商城购买获得物同步
	*/
	class WorldPlayerMallBuyGotInfoSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PLAYER_MALL_BUY_GOT_INFO_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mallBuyGotInfo;
		}

	public:
		MallBuyGotInfo mallBuyGotInfo;
	};

	/**
	*@brief world->client 推送商城商品
	*/
	class WorldPushMallItems : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PUSH_MALL_ITEMS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mallItems;
		}

	public:
		MallItemInfosVec mallItems;
	};

	/**
	*@brief client->world 获取充值推送道具列表请求
	*/
	class WorldGetRechargePushItemsReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GET_RECHARGE_PUSH_ITEMS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world->client 获取充值推送道具列表返回
	*/
	class WorldGetRechargePushItemsRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GET_RECHARGE_PUSH_ITEMS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & itemVec;
		}

		// 错误码
		UInt32 retCode;
		// 推送道具
		RechargePushItemVec itemVec;
	};

	/**
	*@brief client->world 购买充值推送道具请求
	*/
	class WorldBuyRechargePushItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BUY_RECHARGE_PUSH_ITEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & pushId;
		}

		UInt32 pushId;
	};

	/**
	*@brief world->client 购买充值推送道具返回
	*/
	class WorldBuyRechargePushItemRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BUY_RECHARGE_PUSH_ITEM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & pushId & itemVec;
		}

		// 错误码
		UInt32 retCode;
		// 推送ID
		UInt32 pushId;
		// 推送道具
		RechargePushItemVec itemVec;
	};

	/**
	*@brief world->scene or center 检测充值推送请求
	*/
	class WorldCheckRechargePushReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CHECK_RECHARGE_PUSH_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & zoneId & roleId & pushId & lvLower & lvToplimit & vipLvLower & judgePropIDs & daysBeforMin & daysBeforeMax & recent & parameterType & parameter1 & parameter2;
		}

		UInt32 zoneId;
		UInt64 roleId;
		UInt32 pushId;
		UInt32 lvLower;
		UInt32 lvToplimit;
		UInt32 vipLvLower;
		std::vector<UInt32> judgePropIDs;
		UInt32 daysBeforMin;
		UInt32 daysBeforeMax;
		UInt32 recent;
		UInt32 parameterType;
		UInt32 parameter1;
		UInt32 parameter2;
	};

	/**
	*@brief scene or center->world 检测充值推送返回
	*/
	class WorldCheckRechargePushRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CHECK_RECHARGE_PUSH_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & roleId & pushId & zoneId;
		}

		// 错误码
		UInt32 retCode;
		UInt64 roleId;
		UInt32 pushId;
		UInt32 zoneId;
	};

	/**
	*@brief world->scene 场景购买充值推送请求
	*/
	class WorldSceneBuyRechargePushItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SCENE_BUY_RECHARGE_PUSH_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & pushId & price & item;
		}

		UInt64 roleId;
		UInt32 pushId;
		UInt32 price;
		ItemReward item;
	};

	/**
	*@brief world->scene 场景购买充值推送道具返回
	*/
	class WorldSceneBuyRechargePushItemRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SCENE_BUY_RECHARGE_PUSH_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & roleId & pushId;
		}

		// 错误码
		UInt32 retCode;
		UInt64 roleId;
		UInt32 pushId;
	};


	/**
	*@brief client->scene 限定商城套装情况查询
	*/
	class SceneMallFashionLimitedSuitStatusReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_MALL_FASHION_LIMITED_SUIT_STATUS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			
		}
	};

	/**
	*@brief client->scene 限定商城套装情况返回
	*/
	class SceneMallFashionLimitedSuitStatusRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_MALL_FASHION_LIMITED_SUIT_STATUS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}
		//已有套装中道具id
		std::vector<UInt32> result;
	};

}

#endif