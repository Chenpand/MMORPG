#include "PremiumLeagueGamer.h"
#include <CLPremiumLeagueProtocol.h>
#include <CLRecordClient.h>

#include "WSPlayer.h"
#include "WSSortListMgr.h"
#include "WSPlayerMgr.h"
#include "WSRouter.h"
#include "PremiumLeagueMgr.h"
#include "WSSceneMgr.h"
#include <CLHonor.h>

PremiumLeagueGamer::PremiumLeagueGamer()
{
	m_Occu = 0;
	m_Level = 0;
	m_EnrollTime = 0;
	m_EnrollCount = 0;
	m_RemainHP = HP_MP_FULL_VALUE;
	m_RemainMP = HP_MP_FULL_VALUE;
	m_WinTime = 0;
	m_WinNum = 0;
	m_Ranking = 0;
	m_NextRollPos = 0;
	m_BattleFlag = 0;
	m_IsFinishRollBattle = 0;
	m_FinalBattleWinNum = 0;
	m_Score = 0;
	m_Player = NULL;
	m_Dirty = false;
}

PremiumLeagueGamer::~PremiumLeagueGamer()
{

}

bool PremiumLeagueGamer::Init(WSPlayer* player)
{
	if (!player)
	{
		return false;
	}

	SetID(player->GetID());
	SetName(player->GetName());
	m_Occu = player->GetOccu();
	m_Level = player->GetLevel();
	SetPlayer(player);
	return true;
}

void PremiumLeagueGamer::Clear()
{
	SetWinNum(0);
	SetScore(0);
	SetEnrollCount(0);
}

void PremiumLeagueGamer::SyncSelfInfo()
{
	CLProtocol::WorldPremiumLeagueSelfInfo sync;
	sync.info.winNum = GetWinNum();
	sync.info.score = GetScore();
	sync.info.ranking = GetRanking();
	sync.info.enrollCount = GetEnrollCount();
	sync.info.loseNum = IsLose() ? 1 : 0;
	sync.info.preliminayRewardNum = GetPreliminayRewardNum();
	SendProtocol(sync);

	SetDirty(false);
}

void PremiumLeagueGamer::SetScore(UInt32 score)
{
	if (score == m_Score)
	{
		return;
	}

	m_Score = score;
	SetDirty(true);
}

void PremiumLeagueGamer::SetRemainHp(UInt32 hp)
{
	if (hp == m_RemainHP)
	{
		return;
	}

	m_RemainHP = hp;
	SetDirty(true);
}

void PremiumLeagueGamer::SetWinNum(UInt32 num)
{
	if (num == m_WinNum)
	{
		return;
	}

	m_WinNum = num;
	SetDirty(true);
}

void PremiumLeagueGamer::SetEnrollCount(UInt32 count)
{
	if (count == m_EnrollCount)
	{
		return;
	}

	m_EnrollCount = count;
	SetDirty(true);
}

void PremiumLeagueGamer::SetRanking(UInt32 ranking)
{
	if (ranking == m_Ranking)
	{
		return;
	}

	m_Ranking = ranking;
	SetDirty(true);
}

void PremiumLeagueGamer::OnEnroll()
{
	m_EnrollCount = m_EnrollCount + 1;
	SetRemainHp(HP_MP_FULL_VALUE);
	SetRemainMp(HP_MP_FULL_VALUE);
	SetWinNum(0);
	SetScore(0);
	SetEnrollTime((UInt32)CURRENT_TIME.Sec());
	SetWinTime((UInt32)CURRENT_TIME.Sec());

	SyncSelfInfo();
}

void PremiumLeagueGamer::UpdateSortList()
{
	SortListEntry entry;
	entry.sortType = SORTLIST_PREMIUM_LEAGUE;
	entry.id = GetID();
	entry.name = GetName();
	entry.occu = GetOccu();
	entry.level = GetLevel();
	entry.value1 = GetScore();
	entry.value2 = GetWinTime();
	entry.value3 = GetScore();

	auto oldEntry = WSSortListMgr::Instance()->FindEntry(SORTLIST_PREMIUM_LEAGUE, GetID());
	if (!oldEntry || oldEntry->GetSortFactor() < entry.GetSortFactor())
	{
		WSSortListMgr::Instance()->UpdateSortList(entry);
	}
	else
	{
		WSSortListMgr::Instance()->OnUpdateValue3(SORTLIST_PREMIUM_LEAGUE, GetID(), GetScore());
	}
}

