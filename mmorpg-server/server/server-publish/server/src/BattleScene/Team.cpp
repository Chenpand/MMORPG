#include "Team.h"

#include <CLGrid.h>
#include <CLTeamProtocol.h>
#include <CLMatchDefine.h>
#include "PlayerMgr.h"
#include "SSRouter.h"
#include "Scene.h"
#include <CLGameSessionProtocol.h>
#include <CLDungeonProtocol.h>
#include <CLSceneProtocol.h>
#include "SceneMgr.h"
#include "SSDungeonConfigMgr.h"
#include "CityMonster.h"
#include "ActivityMgr.h"

Team::Team(UInt32 id)
{
	m_Id = id;
    m_relayServerId = 0;
    m_raceId = 0;
	m_Master = 0;

	m_LastItemOwner = 0;

	m_TeamBossId = 0;
	m_BossSceneId = 0;
}

Team::~Team()
{
	for(MemberVec::iterator iter = m_Members.begin();
		iter != m_Members.end(); ++iter)
	{
		Player* player = PlayerMgr::Instance()->FindPlayer(iter->roleId);
		if(player != NULL)
		{
			player->SetTeam(NULL);
			player->UpdateTeamInfo();
		}
	}
}

bool Team::OnPlayerRequestStartDungeonRace(Player* player, UInt32 dungeonId, const SceneNpc& cityMonster)
{
	CLProtocol::SceneDungeonStartRes res;

    if (!player)
    {
        return false;
    }

    DebugLogStream << "team(" << GetId() << ") player(" << player->GetID() << ") request start dungeon(" << dungeonId << ")." << LogStream::eos;

    // 只有队长能操作
    if (player->GetID() != GetMaster())
    {
        DebugLogStream << "team(" << GetId() << ") player(" << player->GetID() << ") request start dungeon(" << dungeonId << "), player not team master." << LogStream::eos;
		res.result = ErrorCode::DUNGEON_START_CONDITION;
		player->SendProtocol(res);
        return false;
    }

	auto dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
	if (!dungeonData)
	{
		DebugLogStream << "team(" << GetId() << ") player(" << player->GetID() << ") request start dungeon(" << dungeonId << "), dungeon don't exist." << LogStream::eos;
		res.result = ErrorCode::DUNGEON_START_DUNGEON_NOT_EXIST;
		player->SendProtocol(res);
		return false;
	}

    if (!CanStartDungeonRace(dungeonId, true, true, cityMonster.guid > 0, cityMonster))
    {
		res.result = ErrorCode::DUNGEON_TEAM_START_RACE_FAILED;
		player->SendProtocol(res);

        DebugLogStream << "team(" << GetId() << ") player(" << player->GetID() << ") request start dungeon(" << dungeonId << "), can't start race." << LogStream::eos;
        return false;
    }

	CLProtocol::WorldServerTeamStartDungeonReq req;
	req.teamId = GetId();
	req.dungeonId = dungeonId;
	req.cityMonster = cityMonster;
	Router::SendToWorld(req);

    return true;
}

bool Team::OnWorldServerStartDungeonRes(UInt32 dungeonId, const SceneNpc& cityMonster)
{
	CLProtocol::SceneDungeonStartRes res;

	DebugLogStream << "team(" << GetId() << ") world start dungeon(" << dungeonId << ") response." << LogStream::eos;

	if (!CanStartDungeonRace(dungeonId, true, true, cityMonster.guid > 0, cityMonster))
	{
		DebugLogStream << "team(" << GetId() << ") request start dungeon(" << dungeonId << "), can't start race." << LogStream::eos;

		res.result = ErrorCode::DUNGEON_TEAM_START_RACE_FAILED;
		auto master = PlayerMgr::Instance()->FindNormalPlayer(GetMaster());
		if (master)
		{
			master->SendProtocol(res);
		}

		return false;
	}

	auto dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
	if (!dungeonData)
	{
		DebugLogStream << "team(" << GetId() << ") player(" << GetMaster() << ") request start dungeon(" << dungeonId << "), dungeon don't exist." << LogStream::eos;
		res.result = ErrorCode::DUNGEON_START_DUNGEON_NOT_EXIST;
		auto master = PlayerMgr::Instance()->FindNormalPlayer(GetMaster());
		if (master)
		{
			master->SendProtocol(res);
		}
		return false;
	}

	CLProtocol::WorldServerTeamCreateRaceReq req;
	req.teamId = GetId();
	req.dungeonId = dungeonId;
	req.isHell = dungeonData->IsHellEntry() ? 1 : 0;
	UInt8 seat = 0;
	for (auto member : _GetMemberPlayerInfo())
	{
		CLProtocol::Fighter fighter;
		fighter.accId = member->GetAccID();
		fighter.roleId = member->GetID();
		fighter.pos = seat++;

		req.fighters.push_back(fighter);
	}
	req.cityMonster = cityMonster;
	Router::SendToWorld(req);

	return true;
}

