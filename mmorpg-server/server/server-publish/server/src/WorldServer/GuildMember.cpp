#include "GuildMember.h"
#include <CLRecordClient.h>
#include <CLGuildProtocol.h>
#include <CLGameSessionDefine.h>
#include <CLMailDefine.h>
#include "WSPlayer.h"
#include "GuildMgr.h"
#include "WSSortListMgr.h"
#include "NewTitle.h"

GuildMember::GuildMember()
{
	SetID(0);
	m_TotalContri = 0;
	m_EnterTime = 0;
	m_LeaveTime = 0;
	m_OfflineTime = 0;
	m_GuildId = 0;
	m_Player = NULL;
	
	m_GuildBattleNumber = 0;
	m_GuildBattleScore = 0;
	m_GuildBattleWinStreak = 0;
	m_GuildBattleRewardMask.ClearMask();
	m_GuildBattleRewardMask.ClearDirty(0xFFFFFFFF);
	m_GuildBattleHP.SetDBValue(HP_MP_FULL_VALUE);
	m_GuildBattleMP.SetDBValue(HP_MP_FULL_VALUE);
	m_BattleStatus = GuildBattleMemberStatus::GBS_NORMAL;
	m_ActiveDegree = 0;
	m_ActiveUpdateTime = 0;
	m_VipLevel = 0;
	m_SeasonLevel = 0;
	m_EmblemLevel = 0;
	m_GuildTerrDayReward = 0;
}

GuildMember::~GuildMember()
{

}

bool GuildMember::Init(WSPlayer* player)
{
	if (!player)
	{
		return false;
	}

	SetPlayer(player);
	SetID(player->GetID());
	SetName(player->GetName());
	SetOccu(player->GetOccu());
	SetLevel(player->GetLevel());
	SetVipLevel(player->GetVipLvl());
	SetSeasonLevel(player->GetSeasonLv());
	SetPost(GUILD_POST_NORMAL, false);
	SetLeaveTime(0);
	SetOfflineTime(0);
	SetTotalContribution(0);
	return true;
}

void GuildMember::AddTotalContribution(UInt32 num)
{
	m_TotalContri = m_TotalContri + num;
}

void GuildMember::GetPlayerLabelInfo(PlayerLabelInfo& info)
{
	if (m_Player)
	{
		info.returnStatus = m_Player->IsVeteranReturn() ? 1 : 0;
		info.returnYearTitle = m_Player->GetReturnYearTitle();
	}
	info.guildId = m_GuildId;
}

void GuildMember::SetPost(GuildPost post, bool updateTitle)
{ 
	UInt8 oldPost = m_Post;
	m_Post = (UInt8)post; 
	if (m_Player && updateTitle)
	{
		m_Player->GetTitleMgr().OnPlayerGuildPostChange(oldPost, m_Post);
	}
}

GuildMemberEntry GuildMember::GetEntry()
{
	GuildMemberEntry entry;
	entry.id = GetID();
	entry.name = GetName();
	entry.level = GetLevel();
	entry.occu = GetOccu();
	entry.post = GetPost();
	entry.contribution = GetTotalContribution();
	entry.logoutTime = GetOfflineTime();
	entry.activeDegree = GetActiveDegree();
	entry.vipLevel = GetVipLevel();
	entry.seasonLevel = GetSeasonLevel();
	GetPlayerLabelInfo(entry.playerLabelInfo);
	return entry;
}

GuildTableMember GuildMember::GetTableMember(UInt8 seat, UInt8 type)
{
	GuildTableMember entry;
	entry.id = GetID();
	entry.level = GetLevel();
	entry.name = GetName();
	entry.occu = GetOccu();
	entry.seat = seat;
	entry.type = type;
	GetPlayerLabelInfo(entry.playerLabelInfo);
	return entry;
}

void GuildMember::SyncEventToScene(SyncEventType type, ObjID_t param1)
{
	if (!m_Player)
	{
		return;
	}

	m_Player->SyncEventToScene(type, param1);
}

bool GuildMember::IsInGuild() const
{
	if (GetID() == 0 || GetLeaveTime() > 0)
	{
		return false;
	}

	return true;
}

bool GuildMember::IsCanLottery() const
{
	UInt8 status = (UInt8)m_GuildBattleLotteryStatus;
	if (status == (UInt8)(GuildBattleLotteryStatus::GBLS_CAN))
		return true;
	return false;
}

