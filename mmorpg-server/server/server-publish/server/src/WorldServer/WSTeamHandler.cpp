#include "WSTeamHandler.h"
#include <CLDungeonProtocol.h>
#include "WSPlayerMgr.h"
#include "WSTeamMgr.h"
#include "WSSceneMgr.h"
#include "WSNetwork.h"
#include "WSTeam.h"
#include "TeamMatchMgr.h"
#include "GuildMgr.h"


void HandleProtocol(WSPlayer* player, CLProtocol::WorldCreateTeam& req)
{
	player->CreateTeam(req.target);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldLeaveTeamReq& req)
{
	WSTeam* team = player->GetTeam();
	if (NULL == team || team->IsInVote())
	{
		return;
	}

	if(req.id == player->GetID())	//自身离开
	{
		team->RemoveMember(req.id);
	}
	else
	{
		if(team->GetMaster() != player->GetID()) return;
		team->RemoveMember(req.id);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldSetTeamOption& req)
{
    WSTeam* team = player->GetTeam();
    if (team == NULL) return;

    team->OnPlayerChangeTeamOption(player, (TeamOptionOperType)req.type, req.str, req.param1, req.param2);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldTransferTeammaster& req)
{
	WSTeam* team = player->GetTeam();
	if(team == NULL) return;

	team->TransferMaster(player, req.id);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldDismissTeam& req)
{
	WSTeam* team = player->GetTeam();
	if(team == NULL) return;

	if(team->Dismiss(player))
	{
		WSTeamMgr::Instance()->RemoveTeam(team);
		CL_SAFE_DELETE(team);
		return;
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldQueryTeamList& req)
{
    WSTeamMgr::Instance()->OnQueryTeamList(player, req);
}

class TeamOperSyncVistor : public WSTeamMemberVisitor
{
public:
    TeamOperSyncVistor(ObjID_t id, const CLProtocol::WorldTeamMasterOperSync& sync) : masterId(id), protocol(sync) {}

    bool operator()(WSTeamMember* member)
    {
        if (member && member->player != NULL)
        {
            member->player->SendProtocol(protocol);
        }
        return true;
    }

    ObjID_t                             masterId;
    CLProtocol::WorldTeamMasterOperSync   protocol;
};

void HandleProtocol(WSPlayer* player, CLProtocol::WorldTeamMasterOperSync& sync)
{
    if (!player)
    {
        return;
    }

    WSTeam* team = player->GetTeam();
    if (!team)
    {
        return;
    }

    if (team->GetMaster() != player->GetID())
    {
        return;
    }

    TeamOperSyncVistor vistor(player->GetID(), sync);
    team->VisitMembers(vistor);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldTeamRequesterListReq& req)
{
	if (!player)
	{
		return;
	}

	WSTeam* team = player->GetTeam();
	if (!team)
	{
		return;
	}

	if (team->GetMaster() != player->GetID())
	{
		return;
	}

	CLProtocol::WorldTeamRequesterListRes res;
	res.requesters = team->GetRequesterList();
	player->SendProtocol(res);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldTeamProcessRequesterReq& req)
{
	if (!player)
	{
		return;
	}

	WSTeam* team = player->GetTeam();
	if (!team)
	{
		return;
	}

	if (req.targetId == 0)
	{
		team->ClearRequesterList(player);
	}
	else
	{
		team->ReplyJoinRequest(player, req.targetId, req.agree != 0);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldTeamReportVoteChoice& vote)
{
	if (!player)
	{
		return;
	}

	WSTeam* team = player->GetTeam();
	if (!team)
	{
		return;
	}

	team->OnPlayerReportVote(player, vote.agree != 0);
}

void HandleProtocol(CLProtocol::WorldTeamBroadcastNotify& req)
{
	WSTeam* team = WSTeamMgr::Instance()->FindTeam(req.teamId);
	if(team == NULL) return;

	team->Notify(req.type, req.color, req.word.c_str());
}

void HandleProtocol(CLProtocol::WorldSceneCreateTeamReq& req)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(req.playerId);
	if(player != NULL)
	{
		//player->CreateTeam();
	}
}

void HandleProtocol(CLProtocol::SceneSyncTeamMemberAvatar& sync)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(sync.id);
	if (!player)
	{
		return;
	}

	auto team = player->GetTeam();
	if (!team)
	{
		return;
	}

	team->UpdateAvatar(sync.id, sync.avatar);
}

void HandleProtocol(CLProtocol::WorldServerTeamStartDungeonReq& req)
{
	auto team = WSTeamMgr::Instance()->FindTeam(req.teamId);
	if (!team)
	{
		ErrorLogStream << "can't find team(" << req.teamId << ")." << LogStream::eos;
		return;
	}

	// 如果是怪物攻城，不需要队友同意
	if (req.cityMonster.guid > 0)
	{
		team->SendStartRaceToScene(req.dungeonId, req.cityMonster);
		return;
	}

	// 如果是公会副本组队，判断玩家是否是同一个公会的
	DungeonDataEntry* dataEntry = DungeonDataEntryMgr::Instance()->FindEntry(req.dungeonId);
 	if (dataEntry && dataEntry->subType == DST_GUILD_DUNGEON)
	{
		UInt32 result = 0;
		do 
		{
			// 先检查队长是否有公会
			WSTeamMember* master = team->FindMember(team->GetMaster());
			if (NULL == master)
			{
				result = ErrorCode::ITEM_DATA_INVALID;
				break;
			}

			if (master->guildId == 0)
			{
				result = ErrorCode::GUILD_BATTLE_NOT_EXIST;
				break;
			}

			Guild* guild = GuildMgr::Instance()->FindGuildByID(master->guildId);
			if (NULL == guild)
			{
				result = ErrorCode::GUILD_BATTLE_NOT_EXIST;
				break;
			}

			static UInt32 guild_dungeon_limit = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_DUNGEON_GUILD_LEVEL_LIMIT);
			if (guild->GetLevel() < guild_dungeon_limit)
			{
				result = ErrorCode::GUILD_DUNGEON_GUILD_LEVEL_LIMIT;
				break;
			}

			static UInt32 guild_dungeon_player_limit = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_DUNGEON_PLAYER_LEVEL_LIMIT);
			const MemberVec& member_vec = team->GetMemberVec();
			for (auto& iter : member_vec)
			{
				if (iter.guildId != master->guildId)
				{
					ErrorLogStream << "WorldServerTeamStartDungeonReq, not same guild race!!! guild:" << LogStream::eos;
					result = ErrorCode::GUILD_DUNGEON_NOT_SAME_GUILD;
					break;
				}

				if (NULL == iter.player)
				{
					result = ErrorCode::ITEM_DATA_INVALID;
					break;
				}

				// 玩家等级不够挑战公会地下城
				if (iter.player->GetLevel() < guild_dungeon_player_limit)
				{
					result = ErrorCode::GUILD_DUNGEON_PLAYER_LEVEL_LIMIT;
					break;
				}
			}

		} while (0);

		if (result != ErrorCode::SUCCESS)
		{
			auto master = WSPlayerMgr::Instance()->FindNormalPlayer(team->GetMaster());
			if (master)
			{
				CLProtocol::SceneDungeonStartRes res;
				res.result = result;
				master->SendProtocol(res);
			}
			return;
		}
	}

	auto result = team->OnStartRaceVote(req.dungeonId);
	if (result != ErrorCode::SUCCESS)
	{
		auto master = WSPlayerMgr::Instance()->FindNormalPlayer(team->GetMaster());
		if (master)
		{
			CLProtocol::SceneDungeonStartRes res;
			res.result = result;
			master->SendProtocol(res);
		}
		return;
	}
}

