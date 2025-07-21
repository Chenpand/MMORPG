#ifndef _CL_WORLD_MONOPOLY_PROTOCOL_H_
#define _CL_WORLD_MONOPOLY_PROTOCOL_H_

#include "CLProtocolDefine.h"

namespace CLProtocol
{
	/**
	*@brief client->world ��ѯӲ��
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
	*@brief world->client ��ѯ��ѯӲ�ҷ���
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
	*@brief client->world ��ѯ���̵�ͼ״̬
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
	*@brief world->client ��ѯ���̵�ͼ״̬����
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
	*@brief client->world ����roll����
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
	*@brief world->client ����roll���ӷ���
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
	*@brief ����ף�����һ�
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
	*@brief ����ף�����һ�����
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
	*@brief ����ӵ�е�ף�����б�
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
	*@brief ����ӵ�е�ף�����б���
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
	*@brief �������ף����
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
	*@brief �������ף��������
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
	*@brief ֪ͨ�ͻ��˸����¼����
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
	*@brief ����ף�����Ѷһ��б�
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
	*@brief ����ף�����Ѷһ��б���
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
	*@brief ֪ͨscene���roll�˴��̻
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
	*@brief ֪ͨscene���roll�˴��̻
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