#ifndef __MATCH_RACE_H__
#define __MATCH_RACE_H__

#include "MatchTeam.h"
#include <CLMatchProtocol.h>

enum MatchRaceState
{
    MRS_WAIT_PLAYER_INFO,
    MRS_WAITING_RELAY_SERVER_RES,
    MRS_RACE,
    MRS_NUM,
};

struct Robot;
class MatchRace : public CLObject
{
public:
    MatchRace();
    virtual ~MatchRace();

public:
    virtual bool OnCreate(VMatchTeam teams) = 0;
    virtual bool OnRelayServerCreateRaceRet() = 0;
    virtual bool OnRaceEnd(EGameSessionDismissType ret, const void* raceEnd) = 0;

public:
    inline MatchRaceState GetState() { return m_state; }
    inline void ChangeState(MatchRaceState state)
    {
        m_state = state;
        m_stateChangeTime = Avalon::Time::CurrentTime();
    }
	inline const Avalon::Time& GetStatusTime() const { return m_stateChangeTime; }

    inline UInt32 GetRelayServerID() const { return m_relayServerId; }
	void SetRelayServerID(UInt32 id);

    VMatchTeamMember GetAllMember() const;
    MatchTeamMember* FindMember(ObjID_t id) const;

    inline const VMatchTeam& GetAllTeam() const { return m_teams; }

    bool IsAllPlayerHasRaceInfo() const;

	void SetRobot(Robot* robot) { m_robot = robot; }
	Robot* GetRobot() { return m_robot; }
	UInt32 GetRobotMatchScore() { return m_robotMatchScore; }
	void SetRobotMatchScore(UInt32 score) { m_robotMatchScore = score; }
	UInt32 GetRobotSeasonLevel() const { return m_robotSeasonLevel; }
	void SetRobotSeasonLevel(UInt32 level) { m_robotSeasonLevel = level; }

	void SetRobotAI(RobotAIHardType type) { m_robotAI = type; }
	RobotAIHardType GetRobotAI() { return m_robotAI; }
	UInt32 GetDungeonID() const { return m_dungeonID; }
	void SetDungeonID(UInt32 id) { m_dungeonID = id; }

	UInt32 GetUsedTime() { return CURRENT_TIME.Sec() - m_createTime; }

protected:
	void CopyMatchTeam(VMatchTeam teams);

protected:
    MatchRaceState              m_state;
    VMatchTeam                  m_teams;
    UInt32                      m_relayServerId;
    Avalon::Time                m_stateChangeTime;
	Robot*						m_robot;
	UInt32						m_robotMatchScore;
	UInt32						m_robotSeasonLevel;
	RobotAIHardType				m_robotAI;
	UInt32                      m_dungeonID;
	UInt32						m_createTime;
};

#endif