#include "TCSTeamMgr.h"

#include <CLErrorCode.h>
#include <CLTeamCopyProtocol.h>
#include <CLGameSessionProtocol.h>
#include <CLChatProtocol.h>
#include <CLCrossDungeonDefine.h>
#include <CLTeamCopyTargetDataEntry.h>
#include <CLTeamCopyFieldDataEntry.h>
#include <CLSceneDataEntry.h>
#include <CLTeamCopyDefine.h>
#include <CLMsgParser.h>
#include <CLSysNotify.h>
#include <udplog_typedef.h>

#include "TCSTeam.h"
#include "TCSTeamOne.h"
#include "TCSTeamTwo.h"
#include "TCSTeamCheck.h"
#include "TCSTeamPlayerInfo.h"
#include "TCSPlayerMgr.h"
#include "TCSceneMgr.h"
#include "TCSDungeonMgr.h"
#include "SysUtility.h"
#include "TCSDungeonRace.h"
#include "GameZoneMgr.h"
#include "TCSApplication.h"


UInt32 GetCheckErrCode(UInt32 code)
{
	switch (code)
	{
	case TEAM_COPY_CHECK_CODE_SUCCESS: return ErrorCode::SUCCESS;
	case TEAM_COPY_CHECK_CODE_COUNT: return ErrorCode::TEAM_COPY_COUNT_NOT_ENOUGH;
	case TEAM_COPY_CHECK_CODE_TICKET: return ErrorCode::TEAM_COPY_TICKET_NOT_ENOUGH;
	case TEAM_COPY_CHECK_CODE_COMMISSION: return ErrorCode::TEAM_COPY_COMMISSION_NOT_ENOUGH;
	case TEAM_COPY_CHECK_CODE_UNKNOWN: return ErrorCode::TEAM_COPY_PLAYER_UNKNOWN_ERROR;
	default: break;
	}

	return ErrorCode::SUCCESS;
}

TCSTeamMgr::TCSTeamMgr()
{
	m_TeamIdSeed = 0;
	m_60sTimer.SetInterval(60 * 1000);
}

TCSTeamMgr::~TCSTeamMgr()
{

}

void TCSTeamMgr::OnTick(const Avalon::Time& now)
{
	UInt32 battleTeam = 0;
	for (auto& iter : m_TeamMap)
	{
		iter.second->OnTick(now);

		// 战斗中的队伍
		if (iter.second->GetStatus() == TEAM_COPY_TEAM_STATUS_BATTLE)
		{
			++battleTeam;
		}
	}

	if (m_60sTimer.IsTimeout(now))
	{
		InfoLogStream << "CURRENT BATTLE TEAM NUM:" << battleTeam << LogStream::eos;
	}

	TCSTeamPlayerInfoMap::iterator it = m_TeamPlayerInfoMap.begin();
	while (it != m_TeamPlayerInfoMap.end())
	{
		TCSTeamPlayerInfo& info = it->second;
		if (info.GetTimeOut() == 0)
		{
			++it;
			continue;
		}

		if (info.GetTimeOut() > now.Sec())
		{
			++it;
			continue;
		}

		UInt32 teamId = GET_TEAM_ID(info.GetTeamSquadId());
		TCSTeam* team = _FindTeam(teamId);
		if (team != NULL)
		{
			team->QueryRacePlayerInfoTimeOut(GET_SQUAD_ID(info.GetTeamSquadId()));
		}

		it = m_TeamPlayerInfoMap.erase(it);
	}
}

void TCSTeamMgr::Final()
{
	for (auto& iter : m_TeamMap)
	{
		TCSTeam* team = iter.second;
		if (NULL == team)
			continue;

		if (TCSApplication::Instance()->IsReturnNumStopServer())
		{
			// 战斗中的所有玩家返还次数
			if (team->IsInBattle())
			{
				InfoLogStream << TEAMINFO(team) << " final return challenge num!" << LogStream::eos;
				team->FailedReturnChallengeNum();
			}
		}

		delete team;
	}

	m_TeamMap.clear();
}

bool TCSTeamMgr::IsExistTeam(UInt32 teamId)
{
	return _FindTeam(teamId) != NULL;
}

void TCSTeamMgr::ValCfgSyncToScene(UInt32 connId)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(connId);
	if (NULL == zone)
	{
		ErrorLogStream << "not find zone id:" << connId << LogStream::eos;
		return;
	}

	InfoLogStream << "sync config val to connid:" << connId << LogStream::eos;

	CLProtocol::TeamCopyCfgValRes res;
	res.valCfgMap[TC_VALUE_TICKET_ID_1] = GetTeamCopyValue(TC_VALUE_TICKET_ID_1);
	res.valCfgMap[TC_VALUE_TICKET_NUM_1] = GetTeamCopyValue(TC_VALUE_TICKET_NUM_1);
	res.valCfgMap[TC_VALUE_TICKET_ID_2] = GetTeamCopyValue(TC_VALUE_TICKET_ID_2);
	res.valCfgMap[TC_VALUE_TICKET_NUM_2] = GetTeamCopyValue(TC_VALUE_TICKET_NUM_2);
	
	res.valCfgMap[TC_VALUE_2_TICKET_ID_1] = GetTeamCopyValue(TC_VALUE_2_TICKET_ID_1);
	res.valCfgMap[TC_VALUE_2_TICKET_NUM_1] = GetTeamCopyValue(TC_VALUE_2_TICKET_NUM_1);
	res.valCfgMap[TC_VALUE_2_TICKET_ID_2] = GetTeamCopyValue(TC_VALUE_2_TICKET_ID_2);
	res.valCfgMap[TC_VALUE_2_TICKET_NUM_2] = GetTeamCopyValue(TC_VALUE_2_TICKET_NUM_2);

	zone->TransmitToScene(res);
}

void TCSTeamMgr::HandleCreateTeamReq(TCSPlayer* player, CLProtocol::TeamCopyCreateTeamReq& req)
{
	if (NULL == player) return;
	CLProtocol::TeamCopyCreateTeamRes res;

	do 
	{
		if(TCSApplication::Instance()->IsCloseTeamType(req.teamType))
		{
			res.retCode = ErrorCode::TEAM_COPY_CLOSE;
			break;
		}

		UInt32 teamId = _GetPlayerTeamId(player->GetID());
		if (teamId != 0)
		{
			ErrorLogStream << "player has team, create failed!"<< player->GetID() << LogStream::eos;
			res.retCode = ErrorCode::TEAM_COPY_HAS_TEAM;
			break;
		}

		// 如果是金团，检查是否符合条件
		if (req.teamModel == TEAM_COPY_TEAM_MODEL_GOLD)
		{
			if (TCSApplication::Instance()->GetIsOpenGoldTeam())
			{
				res.retCode = ErrorCode::SYS_NOT_OPEN;
				break;
			}

			if (!player->IsCreateGoldTeam(req.teamType))
			{
				res.retCode = ErrorCode::TEAM_COPY_COND_LIMIT_NOT_CREATE;
				break;
			}
		}

		teamId = ++m_TeamIdSeed;

		TCSTeam* team = _CreateTeam(req.teamType);
		if (NULL == team)
		{
			ErrorLogStream << PLAYERINFO(player) << "create invalid team type:" << req.teamType << LogStream::eos;
			return;
		}

		team->SetTeamType(req.teamType);
		team->SetTeamId(teamId);
		team->SetTeamGrade(req.teamGrade);
		team->SetTeamName(player->GetName());
		team->SetModel(req.teamModel);
		team->SetEquipScore(req.equipScore);
		team->SetCondCommission(req.param);
		team->SetStatus(TEAM_COPY_TEAM_STATUS_PARPARE);
		team->SetChiefId(player->GetID());

		std::string voiceId;
		voiceId.reserve(128);
		voiceId.append(std::to_string(player->GetID())).append("_").append(std::to_string(CURRENT_TIME.Sec()));
		team->SetVoiceRoomId(voiceId);

		res.retCode = team->JoinTeam(player);
		if (res.retCode != ErrorCode::SUCCESS)
		{
			ErrorLogStream << "player create team failed! reason:" << res.retCode << LogStream::eos;
			CL_SAFE_DELETE(team);
			break;
		}

		_AddTeamMap(team);
		_SetPlayerTeamId(player->GetID(), teamId);
		team->TeamDataRes(player);

		InfoLogStream << TEAMINFO((team)) << "create team !" << LogStream::eos;

		res.retCode = ErrorCode::SUCCESS;
	} while (0);

	player->SendProtocol(res);
}

