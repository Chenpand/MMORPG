#include "LostDungeonMgr.h"
#include "LostDungeonTeam.h"
#include "PlayerMgr.h"
#include "DungeonEntrySceneDynamic.h"
#include "LostBattleScene.h"
#include "SSRouter.h"
#include "Player.h"
#include <CLLostDungeonProtocol.h>

LostDungeonMgr::LostDungeonMgr()
{
	m_TeamIdSerial = 0;
}

LostDungeonMgr::~LostDungeonMgr()
{

}

void LostDungeonMgr::CreateTeamsOfScene(UInt32 sceneId)
{
	auto it = m_teams.find(sceneId);
	if (it != m_teams.end()) return;

	TeamVec teamVec;
	m_teams.insert(std::pair<UInt32, TeamVec>(sceneId, teamVec));
	TeamVec& teamVec_ = m_teams[sceneId];
	for (UInt32 i = 1; i <= MAX_TEAM_SIZE; ++i)
	{
		while (++m_TeamIdSerial == 0 || m_idTeams.find(m_TeamIdSerial) != m_idTeams.end())
			++m_TeamIdSerial;
		LostDungeonTeam* team = new LostDungeonTeam(m_TeamIdSerial);
		team->SetSceneId(sceneId);
		team->SetIndex(i);
		teamVec_.push_back(team);
		m_idTeams[m_TeamIdSerial] = team;
	}
}

bool LostDungeonMgr::IsSceneHaveEmptyTeam(UInt32 sceneId)
{
	auto it = m_teams.find(sceneId);
	if (it == m_teams.end())
	{
		return false;
	}

	TeamVec& teamVec = it->second;
	for (auto team : teamVec)
	{
		if (team->GetNum() == 0)
		{
			return true;
		}
	}
	return false;
}

LostDungeonTeam*  LostDungeonMgr::FindTeamById(UInt32 teamId)
{
	auto it = m_idTeams.find(teamId);
	if (it == m_idTeams.end())
	{
		return NULL;
	}
	return it->second;
}

LostDungeonTeam* LostDungeonMgr::GetFirstEmptyTeam(UInt32 sceneId)
{
	auto it = m_teams.find(sceneId);
	if (it == m_teams.end())
	{
		return NULL;
	}
	TeamVec& teamVec = it->second;
	for (auto team : teamVec)
	{
		if (team->GetNum() == 0)
		{
			return team;
		}
	}
	return NULL;
}

LostDungeonTeam* LostDungeonMgr::GetFirstHavePosTeam(UInt32 sceneId)
{
	auto it = m_teams.find(sceneId);
	if (it == m_teams.end())
	{
		return NULL;
	}

	TeamVec teamVec = it->second;

	std::sort(teamVec.begin(), teamVec.end(), [](const LostDungeonTeam* a, const LostDungeonTeam* b)
	{
		if (a->GetNum() != b->GetNum())
		{
			return a->GetNum() > b->GetNum();
		}

		return a->GetIndex() < b->GetIndex();
	});

	for (auto team : teamVec)
	{
		if (team->GetChallengeMode() != LDCM_TEAM)
		{
			continue;
		}
		if (team->GetNum() == LostDungeonTeam::MAX_MEMBER_SIZE)
		{
			continue;
		}
		if (team->GetBattleState() != LDTBS_NORMAL)
		{
			continue;
		}
		return team;
	}
	return NULL;
}

LostDungeonTeam* LostDungeonMgr::GetPlayerTeam(UInt32 sceneId, UInt64 playerId)
{
	LostDungeonTeam* dungeon_team = NULL;
	VisitTeamMember(sceneId, [&](LostDungeonTeam* team, LostDungTeamMember* teamMember) {
		if (!team || !teamMember) return true;
		if (teamMember->roleId == playerId)
		{
			dungeon_team = team;
			return false;
		}
		return true;
	}
	);
	return dungeon_team;
}

