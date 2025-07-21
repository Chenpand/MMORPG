#pragma once

#include "DungeonEntryScene.h"

struct DungeonEntrySceneDynamicKey
{
	DungeonEntrySceneDynamicKey(UInt32	ownSceneId_, UInt32 curDoorId_, UInt32	sceneId_)
		: ownSceneId(ownSceneId_), curDoorId(curDoorId_), sceneId(sceneId_)
	{}

	bool operator<(const DungeonEntrySceneDynamicKey& other) const
	{
		if (other.ownSceneId != ownSceneId)
		{
			return ownSceneId < other.ownSceneId;
		}
		if (other.curDoorId != curDoorId)
		{
			return curDoorId < other.curDoorId;
		}

		return sceneId < other.sceneId;
	}

	//所在场景id
	UInt32	ownSceneId;
	//在场景里的门id
	UInt32 curDoorId;
	//场景id
	UInt32	sceneId;
};

class DungeonEntrySceneDynamic : public DungeonEntryScene
{
	AVALON_DEFINE_OBJECTPOOL(DungeonEntrySceneDynamic, Avalon::NullMutex)
public:
	DungeonEntrySceneDynamic();
	DungeonEntrySceneDynamic(const Scene& scene);
	~DungeonEntrySceneDynamic();

public:
	UInt8 GetType() const { return SCENE_TYPE_DUNGEON_ENTRY_DYNAMIC; }

	void Destroy();

	void SetBattleId(UInt32 battleId) { m_battleId = battleId; }
	UInt32 GetBattleId() { return m_battleId; }

	/**
	 *@brief 场景退出
	 */
	virtual void OnSceneQuit();

	virtual bool CheckPlayerEnter(ObjID_t id, UInt16 level);
	virtual void OnPlayerEnter(Player* player);
	virtual void OnPlayerLeave(Player* player);

	virtual void KickPlayersOut();
	virtual void KickPlayerOut(Player* player);
	virtual void OnKickPlayerTimeout();

private:
	UInt32	m_battleId;

};