void TCSTeamMgr::HandleTeamDataReq(TCSPlayer* player, UInt32 teamType)
{
	if (NULL == player) return;
	
	TCSTeam* team = _FindTeam(player->GetID());
	if (NULL == team)
	{
		CLProtocol::TeamCopyTeamDataRes res;
		player->SendProtocol(res);
		return;
	}

	if (team->GetTeamType() != teamType)
	{
		CLProtocol::TeamCopyTeamDataRes res;
		res.teamType = team->GetTeamType();
		player->SendProtocol(res);
		return;
	}

	team->TeamDataRes(player);
	team->AttackDataNotify(player);
}

void TCSTeamMgr::HandleTeamDetailReq(TCSPlayer* player, UInt32 teamId)
{
	if (NULL == player) return;

	TCSTeam* team = _FindTeam(teamId);
	if (NULL == team) return;

	CLProtocol::TeamCopyTeamDetailRes res;
	res.teamId = team->GetTeamId();
	res.teamType = team->GetTeamType();
	res.teamModel = team->GetModel();
	res.teamName = team->GetTeamName();
	res.totalCommission = team->GetTotalCommission();
	res.bonusCommission = team->CommissionBonusNum();
	res.squadList = team->GetSquadList();

	player->SendProtocol(res);
}

void TCSTeamMgr::HandleTeamListReq(TCSPlayer* player, UInt32 pageNum, UInt32 type, UInt32 model)
{
	if (NULL == player) return;

	std::vector<TCSTeam*> tempVec;
	for (auto& it: m_TeamMap)
	{
		TCSTeam* team = it.second;
		if (NULL == team || team->GetTeamType() != type)
		{
			continue;
		}

		// 队伍战斗，或战斗结束
		if (team->IsYetStartBattle())
		{
			continue;
		}

		// 队伍只剩团长，并且团长已经离线
		if (it.second->GetTeamMemberNum() == 1 && TCSPlayerMgr::Instance()->PlayerIsExpire(team->GetChiefId()))
		{
			continue;
		}

		if (model != TEAM_COPY_TEAM_MODEL_DEFAULT && it.second->GetModel() != model)
		{
			continue;
		}

		tempVec.push_back(team);
	}

	UInt32 pagePlayerNum = GetTeamCopyValue(TC_VALUE_PAGE_NUM);
	if (pagePlayerNum == 0)
		pagePlayerNum = 10;	// 默认每页10个
	
	UInt32 totalPageNum = UInt32(ceil(tempVec.size() / pagePlayerNum));
	if (totalPageNum == 0 && tempVec.size())
		totalPageNum = 1;

	UInt32 n = 0;
	UInt32 k = (pageNum - 1) * pagePlayerNum;

	CLProtocol::TeamCopyTeamListRes res;
	res.curPage = pageNum > totalPageNum ? 1: pageNum;
	res.totalPageNum = totalPageNum;

	for (;k < tempVec.size(); ++k)
	{
		if (n >= pagePlayerNum)
			break;

		TCSTeam* team = tempVec[k];
		if (NULL == team)
			continue;

		++n;
		CLProtocol::TeamCopyTeamProperty property;
		property.teamId = team->GetTeamId();
		property.teamModel = team->GetModel();
		property.commission = team->GetCondCommission();
		property.teamName = team->GetTeamName();
		property.teamType = team->GetTeamType();
		property.teamGrade = team->GetTeamGrade();
		property.memberNum = team->GetTeamMemberNum();
		property.equipScore = team->GetEquipScore();
		property.status = team->GetStatus();

		res.teamPropertyList.push_back(property);
	}

	player->SendProtocol(res);
}

void TCSTeamMgr::HandleTeamApplyReq(TCSPlayer* player, UInt32 teamId, UInt32 isGold)
{
	if (NULL == player) return;

	CLProtocol::TeamCopyTeamApplyRes res;
	res.expireTime = 0;
	res.teamId = teamId;

	do 
	{
		if (_GetPlayerTeamId(player->GetID()) != 0)
		{
			res.retCode = ErrorCode::TEAM_COPY_HAS_TEAM;
			break;
		}

		TCSTeam* team = _FindTeam(teamId);
		if (NULL == team)
		{
			res.retCode = ErrorCode::TEAM_COPY_NO_EXIST_TEAM;
			break;
		}

		res.expireTime = team->GetJoinTimeLimit(player->GetID());
		if (res.expireTime > CURRENT_TIME.Sec())
		{
			res.retCode = ErrorCode::TEAM_COPY_JOIN_TIME_LIMIT;
			break;
		}
		else
		{
			res.expireTime = 0;
		}

		// 如果金主，并且自动同意入团
		if (isGold && team->GetAutoAgreeGold())
		{
			res.retCode = team->JoinTeam(player, isGold);
			if (res.retCode != ErrorCode::SUCCESS)
			{
				break;
			}

			_SetPlayerTeamId(player->GetID(), teamId);
			team->TeamDataBroadcast();
		}
		else
		{
			// 加入申请列表
			res.retCode = team->CheckJoinTeam(player, isGold);
			if (res.retCode != ErrorCode::SUCCESS)
			{
				break;
			}

			res.retCode = team->JoinApply(player->GetID(), isGold);
			if (res.retCode != ErrorCode::SUCCESS)
			{
				break;
			}

			// 通知团长，有人申请
			TCSPlayer* chiefPlayer = TCSPlayerMgr::Instance()->FindTCSPlayer(team->GetChiefId());
			if (chiefPlayer != NULL)
			{
				CLProtocol::TeamCopyApplyNotify notify;
				notify.IsHasApply = 1;
				chiefPlayer->SendProtocol(notify);
			}
		}

		res.retCode = ErrorCode::SUCCESS;

	} while (0);

	player->SendProtocol(res);
}

