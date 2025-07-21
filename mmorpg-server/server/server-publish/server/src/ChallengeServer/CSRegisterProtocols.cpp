#include <CLProtocolDefine.h>

#include "CSNetwork.h"

#include "CSSceneHandler.h"
#include "CSChallengeHandler.h"

template<typename ProtocolT>
class CSPacketFunction : public CLPacketFunction
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

#define CS_REGISTER_PROTOCOL(ProtocolT) \
	RegisterProtocol(ProtocolT::__ID,new CSPacketFunction<ProtocolT>());


void CSNetwork::RegisterProtocols()
{
	CS_REGISTER_PROTOCOL(Protocol::WorldRegisterSceneReq)
	CS_REGISTER_PROTOCOL(Protocol::WorldUnregisterSceneReq)

	RegisterProtocol(Protocol::CHALLENGE_WORLDCHALLENGE_SIGNIN_REQ, new WorldChallengeSigninFunction());
	CS_REGISTER_PROTOCOL(Protocol::ChallengeWorldChallengeSignout)
	CS_REGISTER_PROTOCOL(Protocol::ChallengeWorldChallengeInfoReq)
	CS_REGISTER_PROTOCOL(Protocol::ChallengeWorldChallengeSortListReq)
	CS_REGISTER_PROTOCOL(Protocol::ChallengeWorldChallengeGetAwardReq)
}

