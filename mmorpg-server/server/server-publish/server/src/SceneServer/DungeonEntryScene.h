#ifndef __DUNGEON_ENTRY_SCENE_H__
#define __DUNGEON_ENTRY_SCENE_H__

#include "SingleScene.h"

/**
*@brief 地下城入口场景
*/
class Player;
class DungeonEntryScene : public SingleScene
{
    AVALON_DEFINE_OBJECTPOOL(DungeonEntryScene, Avalon::NullMutex)

public:
    DungeonEntryScene();
    DungeonEntryScene(const Scene& scene);
    ~DungeonEntryScene();

	virtual bool Load(UInt32 mapid, const std::string& config);

public:
    UInt8 GetType() const { return SCENE_TYPE_DUNGEON_ENTRY; }

	/**
	*@brief 地下城所属的主城
	*/
	UInt32 GetMajorCity() const { return m_MajorCity; }
	void SetMajorCity(UInt32 city) { m_MajorCity = city; }

	/**
	*@brief 主城进入地下城的门
	*/
	UInt32 GetMajorDoor() const { return m_MajorDoorID; }
	void SetMajorDoor(UInt32 door) { m_MajorDoorID = door; }

public://事件


private:
	// 地下城所属的主城ID
	UInt32			m_MajorCity;
	// 主城进入地下城的门ID
	UInt32			m_MajorDoorID;
};


#endif