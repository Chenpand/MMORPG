#include "TFSNetwork.h"

#include <CLPacketExecutor.h>

#include "TransferSysHandler.h"

template<typename ProtocolT>
class TFPacketFunction : public CLPacketFunction
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

#define TFS_REGISTER_PROTOCOL(ProtocolT) \
	RegisterProtocol(ProtocolT::__ID,new TFPacketFunction<ProtocolT>());

void TFSNetwork::RegisterProtocols()
{
	TFS_REGISTER_PROTOCOL(CLProtocol::SysTransferProtocol);
}