bool Team::OnRelayServerCreateRaceRet(UInt32 relayServerID, UInt32 dungeonId, UInt64 session, Avalon::SockAddr addr, bool isHell, const SceneNpc& cityMonster)
{
    DebugLogStream << "team(" << GetId() << ") create race success, relay server(" << relayServerID << ") addr(" 
                    << addr.ToString() << ") session(" << session << ") dungeonId(" << dungeonId << ")." << LogStream::eos;

	auto master = PlayerMgr::Instance()->FindPlayer(m_Master);
	if (!master)
	{
		ErrorLogStream << "team(" << GetId() << ") request start dungeon(" << dungeonId << "), team master is not online." << LogStream::eos;
		return false;
	}

	if (!CanStartDungeonRace(dungeonId, true, true, cityMonster.guid > 0, cityMonster))
    {
		CLProtocol::SceneDungeonStartRes res;
		res.result = ErrorCode::DUNGEON_TEAM_START_RACE_FAILED;
		master->SendProtocol(res);

        return false;
	}

	auto dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);

    std::vector<Player*> allMembers = _GetMemberPlayerInfo();

	// 找到老兵
	Player* veteranPlayer = NULL;
	//for (auto member : allMembers)
	//{
	//	if (!member) continue;

	//	if (!member->IsVeteranReturn())
	//	{
	//		continue;			
	//	}

	//	if (!veteranPlayer || (veteranPlayer->GetRoleVeteranReturnTime() < member->GetRoleVeteranReturnTime()))
	//	{
	//		// 最晚到期的老兵
	//		veteranPlayer = member;
	//	}
	//}

	// 共享buff
	std::vector<Buff*> shareBuffs;
	if (veteranPlayer != NULL && dungeonData != NULL)
	{
		ActivityMgr::Instance()->GetVeteranReturnOpActBuffs(veteranPlayer, shareBuffs, dungeonData);
	}

	CLProtocol::WorldDungeonStartRaceReq req;
	req.teamId = GetId();
	req.dungeonId = dungeonId;
	req.isHell = isHell;
	req.passedAreaIndex = 0;
	req.relayId = relayServerID;
	req.session = session;
	req.relayAddr = addr;
	req.cityMonster = cityMonster;
	req.isWeekHell = (dungeonData->subType == DST_WEEK_HELL_ENTRY) ? 1 : 0;

	UInt8 seat = 0;
	for (auto member : allMembers)
	{
		auto info = member->GetDungeonMgr().GetDungeonPlayerInfo(dungeonId);
		auto teamMember = FindMember(member->GetID());
		if (teamMember && SSActivityDungeonMgr::Instance()->IsDungeonTimesLimit(dungeonId))
		{
			info.isAssist = teamMember->statusMask & TEAM_MEMBER_MASK_ASSIST;
		}

		if (teamMember && cityMonster.type == SOT_CITYMONSTER && cityMonster.funcType == CITY_MONSTER_ACTIVITY)
		{
			if (member->GetDailyCityMonsterRemainTimes() == 0)
			{
				info.isAssist = info.isAssist | TEAM_MEMBER_MASK_ASSIST;
			}
		}

		info.raceInfo.pos = seat++;

		// 全队共享buff
		for (auto buff : shareBuffs)
		{
			if (!buff || !buff->GetDataEntry()) continue;

			bool hasShareBuff = false;
			for (auto raceBuff : info.raceInfo.buffs)
			{
				if (raceBuff.m_id == buff->GetId())
				{
					hasShareBuff = true;
					break;
				}
			}

			// 如果其他人没有共享buff
			if (!hasShareBuff)
			{
				RaceBuffInfo raceBuff;
				raceBuff.m_id = buff->GetId();
				raceBuff.m_overlayNums = buff->GetOverlay();
				raceBuff.m_startTime = buff->GetStartTime().MSec();
				raceBuff.m_duration = buff->GetDuration();
				info.raceInfo.buffs.push_back(raceBuff);

				UInt32 expAdd = buff->GetDataEntry()->expAddRate.GetValue();
				info.addition.additionRate[DART_EXP_RATE] += expAdd;
				info.addition.additionRate[DART_BUFF_EXP_RATE] += expAdd;

				InfoLogStream << PLAYERINFO(member) << " add veteran(" << PLAYERINFO(veteranPlayer) << ")'s buff(" << buff->GetId() << ")'s addition, exp add(" << expAdd << ")." << LogStream::eos;
			}
		}

		req.playerInfoes.push_back(info);
	}

	Router::SendToWorld(req);
	
	// 如果怪物攻城活动，在开始时就扣次数
