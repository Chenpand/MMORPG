#include "WorldDailyTodoHandler.h"

#include "WSPlayerMgr.h"
#include "DailyTodo.h"

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGetPlayerDailyTodosReq &req)
{
	CLProtocol::WorldGetPlayerDailyTodosRes res;
	player->GetDailyTodoMgr().GetDailyTodoList(res.dailyTodos);
	player->SendProtocol(res);
}

void HandleProtocol(CLProtocol::WorldDailyTodoSyncData& protocol)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if (!player)
	{
		return;
	}
	player->GetDailyTodoMgr().OnPlayerTaskUpdate(protocol.id, protocol.value);
}