#include <CLProtocolDefine.h>

#include "GASInternalNetwork.h"
#include "GASSysHandler.h"
#include "GASActivityHandler.h"
#include "GASBetHorseHandler.h"
#include "GASHireHandler.h"

template<typename ProtocolT>
class GASPacketFunction : public CLPacketFunction
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

#define GAS_REGISTER_PROTOCOL(ProtocolT) \
	RegisterProtocol(ProtocolT::__ID, new GASPacketFunction<ProtocolT>());


void GASInternalNetwork::RegisterProtocols()
{
	GAS_REGISTER_PROTOCOL(CLProtocol::SysTransmitPlayerFormNodesToGlobal)

	GAS_REGISTER_PROTOCOL(CLProtocol::GASActivityInfoReq)

	GAS_REGISTER_PROTOCOL(CLProtocol::GASGMCommand)

	GAS_REGISTER_PROTOCOL(CLProtocol::GASBetHorseMailRewardRes)

	GAS_REGISTER_PROTOCOL(CLProtocol::GASCheckHireBindReq)

	GAS_REGISTER_PROTOCOL(CLProtocol::WorldQueryHireAlreadyBindReq)
		
	GAS_REGISTER_PROTOCOL(CLProtocol::GASWholeBargainDiscountQuery)
}