void LostDungeonMgr::OnPlayerEnterDungeonEntry(Player* player, DungeonEntrySceneDynamic* scene)
{
	if (!player || !scene)
	{
		return;
	}

	player->SetDungChageMode(LDCM_SINGLE);
	UInt64 sceneId = scene->GetID();
	if (!IsSceneHaveEmptyTeam(sceneId))
	{
		return;
	}

	LostDungeonTeam* team = GetFirstEmptyTeam(sceneId);
	if (!team)
	{
		return;
	}
	LostDungeonTeam::PlayerMapMember members;
	team->AddMember(player, members);
	team->SetChallengeMode(LDCM_SINGLE);
	//同步队伍信息
	CLProtocol::SceneLostDungeonSyncDungeonTeamInfo notify;
	notify.type = SDETIT_ENTER_SYNC;
	notify.challengeMode = player->GetDungChageMode();

	VistTeams(sceneId, [&](LostDungeonTeam* team) {
		LostDungTeamInfo teamInfo;
		team->GetInfo(teamInfo);
		notify.teamInfos.push_back(teamInfo);
		return true;
	});

	VisitTeamMember(sceneId, [&](LostDungeonTeam* team, LostDungTeamMember* teamMember) {
		if (!team || !teamMember) return true;
		notify.teamMemebers.push_back(*teamMember);
		return true;
	});
	player->SendProtocol(notify);
}

void LostDungeonMgr::OnPlayerLeaveDungeonEntry(Player* player, DungeonEntrySceneDynamic* scene)
{
	if (!player || !scene)
	{
		return;
	}
	player->SetDungChageMode(LDCM_SINGLE);
	player->SetDungChageTeamId(0);
	UInt32 sceneId = scene->GetID();
	UInt64 playerId = player->GetID();

	LostDungeonTeam* team = GetPlayerTeam(sceneId,  playerId);
	if (team)
	{
		LostDungeonTeam::PlayerMapMember members;
		team->RemoveMember(playerId, members, false);

		if (team->GetNum() == 0)
		{
			bool update = team->SetBattleState(LDTBS_NORMAL);
			if (update)
			{
				CLProtocol::SceneLostDungeonSyncDungeonTeamInfo syncTeam;
				syncTeam.type = SDETIT_TEAM_STATE;
				LostDungTeamInfo teamInfo;
				team->GetInfo(teamInfo);
				syncTeam.teamInfos.push_back(teamInfo);

				BroadcastMsgToTeamMembersInScene(team->GetSceneId(), syncTeam);
			}
		}
	}
	
}

