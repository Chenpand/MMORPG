#ifndef __SINGLE_SCENE_H__
#define __SINGLE_SCENE_H__

#include "Scene.h"

/**
*@brief ���˳���
*/
class Player;
class SingleScene : public Scene
{
    AVALON_DEFINE_OBJECTPOOL(SingleScene, Avalon::NullMutex)

public:
    SingleScene();
    SingleScene(const Scene& scene);
    ~SingleScene();

public:
    UInt8 GetType() const { return SCENE_TYPE_SINGLE; }

public:
    virtual void VisitObjectsAround(const CLPosition& pos, SceneObjectVisitor& visitor) {}
    virtual void VisitPlayersAround(const CLPosition& pos, CLVisitor<Player>& visitor) {}
    virtual void VisitObjectsNewZone(const SceneObject* obj, SceneObjectVisitor& visitor) {}
    virtual void VisitObjectsOldZone(const SceneObject* obj, SceneObjectVisitor& visitor) {}
    virtual bool CanSeeOthers() { return false; }

    virtual void OnPlayerEnter(Player* player);
    virtual void OnPlayerLeave(Player* player);

public://�¼�


private:
};


#endif