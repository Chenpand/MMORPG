#ifndef _CS_CHALLENGE_HANDLER_H_
#define _CS_CHALLENGE_HANDLER_H_

#include <CLNetwork.h>

class WorldChallengeEnterSceneReqFunction : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const;
};


#endif
