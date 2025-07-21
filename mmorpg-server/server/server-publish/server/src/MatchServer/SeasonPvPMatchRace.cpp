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
		// û�ж��֣�˵���Ǵ������
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
    
            EOL�ȼ��ֹ�ʽΪ��Ra+1= Ra+K*(W-E)																			
																			
        1��	 Ra+1Ϊ������ĵȼ����֣�Ra����ǰ�ĵȼ����֣���һ�ֱ����󣬵ȼ���������ΪK*(W-E)���ȼ��������ֿ���Ϊ����Ҳ����Ϊ����																		
	        ͨ������ʤ�����ȼ��ֻ����ӣ�����ʧ�ܣ��ȼ����ֻ���١�																		
																			
        2��	WΪ���������ʤ��Ϊ1.0��ƽ��Ϊ0.5��ʧ��Ϊ0����ȻҲ���Ը��������ϵͳ������е�����																		
																			
        3��	EΪ����������������ǰ����˫���Ļ��־��������㹫ʽ���£�																		
	        E=1/(1+10^(Rb-Ra)/400)					Ra��Rb����˫������													
																			
        4��	KΪ�����ĸ���ϵ�������ű����Ĵ������ӣ���ҵĻ�������ʵˮƽԽ�ӽ����������ϵ���仯��Χ����(���ݻ��ֶ�����)��		

    */

	/*
		�¼ӹ���
			1. ƽ�ֲ��ӷֺͿ۷�
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
	// ���ֶ�����
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