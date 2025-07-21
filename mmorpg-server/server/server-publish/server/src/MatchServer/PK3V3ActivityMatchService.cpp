#include "Pk3V3ActivityMatchService.h"

UInt32 Pk3V3ActivityMatchService::AdjuestMatchScore(MatchTeamMember* member)
{
	if (!member)
	{
		return SEASON_MIN_MATCH_SCORE;
	}

	/*
	ʤ�����	������ֵ
	��ʤ1		100
	��ʤ2		250
	��ʤ3		450
	��ʤ4		500
	��ʤ5		600
	����1		-50
	����2		-100
	����3		-150
	����4		-200
	����5		-300

	*/

	int score = member->GetScore();

	if (member->GetWinStreak() == 1)
	{
		score += 100;
	}
	if (member->GetWinStreak() == 2)
	{
		score += 250;
	}
	else if (member->GetWinStreak() == 3)
	{
		score += 450;
	}
	else if (member->GetWinStreak() == 4)
	{
		score += 500;
	}
	else if (member->GetWinStreak() == 5)
	{
		score += 600;
	}
	else if (member->GetloseStreak() == 1)
	{
		score -= 50;
	}
	else if (member->GetloseStreak() == 2)
	{
		score -= 100;
	}
	else if (member->GetloseStreak() == 3)
	{
		score -= 150;
	}
	else if (member->GetloseStreak() == 4)
	{
		score -= 200;
	}
	else if (member->GetloseStreak() == 5)
	{
		score -= 300;
	}

	score = (score < (int)SEASON_MIN_MATCH_SCORE) ? (int)SEASON_MIN_MATCH_SCORE : score;
	member->SetScore(score);
	return score;
}