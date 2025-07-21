#ifndef __SEASON_MATCH_ALGORITHM_H__
#define __SEASON_MATCH_ALGORITHM_H__

#include "MatchAlgorithmBase.h"

class SeasonMatchBaseAlorithm : public Match::SeMatchAlorithm
{
public:
	virtual int GetMatchScoreDiff(Match::SeMatchUnit* pkUnit);
};

class SeasonMatchAlgorithm : public Match::MatchAlgorithm
{
public:
    SeasonMatchAlgorithm();
    ~SeasonMatchAlgorithm();

public:
    virtual bool Initialize();
    virtual void OnTick(const Avalon::Time& now);

	virtual bool AddTeam(ObjID_t teamId, UInt8 playerNum, Match::MatchKey& key);
    virtual bool DelTeam(ObjID_t teamId);
    virtual std::vector<Match::MatchTeamResult> GetMatchTeamResult();

private:
    UInt64                          m_lastUpdateTime;
	SeasonMatchBaseAlorithm         m_algorithm;
};

#endif