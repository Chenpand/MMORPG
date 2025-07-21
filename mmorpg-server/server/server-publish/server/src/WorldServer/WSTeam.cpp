#include "WSTeam.h"

#include <set>
#include <CLChatProtocol.h>
#include <CLGameSessionProtocol.h>
#include <CLDungeonDataEntry.h>
#include <CLForbitWord.h>
#include <CLTeamDungeonDataEntry.h>
#include <CLDungeonProtocol.h>
#include <CLSysNotify.h>
#include <CLDungeonTimesDataEntry.h>

#include "WSPlayer.h"
#include "WSPlayerMgr.h"
#include "WSNetwork.h"
#include "WSRouter.h"
#include "TeamMatchMgr.h"
#include "WSServiceSwitchMgr.h"
#include "DungeonRaceMgr.h"


void WSTeamMember::SetPlayer(WSPlayer* p)
{
	if(p != NULL)
	{
		player = p;
		id = p->GetID();
		name = p->GetName();
		level = p->GetLevel();
		occu = p->GetOccu();
		avatar = p->GetAvatar();
		guildId = p->GetGuildID();
		vipLevel = p->GetVipLvl();
		resistMagic = p->GetResistMagic();
		p->GetPlayerLabelInfo(playerLabelInfo);
	}
	else
	{
		player = NULL;
	}
}

WSTeam::WSTeam(WSPlayer* master)
{
	m_Id = 0;
	m_Status = TEAM_STATUS_IDEL;
	m_Master = master->GetID();
	m_DismissTime = 0;
	m_AutoAgree = true;
	m_OptionMask = TO_INIT;

	WSTeamMember member;
	member.enterTime = UInt32(CURRENT_TIME.Sec());
	member.SetPlayer(master);
	m_Members.push_back(member);
	master->SetTeam(this);
	m_IsMatched = false;
	m_KickMasterTimer.SetInterval(SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_CAPTAIN_LEAVE_TEAM_TIME) * Avalon::Time::MSECS_OF_SEC);
	m_KickMasterControl = true;
	m_KickMasterBroadcastTimer.SetInterval(Avalon::Time::MSECS_OF_SEC);
}

WSTeam::WSTeam(WSPlayer* master, WSPlayer* member) : WSTeam(master)
{
	WSTeamMember member2;
	member2.enterTime = UInt32(CURRENT_TIME.Sec());
	member2.SetPlayer(member);
	m_Members.push_back(member2);
	member->SetTeam(this);
}

WSTeam::~WSTeam()
{
}

void WSTeam::OnTick(const Avalon::Time& now)
{
	if (m_Vote.IsTimeout())
	{
		for (auto& member : m_Members)
		{
			if (!m_Vote.HasPlayer(member.id))
			{
				UInt16 type = 0;
				UInt8 color = 0;
				char buffer[CL_MAX_CHAT_LEN + 1];
				bool ret = SysNotifyMgr::Instance()->MakeNotify(3122, type, color, buffer, CL_MAX_CHAT_LEN, member.name);
				if (ret)
				{
					Notify(type, color, buffer);
				}
			}
		}

		OnStartRaceVoteFailure();
	}

	if (m_KickMasterControl && GetStatus() == TEAM_STATUS_IDEL && WSApplication::Instance()->IsTeamAutoKick())
	{
		if (SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_CAPTAIN_LEAVE_TEAM_TIME) * Avalon::Time::MSECS_OF_SEC - m_KickMasterTimer.GetEscapeTime(now) <= Avalon::Time::MSECS_OF_SEC * Avalon::Time::SECS_OF_MIN)
		{
			if (m_KickMasterBroadcastTimer.IsTimeout(now))
			{
				BroadcastTeamKick(true);
			}
		}

		if (m_KickMasterTimer.IsTimeout(now))
		{
			RemoveMember(GetMaster());
			BroadcastTeamKick(false);
		}
	}
}

WSTeamMember* WSTeam::FindMember(ObjID_t id)
{
	for(MemberVec::iterator iter = m_Members.begin();
		iter != m_Members.end(); ++iter)
	{
		if(id == iter->id) return &(*iter);
	}
	return NULL;
}

void WSTeam::OnPlayerChangeTeamOption(WSPlayer* player, TeamOptionOperType type, std::string str, UInt32 param1, UInt32 param2)
{
    if (!player || player->GetID() != GetMaster())
    {
        return;
    }

    if (type == TEAM_OPTION_TARGET)
    {
		auto result = ChangeTeamTarget(param1);
		if (result != ErrorCode::SUCCESS)
		{
			player->SendNotify(result);
			return;
		}
    }
	else if (type == TEAM_OPTION_AUTO_AGREE)
	{
		bool agree = param1 != 0;
		if (IsAutoAgree() == agree)
		{
			return;
		}

		SetAutoAgree(param1 != 0);
	}
	else if (type == TEAM_OPTION_HELL_AUTO_CLOSE)
	{
		DebugLogStream<< "player(" << player->GetID() << ") request change hell auto close(" << param1 << ")." << LogStream::eos;

		bool open = param1 != 0;
		if (IsSetOption(TO_HELL_AUTO_CLOSE) == open)
		{
			DebugLogStream << "player(" << player->GetID() << ") team's hell auto close is already (" << param1 << ")." << LogStream::eos;
			return;
		}

		if (open)
		{
			SetOption(TO_HELL_AUTO_CLOSE);
		}
		else
		{
			ClearOption(TO_HELL_AUTO_CLOSE);
		}
	}
	else
	{
		return;
	}

    CLProtocol::WorldSyncTeamOption sync;
    sync.type = type;
    sync.str = str;
    sync.param1 = param1;
    sync.param2 = param2;
    Broadcast(sync);
}

