#include "TCSTeam.h"

#include <DropItemMgr.h>
#include <CLErrorCode.h>
#include <udplog_typedef.h>
#include <CLSysNotify.h>
#include <CLMatchProtocol.h>
#include <CLItemProtocol.h>
#include <CLTeamCopyProtocol.h>
#include <CLDungeonProtocol.h>
#include <CLSyncObjectProtocol.h>
#include <CLTeamCopyTargetDataEntry.h>
#include <CLTeamCopyFlopDataEntry.h>
#include <CLTeamCopyBossDataEntry.h>
#include <CLTeamCopyRollDataEntry.h>
#include <CLItemDataEntry.h>
#include <CLSceneDataEntry.h>
#include <CLCrossDungeonDefine.h>
#include <CLRecordClient.h>

#include "TCSPlayerMgr.h"
#include "GameZoneMgr.h"
#include "TCSTeamCheck.h"
#include "TCSTeamMgr.h"
#include "TCSDungeonMgr.h"
#include "SysUtility.h"
#include "TCSApplication.h"


// 根据难度获得尾部据点
UInt32 GetTailFieldId(UInt32 teamGrade)
{
	switch (teamGrade)
	{
	case TEAM_COPY_TEAM_GRADE_COMMON:
		return GetTeamCopyValue(TC_VALUE_COMMON_TAIL_FIELD_ID);
	case TEAM_COPY_TEAM_GRADE_DIFF:
		return GetTeamCopyValue(TC_VALUE_DIFF_TAIL_FIELD_ID);
	default:
		break;
	}

	return 0;
}

TCSTeam::TCSTeam()
{
	m_TeamId = 0;
	m_TeamGrade = TEAM_COPY_TEAM_GRADE_COMMON;
	m_Model = TEAM_COPY_TEAM_MODEL_CHALLENGE;
	m_CondCommission = 0;
	m_TotalCommission = 0;
	m_EquipScore = 0;
	m_Status = 0;
	m_ChiefId = 0;
	m_AutoAgreeGold = 0;
	m_CurStage = 0;
	m_PlanModel = TEAM_COPY_PLAN_MODEL_INVALID;
	m_GameOverTime = 0; 
	m_RecruitCD = 0;
	m_ConsumeFlag = false;

	m_VoteEndTime = 0;
	m_CheckCondEndTime = 0;
	m_ForceEndFlag = false;
	m_BattleQuitTeamNum = 0;
	m_ForceEndVoteTime = 0;
	m_ForceEndCoolTime = 0;
	m_RollEndStamp = 0;
}

TCSTeam::~TCSTeam()
{

}

bool TCSTeam::OnTick(const Avalon::Time& now)
{
	// roll奖励
	_OnRollTick(now);

	if (IsGameOver()) // 游戏结束
	{
		return false;
	}

	if (GetGameOverTime() != 0 && now.Sec() >= GetGameOverTime())
	{
		InfoLogStream << TEAMINFO(this) << "game over failed!" << LogStream::eos;
		GameOver(TC_GAME_OVER_REASON_TIME_OVER);
		return false;
	}

	// 通知团长可以强制结束
	if ( !GetForceEndFlag() && GetGameOverTime() - now.Sec() <= GetTeamCopyValue(TC_VALUE_FORCE_END_GAME_TIME))
	{
		SetForceEndFlag(true);
		_NotifyForceEndFlag();
	}

	// 检查条件超时
	if (GetCheckCondEndTime() != 0 && now.Sec() >= GetCheckCondEndTime())
	{
		_CheckCondTimeOut();
	}

	// 投票超时
	if (GetVoteEndTime() != 0 && now.Sec() >= GetVoteEndTime())
	{
		_VoteTimeOut();
	}

	// 强制结束投票超时
	if (GetForceEndVoteTime() != 0 && now.Sec() >= GetForceEndVoteTime())
	{
		ForceEndVoteTimeOut();
	}

	return true;
}

bool TCSTeam::IsYetStartBattle()
{
	return GetStatus() != TEAM_COPY_TEAM_STATUS_PARPARE;
}

bool TCSTeam::IsInBattle()
{
	return GetStatus() == TEAM_COPY_TEAM_STATUS_BATTLE;
}

bool TCSTeam::IsGameOver()
{
	return GetStatus() == TEAM_COPY_TEAM_STATUS_FAILED || GetStatus() == TEAM_COPY_TEAM_STATUS_VICTORY;
}

UInt32 TCSTeam::CommissionBonusNum()
{
	UInt32 commonPlayer = _CommonPlayerNum();
	if (commonPlayer == 0)
		return 0;

	// 佣金税率千分比
	UInt32 commissionTax = GetTeamCopyValue(TC_VALUE_COMMISSION_TAX);
	if (1000 > commissionTax)
	{
		return GetTotalCommission() * (1000 - commissionTax) / 1000 / commonPlayer;
	}
	else
	{
		ErrorLogStream << TEAMINFO(this) << "commission tax error:" << commissionTax << LogStream::eos;
		return GetTotalCommission() / commonPlayer;
	}
}

UInt32 TCSTeam::CheckJoinTeam(TCSPlayer* joinPlayer, UInt32 isGold)
{
	if (NULL == joinPlayer)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}

	if (IsYetStartBattle())
	{
		return ErrorCode::TEAM_COPY_IN_BATTLE;
	}

	if (TeamIsFull())
	{
		return ErrorCode::TEAM_JOIN_TEAM_FULL;
	}

	if (IsJoinTimeLimit(joinPlayer->GetID()))
	{
		return ErrorCode::TEAM_COPY_JOIN_TIME_LIMIT;
	}

	if (isGold && GetModel() != TEAM_COPY_TEAM_MODEL_GOLD)
	{
		return ErrorCode::TEAM_COPY_GOLD_NOT_JOIN;
	}

	if (!isGold && joinPlayer->GetEquipScore() < GetEquipScore())
	{
		return ErrorCode::TEAM_COPY_EQUIP_SCORE_NOT_ENOUGH;
	}

	if (joinPlayer->GetTeamType() != GetTeamType())
	{
		return ErrorCode::TEAM_COPY_TARGET_CUR_STATUS;
	}

	if (!joinPlayer->IsHasChallengeNum(GetTeamType(), GetTeamGrade()))
	{
		return ErrorCode::TEAM_COPY_NUM_LIMIT;
	}

	if (!joinPlayer->IsUnlockGrade(GetTeamType(), GetTeamGrade()))
	{
		return ErrorCode::TEAM_COPY_GRADE_LOCK;
	}

	return ErrorCode::SUCCESS;
}

UInt32 TCSTeam::JoinTeam(TCSPlayer* joinPlayer, UInt32 isGold)
{
	if (NULL == joinPlayer)
	{
		return ErrorCode::TEAM_COPY_PLAYER_LEAVE;
	}

	UInt32 checkRet = CheckJoinTeam(joinPlayer, isGold);
	if (checkRet != ErrorCode::SUCCESS)
	{
		return checkRet;
	}

	// 金主数量限制
	if (isGold != 0 && GetModel() == TEAM_COPY_TEAM_MODEL_GOLD && GoldPlayerNum() >= GetGoldMaxNum())
	{
		return GetGoldNumErrorCode();
	}

	TeamCopyMember member;
	member.accId = joinPlayer->GetAccID();
	member.playerId = joinPlayer->GetID();
	member.playerName = joinPlayer->GetName();
	member.playerOccu = joinPlayer->GetOccu();
	member.playerAwaken = joinPlayer->GetAwaken();
	member.playerLevel = joinPlayer->GetLevel();
	member.equipScore = joinPlayer->GetEquipScore();
	member.ticketIsEnough = joinPlayer->GetTicketIsEnough(GetTeamType());
	member.zoneId = joinPlayer->GetZoneID();
	member.connId = joinPlayer->GetConnId();

	for (auto& iter : m_SquadVec)
	{
		UInt32 newSeat = iter.AllocSeat();
		if (newSeat == 0)	// 小队已满
		{
			continue;
		}

		member.seatId = newSeat;
		member.post |= TEAM_COPY_POST_NORMAL;
		if (isGold != 0)
		{
			member.post |= TEAM_COPY_POST_GOLD;

			// 增加总佣金
			m_TotalCommission += m_CondCommission;
		}

		if (m_PlayerSquad.empty())
		{
			member.post |= TEAM_COPY_POST_CHIEF;
		}
		
		if (iter.teamMemberList.empty())
		{
			member.post |= TEAM_COPY_POST_CAPTAIN;
		}
		else
		{
			// 当队伍所有玩家都是金主，新进玩家不是金主，队长给新进玩家
			bool isAllGold = true;
			TeamCopyMember* captain = NULL;
			for (auto& tt : iter.teamMemberList)
			{
				if (tt.post & TEAM_COPY_POST_CAPTAIN)
					captain = &tt;

				if ( !(tt.post & TEAM_COPY_POST_GOLD))
				{
					isAllGold = false;
					break;
				}
			}

			if (isAllGold && isGold == 0 && captain != NULL)
			{
				member.post |= TEAM_COPY_POST_CAPTAIN;
				captain->post &= ~(TEAM_COPY_POST_CAPTAIN);
			}
		}

		iter.SetSquadStatus(TEAM_COPY_SQUAD_STATUS_PREPARE);
		iter.AddMember(member);

		m_PlayerSquad[member.playerId] = iter.squadId;

		_MemberNotify(member.playerName, 0);

		InfoLogStream << TEAMINFO(this) << " join team success " << PLAYERINFO(joinPlayer) 
			<< " equipScore:" << member.equipScore << " seat:" << member.seatId << " post:" << member.post << LogStream::eos;

		return ErrorCode::SUCCESS;
	}

	ErrorLogStream << TEAMINFO(this) << "join team failed! playerId:"<< member.playerId << " name:" << member.playerName << LogStream::eos;
	return ErrorCode::TEAM_JOIN_TIMEOUT;
}

