#ifndef __CITY_MONSTER_GENERATE_DATA_ENTRY_H__
#define __CITY_MONSTER_GENERATE_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <set>
#include <CLObjectDefine.h>

enum CityMonsterType
{
	CITY_MONSTER_INVALID,
	CITY_MONSTER_ACTIVITY,
	CITY_MONSTER_TASK,
	CITY_MONSTER_LOST_DUNGEON,

	CITY_MONSTER_NUM
};

struct CityMonsterElem
{
	UInt32 sceneId;
	UInt32 monsterId;
	UInt32 prob;
};

/**
*@brief 城镇刷怪表
*/
class CityMonsterGenerateDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow &row);

	UInt32 RandMonster(UInt32 sceneId);

	UInt32 RandScene();

public:
	// ID
	UInt32					id;
	CityMonsterType			monsterType;
	std::vector<UInt32>		sceneIds;
	UInt32					posGroupId;
	UInt32					minNum;
	UInt32					maxNum;
	std::vector<CityMonsterElem>	monsterList;
	UInt32					dungeonId;
};


class CityMonsterGenerateDataEntryMgr : public Avalon::DataEntryMgrBase<CityMonsterGenerateDataEntry>, public Avalon::Singleton<CityMonsterGenerateDataEntryMgr>
{
public:
	//CityMonsterGenerateDataEntry* FindMonsterSolutionByTypeAndScene(CityMonsterType type, UInt32 sceneId);
	std::vector<CityMonsterGenerateDataEntry*> FindMonsterSolutionByType(CityMonsterType type);
	std::vector<CityMonsterGenerateDataEntry*> FindMonsterSolutionByTypeAndSceneId(CityMonsterType type, UInt32 sceneId);
};

/**
*@brief 城镇刷怪位置表
*/

class CityMonsterPositionDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow &row);

public:
	// ID
	UInt32					id;
	UInt32					sceneId;
	UInt32					groupId;
	NpcPos					pos;
};

class CityMonsterPositionDataEntryMgr : public Avalon::DataEntryMgrBase<CityMonsterPositionDataEntry>, public Avalon::Singleton<CityMonsterPositionDataEntryMgr>
{
public:
	bool AddEntry(CityMonsterPositionDataEntry* entry);

	std::set<NpcPos> GetAllPos(UInt32 sceneId, UInt32 groupId);

private:
	std::map<std::pair<UInt32, UInt32>, std::set<NpcPos>> m_MapPos;
};

#endif