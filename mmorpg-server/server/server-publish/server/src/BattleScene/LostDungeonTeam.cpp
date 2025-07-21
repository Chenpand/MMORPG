#include "LostDungeonTeam.h"
#include "Player.h"
#include "LostDungeonMgr.h"
#include "PlayerMgr.h"
#include <CLLostDungeonProtocol.h>


LostDungeonTeam::LostDungeonTeam(UInt32 id)
{
	m_Id = id;
	m_challengeMode = LDCM_SINGLE;
	m_Master = 0;
	m_index = 0;
	m_battleState = LDTBS_NORMAL;
}

LostDungeonTeam::~LostDungeonTeam()
{

}

bool LostDungeonTeam::IsEmpty()
{
	return 0 == GetNum();
}

void LostDungeonTeam::AddMember(Player* player, PlayerMapMember& updates, bool isSwtich,  bool notify)
{
	if (!player) return;
	if (FindMember(player->GetID()) != NULL)
	{
		return;
	}
	if (GetNum() >= MAX_MEMBER_SIZE)
	{
		return;
	}
	//找位子
	UInt8 pos = 1;
	bool findPos = false;
	for (; pos <= MAX_MEMBER_SIZE; ++pos)
	{
		auto it = m_Members.find(pos);
		if (it == m_Members.end())
		{
			findPos = true;
			break;
		}
	}
	if (!findPos)
	{
		return;
	}
	LostDungTeamMember member;
	member.roleId = player->GetID();
	member.teamId = GetId();
	member.pos = pos;
	member.name = player->GetName();
	member.level = player->GetLevel();
	member.occu = player->GetOccu();
	member.timeStamp = (UInt32)CURRENT_TIME.Sec();
	m_Members.insert(std::pair<UInt8, LostDungTeamMember>(pos, member));
	player->SetDungChageTeamId(GetId());

	InfoLogStream << "player(" << player->GetID() <<"), join lost dungeon team(" << GetId() << ")" << LogStream::eos;

	if (isSwtich)
	{
		updates[member.roleId] = member;
		return;
	}

	//通知
	if (notify)
	{
		UInt64 exceptId = 0;
		CLProtocol::SceneLostDungeonSyncDungeonTeamInfo notify;
		notify.type = SDETIT_ADD;
		notify.addMember = member;
		exceptId = member.roleId;
		LostDungeonMgr::Instance()->BroadcastMsgToTeamMembersInScene(GetSceneId(), notify, exceptId);
	}
	return;
}

LostDungTeamMember* LostDungeonTeam::FindMember(ObjID_t id)
{
	for (MemberMap::iterator iter = m_Members.begin();
		iter != m_Members.end(); ++iter)
	{
		if (iter->second.roleId == id)	return &(iter->second);
	}
	return NULL;
}

void LostDungeonTeam::RemoveMember(ObjID_t id, PlayerMapMember& updates, bool isSwtich)
{
	LostDungTeamMember* member = FindMember(id);
	if (!member)	return;
	bool isTeamLeader = member->pos == 1;
	
	m_Members.erase(member->pos);

	if (!isSwtich)
	{
		CLProtocol::SceneLostDungeonSyncDungeonTeamInfo notify;
		notify.type = SDETIT_LEAVE;
		notify.leavePlayerId = id;
		LostDungeonMgr::Instance()->BroadcastMsgToTeamMembersInScene(GetSceneId(), notify);
	}

	//是队长，需要移动其他队员位置
	if (isTeamLeader && m_Members.size() > 0)
	{
		CLProtocol::SceneLostDungeonSyncDungeonTeamInfo notify;
		notify.type = SDETIT_UPDATE;
		MemberMap temp = m_Members;
		m_Members.clear();
		for (auto it : temp)
		{
			LostDungTeamMember member = it.second;
			member.pos--;
			if (member.pos > MAX_MEMBER_SIZE || member.pos < 1)
			{
				continue;
			}
			m_Members.insert(std::pair<UInt8, LostDungTeamMember>(member.pos, member));
			notify.teamMemebers.push_back(member);	
			if (isSwtich)
			{
				updates[member.roleId] = member;
			}
		}
		if (!isSwtich)
		{
			LostDungeonMgr::Instance()->BroadcastMsgToTeamMembersInScene(GetSceneId(), notify);
		}
	}
	if (GetNum() == 0)
	{
		SetChallengeMode(LDCM_SINGLE);
	}
}

void LostDungeonTeam::GetInfo(LostDungTeamInfo& info)
{
	info.teamId = m_Id;
	info.index = m_index;
	info.battleState = m_battleState;
}

bool LostDungeonTeam::VistMembers(const std::function<bool(LostDungeonTeam* team, LostDungTeamMember* member)>& ff)
{
	for (MemberMap::iterator iter = m_Members.begin();
		iter != m_Members.end(); ++iter)
	{
		if (!ff(this, &(iter->second)))
		{
			return false;
		}
	}
	return true;
}

bool LostDungeonTeam::SetBattleState(UInt8 st)
{
	if (st > LDTBS_MAX) return false;
	if (st == m_battleState) return false;

	m_battleState = st;
	return true;
}

void LostDungeonTeam::OnRaceEnd()
{
	MemberMap tempMembers = m_Members;
	m_Members.clear();

	for (auto it : tempMembers)
	{
		Player* player = PlayerMgr::Instance()->FindPlayer(it.second.roleId);
		if (player)
		{
			player->SetDungChageMode(LDCM_SINGLE);
			player->SetDungChageTeamId(0);
		}
		
		CLProtocol::SceneLostDungeonSyncDungeonTeamInfo notify;
		notify.type = SDETIT_LEAVE;
		notify.leavePlayerId = it.second.roleId;
		LostDungeonMgr::Instance()->BroadcastMsgToTeamMembersInScene(GetSceneId(), notify);
	}

	bool update = this->SetBattleState(LDTBS_NORMAL);
	if (update)
	{
		CLProtocol::SceneLostDungeonSyncDungeonTeamInfo syncTeam;
		syncTeam.type = SDETIT_TEAM_STATE;
		LostDungTeamInfo teamInfo;
		this->GetInfo(teamInfo);
		syncTeam.teamInfos.push_back(teamInfo);

		LostDungeonMgr::Instance()->BroadcastMsgToTeamMembersInScene(this->GetSceneId(), syncTeam);
	}
}