#ifndef __ACTIVITY_SCENE_H__
#define __ACTIVITY_SCENE_H__

#include "SingleScene.h"

/**
*@brief �����
*/
class Player;
class ActivityScene : public SingleScene
{
	AVALON_DEFINE_OBJECTPOOL(ActivityScene, Avalon::NullMutex)

public:
	ActivityScene() {}
	ActivityScene(const Scene& scene) : SingleScene(scene) {}
	~ActivityScene() {}

public:
	UInt8 GetType() const { return SCENE_TYPE_ACTIVITY; }

public://�¼�


private:
};


#endif