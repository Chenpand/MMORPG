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

	// 队伍ID
	ObjID_t			teamId;
	// 大段位
	SeasonMainLevel mainLevel;
	// 连胜数
	UInt32			winSteak;
	// 开始时间(s)
	UInt32			startTime;
	// 是否已经匹配到
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
		是否满足赛季等级范围
	*/
	bool IsMatchSeasonLevel(UInt32 sec, SeasonMainLevel level1, SeasonMainLevel level2);

	/*
	是否满足赛季等级范围
	*/
	bool IsMatchWinSteak(UInt32 sec, UInt32 num1, UInt32 num2);

private:
	UInt64													m_lastUpdateTime;
	std::map<ObjID_t, PremiumLeaguePreliminayMatchUnit*>	m_matchUnits;
};

#endif