// 	if (cityMonster.guid > 0)
// 	{
// 		auto monster = SceneMgr::Instance()->FindCityMonster(cityMonster.guid);
// 		if (monster && monster->GetMonsterType() == CITY_MONSTER_ACTIVITY)
// 		{
// 			monster->OnKilled(session);
// 		}
// 	}

	// 所有队员都切到对应的关卡选择界面
	/*for (auto member : allMembers)
	{
		if (!member || member == master || member->GetSceneID() == master->GetSceneID())
		{
			continue;
		}

		auto pos = master->GetPos();
		member->ChangeScene(master->GetSceneID(), pos, 0);
	}
	*/
    return true;
}

bool Team::OnDungeonRaceEnd(const DungeonRaceEndInfo& info)
{
    /*auto race = DungeonRaceMgr::Instance()->FindDungeonRaceById(GetRaceID());
    if (!race)
    {
        ErrorLogStream << "team(" << GetId() << ") race session(" << info.gamesessionId 
                        << ") can't find dugeon race(" << GetRaceID() << ")." << LogStream::eos;

        return false;
    }

    for (auto& endInfo : info.infoes)
    {
        auto player = PlayerMgr::Instance()->FindPlayer(endInfo.roleId);
        if (player)
        {
            race->OnPlayerRaceEnd(player, (LevelScore)endInfo.score, endInfo.beHitCount, info.usedTime);
        }
    }*/

    return true;
}

void Team::OnPlayerLeaveDungeonRace(ObjID_t roleId)
{
    DebugLogStream << "team(" << GetId() << ") player(" << roleId << ") leave dungeon race." << LogStream::eos;

    if (!IsMember(roleId))
    {
        ErrorLogStream << "team(" << GetId() << ") player(" << roleId << ") leave dungeon race, but player is not team member." << LogStream::eos;
        return;
    }

    // 通知relay server玩家离开比赛
    CLProtocol::SceneServerPlayerLeaveRace notify;
    notify.raceId = GetSession();
    notify.roleId = roleId;
    Router::SendToRelay(GetRelayServerID(), notify);

    /*if (GetMaster() == roleId)
    {
        // 如果是队长离开地下城，就把所有玩家都带出地下城
        auto players = _GetMemberPlayerInfo();
        for (auto player : players)
        {
            if (!player || player->GetID() == GetMaster())
            {
                // 队长已经离开地下城了，所以不用处理
                continue;
            }

            if (player->GetEnterSceneID() == 0)
            {
                UInt32 initScene = PlayerMgr::Instance()->GetInitSceneID();
                Scene *scene = SceneMgr::Instance()->FindNormalScene(GetRealSceneId(initScene));
                if (scene)
                {
                    CLPosition pos = scene->GetBirthPoint();
                    player->ChangeScene(initScene, pos, 0, true);
                }
            }
            else
            {
                CLPosition pos = player->GetEnterPos();
                player->ChangeScene(player->GetEnterSceneID(), pos, 0, true);
            }
        }
    }*/
}

