#ifndef _GATESESSION_MGR_H_
#define _GATESESSION_MGR_H_

#include <AvalonSingleton.h>
#include <AvalonTime.h>
#include <CLObjectIndex.h>
#include "GateSession.h"

typedef CLVisitor<GateSession> GateSessionVisitor;

/**
 *@brief ���ػỰ������
 */
class GateSessionMgr : public Avalon::Singleton<GateSessionMgr>
{
	typedef std::map<UInt32,UInt32> AccountMap;

public:
	GateSessionMgr();
	~GateSessionMgr();

	void Init();
	void Final();

public:
	/**
	 *@brief ����һ������
	 */
	GateSession* FindSession(UInt32 id){ return m_Sessions.FindObject(id);}

	/**
	 *@brief ѡ��һ����¼������
	 */
	GateSession* SelectGate();

	/**
	 *@brief �ж�һ������Ƿ�����
	 */
	bool IsPlayerOnline(UInt32 id);

	/**
	 *@brief �ߵ�һ�����
	 */
	void KickOffPlayer(UInt32 id, GateSession* newSession = NULL);

	/**
	 *@brief ����gate�Ự                                                                     
	 */
	void VisitGateSession(GateSessionVisitor& visitor){ m_Sessions.VisitObjects(visitor); }

public://�¼�

	void OnTick(const Avalon::Time& now);

	void OnGateConnected(UInt32 connid);

	void OnGateDisconnected(UInt32 connid);

private:
	//���ػỰmap
	CLIDObjIndex<GateSession>  m_Sessions;
	//�����˺��б�����Ψһ��¼��֤
	AccountMap					m_Accounts;
};

#endif
