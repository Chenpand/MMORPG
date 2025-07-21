#ifndef _CHAMPION_SCENE_H_
#define _CHAMPION_SCENE_H_

#include "SingleScene.h"

/**
*@brief ¹Ú¾üÈü³¡¾°
*/
class Player;
class ChampionScene : public SingleScene
{
	AVALON_DEFINE_OBJECTPOOL(ChampionScene, Avalon::NullMutex)

public:
	ChampionScene() {}
	ChampionScene(const Scene& scene) : SingleScene(scene) {}
	~ChampionScene() {}

public:
	UInt8 GetType() const { return SCENE_TYPE_CHAMPION; }
};

#endif