bool Team::CanStartDungeonRace(UInt32 dungeonId, bool checkTicket, bool sendNotice, bool checkLoading, const SceneNpc& cityMonster)
{
    Player* master = PlayerMgr::Instance()->FindPlayer(GetMaster());
    if (!master)
    {
        ErrorLogStream << "team(" << GetId() << ") request start dungeon(" << dungeonId << "), team master is not online." << LogStream::eos;
        return false;
    }

	auto dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
	if (!dungeonData)
	{
		ErrorLogStream << "player(" << master->GetID() << ") want to start a invalid dungeon(" << dungeonId << ")." << LogStream::eos;
		return false;
	}

	// 检查城镇怪物是否能打
	if (cityMonster.guid > 0)
	{
		auto monster = SceneMgr::Instance()->FindCityMonster(cityMonster.guid);
		if (!monster)
		{
			DebugLogStream << "player(" << master->GetID() << ") want to start monster dungeon(" << dungeonId << ") monster(" << cityMonster.guid << ") not exist." << LogStream::eos;
			BroadcastNotify(ErrorCode::DUNGEON_START_MONSTER_NOT_EXIST);
			return false;
		}

		auto tmpResult = monster->CanFight();
		if (tmpResult != ErrorCode::SUCCESS)
		{
			DebugLogStream << "player(" << master->GetID() << ") want to start monster dungeon(" << dungeonId << ") monster(" << monster->ToString() << ") result(" << tmpResult << ")." << LogStream::eos;
			BroadcastNotify(tmpResult);
			return false;
		}
	}

	bool ret = true;
	// 有没有非助战进入的玩家
	bool hasNormalPlayer = false;
	std::vector<Player*> allMembers;
	for (const auto& member : m_Members)
	{
		auto player = PlayerMgr::Instance()->FindPlayer(member.roleId);
		if (!player || player->GetGameStatus() != PS_NORMAL)
		{
			InfoLogStream << "team(" << GetId() << ") request start dungeon(" << dungeonId << "), player(" << member.roleId << ") is not online." << LogStream::eos;	
			ret = false;
			BroadcastNotify(1055);
			continue;
		}

		// 不能再战斗中
		if (!player->GetScene() || (player->GetScene()->GetType() == SCENE_TYPE_PK || player->GetScene()->GetType() == SCENE_TYPE_ACTIVITY))
		{
			InfoLogStream << "team(" << GetId() << ") request start dungeon(" << dungeonId << "), player(" << member.roleId << ") is in scene(" << player->GetSceneID() << ")." << LogStream::eos;
			ret = false;
			BroadcastNotify(1059, player->GetName());
			continue;
		}

		// 是否在loading中
		if (checkLoading && player->IsClientLoading())
		{
			// 为防止两边状态不对，如果玩家在loading中，服务器向客户端重新请求最新状态
			InfoLogStream << "team(" << GetId() << ") request start dungeon(" << dungeonId << "), player(" << member.roleId << ") is loading." << LogStream::eos;
			ret = false;
			hasNormalPlayer = true;
			BroadcastNotify(1059, player->GetName());

			CLProtocol::SceneQueryLoadingInfo protocol;
			player->SendProtocol(protocol);
			continue;
		}

		// 能不能打
		bool isMaster = member.roleId == GetMaster();
		bool isAssist = (member.statusMask & TEAM_MEMBER_MASK_ASSIST) != 0;

		// 没有次数限制的图没有助战
		if (isAssist && !SSActivityDungeonMgr::Instance()->IsDungeonTimesLimit(dungeonId))
		{
			isAssist = false;
		}

		if (!isAssist)
		{
			hasNormalPlayer = true;
		}

		// 助战模式下不需要判断体力和次数
		UInt32 result = player->GetDungeonMgr().CanPlayThisMap(dungeonId, checkTicket, isMaster, isMaster, !isAssist, !isAssist);
		if (result != ErrorCode::SUCCESS)
		{
			InfoLogStream << "team(" << GetId() << ") request start dungeon(" << dungeonId << "), player(" << member.roleId << ") can't play this map reason(" << result << ")." << LogStream::eos;
			if (result == ErrorCode::DUNGEON_START_NO_FATIGUE)
			{
				BroadcastNotify(1056, player->GetName());
			}
			else if (result == ErrorCode::DUNGEON_START_NO_TICKET)
			{
				ItemDataEntry* itemDataEntry = ItemDataEntryMgr::Instance()->FindEntry(dungeonData->ticket.itemId);
				std::string ticketName = itemDataEntry ? itemDataEntry->name : "";
				BroadcastNotify(3101, player->GetName(), ticketName);
			}
			else if (result == ErrorCode::DUNGEON_START_NO_TIMES)
			{
				BroadcastNotify(1061, player->GetName());
			}
			else if (result == ErrorCode::DUNGEON_START_MUST_SINGLE)
			{
				BroadcastNotify(1106);
			}
			else if (result == ErrorCode::DUNGEON_START_NOT_OPEN_TIME)
			{
				BroadcastNotify(ErrorCode::DUNGEON_START_NOT_OPEN_TIME);
			}
			else
			{
				BroadcastNotify(1058, player->GetName());
			}

			player->GetDungeonMgr().CheckMallGiftActivateCondition(result, dungeonData->subType);

			ret = false;
			continue;
		}
	}

	if (!hasNormalPlayer)
	{
		// 发送通知，ToDO...
		master->SendNotify(3126);
		ret = false;
	}

    return ret;
}