void TCSTeamMgr::HandleTeamQuitReq(TCSPlayer* player)
{
	if (NULL == player) return;

	CLProtocol::TeamCopyTeamQuitRes res;
	do 
	{
		res.retCode = _QuitTeam(player->GetID(), player);
		if (res.retCode != ErrorCode::SUCCESS)
		{
			break;
		}

		res.retCode = ErrorCode::SUCCESS;

	} while (0);

	player->SendProtocol(res);
}

void TCSTeamMgr::HandleTeamApplyListReq(TCSPlayer* player)
{
	TCSTeam* team = _FindTeam(_GetPlayerTeamId(player->GetID()));
	if (NULL == team)
	{
		return;
	}

	if (team->GetChiefId() != player->GetID())
	{
		return;
	}

	team->ApplyListRes(player);
}

void TCSTeamMgr::HandleTeamApplyReplyReq(TCSPlayer* player, UInt32 isAgree, std::vector<UInt64>& playerIds)
{
	if (NULL == player) return;
	
	TCSTeam* team = _FindTeam(player->GetID());
	if (NULL == team)
		return;

	CLProtocol::TeamCopyTeamApplyReplyRes res;
	res.retCode = ErrorCode::SUCCESS;
	res.isAgree = isAgree;
	res.playerIds = playerIds;

	if (isAgree != 0)
	{
		// 队伍已经开战，不能让人加入，同时清空申请玩家
		if (team->IsYetStartBattle())
		{
			team->ClearApply();

			res.retCode = ErrorCode::TEAM_COPY_IN_BATTLE;
			player->SendProtocol(res);
			return;
		}

		for (auto& iter : playerIds)
		{
			res.retCode = team->AgreeApply(iter);
			if (res.retCode == ErrorCode::SUCCESS)
			{
				_SetPlayerTeamId(iter, team->GetTeamId());
			}
			else if(res.retCode == ErrorCode::TEAM_COPY_TARGET_CUR_STATUS)
			{
				player->SendProtocol(res);
				res.retCode = ErrorCode::SUCCESS;
			}

			team->RemoveApply(iter);
		}
	}
	else
	{
		// 通知玩家申请被拒
		CLProtocol::TeamCopyApplyRefuseNotify notify;
		notify.chiefName = team->GetTeamName();
		for (auto& iter : playerIds)
		{
			team->RemoveApply(iter);
			team->JoinTimeLimit(iter, CURRENT_TIME.Sec() + TEAM_JOIN_TIME_LIMIT);

			TCSPlayer* target = TCSPlayerMgr::Instance()->FindTCSPlayer(iter);
			if (target != NULL)
			{
				target->SendProtocol(notify);
			}
		}
	}

	player->SendProtocol(res);
}

void TCSTeamMgr::HandleStartBattleReq(TCSPlayer* player, UInt32 planModel, std::vector<TeamCopyBattlePlan>& battlePlanVec)
{
	if (NULL == player) return;

	CLProtocol::TeamCopyStartBattleRes res;
	TCSTeam* team = _FindTeam(player->GetID());
	do 
	{
		if (NULL == team)
		{
			ErrorLogStream << PLAYERINFO(player) << "not find team!" << LogStream::eos;
			res.retCode = ErrorCode::TEAM_COPY_NO_EXIST_TEAM;
			break;
		}

		if (TCSApplication::Instance()->IsCloseTeamType(team->GetTeamType()))
		{
			team->BroadcastToMember(0, ErrorCode::TEAM_COPY_CLOSE);
			return;
		}

		DebugLogStream << TEAMINFO(team) << " start battle " << PLAYERINFO(player) << LogStream::eos;

		if (player->GetID() != team->GetChiefId())
		{
			res.retCode = ErrorCode::TEAM_COPY_NO_POWER;
			break;
		}

		// 检查开战条件是否满足
		res.retCode = team->CheckStartBattle();
		if (res.retCode != ErrorCode::SUCCESS)
		{
			break;
		}

		if (team->GetTeamType() == TC_TYPE_ONE)
		{
			// 检查客户端提交的配置
			if (planModel == TEAM_COPY_PLAN_MODEL_GUIDE && !_CheckBattlePlan(battlePlanVec))
			{
				res.retCode = ErrorCode::TEAM_COPY_BATTLE_PLAN;
				break;
			}

			// 设置模式
			team->SetPlanModel(planModel);

			// 设置小队难度
			for (auto& iter : battlePlanVec)
			{
				team->SetSquadGrade(iter.squadId, iter.grade);
			}
		}

		// 通知投票
		team->NotifyPlayerVote();

		res.retCode = ErrorCode::SUCCESS;

	} while (0);

	player->SendProtocol(res);
}

void TCSTeamMgr::HandleStartBattleVote(TCSPlayer* player, UInt32 vote)
{
	if (NULL == player) return;
	
	TCSTeam* team = _FindTeam(player->GetID());
	if (NULL == team)
	{
		return;
	}

	InfoLogStream << TEAMINFO(team) << "player id:" << player->GetID() << " vote:" << vote << LogStream::eos;

	team->HandlerVote(player->GetID(), vote);
}

void TCSTeamMgr::HandleStartChallengeReq(TCSPlayer* player, UInt32 fieldId)
{
	if (NULL == player) return;

	CLProtocol::TeamCopyStartChallengeRes res;
	do 
	{
		TCSTeam* team = _FindTeam(player->GetID());
		if (NULL == team)
		{
			res.retCode = ErrorCode::TEAM_COPY_NO_EXIST_TEAM;
			break;
		}

		// 不是队长，没有权限
		if (!team->StartChallengePower(player->GetID()))
		{
			res.retCode = ErrorCode::TEAM_COPY_NO_POWER;
			break;
		}

		res.retCode = team->StartRace(team->GetPlayerSquadId(player->GetID()), fieldId);

	} while (0);

	if (res.retCode != ErrorCode::SUCCESS)
	{
		player->SendProtocol(res);
	}
}

void TCSTeamMgr::QueryPlayerRaceRes(UInt32 teamSquadId, UInt64 roleId, UInt32 result, DungeonRacePlayerInfo& playerInfo)
{
	TCSTeamPlayerInfo* teamPlayerInfo = _FindTeamPlayerInfo(teamSquadId);
	if (NULL == teamPlayerInfo)
	{
		ErrorLogStream << "not find player info team squad id:" << teamSquadId << " roleId:" << roleId
			<< " result:" << result << LogStream::eos;
		return;
	}

	if (result != ErrorCode::SUCCESS)
	{
		ErrorLogStream << "query player info failed team squad id:" << teamSquadId << " roleId:" << roleId
			<< " result:" << result << LogStream::eos;
		return;
	}

	if (!teamPlayerInfo->IsHasPlayer(roleId))
	{
		ErrorLogStream << "player info not has player failed team squad id:" << teamSquadId << " roleId:" << roleId << LogStream::eos;
		return;
	}

	InfoLogStream << "query player res teamSquadId:" << teamSquadId << " roleId:" << roleId << LogStream::eos;

	teamPlayerInfo->SetDungeonRacePlayerInfo(roleId, playerInfo);

	if (teamPlayerInfo->IsAllHasInfo())
	{
		CLProtocol::DungenServerTeamStartRaceReq req;
		req.teamId = teamSquadId;
		req.dungeonId = teamPlayerInfo->GetDungeonId();
		req.passedAreaIndex = 0;
		teamPlayerInfo->GetPlayerInfos(req.fighters, req.playerInfoes);
		
		// 尾部通关，自爆怪物ID
		if (teamPlayerInfo->GetTailDungeonId())
		{
			req.clearedDungeonIds.push_back(teamPlayerInfo->GetTailDungeonId());
		}

		TCSDungeonMgr::Instance()->SendToDungeonSvr(req);

		InfoLogStream << "send to dungeon server start race teamSquadId:" << teamSquadId
			<< " dungeonId:" << teamPlayerInfo->GetDungeonId() << LogStream::eos;
	}
}

