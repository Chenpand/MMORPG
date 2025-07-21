#include <CLProtocolDefine.h>

#include "SysHandler.h"
#include "DPNetwork.h"


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


#define REGISTER_PROTOCOL(ProtocolT) \
	RegisterProtocol(ProtocolT::__ID, new PacketFunction<ProtocolT>());


void Network::RegisterProtocols()
{
	REGISTER_PROTOCOL(CLProtocol::SysTransmitInNodes)
}

