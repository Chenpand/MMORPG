#include "RelaySessionMgr.h"

#include <CLErrorCode.h>
#include <CLLeaveGameProtocol.h>

RelaySessionMgr::RelaySessionMgr()
{
}

RelaySessionMgr::~RelaySessionMgr()
{
	Final();
}

void RelaySessionMgr::Init()
{
}

void RelaySessionMgr::Final()
{

}

RelaySession* RelaySessionMgr::FindSessionByNodeId(UInt32 nodeId)
{
	for (size_t i = 0; i < m_Sessions.size(); i++)
	{
		if (m_Sessions[i].GetNodeID() == nodeId)
		{
			return &m_Sessions[i];
		}
	}

	return NULL;
}

RelaySession* RelaySessionMgr::SelectRelayServer()
{
	// 负载均衡
	// 这里选用加权随机的负载均衡方法，即能力越大的服务器被选中的概率越大

	std::vector<RelaySession*> allUsefulSession;
	UInt32 totalAbality = 0;
	for (auto session : m_Sessions)
	{
		if (session.GetAbality() == 0)
		{
			continue;
		}

		totalAbality += session.GetAbality();
	}

	UInt32 randProb = (UInt32)(rand() / 65536.0f * totalAbality);
	for (auto session : allUsefulSession)
	{
		if (randProb <= session->GetAbality())
		{
			return session;
		}
        randProb -= session->GetAbality();
	}

	return allUsefulSession.empty() ? NULL : allUsefulSession[allUsefulSession.size() - 1];
}

void RelaySessionMgr::UpdateRelayServerAbality(UInt32 nodeID, UInt32 abality)
{
	RelaySession* session = FindSessionByNodeId(nodeID);
	if (!session)
	{
		return;
	}

	session->SetAbality(abality);
}

void RelaySessionMgr::OnTick(const Avalon::Time& now)
{
	for (auto session : m_Sessions)
	{
		session.OnTick(now);
	}
}

bool RelaySessionMgr::OnRelayServerConnected(Int32 id)
{
	RelaySession session;
	if (!session.Init(id))
	{
		return false;
	}

	m_Sessions.push_back(session);
	return true;
}

void RelaySessionMgr::OnRelayServerDisconnected(UInt32 id)
{
	for (size_t i = 0; i < m_Sessions.size(); i++)
	{
		if (m_Sessions[i].GetNodeID() == id)
		{
			m_Sessions.erase(m_Sessions.begin() + i);
			return;
		}
	}
}

