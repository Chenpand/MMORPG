#include <CLProtocolDefine.h>

#include "Network.h"
#include "SortListHandler.h"

#include <CLSysProtocol.h>


template<typename ProtocolT>
class PacketFunction : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const
	{
		ProtocolT protocol;
		if(protocol.Decode(packet))
		{
			HandleProtocol(protocol);
		}
	}
};

template<typename ProtocolT>
class PlayerPacketFunction : public CLParamPacketFunction<ObjID_t>
{
public:
	void operator()(Avalon::Packet* packet, ObjID_t playerId) const
	{
		ProtocolT protocol;
		if(protocol.Decode(packet))
		{
			HandleProtocol(playerId, protocol);
		}
	}
};

class TransmitPlayerProtocolFunction : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const
	{
		CLProtocol::SysTransmitPlayerProtocolInNode protocol;
		if(protocol.Decode(packet))
		{
			protocol.packet->SetConnID(protocol.GetConnID());
			Network::Instance()->ExecutePlayerPacket(protocol.packet, protocol.playerId);
		}
	}
};

#define REGISTER_PROTOCOL(ProtocolT) \
	RegisterProtocol(ProtocolT::__ID, new PacketFunction<ProtocolT>());

#define REGISTER_PLAYER_PROTOCOL(ProtocolT) \
	m_Executor.Bind(ProtocolT::__ID, new PlayerPacketFunction<ProtocolT>());

void Network::RegisterProtocols()
{
	RegisterPlayerProtocols();

	RegisterProtocol(CLProtocol::SYS_TRANSMIT_PLAYERPROTOCOL_INNODES, new TransmitPlayerProtocolFunction());
	REGISTER_PROTOCOL(CLProtocol::WorldSortListUpdateReq);
	REGISTER_PROTOCOL(CLProtocol::WorldSortListVisitReq);
}

void Network::RegisterPlayerProtocols()
{
	REGISTER_PLAYER_PROTOCOL(CLProtocol::WorldSortListReq);
	REGISTER_PLAYER_PROTOCOL(CLProtocol::WorldSortListNearReq);
}