void GuildMember::SendProtocol(Avalon::Protocol& protocol)
{
	if (m_Player != NULL)
		m_Player->SendProtocol(protocol);
}

void GuildMember::OnGuildBattleRaceEnd(GuildBattleMember& guildBattleMember, bool isWin, UInt32 terrId, UInt32 hp, UInt32 mp)
{
	Guild* guild = GuildMgr::Instance()->FindGuildByID(m_GuildId);
	if (guild == NULL) return;
	guild->OnGuildBattleRaceEnd(terrId, guildBattleMember.gotScore);

	SetGuildBattleStatus(GuildBattleMemberStatus::GBS_NORMAL);
	InfoLogStream << "guild(" << m_GuildId << ") member(" << GetID() << ") Set BattleStatus GBS_NORMAL" << LogStream::eos;

	UInt32 oldGuildScore = m_GuildBattleScore;
	m_GuildBattleScore = m_GuildBattleScore + guildBattleMember.gotScore;
	guildBattleMember.totalScore = m_GuildBattleScore;
	m_GuildBattleNumber = m_GuildBattleNumber + 1;
	SetGuildBattleLotteryStatus(GuildBattleLotteryStatus::GBLS_CAN);

	_resultBitmask raceEndBit;
	//PkRaceResult raceResult;
	if (isWin)
	{
		IncGuildBattleWinStreak();
		raceEndBit = _resultBitmask::resultBitmask_win;
		//raceResult = PkRaceResult::PKRR_WIN;
	}
	else
	{
		SetGuildBattleWinStreak(0);
		raceEndBit = _resultBitmask::resultBitmask_lose;
		//raceResult = PkRaceResult::PKRR_LOSE;
	}

	SetGuildBattleHP(hp);
	SetGuildBattleMP(mp);

	guild->SyncMemberInfoToScene(this);

	if (GetPlayer() != NULL)
	{
		GetPlayer()->OnIncGuildBattleScore(guildBattleMember.gotScore);

		CLProtocol::WorldGuildBattleRaceEnd res;
		res.result = (UInt8)raceEndBit;
		res.oldScore = oldGuildScore;
		res.newScore = m_GuildBattleScore;
		GetPlayer()->SendProtocol(res);
	}
}

void GuildMember::OnEnrollBegin()
{
	if (m_GuildBattleScore > 0)
	{
		m_GuildBattleRewardMask.ClearMask();
	}

	m_GuildBattleNumber = 0;
	m_GuildBattleScore = 0;
	m_GuildBattleWinStreak = 0;
	m_GuildBattleHP = HP_MP_FULL_VALUE;
	m_GuildBattleMP = HP_MP_FULL_VALUE;
	m_GuildBattleInspire = 0;
	SetGuildBattleLotteryStatus(GuildBattleLotteryStatus::GBLS_NOT);
	SetGuildBattleStatus(GuildBattleMemberStatus::GBS_NORMAL);
	

	Guild* guild = GuildMgr::Instance()->FindGuildByID(m_GuildId);
	if (guild != NULL)
	{
		guild->SyncMemberInfoToScene(this);
	}
}

void GuildMember::SetInspireSkill(std::vector<RaceSkillInfo>& skills)
{
	Guild* guild = GuildMgr::Instance()->FindGuildByID(m_GuildId);
	if (guild != NULL && guild->GetInspire() > 0)
	{
		RaceSkillInfo info;
		info.level = guild->GetInspire();
		info.id = guild->GetInspireSkillId();
		info.slot = 0;
		skills.push_back(info);
	}
}

UInt32 GuildMember::Inspire()
{
	Guild* guild = GuildMgr::Instance()->FindGuildByID(m_GuildId);
	if (guild == NULL) return ErrorCode::GUILD_BATTLE_NOT_EXIST;
	IncGuildBattleInspire();
	return guild->Inspire();
}

void GuildMember::SaveToDB()
{
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_guild_member", GetID(), false);
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

void GuildMember::InsertToDB()
{
	CLInsertCommand* insertCmd = CLRecordClient::Instance()->PrepareInsertCommand("t_guild_member", GetID());
	GetDBData(insertCmd, false);
	CLRecordClient::Instance()->SendCommand(insertCmd);

	ClearDirty();
}

void GuildMember::DelFromDB()
{
	CLDeleteCommand* deleteCmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_guild_member", GetID());
	CLRecordClient::Instance()->SendCommand(deleteCmd);
}