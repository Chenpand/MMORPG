#include "VSInternalNetwork.h"
#include "VerifyHandler.h"
#include "VerifyCenterHandler.h"

template<typename ProtocolT>
class VSPacketFunction : public CLPacketFunction
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
	RegisterProtocol(ProtocolT::__ID, new VSPacketFunction<ProtocolT>)

void VSInternalNetwork::RegisterProtocols()
{
	VS_REGISTER_PROTOCOL(CLProtocol::VerifyCenterRegisterRes);
	VS_REGISTER_PROTOCOL(CLProtocol::VerifyCreateTaskReq);
	VS_REGISTER_PROTOCOL(CLProtocol::VerifyReportFrame);
	VS_REGISTER_PROTOCOL(CLProtocol::VerifyReportChecksum);
	VS_REGISTER_PROTOCOL(CLProtocol::VerifyReportRaceEnd);
	VS_REGISTER_PROTOCOL(CLProtocol::VerifyReportRaceDismiss);
}