void WSTeam::OnPlayerChangeAssist(WSPlayer* player, bool isAssist)
{
	if (!player)
	{
		return;
	}

	// 只有次数限制的图允许助战
	if (!IsTargetTimesLimit())
	{
		return;
	}

	ChangeMemberAssistStatus(player->GetID(), isAssist, true);
}

UInt32 WSTeam::OnStartRaceVote(UInt32 dungeonId)
{
	// 说明已经有投票了
	if (IsInVote())
	{
		ErrorLogStream << "team(" << GetId() << ") repeat to vote dungeon(" << dungeonId << ")." << LogStream::eos;
		return ErrorCode::DUNGEON_TEAM_START_VOTE_REPEAT;
	}

	auto data = TeamDungeonDataEntryMgr::Instance()->FindEntryByDungeonID(dungeonId);
	if (!data)
	{
		ErrorLogStream << "team(" << GetId() << ") dungeon(" << dungeonId << ") can't play in team." << LogStream::eos;
		return ErrorCode::DUNGEON_TEAM_TARGET_MUST_SINGLE;
	}

	if (GetSize() < data->minPlayerNum)
	{
		ErrorLogStream << "team(" << GetId() << ") dungeon(" << dungeonId << ") need member(" << data->minPlayerNum << ")" << LogStream::eos;
		return ErrorCode::DUNGEON_TEAM_NOT_ENOUGH_MEMBER;
	}

	if (GetSize() > data->maxPlayerNum)
	{
		ErrorLogStream << "team(" << GetId() << ") dungeon(" << dungeonId << ") max member(" << data->maxPlayerNum << ")" << LogStream::eos;
		return ErrorCode::DUNGEON_TEAM_TOO_MANY_MEMBER;
	}

	TeamMatchMgr::Instance()->OnTeamCancelMatch(this);

	m_Vote.Clear();
	m_Vote.dungeonId = dungeonId;
	m_Vote.startTime = (UInt32)CURRENT_TIME.Sec();
	m_Vote.agreePlayerIds.push_back(GetMaster());

	ResetKickMasterTimer();

	// 如果队伍只有一个人，直接进入战斗
	if (GetSize() == 1)
	{
		OnStartRaceVoteAllAgree();
		return ErrorCode::SUCCESS;
	}

	// 通知所有队员(除了队长)
	CLProtocol::WorldTeamRaceVoteNotify notify;
	notify.dungeonId = dungeonId;
	Broadcast(notify);

	return ErrorCode::DUNGEON_TEAM_WAIT_OTHER_VOTE;
}

void WSTeam::OnPlayerReportVote(WSPlayer* player, bool agree)
{
	if (!player)
	{
		return;
	}

	// 没有投票
	if ( !IsInVote())
	{
		ErrorLogStream << "team(" << GetId() << ") player(" << player->GetID() << ") vote not exist." << LogStream::eos;
		return;
	}

	// 已经投过票了
	if (m_Vote.HasPlayer(player->GetID()))
	{
		ErrorLogStream << "team(" << GetId() << ") player(" << player->GetID() << ") repeat vote." << LogStream::eos;
		return;
	}

	if (!agree)
	{
		OnStartRaceVoteFailure(player->GetID());
	}
	else
	{
		CLProtocol::WorldTeamVoteChoiceNotify notify;
		notify.roleId = player->GetID();
		notify.agree = agree ? 1 : 0;
		Broadcast(notify);

		m_Vote.agreePlayerIds.push_back(player->GetID());
		if (m_Vote.agreePlayerIds.size() == GetSize())
		{
			OnStartRaceVoteAllAgree();
		}
	}
}

void WSTeam::OnStartRaceVoteAllAgree()
{
	if ( !IsInVote())
	{
		return;
	}

	DebugLogStream << "team(" << GetId() << ") race vote dungeon(" << m_Vote.dungeonId << ") success." << LogStream::eos;
	SendStartRaceToScene(m_Vote.dungeonId, SceneNpc());
	m_Vote.Clear();
}

void WSTeam::OnStartRaceVoteFailure(ObjID_t roleId, UInt32 noticeId)
{
	if ( !IsInVote())
	{
		return;
	}

	DebugLogStream << "team(" << GetId() << ") race vote dungeon(" << m_Vote.dungeonId << ") failure." << LogStream::eos;

	// 通知玩家投票失败
	CLProtocol::WorldTeamVoteChoiceNotify notify;
	notify.roleId = roleId;
	notify.agree = 0;
	Broadcast(notify);

	if (noticeId)
	{
		Notify(0, 0, SysNotifyMgr::Instance()->MakeString(noticeId).c_str());
	}
	
	m_Vote.Clear();

}

