#ifndef _CL_SHOP_PROTOCOL_H_
#define _CL_SHOP_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLItemDefine.h"

namespace CLProtocol
{
	/**
	*@brief 账号商店查询索引
	*/
	struct AccountShopQueryIndex
	{
		AccountShopQueryIndex() : shopId(0), tabType(0), jobType(0) {}
		AccountShopQueryIndex(UInt32 shopid, UInt8 tabtype, UInt8 jobtype)
		: shopId(shopid), tabType(tabtype), jobType(jobtype) {}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & shopId & tabType & jobType;
		}

		// 查询的商店id
		UInt32 shopId;
		// 查询的页签类别
		UInt8 tabType;
		// 查询的职业类别
		UInt8 jobType;
	};

	/**
	*@brief 账号商店商品信息
	*/
	struct AccountShopItemInfo
	{
		AccountShopItemInfo() :shopItemId(0), tabType(0), jobType(0), itemDataId(0), itemNum(0), accountRefreshType(0),
		accountLimitBuyNum(0), accountRestBuyNum(0), roleRefreshType(0), roleLimitBuyNum(0), roleRestBuyNum(0), extensibleCondition(0),
		needPreviewFunc(0)
		{}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & shopItemId & shopItemName & tabType & jobType & itemDataId & itemNum & costItems & accountRefreshType
				& accountRefreshTimePoint & accountLimitBuyNum & accountRestBuyNum & accountBuyRecordNextRefreshTimestamp
				& roleRefreshType & roleRefreshTimePoint & roleBuyRecordNextRefreshTimestamp & roleLimitBuyNum & roleRestBuyNum 
				& extensibleCondition & needPreviewFunc;
		}

		// 商品id
		UInt32 shopItemId;
		// 商品名称
		std::string shopItemName;
		// 页签类别
		UInt8 tabType;
		// 职业类别
		UInt8 jobType;
		// 上架道具
		UInt32 itemDataId;
		// 上架数量
		UInt32 itemNum;
		// 购买消耗道具
		ItemRewardVec costItems;

		// 账号刷新类型(TimeUtil::RefreshType)
		UInt8 accountRefreshType;
		// 账号刷新时间点
		std::string accountRefreshTimePoint;

		// 账号限购数量(0:不限购)
		UInt32 accountLimitBuyNum;
		// 账号剩余购买数量
		UInt32 accountRestBuyNum;
		// 账号购买记录下次刷新时间
		UInt32 accountBuyRecordNextRefreshTimestamp;

		// 角色刷新类型(TimeUtil::RefreshType)
		UInt8 roleRefreshType;
		// 角色刷新时间点
		std::string roleRefreshTimePoint;
		// 角色购买记录下次刷新时间
		UInt32 roleBuyRecordNextRefreshTimestamp;

		// 角色限购数量(0:不限购)
		UInt32 roleLimitBuyNum;
		// 角色剩余购买数量
		UInt32 roleRestBuyNum;

		// 扩展条件
		UInt32 extensibleCondition;
		// 是否需要预览按钮
		UInt8 needPreviewFunc;
	};

	/**
	*@brief client->world 账号商店商品查询请求
	*/
	class WorldAccountShopItemQueryReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ACCOUNT_SHOP_ITEM_QUERY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & queryIndex;
		}

	public:
		// 查询索引
		AccountShopQueryIndex queryIndex;
	};

	/**
	*@brief world->client 账号商店商品查询返回
	*/
	class WorldAccountShopItemQueryRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ACCOUNT_SHOP_ITEM_QUERY_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & resCode & queryIndex & shopItems & nextGroupOnSaleTime;
		}

	public:
		// 错误码
		UInt32 resCode;
		// 查询索引
		AccountShopQueryIndex queryIndex;
		// 商品集
		std::vector<AccountShopItemInfo> shopItems;
		// 下一波商品上架时间
		UInt32 nextGroupOnSaleTime;
	};

	/**
	*@brief client->world 账号商店商品购买请求
	*/
	class WorldAccountShopItemBuyReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ACCOUNT_SHOP_ITEM_BUY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & queryIndex & buyShopItemId & buyShopItemNum;
		}

	public:
		// 查询索引
		AccountShopQueryIndex queryIndex;
		// 购买的商品id
		UInt32 buyShopItemId;
		// 购买商品数量
		UInt32 buyShopItemNum;
	};

	/**
	*@brief world->client 账号商店商品购买返回
	*/
	class WorldAccountShopItemBuyRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ACCOUNT_SHOP_ITEM_BUY_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & resCode & queryIndex & buyShopItemId & buyShopItemNum 
				& accountRestBuyNum & roleRestBuyNum;
		}

	public:
		// 错误码
		UInt32 resCode;
		// 查询索引
		AccountShopQueryIndex queryIndex;
		// 购买的商品id
		UInt32 buyShopItemId;
		// 购买数量
		UInt32 buyShopItemNum;
		// 账号剩余购买数量
		UInt32 accountRestBuyNum;
		// 角色剩余购买数量
		UInt32 roleRestBuyNum;
	};

	/**
	*@brief world->client 账号商店商品购买返回
	*/
	class WorldAccountShopItemBuyedNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ACCOUNT_SHOP_ITEM_BUYED_NOTIFY_SCENE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & shopId & shopItemId & shopItemNum;
		}

	public:
		//角色id
		ObjID_t playerId;
		// 商店id
		UInt32 shopId;
		// 购买的商品id
		UInt32 shopItemId;
		// 购买数量
		UInt32 shopItemNum;
	};
};

#endif
