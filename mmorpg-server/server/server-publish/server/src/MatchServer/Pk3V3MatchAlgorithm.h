#ifndef __PK_3V3_MATCH_ALGORITHM_H__
#define __PK_3V3_MATCH_ALGORITHM_H__

#include "MatchAlgorithmBase.h"
#include "PK3V3MatchAlgorithmBase.h"


struct Pk3V3MatchResult
{
	std::vector<UInt64> roomIds;
};
typedef std::vector<Pk3V3MatchResult> VPk3V3MatchResult;

class Pk3V3MatchAlgorithm : public Match::MatchAlgorithm
{
public:
	Pk3V3MatchAlgorithm();
	~Pk3V3MatchAlgorithm();

	void Update();

	VPk3V3MatchResult GetMatchResult();

public:
	virtual bool Initialize();
	virtual void OnTick(const Avalon::Time& now);

	virtual bool AddTeam(ObjID_t teamId, UInt8 playerNum, Match::MatchKey& key);
	virtual bool DelTeam(ObjID_t teamId);
	virtual std::vector<Match::MatchTeamResult> GetMatchTeamResult();

private:
	UInt64                          m_lastUpdateTime;
	PK3V3::Pk3V3MatchAlgorithmBase  m_algorithms;
};

#endif