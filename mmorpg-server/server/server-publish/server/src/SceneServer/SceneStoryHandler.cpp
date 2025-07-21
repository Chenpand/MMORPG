#include "SceneStoryHandler.h"

#include "Player.h"

void HandleProtocol(Player *pPlayer, CLProtocol::SceneNotifyStopStory &protocol)
{
	pPlayer->StopStory(protocol.id);
}

void HandleProtocol(Player *player, CLProtocol::SceneNotifyNewBoot &protocol)
{
	player->SetNewBoot(protocol.id);
}

void HandleProtocol(Player *player, CLProtocol::SceneNotifyBootFlag &protocol)
{
	player->SetBootFlag(protocol.bootId);
	//player->SetNewBoot(protocol.bootId);
}
