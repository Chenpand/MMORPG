#ifndef __MATCH_RESULT_H__
#define __MATCH_RESULT_H__

#include <CLDefine.h>
#include <CLObject.h>
#include <AvalonObjectPool.h>
#include <vector>

#include "CLMatchProtocol.h"
#include "CLGameSessionProtocol.h"

class MatchFighter : public CLObject
{
public:
	MatchFighter() : m_Pos(0),m_HasRaceInfo(false) {}

	inline UInt8 GetPos() const { return m_Pos; }
	inline void SetPos(UInt8 pos) { m_Pos = pos; }

    inline bool HasRaceInfo() const { return m_HasRaceInfo; }
    inline void SetRaceInfo(const RacePlayerInfo& info)
    {
        m_HasRaceInfo = true;
        m_RaceInfo = info;
        m_RaceInfo.pos = GetPos();
    }
    inline const RacePlayerInfo& GetRaceInfo() const { return m_RaceInfo; }

    CLProtocol::Fighter GetFighter()
    {
        CLProtocol::Fighter fighter;
        fighter.roleId = GetID();
        fighter.accId = m_RaceInfo.accid;
        fighter.pos = GetPos();

        return fighter;
    }

private:
	UInt8						m_Pos;

    bool                        m_HasRaceInfo;
    RacePlayerInfo				m_RaceInfo;
};

enum MatchResultState
{
    MRS_WAITING_PLAYER_INFO,
    MRS_WAITING_DELAY_SERVER_RES,
    MRS_RACING,
};

class MatchResult : public CLObject
{
public:
	MatchResult() {}

    inline MatchResultState GetState() const { return m_State; }
    inline void ChangeState(MatchResultState state) 
    { 
        m_State = state; 
        m_StateChangeTime = Avalon::Time::CurrentTime();
    }
    
	inline UInt32 GetRelayServerID() const { return m_RelayServerID; }
	inline void SetRelayServerID(UInt32 id) { m_RelayServerID = id; }

	inline Avalon::Time GetCreateTime() const { return m_StateChangeTime; }
	inline void SetCreateTime(Avalon::Time& now) { m_StateChangeTime = now; }
	
	void PushFighter(MatchFighter fighter)
	{
		m_Fighters.push_back(fighter);
	}

	const std::vector<MatchFighter>& GetFighters() const
	{
		return m_Fighters;
	}

    void SetRaceInfo(ObjID_t roleId, const RacePlayerInfo& info)
    {
        for (UInt32 i = 0; i < m_Fighters.size(); i++)
        {
            if (m_Fighters[i].GetID() == roleId)
            {
                m_Fighters[i].SetRaceInfo(info);
            }
        }
    }

    CLProtocol::MatchOpponent GetOpponentInfo(ObjID_t roleId)
    {
        CLProtocol::MatchOpponent opponent;
        if (m_Fighters.size() != 2)
        {
            return opponent;
        }

        for (auto& fighter : m_Fighters)
        {
            if (fighter.GetID() != roleId)
            {
				opponent.zoneId = fighter.GetRaceInfo().zoneId;
                opponent.roleId = fighter.GetID();
                opponent.occu = fighter.GetRaceInfo().occu;
				opponent.name = fighter.GetName();
				opponent.level = fighter.GetRaceInfo().level;
				opponent.seasonLevel = fighter.GetRaceInfo().seasonLevel;
            }
        }

        return opponent;
    }

    bool IsAllPlayerHasRaceInfo()
    {
        for (auto fighter : m_Fighters)
        {
            if (!fighter.HasRaceInfo())
            {
                return false;
            }
        }

        return true;
    }

    bool IsTimeOut(Avalon::Time& now) const
    {
        if (GetState() == MRS_WAITING_PLAYER_INFO)
        {
            return false;
        }
        else if (GetState() == MRS_WAITING_DELAY_SERVER_RES)
        {
            return false;
        }
        else if (GetState() == MRS_RACING)
        {
            if (now.MSec() - m_StateChangeTime.MSec() >= 3 * Avalon::Time::SECS_OF_MIN * Avalon::Time::MSECS_OF_SEC)
            {
                return true;
            }

            return false;
        }
        else
        {
            return true;
        }
    }

private:
    MatchResultState                m_State;
	UInt32							m_RelayServerID;
	Avalon::Time					m_StateChangeTime;
	std::vector<MatchFighter>		m_Fighters;
};

#endif