bool TCSTeam::QuitTeam(UInt64 playerId)
{
	UInt32 squadId = _GetPlayerSquadId(playerId);
	TeamCopySquad* squad = _FindSquad(squadId);
	if (NULL == squad)
	{
		ErrorLogStream << TEAMINFO(this) << "not find squad id:" << squadId << " playerId:" << playerId << LogStream::eos;
		return false;
	}

	UInt32 post = TEAM_COPY_POST_NORMAL;
	std::vector<TeamCopyMember>::iterator tt = squad->teamMemberList.begin();
	for (; tt != squad->teamMemberList.end(); ++tt)
	{
		if (tt->playerId == playerId)
		{
			_MemberNotify(tt->playerName, 1);

			post = tt->post;
			squad->teamMemberList.erase(tt);
			break;
		}
	}

	// 统计开战后退队人数
	if (IsInBattle())
	{
		++m_BattleQuitTeamNum;
		if (m_BattleQuitTeamNum >= GetTeamCopyValue(TC_VALUE_FORCE_END_QUIT_NUM))
		{
			SetForceEndFlag(true);
			_NotifyForceEndFlag();
		}
	}

	// 如果是金主,并且没开战
	if ((post & TEAM_COPY_POST_GOLD) && !IsYetStartBattle())
	{
		m_TotalCommission -= m_CondCommission;
	}

	// 小队人走完了，状态改为待机中
	if (squad->teamMemberList.empty())
	{
		squad->SetSquadStatus(TEAM_COPY_SQUAD_STATUS_INIT);
		SquadMemberNull(squadId);
	}

	m_PlayerSquad.erase(playerId);
	RemoveVote(playerId);

	TCSTeamCheck* teamCheck = TCSTeamMgr::Instance()->GetTeamCheck(GetTeamId());
	if (teamCheck != NULL)
	{
		teamCheck->RemoveCheck(playerId);
	}

	InfoLogStream << TEAMINFO(this) << " quit team player id:" << playerId << LogStream::eos;

	// 退出队伍调整位置
	if (post & TEAM_COPY_POST_CHIEF)	// 团长离开	
	{
		if (GetModel() == TEAM_COPY_TEAM_MODEL_CHALLENGE)
		{
			if ((post & TEAM_COPY_POST_CAPTAIN) && (!squad->teamMemberList.empty()))	// 既是团长又是队长
			{
				squad->teamMemberList.begin()->post |= TEAM_COPY_POST_CAPTAIN;

				DebugLogStream << TEAMINFO(this) << " quit team to captain player id:" 
					<< squad->teamMemberList.begin()->playerId << LogStream::eos;
			}

			for (auto& iter : m_SquadVec)
			{
				std::vector<TeamCopyMember>::iterator it = iter.teamMemberList.begin();
				for (; it != iter.teamMemberList.end(); ++it)
				{
					it->post |= TEAM_COPY_POST_CHIEF;

					DebugLogStream << TEAMINFO(this) << " quit team to chief player id:" << it->playerId << LogStream::eos;

					SetChiefId(it->playerId);
					SetTeamName(it->playerName);
					_NotifyForceEndFlag();
					return true;
				}
			}
		}
		else if (GetModel() == TEAM_COPY_TEAM_MODEL_GOLD)
		{
			if (IsYetStartBattle())
			{
				if ((post & TEAM_COPY_POST_CAPTAIN) && (!squad->teamMemberList.empty()))	// 既是团长又是队长
				{
					bool flag = false;
					for (auto& it: squad->teamMemberList)
					{
						if (it.post & TEAM_COPY_POST_GOLD)
							continue;

						flag = true;
						it.post |= TEAM_COPY_POST_CAPTAIN;

						DebugLogStream << TEAMINFO(this) << " quit team to captain player id:" << it.playerId << LogStream::eos;
					}

					if (!flag)
					{
						squad->teamMemberList.begin()->post |= TEAM_COPY_POST_CAPTAIN;
						DebugLogStream << TEAMINFO(this) << " quit team to captain player id:" 
							<< squad->teamMemberList.begin()->playerId << LogStream::eos;
					}
				}

				bool chiefFlag = false;
				for (auto& iter : m_SquadVec)
				{
					std::vector<TeamCopyMember>::iterator it = iter.teamMemberList.begin();
					for (; it != iter.teamMemberList.end(); ++it)
					{
						if (it->post & TEAM_COPY_POST_GOLD)
							continue;

						chiefFlag = true;

						DebugLogStream << TEAMINFO(this) << " quit team to chief player id:" << it->playerId << LogStream::eos;

						it->post |= TEAM_COPY_POST_CHIEF;
						SetChiefId(it->playerId);
						SetTeamName(it->playerName);
						_NotifyForceEndFlag();
						return true;
					}
				}

				if (!chiefFlag)
				{
					for (auto& iter : m_SquadVec)
					{
						if ( !iter.teamMemberList.empty())
						{
							DebugLogStream << TEAMINFO(this) << " quit team to chief player id:"
								<< iter.teamMemberList.begin()->playerId << LogStream::eos;

							iter.teamMemberList.begin()->post |= TEAM_COPY_POST_CHIEF;
							SetChiefId(iter.teamMemberList.begin()->playerId);
							SetTeamName(iter.teamMemberList.begin()->playerName);
							_NotifyForceEndFlag();
							return true;
						}
					}
				}
			}
			else
			{
				// 解散团队
				CLProtocol::TeamCopyTeamUpdate notify;
				notify.opType = 1;
				notify.teamId = GetTeamId();
				BroadcastToMember(notify);

				for (auto& tt : m_PlayerSquad)
				{
					TCSTeamMgr::Instance()->SetPlayerTeamId(tt.first, 0);
				}

				m_PlayerSquad.clear();
				return true;
			}
		}
	}
	else if (post & TEAM_COPY_POST_CAPTAIN)	// 队长离开
	{
		if (!squad->teamMemberList.empty())
		{
			DebugLogStream << TEAMINFO(this) << " quit team to captain player id:"
				<< squad->teamMemberList.begin()->playerId << LogStream::eos;

			squad->teamMemberList.begin()->post |= TEAM_COPY_POST_CAPTAIN;
		}
	}

	return true;
}

UInt32 TCSTeam::GetTeamMemberNum()
{
	return m_PlayerSquad.size();
}

UInt32 TCSTeam::GoldPlayerNum()
{
	if (GetModel() != TEAM_COPY_TEAM_MODEL_GOLD)
	{
		return 0;
	}

	UInt32 k = 0;
	for (auto& iter : m_SquadVec)
	{
		for (auto it : iter.teamMemberList)
		{
			if (it.post & TEAM_COPY_POST_GOLD)
			{
				++k;
			}
		}
	}

	return k;
}

UInt32 TCSTeam::CheckStartBattle()
{
	if (IsYetStartBattle())
	{
		ErrorLogStream << TEAMINFO(this) << "team yet start battle !" << LogStream::eos;
		return ErrorCode::TEAM_COPY_IN_BATTLE;
	}

	if (GetModel() == TEAM_COPY_TEAM_MODEL_GOLD)
	{
		if (GoldPlayerNum() == 0)
		{
			return ErrorCode::TEAM_COPY_GOLD_NUM_NOT_ENOUGH;
		}

		if (GoldPlayerNum() > _CommonPlayerNum())
		{
			return ErrorCode::TEAM_COPY_GOLD_THAN_COMMON;
		}
	}

	if (GetVoteEndTime() > CURRENT_TIME.Sec())
	{
		return ErrorCode::TEAM_COPY_VOTE_PROCESS;
	}

	if (!TCSApplication::Instance()->GetMemberIsEnough())
	{
		for (auto& iter : m_SquadVec)
		{
			if (iter.teamMemberList.size() == 0)
			{
				return ErrorCode::TEAM_COPY_MEMBER_NOT_ENOUGH;
			}
		}
	}

	// 检查门票，团本服务器有门票是否足够的实时数据
	for (auto& it : m_PlayerSquad)
	{
		TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(it.first);
		if (NULL == player)
		{
			ErrorLogStream << TEAMINFO(this) << "not find player id:" << it.first << LogStream::eos;
			return ErrorCode::TEAM_COPY_PLAYER_LEAVE;
		}

		if (player->GetTicketIsEnough(GetTeamType()) == 0)
		{
			return ErrorCode::TEAM_COPY_TICKET_NOT_ENOUGH;
		}
	}

	return ErrorCode::SUCCESS;
}

UInt32 TCSTeam::JoinApply(UInt64 playerId, UInt32 isGold)
{
	if (m_ApplyMap.size() >= GetTeamCopyValue(TC_VALUE_APPLY_LIST_MAX))
	{
		return ErrorCode::TEAM_COPY_APPLY_LIST_FULL;
	}

	m_ApplyMap[playerId] = isGold;

	return ErrorCode::SUCCESS;
}

void TCSTeam::RemoveApply(UInt64 playerId)
{
	m_ApplyMap.erase(playerId);
}

void TCSTeam::ClearApply()
{
	m_ApplyMap.clear();
}

void TCSTeam::ApplyListRes(TCSPlayer* player)
{
	if (NULL == player) 
		return;
	
	CLProtocol::TeamCopyTeamApplyListRes res;

	for (auto& iter : m_ApplyMap)
	{
		TCSPlayer* applyPlayer = TCSPlayerMgr::Instance()->FindTCSPlayer(iter.first);
		if (NULL == applyPlayer)
			continue;

		TeamCopyApplyProperty applyPro;
		applyPro.playerId = iter.first;
		applyPro.playerName = applyPlayer->GetName();
		applyPro.playerOccu = applyPlayer->GetOccu();
		applyPro.playerLevel = applyPlayer->GetLevel();
		applyPro.equipScore = applyPlayer->GetEquipScore();
		applyPro.isGold = iter.second;
		applyPro.guildId = applyPlayer->GetGuildId();
		applyPro.zoneId = applyPlayer->GetZoneID();

		res.applyList.push_back(applyPro);
	}

	player->SendProtocol(res);
}

UInt32 TCSTeam::AgreeApply(UInt64 playerId)
{
	if (TCSTeamMgr::Instance()->GetPlayerTeamId(playerId) != 0)
	{
		return ErrorCode::TEAM_COPY_HAS_TEAM;
	}

	TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(playerId);
	if (NULL == player)
	{
		return ErrorCode::TEAM_COPY_TARGET_CUR_STATUS;
	}

	if (TCSPlayerMgr::Instance()->PlayerIsExpire(playerId))
	{
		return ErrorCode::TEAM_COPY_TARGET_CUR_STATUS;
	}

	auto iter = m_ApplyMap.find(playerId);
	if (iter == m_ApplyMap.end())
	{
		return ErrorCode::TEAM_COPY_DATA_ERROR;
	}

	UInt32 ret = JoinTeam(player, iter->second);
	if (ret == ErrorCode::SUCCESS)
	{
		TeamDataBroadcast();
	}

	return ret;
}

UInt32 TCSTeam::ApplyNum()
{
	return (UInt32)m_ApplyMap.size();
}

void TCSTeam::SetSquadStatus(UInt32 squadId, UInt32 status)
{
	TeamCopySquad* squad = _FindSquad(squadId);
	if (NULL == squad)
	{
		return;
	}

	squad->SetSquadStatus(status);
}

void TCSTeam::TeamDataBroadcast()
{
	CLProtocol::TeamCopyTeamDataRes res;
	res.teamId = GetTeamId();
	res.equipScore = GetEquipScore();
	res.status = GetStatus();
	res.teamName = GetTeamName();
	res.totalCommission = GetTotalCommission();
	res.bonusCommission = CommissionBonusNum();
	res.autoAgreeGold = GetAutoAgreeGold();
	res.teamType = GetTeamType();
	res.teamModel = GetModel();
	res.teamGrade = GetTeamGrade();
	res.voiceRoomId = GetVoiceRoomId();
	res.squadList = m_SquadVec;

	BroadcastToMember(res);
}

void TCSTeam::TeamDataRes(TCSPlayer* player)
{
	if (NULL == player) return;
	
	CLProtocol::TeamCopyTeamDataRes res;
	res.teamId = GetTeamId();
	res.equipScore = GetEquipScore();
	res.status = GetStatus();
	res.teamName = GetTeamName();
	res.totalCommission = GetTotalCommission();
	res.bonusCommission = CommissionBonusNum();
	res.autoAgreeGold = GetAutoAgreeGold();
	res.teamType = GetTeamType();
	res.teamModel = GetModel();
	res.teamGrade = GetTeamGrade();
	res.voiceRoomId = GetVoiceRoomId();
	res.squadList = m_SquadVec;

	player->SendProtocol(res);
}

