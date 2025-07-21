#ifndef _CL_TRADE_PROTOCOL_H_
#define _CL_TRADE_PROTOCOL_H_

#include "CLProtocolDefine.h"

namespace CLProtocol
{
	/**
	 *@brief server->client 通知交易开始
	 */
	class SceneTradeNotifyBegin : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TRADE_NOTIFY_BEGIN)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & otherName;
		}

	public:
		//对方姓名
		std::string	otherName;
	};

	/**
	 *@brief client->server 请求添加和移除道具
	 */
	class SceneTradeUpdateItemsReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TRADE_UPDATEITEMS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemid & num & type;
		}

	public:
		//道具唯一id
		ObjID_t	itemid;
		//道具数，移除时整体移除
		UInt16	num;
		//0为添加 1为移除
		UInt8	type;
	};

	/**
	 *@brief client->server 请求更新银币
	 */
	class SceneTradeUpdateSilverReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TRADE_UPDATESILVER_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & silver;
		}

	public:
		//银币数
		UInt32 silver;
	};

	/**
	 *@brief server->client 同步自己交易道具
	 */
	class SceneTradeSyncSelfItems : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TRADE_SYNC_SELFITEMS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & items;
		}

	public:
		//道具列表 id和数量
		std::map<ObjID_t,UInt16> items;
	};

	/**
	 *@brief server->client 通知对方增加道具 格式:objid + dataid(UInt32) + num(UInt16) + data(见CLItemProtocol.h) 
	 */
	class SceneTradeNotifyOtherAddItem : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TRADE_NOTIFY_OTHER_ADDITEM)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/** 
	 *@brief server->client 通知对方移除道具
	 */
	class SceneTradeNotifyOtherRemoveItem : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TRADE_NOTIFY_OTHER_REMOVEITEM)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemid;
		}

	public:
		//道具唯一id
		ObjID_t	itemid;
	};

	/**
	 *@brier server->client 同步银币数
	 */
	class SceneTradeSyncSilver : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TRADE_SYNC_SILVER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & selfSilver & otherSilver;
		}

	public:
		//己方银币
		UInt32	selfSilver;
		//对方银币
		UInt32	otherSilver;
	};

	/**
	 *@brief client->server 设置交易状态
	 */
	class SceneTradeSetStatus : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TRADE_SET_STATUS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & status;
		}

	public:
		//状态
		UInt8	status;
	};

	/**
	 *@brief server->client 同步交易状态
	 */
	class SceneTradeSyncStatus : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TRADE_SYNC_STATUS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & selfStatus & otherStatus;
		}

	public:
		//己方状态
		UInt8	selfStatus;
		//对方状态
		UInt8	otherStatus;
	};
};

#endif
