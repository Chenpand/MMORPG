#include "DungeonEntryScene.h"
#include "Player.h"
#include "PlayerMgr.h"
#include "Creature.h"
#include "LostDungeonMgr.h"

DungeonEntryScene::DungeonEntryScene()
{
	m_MajorCity = 0;
	m_MajorDoorID = 0;
}

DungeonEntryScene::DungeonEntryScene(const Scene& scene)
    :SingleScene(scene)
{
	m_MajorCity = 0;
	m_MajorDoorID = 0;
}

DungeonEntryScene::~DungeonEntryScene()
{
}

bool DungeonEntryScene::Load(UInt32 mapid, const std::string& config)
{
	if (!Scene::Load(mapid, config))
	{
		return false;
	}

	auto doors = GetDoors();
	if (doors.size() != 1)
	{
		ErrorLogStream << "dungeon entry scene(" << mapid << ") error door info." << LogStream::eos;
		return false;
	}

	m_MajorCity = doors[0].targetSceneId;
	m_MajorDoorID = doors[0].targetDoorId;

	return true;
}