UInt32 TCSTeam::StartRace(UInt32 squadId, UInt32 fieldId)
{
	if (IsGameOver())
	{
		return ErrorCode::TEAM_COPY_GAME_OVER;
	}

	TeamCopyField* field = FindField(fieldId);
	if (NULL == field)
	{
		return ErrorCode::TEAM_COPY_FIELD_UNLOCK;
	}

	if ( !field->isCanChallenge())
	{
		return ErrorCode::TEAM_COPY_FIELD_UNLOCK;
	}

	if (field->oddNum == 0 && field->rebornTime != 0)
	{
		return ErrorCode::TEAM_COPY_FIELD_REBORN;
	}

	if (field->oddNum == 0)
	{
		return ErrorCode::TEAM_COPY_FIELD_DEFEAT;
	}

	// 如果是能量恢复据点
	if (field->fieldId == GetTeamCopyValue(TC_VALUE_DIFF_ENERGY_REVIVE_FIELD) && 
		((UInt32)CURRENT_TIME.Sec() - (field->energyReviveTime)) < GetTeamCopyValue(TC_VALUE_DIFF_ENERGY_REVIVE_ENTER_RATE))
	{
		return ErrorCode::TEAM_COPY_ENERGY_REVIVE;
	}

	// 如果是boss据点，只能一个队伍进入
	if (field->config->presentedType == TEAM_COPY_APPEAR_BOSS && !field->attackSquadList.empty())
	{
		return ErrorCode::TEAM_COPY_BOSS_FIELD;
	}

	TeamCopySquad* squad = _FindSquad(squadId);
	if (NULL == squad)
	{
		ErrorLogStream << TEAMINFO(this) << "not find squad id:" << squadId << LogStream::eos;
		return ErrorCode::TEAM_COPY_SQUAD_INVALID;
	}

	if (squad->GetSquadStatus() == TEAM_COPY_SQUAD_STATUS_BATTLE)
	{
		return ErrorCode::TEAM_COPY_SQUAD_IN_BATTLE;
	}

	std::set<UInt64> playerIds;
	for (auto& it : squad->teamMemberList)
	{
		TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(it.playerId);
		if (NULL == player)
		{
			ErrorLogStream << TEAMINFO(this) << " not find player id:" << it.playerId << LogStream::eos;
			return ErrorCode::TEAM_COPY_PLAYER_LEAVE;
		}

		if (TCSPlayerMgr::Instance()->PlayerIsExpire(it.playerId))
		{
			DebugLogStream << TEAMINFO(this) << "expire playerId:" << it.playerId << LogStream::eos;
			continue;
		}

		if (player->GetIsLoading())
		{
			return ErrorCode::TEAM_COPY_TEAM_MATE_LOADING;
		}

		playerIds.insert(it.playerId);
	}

	// 所有人都掉线了，或者在loading，或者不在团本场景
	if (playerIds.empty())
	{
		return ErrorCode::TEAM_COPY_PLAYER_LEAVE;
	}

	UInt32 teamSquadId = GET_TEAM_SQUAD_ID(GetTeamId(), squad->squadId);

	// 开始查询玩家信息, 去scene查数据
	CLProtocol::TeamCopyQueryPlayerInfoReq req;
	req.teamSquadId = teamSquadId;
	req.dungeonId = field->config->dungeonId;

	for (auto& iter : playerIds)
	{
		req.roleId = iter;
		
		TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(iter);
		if (NULL == player)
		{
			ErrorLogStream << TEAMINFO(this) << "not find player id:" << iter << LogStream::eos;
			return ErrorCode::ITEM_DATA_INVALID;
		}

		player->SendToScene(req);
	}

	TeamCopyField* tailField = FindField(GetTailFieldId(GetTeamGrade()));
	if (NULL == tailField || tailField->state != TEAM_COPY_FIELD_STATUS_DEFEAT)
	{
		TCSTeamMgr::Instance()->CreateTeamPlayerInfo(teamSquadId, field->config->dungeonId, fieldId, 0, playerIds);
	}
	else
	{
		TCSTeamMgr::Instance()->CreateTeamPlayerInfo(teamSquadId, field->config->dungeonId, fieldId, tailField->config->dungeonId, playerIds);
	}

	// 更新小队状态
	squad->SetSquadStatus(TEAM_COPY_SQUAD_STATUS_BATTLE);

	InfoLogStream << TEAMINFO(this) << "start challenge teamSquadId:" << teamSquadId
		<< " fieldId:" << fieldId << " dungeonId:" << field->config->dungeonId << LogStream::eos;

	return ErrorCode::SUCCESS;
}

bool TCSTeam::StartChallengePower(UInt64 playerId)
{
	TeamCopyMember* member = _FindMember(playerId);
	if (NULL == member)
	{
		return false;
	}

	if (member->post & TEAM_COPY_POST_CHIEF || member->post & TEAM_COPY_POST_CAPTAIN)
	{
		return true;
	}

	return false;
}

UInt32 TCSTeam::GetPlayerSquadId(UInt64 playerId)
{
	return _GetPlayerSquadId(playerId);
}

void TCSTeam::NotifyPlayerVote()
{
	UInt32 voteTime = GetTeamCopyValue(TC_VALUE_VOTE_TIME);
	UInt32 endTime = UInt32(CURRENT_TIME.Sec()) + voteTime;

	// 通知所有队员开战，切入攻坚场景,并且开始投票
	CLProtocol::TeamCopyStartBattleNotify startNotify;
	startNotify.voteDurationTime = voteTime;
	startNotify.voteEndTime = endTime;
	BroadcastToMember(startNotify);

	// 通知已经投过票的人员
	for (auto& iter : m_VoteSet)
	{
		_VoteNotify(iter, 1);
	}

	// 有人退出，再次发起投票直接完成，此时也无须检查
	if (m_VoteSet.size() == GetTeamMemberNum())
	{
		_VoteFinish();
		return;
	}

	// 设置投票结束时间
	SetVoteEndTime(endTime);

	// 队长自动同意
	auto it = m_VoteSet.find(GetChiefId());
	if (it == m_VoteSet.end())
	{
		HandlerVote(GetChiefId(), 1);
	}
}

void TCSTeam::HandlerVote(UInt64 playerId, UInt32 vote)
{
	auto iter = m_VoteSet.find(playerId);
	if (iter != m_VoteSet.end()) // 已经投过票了
	{
		return;
	}

	// 投票时间结束
	if (GetVoteEndTime() == 0)
	{
		return;
	}

	// 通知玩家投票
	_VoteNotify(playerId, vote);

	if (vote == 0)
	{
		SetVoteEndTime(0);
		SetStatus(TEAM_COPY_TEAM_STATUS_PARPARE);
	}
	else
	{
		m_VoteSet.insert(playerId);

		// 全部同意
		if (m_VoteSet.size() == GetTeamMemberNum())
		{
			_VoteFinish();
		}
	}
}

void TCSTeam::RemoveVote(UInt64 playerId)
{
	m_VoteSet.erase(playerId);
}

void TCSTeam::FieldAddAttackSquad(UInt32 squadId, UInt32 fieldId, UInt64 raceId)
{
	TeamCopyField* field = FindField(fieldId);
	if (NULL == field)
	{
		return;
	}

	field->AddAttackSquad(squadId);
	field->AddSquadRace(raceId);

	FieldNotify(fieldId);
}

void TCSTeam::FieldDelAttackSquad(UInt32 squadId, UInt32 fieldId, UInt64 raceId)
{
	TeamCopyField* field = FindField(fieldId);
	if (NULL == field)
	{
		return;
	}

	field->DelAttackSquad(squadId);
	field->DelSquadRace(raceId);

	FieldNotify(fieldId);
}

void TCSTeam::SquadNotify(UInt32 squadId, UInt32 squadStatus)
{
	CLProtocol::TeamCopySquadStatusNotify notify;
	notify.squadId = squadId;
	notify.squadStatus = squadStatus;
	BroadcastToMember(notify);
}

void TCSTeam::AddCommissionOrder(UInt64 playerId, UInt32 connid, UInt64 commissionOrder)
{
	TeamCopyCommission commiss;
	commiss.connId = connid;
	commiss.frozenOrder = commissionOrder;

	m_CommissiomMap[playerId] = commiss;
}