UInt32 LostDungeonMgr::OnPlayerSwitchChallengeMode(Player* player, UInt8 mode)
{
	if (!player) return ErrorCode::ITEM_INVALID_INDEX;
	if (mode != LDCM_SINGLE && mode != LDCM_TEAM)
	{
		return ErrorCode::ITEM_INVALID_INDEX;
	}

	if (!player->GetScene() || player->GetScene()->GetType() != SCENE_TYPE_DUNGEON_ENTRY_DYNAMIC)
	{
		return ErrorCode::ITEM_INVALID_INDEX;
	}

	UInt64 playerId = player->GetID();
	UInt32 sceneId = player->GetScene()->GetID();
	if (mode == LDCM_SINGLE)
	{
		if (!IsSceneHaveEmptyTeam(sceneId))
		{
			return ErrorCode::LOSTDUNGEON_SWITCH_SINGLE_MODE_FAIL;
		}
		
		LostDungeonTeam* old_team = GetPlayerTeam(sceneId, player->GetID());
		if (!old_team || old_team->GetChallengeMode() != LDCM_TEAM)
		{
			return ErrorCode::LOSTDUNGEON_SWITCH_MODE_FAIL;
		}

		if (old_team->GetNum() == 1)
		{
			CLProtocol::SceneLostDungeonSyncDungeonTeamInfo notify;
			notify.type = SDETIT_CHG_MODE;
			notify.challengeMode = LDCM_SINGLE;
			player->SendProtocol(notify);
			old_team->SetChallengeMode(LDCM_SINGLE);
			player->SetDungChageMode(LDCM_SINGLE);
			return ErrorCode::SUCCESS;
		}

		LostDungeonTeam* empty_team = GetFirstEmptyTeam(player->GetScene()->GetID());
		if (!empty_team)
		{
			return ErrorCode::LOSTDUNGEON_SWITCH_MODE_FAIL;
		}
		
		//从旧的队伍中移除掉
		LostDungeonTeam::PlayerMapMember members;

		old_team->RemoveMember(playerId, members, true);
		empty_team->AddMember(player, members, true, false);
		empty_team->SetChallengeMode(LDCM_SINGLE);
		
		if (members.size() > 0)
		{
			CLProtocol::SceneLostDungeonSyncDungeonTeamInfo notify;
			notify.type = SDETIT_UPDATE;
			for (auto it : members)
			{
				notify.teamMemebers.push_back(it.second);
			}
			LostDungeonMgr::Instance()->BroadcastMsgToTeamMembersInScene(sceneId, notify);
		}

		CLProtocol::SceneLostDungeonSyncDungeonTeamInfo notify;
		notify.type = SDETIT_CHG_MODE;
		notify.challengeMode = LDCM_SINGLE;
		player->SendProtocol(notify);

		player->SetDungChageMode(LDCM_SINGLE);
		return ErrorCode::SUCCESS;
	}
	else if (mode == LDCM_TEAM)
	{
		LostDungeonTeam* old_team = GetPlayerTeam(sceneId, player->GetID());
		if (!old_team || old_team->GetChallengeMode() != LDCM_SINGLE)
		{
			return ErrorCode::LOSTDUNGEON_SWITCH_FAIL_NOTEAM;
		}

		LostDungeonTeam* new_team = GetFirstHavePosTeam(sceneId);
		if (!new_team || new_team->GetId() == old_team->GetId())
		{
			old_team->SetChallengeMode(LDCM_TEAM);
			CLProtocol::SceneLostDungeonSyncDungeonTeamInfo notify;
			notify.type = SDETIT_CHG_MODE;
			notify.challengeMode = LDCM_TEAM;
			player->SendProtocol(notify);
			player->SetDungChageMode(LDCM_TEAM);
			return ErrorCode::SUCCESS;
		}

		LostDungeonTeam::PlayerMapMember members;

		old_team->RemoveMember(playerId, members, true);
		old_team->SetChallengeMode(LDCM_SINGLE);
		new_team->AddMember(player, members, true, false);
		
		if (members.size() > 0)
		{
			CLProtocol::SceneLostDungeonSyncDungeonTeamInfo notify;
			notify.type = SDETIT_UPDATE;
			for (auto it : members)
			{
				notify.teamMemebers.push_back(it.second);
			}
			LostDungeonMgr::Instance()->BroadcastMsgToTeamMembersInScene(sceneId, notify);
		}

		CLProtocol::SceneLostDungeonSyncDungeonTeamInfo notify;
		notify.type = SDETIT_CHG_MODE;
		notify.challengeMode = LDCM_TEAM;
		player->SendProtocol(notify);

		player->SetDungChageMode(LDCM_TEAM);
		return ErrorCode::SUCCESS;
	}
	return ErrorCode::ITEM_INVALID_INDEX;
}

void LostDungeonMgr::VistTeams(UInt32 sceneId, const std::function<bool(LostDungeonTeam* team)>& ff)
{
	auto it = m_teams.find(sceneId);
	if (it == m_teams.end())	return;

	TeamVec& teamVec = it->second;
	for (auto team : teamVec)
	{
		if (!ff(team))
		{
			break;
		}
	}
}

void LostDungeonMgr::VisitTeamMember(UInt32 sceneId, const std::function<bool(LostDungeonTeam* team, LostDungTeamMember* member)>& ff)
{
	auto it = m_teams.find(sceneId);
	if (it == m_teams.end())	return;

	TeamVec& teamVec = it->second;
	for (auto team : teamVec)
	{
		if (!team->VistMembers(ff))
		{
			break;
		}
	}
}

void LostDungeonMgr::BroadcastMsgToTeamMembersInScene(UInt32 sceneId, Avalon::Protocol& protocol, UInt64 exceptId)
{
	VisitTeamMember(sceneId, 
		[&](LostDungeonTeam* team, LostDungTeamMember* teamMember) {
		if (!team || !teamMember)	return true;
			if (exceptId > 0 && teamMember->roleId == exceptId)
			{
				return true;
			}
			Player* player = PlayerMgr::Instance()->FindPlayer(teamMember->roleId);
			if (!player)
			{
				return true;
			}
			player->SendProtocol(protocol);
			return true;
		}
		);
}

