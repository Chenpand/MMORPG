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

	float E = 1.0 / (1.0 + (float)pow(10, ((Int32)opponeScore - (Int32)score) / 400.0f));

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