void HandleProtocol(CLProtocol::WorldTeamMatchStartReq& req)
{
	auto player = WSPlayerMgr::Instance()->FindNormalPlayer(req.roleId);
	if (!player)
	{
		return;
	}

	

	CLProtocol::SceneTeamMatchStartRes res;

	if (!player->IsOpenTeamFunction())
	{
		res.result = ErrorCode::TEAM_MIN_LEVEL;
	}
	else if (!player->IsSceneCanJoinTeam())
	{
		res.result = ErrorCode::TEAM_JOIN_NOT_IN_TOWN;
	}
	else
	{
		auto team = player->GetTeam();
		if (team && team->GetMaster() != player->GetID())
		{
			res.result = ErrorCode::TEAM_MATCH_ONLY_MASTER;
		}
		else
		{
			res.result = TeamMatchMgr::Instance()->OnPlayerJoinMatch(player, req.dungeonId);
		}
	}
	

	if (res.result != ErrorCode::SUCCESS)
	{
		player->SendProtocol(res);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldTeamMatchCancelReq& req)
{
	if (!player)
	{
		return;
	}

	CLProtocol::WorldTeamMatchCancelRes res;

	auto team = player->GetTeam();
	if (team && team->GetMaster() != player->GetID())
	{
		res.result = ErrorCode::TEAM_MATCH_ONLY_MASTER;
	}
	else
	{
		res.result = TeamMatchMgr::Instance()->OnPlayerCancelMatch(player);
	}

	if (res.result != ErrorCode::SUCCESS)
	{
		player->SendProtocol(res);
	}
}

void HandleProtocol(CLProtocol::WorldTeamQueryTargetCondRes& res)
{
	auto player = WSPlayerMgr::Instance()->FindNormalPlayer(res.roleId);
	if (!player)
	{
		return;
	}

	auto team = player->GetTeam();
	if (team)
	{
		team->OnChangeRemainTimes(res.roleId, res.teamTargetId, res.remainTimes);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldChangeAssistModeReq& req)
{
	if (!player)
	{
		return;
	}

	auto team = player->GetTeam();
	if (team)
	{
		team->OnPlayerChangeAssist(player, req.isAssist != 0);
	}
}
