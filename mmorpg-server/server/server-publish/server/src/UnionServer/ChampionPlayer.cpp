#include "ChampionPlayer.h"
#include <CLChampionDefine.h>
#include "ChampionService.h"
#include "UnionServiceMgr.h"

ChampionPlayer::ChampionPlayer()
{
	Init();
}

void ChampionPlayer::Init()
{
	m_SaveTimer.SetInterval(1000);
	m_Status = static_cast<UInt8>(ChampionStatus::CS_NULL);
	m_Supporter = 0;
	m_Score = 0;
	m_ScoreChangeTime = 0;
	m_Group = 0;
	m_WinCount = 0;
	m_Rank = 0;
	m_IsWin = 0;
	m_BattleCount = 0;
	m_LoseCount = 0;
	m_SceneID = 0;
	m_Pos = 0;
	m_BoOnline = false;
	m_IsInRace = false;
}

void ChampionPlayer::OnTick(const Avalon::Time& now)
{
	USPlayer::OnTick(now);

	if (m_SaveTimer.IsTimeout(now))
	{
		SaveToDB();
	}
}

bool ChampionPlayer::CanSendProtocol()
{
	return true;
}

void ChampionPlayer::SaveToDB()
{
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_champion_role_list", GetID(), false);
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

void ChampionPlayer::InsterToDB()
{
	CLInsertCommand* insertCmd = CLRecordClient::Instance()->PrepareInsertCommand("t_champion_role_list", GetID());
	GetDBData(insertCmd, false);
	CLRecordClient::Instance()->SendCommand(insertCmd);
	ClearDirty();
}

void ChampionPlayer::LoadDBData(CLRecordCallback* callback)
{
	SetID(callback->GetKey());
	SetDBData(callback);
	SetOnline(false);
}

void ChampionPlayer::SyncBattleCount()
{
	CLProtocol::SceneChampionSelfBattleRecordRes sync;
	sync.win = GetWinCount();
	sync.lose = GetLoseCount();
	if (GetLoseCount() == 1)
	{
		if (GetStatus() == CS_ENROLL)
		{
			sync.relive = CR_CAN;
		}
		else if (GetStatus() == CS_SEA_SELECT_PREPARE)
		{
			sync.relive = CR_ALREADY;
		}
	}
	else if (GetLoseCount() > 1)
	{
		sync.relive = CR_ALREADY;
	}
	else
	{
		sync.relive = CR_NO;
	}
	SendProtocol(sync);
}

void ChampionPlayer::SyncStatus()
{
	CLProtocol::SceneChampionSelfStatusRes sync;
	sync.status = GetStatus();
	SendProtocol(sync);
}

RankRole ChampionPlayer::GetRankRole() const
{
	//获得积分赛显示用数据
	RankRole role;
	role.roleID = GetID();
	role.name = GetName();
	role.score = GetScore();
	role.server = GetServer();
	return role;
}

void ChampionPlayer::SetStatus(ChampionStatus status)
{
	if (status != m_Status)
	{
		m_Status = status;
		SyncStatus();
	}
}

void ChampionPlayer::SetGroup(UInt32 groupNum)
{
	m_Group = groupNum;
}

void ChampionPlayer::SetScore(Int32 score)
{
	if (m_Score == score && m_ScoreChangeTime != 0)
	{
		return;
	}
	m_Score = score; 
	m_ScoreChangeTime = CURRENT_TIME.MSec();

	OnRankChanged();
}

void ChampionPlayer::SetBattleCount(UInt32 count)
{
	if (count == m_BattleCount)
	{
		return;
	}
	m_BattleCount = count;
	OnRankChanged();
}

void ChampionPlayer::SetIsInRace(bool bo)
{
	m_IsInRace = bo;
	InfoLogStream << PLAYERINFO(this) << " set " << (bo ? "true" : " false") << LogStream::eos;
}

void ChampionPlayer::SetIsNoTurn(bool bo)
{
	m_NoTurn = bo;
}

bool ChampionPlayer::CanStartRace(ChampionStatus status)
{
	if (IsInRace())
	{
		InfoLogStream << PLAYERINFO(this) << " cant start race reason is inrace" << LogStream::eos;
		return false;
	}

	if (!GetOnline())
	{
		InfoLogStream << PLAYERINFO(this) << " cant start race reason is not online" << LogStream::eos;
		return false;
	}

	if (!IsLogicSameStatus(status, GetStatus()))
	{
		InfoLogStream << PLAYERINFO(this) << " cant start race reason is not same status,self status is " 
			<< static_cast<UInt32>(GetStatus()) << " champion status is " << static_cast<UInt32>(status) << LogStream::eos;
		return false;
	}

	if (status == CS_SEA_SELECT_BATTLE && GetStatus() == CS_ENROLL)
	{
		InfoLogStream << PLAYERINFO(this) << " cant start race reason is CS_ENROLL" << LogStream::eos;
		return false;
	}

	return true;
}

void ChampionPlayer::AddBattledRole(UInt64 roleID)
{
	if (m_BattledRoleID.count(roleID) > 0)
	{
		return;
	}
	m_BattledRoleID.insert(roleID);
}

bool ChampionPlayer::CanBattleWith(UInt64 roleID)
{
	return m_BattledRoleID.count(roleID) == 0;
}

void ChampionPlayer::ClearBattledRole()
{
	m_BattledRoleID.clear();
}

void ChampionPlayer::NotifyNullTurn()
{
	CLProtocol::UnionChampionNullTrunNotify notify;
	SendProtocol(notify);
}

void ChampionPlayer::OnRankChanged()
{
	auto service = UnionServiceMgr::Instance()->GetUnionService<ChampionService>();
	if (service == nullptr)
	{
		return;
	}
	service->OnPlayerScoreChanged(this);
}
