#ifndef __PK_SCENE_H__
#define __PK_SCENE_H__

#include "SingleScene.h"

/**
*@brief PK³¡¾°
*/
class Player;
class PKScene : public SingleScene
{
	AVALON_DEFINE_OBJECTPOOL(PKScene, Avalon::NullMutex)

public:
	PKScene() {}
	PKScene(const Scene& scene) : SingleScene(scene) {}
	~PKScene() {}

public:
	UInt8 GetType() const { return SCENE_TYPE_PK; }

public://ÊÂ¼þ


private:
};


#endif