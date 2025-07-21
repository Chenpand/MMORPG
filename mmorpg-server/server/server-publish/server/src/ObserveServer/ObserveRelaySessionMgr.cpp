#include "ObserveRelaySessionMgr.h"

#include <CLErrorCode.h>
#include <CLLeaveGameProtocol.h>

ObserveRelaySessionMgr::ObserveRelaySessionMgr()
{
}

ObserveRelaySessionMgr::~ObserveRelaySessionMgr()
{
	Final();
}

void ObserveRelaySessionMgr::Init()
{
}

void ObserveRelaySessionMgr::Final()
{

}

ObserveRelaySession* ObserveRelaySessionMgr::FindSessionByNodeId(UInt32 nodeId)
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

ObserveRelaySession* ObserveRelaySessionMgr::SelectRelayServer()
{
	// 负载均衡
	// 这里选用加权随机的负载均衡方法，即能力越大的服务器被选中的概率越大

	std::vector<ObserveRelaySession*> allUsefulSession;
	UInt32 totalAbality = 0;
    for (UInt32 i = 0; i < m_Sessions.size(); i++)
	{
		if (m_Sessions[i].GetAbality() == 0)
		{
			continue;
		}

		totalAbality += m_Sessions[i].GetAbality();
        allUsefulSession.push_back(&m_Sessions[i]);
	}

	UInt32 randProb = Avalon::Random::RandBetween(1, totalAbality);
	for (auto session : allUsefulSession)
	{
		if (randProb <= session->GetAbality())
		{
			session->SetAbality(session->GetAbality() - 1);
			return session;
		}
        randProb -= session->GetAbality();
	}

	if (allUsefulSession.empty())
	{
		return nullptr;
	}

	allUsefulSession[allUsefulSession.size() - 1]->SetAbality(allUsefulSession[allUsefulSession.size() - 1]->GetAbality() - 1);
	return allUsefulSession[allUsefulSession.size() - 1];

}

void ObserveRelaySessionMgr::UpdateRelayServerAbality(UInt32 nodeID, UInt32 abality)
{
	ObserveRelaySession* session = FindSessionByNodeId(nodeID);
	if (!session)
	{
		return;
	}

	session->SetAbality(abality);
}

void ObserveRelaySessionMgr::AddRelayServerShutdownChecker(UInt32 nodeID)
{
    InfoLogStream << "add relay server(" << nodeID << ") shutdown checker." << LogStream::eos;

    DelRelayServerShutdownChecker(nodeID);

	ObserveRelayServerShutdownChecker checker(nodeID);
    m_shutdownChecker.push_back(checker);
}

void ObserveRelaySessionMgr::DelRelayServerShutdownChecker(UInt32 nodeID)
{
    InfoLogStream << "del relay server(" << nodeID << ") shutdown checker." << LogStream::eos;

    for (UInt32 i = 0; i < m_shutdownChecker.size(); i++)
    {
        const ObserveRelayServerShutdownChecker& checker = m_shutdownChecker[i];
        if (checker.nodeId == nodeID)
        {
            m_shutdownChecker.erase(m_shutdownChecker.begin() + i);
            return;
        }
    }
}

void ObserveRelaySessionMgr::OnTick(const Avalon::Time& now)
{
    for (UInt32 i = 0; i < m_Sessions.size(); i++)
	{
		m_Sessions[i].OnTick(now);
	}

    for (UInt32 i = 0; i < m_shutdownChecker.size(); )
    {
        const ObserveRelayServerShutdownChecker& checker = m_shutdownChecker[i];
        if (!checker.isTimeToCheck(now))
        {
            i++;
            continue;
        }

        InfoLogStream << "it's time to check relay server(" << checker.nodeId << ")." << LogStream::eos;

        // 如果能找到，说明relayserver又连上来了
        if (FindSessionByNodeId(checker.nodeId))
        {
            InfoLogStream << "relay server(" << checker.nodeId << ") is already connected." << LogStream::eos;
            m_shutdownChecker.erase(m_shutdownChecker.begin() + i);
            continue;
        }

        // 超过预定的时间还没连上来，就认为relayserver崩溃了
        m_shutdownChecker.erase(m_shutdownChecker.begin() + i);
    }
}

bool ObserveRelaySessionMgr::OnRelayServerConnected(Int32 id)
{
	ObserveRelaySession session;
	if (!session.Init(id))
	{
        ErrorLogStream << "init relay session(" << id << ") failed." << LogStream::eos;
		return false;
	}

    InfoLogStream << "add relay session(" << id << ")." << LogStream::eos;
	m_Sessions.push_back(session);
    DelRelayServerShutdownChecker(id);
	return true;
}

void ObserveRelaySessionMgr::OnRelayServerDisconnected(UInt32 id)
{
    InfoLogStream << "relay session(" << id << ") disconnected." << LogStream::eos;

	for (size_t i = 0; i < m_Sessions.size(); i++)
	{
		if (m_Sessions[i].GetNodeID() == id)
		{
            AddRelayServerShutdownChecker(id);
			m_Sessions.erase(m_Sessions.begin() + i);
			return;
		}
	}
}

