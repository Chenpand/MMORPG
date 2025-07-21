#ifndef _US_SCENE_HANDLER_H_
#define _US_SCENE_HANDLER_H_

#include <CLPacketExecutor.h>

#include <CLSceneProtocol.h>
#include <CLMoveProtocol.h>
#include <CLTeamCopyProtocol.h>
#include <CLItemProtocol.h>
#include <CLItemThingProtocol.h>

class USPlayer;

void HandleProtocol(CLProtocol::TeamCopyExitSceneReq& protocol);

void HandleProtocol(CLProtocol::TeamCopyPlayerReconnect& protocol);

void HandleProtocol(CLProtocol::SceneCheckItemNumRet& protocol, USPlayer* player);

void HandleProtocol(CLProtocol::SceneItemThingStartRes& protocol, USPlayer* player);

void HandleProtocol(CLProtocol::SceneItemThingCommitRes& protocol, USPlayer* player);

class SceneSyncSceneObjectFunction : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const;
};

class SceneDeleteSceneObjectFunction : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const;
};

class SceneSyncPlayerMoveFunction : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const;
};

#endif
