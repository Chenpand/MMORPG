#include "ChampionScene.h"
#include "USRouter.h"
#include "ChampionPlayer.h"
#include <CLMatchProtocol.h>
#include "ChampionService.h"

void ChampionScene::StartBattle(ChampionStatus status)
{
	CLProtocol::MatchServerChampionBattleReq req;

	auto playerSet = GetCurScenePlayer();
	for (auto i : playerSet)
	{
		auto player = static_cast<ChampionPlayer*>(i);
		if (!player->GetOnline() || !IsLogicSameStatus(player->GetStatus(), status))
		{
			continue;
		}
		req.roleIds.push_back(player->GetID());
	}
	std::random_shuffle(req.roleIds.begin(), req.roleIds.end());
	USRouter::Instance()->SendToMatch(req);
}

std::vector<ChampionPlayer*> ChampionScene::GetTopScorePlayer(UInt32 top)
{
	std::vector<ChampionPlayer*> vec;
	auto playerSet = GetCurScenePlayer();
	for (auto& i : playerSet)
	{
		vec.push_back(static_cast<ChampionPlayer*>(i));
	}
	std::sort(vec.begin(), vec.end(), [&](ChampionPlayer* a, ChampionPlayer* b)->bool {
		if (a->GetScore() > b->GetScore())
		{
			return true;
		}
		else if (a->GetScore() == b->GetScore())
		{
			if (a->GetScoreChangeTime() < b->GetSceneNodeId())
			{
				return true;
			}
		}
		return false;
	});
	vec.erase(vec.begin() + top, vec.end());
	return vec;
}