UInt32 TCSTeam::ChangeSeat(UInt32 srcSeat, UInt32 destSeat)
{
	TeamCopyMember* srcMember = _FindMember(srcSeat);
	TeamCopyMember* destMember = _FindMember(destSeat);

	if (NULL == srcMember)
	{
		return ErrorCode::TEAM_COPY_MEMBER_LEAVE;
	}

	TeamCopySquad* srcSquad = _FindSquad(GetSquadId(srcSeat));
	TeamCopySquad* destSquad = _FindSquad(GetSquadId(destSeat));
	if (NULL == srcSquad || srcSquad->GetSquadStatus() == TEAM_COPY_SQUAD_STATUS_BATTLE ||
		NULL == destSquad || destSquad->GetSquadStatus() == TEAM_COPY_SQUAD_STATUS_BATTLE)
	{
		return ErrorCode::TEAM_COPY_SQUAD_IN_BATTLE;
	}

	if (srcSquad->squadId != destSquad->squadId && IsYetStartBattle())
	{
		UInt32 ret = IsAllowChangeSeat(srcSquad->squadId, destSquad->squadId);
		if (ret != 0)
		{
			return ret;
		}

		srcMember->changeSquadCnt++;
		if (destMember != NULL)
			destMember->changeSquadCnt++;
	}

	UInt32 srcMemberNum = _SquadMemberNum(srcSquad->squadId);
	UInt32 destMemberNum = _SquadMemberNum(destSquad->squadId);

	DebugLogStream << TEAMINFO(this) << " change seat srcSeat:" << srcSeat << " destSeat:" << destSeat << LogStream::eos;

	if (srcMember->post & TEAM_COPY_POST_CAPTAIN)
	{
		if (NULL == destMember)
		{
			if (srcSquad->squadId == destSquad->squadId)
			{
				srcMember->seatId = destSeat;
			}
			else
			{
				TeamCopyMember tempMember = *srcMember;
				tempMember.seatId = destSeat;
				if (!destSquad->teamMemberList.empty())	// 另一队不空有队长，卸掉自己的队长
				{
					tempMember.post &= ~(TEAM_COPY_POST_CAPTAIN);

					DebugLogStream << TEAMINFO(this) << "cancel captain:" << tempMember.playerId << LogStream::eos;
				}

				destSquad->AddMember(tempMember);

				m_PlayerSquad[tempMember.playerId] = destSquad->squadId;

				std::vector<TeamCopyMember>::iterator iter = srcSquad->teamMemberList.begin();
				for (; iter != srcSquad->teamMemberList.end(); ++iter)
				{
					if (iter->seatId == srcSeat)
					{
						srcSquad->teamMemberList.erase(iter);
						break;
					}
				}

				// 把队长给第一个人
				if (!srcSquad->teamMemberList.empty())
				{
					srcSquad->teamMemberList.begin()->post |= TEAM_COPY_POST_CAPTAIN;
					DebugLogStream << TEAMINFO(this) << "to captain:" << srcSquad->teamMemberList.begin()->playerId << LogStream::eos;
				}
				else
					srcSquad->SetSquadStatus(TEAM_COPY_SQUAD_STATUS_INIT);

				destSquad->SetSquadStatus(TEAM_COPY_SQUAD_STATUS_PREPARE);
			}
		}
		else
		{
			if (srcSquad->squadId == destSquad->squadId)
			{
				srcMember->seatId = destSeat;
				destMember->seatId = srcSeat;
			}
			else
			{
				srcMember->post &= ~(TEAM_COPY_POST_CAPTAIN);
				DebugLogStream << TEAMINFO(this) << "cancel captain:" << srcMember->playerId << LogStream::eos;

				bool flag = false;
				if (destMember->post & TEAM_COPY_POST_CAPTAIN)
				{
					destMember->post &= ~(TEAM_COPY_POST_CAPTAIN);
					DebugLogStream << TEAMINFO(this) << "cancel captain:" << destMember->playerId << LogStream::eos;

					flag = true;
				}

				TeamCopyMember tempMember = *srcMember;
				*srcMember = *destMember;
				*destMember = tempMember;
				srcMember->seatId = srcSeat;
				destMember->seatId = destSeat;

				m_PlayerSquad[srcMember->playerId] = srcSquad->squadId;
				m_PlayerSquad[destMember->playerId] = destSquad->squadId;

				srcSquad->teamMemberList.begin()->post |= TEAM_COPY_POST_CAPTAIN;
				DebugLogStream << TEAMINFO(this) << "to captain:" << srcSquad->teamMemberList.begin()->playerId << LogStream::eos;

				if (flag)
				{
					destSquad->teamMemberList.begin()->post |= TEAM_COPY_POST_CAPTAIN;
					DebugLogStream << TEAMINFO(this) << "to captain:" << destSquad->teamMemberList.begin()->playerId << LogStream::eos;
				}
			}
		}
	}
	else
	{
		if (NULL == destMember)
		{
			if (srcSquad->squadId == destSquad->squadId)
			{
				srcMember->seatId = destSeat;
			}
			else
			{
				TeamCopyMember tempMember = *srcMember;
				tempMember.seatId = destSeat;
				if (destSquad->teamMemberList.empty())	// 另一对队空，自己过去当队长
				{
					tempMember.post |= TEAM_COPY_POST_CAPTAIN;
					destSquad->SetSquadStatus(TEAM_COPY_SQUAD_STATUS_PREPARE);
					DebugLogStream << TEAMINFO(this) << "to captain:" << tempMember.playerId << LogStream::eos;
				}

				destSquad->AddMember(tempMember);
				m_PlayerSquad[tempMember.playerId] = destSquad->squadId;

				std::vector<TeamCopyMember>::iterator iter = srcSquad->teamMemberList.begin();
				for (; iter != srcSquad->teamMemberList.end(); ++iter)
				{
					if (iter->seatId == srcSeat)
					{
						srcSquad->teamMemberList.erase(iter);
						break;
					}
				}

				if (srcSquad->teamMemberList.empty())
				{
					srcSquad->SetSquadStatus(TEAM_COPY_SQUAD_STATUS_INIT);
				}
			}
		}
		else
		{
			if (srcSquad->squadId == destSquad->squadId)
			{
				srcMember->seatId = destSeat;
				destMember->seatId = srcSeat;
			}
			else
			{
				bool flag = false;
				if (destMember->post & TEAM_COPY_POST_CAPTAIN)
				{
					destMember->post &= ~(TEAM_COPY_POST_CAPTAIN);
					DebugLogStream << TEAMINFO(this) << "cancel captain:" << destMember->playerId << LogStream::eos;
					flag = true;
				}

				TeamCopyMember tempMember = *srcMember;
				*srcMember = *destMember;
				*destMember = tempMember;

				srcMember->seatId = srcSeat;
				destMember->seatId = destSeat;

				m_PlayerSquad[srcMember->playerId] = srcSquad->squadId;
				m_PlayerSquad[destMember->playerId] = destSquad->squadId;

				// 职位处理
				if (flag)
				{
					destSquad->teamMemberList.begin()->post |= TEAM_COPY_POST_CAPTAIN;
					DebugLogStream << TEAMINFO(this) << "to captain:" << destSquad->teamMemberList.begin()->playerId << LogStream::eos;
				}
			}
		}
	}

	ChangeSeatFinish(srcSquad->squadId, _SquadMemberNum(srcSquad->squadId) < srcMemberNum,
		destSquad->squadId, _SquadMemberNum(destSquad->squadId) > destMemberNum);
	TeamDataBroadcast();
	SquadAttackDataNotify();

	return ErrorCode::SUCCESS;
}

UInt32 TCSTeam::AppointmentPost(UInt64 handlerPlayer, UInt64 playerId, UInt32 post)
{
	if (TCSPlayerMgr::Instance()->PlayerIsExpire(playerId))
	{
		return ErrorCode::TEAM_COPY_PLAYER_LEAVE;
	}

	if (post & TEAM_COPY_POST_CHIEF)
	{
		if (handlerPlayer != GetChiefId())
		{
			return ErrorCode::TEAM_COPY_NO_POWER;
		}

		// 去掉原来的团长
		TeamCopyMember* chief = _FindChief();
		if (NULL == chief)
		{
			return ErrorCode::TEAM_COPY_NOT_FIND_TARGET;
		}

		// 新团长
		TeamCopyMember* member = _FindMember(playerId);
		if (NULL == member)
		{
			return ErrorCode::TEAM_COPY_NOT_FIND_TARGET;
		}

		chief->post &= ~(TEAM_COPY_POST_CHIEF);
		member->post |= TEAM_COPY_POST_CHIEF;

		DebugLogStream << TEAMINFO(this) << "cancel chief:" << chief->playerId << " appointment chief:" << member->playerId << LogStream::eos;

		SetChiefId(member->playerId);
		SetTeamName(member->playerName);
		_NotifyForceEndFlag();

		CLProtocol::TeamCopyAppointmentNotify notify;
		notify.post = post;
		notify.name = member->playerName;
		BroadcastToMember(notify);
	}
	else if (post & TEAM_COPY_POST_CAPTAIN)
	{
		TeamCopyMember* captain = _FindCaptain(handlerPlayer);
		if (NULL == captain)
		{
			return ErrorCode::TEAM_COPY_NOT_FIND_TARGET;
		}
		
		// 去掉原来的队长
		TeamCopyMember* captain_1 = _FindCaptain(playerId);
		if (NULL == captain_1)
		{
			return ErrorCode::TEAM_COPY_NOT_FIND_TARGET;
		}

		if (captain_1->playerId == playerId)
		{
			return ErrorCode::SUCCESS;
		}

		captain_1->post &= ~(TEAM_COPY_POST_CAPTAIN);

		// 新队长
		TeamCopyMember* member = _FindMember(playerId);
		if (NULL == member)
		{
			return ErrorCode::TEAM_COPY_NOT_FIND_TARGET;
		}

		member->post |= TEAM_COPY_POST_CAPTAIN;

		DebugLogStream << TEAMINFO(this) << "cancel captain:" << captain_1->playerId << " appointment captain:" << member->playerId << LogStream::eos;

		CLProtocol::TeamCopyAppointmentNotify notify;
		notify.post = post;
		notify.name = member->playerName;
		BroadcastToMember(notify, _GetPlayerSquadId(playerId));
	}

	TeamDataBroadcast();

	return ErrorCode::SUCCESS;
}

void TCSTeam::TicketUpdate(UInt64 playerId, UInt32 isEnough)
{
	TeamCopyMember* member = _FindMember(playerId);
	if (NULL == member || member->ticketIsEnough == isEnough)
	{
		return;
	}

	member->ticketIsEnough = isEnough;
	TeamDataBroadcast();
}

UInt32 TCSTeam::KickMember(UInt64 playerId)
{
	if (GetStatus() >= TEAM_COPY_TEAM_STATUS_BATTLE)
	{
		return ErrorCode::TEAM_COPY_IN_BATTLE;
	}

	TeamCopyMember* member = _FindMember(playerId);
	if (NULL == member)
	{
		ErrorLogStream << TEAMINFO(this) << "not find member id:" << playerId << LogStream::eos;
		return ErrorCode::TEAM_COPY_PLAYER_LEAVE;
	}

	// 是否金主
	bool isGold = member->post & TEAM_COPY_POST_GOLD;

	if ( !QuitTeam(playerId))
	{
		return ErrorCode::TEAM_COPY_PLAYER_LEAVE;
	}

	// 如果是金主并且已经开战，被踢，需要返还佣金
	if (isGold && IsInBattle())
	{
		auto iter = m_CommissiomMap.find(playerId);
		if (iter == m_CommissiomMap.end())
		{
			ErrorLogStream << TEAMINFO(this) << "not find commission order player id:" << playerId << LogStream::eos;
			return ErrorCode::SUCCESS;
		}

		// 删除佣金记录
		UInt64 orderId = iter->second.frozenOrder;
		UInt32 connId = iter->second.connId;

		m_CommissiomMap.erase(iter);

		CLProtocol::SceneCurrencyFrozenOpReq req;
		req.opType = 0;
		req.guid = orderId;

		GameZone* zone = GameZoneMgr::Instance()->FindGameZone(connId);
		if (NULL == zone)
		{
			ErrorLogStream << TEAMINFO(this) << "op commission failed !!! playerId:" << playerId << " orderId:" << orderId << LogStream::eos;
		}
		else
		{
			InfoLogStream << TEAMINFO(this) << "op commission success playerId:" << playerId << " orderId:" << orderId << LogStream::eos;
			zone->TransmitToScene(req);
		}
	}

	return ErrorCode::SUCCESS;
}

void TCSTeam::QueryRacePlayerInfoTimeOut(UInt32 squadId)
{
	TeamCopySquad* squad = _FindSquad(squadId);
	if (NULL == squad)
	{
		return;
	}

	squad->SetSquadStatus(TEAM_COPY_SQUAD_STATUS_PREPARE);
	SquadNotify(squadId, TEAM_COPY_SQUAD_STATUS_PREPARE);
}

void TCSTeam::JoinTimeLimit(UInt64 playerId, UInt64 expireTime)
{
	m_PlayerJoinTimeLimitMap[playerId] = expireTime;
}

bool TCSTeam::IsJoinTimeLimit(UInt64 playerId)
{
	auto iter = m_PlayerJoinTimeLimitMap.find(playerId);
	if (iter == m_PlayerJoinTimeLimitMap.end())
	{
		return false;
	}

	if (iter->second < CURRENT_TIME.Sec())
	{
		m_PlayerJoinTimeLimitMap.erase(iter);
		return false;
	}

	return true;
}

UInt64 TCSTeam::GetJoinTimeLimit(UInt64 playerId)
{
	auto iter = m_PlayerJoinTimeLimitMap.find(playerId);
	return iter != m_PlayerJoinTimeLimitMap.end() ? iter->second : 0;
}

void TCSTeam::SaveConsume(UInt64 playerId, UInt32 itemId, UInt32 itemNum, UInt32 consumeType)
{
	TeamCopyMember* member = _FindMember(playerId);
	if (NULL == member)
	{
		ErrorLogStream << TEAMINFO(this) << "not find member playerId:" << playerId << LogStream::eos;
		return;
	}

	// 记录玩家消耗
	UInt64 guid = CLRecordClient::Instance()->GenGuid();
	member->consumeMap[guid] = TCConsumeItem(itemId, itemNum, consumeType);

	InfoLogStream << TEAMINFO(this) << "insert playerId:"<< playerId << " playerName:" << member->playerName << " consume guid:"
		<< guid << " itemId:" << itemId << " itemNum:" << itemNum << " consumeType:" << consumeType << LogStream::eos;

	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_team_copy_consume", guid);
	cmd->PutData("owner_id", playerId);
	cmd->PutData("name", member->playerName);
	cmd->PutData("zone_id", member->zoneId);
	cmd->PutData("team_id", GetTeamId());
	cmd->PutData("item_id", itemId);
	cmd->PutData("item_num", itemNum);
	cmd->PutData("consume_type", consumeType);
	cmd->PutData("create_time", (UInt32)CURRENT_TIME.Sec());
	CLRecordClient::Instance()->SendCommand(cmd);
}