std::vector<Player*> Team::_GetMemberPlayerInfo()
{
    std::vector<Player*> allMembers;
    for (auto memberInfo : m_Members)
    {
        Player* member = PlayerMgr::Instance()->FindPlayer(memberInfo.roleId);
        if (!member)
        {
            continue;
        }

        allMembers.push_back(member);
    }

    return allMembers;
}

void Team::SetMembers(const MemberVec& members)
{
	m_Members = members;
	for(MemberVec::iterator iter = m_Members.begin();
		iter != m_Members.end(); ++iter)
	{
		Player* player = PlayerMgr::Instance()->FindNormalPlayer(iter->roleId);
		if(player != NULL)
		{
			player->SetTeam(this);
			player->SyncTeamAround(false);
			SyncScenePos(player, true);
			player->SyncAvatarToTeam();
		}
	}
}

void Team::AddMember(ObjID_t member)
{
	if(m_Members.size() >= MAX_TEAM_SIZE) return;
	if(IsMember(member)) return;

    SceneTeamMember memberInfo;
    memberInfo.roleId = member;

	m_Members.push_back(memberInfo);
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(member);
	if(player != NULL)
	{
		player->SetTeam(this);
		player->SyncTeamAround(false);
		player->SyncTeamAroundToMe();
		SyncScenePos(player, true);
		player->SyncAvatarToTeam();
	}
}

void Team::RemoveMember(ObjID_t member)
{
	for(MemberVec::iterator iter = m_Members.begin();
		iter != m_Members.end(); ++iter)
	{
		if(member == iter->roleId)
		{
			Player* player = PlayerMgr::Instance()->FindPlayer(member);
			if(player != NULL)
			{
				player->SetTeam(NULL);
				player->UpdateTeamInfo();
			}
			m_Members.erase(iter);
			break;
		}
	}
}

bool Team::IsMember(ObjID_t member) const
{
	for(MemberVec::const_iterator iter = m_Members.begin();
		iter != m_Members.end(); ++iter)
	{
		if(iter->roleId == member) return true;
	}
	return false;
}

SceneTeamMember* Team::FindMember(ObjID_t id)
{
	for (auto& member : m_Members)
	{
		if (member.roleId == id)
		{
			return &member;
		}
	}

	return NULL;
}

void Team::VisitMembersAround(UInt32 sceneId, const CLPosition& pos, TeamMemberVisitor& visitor)
{
	for(MemberVec::iterator iter = m_Members.begin();
		iter != m_Members.end(); ++iter)
	{
		Player* player = PlayerMgr::Instance()->FindNormalPlayer(iter->roleId);
		if(player != NULL
			&& player->GetSceneID() == sceneId
			&& player->GetPos().Distance(pos) < player->GetScene()->GetSyncRange())
		{
			if(!visitor(player)) return;
		}
	}
}

void Team::VisitSceneMembers(UInt32 sceneId, TeamMemberVisitor& visitor)
{
	for(MemberVec::iterator iter = m_Members.begin();
		iter != m_Members.end(); ++iter)
	{
		Player* player = PlayerMgr::Instance()->FindNormalPlayer(iter->roleId);
		if(player != NULL && player->GetSceneID() == sceneId)
		{
			visitor(player);
		}
	}
}

UInt32 Team::GetMemberNumInScene()
{
	UInt32 num = 0;
	for(MemberVec::iterator iter = m_Members.begin();
		iter != m_Members.end(); ++iter)
	{
		Player* player = PlayerMgr::Instance()->FindNormalPlayer(iter->roleId);
		if(player != NULL)
		{
			++num;
		}
	}
	return num;
}

UInt32 Team::GetMemberNumAround(UInt32 sceneId, UInt16 x, UInt16 y)
{
	UInt32 num = 0;
	CLPosition pos(x, y);
	for(MemberVec::iterator iter = m_Members.begin();
		iter != m_Members.end(); ++iter)
	{
		Player* player = PlayerMgr::Instance()->FindNormalPlayer(iter->roleId);
		if(player != NULL && player->GetSceneID() == sceneId
			&& player->GetPos().Distance(pos) <= player->GetScene()->GetSyncRange())
		{
			++num;
		}
	}
	return num;
}