void TCSTeamMgr::CreateTeamPlayerInfo(UInt32 teamSquadId, UInt32 dungeonId, UInt32 fieldId, UInt32 tailDungeonId, std::set<UInt64> playerIds)
{
	TCSTeamPlayerInfo teamPlayerInfo;
	teamPlayerInfo.SetTeamSquadId(teamSquadId);
	teamPlayerInfo.SetDungeonId(dungeonId);
	teamPlayerInfo.SetFieldId(fieldId);
	teamPlayerInfo.SetTailDungeonId(tailDungeonId);
	teamPlayerInfo.SetTimeOut(CURRENT_TIME.Sec() + QUERY_DUNGEON_PLAYER_TIME_OUT); 

	for (auto& id : playerIds)
	{
		teamPlayerInfo.AddTeamPlayer(id);
	}

	m_TeamPlayerInfoMap[teamSquadId] = teamPlayerInfo;
}

void TCSTeamMgr::HandlePlayerListReq(TCSPlayer* player)
{
	if (NULL == player) return;
	
	CLProtocol::TeamCopyFindTeamMateRes res;

	std::vector<TCSPlayer*> playerList;
	TCSPlayerMgr::Instance()->RandTCSPlayer(GetTeamCopyValue(TC_VALUE_PUSH_PLAYER_NUM), playerList, player->GetTeamType());

	for (auto& iter : playerList)
	{
		if (NULL == iter || player->GetID() == iter->GetID()) continue;

		TeamCopyApplyProperty playerPro; 
		playerPro.playerId = iter->GetID();
		playerPro.playerName = iter->GetName();
		playerPro.playerOccu = iter->GetOccu();
		playerPro.playerLevel = iter->GetLevel();
		playerPro.equipScore = iter->GetEquipScore();
		playerPro.isGold = 0;
		playerPro.guildId = iter->GetGuildId();
		playerPro.zoneId = iter->GetZoneID();
		res.playerList.push_back(playerPro);
	}

	player->SendProtocol(res);
}

void TCSTeamMgr::HandleInvitePlayerReq(TCSPlayer* player, std::vector<UInt64>& inviteList)
{
	if (NULL == player || inviteList.empty()) return;

	UInt32 teamId = GetPlayerTeamId(player->GetID());
	TCSTeam* team = _FindTeam(teamId);
	if (NULL == team) return;

	if (team->IsYetStartBattle())
	{
		return;
	}

	for (auto& iter : inviteList)
	{
		// 玩家已有队伍
		if (GetPlayerTeamId(iter) != 0)
			continue;

		TCSPlayer* invitePlayer = TCSPlayerMgr::Instance()->FindTCSPlayer(iter);
		if (NULL == invitePlayer)
			continue;
		
		if (invitePlayer->GetTeamType() != team->GetTeamType())
			continue;

		CLProtocol::TeamCopyInviteNotify inviteNotify;
		invitePlayer->SendProtocol(inviteNotify);

		TeamCopyInviteInfo inviteInfo;
		inviteInfo.teamId = teamId;
		inviteInfo.teamType = team->GetTeamType();
		inviteInfo.teamModel = team->GetModel();
		inviteInfo.teamGrade = team->GetTeamGrade();
		inviteInfo.name = player->GetName();
		inviteInfo.level = player->GetLevel();
		inviteInfo.occu = player->GetOccu();
		inviteInfo.awaken = player->GetAwaken();
		invitePlayer->AddInvite(inviteInfo);
	}
}

void TCSTeamMgr::HandleStageFlopReq(TCSPlayer* player, UInt32 stage)
{
	if (NULL == player) return;
	
	TCSTeam* team = _FindTeam(player->GetID());
	if (team != NULL)
	{
		team->FlopDataRes(player, stage);
	}
}

void TCSTeamMgr::HandleChangeSeatReq(TCSPlayer* player, UInt32 srcSeat, UInt32 destSeat)
{
	CLProtocol::TeamCopyChangeSeatRes res;

	do 
	{
		TCSTeam* team = _FindTeam(player->GetID());
		if (NULL == team)
		{
			res.retCode = ErrorCode::TEAM_COPY_DATA_ERROR;
			break;
		}

		if (team->GetChiefId() != player->GetID())
		{
			res.retCode = ErrorCode::TEAM_COPY_NO_POWER;
			break;
		}

		res.retCode = team->ChangeSeat(srcSeat, destSeat);

	} while (0);

	player->SendProtocol(res);
}

void TCSTeamMgr::HandleKickReq(TCSPlayer* player, UInt64 playerId)
{
	if (NULL == player) return;
	
	TCSTeam* team = _FindTeam(player->GetID());
	if (NULL == team) return;

	CLProtocol::TeamCopyKickRes res;
	res.retCode = ErrorCode::SUCCESS;

	do 
	{
		if (player->GetID() == playerId)
		{
			res.retCode = ErrorCode::TEAM_COPY_DATA_ERROR;
			break;
		}

		if (team->GetChiefId() != player->GetID())
		{
			res.retCode = ErrorCode::TEAM_COPY_NO_POWER;
			break;
		}

		res.retCode = team->KickMember(playerId);
		if (res.retCode != ErrorCode::SUCCESS)
		{
			break;
		}

		_SetPlayerTeamId(playerId, 0);
		team->TeamDataBroadcast();
		team->JoinTimeLimit(playerId, CURRENT_TIME.Sec() + TEAM_JOIN_TIME_LIMIT);

		TCSPlayer* kickPlayer = TCSPlayerMgr::Instance()->FindTCSPlayer(playerId);
		if (kickPlayer != NULL)
		{
			CLProtocol::TeamCopyTeamUpdate update;
			update.opType = 1;
			update.teamId = team->GetTeamId();
			kickPlayer->SendProtocol(update);
		}

	} while (0);

	player->SendProtocol(res);
}

void TCSTeamMgr::HandleAppointmentReq(TCSPlayer* player, UInt64 playerId, UInt32 post)
{
	if (NULL == player) return;

	CLProtocol::TeamCopyAppointmentRes res;
	do
	{
		TCSTeam* team = _FindTeam(player->GetID());
		if (NULL == team)
		{
			res.retCode = ErrorCode::TEAM_COPY_DATA_ERROR;
			break;
		}

		res.retCode = team->AppointmentPost(player->GetID(), playerId, post);

	} while (0);

	player->SendProtocol(res);
}

