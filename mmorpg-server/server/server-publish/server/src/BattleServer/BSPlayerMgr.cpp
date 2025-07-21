#include "BSPlayerMgr.h"

#include <CLGateProtocol.h>
#include <CLDungeonProtocol.h>

#include "BSRouter.h"

BSPlayerMgr::BSPlayerMgr()
{
	//m_Timer.SetInterval(10 * 1000);
}

BSPlayerMgr::~BSPlayerMgr()
{

}

void BSPlayerMgr::OnTick(const Avalon::Time& now)
{

}

void BSPlayerMgr::AddPlayer(BSPlayer* player)
{
	if (NULL == player) return;
	auto iter = m_players.find(player->GetID());
	if (iter != m_players.end())
	{
		ErrorLogStream << " BSPlayer Exist! id:" << player->GetID() << LogStream::eos;
		RemovePlayer(player->GetID());
	}

	m_players.insert(std::make_pair(player->GetID(), player));

	InfoLogStream << "add player id:" << player->GetID() << LogStream::eos;
}

BSPlayer* BSPlayerMgr::FindPlayer(UInt64 playerId)
{
	auto iter = m_players.find(playerId);
	if (iter == m_players.end())
	{
		return NULL;
	}

	return iter->second;
}

void BSPlayerMgr::RemovePlayer(UInt64 playerId)
{
	auto iter = m_players.find(playerId);
	if (iter == m_players.end())
	{
		return;
	}
	CL_SAFE_DELETE(iter->second);
	m_players.erase(playerId);
}



void BSPlayerMgr::ExecuteProtocol(CLProtocol::GateTransmitPacket& protocol)
{
	BSPlayer* player = FindPlayer(protocol.id);
	if (!player)	return;

	m_Executor.ExecutePacket(protocol.packet, player);
}
