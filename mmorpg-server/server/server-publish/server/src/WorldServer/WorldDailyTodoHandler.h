#ifndef _WORLD_GET_PLAYER_DAILY_TODO_H_
#define _WORLD_GET_PLAYER_DAILY_TODO_H_
#include <CLDailyTodoProtocol.h>

class WSPlayer;

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGetPlayerDailyTodosReq &req);

void HandleProtocol(CLProtocol::WorldDailyTodoSyncData& protocol);
#endif