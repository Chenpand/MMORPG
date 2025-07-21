#ifndef __BATTLE_RACE_H__
#define __BATTLE_RACE_H__

#include "BSTeam.h"
#include "BSBaseRace.h"

enum BattleRaceState
{
	BRS_WAIT_PLAYER_INFO,
	BRS_WAIT_RELAY_SERVER_RES,
	BRS_RACE,
};

class BattleRace : public BSBaseRace
{
public:
	BattleRace(UInt32 battleId, UInt8 raceType);
	~BattleRace();

public:
	bool OnCreate(VBSTeam teams);
	bool OnRaceEnd(EGameSessionDismissType ret, const PkRaceEndInfo& raceEndInfo);

public:
	inline BattleRaceState GetState() { return m_state; }
	inline void SetState(BattleRaceState state) { m_state = state; }
	
    VBSTeamMember GetAllMember() const;
    BSTeamMember* FindMember(UInt64 id) const;

	inline const VBSTeam& GetAllTeam() const { return m_teams; }

	bool IsAllPlayerHasRaceInfo() const;

	void SetRaceInfo(UInt64 playerId, const RacePlayerInfo& info);

	bool RequestRelayServerStart(void);

	void SetPlayerStateStatus(UInt64 playerId, BattlePlayerState status);

	void SetPlayerRaceID(UInt64 playerId, UInt64 raceId);

	bool SendToRelay(Avalon::Protocol& protocol);

private:
	BattleRaceState	m_state;
	VBSTeam			m_teams;
};

#endif