void WSTeam::SendStartRaceToScene(UInt32 dungeonId, SceneNpc cityMonster)
{
	auto master = WSPlayerMgr::Instance()->FindNormalPlayer(GetMaster());
	if (master)
	{
		CLProtocol::WorldServerTeamStartDungeonRes res;
		res.teamId = GetId();
		res.dungeonId = dungeonId;
		res.cityMonster = cityMonster;
		master->SendToScene(res);
	}
}

void WSTeam::TransferMaster(WSPlayer* player, ObjID_t id)
{
	if (player == NULL || player->GetID() != GetMaster() || player->GetID() == id)
	{
		return;
	}

    auto member = FindMember(id);
	if (member == NULL)
	{
		return;
	}

	m_Master = id;

	ResetKickMasterTimer();

	//同步队长所有成员
	CLProtocol::WorldSyncTeammaster sync;
	sync.master = m_Master;
	Broadcast(sync);

	SyncTeamBaseToScene();
}

void WSTeam::SetStatus(TeamStatus status)
{
	if (m_Status != status)
	{
		ResetKickMasterTimer();
	}
	m_Status = status;
}

TeamStatus WSTeam::GetStatus() const
{
	if (m_Status != TEAM_STATUS_IDEL)
	{
		return m_Status;
	}

	// 当队伍状态是空闲的时候，需要判断所有玩家是不是都不在战斗中了
	// 临时代码，这样做安全点，下一期把这个机制改掉
	// ToDo...
	if (WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_NEW_TEAM_STATUS))
	{
		for (auto& member : m_Members)
		{
			if (member.isRacing == true)
			{
				return TEAM_STATUS_RACING;
			}
		}
	}

	return m_Status;
}

UInt32 WSTeam::ChangeTeamTarget(UInt32 target)
{
	auto teamDungeonData = TeamDungeonDataEntryMgr::Instance()->FindEntry(target);
	if (!teamDungeonData || teamDungeonData->type == TEAM_DUNGEON_MENU)
	{
		return ErrorCode::TEAM_TARGET_INVALID;
	}

	if (GetSize() > teamDungeonData->maxPlayerNum)
	{
		return ErrorCode::TEAM_TOO_MANY_PLAYER;
	}
	
	SetTeamTarget(target);
	SetTargetDungeonID(teamDungeonData->dungeonId);
	SetMaxTeamMemberNum(teamDungeonData->maxPlayerNum);

	// 同步队伍属性给被邀请者
	SyncTeamAttrToInvitePlayer();

	if (IsTargetTimesLimit())
	{
		QueryAllMemberRemainTimes(target);
	}
	else
	{
		// 如果目标图是没有次数限制，强制把所有成员的剩余次数置为0，取消助战模式
		for (auto& member : m_Members)
		{
			SetMemberRemainTimes(member.id, 0);
			ChangeMemberAssistStatus(member.id, false);
		}
	}

	ControlKickMaster(teamDungeonData->autoKick);

	return ErrorCode::SUCCESS;
}

void WSTeam::AddMember(WSPlayer* player, bool sendNotify, bool cancelMatch)
{
	if(player == NULL) return;

    if (FindMember(player->GetID()) != NULL)
    {
        ErrorLogStream << "player(" << player->GetID() << ") is already in team(" << GetId() << ")." << LogStream::eos;
        return;
    }

	InfoLogStream << "player(" << player->GetID() << ") join team(" << GetId() << ")." << LogStream::eos;

	if (cancelMatch)
	{
		TeamMatchMgr::Instance()->OnPlayerCancelMatch(player);
	}

	WSTeamMember member;
	member.enterTime = UInt32(CURRENT_TIME.Sec());
	member.SetPlayer(player);

	//通知其他人新成员加入
	CLProtocol::WorldNotifyNewTeamMember notify;
	notify.info = member.GetInfo();
	Broadcast(notify);

	m_Members.push_back(member);
	player->SetTeam(this);

	//通知新成员进入到场景
	CLProtocol::WorldNotifyMemberInToScene notifyScene;
	notifyScene.id = GetId();
	notifyScene.memberId = player->GetID();
	BroadcastToScene(notifyScene);

	//更新场景队伍信息
	UpdateSceneInfo();

	//同步队伍信息给新成员
	CLProtocol::WorldSyncTeamInfo sync;
	FillTeamInfo(sync);
	player->SendProtocol(sync);

	// 有玩家加入，取消之前的投票
	OnStartRaceVoteFailure(0, 3123);

	// 不管是不是通过邀请加入的，从邀请列表中删除
	DelInvitePlayer(player->GetID());

	// 获取地下城剩余次数
	QueryRemainTimes(player->GetID(), GetTeamTarget());

	if (WSApplication::Instance()->IsTeamAutoKick())
	{
		BroadcastTeamKick(true);
	}

	if (sendNotify)
	{
		CLProtocol::WorldTeamRequestResultNotify notify;
		notify.agree = 1;
		player->SendProtocol(notify);
	}

	DebugLogStream << "team(" << GetId() << ") add player(" << player->GetID() << ") current size(" << (UInt32)m_Members.size() << ")." << LogStream::eos;
}

