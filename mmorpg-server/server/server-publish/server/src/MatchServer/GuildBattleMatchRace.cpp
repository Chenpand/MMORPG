#include "GuildBattleMatchRace.h"
#include "MSRouter.h"

bool GuildBattleMatchRace::OnRaceEnd(EGameSessionDismissType ret, const void* raceEnd)
{
	InfoLogStream << "race (" << GetID() << ") recv race end, ret(" << ret << ")" << LogStream::eos;

	PkRaceEndInfo raceEndInfo = *(PkRaceEndInfo*)raceEnd;

	if (GetState() != MRS_RACE)
	{
		ErrorLogStream << "race (" << GetID() << ") not racing, state(" << GetState() << ")." << LogStream::eos;
		return false;
	}

	// 结果为空
	if (raceEndInfo.empty())
	{
		ErrorLogStream << "race (" << GetID() << ") result is empty." << LogStream::eos;
		return false;
	}

	CLProtocol::MatchServerGuildBattleRaceEnd endReq;
	endReq.sessionId = raceEndInfo.gamesessionId;
	endReq.result = 0;
	std::set<UInt32> wsIds;
	for (auto& result : raceEndInfo.results)
	{
		auto member = FindMember(result.roleId);
		if (!member)
		{
			InfoLogStream << "race (" << GetID() << ") can't find member(" << result.roleId << ")." << LogStream::eos;
			continue;
		}

		if (wsIds.find(member->GetWsID()) == wsIds.end())
		{
			wsIds.insert(member->GetWsID());
		}

		auto resultType = GetRaceResult(result.resultFlag);
		if (resultType == MATCH_RACE_RESULT_WIN)
		{
			endReq.winner.id = member->GetID();
			endReq.winner.name = member->GetName();
			endReq.winner.serverName = member->GetRaceInfo().serverName;
			endReq.winner.guildName = member->GetRaceInfo().guildName;
			endReq.winerRemainHp = result.remainHp;
			endReq.winerRemainMp = result.remainMp;
		}
		else if (resultType == MATCH_RACE_RESULT_LOSE)
		{
			endReq.loser.id = member->GetID();
			endReq.loser.name = member->GetName();
			endReq.loser.serverName = member->GetRaceInfo().serverName;
			endReq.loser.guildName = member->GetRaceInfo().guildName;
		}
		else
		{
			endReq.result = resultType == MATCH_RACE_RESULT_DRAW ? resultBitmask_draw : resultBitmask_invalid;

			if (endReq.winner.id == 0)
			{
				endReq.winner.id = member->GetID();
				endReq.winner.name = member->GetName();
				endReq.winner.serverName = member->GetRaceInfo().serverName;
				endReq.winner.guildName = member->GetRaceInfo().guildName;
			}
			else
			{
				endReq.loser.id = member->GetID();
				endReq.loser.name = member->GetName();
				endReq.loser.serverName = member->GetRaceInfo().serverName;
				endReq.loser.guildName = member->GetRaceInfo().guildName;
			}
		}
	}

	// 使用了机器人
	if (m_robot)
	{
		if (endReq.winner.id == 0)
		{
			endReq.winner.name = m_robot->raceInfo.name;
		}
		else
		{
			endReq.loser.name = m_robot->raceInfo.name;
		}
	}

	for (auto wsId : wsIds)
	{
		MSRouter::Instance()->SendToWorld(wsId, endReq);
	}
	
	return true;
}

void GuildBattleMatchRace::CountMatchScoreChanges()
{
	VMatchTeamMember members = GetAllMember();
	for (auto member : members)
	{
		if (!member)
		{
			continue;
		}

		UInt32 score = member->GetScore();
		UInt32 opponentScore = 0;
		auto opponent = GetOpponent(member->GetID());
		// 没有对手，说明是打机器人
		if (opponent)
		{
			opponentScore = opponent->GetScore();
		}

		member->SetScoreChange(MATCH_RACE_RESULT_WIN, CountWinScoreAddition(score, opponentScore));
		member->SetScoreChange(MATCH_RACE_RESULT_LOSE, 50);
		member->SetScoreChange(MATCH_RACE_RESULT_DRAW, 0);
		member->SetScoreChange(MATCH_RACE_RESULT_ERROR, 0);
	}
}

int GuildBattleMatchRace::CountWinScoreAddition(UInt32 score, UInt32 opponeScore)
{
	// 连胜额外积分加成
	int winStreakAddition[] = { 0, 30, 50, 80, 140 };
	// 终结对方连胜积分加成
	int breakWinStreakAddition[] = { 0, 0, 50, 75, 100 };

	// 胜利的基础加成
	int scoreAdd = 100;

	// 连胜额外积分加成
	auto winStreakAdditionSize = sizeof(winStreakAddition) / sizeof(winStreakAddition[0]);
	if (score >= winStreakAdditionSize)
	{
		scoreAdd += winStreakAddition[winStreakAdditionSize - 1];
	}
	else
	{
		scoreAdd += winStreakAddition[score];
	}

	// 终结对方连胜积分加成
	auto breakWinStreakAdditionSize = sizeof(breakWinStreakAddition) / sizeof(breakWinStreakAddition[0]);
	if (opponeScore >= breakWinStreakAdditionSize)
	{
		scoreAdd += breakWinStreakAddition[breakWinStreakAdditionSize - 1];
	}
	else
	{
		scoreAdd += breakWinStreakAddition[opponeScore];
	}

	return scoreAdd;
}