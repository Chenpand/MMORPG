#ifndef _CL_WORLD_MONOPOLY_PROTOCOL_H_
#define _CL_WORLD_MONOPOLY_PROTOCOL_H_

#include "CLProtocolDefine.h"

namespace CLProtocol
{
	/**
	*@brief client->world 查询硬币
	*/
	class WorldMonopolyCoinReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MONOPOLY_COIN_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}

	public:

	};

	/**
	*@brief world->client 查询查询硬币返回
	*/
	class WorldMonopolyCoinRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MONOPOLY_COIN_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & num;
		}

	public:
		UInt32 num;
	};

	/**
	*@brief client->world 查询大富翁地图状态
	*/
	class WorldMonopolyStatusReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MONOPOLY_STATUS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}

	public:

	};

	/**
	*@brief world->client 查询大富翁地图状态返回
	*/
	class WorldMonopolyStatusRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MONOPOLY_STATUS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & turn & currentGrid & rollTimes & buff;
		}

	public:
		UInt32 turn;
		UInt32 currentGrid;
		UInt32 rollTimes;
		UInt32 buff;
	};

	/**
	*@brief client->world 请求roll骰子
	*/
	class WorldMonopolyRollReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MONOPOLY_ROLL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}

	public:

	};

	/**
	*@brief world->client 请求roll骰子返回
	*/
	class WorldMonopolyRollRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MONOPOLY_ROLL_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & errorCode & step;
		}

	public:
		UInt32 errorCode;
		UInt32 step;
	};

	/**
	*@brief 请求祝福卡兑换
	*/
	class WorldMonopolyCardExchangeReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MONOPOLY_CARD_EXCHANGE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

	public:
		UInt32 id;
	};

	
	/**
	*@brief 请求祝福卡兑换返回
	*/
	class WorldMonopolyCardExchangeRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MONOPOLY_CARD_EXCHANGE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & errorCode;
		}

	public:
		UInt32 errorCode;
	};


	/**
	*@brief 请求拥有的祝福卡列表
	*/
	class WorldMonopolyCardListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MONOPOLY_CARD_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			
		}

	public:
		
	};

	struct MonpolyCard
	{
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & num;
		}
		UInt32 id;
		UInt32 num;
	};


	/**
	*@brief 请求拥有的祝福卡列表返回
	*/
	class WorldMonopolyCardListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MONOPOLY_CARD_LIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & cardList;
		}

	public:
		std::vector<MonpolyCard> cardList;
	};


	/**
	*@brief 请求出售祝福卡
	*/
	class WorldMonopolySellCardReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MONOPOLY_SELL_CARD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & num;
		}

	public:
		UInt32 id;
		UInt32 num;
	};

	/**
	*@brief 请求出售祝福卡返回
	*/
	class WorldMonopolySellCardRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MONOPOLY_SELL_CARD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & errorCode;
		}

	public:
		UInt32 errorCode;
	};

	/**
	*@brief 通知客户端格子事件结果
	*/
	class WorldMonopolyNotifyResult : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MONOPOLY_NOTIFY_RESULT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & resultId & param;
		}

	public:
		UInt32 resultId;
		UInt64 param;
	};

	/**
	*@brief 请求祝福卡已兑换列表
	*/
	class WorldMonopolyCardExchangedListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MONOPOLY_CARD_EXCHANGED_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}

	public:

	};

	/**
	*@brief 请求祝福卡已兑换列表返回
	*/
	class WorldMonopolyCardExchangedListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MONOPOLY_CARD_EXCHANGED_LIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & cardList;
		}

	public:
		std::vector<MonpolyCard> cardList;
	};

	/**
	*@brief 通知scene玩家roll了大富翁活动
	*/
	class SceneMonopolyPlayerRollNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCEME_ON_PLAYER_MONOPOLY_ROLL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId;
		}

	public:
		UInt64 roleId;
	};	
	
	/**
	*@brief 通知scene玩家roll了大富翁活动
	*/
	class WorldMonopolyGMJoinDungeon : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MONOPOLY_GM_JOIN_DUNGEON)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & dungeonId;
		}

	public:
		UInt64 roleId;
		UInt32 dungeonId;
	};
	

}

#endif