#include "ASInternalNetwork.h"
#include "ASExternalNetwork.h"

#include "ASSysHandler.h"
#include "ASLoginHandler.h"
#include "ASLoggerHandler.h"
#include "ASConsoleHandler.h"
#include "ASBillingHandler.h"

template<typename ProtocolT>
class ASPacketFunction : public CLPacketFunction
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

#define AS_REGISTER_PROTOCOL(ProtocolT) \
	RegisterProtocol(ProtocolT::__ID,new ASPacketFunction<ProtocolT>);

void ASInternalNetwork::RegisterProtocols()
{
	AS_REGISTER_PROTOCOL(CLProtocol::GateNetAddressRet)
	AS_REGISTER_PROTOCOL(CLProtocol::AdminSyncGatePlayerNum)
	AS_REGISTER_PROTOCOL(CLProtocol::AdminSyncWorldPlayerNum)
	AS_REGISTER_PROTOCOL(CLProtocol::AdminSyncLoginQueueInfo)

	AS_REGISTER_PROTOCOL(CLProtocol::SysTransmitInNodes)
	AS_REGISTER_PROTOCOL(CLProtocol::SysTransmitPlayerProtocolInNode)
	AS_REGISTER_PROTOCOL(CLProtocol::SysNetLogger)
	RegisterProtocol(CLProtocol::SYS_DB_LOGGER, new DBLoggerFunction());

	AS_REGISTER_PROTOCOL(CLProtocol::AdminConsoleResponse)

	AS_REGISTER_PROTOCOL(CLProtocol::WorldBillingGiveItemRet)
	AS_REGISTER_PROTOCOL(CLProtocol::AdminPlayerGateInfoRet)
	AS_REGISTER_PROTOCOL(CLProtocol::SysTransmitFromGlobalToNode)
	AS_REGISTER_PROTOCOL(CLProtocol::SysTransmitPlayerFormNodesToGlobal)
	AS_REGISTER_PROTOCOL(CLProtocol::SysTransmitInNodesType)
	AS_REGISTER_PROTOCOL(CLProtocol::SysTransmitPlayerToTeamCopy)
	AS_REGISTER_PROTOCOL(CLProtocol::SysTransmitPlayerToUnion)
	AS_REGISTER_PROTOCOL(CLProtocol::UnionNotifyOpenFunction)
	AS_REGISTER_PROTOCOL(CLProtocol::SysTransmitToUnion)
	AS_REGISTER_PROTOCOL(CLProtocol::SysTransferProtocol)
}

void ASExternalNetwork::RegisterProtocols()
{
	AS_REGISTER_PROTOCOL(CLProtocol::AdminLoginNewPlayer)
}

