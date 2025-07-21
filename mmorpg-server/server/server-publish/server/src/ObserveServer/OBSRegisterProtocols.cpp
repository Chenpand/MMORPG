#include "OBSInternalNetwork.h"
#include "ObserveHandler.h"

#include <CLObserveProtocol.h>

template<typename ProtocolT>
class OBSPacketFunction : public CLPacketFunction
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

#define OBS_REGISTER_PROTOCOL(ProtocolT) \
	RegisterProtocol(ProtocolT::__ID,new OBSPacketFunction<ProtocolT>)

void OBSInternalNetwork::RegisterProtocols()
{
	OBS_REGISTER_PROTOCOL(CLProtocol::SysTransmitRaceProtocolInNode);
	OBS_REGISTER_PROTOCOL(CLProtocol::UnionChampionObserveReq);
	OBS_REGISTER_PROTOCOL(CLProtocol::ObserveRealyAddObservePlayerRes);
	OBS_REGISTER_PROTOCOL(CLProtocol::ObserveCancelOverload);
	OBS_REGISTER_PROTOCOL(CLProtocol::ObserveStartRace);
	OBS_REGISTER_PROTOCOL(CLProtocol::ObserveRealyCreateRaceRes);
	OBS_REGISTER_PROTOCOL(CLProtocol::ObserveSyncFrame);
	OBS_REGISTER_PROTOCOL(CLProtocol::RelayAbalitySync);
}