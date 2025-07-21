#include "RSInternalNetwork.h"
#include "ReplayHandler.h"

template<typename ProtocolT>
class RSPacketFunction : public CLPacketFunction
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

#define RS_REGISTER_PROTOCOL(ProtocolT) \
	RegisterProtocol(ProtocolT::__ID, new RSPacketFunction<ProtocolT>)

void RSInternalNetwork::RegisterProtocols()
{
	RS_REGISTER_PROTOCOL(CLProtocol::ReplayReportPlayerOnline);
	RS_REGISTER_PROTOCOL(CLProtocol::ReplayReportPlayerOffline);
	RS_REGISTER_PROTOCOL(CLProtocol::ReplayReportRace);
	RS_REGISTER_PROTOCOL(CLProtocol::ReplayListReq);
	RS_REGISTER_PROTOCOL(CLProtocol::ReplayReportView);
	//RS_REGISTER_PROTOCOL(CLProtocol::ReplayFileRes);
	RegisterProtocol(CLProtocol::ReplayFileRes::__ID, new ReplayFileResFunction);
	
}