ObjID_t	Team::CheckMembersLevel(UInt16 minLevel)
{
	for(MemberVec::iterator iter = m_Members.begin();
		iter != m_Members.end(); ++iter)
	{
		Player* player = PlayerMgr::Instance()->FindNormalPlayer(iter->roleId);
		if(player != NULL && player->GetLevel() < minLevel) return iter->roleId;
	}
	return 0;
}

UInt16 Team::GetMembersMaxLevel()
{
	UInt16 maxLevel = 0;
	for(MemberVec::iterator iter = m_Members.begin();
		iter != m_Members.end(); ++iter)
	{
		Player* player = PlayerMgr::Instance()->FindNormalPlayer(iter->roleId);
		if(player != NULL && player->GetLevel() > maxLevel) maxLevel = player->GetLevel();
	}
	return maxLevel;
}

UInt16 Team::GetMembersAverageLevel()
{
	UInt16 totalLevel = 0;
	for (MemberVec::iterator iter = m_Members.begin();
		iter != m_Members.end(); ++iter)
	{
		Player* player = PlayerMgr::Instance()->FindNormalPlayer(iter->roleId);
		if (player != NULL)
		{
			totalLevel += player->GetLevel();
		}
	}
	UInt16 averageLevel = 0;
	if (GetMemberNumInScene() != 0)
	{
		averageLevel = totalLevel / GetMemberNumInScene();
	}
	return averageLevel;
}
ObjID_t	Team::CheckPackGridNum(UInt32 num)
{
	for(MemberVec::iterator iter = m_Members.begin();
		iter != m_Members.end(); ++iter)
	{
		Player* player = PlayerMgr::Instance()->FindNormalPlayer(iter->roleId);
		if(player != NULL && player->GetPackGridLeft() < num) return iter->roleId;
	}
	return 0;
}

ObjID_t Team::CheckTaskAcceptable(UInt32 id)
{
	for(MemberVec::iterator iter = m_Members.begin();
		iter != m_Members.end(); ++iter)
	{
		Player* player = PlayerMgr::Instance()->FindNormalPlayer(iter->roleId);
		if((player != NULL && !player->IsTaskAcceptable(id)) 
			|| player->GetTaskMgr().GetTaskNum() >= MAX_ACCEPT_TASK_NUM) return iter->roleId;
	}
	return 0;
}

void Team::AcceptTask(UInt32 id, UInt8 quality)
{
	for(MemberVec::iterator iter = m_Members.begin();
		iter != m_Members.end(); ++iter)
	{
		Player* player = PlayerMgr::Instance()->FindNormalPlayer(iter->roleId);
		if(player != NULL)
		{
			player->AcceptTask(id, quality);
		}
	}
}

void Team::SubmitTask(UInt32 id, UInt32 sceneId, UInt16 x, UInt16 y)
{
	CLPosition pos(x,y);
	for(MemberVec::iterator iter = m_Members.begin();
		iter != m_Members.end(); ++iter)
	{
		Player* player = PlayerMgr::Instance()->FindNormalPlayer(iter->roleId);
		if(player != NULL 
			&& player->GetSceneID() == sceneId
			&& player->GetPos().Distance(pos) <= player->GetScene()->GetSyncRange())
		{
			Task* task = player->FindTask(id);
			if(task != NULL && task->GetStatus() == TASK_UNFINISH)
			{
				task->SetStatus(TASK_FINISHED);
				player->SubmitTask(id);
			}
		}
	}
}

void Team::UpdateTeamInfo()
{
	for(MemberVec::iterator iter = m_Members.begin();
		iter != m_Members.end(); ++iter)
	{
		Player* player = PlayerMgr::Instance()->FindNormalPlayer(iter->roleId);
		if(player != NULL)
		{
			player->UpdateTeamInfo();
		}
	}
}

void Team::UpdateTeamMemberStatus(ObjID_t id, UInt8 status)
{
    for (MemberVec::iterator iter = m_Members.begin();
        iter != m_Members.end(); ++iter)
    {
        if (iter->roleId == id)
        {
            iter->statusMask = status;
        }
    }
}

