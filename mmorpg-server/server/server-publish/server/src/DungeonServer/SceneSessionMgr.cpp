#include "SceneSessionMgr.h"
#include <CLCenterProtocol.h>

SceneSessionMgr::SceneSessionMgr()
{
}

SceneSessionMgr::~SceneSessionMgr()
{
}

bool SceneSessionMgr::Init()
{
	return true;
}

void SceneSessionMgr::Final()
{
	CLDeleteVisitor<SceneSession> delVisitor;
	m_Sessions.VisitObjects(delVisitor);
	m_Sessions.Clear();

}

bool SceneSessionMgr::AddSceneSession(SceneSession* session)
{
	if (!m_Sessions.AddObject(session))
	{
		return false;
	}
	return true;
}

void SceneSessionMgr::RemoveSceneSession(SceneSession* session)
{
	m_Sessions.RemoveObject(session);
}

SceneSession* SceneSessionMgr::FindSceneSession(UInt32 id)
{
	return m_Sessions.FindObject(id);
}

void SceneSessionMgr::OnConnected(UInt32 id)
{
	SceneSession* session = new SceneSession();
	if (!session->Init(id))
	{
		CL_SAFE_DELETE(session);
		return;
	}
	if (!AddSceneSession(session))
	{
		CL_SAFE_DELETE(session);
		return;
	}
}

void SceneSessionMgr::OnDisconnected(UInt32 id)
{
	SceneSession* session = FindSceneSession(id);
	if (session != NULL)
	{
		RemoveSceneSession(session);
		CL_SAFE_DELETE(session);
	}
}
