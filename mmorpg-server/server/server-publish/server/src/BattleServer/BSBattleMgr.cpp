#include "BSBattleMgr.h"
#include "CLErrorCode.h"
#include "BSNetwork.h"
#include "GameZoneMgr.h"
#include "BattleSceneMgr.h"
#include "RelaySessionMgr.h"
#include "BSSortListMgr.h"
#include <CLMatchProtocol.h>
#include <CLGameSessionProtocol.h>
#include <CLRelayServerProtocol.h>
#include "BSApplication.h"

BSBattleMgr::BSBattleMgr()
{
	m_TickTimer.SetInterval(500);
	m_PreBattleCount = 0;
	m_BattleTimeOut = 0;
	m_StartTime = 0;
	m_EndTime = 0;
	m_AwardTime = 0;
}

BSBattleMgr::~BSBattleMgr()
{
	std::map<UInt32, BSBattle*>::iterator iter = m_Battles.begin();
	while (iter != m_Battles.end())
	{
		BSBattle* battle = iter->second;
		++iter;
		if (battle == NULL) continue;
		CL_SAFE_DELETE(battle);
	}
	m_Battles.clear();

	for (BSTeamMap::iterator iter = m_WaitTeams.begin(); iter != m_WaitTeams.end(); ++iter)
	{
		CL_SAFE_DELETE(iter->second);
	}
	m_WaitTeams.clear();

	for (std::map<UInt64, BattleRace*>::iterator iter = m_BattleRaces.begin(); iter != m_BattleRaces.end(); ++iter)
	{
		CL_SAFE_DELETE(iter->second);
	}
	m_BattleRaces.clear();

	m_PlayerBattleVecMap.clear();
}

UInt64 BSBattleMgr::GenRaceID()
{
	// 生成唯一的比赛ID,括号内为相应字段的位数
	// NodeID(48-63) +  时间(s)(32-67) + 随机数(0-32)
	Avalon::Time now = Avalon::Time::CurrentTime();
	UInt64 randNum = (UInt64)rand();
	UInt64 raceID = ((UInt64(ZONE_ID) << 47) + (randNum << 31) + now.Sec());
	return raceID;
}

void BSBattleMgr::Init(UInt32 preBattleCount, UInt32 battleTimeOut)
{
	m_PreBattleCount = preBattleCount ? preBattleCount : 5;
	m_BattleTimeOut = battleTimeOut ? battleTimeOut : 40 * 60;
}

void BSBattleMgr::OnTick(const Avalon::Time& now)
{
	if (!m_TickTimer.IsTimeout(now))
	{
		return;
	}

	std::map<UInt32, BSBattle*>::iterator iter = m_Battles.begin();
	while (iter != m_Battles.end())
	{
		BSBattle* battle = iter->second;
		++iter;
		if (battle != NULL)
		{
			battle->OnTick(now);
		}
	}

	if (m_Battles.size() >= 3000)
	{
		return;
	}

	//匹配，创建战场
	while (m_WaitTeams.size() >= GetPreBattleCount())
	{
		if (!MatchChiji())
		{
			break;
		}
	}

	if (now.Sec() >= m_AwardTime && m_AwardTime != 0)
	{
		m_AwardTime = 0;
		// 发奖并清理数据
		BSSortListMgr::Instance()->ClearPlayerBestRank();
	}
}

void BSBattleMgr::AddTeam(BSTeam* team)
{
	if (!team)
	{
		return;
	}

	m_WaitTeams.insert(std::make_pair(team->GetId(), team));
}

void BSBattleMgr::DelTeam(UInt32 teamID)
{
	BSTeamMap::iterator iter = m_WaitTeams.find(teamID);
	if (iter != m_WaitTeams.end())
	{
		CL_SAFE_DELETE(iter->second);
		m_WaitTeams.erase(iter);
	}
}

void BSBattleMgr::DelBattle(BSBattle* battle)
{
	std::map<UInt32, BSBattle*>::iterator iter = m_Battles.find(battle->GetId());
	if (iter != m_Battles.end())
	{
		m_Battles.erase(iter);
	}

	CL_SAFE_DELETE(battle);
}

BSBattle* BSBattleMgr::GetBattle(UInt32 battleID)
{
	std::map<UInt32, BSBattle*>::iterator iter = m_Battles.find(battleID);
	if (iter != m_Battles.end())
	{
		return iter->second;
	}

	return NULL;
}

void BSBattleMgr::DelBattleRace(BattleRace* race)
{
	std::map<UInt64, BattleRace*>::iterator iter = m_BattleRaces.find(race->GetRaceID());
	if (iter != m_BattleRaces.end())
	{
		m_BattleRaces.erase(iter);
	}

	CL_SAFE_DELETE(race);
}