void TCSTeamMgr::HandleAutoAgreeGoldReq(TCSPlayer* player, UInt32 isAutoAgree)
{
	if (NULL == player) return;

	CLProtocol::TeamCopyAutoAgreeGoldRes res;
	res.isAutoAgree = 0;

	do 
	{
		TCSTeam* team = _FindTeam(player->GetID());
		if (NULL == team)
		{
			res.retCode = ErrorCode::TEAM_COPY_DATA_ERROR;
			break;
		}

		if (team->GetChiefId() != player->GetID())
		{
			res.retCode = ErrorCode::TEAM_COPY_NO_POWER;
			break;
		}

		if (team->GetModel() != TEAM_COPY_TEAM_MODEL_GOLD)
		{
			res.retCode = ErrorCode::TEAM_COPY_NOT_GOLD_TEAM;
			break;
		}

		res.retCode = ErrorCode::SUCCESS;
		res.isAutoAgree = isAutoAgree;
		team->SetAutoAgreeGold(isAutoAgree);

	} while (0);

	player->SendProtocol(res);
}

void TCSTeamMgr::HandleInviteChoiceReq(TCSPlayer* player, UInt32 isAgree, std::vector<UInt32> teamIds)
{
	if (NULL == player || teamIds.empty())
		return;

	CLProtocol::TeamCopyInviteChoiceRes res;
	res.retCode = ErrorCode::SUCCESS;

	do 
	{
		if (GetPlayerTeamId(player->GetID()) != 0)
		{
			res.retCode = ErrorCode::TEAM_COPY_HAS_TEAM;
			break;
		}

		if (isAgree == 0)
		{
			res.teamIds = teamIds;
			for (auto& iter : teamIds)
			{
				player->DelInviteInfo(iter);
			}
		}
		else
		{
			UInt32 id = *(teamIds.begin());
			if ( !player->DelInviteInfo(id ))
			{
				res.retCode = ErrorCode::TEAM_COPY_NO_INVITE;
				break;
			}

			TCSTeam* team = _FindTeam(id);
			if (NULL == team)
			{
				res.retCode = ErrorCode::TEAM_COPY_TEAM_DISMISS;
				break;
			}

			res.retCode = team->JoinTeam(player);
			if (res.retCode != ErrorCode::SUCCESS)
			{
				break;
			}

			_SetPlayerTeamId(player->GetID(), id);
			team->TeamDataBroadcast();
		}
		
	} while (0);

	player->SendProtocol(res);
}

UInt32 GetNotifyId(TCSTeam* team)
{
	if (NULL == team)
		return 0;

	UInt32 notifyId = 0;
	switch (team->GetTeamType())
	{
	case TC_TYPE_ONE:
	{
		if (team->GetTeamGrade() == TEAM_COPY_TEAM_GRADE_COMMON)
		{
			notifyId = 3210;
			if (team->GetModel() == TEAM_COPY_TEAM_MODEL_GOLD)
				notifyId = 3211;
		}
		else if (team->GetTeamGrade() == TEAM_COPY_TEAM_GRADE_DIFF)
		{
			notifyId = 3217;
			if (team->GetModel() == TEAM_COPY_TEAM_MODEL_GOLD)
				notifyId = 3218;
		}
	}
	break;
	case TC_TYPE_TWO:
	{
		if (team->GetTeamGrade() == TEAM_COPY_TEAM_GRADE_COMMON)
		{
			notifyId = 3212;
			if (team->GetModel() == TEAM_COPY_TEAM_MODEL_GOLD)
				notifyId = 3213;
		}
		else if (team->GetTeamGrade() == TEAM_COPY_TEAM_GRADE_DIFF)
		{
			notifyId = 3220;
			if (team->GetModel() == TEAM_COPY_TEAM_MODEL_GOLD)
				notifyId = 3221;
		}
	}
	break;
	default:break;
	}

	return notifyId;
}

void TCSTeamMgr::HandleRecruitReq(TCSPlayer* player)
{
	if (NULL == player) return;

	CLProtocol::TeamCopyRecruitRes res;

	do 
	{
		TCSTeam* team = _FindTeam(player->GetID());
		if (NULL == team)
		{
			res.retCode = ErrorCode::TEAM_COPY_NO_EXIST_TEAM;
			break;
		}

		if (team->IsYetStartBattle())
		{
			res.retCode = ErrorCode::TEAM_COPY_IN_BATTLE;
			break;
		}

		if (team->TeamIsFull())
		{
			res.retCode = ErrorCode::TEAM_COPY_TEAM_FULL;
			break;
		}

		UInt32 curTime = (UInt32)CURRENT_TIME.Sec();
		if (team->GetRecruitCD() + GetTeamCopyValue(TC_VALUE_RECRUIT_CD) > curTime)
		{
			res.retCode = ErrorCode::TEAM_COPY_RECRUIT_CD;
			break;
		}

		team->SetRecruitCD(curTime);
		res.retCode = ErrorCode::SUCCESS;
		
		UInt32 notifyId = GetNotifyId(team);
		if (notifyId == 0)
			return;
		
		std::string systemLinkStr;
		MsgPackage::GetSystemSuffixesMsgTag(systemLinkStr, SYSTEM_SUFFIXES_TYPE_TEAM_COPY, team->GetTeamId());
		std::string content = SysNotifyMgr::Instance()->MakeString(notifyId, systemLinkStr);

		CLProtocol::SceneSyncChat protocol;
		protocol.channel = CHAT_CHANNEL_TEAM_COPY_PREPARE;
		protocol.objid = player->GetID();
		protocol.objname = player->GetName();
		protocol.occu = player->GetOccu();
		protocol.level = player->GetLevel();
		protocol.word = content;
		protocol.bLink = true;
		protocol.isGm = 0;
		protocol.voiceKey = "";
		protocol.voiceDuration = 0;
		protocol.headFrame = 0;
		TCSceneMgr::Instance()->BroadcastProtocol(protocol, team->GetTeamType());

	} while (0);

	player->SendProtocol(res);
}

void TCSTeamMgr::HandleLinkJoinReq(TCSPlayer* player, UInt32 teamId, UInt32 isGold)
{
	if (NULL == player) return;
	
	CLProtocol::TeamCopyLinkJoinRes res;
	do 
	{
		if (GetPlayerTeamId(player->GetID()) != 0)
		{
			res.retCode = ErrorCode::TEAM_COPY_HAS_TEAM;
			break;
		}

		TCSTeam* team = _FindTeam(teamId);
		if (NULL == team)
		{
			res.retCode = ErrorCode::TEAM_COPY_NO_EXIST_TEAM;
			break;
		}

		res.retCode = team->JoinTeam(player, isGold);
		if (res.retCode != ErrorCode::SUCCESS)
		{
			break;
		}

		_SetPlayerTeamId(player->GetID(), teamId);
		team->TeamDataBroadcast();

		// 把玩家从申请列表删掉
		team->RemoveApply(player->GetID());

	} while (0);

	player->SendProtocol(res);
}

