#if 0
#include "RLPlayerMgr.h"
#include "CLRelayServerProtocol.h"
#include "CLPacketExecutor.h"

class RLPlayer;

template<typename ProtocolT>
class RLPlayerPacketFunction : public CLParamPacketFunction<RLPlayer*>
{
public:
	void operator()(Avalon::Packet* packet, RLPlayer* player) const
	{
		ProtocolT protocol;
		if (protocol.Decode(packet))
		{
			HandleProtocol(player, protocol); //
		}
	}
};


#define RLSVR_REGISTER_PROTOCOL(ProtocolT) \
	m_Executor.Bind(ProtocolT::__ID,new RLPlayerPacketFunction<ProtocolT>);

void RLPlayerMgr::RegisterProtocols()
{
	RLSVR_REGISTER_PROTOCOL(CLProtocol::RelaySvrFrameChecksumRequest)

	return;
}

#endif