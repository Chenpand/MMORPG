#include "GateSessionMgr.h"

#include <CLErrorCode.h>
#include <CLLeaveGameProtocol.h>

GateSessionMgr::GateSessionMgr()
{
}

GateSessionMgr::~GateSessionMgr()
{
	Final();
}

void GateSessionMgr::Init()
{
}

void GateSessionMgr::Final()
{
	CLDeleteVisitor<GateSession> deleteVisitor;
	m_Sessions.VisitObjects(deleteVisitor);
	m_Sessions.Clear();
}

GateSession* GateSessionMgr::SelectGate()
{
	class SelectVisitor : public GateSessionVisitor
	{
	public:
		SelectVisitor():m_pSession(NULL){}

		bool operator()(GateSession* session)
		{
			if(m_pSession == NULL 
				|| session->GetPlayerNum() < m_pSession->GetPlayerNum())
			{
				m_pSession = session;
			}
			return true;
		}

		GateSession* GetSession(){ return m_pSession; }

	private:
		GateSession *m_pSession;
	};
	SelectVisitor selectVisitor;
	
	m_Sessions.VisitObjects(selectVisitor);
	return selectVisitor.GetSession();
}

void GateSessionMgr::OnGateConnected(UInt32 connid)
{
	GateSession* session = new GateSession();
	if(!session->Init(connid))
	{
		CL_SAFE_DELETE(session);
		return;
	}

	if(!m_Sessions.AddObject(session))
	{
		CL_SAFE_DELETE(session);
		return;
	}
}

void GateSessionMgr::OnGateDisconnected(UInt32 connid)
{
	GateSession* session = m_Sessions.FindObject(connid);
	if(session == NULL) return;

	m_Sessions.RemoveObject(session);
	CL_SAFE_DELETE(session);
}

