#ifndef _CL_TRANSFER_PROTOCOL_H_
#define _CL_TRANSFER_PROTOCOL_H_

#include <AvalonPacket.h>
#include <CLProtocolDefine.h>
#include <CLRelationDefine.h>
#include <CLMatchDefine.h>

namespace CLProtocol
{
	/**
	*@brief �����ѯ���װ��tips
	*/
	class TransferQueryPlayerTipsOne : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(TRANSFER_QUERY_PLAYER_TIPS_ONE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & id;
		}
	public:
		TransferQueryPlayerTipsOne() : type(0), id(0) {}

		UInt8 type;
		ObjID_t	id;
	};

	/**
	*@brief world->node->world �����ѯ���װ��tips
	*/
	class TransferQueryPlayerTipsTwo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(TRANSFER_QUERY_PLAYER_TIPS_TWO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & (*packet);
		}

		TransferQueryPlayerTipsTwo() { packet = Avalon::Packet::Create(); }
		TransferQueryPlayerTipsTwo(Avalon::Packet* pack) { packet = pack; }
		~TransferQueryPlayerTipsTwo() { Avalon::Packet::Destroy(packet); }

		void Detach() { packet = NULL; }

	public:

		Avalon::Packet* packet;
	};

	/**
	*@brief world->node->scene �����ѯ�����Ϣ
	*/
	class TransferQueryPlayerOne : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(TRANSFER_QUERY_PLAYER_ONE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief scene->world->node �����ѯ�����Ϣ
	*/
	class TransferQueryPlayerTwo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(TRANSFER_QUERY_PLAYER_TWO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & watchInfo;
		}
		PlayerWatchInfo watchInfo;
	};

	/**
	*@brief world->node->scene ��ѯһ�������ϸ��Ϣ
	*/
	class TransferQueryPlayerDetailOne : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(TRANSFER_QUERY_PLAYER_DETAIL_ONE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief scene->world->node ��ѯһ�������ϸ��Ϣ
	*/
	class TransferQueryPlayerDetailTwo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(TRANSFER_QUERY_PLAYER_DETAIL_TWO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

		RacePlayerInfo	info;
	};
}

#endif // !_CL_TRANSFER_PROTOCOL_H_