UInt32 LostDungeonMgr::OnPlayerPkReq(Player* attacker, Player* attackeder, UInt32	dungeonId)
{
	if (!attacker || !attackeder)
	{
		return ErrorCode::ITEM_INVALID_INDEX;
	}

	LostBattleScene* lostScene = dynamic_cast<LostBattleScene*>(attacker->GetScene());
	if (!lostScene)
	{
		return ErrorCode::ITEM_INVALID_INDEX;
	}

	if (lostScene->FindPlayer(attackeder->GetID()) == NULL)
	{
		return ErrorCode::LOSTDUNGEON_PK_FAIL_TARGT_LEAVING;
	}

	if (attackeder->GetGameStatus() != PS_NORMAL)
	{
		return   ErrorCode::LOSTDUNGEON_PK_FAIL_TARGT_LEAVING;
	}

	if (attackeder->GetLostDungLeaveTag())
	{
		return   ErrorCode::LOSTDUNGEON_PK_FAIL_TARGT_LEAVING;
	}

	if (attackeder->GetLostDungBatSt() != LDPBT_NORMAL)
	{
		return   ErrorCode::LOSTDUNGEON_PK_FAIL_TARGT_BATELING;
	}

	if (attackeder->GetLostDungProtTime() > 0 && attackeder->GetLostDungProtTime() > (UInt32)CURRENT_TIME.Sec())
	{
		return   ErrorCode::LOSTDUNGEON_PK_FAIL_TARGT_PROTEDING;
	}

	//任务检查....
	CLProtocol::BattleLostDungeonPkReq req;
	req.dungeonId = dungeonId;
	req.attacker = attacker->GetLostBattleRacePlayerInfo();
	req.attackeder = attackeder->GetLostBattleRacePlayerInfo();
	
	req.battleId = lostScene->GetBattleId();

	Router::SendToBattle(req);
	
	attacker->SetLostDungBatSt(LDPBT_BATTLE_PVP);
	attackeder->SetLostDungBatSt(LDPBT_BATTLE_PVP);

	return ErrorCode::SUCCESS;
}

void LostDungeonMgr::SetTeamState(UInt32 battleId, UInt32 teamId, UInt8 st)
{
	InfoLogStream << "sync team state battleId : " << battleId << ", teamId : " << teamId
		<< ", state : " << st << LogStream::eos;
	LostDungeonTeam* team = FindTeamById(teamId);
	if (!team)
	{
		ErrorLogStream << " sync team state fail not find team, battleId : " << battleId << ", teamId : " << teamId
			<< ", state : " << st << LogStream::eos;
		return;
	}

	bool update = team->SetBattleState(st);
	if (update)
	{
		CLProtocol::SceneLostDungeonSyncDungeonTeamInfo syncTeam;
		syncTeam.type = SDETIT_TEAM_STATE;
		LostDungTeamInfo teamInfo;
		team->GetInfo(teamInfo);
		syncTeam.teamInfos.push_back(teamInfo);

		BroadcastMsgToTeamMembersInScene(team->GetSceneId(), syncTeam);
		InfoLogStream << "sync team info type : " << (UInt32)SDETIT_TEAM_STATE << ", teamId : " << teamInfo.teamId
			<< ", index : " << teamInfo.index << ", battleState : " << (UInt32)teamInfo.battleState << LogStream::eos;
	}
}

void LostDungeonMgr::OnTeamRaceEnd(UInt32 teamId)
{
	InfoLogStream << "rev team race end team id : " << teamId << LogStream::eos;

	LostDungeonTeam* team = FindTeamById(teamId);

	if (!team)
	{
		InfoLogStream << "rev team race end error, not find team id : " << teamId << LogStream::eos;
	}

	team->OnRaceEnd();
}

void LostDungeonMgr::OnPlayerAbnExit(Player* player)
{
	if (!player)	return;
	if (player->GetDungChageTeamId() == 0)	return;

	LostDungeonTeam* team = FindTeamById(player->GetDungChageTeamId());
	if (!team) return;

	LostDungeonTeam::PlayerMapMember members;
	team->RemoveMember(player->GetID(), members, false);

	if (team->GetNum() == 0)
	{
		bool update = team->SetBattleState(LDTBS_NORMAL);
		if (update)
		{
			CLProtocol::SceneLostDungeonSyncDungeonTeamInfo syncTeam;
			syncTeam.type = SDETIT_TEAM_STATE;
			LostDungTeamInfo teamInfo;
			team->GetInfo(teamInfo);
			syncTeam.teamInfos.push_back(teamInfo);

			BroadcastMsgToTeamMembersInScene(team->GetSceneId(), syncTeam);
		}
	}
}