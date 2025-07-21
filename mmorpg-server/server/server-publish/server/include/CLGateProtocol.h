#ifndef _CL_GATE_PROTOCOL_H_
#define _CL_GATE_PROTOCOL_H_

#include <AvalonPacket.h>
#include "CLProtocolDefine.h"

namespace CLProtocol
{
	/**
	 *@brief scene,world,->gate
	 *      ���� guid+packetid+���ݵ���ʽ
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

		//���id
		ObjID_t id;
		//ת������Ϣ��
		Avalon::Packet* packet;
	};

	/**
	 *@brief scene->gate �㲥��Ϣ������Χ
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

		//����id
		UInt32	sceneid;
		//λ��
		UInt32	grid;
		//�ų�ָ�����
		ObjID_t	exceptobj;
		// Ŀ������б�
		std::vector<ObjID_t> targetObjList;
		//��Ϣ��
		Avalon::Packet* packet;
	};

	/**
	 *@brief scene->gate �㲥��Ϣ��������
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
		//����id
		UInt32 sceneid;
		//����1
		UInt32 grid1;
		//����2
		UInt32 grid2;
        //�ų�ָ�����
        ObjID_t	exceptobj;
		// Ŀ������б�
		std::vector<ObjID_t> targetObjList;
		//��Ϣ��
		Avalon::Packet* packet;
	};

	/**
	 *@brief server->gate �㲥��������� 
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
		//�Ƿ�ֻ�����������
		UInt8	bZone;
		//��͵ȼ�
		UInt16	level;
		//��Ϣ��
		Avalon::Packet* packet;
	};

	/**
	 *@brief server->gate ͬ����һ�����Ϣ��gate
	 */
	class GateSyncPlayerBaseInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_SYNC_PLAYER_BASEINFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & level;
		}

		//���id
		ObjID_t	playerId;
		//��ҵȼ�
		UInt16	level;
	};
}

#endif
