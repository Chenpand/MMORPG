#ifndef __DUNGEON_MATCH_RACE_H__
#define __DUNGEON_MATCH_RACE_H__

#include "MatchRace.h"

class DungeonMatchRace : public MatchRace
{
public:
    DungeonMatchRace();
    virtual ~DungeonMatchRace();

    inline void SetDungeonID(UInt32 dungeonId) { m_dungeonId = dungeonId; }
    inline UInt32 GetDungeonID() const { return m_dungeonId; }

    inline void SetWsTeamID(UInt32 id) { m_wsTeamId = id; }
    inline UInt32 GetWsTeamID() const { return m_wsTeamId; }

    inline void SetWsID(UInt32 id) { m_wsId = id; }
    inline UInt32 GetWsID() const { return m_wsId; }

    inline void SetHell(UInt8 hell) { m_isHell = hell; }
    inline UInt8 IsHell() const { return m_isHell; }

	inline void SetCityMonster(SceneNpc monster) { m_cityMonster = monster; }
	inline SceneNpc GetCityMonster() const { return m_cityMonster; }

public:
    virtual bool OnCreate(VMatchTeam teams);
    virtual bool OnRelayServerCreateRaceRet();
    virtual bool OnRaceEnd(EGameSessionDismissType ret, const void* raceEnd);

private:
    UInt32      m_dungeonId;
    UInt32      m_wsTeamId;
    UInt32      m_wsId;
    UInt8       m_isHell;
	SceneNpc	m_cityMonster;
};

#endif