void TCSTeamMgr::HandleCheckCondRes(UInt32 retCode, UInt32 teamId, UInt64 roldId, UInt64 commissionOrder, UInt32 connId, 
	const TCConsumeItem& commission, const std::vector<TCConsumeItem>& ticket)
{
	TCSTeam* team = _FindTeam(teamId);
	if (NULL == team)
	{
		ErrorLogStream << "not find teamId:" << teamId << " roleId:" << roldId << " code:" << retCode << LogStream::eos;
		return;
	}

	// 有人检查失败，通知所有人，开战失败
	if (retCode != TEAM_COPY_CHECK_CODE_SUCCESS)
	{
		ErrorLogStream << "player check failed! teamId:" << teamId << " roleId:" << roldId << " code:" << retCode << LogStream::eos;

		team->SetConsumeFlag(true);

		// 返还成功扣除消耗
		team->MemberReturnConsume();

		CLProtocol::TeamCopyStartBattleRes res;
		res.retCode = GetCheckErrCode(retCode);

		TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(roldId);
		if (player != NULL)
			res.roleName = player->GetName();

		team->BroadcastToMember(res);

		CLProtocol::TeamCopyVoteNotify voteF;
		voteF.roleId = roldId;
		voteF.vote = 0;
		team->BroadcastToMember(voteF);

		team->RemoveVote(roldId);
		team->SetCheckCondEndTime(0);
		return;
	}

	TCSTeamCheck* check = GetTeamCheck(teamId);
	if (NULL == check)
	{
		ErrorLogStream << "team check not find teamId:" << teamId << " roleId:" << roldId << " code:" << retCode << LogStream::eos;
		return;
	}

	check->RemoveCheck(roldId);


	// 增加玩家的挑战次数，在创建和加入队伍时会检查次数
	TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(roldId);
	if (player != NULL)
	{
		player->AddChallengeNum(team->GetTeamType(), team->GetTeamGrade());
		player->CalNotFlopStage(team->GetTeamType(), team->GetTeamGrade());

		InfoLogStream << "player check success" << TEAMINFO(team) << " role " << PLAYERINFO(player) << LogStream::eos;
	}
	else
	{
		ErrorLogStream << TEAMINFO(team) << "not find player id:" << roldId << LogStream::eos;
	}

	// 消耗门票存库
	for (auto& it : ticket)
	{
		team->SaveConsume(roldId, it.id, it.num, TEAM_COPY_CONSUME_TYPE_TICKET);
	}

	if (commissionOrder != 0)
	{
		// 消耗佣金存库
		team->SaveConsume(roldId, commission.id, commission.num, TEAM_COPY_CONSUME_TYPE_COMMISSION);

		// 保存玩家佣金的订单号
		team->AddCommissionOrder(roldId, connId, commissionOrder);
	}

	// 有人扣消耗失败了，立马返还
	if (team->GetConsumeFlag())
	{
		team->MemberReturnConsume(roldId);
	}

	if (check->IsAllCheck())
	{
		DelTeamCheck(teamId);
		team->SetCheckCondEndTime(0);
		team->CheckCondFinish();
	}
}

void TCSTeamMgr::HandlerStartRaceRes(CLProtocol::DungenServerTeamStartRaceRes& res)
{
	UInt32 teamId = GET_TEAM_ID(res.teamId);
	TCSTeam* team = _FindTeam(teamId);
	if (NULL == team)
	{
		ErrorLogStream << "not find team id:" << teamId << LogStream::eos;
		return;
	}

	team->StartRaceRes(res);
	_TeamPlayerInfoEnd(res.teamId);
}

void TCSTeamMgr::HandlerRaceEnd(CLProtocol::DungeonSvrDungeonRaceEnd& end)
{
	UInt64 raceId = end.endInfo.gamesessionId;
	TCSDungeonRace* dungeonRace = TCSDungeonMgr::Instance()->FindRaceByRaceId(raceId);
	if (NULL == dungeonRace)
	{
		ErrorLogStream << "not find race:" << raceId << " team id:" << end.teamId << LogStream::eos;
		return;
	}

	UInt32 fieldId = dungeonRace->GetFieldId();
	UInt64 createTime = dungeonRace->GetCreateTime();

	// 比赛结束删除比赛
	TCSDungeonMgr::Instance()->RemoveRace(raceId);

	UInt32 teamId = GET_TEAM_ID(end.teamId);
	TCSTeam* team = _FindTeam(teamId);
	if (NULL == team)
	{
		ErrorLogStream << "not find team id:" << teamId << LogStream::eos;
		return;
	}

	UInt32 dungeonId = 0;
	TeamCopyFieldDataEntry* fieldData = TeamCopyFieldDataEntryMgr::Instance()->FindEntry(fieldId);
	if (fieldData != NULL)
	{
		dungeonId = fieldData->dungeonId;
	}

	team->RaceEnd(end, fieldId, dungeonId, createTime);
}

void TCSTeamMgr::HandlerPhaseBoss(UInt64 roleId, UInt32 phase, UInt32 curFrame, UInt32 bossBloodRate)
{
	TCSTeam* team = _FindTeam(roleId);
	if (NULL == team)
	{
		ErrorLogStream << "not find team roleId:" << roleId << LogStream::eos;
		return;
	}

	team->PhaseBossInfo(roleId, phase, curFrame, bossBloodRate);
}

void TCSTeamMgr::HandleForceEndReq(TCSPlayer* player)
{
	if (NULL == player) return;

	CLProtocol::TeamCopyForceEndRes res;
	do 
	{
		TCSTeam* team = _FindTeam(player->GetID());
		if (NULL == team)
		{
			res.retCode = ErrorCode::TEAM_COPY_NO_EXIST_TEAM;
			break;
		}

		if (team->GetChiefId() != player->GetID())
		{
			res.retCode = ErrorCode::TEAM_COPY_NO_POWER;
			break;
		}

		if ( !team->GetForceEndFlag())
		{
			res.retCode = ErrorCode::TEAM_COPY_FORCE_END_FAILED;
			break;
		}

		if ((UInt32)CURRENT_TIME.Sec() < team->GetForceEndCoolTime() + GetTeamCopyValue(TC_VALUE_FORCE_END_COOL_TIME))
		{
			res.retCode = ErrorCode::TEAM_COPY_FORCE_END_CD;
			break;
		}

		// 设置冷却时间
		team->SetForceEndCoolTime((UInt32)CURRENT_TIME.Sec());

		res.retCode = ErrorCode::SUCCESS;

		// 通知队伍成员发起投票
		UInt32 voteDurationTime = GetTeamCopyValue(TC_VALUE_FORCE_END_VOTE_TIME);
		UInt32 voteEndTime = UInt32(CURRENT_TIME.Sec()) + voteDurationTime;

		// 设置投票结束时间
		team->SetForceEndVoteTime(voteEndTime);

		CLProtocol::TeamCopyForceEndVoteNotify notify;
		notify.voteDurationTime = voteDurationTime;
		notify.voteEndTime = voteEndTime;
		team->BroadcastToMember(notify);

		// 团长自动同意
		team->ForceEndVote(player, 1);

	} while (0);
	
	player->SendProtocol(res);
}

void TCSTeamMgr::HandleForceEndVoteReq(TCSPlayer* player, UInt32 vote)
{
	if (NULL == player) return;
	
	TCSTeam* team = _FindTeam(player->GetID());
	if(NULL == team)
	{
		return;
	}

	team->ForceEndVote(player, vote);
}

