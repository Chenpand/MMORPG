#ifndef _CS_CHALLENGE_HANDLER_H_
#define _CS_CHALLENGE_HANDLER_H_

#include <CLNetwork.h>
#include <CLWorldChallengeProtocol.h>

class WorldChallengeSigninFunction : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const;
};

void HandleProtocol(Protocol::ChallengeWorldChallengeSignout& protocol);

void HandleProtocol(Protocol::ChallengeWorldChallengeInfoReq& protocol);

void HandleProtocol(Protocol::ChallengeWorldChallengeSortListReq& protocol);

void HandleProtocol(Protocol::ChallengeWorldChallengeGetAwardReq& protocol);

#endif