BattleRace* BSBattleMgr::GetBattleRace(UInt64 raceID)
{
	std::map<UInt64, BattleRace*>::iterator iter = m_BattleRaces.find(raceID);
	if (iter != m_BattleRaces.end())
	{
		return iter->second;
	}

	return NULL;
}

bool BSBattleMgr::Match()
{
	if (m_WaitTeams.size() < GetPreBattleCount())
	{
		return false;
	}

	UInt32 matched = 0;
	std::list<UInt32> teamOneMemberList;
	std::list<UInt32> teamTwoMemberList;
	std::list<UInt32> teamTriMemberList;
	for (BSTeamMap::iterator iter = m_WaitTeams.begin(); iter != m_WaitTeams.end(); ++iter)
	{
		if (matched >= GetPreBattleCount())
		{
			break;
		}

		if (iter->second->IsFull())
		{
			teamTriMemberList.push_back(iter->second->GetId());
			++matched;
		}

		if (iter->second->GetSize() == 1)
		{
			teamOneMemberList.push_back(iter->second->GetId());
		}

		if (iter->second->GetSize() == 2)
		{
			teamTwoMemberList.push_back(iter->second->GetId());
		}
	}

	//优先匹配单人和双人的队伍
	if (teamOneMemberList.size() > teamTwoMemberList.size())
	{
		matched = matched + teamTwoMemberList.size() + ((teamOneMemberList.size() - teamTwoMemberList.size()) / MAX_MEMBER_IN_TEAM);
	}
	else
	{
		matched = matched + teamOneMemberList.size();
	}
	
	if (matched < GetPreBattleCount())
	{
		return false;
	}

	UInt32 sceneNodeID = BattleSceneMgr::Instance()->SelectSceneServer();
	if (sceneNodeID <= 0)
	{
		return false;
	}

	BSBattle* battle = new BSBattle();
	if (!battle)
	{
		return false;
	}

	battle->SetSceneNodeID(sceneNodeID);

	UInt32 count = 0;
	std::list<UInt32>::iterator iterH = teamTriMemberList.begin();
	while (iterH != teamTriMemberList.end())
	{
		if (count == GetPreBattleCount())
		{
			battle->SetId(GenBattleID());
			m_Battles.insert(std::make_pair(battle->GetId(), battle));
			return true;
		}

		BSTeamMap::iterator iter = m_WaitTeams.find(*iterH);
		if (iter == m_WaitTeams.end())
		{
			++iterH;
			continue;
		}

		battle->AddTeam(iter->second);
		m_WaitTeams.erase(iter);
		++count;

		iterH = teamTriMemberList.erase(iterH);
	}

	std::list<UInt32>::iterator iterA = teamOneMemberList.begin();
	std::list<UInt32>::iterator iterB = teamTwoMemberList.begin();
	while (iterA != teamOneMemberList.end() && iterB != teamTwoMemberList.end())
	{
		if (count == GetPreBattleCount())
		{
			battle->SetId(GenBattleID());
			m_Battles.insert(std::make_pair(battle->GetId(), battle));
			return true;
		}

		teamOneMemberList.pop_front();
		teamTwoMemberList.pop_front();
		
		BSTeamMap::iterator iter = m_WaitTeams.find(*iterB);
		BSTeamMap::iterator iter1 = m_WaitTeams.find(*iterA);
		if (iter == m_WaitTeams.end() || iter1 == m_WaitTeams.end())
		{
			break;
		}

		const VBSTeamMember& Members = iter1->second->GetTeamMembers();
		for (auto& member : Members)
		{
			iter->second->AddMember(member);
		}

		CL_SAFE_DELETE(iter1->second);
		m_WaitTeams.erase(iter1);

		battle->AddTeam(iter->second);
		m_WaitTeams.erase(iter);
		++count;

		iterA = teamOneMemberList.erase(iterA);
		iterB = teamTwoMemberList.erase(iterB);
	}

	while (teamOneMemberList.size() > MAX_MEMBER_IN_TEAM)
	{
		if (count == GetPreBattleCount())
		{
			battle->SetId(GenBattleID());
			m_Battles.insert(std::make_pair(battle->GetId(), battle));
			return true;
		}

		std::list<UInt32>::iterator iterE = teamOneMemberList.begin();
		teamOneMemberList.pop_front();
		std::list<UInt32>::iterator iterF = teamOneMemberList.begin();
		teamOneMemberList.pop_front();
		std::list<UInt32>::iterator iterG = teamOneMemberList.begin();
		teamOneMemberList.pop_front();

		BSTeamMap::iterator iter = m_WaitTeams.find(*iterE);
		BSTeamMap::iterator iter1 = m_WaitTeams.find(*iterF);
		BSTeamMap::iterator iter2 = m_WaitTeams.find(*iterG);
		if (iter == m_WaitTeams.end() || iter1 == m_WaitTeams.end() || iter2 == m_WaitTeams.end())
		{
			break;
		}

		const VBSTeamMember& Members1 = iter1->second->GetTeamMembers();
		for (auto& member : Members1)
		{
			iter->second->AddMember(member);
		}

		const VBSTeamMember& Members2 = iter2->second->GetTeamMembers();
		for (auto& member : Members2)
		{
			iter->second->AddMember(member);
		}

		CL_SAFE_DELETE(iter1->second);
		m_WaitTeams.erase(iter1);
		CL_SAFE_DELETE(iter2->second);
		m_WaitTeams.erase(iter2);

		battle->AddTeam(iter->second);
		m_WaitTeams.erase(iter);
		++count;
	}

	CL_SAFE_DELETE(battle);

	return false;
}

