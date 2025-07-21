#ifndef __DUNGEON_MATCH_ALGORITHM_H__
#define __DUNGEON_MATCH_ALGORITHM_H__

#include "MatchAlgorithmBase.h"

class DungeonMatchAlgorithm : public Match::MatchAlgorithm
{
public:
	DungeonMatchAlgorithm();
	~DungeonMatchAlgorithm();

public:
	virtual bool Initialize();
	virtual void OnTick(const Avalon::Time& now);

	virtual bool AddTeam(ObjID_t teamId, UInt8 playerNum, Match::MatchKey& key);
	virtual bool DelTeam(ObjID_t teamId);
	virtual std::vector<Match::MatchTeamResult> GetMatchTeamResult();

private:
	UInt64										m_lastUpdateTime;
	HashMap<UInt32, Match::SeMatchAlorithm*>    m_mapAlgorithm;
};

#endif