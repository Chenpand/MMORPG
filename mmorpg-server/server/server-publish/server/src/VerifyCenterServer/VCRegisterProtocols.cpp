#include "VCInternalNetwork.h"
#include "VerifyCenterHandler.h"

template<typename ProtocolT>
class VCPacketFunction : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const
	{
		ProtocolT protocol;
		if (protocol.Decode(packet))
		{
			HandleProtocol(protocol);
		}
	}
};

#define VS_REGISTER_PROTOCOL(ProtocolT) \
	RegisterProtocol(ProtocolT::__ID, new VCPacketFunction<ProtocolT>)

void VCInternalNetwork::RegisterProtocols()
{
	VS_REGISTER_PROTOCOL(CLProtocol::VerifyCenterRegisterReq);
	VS_REGISTER_PROTOCOL(CLProtocol::VerifyCenterReportAbility);
}