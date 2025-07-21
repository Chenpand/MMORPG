#include "SingleScene.h"
#include "Player.h"
#include "Creature.h"

SingleScene::SingleScene()
{
}

SingleScene::SingleScene(const Scene& scene)
    :Scene(scene)
{
}

SingleScene::~SingleScene()
{
}

void SingleScene::OnPlayerEnter(Player* player)
{
    if (!player)
    {
        return;
    }

    player->SetHidden(true);
}

void SingleScene::OnPlayerLeave(Player* player)
{
    if (!player)
    {
        return;
    }

    player->SetHidden(false);
}