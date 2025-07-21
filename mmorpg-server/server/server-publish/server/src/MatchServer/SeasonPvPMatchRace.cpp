#include "SeasonPvPMatchRace.h"
#include "MSRouter.h"

SeasonPvPMatchRace::SeasonPvPMatchRace()
{

}

SeasonPvPMatchRace::~SeasonPvPMatchRace()
{

}

bool SeasonPvPMatchRace::OnRaceEnd(EGameSessionDismissType ret, const void* raceEnd)
{
    InfoLogStream << "race (" << GetID() << ") recv race end, ret(" << ret << ")" << LogStream::eos;

    PkRaceEndInfo raceEndInfo = *(PkRaceEndInfo*)raceEnd;

    if (GetState() != MRS_RACE)
    {
        ErrorLogStream << "race (" << GetID() << ") not racing, state(" << GetState() << ")." << LogStream::eos;
        return false;
    }

    for (auto& result : raceEndInfo.results)
    {
        auto member = FindMember(result.roleId);
        if (!member)
        {
            InfoLogStream << "race (" << GetID() << ") can't find member(" << result.roleId << ")." << LogStream::eos;
            continue;
        }

        CLProtocol::MatchServerRaceEnd endReq;
        endReq.type = member->GetPkType();
        endReq.opponent = GetOpponentInfo(result.roleId);
        endReq.result = result;
		endReq.dungeonID = GetDungeonID();
		endReq.usedTime = GetUsedTime();
        endReq.seasonScoreChange = member->GetScoreChange(GetRaceResult(result.resultFlag));
		
        MSRouter::Instance()->SendToWorld(member->GetWsID(), endReq);
    }
    return true;
}

void SeasonPvPMatchRace::CountMatchScoreChanges()
{
    VMatchTeamMember members = GetAllMember();
    for (auto member : members)
    {
        if (!member)
        {
            continue;
        }

		UInt32 score = member->GetScore();
		UInt32 opponentScore = score;
        auto opponent = GetOpponent(member->GetID());
		// 没有对手，说明是打机器人
        if (opponent)
        {
			opponentScore = opponent->GetScore();
        }
		else
		{
			m_robotMatchScore = GenRobotMatchScore(member->GetScore());
			opponentScore = GetRobotMatchScore();
		}

        member->SetScoreChange(MATCH_RACE_RESULT_WIN, _CountMatchScoreChange(score, opponentScore, MATCH_RACE_RESULT_WIN));
        member->SetScoreChange(MATCH_RACE_RESULT_LOSE, _CountMatchScoreChange(score, opponentScore, MATCH_RACE_RESULT_LOSE));
        member->SetScoreChange(MATCH_RACE_RESULT_DRAW, _CountMatchScoreChange(score, opponentScore, MATCH_RACE_RESULT_DRAW));
        member->SetScoreChange(MATCH_RACE_RESULT_ERROR, 0);
    }
}

Int32 SeasonPvPMatchRace::_CountMatchScoreChange(UInt32 score, UInt32 opponeScore, MatchRaceResult result)
{
    /*
    
            EOL等级分公式为：Ra+1= Ra+K*(W-E)																			
																			
        1）	 Ra+1为比赛后的等级积分，Ra比赛前的等级积分，即一局比赛后，等级增长积分为K*(W-E)，等级增长积分可以为正，也可以为负。																		
	        通常比赛胜利，等级分会增加，比赛失败，等级积分会减少。																		
																			
        2）	W为比赛结果，胜利为1.0，平手为0.5，失败为0，当然也可以根据自身的系统需求进行调整。																		
																			
        3）	E为比赛的期望，由赛前比赛双方的积分决定，计算公式如下：																		
	        E=1/(1+10^(Rb-Ra)/400)					Ra和Rb代表双方积分													
																			
        4）	K为比赛的浮动系数，随着比赛的次数增加，玩家的积分与真实水平越接近，这个浮动系数变化范围减少(根据积分段适配)。		

    */

	/*
		新加规则：
			1. 平局不加分和扣分
							add by zjy
							2017.04.27
	*/

    if (result == MATCH_RACE_RESULT_ERROR || result == MATCH_RACE_RESULT_DRAW)
    {
        return 0;
    }

    float W = 0.0f;
    if (result == MATCH_RACE_RESULT_WIN)
    {
        W = 1.0f;
    }
    else if (result == MATCH_RACE_RESULT_DRAW)
    {
        W = 0.5f;
    }

    float E = 1.0f / (1.0f + (float)pow(10, ((Int32)opponeScore - (Int32)score) / 400.0f));

    float K = 1.0f;
	// 积分段修正
	struct MatchScoreLevelAdapt {
		UInt32 matchScore;
		UInt32 value;
	};
	static MatchScoreLevelAdapt s_matchScoreLevelAdapt[] = {
		{ 1200, 40 }, { 1400, 36 }, { 1600, 32 },
		{ 1700, 30 }, { 1900, 28 }, { 2000, 26 },
		{ 2100, 24 }, { 2200, 20 }, { 2300, 18 },
		{ 2400, 16 }, { 2500, 14 }, { 2600, 12 },
		{ 2700, 10 }, { 2800, 8 }, { 2900, 6 },
		{ 3000, 4 }, { 9999999, 2 }
	};

	//int iLevelScore = 0;
	for (size_t i = 0; i < sizeof(s_matchScoreLevelAdapt) / sizeof(s_matchScoreLevelAdapt[0]); i++)
	{
		if (score <= s_matchScoreLevelAdapt[i].matchScore)
		{
			K = (float)s_matchScoreLevelAdapt[i].value;
			break;
		}
	}

    Int32 newScore = score + (Int32)(K * (W - E));
    const Int32 MIN_SCORE = 0;
    const Int32 MAX_SCORE = 5000;

    newScore = newScore < MIN_SCORE ? MIN_SCORE : newScore;
    newScore = newScore > MAX_SCORE ? MAX_SCORE : newScore;

    return (Int32)newScore - (Int32)score;
}