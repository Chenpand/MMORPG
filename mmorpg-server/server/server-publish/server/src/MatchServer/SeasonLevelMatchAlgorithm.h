#ifndef __SEASON_LEVEL_MATCH_ALGORITHM_H__
#define __SEASON_LEVEL_MATCH_ALGORITHM_H__

#include "MatchAlgorithmBase.h"
#include <CLSeasonDefine.h>

struct GuildLevelMatchUnit
{
	GuildLevelMatchUnit()
	{
		wsId = 0;
		serverCreateTime = 0;
		roleId = 0;
		teamId = 0;
		level = 0;
		mainLevel = SML_BRONZE;
		smallLevel = 0;
		matchScore = 0;
		isPromotion = false;
		isWudaodahui = false;
		startTime = 0;
		dirty = false;
	}

	// 服务器ID
	UInt32	wsId;
	// 服务器开服时间
	UInt32  serverCreateTime;
	// 角色ID
	ObjID_t	roleId;
	// 队伍ID
	ObjID_t teamId;
	// 等级
	UInt16	level;
	// 大段位
	SeasonMainLevel mainLevel;
	// 小段位
	UInt32	smallLevel;
	// 匹配积分
	UInt32  matchScore;
	// 黑名单
	std::vector<ObjID_t> blackList;
	// 是否是升段站
	bool	isPromotion;
	// 是否是武道大会
	bool	isWudaodahui;
	// 开始时间(s)
	UInt32	startTime;
	// 是否已经匹配到
	bool	dirty;
};

class SeasonLevelMatchAlgorithm : public Match::MatchAlgorithm
{
	typedef std::vector<GuildLevelMatchUnit*> MatchPool;
public:
	SeasonLevelMatchAlgorithm();
	~SeasonLevelMatchAlgorithm();

public:
	virtual bool Initialize();
	virtual void OnTick(const Avalon::Time& now);

	virtual bool AddTeam(ObjID_t teamId, UInt8 playerNum, Match::MatchKey& key);
	virtual bool DelTeam(ObjID_t teamId);
	virtual std::vector<Match::MatchTeamResult> GetMatchTeamResult();

protected:
	/**
	*@brief 计算可匹配的积分差
	*/
	UInt32 _GetMatchScoreDiff(GuildLevelMatchUnit* unit);

	/**
	*@brief 获取匹配段位范围
	*/
	void _GetMatchLevelRange(GuildLevelMatchUnit* unit, SeasonMainLevel& minMainLevel, SeasonMainLevel& maxMainLevel);

	/**
	*@brief 获取可匹配的最大等级
	*/
	UInt16 _GetMatchMaxLevel(UInt16 level);

	GuildLevelMatchUnit* _FindOpponent(GuildLevelMatchUnit* unit);
	bool IsMatch(GuildLevelMatchUnit* a, GuildLevelMatchUnit* b);
	void _DelFromPool(GuildLevelMatchUnit* unit);

private:
	UInt64										m_lastUpdateTime;

	std::map<ObjID_t, GuildLevelMatchUnit*>		m_matchUnits;
};

#endif