void TCSTeamMgr::HandleModifyEquipScore(TCSPlayer* player, UInt32 equipScore)
{
	if (NULL == player) return;

	CLProtocol::TeamCopyModifyEquipScoreRes res;

	do 
	{
		TCSTeam* team = _FindTeam(player->GetID());
		if (NULL == team)
		{
			res.retCode = ErrorCode::TEAM_COPY_NO_EXIST_TEAM;
			break;
		}

		if (team->GetChiefId() != player->GetID())
		{
			res.retCode = ErrorCode::TEAM_COPY_NO_POWER;
			break;
		}

		UInt32 oldEquipScore = team->GetEquipScore();
		if (oldEquipScore == equipScore)
		{
			res.retCode = ErrorCode::SUCCESS;
			break;
		}

		team->SetEquipScore(equipScore);
		team->TeamDataBroadcast();
		res.retCode = ErrorCode::SUCCESS;

		// 增加装备评分，清除评分低的玩家
		if (equipScore > oldEquipScore)
		{
			team->ClearEquipScoreLowPlayer();
		}

		if (team->ApplyNum() == 0)
		{
			CLProtocol::TeamCopyApplyNotify notify;
			notify.IsHasApply = 0;
			player->SendProtocol(notify);
		}

	} while (0);

	player->SendProtocol(res);
}

void TCSTeamMgr::HandleSquadMoveGrid(TCSPlayer* player, UInt32 targetGridId, UInt32 targetObjId)
{
	if (NULL == player) return;

	TCSTeam* team = _FindTeam(player->GetID());
	if (NULL == team)
	{
		return;
	}

	team->MoveGrid(player, targetGridId, targetObjId);
}

void TCSTeamMgr::HandleRollReward(TCSPlayer* player, UInt32 isNeed)
{
	if (NULL == player) return;

	TCSTeam* team = _FindTeam(player->GetID());
	if (NULL == team)
	{
		return;
	}

	team->RollRewardPoint(player, isNeed);
}

void TCSTeamMgr::BossPhaseAddField(UInt32 teamSquadId, UInt32 fieldId)
{
	//UInt32 teamId = GET_TEAM_ID(teamSquadId);
	//TCSTeam* team = _FindTeam(teamId);
	//if (NULL == team)
	//{
	//	ErrorLogStream << "not find team id:" << teamId << LogStream::eos;
	//	return;
	//}

	//team->AddField(TeamCopyFieldDataEntryMgr::Instance()->FindEntry(fieldId));
	//team->FieldNotify(fieldId);
}

void TCSTeamMgr::BossPhaseDelField(UInt32 teamSquadId, UInt32 fieldId)
{
	//UInt32 teamId = GET_TEAM_ID(teamSquadId);
	//TCSTeam* team = _FindTeam(teamId);
	//if (NULL == team)
	//{
	//	ErrorLogStream << "not find team id:" << teamId << LogStream::eos;
	//	return;
	//}

	//// 立即结束上一个据点的比赛,将据点状态设为歼灭
	//team->FieldRaceEnd(fieldId);
	//team->DefeatField(fieldId);
	//team->FieldNotify(fieldId);
}

void TCSTeamMgr::OnPlayerQuit(UInt64 playerId)
{
	TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(playerId);
	_QuitTeam(playerId, player);
}

void TCSTeamMgr::TicketUpdate(UInt64 playerId, UInt32 isEnough)
{
	TCSTeam* team = _FindTeam(playerId);
	if (NULL == team || team->IsYetStartBattle())
	{
		return;
	}

	team->TicketUpdate(playerId, isEnough);
}

UInt32 TCSTeamMgr::GetPlayerTeamId(UInt64 playerId)
{
	return _GetPlayerTeamId(playerId);
}

void TCSTeamMgr::SetPlayerTeamId(UInt64 playerId, UInt32 teamId)
{
	return _SetPlayerTeamId(playerId, teamId);
}

UInt32 TCSTeamMgr::GetPlayerTeamSquadId(UInt64 playerId)
{
	TCSTeam* team = _FindTeam(playerId);
	if (NULL == team)
	{
		return 0;
	}

	return GET_TEAM_SQUAD_ID(team->GetTeamId(), team->GetPlayerSquadId(playerId));
}

bool TCSTeamMgr::IsReconnect(UInt32 teamId)
{
	TCSTeam* team = _FindTeam(teamId);
	if (NULL == team)
		return false;
	
	switch (team->GetStatus())
	{
	case TEAM_COPY_TEAM_STATUS_PARPARE : return false;
	case TEAM_COPY_TEAM_STATUS_BATTLE : return true;
	default:
		break;
	}
	
	return false;
}

void TCSTeamMgr::SyncAllObjectToPlayer(TCSPlayer* player)
{
	if (NULL == player) return;
	
	TCSTeam* team = _FindTeam(player->GetID());
	if (NULL == team)
	{
		return;
	}

	for (auto& iter : team->GetMemberMap())
	{
		if (iter.first == player->GetID())
		{
			continue;
		}

		TCSPlayer* syncPlayer = TCSPlayerMgr::Instance()->FindTCSPlayer(iter.first);
		if (NULL == syncPlayer)
			continue;

		if (syncPlayer->GetSceneSubType() != SCENE_SUB_TEAM_COPY_ATTACK)
			continue;

		//DebugLogStream << "team sync playerId:" << player->GetID() << " targetId:" << iter.first << LogStream::eos;

		player->SendPacket(TCSceneMgr::Instance()->GetPlayerSyncPacket(iter.first));
	}
}

void TCSTeamMgr::GMClearAllField(UInt64 playerId)
{
	TCSTeam* team = _FindTeam(playerId);
	if (NULL == team) return;

	UInt32 squadId = team->GetPlayerSquadId(playerId);
	team->GMClearAllField(squadId);

	DebugLogStream << TEAMINFO(team) << "player id:" << playerId << " gm clear all field!" << LogStream::eos;
}

void TCSTeamMgr::GMClearField(UInt64 playerId, UInt32 fieldId)
{
	TCSTeam* team = _FindTeam(playerId);
	if (NULL == team) return;

	UInt32 squadId = team->GetPlayerSquadId(playerId);

	// 处理目标
	team->ClearField(squadId, fieldId);

	DebugLogStream << TEAMINFO(team) << "player id:" << playerId << " gm clear field id:" << fieldId << LogStream::eos;
}

void TCSTeamMgr::GMClearSquadStatus(UInt64 playerId)
{
	TCSTeam* team = _FindTeam(playerId);
	if (NULL == team) return;

	UInt32 squadId = team->GetPlayerSquadId(playerId);
	team->SetSquadStatus(squadId, TEAM_COPY_SQUAD_STATUS_PREPARE);
	team->SquadNotify(squadId, TEAM_COPY_SQUAD_STATUS_PREPARE);
}

void TCSTeamMgr::GMTeamCopyFailed(UInt64 playerId)
{
	TCSTeam* team = _FindTeam(playerId);
	if (NULL == team) 
		return;

	if ( !team->IsInBattle())
		return;

	UInt32 curTime = (UInt32)CURRENT_TIME.Sec();

	// 3秒后结束
	team->SetGameOverTime(curTime + 3);
}

void TCSTeamMgr::GMEnergyReviveRate(UInt64 playerId, UInt32 rate)
{
	TCSTeam* team = _FindTeam(playerId);
	if (NULL == team)
		return;

	team->GMUpdateEnergyField(rate);
}

