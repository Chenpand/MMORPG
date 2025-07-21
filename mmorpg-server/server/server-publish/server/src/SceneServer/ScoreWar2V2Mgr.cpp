#include "ScoreWar2V2Mgr.h"
#include "PlayerMgr.h"
#include "SSRouter.h"
#include <CL2V2ScoreWarProtocol.h>

bool ScoreWar2V2Mgr::Init()
{
	return true;
}

void ScoreWar2V2Mgr::OnTick(Avalon::Time now)
{
	if (m_IsInit) return;

	if (m_LastInitTime + 5 < (UInt32)now.Sec())
	{
		CLProtocol::Cross2V2ScoreWarInfoReq req;
		Router::SendToCross(req);
	}

	m_LastInitTime = (UInt32)now.Sec();
}

void ScoreWar2V2Mgr::OnCrossSyncScoreWarInfo(ScoreWar2V2Status status, UInt32 statusEndTime, UInt32 startTime)
{
	SetScoreWarStatus((ScoreWar2V2Status)status);
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
			ScoreWar2V2Mgr::Instance()->SyncScoreWarInfo(pPlayer);
			return true;
		}
	};

	SyncScoreWarInfoPlayerVisitor visitor;
	PlayerMgr::Instance()->VisitNormalPlayers(visitor);

}

void ScoreWar2V2Mgr::SyncScoreWarInfo(Player* player)
{
	if (GetScoreWarStatus() != ScoreWar2V2Status::SWS_2V2_INVALID && player->Get2V2ScoreWarLastBattleTime() < GetStartTime())
	{
		UInt32 battleCount = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_2V2_SCORE_WAR_PARTAKE_NUM);
		player->Set2V2ScoreWarWinBattleCount(0);
		player->Set2V2ScoreWarBattleCount(battleCount);
		player->Set2V2ScoreWarScore(SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_2V2_SCORE_WAR_START_SCORE));
		player->Clear2V2ScoreWarReward();
	}

	CLProtocol::Scene2V2SyncScoreWarInfo info;
	info.status = (UInt8)GetScoreWarStatus();
	info.statusEndTime = GetStatusEndTime();
	player->SendProtocol(info);

	//SyncScoreWarBattleInfo(player);
}

void ScoreWar2V2Mgr::SyncScoreWarBattleInfo(Player* player)
{
	if (player == NULL) return;
	CLProtocol::World2V2ScoreWarSyncBattleInfo sync;
	sync.battleCount = player->Get2V2ScoreWarBattleCount();
	sync.matchScore = player->Get2V2ScoreWarScore();
	sync.playerId = player->GetID();
	Router::SendToWorld(sync);
}

bool ScoreWar2V2Mgr::IsBattle()
{
	return m_Status == ScoreWar2V2Status::SWS_2V2_BATTLE || m_Status == ScoreWar2V2Status::SWS_2V2_WAIT_END;
}

bool ScoreWar2V2Mgr::IsMatch()
{
	return m_Status == ScoreWar2V2Status::SWS_2V2_BATTLE;
}