void WSTeam::RemoveMember(ObjID_t playerId)
{
    DebugLogStream << "team(" << GetId() << ") player(" << playerId << ") leave team." << LogStream::eos;

	for(MemberVec::iterator iter = m_Members.begin();
		iter != m_Members.end(); ++iter)
	{
		if(iter->id == playerId)
		{
			WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(playerId);
			if(player != NULL)
			{
				player->SetTeam(NULL);
			}

			//通知成员离开
			CLProtocol::WorldNotifyMemberLeave notify;
			notify.id = playerId;
			Broadcast(notify);

			m_Members.erase(iter);
			UpdateSceneInfo();

			//通知场景服务器玩家离开队伍
			CLProtocol::WorldNotifyMemberOutToScene notifyScene;
			notifyScene.id = GetId();
			notifyScene.memberId = playerId;
			BroadcastToScene(notifyScene);

			//队长离开重新选择队长
			if(GetMaster() == playerId)
			{
				JudgeMaster();
			}

			// 有玩家离开，取消之前的投票
			OnStartRaceVoteFailure(0, 3123);

			DebugLogStream << "team(" << GetId() << ") remove player(" << playerId << ") current  size(" << (UInt32)m_Members.size() << ")." << LogStream::eos;

			return;
		}
	}
}

void WSTeam::VisitMembers(WSTeamMemberVisitor& visitor)
{
	for(MemberVec::iterator iter = m_Members.begin();
		iter != m_Members.end(); ++iter)
	{
        if (!visitor(&(*iter))) return;
	}
}

std::vector<WSPlayer*> WSTeam::GetAllMembers()
{
	std::vector<WSPlayer*> players;
	for (auto member : m_Members)
	{
		if (member.player)
		{
			players.push_back(member.player);
		}
	}

	return players;
}

TeamBaseInfo WSTeam::GetBaseInfo(UInt64 fromId)
{
    TeamBaseInfo info;
    info.target = GetTeamTarget();
    info.teamId = GetId();
    info.memberNum = (UInt8)m_Members.size();
    info.maxMemberNum = GetMaxTeamMemberNum();

	WSTeamMember* from = NULL;
	if (fromId == 0)
		from = FindMember(GetMaster());
	else
		from = FindMember(fromId);

	if (from != NULL)
	{
		info.masterInfo = from->GetBaseInfo();
	}

    return info;

}

UInt32 WSTeam::RequestJoinTeam(WSPlayer* player, const std::string& passwd)
{
	if (!player)
	{
		return ErrorCode::TEAM_ERROR;
	}

	if (IsFull())	//队伍已满
	{
		return ErrorCode::TEAM_JOIN_TEAM_FULL;
	}

	auto targetData = TeamDungeonDataEntryMgr::Instance()->FindEntry(GetTeamTarget());
	if (targetData && targetData->minLevel > player->GetLevel())
	{
		return ErrorCode::TEAM_JOIN_LEVEL_LIMIT;
	}

	// 队伍在战斗中
	if (GetStatus() != TEAM_STATUS_IDEL)
	{
		return ErrorCode::TEAM_JOIN_RACING;
	}

	if (player->GetTeam())
	{
		return ErrorCode::TEAM_JOIN_TEAM_HAS_TEAM;
	}

	return ErrorCode::SUCCESS;
}

bool WSTeam::Dismiss(WSPlayer* player)
{
	if(player == NULL || m_Master != player->GetID()) return false;

	Dismiss();
	return true;
}

void WSTeam::Dismiss()
{
	InfoLogStream << "team(" << GetId() << ") dismiss..." << LogStream::eos;

	//挨个通知成员离开
	CLProtocol::WorldNotifyMemberLeave notify;
	for (MemberVec::iterator iter = m_Members.begin();
		iter != m_Members.end(); ++iter)
	{
		if (iter->player != NULL)
		{
			notify.id = iter->id;
			iter->player->SetTeam(NULL);
			iter->player->SendProtocol(notify);
		}
	}

	m_Members.clear();

	UpdateSceneInfo();

	m_Master = 0;
	m_DismissTime = UInt32(CURRENT_TIME.Sec()) + EMPTY_DISMISS_WAITTIME;
	InfoLogStream << "team(" << GetId() << ") set dismiss time(" << m_DismissTime << ")." << LogStream::eos;
}

void WSTeam::AddRequester(WSPlayer* player)
{
	if (!player)
	{
		return;
	}

	for (const auto& requester : m_Requesters)
	{
		if (requester.id == player->GetID())
		{
			return;
		}
	}

	TeamRequester requester;
	requester.id = player->GetID();
	requester.level = player->GetLevel();
	requester.occu = player->GetOccu();
	requester.name = player->GetName();
	player->GetPlayerLabelInfo(requester.playerLabelInfo);
	m_Requesters.push_back(requester);

	auto master = WSPlayerMgr::Instance()->FindNormalPlayer(GetMaster());
	if (master)
	{
		CLProtocol::WorldTeamNotifyNewRequester notify;
		master->SendProtocol(notify);
	}
}

TeamRequester* WSTeam::FindRequester(ObjID_t id)
{
	for (auto& requester : m_Requesters)
	{
		if (requester.id == id)
		{
			return &requester;
		}
	}

	return NULL;
}

