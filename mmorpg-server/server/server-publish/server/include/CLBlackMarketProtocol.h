#ifndef _CL_BLACKMARKET_PROTOCOL_H_
#define _CL_BLACKMARKET_PROTOCOL_H_

#include <AvalonProtocol.h>
#include "CLProtocolDefine.h"
#include "CLBlackMarketDefine.h"

namespace CLProtocol
{
	/**
	*@brief world->client 同步类型
	*/
	class WorldBlackMarketSyncType : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BLACKMARKET_SYNC_TYPE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type;
		}
	public:
		//黑市商人类型(枚举BLACKMARKET_TYPE)
		UInt8	type;
	};

	/**
	*@brief client->world 请求竞拍列表
	*/
	class WorldBlackMarketAuctionListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BLACKMARKET_AUCTION_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	public:

	};

	/**
	*@brief world->client 请求竞拍列表返回
	*/
	class  WorldBlackMarketAuctionListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BLACKMARKET_AUCTION_LIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & items;
		}
	public:
		//竞拍列表
		std::vector<BlackMarketAuctionInfo>    items;
	};

	/**
	*@brief client->world 请求竞拍
	*/
	class WorldBlackMarketAuctionReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BLACKMARKET_AUCTION_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & auction_guid & item_guid & auction_price;
		}
	public:
		//竞拍item项guid
		UInt64 auction_guid;
		//装备id
		UInt64 item_guid;
		//竞拍价格
		UInt32	auction_price;
	};

	/**
	*@brief world->client 竞拍返回
	*/
	class WorldBlackMarketAuctionRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BLACKMARKET_AUCTION_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & item;
		}
	public:
		UInt32 code;
		BlackMarketAuctionInfo item;
	};

	/**
	*@brief client->world 取消竞拍请求
	*/
	class WorldBlackMarketAuctionCancelReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BLACKMARKET_AUCTION_CANCEL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & auction_guid;
		}
	public:
		//竞拍item项guid
		UInt64 auction_guid;
	};

	/**
	*@brief world->client 取消竞拍返回
	*/
	class WorldBlackMarketAuctionCancelRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BLACKMARKET_AUCTION_CANCEL_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & item;
		}
	public:
		UInt32 code;
		BlackMarketAuctionInfo item;
	};

	/**
	*@brief world->client 通知刷新
	*/
	class WorldBlackMarketNotifyRefresh : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BLACKMARKET_NOTIFY_REFRESH)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	protected:
	private:
	};
};

#endif