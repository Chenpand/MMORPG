#ifndef __GUILD_BATTLE_MATCH_ALGORITHM_H__
#define __GUILD_BATTLE_MATCH_ALGORITHM_H__

#include "MatchAlgorithmBase.h"

struct GuildBattleMatchUnit
{
	GuildBattleMatchUnit()
	{
		memset(this, 0, sizeof(*this));
	}

	// 队伍ID
	ObjID_t teamId;
	// 公会ID
	ObjID_t	guildId;
	// 领地ID
	UInt8	terrId;
	// 连胜数
	UInt32	winSteak;
	// 开始时间(s)
	UInt32	startTime;
	// 是否已经匹配到
	bool	dirty;
};

class GuildBattleMatchAlgorithm : public Match::MatchAlgorithm
{
	const static UInt32 GUILD_BATTLE_MATCH_POOL_MAX_WINSTEAK = 5;
	typedef std::vector<GuildBattleMatchUnit*> TerrMatchPool;
public:
	GuildBattleMatchAlgorithm();
	~GuildBattleMatchAlgorithm();

public:
	virtual bool Initialize();
	virtual void OnTick(const Avalon::Time& now);

	virtual bool AddTeam(ObjID_t teamId, UInt8 playerNum, Match::MatchKey& key);
	virtual bool DelTeam(ObjID_t teamId);
	virtual std::vector<Match::MatchTeamResult> GetMatchTeamResult();

protected:
	GuildBattleMatchUnit* _FindOpponent(GuildBattleMatchUnit* unit);
	void GetMatchRange(GuildBattleMatchUnit* unit, UInt32& begin, UInt32& end);
	bool IsMatch(GuildBattleMatchUnit* a, GuildBattleMatchUnit* b);
	void _DelFromPool(GuildBattleMatchUnit* unit);

private:
	UInt64										m_lastUpdateTime;

	std::map<ObjID_t, GuildBattleMatchUnit*>	m_matchUnits;
	std::map<UInt8, TerrMatchPool>				m_matchPool;
};

#endif