void WSTeam::DelRequester(ObjID_t id)
{
	for (UInt32 i = 0; i < m_Requesters.size(); i++)
	{
		if (m_Requesters[i].id == id)
		{
			m_Requesters.erase(m_Requesters.begin() + i);
			return;
		}
	}
}

void WSTeam::ClearRequesterList(WSPlayer* player)
{
	if (!player || player->GetID() != GetMaster())
	{
		return;
	}

	m_Requesters.clear();

	CLProtocol::WorldTeamProcessRequesterRes res;
	res.targetId = 0;
	res.result = ErrorCode::SUCCESS;
	player->SendProtocol(res);
}

void WSTeam::ReplyJoinRequest(WSPlayer* player, ObjID_t targetId, bool agree)
{
	CLProtocol::WorldTeamProcessRequesterRes res;
	res.targetId = targetId;
	if (!player || player->GetID() != GetMaster())
	{
		return;
	}

	TeamRequester* requester = FindRequester(targetId);
	if (!requester)
	{
		res.result = ErrorCode::TEAM_REPLY_PLAYER_INVALID;
		player->SendProtocol(res);
		return;
	}

	if (agree)
	{
		auto targetPlayer = WSPlayerMgr::Instance()->FindNormalPlayer(targetId);
		if (!targetPlayer)
		{
			res.result = ErrorCode::TEAM_REPLY_PLAYER_OFFLINE;
			player->SendProtocol(res);
			return;
		}

		if (targetPlayer->GetTeam())
		{
			res.result = ErrorCode::TEAM_INVITE_TARGET_IN_TEAM;
			player->SendProtocol(res);
			return;
		}

		if (!targetPlayer->IsSceneCanJoinTeam())
		{
			res.result = ErrorCode::TEAM_INVITE_TARGET_BUSY;
			player->SendProtocol(res);
			return;
		}

		if (IsFull())
		{
			res.result = ErrorCode::TEAM_REPLY_TEAM_FULL;
			player->SendProtocol(res);
			return;
		}

		if (DungeonRaceMgr::Instance()->IsPlayerInRace(targetPlayer->GetID()))
		{
			res.result = ErrorCode::TEAM_INVITE_TARGET_BUSY;
			player->SendProtocol(res);
			return;
		}

		// 加入队伍
		AddMember(targetPlayer, true);
	}
	else
	{
		auto targetPlayer = WSPlayerMgr::Instance()->FindNormalPlayer(targetId);
		if (targetPlayer)
		{
			// 发送结果给目标玩家
			CLProtocol::WorldTeamRequestResultNotify notify;
			notify.agree = 0;
			targetPlayer->SendProtocol(notify);
		}
	}

	DelRequester(targetId);

	res.result = ErrorCode::SUCCESS;
	player->SendProtocol(res);
}

void WSTeam::AddInvitePlayer(WSPlayer* player)
{
	if (!player)
	{
		return;
	}

	for (auto& info : m_InviteList)
	{
		if (info.roleId == player->GetID())
		{
			info.endTime = (UInt32)CURRENT_TIME.Sec() + TEAM_INVITE_VALID_TIME;
			return;
		}
	}

	TeamInviteInfo info;
	info.roleId = player->GetID();
	info.endTime = (UInt32)CURRENT_TIME.Sec() + TEAM_INVITE_VALID_TIME;
	m_InviteList.push_back(info);
}

void WSTeam::DelInvitePlayer(ObjID_t id)
{
	for (UInt32 i = 0; i < m_InviteList.size(); i++)
	{
		if (m_InviteList[i].roleId == id)
		{
			m_InviteList.erase(m_InviteList.begin() + i);
			return;
		}
	}
}

bool WSTeam::IsInInviteList(ObjID_t id)
{
	for (UInt32 i = 0; i < m_InviteList.size(); i++)
	{
		if (m_InviteList[i].roleId == id)
		{
			UInt32 curSec = (UInt32)CURRENT_TIME.Sec();
			if (curSec > m_InviteList[i].endTime)
			{
				m_InviteList.erase(m_InviteList.begin() + i);
				return false;
			}
			return true;
		}
	}

	return false;
}

void WSTeam::ClearInviteList(bool notify)
{
	if (notify)
	{
		CLProtocol::WorldTeamInviteClearNotify notify;
		notify.teamId = GetId();
		BroadcastToInvitePlayer(notify);
	}

	m_InviteList.clear();
}

void WSTeam::SyncTeamAttrToInvitePlayer()
{
	CLProtocol::WorldTeamInviteSyncAttr sync;
	sync.teamId = GetId();
	sync.target = GetTeamTarget();
	BroadcastToInvitePlayer(sync);
}

void WSTeam::BroadcastToInvitePlayer(Avalon::Protocol& protocol)
{
	auto packet = Avalon::Packet::Create();
	if (!protocol.Encode(packet))
	{
		Avalon::Packet::Destroy(packet);
		return;
	}

	for (auto& invite : m_InviteList)
	{
		auto player = WSPlayerMgr::Instance()->FindNormalPlayer(invite.roleId);
		if (!player)
		{
			continue;
		}

		player->SendPacket(packet);
	}

	Avalon::Packet::Destroy(packet);
}

