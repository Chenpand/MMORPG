#ifndef _WORLD_NEW_TITLE_HANDLER_H_
#define _WORLD_NEW_TITLE_HANDLER_H_
#include <CLNewTitleProtocol.h>

class WSPlayer;

void HandleProtocol(WSPlayer* player, CLProtocol::WorldNewTitleTakeUpReq &req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldNewTitleTakeOffReq &req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldNewTitleTakeUpGuildPostReq &req);

void HandleProtocol(CLProtocol::SceneNewTitleModifyWorld& protocol);

void HandleProtocol(CLProtocol::SceneNewTitleAddWorld& protocol);
#endif