#ifndef _TCS_DUNGEON_RACE_H_
#define _TCS_DUNGEON_RACE_H_

#include <AvalonObjectPool.h>
#include <CLDefine.h>

/**
*@brief 团本地下城比赛
*/
class TCSDungeonRace
{
	AVALON_DEFINE_OBJECTPOOL(TCSDungeonRace, Avalon::NullMutex)

public:
	TCSDungeonRace();
	~TCSDungeonRace();

public:
	/**
	*@brief 比赛Id
	*/
	inline void SetRaceId(UInt64 id) { m_RaceId = id; }
	inline UInt64 GetRaceId() { return m_RaceId; }

	/**
	*@brief 队伍小队Id
	*/
	inline void SetTeamSquadId(UInt32 id) { m_TeamSquadId = id; }
	inline UInt32 GetTeamSquadId() { return m_TeamSquadId; }

	/**
	*@brief 据点Id
	*/
	inline void SetFieldId(UInt32 id) { m_FieldId = id; }
	inline UInt32 GetFieldId() { return m_FieldId; }


	/**
	*@brief 开始时间
	*/
	inline void SetCreateTime(UInt64 time) { m_CreateTime = time; }
	inline UInt64 GetCreateTime() { return m_CreateTime; }
	
private:

	// 比赛Id
	UInt64 m_RaceId;
	// 队伍小队Id
	UInt32 m_TeamSquadId;
	// 据点id
	UInt32 m_FieldId;
	// 开始时间
	UInt64 m_CreateTime;
};

#endif
