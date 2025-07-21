#include "ServiceSessionMgr.h"
#include <CLCenterProtocol.h>

ServiceSessionMgr::ServiceSessionMgr()
{
}

ServiceSessionMgr::~ServiceSessionMgr()
{
}

bool ServiceSessionMgr::Init()
{
	return true;
}

void ServiceSessionMgr::Final()
{
	CLDeleteVisitor<ServiceSession> delVisitor;
	m_Sessions.VisitObjects(delVisitor);
	m_Sessions.Clear();

	m_KeyMapSessions.clear();
}

bool ServiceSessionMgr::AddServiceSession(ServiceSession* session)
{
	if (!m_Sessions.AddObject(session))
	{
		return false;
	}
	if (!m_KeyMapSessions.insert(std::make_pair(session->GetKey(), session)).second)
	{
		m_Sessions.RemoveObject(session);
		return false;
	}
	return true;
}

void ServiceSessionMgr::RemoveServiceSession(ServiceSession* session)
{
	m_Sessions.RemoveObject(session);
	m_KeyMapSessions.erase(session->GetKey());
}

ServiceSession* ServiceSessionMgr::FindServiceSession(UInt32 id)
{
	return m_Sessions.FindObject(id);
}

ServiceSession* ServiceSessionMgr::FindServiceSessionByKey(ServiceSessionKey key)
{
	ServiceSessionMap::iterator iter = m_KeyMapSessions.find(key);
	if (iter != m_KeyMapSessions.end()) return iter->second;
	return NULL;
}

void ServiceSessionMgr::OnConnected(UInt32 id)
{
	ServiceSession* session = new ServiceSession();
	if (!session->Init(id))
	{
		CL_SAFE_DELETE(session);
		return;
	}
	if (!AddServiceSession(session))
	{
		CL_SAFE_DELETE(session);
		return;
	}
}

void ServiceSessionMgr::OnDisconnected(UInt32 id)
{
	ServiceSession* session = FindServiceSession(id);
	if (session != NULL)
	{
		RemoveServiceSession(session);
		CL_SAFE_DELETE(session);
	}
}
