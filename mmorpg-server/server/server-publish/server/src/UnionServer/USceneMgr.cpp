#include "USceneMgr.h"

#include <CLSceneProtocol.h>
#include <CLSceneDefine.h>

#include "UnionService.h"
#include "USNetwork.h"
#include "USPlayerMgr.h"

USceneMgr::USceneMgr(UnionService& service) : m_UnionService(service)
{

}

USceneMgr::~USceneMgr()
{
	ClearScene();
}

bool USceneMgr::Init()
{
	return true;
}

void USceneMgr::Final()
{
	
}

void USceneMgr::OnTick(const Avalon::Time& now)
{
	
}

UScene* USceneMgr::AddScene(UInt32 Id)
{
	auto scene = FindUScene(Id);
	if (scene != nullptr)
	{
		return nullptr;
	}
	else
	{
		auto scene = m_UnionService.CreateScene();
		m_SceneMap[Id] = scene;
		scene->SetSceneID(Id);
		return scene;
	}
}

void USceneMgr::RemoveScene(UInt32 id)
{
	UScene* scene = FindUScene(id);
	if (scene != nullptr)
	{
		m_SceneMap.erase(id);
		delete scene;
	}
}

void USceneMgr::ClearScene()
{
	for (auto sceneIter : m_SceneMap)
	{
		auto scene = sceneIter.second;
		delete scene;
	}
	m_SceneMap.clear();
}

std::map<UInt32, UScene*>& USceneMgr::GetAllScene()
{
	return m_SceneMap;
}

void USceneMgr::AddSyncPacket(UInt64 playerId, Avalon::Packet* packet)
{
	if (nullptr == packet) return;

	auto iter = m_SyncPacketMap.find(playerId);
	if (iter != m_SyncPacketMap.end())
	{
		if (iter->second != nullptr)
			Avalon::Packet::Destroy(iter->second);
	}

	Avalon::Packet* newPacket = Avalon::Packet::Create();
	packet->CopyTo(newPacket);
	m_SyncPacketMap[playerId] = newPacket;
}

void USceneMgr::RemoveSyncPacket(UInt64 playerId)
{
	auto iter = m_SyncPacketMap.find(playerId);
	if (iter == m_SyncPacketMap.end())
	{
		return;
	}

	if (iter->second != nullptr)
		Avalon::Packet::Destroy(iter->second);

	m_SyncPacketMap.erase(iter);
}

void USceneMgr::SyncAllObjectToPlayer(USPlayer* player, UInt32 id)
{
	if (nullptr == player) return;
	
	UScene* scene = FindUScene(id);
	if (nullptr == scene)
	{
		return;
	}

	// 最多同步20个玩家
	UInt32 syncPlayerNum = 5;

	std::set<USPlayer*>& playerSet = scene->GetCurScenePlayer();
	for (auto& aPlayer : playerSet)
	{
		if (syncPlayerNum == 0)
			return;

		auto iter = m_SyncPacketMap.find(aPlayer->GetID());
		if (iter == m_SyncPacketMap.end() || iter->second == nullptr || aPlayer->GetID() == player->GetID())
		{
			continue;
		}

		player->SendPacket(iter->second);
		--syncPlayerNum;

		//DebugLogStream << "scene sync playerId:" << player->GetID() << " targetId:" << id << LogStream::eos;
	}
}

Avalon::Packet* USceneMgr::GetPlayerSyncPacket(UInt64 playerId)
{
	auto iter = m_SyncPacketMap.find(playerId);
	if (iter == m_SyncPacketMap.end())
	{
		return nullptr;
	}

	return iter->second;
}

void USceneMgr::BroadcastPacket(Avalon::Packet* packet)
{

	for (auto& iter : m_SceneMap)
	{
		auto scene = iter.second;
		scene->BroadcastPacket(packet);
	}
}

void USceneMgr::BroadcastProtocol(Avalon::Protocol& protocol)
{
	for (auto& iter : m_SceneMap)
	{
		auto scene = iter.second;
		scene->BroadcastProtocol(protocol);
	}
}

UScene* USceneMgr::FindUScene(UInt32 id)
{
	auto iter = m_SceneMap.find(id);
	return iter != m_SceneMap.end() ? iter->second : nullptr;
}