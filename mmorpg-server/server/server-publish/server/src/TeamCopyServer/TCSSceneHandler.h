#ifndef _TCS_SCENE_HANDLER_H_
#define _TCS_SCENE_HANDLER_H_

#include <CLPacketExecutor.h>

#include <CLSceneProtocol.h>
#include <CLMoveProtocol.h>
#include <CLTeamCopyProtocol.h>

void HandleProtocol(CLProtocol::TeamCopyExitSceneReq& protocol);

void HandleProtocol(CLProtocol::TeamCopyPlayerReconnect& protocol);

class TeamCopyEnterSceneReqFunction : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const;
};

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
