#ifndef __OBSERVE_PROTOCOL_H__
#define __OBSERVE_PROTOCOL_H__


#include "CLProtocolDefine.h"
#include <AvalonPacket.h>
#include <AvalonSocket.h>
#include <CLRelayServerProtocol.h>

namespace CLProtocol
{
	/**
	*@brief Client->Scene �����ս
	*/
	class SceneChampionObserveReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_OBSERVE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & raceID;
		}
	public:
		UInt64 raceID;
	};


	/**
	*@brief Scene->Client �����ս����
	*/
	class SceneChampionObserveRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_OBSERVE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleID & raceID & addr;
		}
	public:
		UInt64 roleID;
		UInt64 raceID;
		Avalon::SockAddr	addr;
	};


	/**
	*@brief Scene->Union �����ս
	*/
	class UnionChampionObserveReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(UNION_CHAMPION_OBSERVE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & raceID & roleID;
		}
	public:
		UInt64 raceID;
		UInt64 roleID;
	};

	/**
	*@brief Observe->ObserveRealy �������ӹ�ս���
	*/
	class ObserveRealyAddObservePlayerReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(OBSERVEREALY_ADD_OBSERVE_PLAYER_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & raceID & roleID;
		}
	public:
		UInt64 raceID;
		UInt64 roleID;
	};

	/**
	*@brief ObserveRealy->Observe �������ӹ�ս��ҷ���
	*/
	class ObserveRealyAddObservePlayerRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(OBSERVEREALY_ADD_OBSERVE_PLAYER_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & raceID & roleID & addr;
		}
	public:
		UInt64 raceID;
		UInt64 roleID;
		Avalon::SockAddr	addr;
	};

	/**
	*@brief Observe->ObserveRealy ���󴴽�����
	*/
	class ObserveRealyCreateRaceReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(OBSERVEREALY_CREATE_RACE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & raceID & *packet & frames;
		}
		ObserveRealyCreateRaceReq()
		{
			packet = Avalon::Packet::Create();
		}

		virtual ~ObserveRealyCreateRaceReq()
		{
			if (packet)
			{
				Avalon::Packet::Destroy(packet);
			}
		}

		void Detach()
		{
			packet = NULL;
		}
	public:
		UInt64 raceID;
		// ��ʼ����
		Avalon::Packet*	packet;
		std::vector<Frame>	frames;
	};

	/**
	*@brief ObserveRealy->Observe ���󴴽���������
	*/
	class ObserveRealyCreateRaceRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(OBSERVEREALY_CREATE_RACE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & raceID & result;
		}
	public:
		UInt64 raceID;
		UInt32 result;
	};

	/**
	*@brief Realy->Observe ֪ͨObserve�ڱ�relay�Ͽ�ʼ���µı���
	*/
	class ObserveStartRace : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(OBSERVE_START_RACE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & raceID &*packet;
		}
		ObserveStartRace()
		{
			packet = Avalon::Packet::Create();
		}

		virtual ~ObserveStartRace()
		{
			if (packet)
			{
				Avalon::Packet::Destroy(packet);
			}
		}

		void Detach()
		{
			packet = NULL;
		}
	public:
		UInt64 raceID;
		// ��ʼ����
		Avalon::Packet*	packet;
	};

	/**
	*@brief Observe->Realy �������relay�ϱ�����֡����
	*/
	class RelayStartReceiveRaceFrame : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RELAY_START_RECEIVE_RACE_FRAME)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & raceID;
		}
	public:
		UInt64 raceID;
	};

	/**
	*@brief Realy->Observe ͬ������֡����
	*/
	class ObserveSyncFrame : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(OBSERVE_SYNC_FRAME)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & raceId & frames;
		}

		ObjID_t				raceId;
		std::vector<Frame>	frames;
	};

	/**
	*@brief ObserveRealy->Observe ȡ��һ������
	*/
	class ObserveCancelOverload : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(OBSERVE_CANCEL_OVERLOAD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	};
}




#endif // 
