#include "USPlayerMgr.h"

#include <CLSysProtocol.h>
#include "UnionService.h"


USPlayerMgr::USPlayerMgr(UnionService& service) : m_UnionService(service)
{

}

USPlayerMgr::~USPlayerMgr()
{
	ClearUSPlayer();
}

void USPlayerMgr::Init()
{
	RegisterProtocols();
}

void USPlayerMgr::OnTick(const Avalon::Time& now)
{
	for (auto& player : m_PlayerVec)
	{
		if (nullptr == player)
		{
			ErrorLogStream << "null player" << LogStream::eos;
			continue;
		}
		player->OnTick(now);
	}
}

USPlayer* USPlayerMgr::FindUSPlayer(UInt64 id)
{
	auto iter = m_PlayerMap.find(id);
	if (iter == m_PlayerMap.end())
	{
		return NULL;
	}

	return iter->second;
}

USPlayer * USPlayerMgr::FindUSPlayerByAccid(UInt32 accid)
{
	return nullptr;
}

USPlayer* USPlayerMgr::AddUSPlayer(UInt64 id)
{
	auto iter = m_PlayerMap.find(id);
	if (iter != m_PlayerMap.end())
	{
		//ErrorLogStream << " USPlayer Exist! id:"<< id << LogStream::eos;
		return iter->second;
	}
	auto player = m_UnionService.CreatePlayer();
	player->SetID(id);
	m_PlayerMap.insert(std::make_pair(id, player));
	m_PlayerVec.push_back(player);

	//InfoLogStream << "add player id:" << id << LogStream::eos;

	return player;
}

void USPlayerMgr::RemoveUSPlayer(UInt64 id)
{
	m_PlayerMap.erase(id);

	USPlayerVec::iterator iter = m_PlayerVec.begin();
	for (; iter != m_PlayerVec.end(); ++iter)
	{
		if ((*iter)->GetID() == id)
		{
			CL_SAFE_DELETE(*iter);
			m_PlayerVec.erase(iter);
			InfoLogStream << "remove player id:" << id << LogStream::eos;
			return;
		}
	}
}

void USPlayerMgr::ClearUSPlayer()
{
	for (auto& iter : m_PlayerVec)
	{
		if (iter)
		{
			CL_SAFE_DELETE(iter);
		}
	}
	m_PlayerMap.clear();
	m_PlayerVec.clear();
}

USPlayerVec& USPlayerMgr::GetAllUSPlayer()
{
	return m_PlayerVec;
}

USPlayerVec USPlayerMgr::GetSuitUSPlayer(std::function<bool(USPlayer*)> suit)
{
	USPlayerVec vec;
	for (auto i : m_PlayerVec)
	{
		if (suit(i))
		{
			vec.push_back(i);
		}
	}
	return vec;
}

void USPlayerMgr::BroadcastToPlayer(Avalon::Packet* packet)
{
	for (auto& iter : m_PlayerVec)
	{
		if (NULL == iter)
		{
			continue;
		}

		iter->SendPacket(packet);
	}
}

void USPlayerMgr::BroadcastToPlayer(Avalon::Protocol& protocol)
{
	for (auto& iter : m_PlayerVec)
	{
		if (NULL == iter)
		{
			continue;
		}

		iter->SendProtocol(protocol);
	}
}

void USPlayerMgr::ExecuteProtocol(const CLProtocol::SysTransmitPlayerToUnion& protocol)
{
	if ( !m_IDExecutor.ExecutePacket(protocol.packet, {protocol.accId, protocol.playerId, protocol.GetConnID(), protocol.sourceNode}))
	{
		USPlayer* player = FindUSPlayer(protocol.playerId);
		if (NULL == player)
		{
			return;
		}

		protocol.packet->SetConnID(protocol.GetConnID());
		m_Executor.ExecutePacket(protocol.packet, player);
	}
}

