#include "BSLostTeamDungeonRace.h"
#include "BSLostBattleMgr.h"
#include "BSPlayerMgr.h"
#include "BSRaceMgr.h"
#include "BSRouter.h"

#include <CLTeamProtocol.h>
#include <CLLostDungeonProtocol.h>
#include <CLChatProtocol.h>
#include <CLSysNotify.h>

BSLostTeamDungeonRace::BSLostTeamDungeonRace()
	:BSBaseRace(0)
{
	m_ownerBattle = NULL;
	m_Master = 0;
	m_TeamId = 0;
}

BSLostTeamDungeonRace::BSLostTeamDungeonRace(UInt8 raceType)
	: BSBaseRace(raceType)
{
	m_ownerBattle = NULL;
	m_Master = 0;
	m_TeamId = 0;
}

BSLostTeamDungeonRace::~BSLostTeamDungeonRace()
{
	for (auto member : m_teamMembers)
	{
		auto player = BSPlayerMgr::Instance()->FindPlayer(member.roleId);
		if (player)
		{
		}
	}
}

void BSLostTeamDungeonRace::OnCreate(CLProtocol::DungenServerTeamStartRaceReq& teamStartRaceReq)
{
	m_teamStartRaceReq = teamStartRaceReq;
	this->SetDungeonID(m_teamStartRaceReq.dungeonId);
	for (auto playerInfo : m_teamStartRaceReq.playerInfoes)
	{
		LostDungRaceMember member;
		member.roleId = playerInfo.raceInfo.roleId;
		member.name = playerInfo.raceInfo.name;
		member.level = playerInfo.raceInfo.level;
		m_teamMembers.push_back(member);
	}

	if (m_teamMembers.size() > 0)
	{
		m_Master = m_teamMembers[0].roleId;
	}
	m_TeamId = teamStartRaceReq.teamId;
	std::ostringstream logss;
	logss << "create team dungeon race, teamId : " << teamStartRaceReq.teamId << ", dungeonId : " << m_teamStartRaceReq.dungeonId
		<< ", race id : " << GetRaceID() << "player ids : ";

	for (auto& member : m_teamMembers)
	{
		logss << member.roleId <<", ";
	}
	InfoLogStream << logss.str() << LogStream::eos;
}

BSLostBattle* BSLostTeamDungeonRace::GetLostBattle()
{
	if (!m_ownerBattle)
	{
		auto battle = BSLostBattleMgr::Instance()->FindBattle(m_battleID);
		if (!battle) return NULL;
		m_ownerBattle = dynamic_cast<BSLostBattle*> (battle);
		if (!m_ownerBattle)  return NULL;
	}
	return m_ownerBattle;
}

UInt32 BSLostTeamDungeonRace::StartRace()
{
	this->SetStatus(BRCS_CREATING);
	for (auto member : m_teamMembers)
	{
		auto player = BSPlayerMgr::Instance()->FindPlayer(member.roleId);
		if (player)
		{
			player->SetLostState(LDBPS_MATCHING);
			player->SetLostRaceId(GetRaceID());
		}
	}
	if (m_teamMembers.size() == 1)
	{
		//转发给DungeonServer
		m_teamStartRaceReq.battleTempRaceId = this->GetRaceID();
		Router::SendToDungeon(m_teamStartRaceReq);
		InfoLogStream << "team dungeon race req start send proto to DungonServer, raceid : "
			<< GetRaceID() << ", teamid : " << m_TeamId << LogStream::eos;
		return ErrorCode::SUCCESS;
	}
	else if (m_teamMembers.size() > 1)
	{
		OnStartRaceVote(GetDungeonID());
	}
	return ErrorCode::SUCCESS;
}