void PremiumLeagueGamer::OnBattleWin()
{
	DebugLogStream << "premium league battle player(" << GetID() << ") win." << LogStream::eos;

	SetFinalBattleWinNum(GetFinalBattleWinNum() + 1);
	CLProtocol::WorldPremiumLeagueBattleGamerUpdate update;
	update.roleId = GetID();
	update.winNum = GetFinalBattleWinNum();
	update.isLose = 0;
	Router::BroadcastToPlayer(update);

	// 更新淘汰赛标志
	m_BattleFlag = (m_BattleFlag << 1) | 0x01;

	SetFinishRollBattle(true);
	SaveToDB();

	// 更新战斗记录
	auto battleRecord = PremiumLeagueMgr::Instance()->FindBattleByRoleAndStatus(GetID(), PremiumLeagueMgr::Instance()->GetStatus());
	if (!battleRecord)
	{
		ErrorLogStream << "can't find premium battle by role(" << GetID() << ") status(" << (UInt8)PremiumLeagueMgr::Instance()->GetStatus() << ")." << LogStream::eos;
	}
	else
	{
		battleRecord->isEnd = 1;
		battleRecord->winnerId = GetID();
		battleRecord->UpdateToDB();
		battleRecord->SyncToPlayer();
	}

	// 结算
	CLProtocol::WorldPremiumLeagueRaceEnd ret;
	ret.result = resultBitmask_win;
	ret.isPreliminay = 0;
	SendProtocol(ret);

	PremiumLeagueMgr::Instance()->SendHonorToScene(GetID(), PK_PREMIUM_LEAGUE_BATTLE, 0);
}

void PremiumLeagueGamer::OnBattleLose()
{
	DebugLogStream << "premium league battle player(" << GetID() << ") lose." << LogStream::eos;
	SetRemainHp(0);
	SetRemainMp(0);
	SetNextRollPos(0);
	SetFinishRollBattle(true);

	SaveToDB();

	// 结算
	CLProtocol::WorldPremiumLeagueRaceEnd ret;
	ret.result = resultBitmask_lose;
	ret.isPreliminay = 0;
	SendProtocol(ret);

	PremiumLeagueMgr::Instance()->SendHonorToScene(GetID(), PK_PREMIUM_LEAGUE_BATTLE, 0);

	CLProtocol::WorldPremiumLeagueBattleGamerUpdate update;
	update.roleId = GetID();
	update.winNum = GetFinalBattleWinNum();
	update.isLose = 1;
	Router::BroadcastToPlayer(update);
}

void PremiumLeagueGamer::SendProtocol(Avalon::Protocol& protocol)
{
	if (GetPlayer())
	{
		GetPlayer()->SendProtocol(protocol);
	}
}

void PremiumLeagueGamer::SaveToDB()
{
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_premium_league_gamer", GetID(), false);
	GetDBData(updateCmd, true);
	if (updateCmd->GetDataNum() > 0)
	{
		CLRecordClient::Instance()->SendCommand(updateCmd);
	}
	else
	{
		CL_SAFE_DELETE(updateCmd);
	}
}

void PremiumLeagueGamer::InsertToDB()
{
	CLInsertCommand* insertCmd = CLRecordClient::Instance()->PrepareInsertCommand("t_premium_league_gamer", GetID());
	GetDBData(insertCmd, false);
	insertCmd->PutData("name", GetName());
	CLRecordClient::Instance()->SendCommand(insertCmd);

	ClearDirty();
}

void PremiumLeagueGamer::DelFromDB()
{
	CLDeleteCommand* deleteCmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_premium_league_gamer", GetID());
	CLRecordClient::Instance()->SendCommand(deleteCmd);
}

PremiumLeagueBattleGamer PremiumLeagueGamer::GetBattleGamerInfo() const
{
	PremiumLeagueBattleGamer info;
	info.roleId = GetID();
	info.name = GetName();
	info.occu = GetOccu();
	info.ranking = GetRanking();
	info.winNum = GetFinalBattleWinNum();
	info.isLose = IsLose() ? 1 : 0;

	return info;

}

PremiumLeagueBattleFighter PremiumLeagueGamer::GetBattleFighterInfo() const
{
	PremiumLeagueBattleFighter info;
	info.id = GetID();
	info.name = GetName();
	info.occu = GetOccu();
	return info;
}

bool PremiumLeagueGamer::CanStartBattle()
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(GetID());
	if (!player)
	{
		return false;
	}

	auto scene = WSSceneMgr::Instance()->FindScene(player->GetSceneID());
	if (!scene->GetDataEntry() || scene->GetDataEntry()->subType != SCENE_SUB_PREMIUM_LEAGUE)
	{
		return false;
	}

	/*if (player->GetTeam())
	{
		return false;
	}*/

	return true;
}