void BSBattleMgr::ClearWaitTeams()
{
	for (BSTeamMap::iterator iter = m_WaitTeams.begin(); iter != m_WaitTeams.end(); ++iter)
	{
		CL_SAFE_DELETE(iter->second);
	}
	m_WaitTeams.clear();
}

bool BSBattleMgr::MatchChiji()
{
	UInt32 sceneNodeID = BattleSceneMgr::Instance()->SelectSceneServer();
	if (sceneNodeID <= 0)
	{
		ErrorLogStream << "no free battle scene server." << LogStream::eos;
		ClearWaitTeams();
		return false;
	}

	BSBattle* battle = new BSBattle();
	if (!battle)
	{
		ErrorLogStream << "no more free memory." << LogStream::eos;
		ClearWaitTeams();
		return false;
	}

	battle->SetSceneNodeID(sceneNodeID);
	battle->SetId(GenBattleID());
	m_Battles.insert(std::make_pair(battle->GetId(), battle));

	UInt32 count = 0;
	BSTeamMap::iterator iter = m_WaitTeams.begin();
	for (; iter != m_WaitTeams.end();)
	{
		if (count >= BSBattleMgr::Instance()->GetPreBattleCount())
		{
			break;
		}

		battle->AddTeam(iter->second);
		iter = m_WaitTeams.erase(iter);

		++count;
	}

	DebugLogStream << "battle(" << battle->GetId() << ") match success" << LogStream::eos;

	// 通知场景服建立索引
	battle->NotifyCreateSceneBattle();
	// 通知客户端完成匹配，可以切入战场场景
	//battle->NotifySwitchBattleScene();

	return true;
}

void BSBattleMgr::RegistBattleRace(BattleRace* race)
{
	std::map<UInt64, BattleRace*>::iterator iter = m_BattleRaces.find(race->GetRaceID());
	if (iter != m_BattleRaces.end())
	{
		CL_SAFE_DELETE(iter->second);
		iter->second = race;
	}
	else
	{
		m_BattleRaces[race->GetRaceID()] = race;
	}
}

bool BSBattleMgr::OnMatchSuccess(UInt32 battleId, UInt32 dungeonID, VBSTeam teams)
{
	BSBattle* battle = GetBattle(battleId);
	if (!battle)
	{
		ErrorLogStream << "can't find battle : " << battleId << LogStream::eos;
		return false;
	}

	RelaySession* relaySession = RelaySessionMgr::Instance()->SelectRelayServer();
	if (!relaySession)
	{
		ErrorLogStream << "no free relay server." << LogStream::eos;
		return false;
	}

	BattleRace* race = new BattleRace(battleId, RT_CHIJI);
	if (!race)
	{
		ErrorLogStream << "no more free memory." << LogStream::eos;
		return false;
	}

	race->SetRaceID(GenRaceID());
	race->SetDungeonID(dungeonID);
	race->OnCreate(teams);
	race->SetRelayServerID(relaySession->GetNodeID());

	RegistBattleRace(race);

	auto members = race->GetAllMember();

	CLProtocol::MatchServerQueryPlayerInfoReq req;
	req.type = MATCH_QUERY_RACE_INFO;
	req.sessionId = race->GetRaceID();
	for (auto member : members)
	{
		if (member.BeRobot())
		{
			continue;
		}

		req.roleId = member.m_PlayerId;
		BSNetwork::Instance()->SendToBScene(battle->GetSceneNodeID(), req);
	}

	return true;
}

