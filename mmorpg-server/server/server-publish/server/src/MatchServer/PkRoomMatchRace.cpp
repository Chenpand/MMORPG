#include "PkRoomMatchRace.h"
#include "MSRouter.h"
#include <CLRoomDefine.h>

PkRoomMatchRace::PkRoomMatchRace(PkType pkType)
{
	m_PkType = pkType;
}

PkRoomMatchRace::~PkRoomMatchRace()
{

}

bool PkRoomMatchRace::OnRaceEnd(EGameSessionDismissType ret, const void* raceEnd)
{
	InfoLogStream << "race (" << GetID() << ") recv race end, ret(" << ret << ")" << LogStream::eos;

	PkRaceEndInfo raceEndInfo = *(PkRaceEndInfo*)raceEnd;

	if (GetState() != MRS_RACE)
	{
		ErrorLogStream << "race (" << GetID() << ") not racing, state(" << GetState() << ")." << LogStream::eos;
		return false;
	}

	std::set<UInt32> wsIds;
	CLProtocol::MatchServerPkRoomRaceEnd endReq;
	endReq.raceId = GetID();
	endReq.type = m_PkType;
	endReq.dungeonID = GetDungeonID();
	endReq.usedTime = GetUsedTime();

	auto allMembers = GetAllMember();
	std::vector<UInt32> memberScoreBySlot[ROOM_SLOT_GROUP_NUM];
	UInt32 slotScores[ROOM_SLOT_GROUP_NUM];
	memset(slotScores, 0, sizeof(slotScores));

	for (auto member : allMembers)
	{
		auto slot = GetRoomSlotGroup(member->GetSeat());
		if (slot >= ROOM_SLOT_GROUP_NUM)
		{
			continue;
		}

		memberScoreBySlot[slot].push_back(member->GetScore());
	}

	slotScores[ROOM_SLOT_GROUP_RED] = _GetRoomScore(memberScoreBySlot[ROOM_SLOT_GROUP_RED]);
	slotScores[ROOM_SLOT_GROUP_BLUE] = _GetRoomScore(memberScoreBySlot[ROOM_SLOT_GROUP_BLUE]);

	for (auto& info : raceEndInfo.results)
	{
		auto member = FindMember(info.roleId);
		if (!member)
		{
			InfoLogStream << "race (" << GetID() << ") can't find member(" << info.roleId << ")." << LogStream::eos;
			continue;
		}

		wsIds.insert(member->GetWsID());

		Pk3V3PlayerRaceEndInfo result;
		result.roomId = member->GetTeamID();
		result.info = info;
		result.matchScore = member->GetScore();
		result.seasonLevel = member->GetSeasonLevel();
		result.seasonStar = member->GetSeasonStar();
		result.seasonExp = member->GetSeasonExp();

		do 
		{
			result.matchScoreChange = 0;
			if (m_PkType != PK_3V3_MATCH)
			{
				break;
			}

			auto slot = GetRoomSlotGroup(member->GetSeat());
			if (slot == ROOM_SLOT_GROUP_RED)
			{
				result.matchScoreChange = _CountMatchScoreChange(member->GetScore(), slotScores[ROOM_SLOT_GROUP_BLUE], GetRaceResult(result.info.resultFlag));
			}
			else if (slot == ROOM_SLOT_GROUP_BLUE)
			{
				result.matchScoreChange = _CountMatchScoreChange(member->GetScore(), slotScores[ROOM_SLOT_GROUP_RED], GetRaceResult(result.info.resultFlag));
			}
			else
			{
				break;
			}
		} while (0);
		
		endReq.results.push_back(result);
	}

	for (auto id : wsIds)
	{
		MSRouter::Instance()->SendToWorld(id, endReq);
	}
	return true;
}

UInt32 PkRoomMatchRace::_GetRoomScore(std::vector<UInt32> scores)
{
	std::sort(scores.begin(), scores.end());

	if (scores.size() == 0)
	{
		return 0;
	}
	else if (scores.size() == 1)
	{
		return scores[0];
	}
	else if (scores.size() == 2)
	{
		return (scores[0] * 20 + scores[1] * 80) / 100;
	}
	else
	{
		return (scores[0] * 10 + scores[1] * 20 + scores[2] * 70) / 100;
	}

	return 0;
}

Int32 PkRoomMatchRace::_CountMatchScoreChange(UInt32 score, UInt32 opponeScore, MatchRaceResult result)
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

	float E = 1.0 / (1.0 + (float)pow(10, ((Int32)opponeScore - (Int32)score) / 400.0f));

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