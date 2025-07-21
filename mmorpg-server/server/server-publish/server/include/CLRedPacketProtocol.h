#ifndef __CL_RED_PACKET_PROTOCOL_H__
#define __CL_RED_PACKET_PROTOCOL_H__

#include <AvalonProtocol.h>
#include "CLProtocolDefine.h"
#include "CLRedPacketDefine.h"

namespace CLProtocol
{
	/**
	*@brief world->client 登录时同步红包基础信息
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
	*@brief world->client 通知获得新红包
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
	*@brief world->client 通知有新红包可领
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
	*@brief world->client 通知删除红包
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
	*@brief world->client 通知修改红包状态
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
		// 状态
		UInt8			status;
	};

	/**
	*@brief client->world 请求发红包
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
	*@brief world->client 发红包返回
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
	*@brief client->world 请求打开红包(如果已经打开过了，那就是查看)
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
	*@brief world->client 返回打开红包请求
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
		// 返回
		UInt32	result;
		// 红包详细信息
		RedPacketDetail detail;
	};

	/**
	*@brief scene->world 请求添加红包
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
		// 角色ID
		ObjID_t	id;
		// 来源
		UInt16	reason;
	};

	/**
	*@brief client->world 请求发自费红包
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
	*@brief world->client 请求发自费红包
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
	*@brief world->client 同步红包记录
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
	*@brief client->world 获取公会红包信息请求
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
	*@brief world->client 获取公会红包信息返回
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