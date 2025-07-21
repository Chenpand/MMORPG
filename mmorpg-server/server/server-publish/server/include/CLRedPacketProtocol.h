#ifndef __CL_RED_PACKET_PROTOCOL_H__
#define __CL_RED_PACKET_PROTOCOL_H__

#include <AvalonProtocol.h>
#include "CLProtocolDefine.h"
#include "CLRedPacketDefine.h"

namespace CLProtocol
{
	/**
	*@brief world->client ��¼ʱͬ�����������Ϣ
	*/
	class WorldSyncRedPacket : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_REDPACKET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & entrys;
		}

	public:
		std::vector<RedPacketBaseEntry> entrys;
	};

	/**
	*@brief world->client ֪ͨ����º��
	*/
	class WorldNotifyGotNewRedPacket : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_GET_NEW_REDPACKET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & entry;
		}

	public:
		RedPacketBaseEntry	entry;
	};

	/**
	*@brief world->client ֪ͨ���º������
	*/
	class WorldNotifyNewRedPacket : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_NEW_REDPACKET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & entrys;
		}

	public:
		std::vector<RedPacketBaseEntry>	entrys;
	};

	/**
	*@brief world->client ֪ͨɾ�����
	*/
	class WorldNotifyDelRedPacket : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_DEL_REDPACKET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & redPacketList;
		}

	public:
		std::vector<ObjID_t>			redPacketList;
	};

	/**
	*@brief world->client ֪ͨ�޸ĺ��״̬
	*/
	class WorldNotifySyncRedPacketStatus : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_REDPACKET_SYNC_STATUS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & status;
		}

	public:
		ObjID_t			id;
		// ״̬
		UInt8			status;
	};

	/**
	*@brief client->world ���󷢺��
	*/
	class WorldSendRedPacketReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_REDPACKET_SEND_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & num;
		}

	public:
		ObjID_t			id;
		UInt8			num;
	};

	/**
	*@brief world->client ���������
	*/
	class WorldSendRedPacketRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_REDPACKET_SEND_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

	public:
		UInt32	result;
	};

	/**
	*@brief client->world ����򿪺��(����Ѿ��򿪹��ˣ��Ǿ��ǲ鿴)
	*/
	class WorldOpenRedPacketReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_REDPACKET_OPEN_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

	public:
		ObjID_t			id;
	};

	/**
	*@brief world->client ���ش򿪺������
	*/
	class WorldOpenRedPacketRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_REDPACKET_OPEN_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & detail;
		}

	public:
		// ����
		UInt32	result;
		// �����ϸ��Ϣ
		RedPacketDetail detail;
	};

	/**
	*@brief scene->world ������Ӻ��
	*/
	class SceneAddRedPacketReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ADD_RED_PACEKT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & reason;
		}

	public:
		// ��ɫID
		ObjID_t	id;
		// ��Դ
		UInt16	reason;
	};

	/**
	*@brief client->world �����ԷѺ��
	*/
	class WorldSendCustomRedPacketReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_REDPACKET_SEND_CUSTOM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & reason & name & num;
		}

		UInt16	reason;
		std::string name;
		UInt8	num;
	};
	
	/**
	*@brief world->client �����ԷѺ��
	*/
	class WorldSendCustomRedPacketRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_REDPACKET_SEND_CUSTOM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & redPacketId;
		}

		UInt32	result;
		ObjID_t	redPacketId;
	};

	/**
	*@brief world->client ͬ�������¼
	*/
	class WorldSyncRedPacketRecord : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_REDPACKET_SYNC_RECORD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & adds & dels & updates;
		}

		std::vector<RedPacketRecord> adds;
		std::vector<ObjID_t> dels;
		std::vector<RedPacketRecord> updates;
	};

	/**
	*@brief client->world ��ȡ��������Ϣ����
	*/
	class WorldGetGuildRedPacketInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_REDPACKET_GUILD_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
		
	};



	/**
	*@brief world->client ��ȡ��������Ϣ����
	*/
	class WorldGetGuildRedPacketInfoRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_REDPACKET_GUILD_INFO_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & infos;
		}
		UInt32									code;
		std::vector<GuildRedPacketSpecInfo>		infos;
	};
};

#endif