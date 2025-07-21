#include "Pk3V3ActivityMatchService.h"

UInt32 Pk3V3ActivityMatchService::AdjuestMatchScore(MatchTeamMember* member)
{
	if (!member)
	{
		return SEASON_MIN_MATCH_SCORE;
	}

	/*
	胜负情况	修正分值
	连胜1		100
	连胜2		250
	连胜3		450
	连胜4		500
	连胜5		600
	连负1		-50
	连负2		-100
	连负3		-150
	连负4		-200
	连负5		-300

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