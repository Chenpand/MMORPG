#ifndef __PREMIUM_LEAGUE_PRELIMINAY_MATCH_ALGORITHM_H__
#define __PREMIUM_LEAGUE_PRELIMINAY_MATCH_ALGORITHM_H__

#include "MatchAlgorithmBase.h"
#include <CLSeasonDefine.h>

struct PremiumLeaguePreliminayMatchUnit
{
	PremiumLeaguePreliminayMatchUnit()
	{
		memset(this, 0, sizeof(*this));
	}

	// ����ID
	ObjID_t			teamId;
	// ���λ
	SeasonMainLevel mainLevel;
	// ��ʤ��
	UInt32			winSteak;
	// ��ʼʱ��(s)
	UInt32			startTime;
	// �Ƿ��Ѿ�ƥ�䵽
	bool			dirty;
};

class PremiumLeaguePreliminayMatchAlgorithm : public Match::MatchAlgorithm
{
	typedef std::vector<PremiumLeaguePreliminayMatchUnit*> TerrMatchPool;
public:
	PremiumLeaguePreliminayMatchAlgorithm();
	~PremiumLeaguePreliminayMatchAlgorithm();

public:
	virtual bool Initialize();
	virtual void OnTick(const Avalon::Time& now);

	virtual bool AddTeam(ObjID_t teamId, UInt8 playerNum, Match::MatchKey& key);
	virtual bool DelTeam(ObjID_t teamId);
	virtual std::vector<Match::MatchTeamResult> GetMatchTeamResult();

protected:
	PremiumLeaguePreliminayMatchUnit* _FindOpponent(PremiumLeaguePreliminayMatchUnit* unit);
	bool IsMatch(PremiumLeaguePreliminayMatchUnit* a, PremiumLeaguePreliminayMatchUnit* b);
	void _DelFromPool(PremiumLeaguePreliminayMatchUnit* unit);

	/*
		�Ƿ����������ȼ���Χ
	*/
	bool IsMatchSeasonLevel(UInt32 sec, SeasonMainLevel level1, SeasonMainLevel level2);

	/*
	�Ƿ����������ȼ���Χ
	*/
	bool IsMatchWinSteak(UInt32 sec, UInt32 num1, UInt32 num2);

private:
	UInt64													m_lastUpdateTime;
	std::map<ObjID_t, PremiumLeaguePreliminayMatchUnit*>	m_matchUnits;
};

#endif