void TCSTeam::RemoveAllMemberConsume()
{
	for (auto& iter : m_SquadVec)
	{
		for (auto& it : iter.teamMemberList)
		{
			for (auto con : it.consumeMap)
			{
				InfoLogStream << TEAMINFO(this) << "remove playerId:" << it.playerId << " playerName:" 
					<< it.playerName << " consume guid:" << con.first << LogStream::eos;
				RemoveDBConsume(con.first);
			}

			it.consumeMap.clear();
		}
	}
}

void TCSTeam::RemoveDBConsume(UInt64 guid)
{
	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_team_copy_consume", guid);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void TCSTeam::GameOver(UInt32 overReason)
{
	if (overReason != TCGameOverReason::TC_GAME_OVER_REASON_VICTORY)return;
	CLProtocol::TeamCopyTeamPlayerInfo info;
	std::set<UInt32> zoneIds;
	for (auto iter : m_PlayerSquad)
	{
		info.playerIdVec.push_back(iter.first);
		TCSPlayer* tsPlayer = TCSPlayerMgr::Instance()->FindTCSPlayer(iter.first);
		if (tsPlayer == NULL)
		{
			ErrorLogStream << "player id(" << iter.first << ") not find" << LogStream::eos;
		}
		else
		{
			zoneIds.insert(tsPlayer->GetZoneID());
		}
	}
	info.teamType = GetTeamType();
	info.teamGrade = GetTeamGrade();
	info.costTime = (UInt32)CURRENT_TIME.Sec() - _GetGameStartTime();
	GameZoneMgr::Instance()->BroadcastToGameZoneWorldByZoneId(info,zoneIds);

}

void TCSTeam::MemberExpire(UInt64 playerId, UInt64 expireTime)
{
	TeamCopyMember* member = _FindMember(playerId);
	if (NULL == member || member->expireTime == expireTime)
	{
		return;
	}

	member->expireTime = expireTime;

	CLProtocol::TeamCopyPlayerExpireNotify notify;
	notify.playerId = playerId;
	notify.expireTime = expireTime;
	BroadcastToMember(notify);
}

void TCSTeam::MemberReturnConsume(UInt64 playerId)
{
	for (auto& iter : m_SquadVec)
	{
		for (auto& member : iter.teamMemberList)
		{
			if (playerId != 0 && playerId != member.playerId)
			{
				continue;
			}

			ItemRewardVec itemVec;
			for (auto& con : member.consumeMap)
			{
				RemoveDBConsume(con.first);

				ItemReward reward;
				reward.id = con.second.id;
				reward.num = con.second.num;
				itemVec.push_back(reward);
			}

			member.consumeMap.clear();

			// 如果没开战，玩家有消耗，需要返还
			TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(member.playerId);
			if (player != NULL && !itemVec.empty() && !IsYetStartBattle())
			{
				InfoLogStream << TEAMINFO(this) << "remove consume player " << PLAYERINFO(player) << LogStream::eos;
				
				std::string reason = GetReason(SOURCE_TYPE_TEAM_COPY_CONSUME_RETURN);
				player->AddRewards(reason, itemVec, false);

				// 挑战次数
				player->ReturnChallengeNum(GetTeamType(), GetTeamGrade());
			}
		}
	}
}

void TCSTeam::ForceEndVote(TCSPlayer* player, UInt32 vote)
{
	if (NULL == player || !GetForceEndFlag())
	{
		return;
	}

	if (m_ForceEndVoteMap.find(player->GetID()) != m_ForceEndVoteMap.end())
	{
		return;
	}

	InfoLogStream << TEAMINFO(this) << "force end " << PLAYERINFO(player) << " vote:" << vote << LogStream::eos;

	bool isAgree = vote ? true : false;
	m_ForceEndVoteMap[player->GetID()] = isAgree;

	CLProtocol::TeamCopyForceEndMemberVote memberVote;
	memberVote.roleId = player->GetID();
	memberVote.vote = vote;
	BroadcastToMember(memberVote);

	UInt32 voteNum = 0;
	for (auto& it : m_ForceEndVoteMap)
	{
		if (it.second == isAgree)
			++voteNum;
	}

	UInt32 isSuccess = 0;
	CLProtocol::TeamCopyForceEndVoteResult voteResult;
	if (isAgree)
	{
		UInt32 halfPlayerNum = GetTeamMemberNum() / 2 + 1;
		if (voteNum >= halfPlayerNum)
		{
			isSuccess = 1;
			voteResult.result = 0;
			BroadcastToMember(voteResult);

			SetForceEndVoteTime(0);
			m_ForceEndVoteMap.clear();
			SetGameOverTime((UInt32)CURRENT_TIME.Sec());

			InfoLogStream << TEAMINFO(this) << "force end success! " << LogStream::eos;
		}
	}
	else
	{
		UInt32 halfPlayerNum = 0;
		if (GetTeamMemberNum() % 2 == 0)
			halfPlayerNum = GetTeamMemberNum() / 2;
		else
			halfPlayerNum = GetTeamMemberNum() / 2 + 1;

		if (voteNum >= halfPlayerNum)
		{
			isSuccess = 0;
			voteResult.result = 1;
			BroadcastToMember(voteResult);

			SetForceEndVoteTime(0);
			m_ForceEndVoteMap.clear();

			InfoLogStream << TEAMINFO(this) << "force end failed! " << LogStream::eos;
		}
	}

	// 强制结束udp日志，1失败
	UInt32 waiverNum = GetTeamMemberNum() - m_ForceEndVoteMap.size();
	if (isAgree)
	{
		player->SendUdpLog("team_copy_force_end", LOG_TEAM_FORCE_END, vote, GetTeamId(), m_BattleQuitTeamNum, voteNum,
			m_ForceEndVoteMap.size() - voteNum, waiverNum, GetTeamMemberNum(), isSuccess,
			GetGameOverTime() - (UInt32)CURRENT_TIME.Sec(), GetTeamGrade(), GetTeamType());
	}
	else
	{
		player->SendUdpLog("team_copy_force_end", LOG_TEAM_FORCE_END, vote, GetTeamId(), m_BattleQuitTeamNum,
			m_ForceEndVoteMap.size() - voteNum, voteNum, waiverNum, GetTeamMemberNum(), isSuccess,
			isSuccess ? 0:GetGameOverTime() - (UInt32)CURRENT_TIME.Sec(), GetTeamGrade(), GetTeamType());
	}
}

void TCSTeam::ForceEndVoteTimeOut()
{
	// 超时失败
	CLProtocol::TeamCopyForceEndVoteResult voteResult;
	voteResult.result = 1;
	BroadcastToMember(voteResult);

	InfoLogStream << TEAMINFO(this) << "force end time out!" << LogStream::eos;

	SetForceEndVoteTime(0);
	m_ForceEndVoteMap.clear();
}

void TCSTeam::ReturnConsumeByType(UInt32 consumeType)
{
	for (auto& iter : m_SquadVec)
	{
		for (auto& member : iter.teamMemberList)
		{
			ItemRewardVec itemVec;

			std::map<UInt64, TCConsumeItem>::iterator con = member.consumeMap.begin();
			for(; con != member.consumeMap.end();)
			{
				if (con->second.consumeType != consumeType)
				{
					++con;
					continue;
				}

				RemoveDBConsume(con->first);

				ItemReward reward;
				reward.id = con->second.id;
				reward.num = con->second.num;
				itemVec.push_back(reward);

				con = member.consumeMap.erase(con);
			}

			TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(member.playerId);
			if (player != NULL && !itemVec.empty())
			{
				InfoLogStream << TEAMINFO(this) << "remove consume player" << PLAYERINFO(player) << LogStream::eos;

				std::string reason = GetReason(SOURCE_TYPE_TEAM_COPY_FORCE_RETURN);
				player->AddRewards(reason, itemVec, false);
			}
		}
	}
}

void TCSTeam::FailedReturnChallengeNum()
{
	for (auto& iter : m_PlayerSquad)
	{
		TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(iter.first);
		if (NULL == player)
		{
			ErrorLogStream << TEAMINFO(this) << "not find player id:" << iter.first << LogStream::eos;
			continue;
		}

		InfoLogStream << TEAMINFO(this) << "return player challenge num " << PLAYERINFO(player) << LogStream::eos;

		player->ReturnChallengeNum(GetTeamType(), GetTeamGrade());
	}
}

void TCSTeam::FlopDataRes(TCSPlayer* player, UInt32 stage)
{
	if (NULL == player) return;

	UInt32 squadId = GetPlayerSquadId(player->GetID());

	CLProtocol::TeamCopyStageFlopRes res;
	res.stageId = stage;

	auto iter = m_FlopMap.find(stage);
	if (iter != m_FlopMap.end())
	{
		for (auto& it : iter->second)
		{
			if (stage != TEAM_COPY_STAGE_FINAL && GetPlayerSquadId(it.playerId) != squadId)
				continue;

			res.flopList.push_back(it);
		}
	}

	player->SendProtocol(res);
}

void TCSTeam::UdpFieldSettle(UInt32 squadId, UInt32 fieldId, UInt32 dungeonId, UInt64 createTime, UInt32 result)
{
	TeamCopySquad* squad = _FindSquad(squadId);
	if (NULL == squad)
	{
		return;
	}

	for (auto& iter : squad->teamMemberList)
	{
		TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(iter.playerId);
		if (NULL == player)
			continue;

		player->SendUdpLog("team_copy_field_settle", LOG_TEAM_COPY_FIELD_SETTLE, GetTeamId(), squadId, GetCurStage(), fieldId,
			dungeonId, CURRENT_TIME.Sec() - _GetGameStartTime(), result, GetTeamGrade(), GetTeamType());
	}
}

void TCSTeam::ClearEquipScoreLowPlayer()
{
	std::vector<UInt64> tempMap;
	for (auto& iter : m_ApplyMap)
	{
		// 金主不清除
		if (iter.second != 0)
			continue;

		TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(iter.first);
		if (NULL == player)
			continue;

		if (GetEquipScore() > player->GetEquipScore())
		{
			tempMap.push_back(iter.first);
		}
	}

	for (auto& it : tempMap)
	{
		m_ApplyMap.erase(it);
	}
}

void TCSTeam::GMUpdateEnergyField(UInt32 rate)
{
	if (!IsInBattle() || GetTeamGrade() != TEAM_COPY_TEAM_GRADE_DIFF)
	{
		return;
	}

	TeamCopyField* energyField = FindField(GetTeamCopyValue(TC_VALUE_DIFF_ENERGY_REVIVE_FIELD));
	if (NULL == energyField)
	{
		return;
	}

	// 进度为千分比，最大1000
	if (rate > 1000) rate = 1000;

	energyField->energyReviveTime = (UInt32)(CURRENT_TIME.Sec()) - rate;
	energyField->state = TEAM_COPY_FIELD_STATUS_ENERGY_REVIVE;
	FieldNotify(energyField->fieldId);

	_HandleEnergyReviveField(rate, energyField->config->dungeonId);
}

void TCSTeam::RollRewardPoint(TCSPlayer* player, UInt32 isNeed)
{
	if (NULL == player) return;

	if (m_RollEndStamp == 0)
	{
		ErrorLogStream << TEAMINFO(this) << " roll end playerId:" << player->GetID() << LogStream::eos;
		return;
	}
	
	CLProtocol::TeamCopyRollChoiceRes res;
	res.retCode = ErrorCode::SUCCESS;
	res.isNeed = isNeed;

	do 
	{
		auto iter = m_RollMap.find(player->GetID());
		if (iter == m_RollMap.end())
		{
			res.retCode = ErrorCode::ITEM_DATA_INVALID;
			break;
		}

		// 检查资格
		if (iter->second.rollOption != TC_ROLL_NORMAL)
		{
			break;
		}

		UInt32 point = _GetRollPoint();
		iter->second.rollPoint = point;
		res.rollPoint = point;

		InfoLogStream << TEAMINFO(this) << " playerId:" << player->GetID() << " roll point:" << point << LogStream::eos;

	} while (0);

	player->SendProtocol(res);
}

void TCSTeam::BroadcastToMember(Avalon::Protocol& protocol, UInt32 squadId)
{
	for (auto& iter : m_PlayerSquad)
	{
		if (squadId != 0 && iter.second != squadId)
		{
			continue;
		}

		TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(iter.first);
		if (NULL == player)
		{
			continue;
		}

		player->SendProtocol(protocol);
	}
}

void TCSTeam::BroadcastToMember(Avalon::Packet& packet, UInt32 squadId)
{
	for (auto& iter : m_PlayerSquad)
	{
		if (squadId != 0 && iter.second != squadId)
		{
			continue;
		}

		TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(iter.first);
		if (NULL == player)
		{
			continue;
		}

		player->SendPacket(&packet);
	}
}

void TCSTeam::BroadcastToExceptMember(Avalon::Protocol& protocol, UInt64 exceptPlayer)
{
	for (auto& iter : m_PlayerSquad)
	{
		if (exceptPlayer == iter.first)
			continue;
		
		TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(iter.first);
		if (NULL == player)
		{
			continue;
		}

		player->SendProtocol(protocol);
	}
}

void TCSTeam::BroadcastToExceptMember(Avalon::Packet& packet, UInt64 exceptPlayer)
{
	for (auto& iter : m_PlayerSquad)
	{
		if (exceptPlayer == iter.first)
			continue;

		TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(iter.first);
		if (NULL == player)
		{
			continue;
		}

		player->SendPacket(&packet);
	}
}

TeamCopyMember* TCSTeam::_FindMember(UInt64 playerId)
{
	UInt32 squadId = _GetPlayerSquadId(playerId);
	TeamCopySquad* squad = _FindSquad(squadId);
	if (NULL == squad)
	{
		ErrorLogStream << TEAMINFO(this) << "not find squad id:" << squadId << " playerId:" << playerId << LogStream::eos;
		return NULL;
	}

	for (auto& it : squad->teamMemberList)
	{
		if (it.playerId == playerId)
		{
			return &it;
		}
	}

	return NULL;
}

TeamCopyMember* TCSTeam::_FindMember(UInt32 seatId)
{
	for (auto& iter : m_SquadVec)
	{
		for (auto& it : iter.teamMemberList)
		{
			if (it.seatId == seatId)
			{
				return &it;
			}
		}
	}

	return NULL;
}

TeamCopyMember* TCSTeam::_FindCaptain(UInt64 playerId)
{
	UInt32 squadId = _GetPlayerSquadId(playerId);
	TeamCopySquad* squad = _FindSquad(squadId);
	if (NULL == squad)
	{
		ErrorLogStream << TEAMINFO(this) << "not find squad id:" << squadId << " playerId:" << playerId << LogStream::eos;
		return NULL;
	}

	for (auto& iter : squad->teamMemberList)
	{
		if (iter.post & TEAM_COPY_POST_CAPTAIN)
		{
			return &iter;
		}
	}

	return NULL;
}

TeamCopyMember* TCSTeam::_FindChief()
{
	UInt32 squadId = _GetPlayerSquadId(GetChiefId());
	TeamCopySquad* squad = _FindSquad(squadId);
	if (NULL == squad)
	{
		ErrorLogStream << TEAMINFO(this) << "not find squad id:" << squadId << LogStream::eos;
		return NULL;
	}

	for (auto& iter : squad->teamMemberList)
	{
		if (iter.playerId == GetChiefId())
		{
			return &iter;
		}
	}

	return NULL;
}

TeamCopySquad* TCSTeam::_FindSquad(UInt32 squadId)
{
	if (squadId == 0 || squadId > GetTeamCopyValue(TC_VALUE_SQUAD_NUM))
	{
		return NULL;
	}

	return &(m_SquadVec[squadId - 1]);
}

UInt32 TCSTeam::_SquadMemberNum(UInt32 squadId)
{
	TeamCopySquad* squad = _FindSquad(squadId);
	if (NULL == squad)
	{
		return true;
	}

	return squad->teamMemberList.size();
}

UInt32 TCSTeam::_GetPlayerSquadId(UInt64 playerId)
{
	auto iter = m_PlayerSquad.find(playerId);
	return iter != m_PlayerSquad.end() ? iter->second : 0;
}

UInt32 TCSTeam::_UpdateField(UInt32 fieldId)
{
	TeamCopyField* copyField = FindField(fieldId);
	if (NULL == copyField)
	{
		//ErrorLogStream << TEAMINFO(this) << "not find field id:" << fieldId << LogStream::eos;
		return TEAM_COPY_FIELD_STATUS_INVALID;
	}

	// 如果是能量据点，并且有能量恢复。需要缩减能量回复的比例
	if (copyField->fieldId == GetTeamCopyValue(TC_VALUE_DIFF_ENERGY_REVIVE_FIELD) && copyField->energyReviveTime != 0)
	{
		UInt32 curTime = (UInt32)CURRENT_TIME.Sec();
		UInt32 reduceTime = GetTeamCopyValue(TC_VALUE_DIFF_ENERGY_REVIVE_CLEAR_REDUCE_RATE);
		if (curTime - (copyField->energyReviveTime) <= reduceTime)
		{
			copyField->energyReviveTime = curTime;
		}
		else if(curTime - (copyField->energyReviveTime) >= 1000) // 1000秒达到百分之百能量积蓄
		{
			copyField->energyReviveTime = (curTime - reduceTime);
		}
		else
		{
			copyField->energyReviveTime += reduceTime;
		}

		// 通知战斗中的，能量据点压制情况
		_HandleEnergyReviveField(curTime - (copyField->energyReviveTime), copyField->config->dungeonId);

		FieldNotify(fieldId);

		return copyField->state;
	}

	// 据点已经被歼灭
	if (copyField->state == TEAM_COPY_FIELD_STATUS_DEFEAT)
	{
		return TEAM_COPY_FIELD_STATUS_INVALID;
	}

	// 处理当前据点
	if (copyField->oddNum != 0)
	{
		copyField->oddNum--;
	}

	if (copyField->rebornTime == 0 && copyField->config->rebornTime != 0 && copyField->oddNum < copyField->config->defeatCond)
	{
		copyField->rebornTime = UInt32(CURRENT_TIME.Sec()) + copyField->config->rebornTime;
		copyField->state = TEAM_COPY_FIELD_STATUS_REBORN;
	}

	// 需要更新的据点
	std::vector<UInt32> filedVec;
	filedVec.push_back(fieldId);

	std::vector<TeamCopyFieldDataEntry*> tempVec;

	// 是否解锁新据点
	if (copyField->isFinish())
	{
		std::vector<TeamCopyFieldDataEntry*> fieldVec;
		TeamCopyFieldDataEntryMgr::Instance()->GetUnlockFieldList(fieldId, fieldVec);
		for (auto& fv : fieldVec)
		{
			// 据点已解锁
			TeamCopyField* tt1 = FindField(fv->id);
			if (tt1 != NULL)
			{
				continue;
			}

			// 据点未解锁，查看其他关联据点是否解锁
			bool isUnlock = true;
			for (auto& uc : fv->unlockedCond)
			{
				if (uc == fieldId)
					continue;

				TeamCopyField* tt2 = FindField(uc);
				if (NULL == tt2 || !(tt2->isFinish()))
				{
					isUnlock = false;
					break;
				}
			}

			if (isUnlock)
			{
				tempVec.push_back(fv);
			}
		}

		// 据点被歼灭
		if (copyField->config->rebornTime == 0)
		{
			copyField->state = TEAM_COPY_FIELD_STATUS_DEFEAT;

			// 更新关联据点状态
			for (auto& ff : copyField->config->updateFieldStatusVec)
			{
				TeamCopyField* updateField = FindField(ff.fieldId);
				if (NULL == updateField)
					continue;
	
				updateField->state = ff.status;
				filedVec.push_back(updateField->fieldId);
			}

			// 结束该剧点中的其他小队战斗
			FieldRaceEnd(fieldId);

			// 如果是boss衍生据点，则通知地下城，该剧点被歼灭
			if (copyField->config->presentedType == TEAM_COPY_APPEAR_BOSS_ATTACH)
			{
				if(_BossFieldAttachDestroyNotify(copyField->config->dungeonId))
				{
					copyField->state = TEAM_COPY_FIELD_STATUS_UNLOCKING;
					copyField->oddNum = 1;
				}
			}

			// 尾部关卡联动
			UInt32 tailField = GetTailFieldId(GetTeamGrade());
			if (copyField->fieldId == tailField)
			{
				InfoLogStream << TEAMINFO(this) << " tail field destroy!" << LogStream::eos;
				NotifyAllSquadDungeonDestory(copyField->config->dungeonId);
			}

			// 噩梦难度,能量恢复据点联动
			if (copyField->fieldId == GetTeamCopyValue(TC_VALUE_DIFF_ENERGY_REVIVE_FIELD))
			{
				copyField->state = TEAM_COPY_FIELD_STATUS_ENERGY_REVIVE;
				copyField->oddNum = 1;
				copyField->energyReviveTime = (UInt32)CURRENT_TIME.Sec();
			}

			// 通知据点状态
			_FieldStatusNotify(fieldId, copyField->state);
		}
	}

	// 开启新据点
	for (auto& it : tempVec)
	{
		AddField(it);
		filedVec.push_back(it->id);
	}

	// 通知据点
	FieldNotify(filedVec);

	return copyField->state;
}

void TCSTeam::_StageEnd(UInt32 stage, UInt32 param)
{
	CLProtocol::TeamCopyStageEnd end;
	end.stageId = stage;
	end.passType = param;
	BroadcastToMember(end);
}

void TCSTeam::_AddFlop(UInt32 stage, const TeamCopyFlop& flop)
{
	auto iter = m_FlopMap.find(stage);
	if (iter == m_FlopMap.end())
	{
		std::vector<TeamCopyFlop> tempVec;
		tempVec.push_back(flop);
		m_FlopMap[stage] = tempVec;
	}
	else
	{
		iter->second.push_back(flop);
	}
}

UInt32 TCSTeam::_CommonPlayerNum()
{
	if (GetModel() == TEAM_COPY_TEAM_MODEL_GOLD)
	{
		return GetTeamMemberNum() - GoldPlayerNum();
	}

	return GetTeamMemberNum();
}

void TCSTeam::_VoteNotify(UInt64 playerId, UInt32 vote)
{
	CLProtocol::TeamCopyVoteNotify  notify;
	notify.roleId = playerId;
	notify.vote = vote;
	BroadcastToMember(notify);
}

void TCSTeam::_VoteFinish()
{
	DebugLogStream << TEAMINFO(this) << "vote finish !" << LogStream::eos;

	SetVoteEndTime(0);
	SetStatus(TEAM_COPY_TEAM_STATUS_PARPARE);

	SetCheckCondEndTime((UInt32)CURRENT_TIME.Sec() + GetTeamCopyValue(TC_VALUE_CHECK_COND_TIME));

	SetConsumeFlag(false);

	// 扣除个玩家门票
	CLProtocol::TeamCopyPlayerCheckReq checkReq;
	checkReq.teamId = GetTeamId();
	checkReq.teamGrade = GetTeamGrade();
	checkReq.teamMode = GetModel();

	// 团本门票
	{
		GetTicket(checkReq.consumeItemVec);
	}

	// 清除检查条件列表
	TCSTeamMgr::Instance()->DelTeamCheck(GetTeamId());

	for (auto& iter : m_VoteSet)
	{
		TeamCopyMember* member = _FindMember(iter);
		if (NULL == member)
		{
			ErrorLogStream << TEAMINFO(this) << "not find member player id:" << iter << LogStream::eos;
			return;
		}

		// 加入检查条件列表
		TCSTeamMgr::Instance()->AddTeamCheck(GetTeamId(), iter);

		// 如果是金主，检查背包中的金币是否够佣金
		if (member->post & TEAM_COPY_POST_GOLD)
		{
			checkReq.forzenCommission.id = GetTeamCopyValue(TC_VALUE_COMMISSION_ID);
			checkReq.forzenCommission.num = GetCondCommission();
		}
		else
		{
			checkReq.forzenCommission.id = 0;
			checkReq.forzenCommission.num = 0;
		}

		checkReq.roleId = iter;

		TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(iter);
		if (NULL == player)
		{
			ErrorLogStream << TEAMINFO(this) << "send check packet error! player id:" << iter << LogStream::eos;
			continue;
		}

		player->SendToScene(checkReq);
	}
}

void TCSTeam::_VoteTimeOut()
{
	CLProtocol::TeamCopyVoteFinish voteF;
	voteF.result = 1;

	for (auto& iter : m_PlayerSquad)
	{
		auto it = m_VoteSet.find(iter.first);
		if (it == m_VoteSet.end())
		{
			TeamCopyMember* member = _FindMember(iter.first);
			if (NULL == member)
			{
				continue;
			}

			voteF.notVotePlayer.push_back(member->playerName);
		}
	}

	if (!voteF.notVotePlayer.empty())
	{
		BroadcastToMember(voteF);
	}

	SetVoteEndTime(0);
	SetStatus(TEAM_COPY_TEAM_STATUS_PARPARE);
}

void TCSTeam::_CheckCondTimeOut()
{
	SetCheckCondEndTime(0);

	TCSTeamCheck* check = TCSTeamMgr::Instance()->GetTeamCheck(GetTeamId());
	if (NULL == check)
	{
		ErrorLogStream << TEAMINFO(this) << "not find team check!" << LogStream::eos;
		return;
	}

	CLProtocol::TeamCopyStartBattleRes res;
	res.retCode = ErrorCode::TEAM_COPY_PLAYER_UNKNOWN_ERROR;

	for (auto& iter : check->GetCheckPlayerSet())
	{
		TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(iter);
		if (NULL == player)
		{
			continue;
		}

		ErrorLogStream << TEAMINFO(this) << "teamMember:" << player->GetName() << " check cond time!" << LogStream::eos;

		res.roleName = player->GetName();
		BroadcastToMember(res);
	}
}

void TCSTeam::_GameOverCommission(bool isReturn)
{
	if (GetModel() != TEAM_COPY_TEAM_MODEL_GOLD)
	{
		return;
	}

	CLProtocol::SceneCurrencyFrozenOpReq req;
	req.opType = isReturn ? 0 : 1;

	InfoLogStream << TEAMINFO(this) << "start op commission type:" << req.opType << LogStream::eos;

	for (auto& iter : m_CommissiomMap)
	{
		GameZone* zone = GameZoneMgr::Instance()->FindGameZone(iter.second.connId);
		if (NULL == zone)
		{
			ErrorLogStream << TEAMINFO(this) << "op commission failed !!! playerId:" << iter.first << " orderId:" << iter.second.frozenOrder << LogStream::eos;
			continue;
		}

		InfoLogStream << TEAMINFO(this) << "op commission success playerId:" << iter.first << " orderId:" << iter.second.frozenOrder << LogStream::eos;

		req.guid = iter.second.frozenOrder;
		zone->TransmitToScene(req);
	}
}

void TCSTeam::_TeamStatusNotify(UInt32 status)
{
	CLProtocol::TeamCopyTeamStatusNotify notify;
	notify.teamId = GetTeamId();
	notify.teamStatus = status;
	BroadcastToMember(notify);
}

void TCSTeam::_CommissionBonus(UInt32 senderId, UInt32 titleId, UInt32 contentId)
{
	if (GetModel() != TEAM_COPY_TEAM_MODEL_GOLD)
	{
		return;
	}

	// 计算佣金分成数量
	UInt32 comNum = CommissionBonusNum();
	ItemReward reward(GetTeamCopyValue(TC_VALUE_COMMISSION_ID), comNum, 0, 0);

	std::vector<ItemReward> rewardVec;
	rewardVec.push_back(reward);

	std::string sender = SysNotifyMgr::Instance()->MakeString(senderId);
	std::string title = SysNotifyMgr::Instance()->MakeString(titleId);
	std::string content = SysNotifyMgr::Instance()->MakeString(contentId);
	std::string reason = GetReason(SOURCE_TYPE_TEAM_COPY_COMMISSION_BONUS);

	for (auto& iter : m_SquadVec)
	{
		for (auto& it : iter.teamMemberList)
		{
			if ( !(it.post & TEAM_COPY_POST_GOLD))
			{
				if ( !SysUtility::SendMail(it.connId, it.playerId, sender, title, content, reason, rewardVec) != ErrorCode::SUCCESS)
				{
					ErrorLogStream << TEAMINFO(this) << "CommissionBonus Mail Error! playerId" << it.playerId << " commission:" << comNum << LogStream::eos;
				}
			}
		}
	}
}

void TCSTeam::_AddModelPassNum(UInt32 model, UInt32 costTime)
{
	for (auto& iter : m_PlayerSquad)
	{
		TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(iter.first);
		if (NULL == player)
		{
			ErrorLogStream << TEAMINFO(this) << "not find player id:" << iter.first << LogStream::eos;
			continue;
		}

		InfoLogStream << TEAMINFO(this) << "add player challenge num " << PLAYERINFO(player) << LogStream::eos;

		player->AddModelNum(GetTeamType(), model, 1);

		// 通知各玩家的scene通关事件
		CLProtocol::SceneSyncEvent syncEvent;
		syncEvent.playerId = player->GetID();
		syncEvent.type = SET_TEAM_COPY_CLEAR_DUNGEON;
		syncEvent.param1 = GetTeamGrade();
		syncEvent.param2 = costTime;
		syncEvent.param3 = GetTeamType();
		player->SendToScene(syncEvent);
	}
}

void TCSTeam::_MemberNotify(const std::string& name, UInt32 flag)
{
	CLProtocol::TeamCopyMemberNotify notify;
	notify.memberName = name;
	notify.flag = flag;
	BroadcastToMember(notify);
}

bool TCSTeam::_BossFieldAttachDestroyNotify(UInt32 dungeonId)
{
	// 找到boss据点
	UInt32 bossFieldId = TeamCopyFieldDataEntryMgr::Instance()->GetBossFieldId(GetTeamGrade());
	TeamCopyField* bossField = FindField(bossFieldId);
	if (NULL == bossField)
		return false;

	for (auto& tt : bossField->fieldRaceList)
	{
		InfoLogStream << TEAMINFO(this) << "boss attach fieldId:" << bossFieldId << " dungeonId:" << dungeonId << " raceId:" << tt << LogStream::eos;
		TCSDungeonMgr::Instance()->DungeonRaceDestroy(tt, dungeonId);
	}

	// 如果是boss衍生据点，特殊处理
	if (bossField->fieldRaceList.empty())
	{
		return true;
	}

	return false;
}

void TCSTeam::_FieldStatusNotify(UInt32 fieldId, UInt32 status)
{
	CLProtocol::TeamCopyFieldStatusNotify notify;
	notify.fieldId = fieldId;
	notify.fieldStatus = status;
	BroadcastToMember(notify);
}

void TCSTeam::_AddForceEndRace(UInt64 raceId)
{
	m_ForceEndRaceSet.insert(raceId);
}

bool TCSTeam::_IsForceEndRace(UInt64 raceId)
{
	auto it = m_ForceEndRaceSet.find(raceId);
	return it != m_ForceEndRaceSet.end();
}

void TCSTeam::_UdpStartBattle()
{
	for (auto& iter : m_PlayerSquad)
	{
		TeamCopyMember* member = _FindMember(iter.first);
		if (NULL == member)
			continue;

		TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(iter.first);
		if (NULL == player)
			continue;

		UInt32 isGold = (member->post & TEAM_COPY_POST_GOLD) ? 1 : 0;
		player->SendUdpLog("team_copy_start_battle", LOG_TEAM_COPY_START_BATTLE, member->post, isGold, player->IsCreateGoldTeam(GetTeamType()),
			GetTeamId(), GetModel(), GetTeamMemberNum(), 0, GetPlanModel(), GetCondCommission(), GetEquipScore(), 
			GetTeamGrade(), GetTeamType());
	}
}

void TCSTeam::_NotifyForceEndFlag()
{
	if ( !GetForceEndFlag())
	{
		return;
	}

	TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(m_ChiefId);
	if (NULL != player)
	{
		CLProtocol::TeamCopyForceEndFlag flag;
		player->SendProtocol(flag);
	}
}

void TCSTeam::_HandleEnergyReviveField(UInt32 progress, UInt32 energyDungeonId)
{
	if (GetTeamGrade() != TEAM_COPY_TEAM_GRADE_DIFF)
	{
		return;
	}

	// boss据点通知能量积蓄
	_NotifyRaceEnergyRevive(progress, energyDungeonId, TEAM_COPY_APPEAR_BOSS);

	// boss衍生据点通知能量积蓄
	_NotifyRaceEnergyRevive(progress, energyDungeonId, TEAM_COPY_APPEAR_BOSS_ATTACH);
}

void TCSTeam::_NotifyRaceEnergyRevive(UInt32 progress, UInt32 energyDungeonId, UInt32 needNotifyFieldType)
{
	CLProtocol::TeamCopyNotifyBimsEnergyAccuProgress notify;
	notify.progress = progress;
	notify.dungeonId = energyDungeonId;

	std::vector<TeamCopyFieldDataEntry*> fieldList;
	TeamCopyFieldDataEntryMgr::Instance()->GetFieldList(GetTeamGrade(), needNotifyFieldType, fieldList);
	for (auto& iter : fieldList)
	{
		TeamCopyField* field = FindField(iter->id);
		if (NULL == field)
		{
			ErrorLogStream << TEAMINFO(this) << "not find field:" << iter->id << LogStream::eos;
			continue;
		}

		for (auto& it : field->fieldRaceList)
		{
			notify.raceSessionId = it;
			TCSDungeonMgr::Instance()->SendToDungeonSvr(notify);
		}
	}
}

UInt32 TCSTeam::_GetGameStartTime()
{
	return GetGameOverTime() - GetTeamCopyValue(TC_VALUE_ONE_GAME_TIME);
}

UInt32 TCSTeam::_GameIsCrossDate(UInt32 time)
{
	if (!isSameWeek(time * 1000, _GetGameStartTime() * 1000, GetTCRefreshWeek(GetTeamType()), 6))
	{
		return TEAM_COPY_CROSS_WEEK;
	}
	else if (!CheckInSameLogicDay(time, _GetGameStartTime()))
	{
		return TEAM_COPY_CROSS_DAY;
	}

	return TEAM_COPY_CROSS_NULL;
}

bool TCSTeam::_StageIsSettle(UInt32 stage, UInt64 playerId)
{
	auto iter = m_StageSettlePlayerMap.find(stage);
	if (iter == m_StageSettlePlayerMap.end())
	{
		return false;
	}

	auto it = iter->second.find(playerId);
	if (it == iter->second.end())
	{
		return false;
	}

	return true;
}

void TCSTeam::_RaceSettlement(UInt32 squadId, UInt64 raceId)
{
	// 暂时不处理强制结束的比赛
	if (_IsForceEndRace(raceId))
	{
		return;
	}

	TeamCopySquad* squad = _FindSquad(squadId);
	if (NULL == squad)
	{
		ErrorLogStream << TEAMINFO(this) << " not find squad id:" << squadId << LogStream::eos;
		return;
	}

	std::set<UInt64>& tempSet = m_StageSettlePlayerMap[GetCurStage()];
	for (auto& it : squad->teamMemberList)
	{
		tempSet.insert(it.playerId);
	}
}

void TCSTeam::_RollReward()
{
	// 是否配置该类型、难度的团本
	TeamCopyRollDataEntry* rollEntry = TeamCopyRollDataEntryMgr::Instance()->FindEntry(GetTeamType(), GetTeamGrade());
	if (NULL == rollEntry || rollEntry->rewardNum == 0)
	{
		return;
	}

	// 触发概率
	if ( !Avalon::Random::SelectInTenThousand(rollEntry->triggerRate))
	{
		InfoLogStream << TEAMINFO(this) << " roll rate failed!" << LogStream::eos;
		return;
	}

	// 执行roll奖励
	auto dropResult = DropItemMgr::Instance()->GenDropItems(0, 0, rollEntry->dropId, 0, 0);
	if (dropResult.dropItems.empty())
	{
		ErrorLogStream << TEAMINFO(this) << "roll reward empty!" << LogStream::eos;
		return;
	}

	UInt32 rewardId = dropResult.dropItems[0].typeId;
	UInt32 rewardNum = dropResult.dropItems[0].num;

	InfoLogStream << TEAMINFO(this) << "roll reward itemId:" << rewardId << " itemNum:" << rewardNum << LogStream::eos;

	// 符合职业要求的玩家
	ItemDataEntry* itemEntry = ItemDataEntryMgr::Instance()->FindEntry(rewardId);
	if (NULL == itemEntry)
	{
		ErrorLogStream << TEAMINFO(this) << " item not find id:" << rewardId << LogStream::eos;
		return;
	}

	// 玩家的roll选项(key->playerId, val->TCRollOption)
	UInt32 occuLimitPlayerNum = 0;
	std::map<UInt64, UInt32> playerRollMap;
	std::set<UInt64> hasNumPlayerSet;
	for (auto& iter : m_PlayerSquad)
	{
		TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(iter.first);
		if (NULL == player)
		{
			ErrorLogStream << TEAMINFO(this) << " not find player:" << iter.first << LogStream::eos;
			continue;
		}

		// 有次数的玩家
		if (player->GetRollNum(GetTeamType(), GetTeamGrade()) < rollEntry->rewardNum)
		{
			hasNumPlayerSet.insert(iter.first);
			playerRollMap[iter.first] = TC_ROLL_NORMAL;

			// 职业筛查必须
			if (!itemEntry->IsMeetOccuRequirement(Occupation(player->GetOccu())))
			{
				++occuLimitPlayerNum;
				playerRollMap[iter.first] = TC_ROLL_NO_NEED;
			}
		}
		else
		{
			playerRollMap[iter.first] = TC_ROLL_NO_NUM;
		}
	}

	// 大家都没次数
	if (hasNumPlayerSet.empty())
	{
		InfoLogStream << TEAMINFO(this) << "not player has roll num!" << LogStream::eos;
		return;
	}

	// 有次数的玩家是否最终翻牌
	UInt32 hasNumAndFinalFlop = 0;
	for (auto& it : hasNumPlayerSet)
	{
		for (auto& tt : m_FlopMap[TEAM_COPY_STAGE_FINAL])
		{
			if (tt.playerId == it && tt.isLimit != TEAM_COPY_FLOP_LIMIT_NULL)
			{
				++ hasNumAndFinalFlop;
				playerRollMap[it] = TC_ROLL_NO_POWER;
			}
		}
	}

	if (hasNumAndFinalFlop == hasNumPlayerSet.size())
	{
		InfoLogStream << TEAMINFO(this) << "roll failed roll num and final flop num:" << hasNumAndFinalFlop << LogStream::eos;
		return;
	}

	UInt32 rollTime = GetTeamCopyValue(TC_VALUE_ROLL_TIME);
	m_RollEndStamp = UInt32(CURRENT_TIME.Sec()) + rollTime;

	CLProtocol::TeamCopyRollNotify rollNotify;
	rollNotify.rollEndStamp = m_RollEndStamp;
	rollNotify.rollTime = rollTime;
	rollNotify.rollItemId = rewardId;
	rollNotify.rollItemNum = rewardNum;

	// 有次数的人都被职业限制了，奖励转换
	if (occuLimitPlayerNum == hasNumPlayerSet.size())
	{
		InfoLogStream << TEAMINFO(this) << "occu limit roll reward convert!" << LogStream::eos;

		rollNotify.convertItem = rollEntry->convertItemVec;
		for (auto& it : rollEntry->convertItemVec)
		{
			m_RollRewards.push_back(ItemReward(it.itemId, it.itemNum, 0, 0));
		}
	}
	else
	{
		m_RollRewards.push_back({ rewardId, rewardNum, 0, 0});
		m_RollPointVec = TeamCopyRollDataEntryMgr::Instance()->GetRollPointVec();
	}

	for (auto& iter : playerRollMap)
	{
		TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(iter.first);
		if (NULL == player)
		{
			ErrorLogStream << TEAMINFO(this) << " not find playerId:" << iter.first << LogStream::eos;
			continue;
		}

		rollNotify.rollOption = iter.second;
		player->SendProtocol(rollNotify);

		// 发转换邮件
		if (occuLimitPlayerNum == hasNumPlayerSet.size())
		{
			if (iter.second == TC_ROLL_NORMAL)
			{
				_SendRollMail(player->GetID(), player->GetConnId());
			}
		}
		else
		{
			TCPlayerRoll playerRoll;
			playerRoll.playerId = iter.first;
			playerRoll.rollPoint = 0;
			playerRoll.rollOption = iter.second;
			m_RollMap[iter.first] = playerRoll;
		}
	}
}

UInt32 TCSTeam::_GetRollPoint()
{
	UInt32 randNum = Avalon::Random::RandBetween(1, m_RollPointVec.size()) - 1;
	UInt32 point = m_RollPointVec[randNum];
	m_RollPointVec.erase(m_RollPointVec.begin() + randNum);

	return point;
}

void TCSTeam::_OnRollTick(const Avalon::Time& now)
{
	if (m_RollEndStamp == 0 || UInt32(now.Sec()) < m_RollEndStamp)
	{
		return;
	}

	m_RollEndStamp = 0;
	UInt32 maxPoint = 0;
	UInt64 winPlayerId = 0;
	CLProtocol::TeamCopyRollResult result;

	for (auto& iter : m_RollMap)
	{
		TCPlayerRoll& playerRoll = iter.second;

		// 扣次数
		if (playerRoll.rollOption == TC_ROLL_NORMAL || playerRoll.rollOption == TC_ROLL_NO_NEED)
		{
			TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(iter.first);
			if (NULL == player)
			{
				ErrorLogStream << TEAMINFO(this) << " not find playerId:" << iter.first << LogStream::eos;
				continue;
			}

			player->AddRollNum(GetTeamType(), GetTeamGrade());
			player->PlayerInfoToScene(GetTeamType());
			player->PlayerInfoToClient(GetTeamType());
		}

		// 没有选择的玩家帮他选择谦让
		if (playerRoll.rollOption == TC_ROLL_NORMAL && playerRoll.rollPoint == 0)
		{
			playerRoll.rollOption = TC_ROLL_NO_NEED;
		}
		
		result.rollVec.push_back(playerRoll);

		InfoLogStream << TEAMINFO(this) << " roll time end " << playerRoll.SerializeStr() << LogStream::eos;

		if (playerRoll.rollPoint > maxPoint)
		{
			maxPoint = playerRoll.rollPoint;
			winPlayerId = playerRoll.playerId;
		}
	}

	// 最大点数为0，所有人都选择了谦让, 给所有谦让的人roll点
	if (maxPoint == 0)
	{
		result.rollVec.clear();

		for (auto& iter : m_RollMap)
		{
			TCPlayerRoll& playerRoll = iter.second;
			if (playerRoll.rollOption != TC_ROLL_NO_NEED)
			{
				result.rollVec.push_back(playerRoll);
				continue;
			}

			playerRoll.rollOption = TC_ROLL_NORMAL;
			playerRoll.rollPoint = _GetRollPoint();

			result.rollVec.push_back(playerRoll);

			if (playerRoll.rollPoint > maxPoint)
			{
				maxPoint = playerRoll.rollPoint;
				winPlayerId = playerRoll.playerId;
			}
		}
	}

	InfoLogStream << TEAMINFO(this) << " roll end playerId:" << winPlayerId << " obtain reward! " << LogStream::eos;
	result.rewardPlayer = winPlayerId;
	BroadcastToMember(result);

	// 发放奖励
	TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(winPlayerId);
	if (player != NULL)
	{
		_SendRollMail(winPlayerId, player->GetConnId());
	}
}

void TCSTeam::_SendRollMail(UInt64 playerId, UInt32 playerConnId)
{
	std::string sender = SysNotifyMgr::Instance()->MakeString(10090);
	std::string title = SysNotifyMgr::Instance()->MakeString(10095);
	std::string content = SysNotifyMgr::Instance()->MakeString(10096);
	std::string reason = GetReason(SOURCE_TYPE_TEAM_COPY_ROLL_REWARD, GetTeamType(), GetTeamGrade());
	if (!SysUtility::SendMail(playerConnId, playerId, sender, title, content, reason, m_RollRewards) != ErrorCode::SUCCESS)
	{
		ErrorLogStream << TEAMINFO(this) << "roll mail error! playerId:" << playerId << LogStream::eos;
	}
}

void TCSTeam::CheckCondFinish()
{
	m_VoteSet.clear();
	SetGameOverTime(GetTeamCopyValue(TC_VALUE_ONE_GAME_TIME) + (UInt32)CURRENT_TIME.Sec());

	// 广播客户端， 投票完成
	CLProtocol::TeamCopyVoteFinish voteF;
	voteF.result = 0;
	BroadcastToMember(voteF);

	SetStatus(TEAM_COPY_TEAM_STATUS_BATTLE);
	_TeamStatusNotify(TEAM_COPY_TEAM_STATUS_BATTLE);

	// 开战日志
	_UdpStartBattle();
}