void WSTeam::BroadcastToScene(Avalon::Protocol& protocol)
{
	for(std::map<UInt32, WSPlayer*>::iterator iter = m_ScenePlayers.begin();
		iter != m_ScenePlayers.end(); ++iter)
	{
		iter->second->SendToScene(protocol);
	}
}

void WSTeam::Broadcast(Avalon::Protocol& protocol)
{
	for(MemberVec::iterator iter = m_Members.begin();
		iter != m_Members.end(); ++iter)
	{
		if(iter->player != NULL)
		{
			iter->player->SendProtocol(protocol);
		}
	}
}

void WSTeam::BroadcastExceptMaster(Avalon::Protocol& protocol)
{
	for (MemberVec::iterator iter = m_Members.begin();
		iter != m_Members.end(); ++iter)
	{
		if (iter->player != NULL && iter->player->GetID() != GetMaster())
		{
			iter->player->SendProtocol(protocol);
		}
	}
}

void WSTeam::Notify(UInt16 type, UInt8 color, const char* word)
{
	CLProtocol::SysNotify notify;
	notify.type = type;
	notify.color = color;
	notify.channel = 0;
	notify.word = word;

	Broadcast(notify);
}

void WSTeam::BroadcastMemberStatus(ObjID_t id)
{
	auto member = FindMember(id);
	if (!member)
	{
		return;
	}

	{
		CLProtocol::WorldSyncTeamMemberProperty sync;
		sync.memberId = id;
		sync.type = TEAM_MEMBER_PROPERTY_STATUS_MASK;
		sync.value = member->GetStatusMask();
		Broadcast(sync);
	}

	{
		CLProtocol::WorldSyncTeamMemberStatus notify;
		notify.id = id;
		notify.statusMask = member->GetStatusMask();
		Broadcast(notify);
	}
}

bool WSTeam::OnPlayerOnline(WSPlayer* player)
{
	if(player == NULL) return false;

	WSTeamMember* member = FindMember(player->GetID());
	if(member == NULL) return false;

	

	member->SetPlayer(player);
	player->SetTeam(this);
	if(m_DismissTime !=0)
	{
		m_Master = player->GetID();
		m_DismissTime = 0;
	}
	
	//通知玩家上线
	BroadcastMemberStatus(player->GetID());
			
	//上线将队伍数据发给玩家
	CLProtocol::WorldSyncTeamInfo sync;
	FillTeamInfo(sync);
	player->SendProtocol(sync);

	//判断更新场景信息
	UpdateSceneInfo();
	return true;
}

void WSTeam::OnPlayerOffline(WSPlayer* player)
{
	if(player == NULL) return;

    DebugLogStream << "team(" << GetId() << ") player(" << player->GetID() << ") offline." << LogStream::eos;

	WSTeamMember* member = FindMember(player->GetID());
	if(member == NULL) return;

	RemoveMember(player->GetID());

	/*member->SetPlayer(NULL);
	player->SetTeam(NULL);
	UpdateSceneInfo();

	//通知玩家下线
    CLProtocol::WorldSyncTeamMemberStatus notify;
    notify.id = player->GetID();
    notify.statusMask = member->GetStatusMask();
	Broadcast(notify);

	if(GetMaster() == player->GetID())
	{
		JudgeMaster();
	}*/
}

void WSTeam::OnSceneServerChanged(WSPlayer* player)
{
	if(player == NULL) return;

	WSTeamMember* member = FindMember(player->GetID());
	if(member == NULL) return;

	UpdateSceneInfo();
}

void WSTeam::OnPlayerLevelChanged(WSPlayer* player)
{
	if(player == NULL) return;

	WSTeamMember* member = FindMember(player->GetID());
	if(member == NULL) return;

	member->level = player->GetLevel();

	// 同步到客户端
	CLProtocol::WorldSyncTeamMemberProperty sync;
	sync.memberId = player->GetID();
	sync.type = TEAM_MEMBER_PROPERTY_LEVEL;
	sync.value = member->level;
	Broadcast(sync);
}

void WSTeam::OnPlayerOccuChanged(WSPlayer* player)
{
	if (player == NULL) return;

	WSTeamMember* member = FindMember(player->GetID());
	if (member == NULL) return;

	member->occu = player->GetOccu();

	// 同步到客户端
	CLProtocol::WorldSyncTeamMemberProperty sync;
	sync.memberId = player->GetID();
	sync.type = TEAM_MEMBER_PROPERTY_OCCU;
	sync.value = member->occu;
	sync.playerLabelInfo = member->playerLabelInfo;
	Broadcast(sync);
}

void WSTeam::OnPlayerVipLevelChanged(WSPlayer* player)
{
	if (player == NULL) return;

	WSTeamMember* member = FindMember(player->GetID());
	if (member == NULL) return;

	member->vipLevel = player->GetVipLvl();

	// 同步到客户端
	CLProtocol::WorldSyncTeamMemberProperty sync;
	sync.memberId = player->GetID();
	sync.type = TEAM_MEMBER_PROPERTY_VIP_LEVEL;
	sync.value = member->vipLevel;
	Broadcast(sync);
}

void WSTeam::OnDismissed()
{
	ClearInviteList();
	for (auto& member : m_Members)
	{
		auto player = member.player;
		if (player)
		{
			player->SetTeam(NULL);
		}
	}
}

