#ifndef __PK_PREPARE_ENTRY_SCENE_H__
#define __PK_PREPARE_ENTRY_SCENE_H__

#include "SingleScene.h"

/**
*@brief ս��׼������
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

public://�¼�


private:
};


#endif