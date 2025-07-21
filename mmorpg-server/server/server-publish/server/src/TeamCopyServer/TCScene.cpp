#include "TCScene.h"

#include "TCSPlayerMgr.h"

TCScene::TCScene()
{
	
}

TCScene::~TCScene()
{

}

void TCScene::AddPlayer(UInt64 playerId)
{
	auto iter = m_PlayerSet.find(playerId);
	if (iter != m_PlayerSet.end())
	{
		return;
	}

	m_PlayerSet.insert(playerId);
	m_PlayerVec.push_back(playerId);
}

void TCScene::RemovePlayer(UInt64 playerId)
{
	m_PlayerSet.erase(playerId);
	std::vector<UInt64>::iterator it = m_PlayerVec.begin();
	for (;it != m_PlayerVec.end();++it)
	{
		if (*it == playerId)
		{
			m_PlayerVec.erase(it);
			return;
		}
	}
}

void TCScene::BroadcastPacket(Avalon::Packet* packet)
{
	if (NULL == packet) return;
	
	for (auto& iter : m_PlayerVec)
	{
		TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(iter);
		if (NULL == player)
		{
			continue;
		}

		player->SendPacket(packet);
	}
}

void TCScene::BroadcastProtocol(Avalon::Protocol& protocol)
{
	for (auto& iter : m_PlayerVec)
	{
		TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(iter);
		if (NULL == player)
		{
			continue;
		}

		player->SendProtocol(protocol);
	}
}