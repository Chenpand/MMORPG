#include "ScoreWarMgr.h"
#include "PlayerMgr.h"
#include <CLScoreWarProtocol.h>
#include "SSRouter.h"

bool ScoreWarMgr::Init()
{
	return true;
}

void ScoreWarMgr::OnTick(Avalon::Time now)
{
	if (m_IsInit) return;

	if (m_LastInitTime + 5 < (UInt32)now.Sec())
	{
		CLProtocol::CrossScoreWarInfoReq req;
		Router::SendToCross(req);
	}

	m_LastInitTime = (UInt32)now.Sec();
}

void ScoreWarMgr::OnCrossSyncScoreWarInfo(ScoreWarStatus status, UInt32 statusEndTime, UInt32 startTime)
{
	SetScoreWarStatus((ScoreWarStatus)status);
	SetStatusEndTime(statusEndTime);
	SetStartTime(startTime);
	m_IsInit = true;

	class SyncScoreWarInfoPlayerVisitor : public PlayerVisitor
	{
	public:
		SyncScoreWarInfoPlayerVisitor() {}
		~SyncScoreWarInfoPlayerVisitor() {}

		bool operator()(Player* pPlayer)
		{
			ScoreWarMgr::Instance()->SyncScoreWarInfo(pPlayer);
			return true;
		}
	};

	SyncScoreWarInfoPlayerVisitor visitor;
	PlayerMgr::Instance()->VisitNormalPlayers(visitor);

}

void ScoreWarMgr::SyncScoreWarInfo(Player* player)
{
	if (GetScoreWarStatus() != ScoreWarStatus::SWS_INVALID && player->GetScoreWarLastBattleTime() < GetStartTime())
	{
		UInt32 battleCount = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_SCORE_WAR_BATTLE_COUNT);
		player->SetScoreWarWinBattleCount(0);
		player->SetScoreWarBattleCount(battleCount);
		player->SetScoreWarScore(0);
		player->ClearScoreWarReward();
		player->InitScoreWarStraightCount();
	}

	CLProtocol::SceneSyncScoreWarInfo info;
	info.status = (UInt8)GetScoreWarStatus();
	info.statusEndTime = GetStatusEndTime();
	player->SendProtocol(info);

	SyncScoreWarBattleInfo(player);
}

void ScoreWarMgr::SyncScoreWarBattleInfo(Player* player)
{
	if (player == NULL) return;
	CLProtocol::WorldScoreWarSyncBattleInfo sync;
	sync.battleCount = player->GetScoreWarBattleCount();
	sync.loseStreak = player->GetScoreWarLoseStraight();
	sync.matchScore = player->GetMatchScore();
	sync.playerId = player->GetID();
	sync.winStreak = player->GetScoreWarWinStraight();
	Router::SendToWorld(sync);
}

bool ScoreWarMgr::IsBattle()
{
	return m_Status == ScoreWarStatus::SWS_BATTLE || m_Status == ScoreWarStatus::SWS_WAIT_END;
}

bool ScoreWarMgr::IsMatch()
{
	return m_Status == ScoreWarStatus::SWS_BATTLE;
}
