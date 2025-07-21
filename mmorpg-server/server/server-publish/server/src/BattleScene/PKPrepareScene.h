#ifndef __PK_PREPARE_ENTRY_SCENE_H__
#define __PK_PREPARE_ENTRY_SCENE_H__

#include "SingleScene.h"

/**
*@brief 战斗准备场景
*/
class Player;
class PKPrepareScene : public Scene
{
	AVALON_DEFINE_OBJECTPOOL(PKPrepareScene, Avalon::NullMutex)

public:
	PKPrepareScene();
	PKPrepareScene(const Scene& scene);
	~PKPrepareScene();

public:
	UInt8 GetType() const { return SCENE_TYPE_PK_PREPARE; }

public://事件


private:
};


#endif