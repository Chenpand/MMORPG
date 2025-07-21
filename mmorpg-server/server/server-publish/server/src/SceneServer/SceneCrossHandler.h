#ifndef _SCENE_WORLD_CHALLENGE_HANDLER_H_
#define _SCENE_WORLD_CHALLENGE_HANDLER_H_

#include <CLNetwork.h>
#include <CLCrossProtocol.h>
#include <CLSysProtocol.h>

/*
class Player;

void HandleProtocol(CLProtocol::SceneWorldChallengeSigninReq& protocol);

class SendChallengersFunction : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const;
};

void HandleProtocol(CLProtocol::ChallengeWorldChallengeSyncResult& protocol);

void HandleProtocol(CLProtocol::ChallengeWorldChallengeNotifyBattle& protocol);
*/

class WorldCrossEnterSceneFunction : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const;
};

void HandleProtocol(CLProtocol::CrossEnterSceneRes& protocol);

void HandleProtocol(CLProtocol::CrossExitSceneReq& protocol);

void HandleProtocol(CLProtocol::CrossExitSceneRes& protocol);

void HandleProtocol(CLProtocol::SysTransmitPlayerProtocolInNode& trans);

#endif