void TCSTeamMgr::GMSetSquadPro(UInt64 playerId, UInt32 grid, UInt32 cd)
{
	TCSTeam* team = _FindTeam(playerId);
	if (NULL == team)
		return;

	team->SetSquadPro(playerId, grid, cd);
}

void TCSTeamMgr::GMClearGridObj(UInt64 playerId, UInt32 gridType)
{
	TCSTeam* team = _FindTeam(playerId);
	if (NULL == team)
		return;

	team->GMClearGridObj(playerId, gridType);
}

TCSTeamCheck* TCSTeamMgr::GetTeamCheck(UInt32 teamId)
{
	auto iter = m_TeamCheck.find(teamId);
	return iter != m_TeamCheck.end() ? &(iter->second) : NULL;
}

void TCSTeamMgr::DelTeamCheck(UInt32 teamId)
{
	auto iter = m_TeamCheck.find(teamId);
	if (iter == m_TeamCheck.end())
	{
		return;
	}

	m_TeamCheck.erase(teamId);
}

void TCSTeamMgr::AddTeamCheck(UInt32 teamId, UInt64 playerId)
{
	auto iter = m_TeamCheck.find(teamId);
	if (iter == m_TeamCheck.end())
	{
		TCSTeamCheck teamCheck;
		teamCheck.SetTeamId(teamId);
		teamCheck.JoinCheck(playerId);
		m_TeamCheck[teamId] = teamCheck;
	}
	else
	{
		iter->second.JoinCheck(playerId);
	}
}

void TCSTeamMgr::PlayerExpire(UInt64 playerId, UInt64 expireTime)
{
	TCSTeam* team = _FindTeam(playerId);
	if (NULL == team)
	{
		return;
	}

	team->MemberExpire(playerId, expireTime);
}

UInt32 TCSTeamMgr::GetAttackFieldId(UInt32 teamSquadId)
{
	TCSTeamPlayerInfo* teamPlayerInfo = _FindTeamPlayerInfo(teamSquadId);
	return teamPlayerInfo != NULL ? teamPlayerInfo->GetFieldId() : 0;
}

void TCSTeamMgr::BroadcastToTeam(Avalon::Protocol& protocol, UInt32 teamId, UInt64 exceptPlayer)
{
	TCSTeam* team = _FindTeam(teamId);
	if (NULL == team)
	{
		return;
	}

	team->BroadcastToExceptMember(protocol, exceptPlayer);
}

void TCSTeamMgr::BroadcastToTeam(Avalon::Packet* packet, UInt32 teamId, UInt64 exceptPlayer)
{
	TCSTeam* team = _FindTeam(teamId);
	if (NULL == team)
	{
		return;
	}

	team->BroadcastToExceptMember(*packet, exceptPlayer);
}

void TCSTeamMgr::BroadcastToSquad(Avalon::Protocol& protocol, UInt32 teamId, UInt64 playerId)
{
	TCSTeam* team = _FindTeam(teamId);
	if (NULL == team)
	{
		return;
	}

	UInt32 squadId = team->GetPlayerSquadId(playerId);
	team->BroadcastToMember(protocol, squadId);
}

void TCSTeamMgr::BroadcastToSquad(Avalon::Packet* packet, UInt32 teamId, UInt64 playerId)
{
	TCSTeam* team = _FindTeam(teamId);
	if (NULL == team)
	{
		return;
	}

	UInt32 squadId = team->GetPlayerSquadId(playerId);
	team->BroadcastToMember(*packet, squadId);
}

TCSTeam* TCSTeamMgr::_FindTeam(UInt32 teamId)
{
	auto iter = m_TeamMap.find(teamId);
	return iter != m_TeamMap.end() ? iter->second : NULL;
}

TCSTeam* TCSTeamMgr::_FindTeam(UInt64 playerId)
{
	return _FindTeam(_GetPlayerTeamId(playerId));
}

void TCSTeamMgr::_AddTeamMap(TCSTeam* team)
{
	if (!m_TeamMap.insert(std::make_pair(team->GetTeamId(), team)).second)
	{
		ErrorLogStream << "repeated create team chief id:"<< team->GetChiefId() << LogStream::eos;
	}
}

UInt32 TCSTeamMgr::_QuitTeam(UInt64 playerId, TCSPlayer* player)
{
	TCSTeam* team = _FindTeam(playerId);
	if (NULL == team)
	{
		return ErrorCode::SUCCESS;
	}

	if (player != NULL && team->IsInBattle())
	{
		// 返还次数
		if (player->IsOddFreeQuitTeamNum(team->GetTeamType()))
		{
			player->ReturnChallengeNum(team->GetTeamType(), team->GetTeamGrade());
			player->AddQuitTeamNum(team->GetTeamType());
		}
	}

	_SetPlayerTeamId(playerId, 0);
	team->QuitTeam(playerId);

	if (team->GetTeamMemberNum() == 0)
	{
		InfoLogStream << TEAMINFO(team) << "team dismiss !" << LogStream::eos;
		UInt32 oldTeamId = team->GetTeamId();
		delete team;
		m_TeamMap.erase(oldTeamId);
	}
	else
	{
		team->TeamDataBroadcast();
	}

	return ErrorCode::SUCCESS;
}

UInt32 TCSTeamMgr::_GetPlayerTeamId(UInt64 playerId)
{
	auto iter = m_PlayerTeamIdMap.find(playerId);
	return iter != m_PlayerTeamIdMap.end() ? iter->second : 0;
}

void TCSTeamMgr::_SetPlayerTeamId(UInt64 playerId, UInt32 teamId)
{
	if (playerId == 0)
	{
		return;
	}

	// 如果队伍是0，则删除玩家
	if (teamId == 0)
	{
		m_PlayerTeamIdMap.erase(playerId);
		return;
	}

	m_PlayerTeamIdMap[playerId] = teamId;
}

bool TCSTeamMgr::_CheckBattlePlan(const std::vector<TeamCopyBattlePlan>& battlePlanVec)
{
	UInt32 squadNum = GetTeamCopyValue(TC_VALUE_SQUAD_NUM);
	if (battlePlanVec.size() != squadNum)
	{
		return false;
	}

	std::set<UInt32> gradeSet;
	std::set<UInt32> squadSet;
	for (auto& iter : battlePlanVec)
	{
		gradeSet.insert(iter.grade);
		squadSet.insert(iter.squadId);
	}

	if (gradeSet.size() != squadSet.size() || squadSet.size() != squadNum)
	{
		return false;
	}

	return true;
}

TCSTeamPlayerInfo* TCSTeamMgr::_FindTeamPlayerInfo(UInt32 teamSquadId)
{
	auto iter = m_TeamPlayerInfoMap.find(teamSquadId);
	return iter != m_TeamPlayerInfoMap.end() ? &(iter->second) : NULL;
}

void TCSTeamMgr::_TeamPlayerInfoEnd(UInt32 teamSquadId)
{
	m_TeamPlayerInfoMap.erase(teamSquadId);
}

TCSTeam* TCSTeamMgr::_CreateTeam(UInt32 tcType)
{
	switch (tcType)
	{
	case TC_TYPE_ONE: return new TCSTeamOne();
	case TC_TYPE_TWO: return new TCSTeamTwo();
	default:
		break;
	}

	return NULL;
}