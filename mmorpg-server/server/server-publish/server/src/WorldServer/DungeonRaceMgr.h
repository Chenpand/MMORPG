#ifndef __DUNGEON_RACE_MGR_H__
#define __DUNGEON_RACE_MGR_H__

#include <CLDefine.h>
#include <AvalonSingleton.h>
#include "DungeonRace.h"

class DungeonRaceMgr : public Avalon::Singleton<DungeonRaceMgr>
{
public:
    DungeonRaceMgr();
    ~DungeonRaceMgr();

    void OnTick(const Avalon::Time& now);

public:
    DungeonRace* CreateDungeonRace(UInt32 dungeonId);
    void DestoryDungeonRace(DungeonRace* race);

    DungeonRace* FindDungeonRaceById(UInt64 id);
    DungeonRace* FindDungeonRaceByRoleId(UInt64 roleId);
	DungeonRace* FindDungeonRaceBySession(ObjID_t session);
	bool IsPlayerInRace(ObjID_t roleid);

    void JoinDungeonRace(DungeonRace* dungeon, const DungeonRacePlayerInfo& info);
    void LeaveDungeonRace(UInt64 roleId, bool isDisconnect);

	void AddDungeonRaceToSessionMgr(ObjID_t session, DungeonRace* race);

	/**
	*@brief 根据地下城入口，随机最终的地下城ID
	*/
	UInt32 GenHellDungeonID(UInt32 entryDungeonId);

	/**
	*@brief 根据地下城入口，随机最终的地下城ID
	*/
	UInt32 GenDungeonIDByEntry(UInt32 entryDungeonId);
private:
    UInt64 GenDungeonRaceID();

private:
    // 当玩家离开地下城，地下城数据的保存时间
    const UInt32                    TIME_TO_WAIT_CLOSE_WHEN_PLAYER_LEAVE = /*1 * Avalon::Time::SECS_OF_MIN * Avalon::Time::MSECS_OF_SEC*/0; 
    HashMap<UInt64, DungeonRace*>   m_dungeones;
    HashMap<UInt64, DungeonRace*>   m_roleId2Dungeon;
	HashMap<ObjID_t, DungeonRace*>	m_session2Dungeon;
    UInt64                          m_dungeonIdSeed;
};

#endif