void WSTeam::OnDayChanged()
{
	if (IsTargetTimesLimit())
	{
		QueryAllMemberRemainTimes(GetTeamTarget());
	}
}

void WSTeam::UpdateSceneInfo()
{
	std::set<UInt32> oldScenes;
	for(ScenePlayerMap::iterator iter = m_ScenePlayers.begin();
		iter != m_ScenePlayers.end(); ++iter)
	{
		oldScenes.insert(iter->first);
	}
	m_ScenePlayers.clear();
	ScenePlayerMap newScenePlayers;

	for(MemberVec::iterator iter = m_Members.begin();
		iter != m_Members.end(); ++iter)
	{
		WSPlayer* player = iter->player;
		if(player != NULL)
		{
			m_ScenePlayers.insert(std::make_pair(player->GetSceneNode(), player));
		}
	}

	for(ScenePlayerMap::iterator iter = m_ScenePlayers.begin();
		iter != m_ScenePlayers.end(); ++iter)
	{
		std::set<UInt32>::iterator oldIter = oldScenes.find(iter->first);
		if(oldIter != oldScenes.end())
		{
			oldScenes.erase(oldIter);
		}
		else
		{
			newScenePlayers.insert(std::make_pair(iter->first, iter->second));
		}
	}

	if(!newScenePlayers.empty())
	{
		CLProtocol::WorldSyncTeamInfoToScene sync;
		FillSceneTeamInfo(sync);
		for(ScenePlayerMap::iterator iter = newScenePlayers.begin();
			iter != newScenePlayers.end(); ++iter)
		{
			iter->second->SendToScene(sync);
		}
	}

	if(!oldScenes.empty())
	{
		CLProtocol::WorldNotifyDelTeamToScene notify;
		notify.id = GetId();
		for(std::set<UInt32>::iterator iter = oldScenes.begin();
			iter != oldScenes.end(); ++iter)
		{
			Avalon::NetConnectionPtr conn = WSNetwork::Instance()->FindConnection(*iter);
			if(conn != NULL)
			{
				CL_SEND_PROTOCOL(conn, notify);
			}
		}
	}
}

void WSTeam::SyncTeamInfo()
{
	CLProtocol::WorldSyncTeamInfo sync;
	FillTeamInfo(sync);
	Broadcast(sync);
}

void WSTeam::SyncTeamBaseToScene()
{
	CLProtocol::WorldSyncTeamBaseToScene sync;
	sync.id = GetId();
	sync.master = m_Master;
	BroadcastToScene(sync);
}

void WSTeam::BroadcastTeamKick(bool status)
{
	CLProtocol::WorldNotifyTeamKick msg;
	if (!status || m_KickMasterTimer.GetLastTime() == 0)
	{
		msg.endTime = 0;
	}
	else
	{
		msg.endTime = m_KickMasterTimer.GetLastTime() + SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_CAPTAIN_LEAVE_TEAM_TIME) * Avalon::Time::MSECS_OF_SEC;
	}
	Broadcast(msg);
}

void WSTeam::FillTeamInfo(CLProtocol::WorldSyncTeamInfo& sync)
{
	sync.master = m_Master;
	sync.id = GetNumber();
    sync.target = m_target;
	sync.autoAgree = IsAutoAgree() ? 1 : 0;
	sync.options = m_OptionMask;
	for(MemberVec::iterator iter = m_Members.begin();
		iter != m_Members.end(); ++iter)
	{
		TeammemberInfo info = iter->GetInfo();
		sync.members.push_back(info);
	}
}

void WSTeam::FillSceneTeamInfo(CLProtocol::WorldSyncTeamInfoToScene& sync)
{
	sync.id = GetId();
	sync.master = m_Master;
	for(MemberVec::iterator iter = m_Members.begin();
		iter != m_Members.end(); ++iter)
	{
        SceneTeamMember member;
        member.roleId = iter->id;
        member.statusMask = iter->GetStatusMask();
		sync.members.push_back(member);
	}
}

void WSTeam::JudgeMaster()
{
	UInt32	enterTime = UInt32(-1);
	ObjID_t oldMaster = m_Master;
	for(MemberVec::iterator iter = m_Members.begin();
		iter != m_Members.end(); ++iter)
	{
		if(iter->player != NULL)
		{
			if(iter->enterTime < enterTime)
			{
				m_Master = iter->id;
				enterTime = iter->enterTime;
			}
		}
	}

	if(m_Master != oldMaster)
	{
		CLProtocol::WorldSyncTeammaster sync;
		sync.master = m_Master;
		Broadcast(sync);
		ResetKickMasterTimer();
		SyncTeamBaseToScene();
	}
	else
	{
		m_DismissTime = UInt32(CURRENT_TIME.Sec()) + EMPTY_DISMISS_WAITTIME;
		InfoLogStream << "team(" << GetId() << ") set dismiss time(" << m_DismissTime << ")." << LogStream::eos;
	}
}

void WSTeam::ResetKickMasterTimer()
{
	m_KickMasterTimer.SetLastTime(0);
	BroadcastTeamKick(false);
}

void WSTeam::ControlKickMaster(bool bo)
{
	if (!bo)
	{
		ResetKickMasterTimer();
	}
	m_KickMasterControl = bo;
}