void BSLostTeamDungeonRace::OnStartRaceFail()
{
	for (auto member : m_teamMembers)
	{
		auto player = BSPlayerMgr::Instance()->FindPlayer(member.roleId);
		if (player)
		{
			player->SetLostState(LDBPS_NORAML);
			player->SetLostRaceId(0);
		}
	}
	SyncTeamState(LDTBS_NORMAL);
}

void BSLostTeamDungeonRace::OnTick(const Avalon::Time& now)
{
	if (m_Vote.IsTimeout())
	{
		for (auto& member : m_teamMembers)
		{
			if (!m_Vote.HasPlayer(member.roleId))
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
}

BSBaseBattle* BSLostTeamDungeonRace::GetBattle()
{
	return GetLostBattle();
}

void BSLostTeamDungeonRace::Broadcast(Avalon::Protocol& protocol)
{
	for (auto member : m_teamMembers)
	{
		BSPlayer* player = BSPlayerMgr::Instance()->FindPlayer(member.roleId);
		if (player)
		{
			player->sendToClient(protocol);
		}
	}
}

UInt32 BSLostTeamDungeonRace::OnStartRaceVote(UInt32 dungeonId)
{
	// 说明已经有投票了
	if (m_Vote.dungeonId > 0)
	{
		ErrorLogStream << "team(" << GetTeamId() << ") repeat to vote dungeon(" << dungeonId << ")." << LogStream::eos;
		return ErrorCode::DUNGEON_TEAM_START_VOTE_REPEAT;
	}

	m_Vote.Clear();
	m_Vote.dungeonId = dungeonId;
	m_Vote.startTime = (UInt32)CURRENT_TIME.Sec();
	m_Vote.agreePlayerIds.push_back(GetMaster());


	// 通知所有队员
	CLProtocol::BattleTeamRaceVoteNotify notify;
	notify.dungeonId = dungeonId;
	notify.teamId = m_TeamId;
	Broadcast(notify);

	return ErrorCode::DUNGEON_TEAM_WAIT_OTHER_VOTE;
}

void  BSLostTeamDungeonRace::OnPlayerReportVote(BSPlayer* player, bool agree)
{
	if (!player)
	{
		return;
	}
	// 没有投票
	if (m_Vote.dungeonId == 0)
	{
		ErrorLogStream << "team(" << GetTeamId() << ") player(" << player->GetID() << ") vote not exist." << LogStream::eos;
		return;
	}

	// 已经投过票了
	if (m_Vote.HasPlayer(player->GetID()))
	{
		ErrorLogStream << "team(" << GetTeamId() << ") player(" << player->GetID() << ") repeat vote." << LogStream::eos;
		return;
	}

	if (!agree)
	{
		OnStartRaceVoteFailure(player->GetID());
	}
	else
	{
		CLProtocol::BattleTeamVoteChoiceNotify notify;
		notify.roleId = player->GetID();
		notify.agree = agree ? 1 : 0;
		Broadcast(notify);

		m_Vote.agreePlayerIds.push_back(player->GetID());
		if (m_Vote.agreePlayerIds.size() == GetMemberSize())
		{
			OnStartRaceVoteAllAgree();
		}
	}
}

void BSLostTeamDungeonRace::OnStartRaceVoteAllAgree()
{
	if (m_Vote.dungeonId == 0)
	{
		return;
	}

	DebugLogStream << "team(" << GetTeamId() << ") race vote dungeon(" << m_Vote.dungeonId << ") success." << LogStream::eos;
	//转发给DungeonServer
	m_teamStartRaceReq.battleTempRaceId = this->GetRaceID();
	Router::SendToDungeon(m_teamStartRaceReq);
	m_Vote.Clear();
}

void BSLostTeamDungeonRace::OnStartRaceVoteFailure(ObjID_t roleId, UInt32 noticeId)
{
	if (m_Vote.dungeonId == 0)
	{
		return;
	}

	DebugLogStream << "team(" << GetTeamId() << ") race vote dungeon(" << m_Vote.dungeonId << ") failure." << LogStream::eos;

	// 通知玩家投票失败
	CLProtocol::BattleTeamVoteChoiceNotify notify;
	notify.roleId = roleId;
	notify.agree = 0;
	Broadcast(notify);

	if (noticeId)
	{
		Notify(0, 0, SysNotifyMgr::Instance()->MakeString(noticeId).c_str());
	}
	m_Vote.Clear();

	for (auto member : m_teamMembers)
	{
		auto player = BSPlayerMgr::Instance()->FindPlayer(member.roleId);
		if (player)
		{
			player->SetLostState(LDBPS_NORAML);
			player->SetLostRaceId(0);
		}
	}

	SetStatus(BRCS_WAIT_CLOSE);
	SetDestoryTime((UInt32)CURRENT_TIME.Sec());
	SyncTeamState(LDTBS_NORMAL);
}

void BSLostTeamDungeonRace::Notify(UInt16 type, UInt8 color, const char* word)
{
	CLProtocol::SysNotify notify;
	notify.type = type;
	notify.color = color;
	notify.channel = 0;
	notify.word = word;

	Broadcast(notify);
}

void BSLostTeamDungeonRace::SyncTeamState(LostDungTeamBattleSt st, UInt8 isEnd)
{
	CLProtocol::BattleSynLostDungTeamSt sync;
	sync.battleId = m_ownerBattle ? m_ownerBattle->GetId() : 0;
	sync.teamId = GetTeamId();
	sync.state = st;
	sync.isEnd = isEnd;

	SendToBScene(sync);
	InfoLogStream << "sync team state battleId : " << sync.battleId << ", teamId : " << sync.teamId 
		<< ", state : " << sync.state << ", isEnd : " << sync.isEnd << LogStream::eos;
}

void BSLostTeamDungeonRace::OnStartDungeonRes(UInt32 result, UInt64 gamesessionID)
{
	DebugLogStream << "monster dungeon race (" << gamesessionID << ") recv start response, result(" << result << LogStream::eos;
	if (result != 0)
	{
		ErrorLogStream << "dungeon server return error(" << result << "), race id(" << GetRaceID() << ")." << LogStream::eos;
		SetStatus(BRCS_WAIT_CLOSE);
		SetDestoryTime((UInt32)CURRENT_TIME.Sec());
		OnStartRaceFail();
		return;
	}

	SetStatus(BRCS_RACING);
	BSRaceMgr::Instance()->RemoveRace(GetRaceID());
	this->SetRaceID(gamesessionID);
	BSRaceMgr::Instance()->AddRace(this);

	for (auto member : m_teamMembers)
	{
		BSPlayer* player = BSPlayerMgr::Instance()->FindPlayer(member.roleId);
		if (player)
		{
			player->SetLostState(LDBPS_RACING);
			player->SetLostRaceId(GetRaceID());
			BSRaceMgr::Instance()->OnPlayerJoinRace(this, player);
		}
	}

	SyncTeamState(LDTBS_BATTLE);
}

void BSLostTeamDungeonRace::OnDungeonEnd(UInt8 result, DungeonRaceEndInfo& endInfo)
{

	for (auto member : m_teamMembers)
	{
		BSPlayer* player = BSPlayerMgr::Instance()->FindPlayer(member.roleId);
		if (player)
		{
		}
	}

	for (auto playerInfo : endInfo.infoes)
	{
		BSPlayer* player = BSPlayerMgr::Instance()->FindPlayer(playerInfo.roleId);
		if (!player)
		{
			continue;
		}
		player->SyncEventToBScene(SET_LOST_DUNG_HP_MP, playerInfo.playerRemainHp, playerInfo.playerRemainMp);
	}

	CLProtocol::BattleNotifyTeamRaceEnd notify;
	notify.teamId = GetTeamId();
	notify.reason = result;
	SendToBScene(notify);
	//SyncTeamState(LDTBS_NORMAL, 1);
}