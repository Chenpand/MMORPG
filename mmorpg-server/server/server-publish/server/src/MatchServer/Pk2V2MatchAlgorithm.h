#ifndef __2V2_MATCH_ALGORITHM_H__
#define __2V2_MATCH_ALGORITHM_H__

#include "MatchAlgorithmBase.h"
#include "SeTimer.h"

class Team
{
public:
	ObjID_t id;
	UInt32 score = 0;
};

class MatchUnit
{
public:
	MatchUnit() = default;
	MatchUnit(UInt32 score, UInt32 lv);
	bool IsInRange(MatchUnit &unit);
	UInt32 GetTeamNum();
public:
	std::vector<Team> teamVec;
	UInt32 score = 0;
	UInt32 rangeScore = 0;
	UInt64 lastRangeTime = 0;
	UInt32 maxLvRange = 0;
	UInt32 lv = 0;
};

class Pk2V2MatchAlgorithm : public Match::MatchAlgorithm
{
public:
	Pk2V2MatchAlgorithm();
	~Pk2V2MatchAlgorithm();

public:
	virtual bool Initialize();
	virtual void OnTick(const Avalon::Time& now);

	virtual bool AddTeam(ObjID_t teamId, UInt8 playerNum, Match::MatchKey& key);
	virtual bool DelTeam(ObjID_t teamId);
	virtual std::vector<Match::MatchTeamResult> GetMatchTeamResult();
	virtual UInt32 GetMatchPlayerNum() { return 0; }

private:
	std::map<ObjID_t, MatchUnit> m_FindTeammateMap;
	std::map<ObjID_t, MatchUnit> m_FindEnemyMap;
	std::vector<Match::MatchTeamResult> m_MatchResults;
	Match::SeTimer waitTimer;

private:
	void DelFromMateMap(ObjID_t teamId);
	void DelFromEnemyMap(ObjID_t teamId);

};

#endif