#ifndef _CL_GATE_PROTOCOL_H_
#define _CL_GATE_PROTOCOL_H_

#include <AvalonPacket.h>
#include "CLProtocolDefine.h"

namespace CLProtocol
{
	/**
	 *@brief scene,world,->gate
	 *      采用 guid+packetid+内容的形式
	 */
	class GateTransmitPacket : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_TRANSMIT_PACKET)
	public:
		GateTransmitPacket(Avalon::Packet* _packet){ packet = _packet; }
		GateTransmitPacket(){ packet = Avalon::Packet::Create(); }
		~GateTransmitPacket(){ Avalon::Packet::Destroy(packet); }

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & (*packet);		
		}

		void Detach(){ packet = NULL; }

		//玩家id
		ObjID_t id;
		//转发的消息包
		Avalon::Packet* packet;
	};

	/**
	 *@brief scene->gate 广播消息包到周围
	 */
	class GateBroadcastAround : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_BROADCAST_AROUND)
	public:
		GateBroadcastAround(Avalon::Packet* _packet){ packet = _packet; }
		GateBroadcastAround(){ packet = Avalon::Packet::Create(); }
		~GateBroadcastAround(){ Avalon::Packet::Destroy(packet); }

		void Detach(){ packet = NULL; }

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sceneid & grid & exceptobj & targetObjList & (*packet);
		}

		//场景id
		UInt32	sceneid;
		//位置
		UInt32	grid;
		//排除指定玩家
		ObjID_t	exceptobj;
		// 目标玩家列表
		std::vector<ObjID_t> targetObjList;
		//消息包
		Avalon::Packet* packet;
	};

	/**
	 *@brief scene->gate 广播消息到新区域
	 */
	class GateBroadcastExcept : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_BROADCAST_EXCEPT)
	public:
		GateBroadcastExcept(Avalon::Packet *_packet){ packet = _packet; }
		GateBroadcastExcept(){ packet = Avalon::Packet::Create(); }
		~GateBroadcastExcept(){ Avalon::Packet::Destroy(packet); }

		void Detach(){ packet = NULL; }

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sceneid & grid1 & grid2 & exceptobj & targetObjList & (*packet);
		}

	public:
		//场景id
		UInt32 sceneid;
		//网格1
		UInt32 grid1;
		//网格2
		UInt32 grid2;
        //排除指定玩家
        ObjID_t	exceptobj;
		// 目标玩家列表
		std::vector<ObjID_t> targetObjList;
		//消息包
		Avalon::Packet* packet;
	};

	/**
	 *@brief server->gate 广播到所有玩家 
	 */
	class GateBroadcastAll : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_BROADCAST_ALL)
	public:
		GateBroadcastAll(Avalon::Packet* pack){	packet = pack; level = 0; bZone = 0; }
		GateBroadcastAll(){ packet = Avalon::Packet::Create(); level = 0; bZone = 0; }
		~GateBroadcastAll(){ Avalon::Packet::Destroy(packet); }

		void Detach(){ packet = NULL; }

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & bZone & level & (*packet);
		}

	public:
		//是否只发给本服玩家
		UInt8	bZone;
		//最低等级
		UInt16	level;
		//消息包
		Avalon::Packet* packet;
	};

	/**
	 *@brief server->gate 同步玩家基本信息到gate
	 */
	class GateSyncPlayerBaseInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_SYNC_PLAYER_BASEINFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & level;
		}

		//玩家id
		ObjID_t	playerId;
		//玩家等级
		UInt16	level;
	};
}

#endif