void WSTeam::QueryRemainTimes(ObjID_t roleId, UInt32 targetId)
{
	auto member = FindMember(roleId);
	if (member && member->player)
	{
		CLProtocol::WorldTeamQueryTargetCondReq req;
		req.roleId = roleId;
		req.teamTargetId = targetId;

		member->player->SendToScene(req);
	}
}

void WSTeam::QueryAllMemberRemainTimes(UInt32 targetId)
{
	for (auto& member : m_Members)
	{
		QueryRemainTimes(member.id, targetId);
	}
}

void WSTeam::UpdateAvatar(ObjID_t id, const PlayerAvatar& avatar)
{
	auto member = this->FindMember(id);
	if (!member)
	{
		return;
	}

	if (member->avatar == avatar)
	{
		return;
	}

	member->avatar = avatar;

	// 同步到客户端
	CLProtocol::WorldSyncTeammemberAvatar sync;
	sync.memberId = id;
	sync.avatar = avatar;
	Broadcast(sync);
}

void WSTeam::ChangeMemberAssistStatus(ObjID_t id, bool isAssist, bool modifyByPlayer)
{
	auto member = this->FindMember(id);
	if (!member)
	{
		return;
	}

	if (member->isAssist == isAssist)
	{
		return;
	}

	if (!modifyByPlayer)
	{
		return;
	}

	member->isAssist = isAssist;
	member->isAssistByPlayer = modifyByPlayer;

	{
		BroadcastMemberStatus(id);
	}
	
	{
		CLProtocol::WorldSyncTeamMemberStatusToScene sync;
		sync.id = GetId();
		sync.roleId = id;
		sync.statusMask = member->GetStatusMask();
		BroadcastToScene(sync);
	}
}

void WSTeam::ChangeMemberGuildID(ObjID_t id, ObjID_t guildId)
{
	auto member = this->FindMember(id);
	if (!member)
	{
		return;
	}

	if (member->guildId == guildId)
	{
		return;
	}

	member->guildId = guildId;
	
	// 同步到客户端
	CLProtocol::WorldSyncTeamMemberProperty sync;
	sync.memberId = id;
	sync.type = TEAM_MEMBER_PROPERTY_GUIDID;
	sync.value = guildId;
	Broadcast(sync);
}

void WSTeam::OnChangeRemainTimes(ObjID_t id, UInt32 targetId, UInt32 remainTimes)
{
	auto member = this->FindMember(id);
	if (!member)
	{
		return;
	}

	if (targetId != GetTeamTarget())
	{
		return;
	}

	SetMemberRemainTimes(id, remainTimes);
}

void WSTeam::SetMemberRemainTimes(ObjID_t id, UInt32 remainTimes)
{
	auto member = this->FindMember(id);
	if (!member)
	{
		return;
	}

	// 如果此时为0，强制改成助战模式
	if (remainTimes == 0 && IsTargetTimesLimit())
	{
		ChangeMemberAssistStatus(id, true);
	}
	else
	{
		ChangeMemberAssistStatus(id, false);
	}

	if (remainTimes == member->remainTimes)
	{
		return;
	}

	member->remainTimes = remainTimes;

	// 同步到客户端
	CLProtocol::WorldSyncTeamMemberProperty sync;
	sync.memberId = id;
	sync.type = TEAM_MEMBER_PROPERTY_REMAIN_TIMES;
	sync.value = remainTimes;
	Broadcast(sync);
}

void WSTeam::OnChangePlayerRacingStatus(ObjID_t id, bool racing)
{
	auto member = this->FindMember(id);
	if (!member || member->isRacing == racing)
	{
		return;
	}

	member->isRacing = racing;
	BroadcastMemberStatus(id);
}

bool WSTeam::IsTargetTimesLimit()
{
	auto targetData = TeamDungeonDataEntryMgr::Instance()->FindEntry(GetTeamTarget());
	if (targetData && targetData->type == TEAM_CITY_MONSTER)
	{
		return true;
	}

	UInt32 tagetDungeonId = GetTargetDungeonID();
	auto dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(tagetDungeonId);
	if (!dungeonData)
	{
		return false;
	}

	auto data = DungeonTimesDataEntryMgr::Instance()->FindEntry(dungeonData->subType);
	if (data && data->baseTimes > 0)
	{
		return true;
	}
	return false;
}

UInt16 WSTeam::GetMinLevelLimit()
{
	auto targetData = TeamDungeonDataEntryMgr::Instance()->FindEntry(GetTeamTarget());
	if (targetData)
	{
		return targetData->minLevel;
	}

	return 0;
}

UInt8 WSTeam::GetMaxTeamMemberNum() const
{
	return m_maxMemberNum;
}

void WSTeam::OnPlayerResistMageicChanged(WSPlayer* player)
{
	if (player == NULL) return;

	WSTeamMember* member = FindMember(player->GetID());
	if (member == NULL) return;

	member->resistMagic = player->GetResistMagic();

	// 同步到客户端
	CLProtocol::WorldSyncTeamMemberProperty sync;
	sync.memberId = player->GetID();
	sync.type = TEAM_MEMBER_PROPERTY_RESIST_MAGIC;
	sync.value = member->resistMagic;
	Broadcast(sync);
}