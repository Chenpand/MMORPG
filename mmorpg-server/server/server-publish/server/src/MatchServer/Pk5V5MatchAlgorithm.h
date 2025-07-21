#ifndef __PK_5V5_MATCH_ALGORITHM_H__
#define __PK_5V5_MATCH_ALGORITHM_H__

#include "MatchAlgorithmBase.h"

class Pk5V5MatchAlgorithm : public Match::MatchAlgorithm
{
public:
	Pk5V5MatchAlgorithm();
	~Pk5V5MatchAlgorithm();

public:
	virtual bool Initialize();
	virtual void OnTick(const Avalon::Time& now);

	virtual bool AddTeam(ObjID_t teamId, UInt8 playerNum, Match::MatchKey& key);
	virtual bool DelTeam(ObjID_t teamId);
	virtual std::vector<Match::MatchTeamResult> GetMatchTeamResult();
	virtual UInt32 GetMatchPlayerNum() { return 0; }

private:
	std::vector<Match::MatchTeamResult> m_MatchResults;
	ObjID_t								m_WaitingTeamId;
};

#endif