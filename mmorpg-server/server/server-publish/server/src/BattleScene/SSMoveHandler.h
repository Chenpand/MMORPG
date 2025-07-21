#ifndef _SS_MOVE_HANDLER_H_
#define _SS_MOVE_HANDLER_H_

#include <CLNetwork.h>
#include <CLMoveProtocol.h>
#include <CLMatchProtocol.h>

class Player;

void HandleProtocol(Player* player,CLProtocol::SceneMoveRequire& protocol);

void HandleProtocol(CLProtocol::SceneBeginChangeSceneReq& protocol);

void HandleProtocol(CLProtocol::SceneBeginChangeSceneRet& protocol);

class TransObjectDataFunction : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const;
};

void HandleProtocol(CLProtocol::SceneChangeScene& protocol);

void HandleProtocol(CLProtocol::SceneEnterScenePullPlayer& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneLeaveDynScene& req);

void HandleProtocol(CLProtocol::ScenePlayerPosReq &req);

void HandleProtocol(CLProtocol::WorldMatchRaceStart& req);

void HandleProtocol(Player* player, CLProtocol::SceneReturnToTown& req);

void HandleProtocol(Player* player, CLProtocol::ScenePlayerChangeSceneReq& req);

#endif
