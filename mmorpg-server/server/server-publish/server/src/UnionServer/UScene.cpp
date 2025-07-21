#include "UScene.h"

#include "USPlayer.h"

UScene::UScene()
{
	
}

UScene::~UScene()
{
	ClearPlayer();
}

USPlayer* UScene::AddPlayer(USPlayer* player)
{
	if (player == nullptr)
	{
		return nullptr;
	}
	auto iter = m_PlayerSet.find(player);
	if (iter != m_PlayerSet.end())
	{
		return *iter;
	}

	m_PlayerSet.insert(player);
	
	return player;
}

void UScene::RemovePlayer(USPlayer* player)
{
	if (player == nullptr)
	{
		return;
	}
	m_PlayerSet.erase(player);
}

void UScene::ClearPlayer()
{
	m_PlayerSet.clear();
}

UInt32 UScene::GetPlayerNum()
{
	return m_PlayerSet.size();
}

void UScene::BroadcastPacket(Avalon::Packet* packet)
{
	if (nullptr == packet) return;
	
	for (auto& player : m_PlayerSet)
	{
		player->SendPacket(packet);
	}
}

void UScene::BroadcastProtocol(Avalon::Protocol& protocol)
{
	for (auto& player : m_PlayerSet)
	{
		player->SendProtocol(protocol);
	}
}