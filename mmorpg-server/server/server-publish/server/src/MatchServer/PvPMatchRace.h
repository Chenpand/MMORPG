#ifndef __PVP_MATCH_RACE_H__
#define __PVP_MATCH_RACE_H__

#include "MatchRace.h"
#include "RobotMgr.h"
/*
    ������ϰ
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
    *@brief �������ս������µĻ��ֱ仯
    */
    virtual void CountMatchScoreChanges();

	/**
	*@brief ��������˻���
	*/
	UInt32 GenRobotMatchScore(UInt32 matchScore);

protected:
    MatchRaceResult GetRaceResult(UInt32 result);
    MatchTeamMember* GetOpponent(ObjID_t roleId);
    CLProtocol::MatchOpponent GetOpponentInfo(ObjID_t roleId);

protected:
	

};

#endif
