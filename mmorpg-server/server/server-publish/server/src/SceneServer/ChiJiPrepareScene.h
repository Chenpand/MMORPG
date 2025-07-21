#ifndef __CHIJI_PREPARE_ENTRY_SCENE_H__
#define __CHIJI_PREPARE_ENTRY_SCENE_H__

#include "SingleScene.h"

/**
*@brief 吃鸡准备场景
*/
class Player;
class ChiJiPrepareScene : public Scene
{
	AVALON_DEFINE_OBJECTPOOL(ChiJiPrepareScene, Avalon::NullMutex)

public:
	ChiJiPrepareScene();
	ChiJiPrepareScene(const Scene& scene);
	~ChiJiPrepareScene();

public:
	UInt8 GetType() const { return SCENE_TYPE_CHIJI_PREPARE; }

public://事件


private:
};

#endif
