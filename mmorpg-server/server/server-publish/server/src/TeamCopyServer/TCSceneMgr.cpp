#include "TCSceneMgr.h"

#include <CLSceneProtocol.h>
#include <CLSceneDefine.h>
#include <CLTeamCopyDefine.h>

#include "TCSNetwork.h"
#include "TCSPlayerMgr.h"

TCSceneMgr::TCSceneMgr()
{

}

TCSceneMgr::~TCSceneMgr()
{

}

bool TCSceneMgr::Init()
{
	return true;
}

void TCSceneMgr::Final()
{
	
}

void TCSceneMgr::OnTick(const Avalon::Time& now)
{
	
}

void TCSceneMgr::AddToPrepareScene(UInt32 copyId, UInt64 playerId)
{
	TCScene* scene = _FindTCScene(copyId);
	if (scene != NULL)
	{
		scene->AddPlayer(playerId);
	}
	else
	{
		TCScene scene;
		scene.AddPlayer(playerId);
		m_PrepareSceneMap[copyId] = scene;
	}
}

void TCSceneMgr::RemoveToPrepareScene(UInt32 copyId, UInt64 playerId)
{
	TCScene* scene = _FindTCScene(copyId);
	if (scene != NULL)
	{
		scene->RemovePlayer(playerId);
	}
}

void TCSceneMgr::AddSyncPacket(UInt64 playerId, Avalon::Packet* packet)
{
	if (NULL == packet) return;

	auto iter = m_SyncPacketMap.find(playerId);
	if (iter != m_SyncPacketMap.end())
	{
		if (iter->second != NULL)
			Avalon::Packet::Destroy(iter->second);
	}

	Avalon::Packet* newPacket = Avalon::Packet::Create();
	packet->CopyTo(newPacket);
	m_SyncPacketMap[playerId] = newPacket;
}

void TCSceneMgr::RemoveSyncPacket(UInt64 playerId)
{
	auto iter = m_SyncPacketMap.find(playerId);
	if (iter == m_SyncPacketMap.end())
	{
		return;
	}

	if (iter->second != NULL)
		Avalon::Packet::Destroy(iter->second);

	m_SyncPacketMap.erase(iter);
}

void TCSceneMgr::SyncAllObjectToPlayer(TCSPlayer* player, UInt32 copyId)
{
	if (NULL == player) return;
	
	TCScene* scene = _FindTCScene(copyId);
	if (NULL == scene)
	{
		return;
	}

	// 最多同步20个玩家
	UInt32 syncPlayerNum = 20;

	std::vector<UInt64>& playerIds = scene->GetCurScenePlayer();
	std::random_shuffle(playerIds.begin(), playerIds.end());
	for (auto& id : playerIds)
	{
		if (syncPlayerNum == 0)
			return;

		auto iter = m_SyncPacketMap.find(id);
		if (iter == m_SyncPacketMap.end() || iter->second == NULL || id == player->GetID())
		{
			continue;
		}

		player->SendPacket(iter->second);
		--syncPlayerNum;

		//DebugLogStream << "scene sync playerId:" << player->GetID() << " targetId:" << id << LogStream::eos;
	}
}

Avalon::Packet* TCSceneMgr::GetPlayerSyncPacket(UInt64 playerId)
{
	auto iter = m_SyncPacketMap.find(playerId);
	if (iter == m_SyncPacketMap.end())
	{
		return NULL;
	}

	return iter->second;
}

void TCSceneMgr::BroadcastPacket(Avalon::Packet* packet, UInt32 copyId)
{
	TCScene* scene = _FindTCScene(copyId);
	if (scene != NULL)
	{
		scene->BroadcastPacket(packet);
	}
}

void TCSceneMgr::BroadcastProtocol(Avalon::Protocol& protocol, UInt32 copyId)
{
	TCScene* scene = _FindTCScene(copyId);
	if (scene != NULL)
	{
		scene->BroadcastProtocol(protocol);
	}
}

TCScene* TCSceneMgr::_FindTCScene(UInt32 copyId)
{
	auto iter = m_PrepareSceneMap.find(copyId);
	return iter != m_PrepareSceneMap.end() ? &(iter->second) : NULL;
}