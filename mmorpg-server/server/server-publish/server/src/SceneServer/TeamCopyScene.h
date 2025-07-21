#ifndef _TEAM_COPY_SCENE_H_
#define _TEAM_COPY_SCENE_H_

#include "SingleScene.h"

/**
*@brief Õ≈±æ≥°æ∞
*/
class Player;
class TeamCopyScene : public SingleScene
{
	AVALON_DEFINE_OBJECTPOOL(TeamCopyScene, Avalon::NullMutex)

public:
	TeamCopyScene() {}
	TeamCopyScene(const Scene& scene) : SingleScene(scene) {}
	~TeamCopyScene() {}

public:
	UInt8 GetType() const { return SCENE_TYPE_TEAM_COPY; }
};

#endif
