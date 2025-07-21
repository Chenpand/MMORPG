#include "BSBaseRace.h"
#include "RelaySessionMgr.h"
#include "BSNetwork.h"
#include "BSBaseBattle.h"
#include "BSPlayer.h"

BSBaseRace::BSBaseRace(UInt32 battleId, UInt8 raceType)
	: m_raceType(raceType), m_battleID(battleId)
{
	
	SetRaceID(0);
	m_start_match_time = 0;
}

BSBaseRace::BSBaseRace(UInt8 raceType)
	: m_raceType(raceType)
{
	SetRaceID(0);
	SetBattleID(0);
	
}

BSBaseRace::~BSBaseRace()
{
}

bool BSBaseRace::IsTimeToDestory(const Avalon::Time& now)
{
	if (GetStatus() != BRCS_WAIT_CLOSE)
	{
		return false;
	}

	if (m_destoryTime == 0 || now < m_destoryTime)
	{
		return false;
	}

	return true;
}

bool BSBaseRace::SendToRelay(Avalon::Protocol& protocol)
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

void BSBaseRace::SendToBScene(Avalon::Protocol& protocol)
{
	BSBaseBattle* battle = GetBattle();
	if (!battle)	return;

	BSNetwork::Instance()->SendToBScene(battle->GetSceneNodeID(), protocol);
}

void BSBaseRace::OnPlayerJoin(BSPlayer* player)
{
	if (!player) return;
	m_players.insert(player->GetID());
}

void BSBaseRace::OnPlayerLeave(UInt64 roleId, bool isDisconnect)
{
	m_players.erase(roleId);
	if (m_players.size() == 0)
	{
		OnAllPlayerLeave();
	}
}

void BSBaseRace::OnAllPlayerLeave()
{
	this->SetStatus(BRCS_WAIT_CLOSE);
	this->SetDestoryTime(Avalon::Time::CurrentTime());
}