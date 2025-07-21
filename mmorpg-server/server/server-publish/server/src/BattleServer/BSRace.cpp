#include "BSRace.h"
#include "BSNetwork.h"
#include "BSBattleMgr.h"
#include "RelaySessionMgr.h"
#include <CLGameSessionProtocol.h>
#include <CLBattleProtocol.h>

BattleRace::BattleRace(UInt32 battleId, UInt8 raceType)
	:BSBaseRace(battleId, raceType)
{
	SetState(BRS_WAIT_PLAYER_INFO);
}

BattleRace::~BattleRace()
{
	m_teams.clear();
}

bool BattleRace::OnCreate(VBSTeam teams)
{
	m_teams = teams;
	return true;
}

bool BattleRace::OnRaceEnd(EGameSessionDismissType ret, const PkRaceEndInfo& raceEndInfo)
{
	if (GetState() != BRS_RACE)
	{
		DebugLogStream << "race status error, status : " << (UInt32)GetState() << LogStream::eos;
		return false;
	}

	BSBattle* battle = BSBattleMgr::Instance()->GetBattle(GetBattleID());
	if (!battle)
	{
		DebugLogStream << "race battle id error, id : " << GetBattleID() << LogStream::eos;
		return false;
	}

	CLProtocol::SceneBattleKillSomeOne notify;
	notify.battleID = GetBattleID();
	notify.playerID = 0;
	notify.killerID = 0;
	notify.otherID = 0;

	for (auto& info : raceEndInfo.results)
	{
		for (auto team : m_teams)
		{
			auto member = team->FindMember(info.roleId);
			if (!member)
			{
				//DebugLogStream << "can't find player, id : " << info.roleId << LogStream::eos;
				continue;
			}

			//reset race info flag
			member->m_hasRaceInfo = false;
			member->m_IsPk = true;
			member->SetRaceId(0);

			if (member->GetStatus() != BPS_PK)
			{
				DebugLogStream << "player : " << info.roleId << ", status : " << (UInt32)(member->GetStatus()) << LogStream::eos;
				continue;
			}

			if (info.resultFlag & resultBitmask_win)
			{
				team->SetStatus(BTS_ALIVE);
				notify.killerID = info.roleId;
				notify.remainHp = (Int32)info.remainHp;
				notify.remainMp = (Int32)info.remainMp;

				//设置玩家状态
				member->SetStatus(BPS_ALIVE);
				++member->m_kills;
				member->m_protectTimestamp = Avalon::Time::CurrentTime().Sec() + 30;

				InfoLogStream << "player:" << info.roleId << ", is alive, protect:" << member->m_protectTimestamp << LogStream::eos;
			}
			else
			{
				InfoLogStream << "player:" << info.roleId << ", is out" << LogStream::eos;
				if (0 == notify.playerID)
				{
					notify.playerID = info.roleId;
				}
				else
				{
					//有可能平局的
					notify.otherID = info.roleId;
				}
			}

			CLProtocol::MatchOpponent opponent;
			opponent.zoneId = member->m_ConnId;
			opponent.roleId = member->m_PlayerId;
			opponent.name = member->m_PlayerName;
			opponent.occu = member->m_PlayerOccu;

			CLProtocol::MatchServerRaceEnd endReq;
			endReq.type = PK_1V1_CHIJI;
			endReq.opponent = opponent;
			endReq.result = info;
			BSNetwork::Instance()->SendToBScene(battle->GetSceneNodeID(), endReq);
		}
	}

	//补充一下playerID，让它与killerID或otherID成对出现
	if (0 == notify.playerID)
	{
		for (auto& info : raceEndInfo.results)
		{
			if (info.roleId != notify.killerID && info.roleId != notify.otherID)
			{
				notify.playerID = info.roleId;
				break;
			}
		}
	}

	InfoLogStream << "battleID:" << notify.battleID << ", playerID:" << notify.playerID << ", killerID:" << notify.killerID << ", otherID:" << notify.otherID << LogStream::eos;

	BSNetwork::Instance()->SendToBScene(battle->GetSceneNodeID(), notify);

	battle->OnRaceEnd();

	return true;
}

VBSTeamMember BattleRace::GetAllMember() const
{
    VBSTeamMember members;
    for (auto team : m_teams)
    {
        if (!team)
        {
            continue;
        }

        for (auto member : team->GetTeamMembers())
        {
            members.push_back(member);
        }
    }

    return members;
}

BSTeamMember* BattleRace::FindMember(UInt64 id) const
{
    for (auto team : m_teams)
    {
        if (!team)
        {
            continue;
        }

        auto member = team->FindMember(id);
		if (!member)
		{
			continue;
		}

        return member;
    }

    return NULL;
}

bool BattleRace::IsAllPlayerHasRaceInfo() const
{
    VBSTeamMember members = GetAllMember();
    for (auto member : members)
    {
        if (!member.HasRaceInfo())
        {
            return false;
        }
    }

    return true;
}

void BattleRace::SetRaceInfo(UInt64 playerId, const RacePlayerInfo& info)
{
	BSTeamMember* member = FindMember(playerId);
	if (member)
	{
		member->m_hasRaceInfo = true;
		member->m_raceInfo = info;
	}
}

bool BattleRace::RequestRelayServerStart(void)
{
	CLProtocol::MatchSvrStartGameReq startReq;
	startReq.raceType = RT_CHIJI;
	startReq.gamesessionID = GetRaceID();

	UInt8 pos = 0;
	CLProtocol::Fighter fighter;
	for (auto team : m_teams)
	{
		const VBSTeamMember& members = team->GetTeamMembers();
		for (auto& member : members)
		{
			fighter.roleId = member.m_PlayerId;
			fighter.accId = member.m_AccId;
			fighter.pos = pos++;
			fighter.wsId = member.m_ConnId;

			startReq.gamesessionFighter.push_back(fighter);
		}
	}

	SendToRelay(startReq);
	//BSNetwork::Instance()->SendToRelay(startReq);

	SetState(BRS_WAIT_RELAY_SERVER_RES);

	return true;
}

void BattleRace::SetPlayerStateStatus(UInt64 playerId, BattlePlayerState status)
{
	BSTeamMember* member = FindMember(playerId);
	if (member)
	{
		member->SetStatus(status);
	}
}

void BattleRace::SetPlayerRaceID(UInt64 playerId, UInt64 raceId)
{
	BSTeamMember* member = FindMember(playerId);
	if (member)
	{
		member->SetRaceId(raceId);
	}
}

bool BattleRace::SendToRelay(Avalon::Protocol& protocol)
{
	RelaySession* session = RelaySessionMgr::Instance()->FindSessionByNodeId(GetRelayServerID());
	if (!session)
	{
		ErrorLogStream << "can't find relay server : " << GetRelayServerID() << LogStream::eos;
		return false;
	}

	session->SendProtocol(protocol);
	return true;
}
