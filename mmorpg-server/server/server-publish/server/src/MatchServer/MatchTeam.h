#ifndef __MATCH_TEAM_H__
#define __MATCH_TEAM_H__

#include <CLDefine.h>
#include <CLObject.h>
#include <AvalonObjectPool.h>

#include "MatchTeamMember.h"

class MatchTeam : public CLObject
{
    AVALON_DEFINE_OBJECTPOOL(MatchTeam, Avalon::NullMutex)
public:
    MatchTeam();
	MatchTeam(const MatchTeam& other);
    ~MatchTeam();

	MatchTeam& operator=(const MatchTeam& other);

public:
    void AddMember(MatchTeamMember* member);
    void DelMember(ObjID_t id);
    MatchTeamMember* FindMember(ObjID_t id) const;
    const VMatchTeamMember& GetTeamMembers() const { return m_members; }
    UInt8 GetMemberNum() const { return (UInt8)m_members.size(); }

	inline UInt32 GetCreateTime() const { return m_createTime; }

    inline void SetScore(UInt32 score) { m_score = score; }
    inline UInt32 GetScore() const { return m_score; }

    inline void SetRaceID(ObjID_t id) { m_raceId = id; }
    inline ObjID_t GetRaceID() const { return m_raceId; }

    inline void SetWsTeamID(UInt32 id) { m_wsTeamId = id; }
    inline UInt32 GetWsTeamID() const { return m_wsTeamId; }

    inline void SetWsID(UInt32 id) { m_wsId = id; }
    inline UInt32 GetWsID() const { return m_wsId; }

    inline void SetDungeonID(UInt32 id) { m_dungeonId = id; }
    inline UInt32 GetDungeonID() const { return m_dungeonId; }

    inline void SetHell(UInt8 hell) { m_isHell = hell; }
    inline UInt8 IsHell() const { return m_isHell; }

	inline void SetMatching(bool matching) { m_isMatching = matching; }
	inline bool IsMatching() const { return m_isMatching; }

	inline void SetWaitMatchTime(UInt32 time) { m_waitMatchTime = time; }
	inline UInt32 GetWaitMatchTime() const { return m_waitMatchTime; }

	inline void SetGuildID(ObjID_t id) { m_guildId = id; }
	inline ObjID_t GetGuildID() const { return m_guildId; }

	inline void SetTerrID(UInt8 id) { m_terrId = id; }
	inline UInt8 GetTerrID() const { return m_terrId; }

	inline void SetPremiumLeagueBattleID(ObjID_t id) { m_premiumLeagueBattleId = id; }
	inline ObjID_t GetPremiumLeagueBattleID() const { return m_premiumLeagueBattleId; }

	inline void SetGroupID(UInt32 id) { m_GroupID = id; }
	inline UInt32 GetGroupID() const { return m_GroupID; }

	inline void SetOrderID(UInt32 id) { m_OrderID = id; }
	inline UInt32 GetOrderID() const { return m_OrderID; }
	
	

	inline void SetCityMonster(const SceneNpc& monster) { m_cityMonster = monster; }
	inline SceneNpc GetCityMonster() const { return m_cityMonster; }

	inline void SetLv(const UInt32& lv) { m_Lv = lv; }
	inline UInt32 GetLv() const { return m_Lv; }

public: 
    static ObjID_t GenID()
    {
        static ObjID_t id = 0;
        return ++id;
    }

private:
	// 创建时间
	UInt32					m_createTime;
	// 所有成员
    VMatchTeamMember        m_members;
	// 正在匹配中
	bool					m_isMatching;
	// 最多等待匹配到这个时候，如果超过这个时间，就使用机器人
	UInt32					m_waitMatchTime;
    // 匹配积分
    UInt32                  m_score;
    // 比赛ID
    ObjID_t                 m_raceId;
    // WorldServer的ID
    UInt32                  m_wsId;
    // WorldServer上的队伍ID
    UInt32                  m_wsTeamId;
    // 地下城ID
    UInt32                  m_dungeonId;
    // 是否深渊模式
    UInt8                   m_isHell;
	// 公会ID
	ObjID_t					m_guildId;
	// 领地ID
	UInt8					m_terrId;
	// 赏金联赛比赛ID
	ObjID_t					m_premiumLeagueBattleId;
	// 城镇怪物
	SceneNpc				m_cityMonster;
	// 等级
	UInt32					m_Lv;
	// 冠军赛分组ID
	UInt32                  m_GroupID;
	// 冠军赛5局序列ID
	UInt32                  m_OrderID;
};

typedef std::vector<MatchTeam*> VMatchTeam;

#endif 