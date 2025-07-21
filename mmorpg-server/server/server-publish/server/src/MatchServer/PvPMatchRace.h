#ifndef __PVP_MATCH_RACE_H__
#define __PVP_MATCH_RACE_H__

#include "MatchRace.h"
#include "RobotMgr.h"
/*
    自由练习
*/

class PvPMatchRace : public MatchRace
{
public:
    PvPMatchRace();
    virtual ~PvPMatchRace();

public:
    virtual bool OnCreate(VMatchTeam teams);
    virtual bool OnRelayServerCreateRaceRet();
    virtual bool OnRaceEnd(EGameSessionDismissType ret, const void* raceEnd);

protected:
    /**
    *@brief 计算各种战斗结果下的积分变化
    */
    virtual void CountMatchScoreChanges();

	/**
	*@brief 计算机器人积分
	*/
	UInt32 GenRobotMatchScore(UInt32 matchScore);

protected:
    MatchRaceResult GetRaceResult(UInt32 result);
    MatchTeamMember* GetOpponent(ObjID_t roleId);
    CLProtocol::MatchOpponent GetOpponentInfo(ObjID_t roleId);

protected:
	

};

#endif
