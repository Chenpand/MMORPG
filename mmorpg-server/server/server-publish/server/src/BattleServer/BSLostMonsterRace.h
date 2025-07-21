#pragma once

#include "BSBaseRace.h"
#include <CLGameSessionProtocol.h>

class BSLostBattle;

//迷失地牢击杀小怪比赛对象
class BSLostMonsterRace : public BSBaseRace
{
public:
	BSLostMonsterRace();
	BSLostMonsterRace(UInt8 raceType);
	~BSLostMonsterRace();

	void OnCreate(UInt64 playerId, CLProtocol::DungenServerTeamStartRaceReq& teamStartRaceReq);

	inline void SetLostBattle(BSLostBattle*  battle) { m_ownerBattle = battle; }
	BSLostBattle* GetLostBattle();

	virtual UInt32 StartRace();
	virtual void OnStartRaceFail();
	virtual void OnStartDungeonRes(UInt32 result, UInt64 gamesessionID);
	virtual void OnDungeonEnd(UInt8 result, DungeonRaceEndInfo& endInfo);
private:
	ObjID_t m_playerId;
	SceneNpc m_battleMonster;

	BSLostBattle*  m_ownerBattle; //所在战场
	CLProtocol::DungenServerTeamStartRaceReq m_teamStartRaceReq;
};