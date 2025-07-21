#include "SceneTradeHandler.h"

#include "Player.h"
#include "Trade.h"

void HandleProtocol(Player* player, CLProtocol::SceneTradeUpdateItemsReq& req)
{
	if(player->GetTrade() == NULL) return;

	if(req.type == 0)
	{
		player->GetTrade()->AddItem(player, req.itemid, req.num);
	}
	else
	{
		player->GetTrade()->RemoveItem(player, req.itemid);
	}
}

void HandleProtocol(Player* player, CLProtocol::SceneTradeUpdateSilverReq& req)
{
	if(player->GetTrade() == NULL) return;

	player->GetTrade()->SetSilver(player, req.silver);
}

void HandleProtocol(Player* player, CLProtocol::SceneTradeSetStatus& req)
{
	if(player->GetTrade() == NULL) return;

	player->GetTrade()->SetStatus(player, req.status);
}
