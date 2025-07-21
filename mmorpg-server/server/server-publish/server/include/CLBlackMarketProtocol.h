#ifndef _CL_BLACKMARKET_PROTOCOL_H_
#define _CL_BLACKMARKET_PROTOCOL_H_

#include <AvalonProtocol.h>
#include "CLProtocolDefine.h"
#include "CLBlackMarketDefine.h"

namespace CLProtocol
{
	/**
	*@brief world->client ͬ������
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
		//������������(ö��BLACKMARKET_TYPE)
		UInt8	type;
	};

	/**
	*@brief client->world �������б�
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
	*@brief world->client �������б���
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
		//�����б�
		std::vector<BlackMarketAuctionInfo>    items;
	};

	/**
	*@brief client->world ������
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
		//����item��guid
		UInt64 auction_guid;
		//װ��id
		UInt64 item_guid;
		//���ļ۸�
		UInt32	auction_price;
	};

	/**
	*@brief world->client ���ķ���
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
	*@brief client->world ȡ����������
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
		//����item��guid
		UInt64 auction_guid;
	};

	/**
	*@brief world->client ȡ�����ķ���
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
	*@brief world->client ֪ͨˢ��
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