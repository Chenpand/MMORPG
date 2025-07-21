#ifndef __MATCH_TEAM_MEMBER_H__
#define __MATCH_TEAM_MEMBER_H__

#include <CLDefine.h>
#include <CLObject.h>
#include <AvalonObjectPool.h>
#include <CLMatchProtocol.h>
#include <CLGameSessionProtocol.h>

// 匹配战斗结果
enum MatchRaceResult
{
    // 胜利
    MATCH_RACE_RESULT_WIN,
    // 失败
    MATCH_RACE_RESULT_LOSE,
    // 平
    MATCH_RACE_RESULT_DRAW,
    // 异常
    MATCH_RACE_RESULT_ERROR,
    // 
    MATCH_RACE_RESULT_NUM
};

class MatchTeamMember : public CLObject
{
    AVALON_DEFINE_OBJECTPOOL(MatchTeamMember, Avalon::NullMutex)
public:
    MatchTeamMember();
    ~MatchTeamMember();
	MatchTeamMember(const MatchTeamMember& other);
	MatchTeamMember& operator=(const MatchTeamMember& other);

	inline void SetLevel(UInt16 level) { m_level = level; }
	inline UInt16 GetLevel() const { return m_level; }

    inline void SetScore(UInt32 score) { m_score = score; }
    inline UInt32 GetScore() const { return m_score; }

	inline void SetPkType(PkType type) { m_pkType = type; }
	inline PkType GetPkType() const { return m_pkType; }

    inline void SetWsID(UInt32 id) { m_wsId = id; }
    inline UInt32 GetWsID() const { return m_wsId; }

    inline void SetSeat(UInt8 seat) { m_seat = seat; }
    inline UInt8 GetSeat() const { return m_seat; }

    inline void SetScoreChange(MatchRaceResult result, Int32 change) { m_scoreChanges[result] = change; }
    inline Int32 GetScoreChange(MatchRaceResult result) const { return m_scoreChanges[result]; }

	inline void SetFightCount(UInt32 count) { m_fightCount = count; }
	inline UInt32 GetFightCount() const { return m_fightCount; }

	UInt32 GetDailyCount() const { return m_dailyCount; }
	void SetDailyCount(UInt32 count) { m_dailyCount = count; }

	UInt32 GetWinStreak() const { return m_winStreak; }
	void SetWinStreak(UInt32 num) { m_winStreak = num; }

	UInt32 GetloseStreak() const { return m_loseStreak; }
	void SetloseStreak(UInt32 num) { m_loseStreak = num; }

	ObjID_t GetGuildID() const { return m_guildId; }
	void SetGuildID(ObjID_t id) { m_guildId = id; }

	UInt8 GetTerrID() const { return m_terrId; }
	void SetTerrID(UInt8 id) { m_terrId = id; }

	UInt32 GetSeasonLevel() const { return m_seasonLevel; }
	void SetSeasonLevel(UInt32 level) { m_seasonLevel = level; }

	UInt32 GetSeasonStar() const { return m_seasonStar; }
	void SetSeasonStar(UInt32 star) { m_seasonStar = star; }

	UInt32 GetSeasonExp() const { return m_seasonExp; }
	void SetSeasonExp(UInt32 exp) { m_seasonExp = exp; }

	void SetStartTime(UInt64 time) { m_startTime = time; }
	UInt64 GetStartTime() { return m_startTime; }

	void SetServerCreateTime(UInt32 time) { m_serverCreateTime = time; }
	UInt32 GetServerCreateTime() const { return m_serverCreateTime; }

	void SetTeamID(UInt32 id) { m_teamId = id; }
	UInt32 GetTeamID() const { return m_teamId; }

    inline bool HasRaceInfo() const { return m_hasRaceInfo; }
    inline void SetRaceInfo(const RacePlayerInfo& info)
    {
        m_hasRaceInfo = true;
        m_raceInfo = info;
        m_raceInfo.pos = GetSeat();
    }
	inline void SetDungeonInfo(const DungeonRacePlayerInfo& info)
	{
		m_hasRaceInfo = true;
		m_dungeonInfo = info;
		m_dungeonInfo.raceInfo.pos = GetSeat();
		m_raceInfo = m_dungeonInfo.raceInfo;
	}
    inline const RacePlayerInfo& GetRaceInfo() const { return m_raceInfo; }
	inline const DungeonRacePlayerInfo& GetDungeonInfo() const { return m_dungeonInfo; }

    void SetByFigher(CLProtocol::Fighter fighter);
    CLProtocol::Fighter GetFighter() const;

private:
    UInt32                      m_score;
    UInt32                      m_wsId;
	// 公会ID
	ObjID_t						m_guildId;
	// 领地ID
	UInt8						m_terrId;
	// PK类型
	PkType						m_pkType;
    
    UInt8                       m_seat;
    bool                        m_hasRaceInfo;
    RacePlayerInfo				m_raceInfo;
	DungeonRacePlayerInfo		m_dungeonInfo;
	UInt32						m_fightCount;
	UInt32						m_dailyCount;
	UInt32						m_winStreak;
	UInt32						m_loseStreak;
	UInt32						m_seasonLevel;
	UInt32						m_seasonStar;
	UInt32						m_seasonExp;
	UInt16						m_level;
	UInt64						m_startTime;
	UInt32						m_teamId;
	UInt32						m_serverCreateTime;

    // 比赛结束后积分变动（胜、败、平、异常三种情况）
    Int32                       m_scoreChanges[MATCH_RACE_RESULT_NUM];
};

typedef std::vector<MatchTeamMember*> VMatchTeamMember;

#endif