void BSBattleMgr::OnQueryPlayerRaceInfoRes(UInt64 gamesessionID, UInt64 playerId, UInt8 result, RacePlayerInfo& info)
{
	BattleRace* race = GetBattleRace(gamesessionID);
	if (!race)
	{
		ErrorLogStream << "player(" << playerId << ") can't find match result (" << gamesessionID << ")." << LogStream::eos;
		return;
	}

	DebugLogStream << "player(" << playerId << ") query info result (" << result << ") gamesessionID (" << gamesessionID << ")." << LogStream::eos;

	// 获取玩家信息失败
	if (result != 0)
	{
		ErrorLogStream << "player(" << playerId << ") is offline, match result (" << gamesessionID << ")." << LogStream::eos;
		return;
	}

	race->SetRaceInfo(playerId, info);
	if (!race->IsAllPlayerHasRaceInfo())
	{
		return;
	}

	race->RequestRelayServerStart();
}

void BSBattleMgr::OnRelayServerStartResponse(UInt32 result, UInt64 gamesessionID, UInt32 abality, const Avalon::SockAddr& address)
{
	BattleRace* race = GetBattleRace(gamesessionID);
	if (!race)
	{
		return;
	}

	BSBattle* battle = GetBattle(race->GetBattleID());
	if (!battle)
	{
		return;
	}

	DebugLogStream << "race (" << gamesessionID << ") recv start response, result(" << result << ") abality(" << abality << ")." << LogStream::eos;

	if (result != 0)
	{
		ErrorLogStream << "relay server return error(" << result << "), match(" << gamesessionID << ")." << LogStream::eos;
		return;
	}

	race->SetState(BRS_RACE);
	race->SetRaceAddr(address);

	// 更新RelayServer负载
	RelaySessionMgr::Instance()->UpdateRelayServerAbality(race->GetRelayServerID(), abality);

	CLProtocol::WorldNotifyRaceStart raceStartInfo;
	raceStartInfo.raceType = RT_CHIJI;
	raceStartInfo.session = gamesessionID;
	raceStartInfo.addr = address;
	raceStartInfo.dungeonID = race->GetDungeonID();
	raceStartInfo.battleFlag = BSApplication::Instance()->GetBattleFlag();

	CLProtocol::MatchSvrNotifyRacePlayerInfo notify;
	notify.gamesessionID = gamesessionID;
	notify.pkDungeonId = raceStartInfo.dungeonID;
	notify.battleFlag = raceStartInfo.battleFlag;
	UInt8 pos = 0;

	auto members = race->GetAllMember();
	for (auto member : members)
	{
		member.m_raceInfo.pos = pos++;
		raceStartInfo.players.push_back(member.m_raceInfo);

		notify.pkType = PK_1V1_CHIJI;
		notify.players.push_back(member.m_raceInfo);
	}

	for (auto member : members)
	{
		if (member.BeRobot())
		{
			continue;
		}

		//切换状态
		race->SetPlayerStateStatus(member.m_PlayerId, BPS_PK);
		race->SetPlayerRaceID(member.m_PlayerId, gamesessionID);

		raceStartInfo.roleId = member.m_PlayerId;
		auto zone = GameZoneMgr::Instance()->FindGameZone(member.m_ConnId);
		if (!zone)
		{
			continue;
		}

		zone->SendToPlayer(raceStartInfo.roleId, raceStartInfo);
	}

	race->SendToRelay(notify);
	//BSNetwork::Instance()->SendToRelay(notify);

	// 战斗初始包
	{
		CLProtocol::RelaySvrRaceInitPacket init;
		init.raceId = gamesessionID;
		if (raceStartInfo.Encode(init.packet))
		{
			race->SendToRelay(init);
			//BSNetwork::Instance()->SendToRelay(init);
		}
	}

	CLProtocol::WorldMatchRaceStart req;
	for (auto member : members)
	{
		//DebugLogStream << "player:" << member.m_PlayerId << LogStream::eos;

		req.roleId = member.m_PlayerId;

		//DebugLogStream << "race(" << battle->GetId() << ") send race start to player(" << req.roleId << ") in node(" << battle->GetSceneNodeID() << ")." << LogStream::eos;

		BSNetwork::Instance()->SendToBScene(battle->GetSceneNodeID(), req);
	}
}

