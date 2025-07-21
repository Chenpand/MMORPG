#include "BSTeamMgr.h"
#include "BSBattleMgr.h"
#include "BattleSceneMgr.h"
#include "CLErrorCode.h"
#include <CLBattleProtocol.h>

BSTeamMgr::BSTeamMgr()
{
	m_CurrentID = 100;
	m_TickTimer2000ms.SetInterval(2000);
	m_RobotPlayerID = 10000;
}

BSTeamMgr::~BSTeamMgr()
{
	m_PlayerMap.clear();

	for (BSTeamMap::iterator iter = m_Teams.begin(); iter != m_Teams.end(); ++iter)
	{
		CL_SAFE_DELETE(iter->second);
	}
	m_Teams.clear();
}

void BSTeamMgr::OnTick(const Avalon::Time& now)
{
	if (m_TickTimer2000ms.IsTimeout(now))
	{
		CLProtocol::BattleNotifyPrepareInfo notify;
		notify.playerNum = 0;
		notify.totalNum = BSBattleMgr::Instance()->GetPreBattleCount();

		BSTeamMap::iterator iter = m_Teams.begin();
		while (iter != m_Teams.end())
		{
			notify.playerNum += iter->second->GetSize();
			++iter;
		}

		//DebugLogStream << "prepare player nums:" << notify.playerNum << LogStream::eos;

		iter = m_Teams.begin();
		while (iter != m_Teams.end())
		{
			iter->second->NotifyMsgToAll(notify);
			++iter;
		}
	}

	if (m_Teams.size() >= BSBattleMgr::Instance()->GetPreBattleCount())
	{
		UInt32 count = 0;
		BSTeamMap::iterator iter = m_Teams.begin();
		while (iter != m_Teams.end())
		{
			if (count >= BSBattleMgr::Instance()->GetPreBattleCount())
			{
				break;
			}

			iter->second->OnMatch();
			BSBattleMgr::Instance()->AddTeam(iter->second);
			iter = m_Teams.erase(iter);

			++count;
		}

		//InfoLogStream << "team match success." << LogStream::eos;
	}
}

UInt32 BSTeamMgr::CreateTeam(const BSTeamMember& master)
{
	//未开放服务
	UInt32 sceneNodeID = BattleSceneMgr::Instance()->SelectSceneServer();
	if (sceneNodeID <= 0)
	{
		return ErrorCode::SYS_NOT_OPEN;
	}

	if (m_PlayerMap.find(master.m_PlayerId) != m_PlayerMap.end())
	{
		return ErrorCode::TEAM_CREATE_TEAM_HAS_TEAM;
	}

	BSTeam* team = new BSTeam();
	if (!team)
	{
		return ErrorCode::TEAM_ERROR;
	}

	team->SetId(m_CurrentID);
	++m_CurrentID;

	team->AddMember(master);
	AddPlayerMap(master.m_PlayerId, team->GetId());
	m_Teams.insert(std::make_pair(team->GetId(), team));

	return ErrorCode::SUCCESS;
}

UInt32 BSTeamMgr::JoinTeam(UInt32 teamId, const BSTeamMember& member)
{
	if (m_PlayerMap.find(member.m_PlayerId) != m_PlayerMap.end())
	{
		return ErrorCode::TEAM_JOIN_TEAM_HAS_TEAM;
	}

	BSTeamMap::iterator iter = m_Teams.find(teamId);
	 if (iter == m_Teams.end())
	 {
		 return ErrorCode::TEAM_JOIN_TEAM_UNEXIST;
	 }
	 
	 if (iter->second->IsFull())
	 {
		 return ErrorCode::TEAM_JOIN_TEAM_FULL;
	 }

	 iter->second->AddMember(member);
	 AddPlayerMap(member.m_PlayerId, iter->second->GetId());

	 return ErrorCode::SUCCESS;
}

UInt32 BSTeamMgr::LeaveTeam(UInt64 playerId, UInt32 teamId)
{
	BSTeamMap::iterator iter = m_Teams.find(teamId);
	if (iter != m_Teams.end())
	{
		iter->second->DelMember(playerId);
		DelPlayerMap(playerId);

		if (iter->second->GetSize() <= 0)
		{
			CL_SAFE_DELETE(iter->second);
			m_Teams.erase(iter);
		}
	}

	return ErrorCode::SUCCESS;
}

UInt32 BSTeamMgr::DismissTeam(UInt64 playerId, UInt32 teamId)
{
	std::map<UInt64, UInt32>::iterator player_iter = m_PlayerMap.find(playerId);
	if (player_iter == m_PlayerMap.end())
	{
		return ErrorCode::TEAM_ERROR;
	}

	BSTeamMap::iterator iter = m_Teams.find(teamId);
	if (iter == m_Teams.end())
	{
		return ErrorCode::TEAM_ERROR;
	}

	iter->second->Dismiss();
	CL_SAFE_DELETE(iter->second);
	m_Teams.erase(iter);

	return ErrorCode::SUCCESS;
}

UInt32 BSTeamMgr::GetTeamID(UInt64 playerId)
{
	auto iter = m_PlayerMap.find(playerId);
	return iter != m_PlayerMap.end() ? iter->second : 0;
}

void BSTeamMgr::AddPlayerMap(UInt64 playerId, UInt32 teamId)
{
	std::map<UInt64, UInt32>::iterator player_iter = m_PlayerMap.find(playerId);
	if (player_iter != m_PlayerMap.end())
	{
		player_iter->second = teamId;
	}
	else
	{
		m_PlayerMap.insert(std::make_pair(playerId, teamId));
	}
}

void BSTeamMgr::DelPlayerMap(UInt64 playerId)
{
	std::map<UInt64, UInt32>::iterator player_iter = m_PlayerMap.find(playerId);
	if (player_iter != m_PlayerMap.end())
	{
		m_PlayerMap.erase(player_iter);
	}
}

void BSTeamMgr::AddRobot()
{
	//RobotAIHardType aiHard = ROBOT_AI_FISH;
	RobotAttrHardType attrHard = ROBOT_ATTR_EASY;
	Robot* robot = RobotMgr::Instance()->GenRobot(attrHard, 60);
	if (NULL == robot)
	{
		return;
	}

	BSTeamMember member;
	member.m_PlayerId = m_RobotPlayerID;
	member.m_hasRaceInfo = true;
	member.m_robot = true;
	member.hardType = robot->hardType;
	member.m_raceInfo = robot->raceInfo;

	CreateTeam(member);
	++m_RobotPlayerID;
	if (m_RobotPlayerID > 10000000)
	{
		m_RobotPlayerID = 10000;
	}
}