bool Team::IsAllMemberReady()
{
    for (MemberVec::iterator iter = m_Members.begin();
        iter != m_Members.end(); ++iter)
    {
        if (!iter->isReady())
        {
            return false;
        }
    }

    return true;
}

/*
//by huchenhui
void Team::BroadcastNotify(UInt32 id, ... )
{
	va_list args;
	va_start(args, id);
	UInt16 type = 0;
	UInt8 color = 0;
	char buffer[CL_MAX_CHAT_LEN+1];
	memset(buffer, 0, sizeof(buffer));
	bool ret = SysNotifyMgr::Instance()->MakeNotify(id, type, color, buffer, CL_MAX_CHAT_LEN, args);
	va_end(args);
	if(!ret) return;

	Notify(type, color, buffer);
}
*/

void Team::Notify(UInt16 type, UInt8 color, const char* word)
{
	if(word == NULL) return;

	CLProtocol::WorldTeamBroadcastNotify notify;
	notify.teamId = GetId();
	notify.type = type;
	notify.color = color;
	notify.word  = word;
	Router::SendToWorld(notify);
}

void Team::BroadcastProtocol(Avalon::Protocol& protocol)
{
    auto members = _GetMemberPlayerInfo();
    for (auto member : members)
    {
        if (!member)
        {
            continue;
        }

        member->SendProtocol(protocol);
    }
}

void Team::SetTaskVar(UInt32 taskId, const char* name, Int64 val)
{
	if(name == NULL) return;

	for(MemberVec::iterator iter = m_Members.begin();
		iter != m_Members.end(); ++iter)
	{
		Player* player = PlayerMgr::Instance()->FindNormalPlayer(iter->roleId);
		if(player != NULL)
		{
			Task* task = player->FindTask(taskId);
			if(task != NULL)
			{
				task->SetVar(name, val);
			}
		}
	}
}
bool Team::IsMixedTeamInScene(UInt32 sceneId)
{	
	UInt8 sex = 2;
	for (MemberVec::iterator iter = m_Members.begin();
		iter != m_Members.end(); ++iter)
	{
		Player* player = PlayerMgr::Instance()->FindNormalPlayer(iter->roleId);
		if (player != NULL && player->GetSceneID() == sceneId)
		{
			if (sex == 2)
			{
				sex = player->GetSex();
				continue;
			}
			else if(sex != player->GetSex())
			{
				return true;
			}
		}
	}
	return false;
}
void Team::OnEnterScene(Player* player)
{
	SyncScenePos(player, true);
}

void Team::OnLeaveScene(Player* player)
{
	class OnLeaveSceneVisitor : public TeamMemberVisitor
	{
	public:
		OnLeaveSceneVisitor(Player* player):m_pPlayer(player){}

		bool operator()(Player* player)
		{
			if(m_pPlayer == player) return true;
			if(m_pPlayer->Distance(player) >= m_pPlayer->GetScene()->GetSyncRange())
			{
				CLProtocol::SceneNotifyTeamMemberLeaveScene notify;
				notify.id = m_pPlayer->GetID();
				player->SendProtocol(notify);
			}
			return true;
		}
	private:
		Player* m_pPlayer;
	};
	OnLeaveSceneVisitor visitor(player);
	VisitSceneMembers(player->GetSceneID(), visitor);
}

void Team::OnMoved(Player* player)
{
	SyncScenePos(player, false);
}

void Team::SyncScenePos(Player* player, bool bOtherPos)
{
	class SyncScenePosVisitor : public TeamMemberVisitor
	{
	public:
		SyncScenePosVisitor(Player* player, bool bOtherPos)
			:m_pPlayer(player),m_bOtherPos(bOtherPos){}

		bool operator()(Player* player)
		{
			if(m_pPlayer == player) return true;
			if(m_pPlayer->Distance(player) >= m_pPlayer->GetScene()->GetSyncRange())
			{
				CLProtocol::SceneSyncTeamMemberPos sync;
				
				if(m_bOtherPos)
				{
					sync.id = player->GetID();
					sync.pos = player->GetPos();
					m_pPlayer->SendProtocol(sync);
				}
				
				sync.id = m_pPlayer->GetID();
				sync.pos = m_pPlayer->GetPos();
				player->SendProtocol(sync);
			}
			return true;
		}
	private:
		Player* m_pPlayer;
		bool	m_bOtherPos;
	};
	SyncScenePosVisitor visitor(player, bOtherPos);
	VisitSceneMembers(player->GetSceneID(), visitor);
}