void BSBattleMgr::OnRaceEnd(ObjID_t gamesessionID, EGameSessionDismissType ret, const PkRaceEndInfo& raceEndInfo)
{
	DebugLogStream << "race : " << gamesessionID << ", ret : " << (UInt32)ret << LogStream::eos;
	std::map<UInt64, BattleRace*>::iterator iter = m_BattleRaces.find(gamesessionID);
	if (iter == m_BattleRaces.end())
	{
		DebugLogStream << "can't find race : " << gamesessionID << LogStream::eos;
		return;
	}

	iter->second->OnRaceEnd(ret, raceEndInfo);

	CL_SAFE_DELETE(iter->second);
	m_BattleRaces.erase(iter);
}

void  BSBattleMgr::OnSomeoneDead(UInt32 battleID, UInt64 playerID, UInt64 killerID, UInt32 reason)
{
	DebugLogStream << "battle:" << battleID << ", player:" << playerID << LogStream::eos;

	std::map<UInt32, BSBattle*>::iterator iter = m_Battles.find(battleID);
	if (iter != m_Battles.end())
	{
		BSBattle* battle = iter->second;
		battle->OnSomeoneDead(playerID, killerID, reason);
	}
}

UInt32 BSBattleMgr::PkSomeOne(UInt32 battleID, UInt32 dungeonID, UInt64 roleId, UInt64 dstId)
{
	std::map<UInt32, BSBattle*>::iterator iter = m_Battles.find(battleID);
	if (iter != m_Battles.end())
	{
		BSBattle* battle = iter->second;
		return battle->PkSomeOne(roleId, dstId, dungeonID);
	}

	return ErrorCode::TEAM_ERROR;
}

UInt32 BSBattleMgr::LockSomeOne(UInt32 battleID, UInt64 roleId, UInt64 dstId)
{
	std::map<UInt32, BSBattle*>::iterator iter = m_Battles.find(battleID);
	if (iter != m_Battles.end())
	{
		BSBattle* battle = iter->second;
		return battle->LockSomeOne(roleId, dstId);
	}

	return ErrorCode::TEAM_ERROR;
}

void BSBattleMgr::OnPlayerLeaveBattleScene(CLProtocol::BattleExitSceneRes& protocol)
{
	//DebugLogStream << "player:" << protocol.roleId << ", leave scene" << LogStream::eos;

	std::map<UInt64, std::vector<UInt32>>::iterator iter = m_PlayerBattleVecMap.find(protocol.roleId);
	if (iter == m_PlayerBattleVecMap.end())
	{
		return;
	}

	for (auto sceneNodeID : iter->second)
	{
		DebugLogStream << "player:" << protocol.roleId << ", leave scene:" << sceneNodeID << LogStream::eos;
		BSNetwork::Instance()->SendToBScene(sceneNodeID, protocol);
	}

	m_PlayerBattleVecMap.erase(iter);
}

void BSBattleMgr::OnPlayerEnterBattleScene(UInt64 playerID, UInt32 sceneNodeID)
{
	//DebugLogStream << "player:" << playerID << ", enter scene:" << sceneNodeID << LogStream::eos;

	std::map<UInt64, std::vector<UInt32>>::iterator iter = m_PlayerBattleVecMap.find(playerID);
	if (iter == m_PlayerBattleVecMap.end())
	{
		DebugLogStream << "player:" << playerID << ", enter scene:" << sceneNodeID << LogStream::eos;

		std::vector<UInt32> tmpVec;
		tmpVec.push_back(sceneNodeID);
		m_PlayerBattleVecMap[playerID] = tmpVec;
	}
	else
	{
		for (auto nodeID : iter->second)
		{
			if (nodeID == sceneNodeID)
			{
				return;
			}
		}

		DebugLogStream << "player:" << playerID << ", enter scene:" << sceneNodeID << LogStream::eos;
		iter->second.push_back(sceneNodeID);
	}
}

bool BSBattleMgr::PlayerOnline(UInt64 playerID)
{
	std::map<UInt64, std::vector<UInt32>>::iterator iter = m_PlayerBattleVecMap.find(playerID);
	if (iter == m_PlayerBattleVecMap.end())
	{
		return false;
	}

	return true;
}

void BSBattleMgr::SetActivityTime(UInt32 startTime, UInt32 endTime)
{
	DebugLogStream << "startTime:" << startTime << ", endTime:" << endTime << LogStream::eos;

	if (0 == startTime && 0 == endTime && m_StartTime != 0 && m_EndTime != 0)
	{
		m_AwardTime = CURRENT_TIME.Sec() + 300;
	}

	m_StartTime = startTime;
	m_EndTime = endTime;

	if (m_StartTime != 0 && m_EndTime != 0 && m_StartTime < m_EndTime)
	{
		m_AwardTime = m_EndTime + 300;
	}
}
