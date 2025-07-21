#ifndef _SERVICESESSION_MGR_H_
#define _SERVICESESSION_MGR_H_

#include <CLObjectIndex.h>
#include <AvalonSingleton.h>
#include <AvalonProtocol.h>
#include "ServiceSession.h"

/** 
 *@brief �淨���������ӹ�����
 */
class ServiceSessionMgr : public Avalon::Singleton<ServiceSessionMgr>
{
	typedef std::map<ServiceSessionKey, ServiceSession*>	ServiceSessionMap;

public:
	ServiceSessionMgr();
	~ServiceSessionMgr();

public:
	bool Init();
	void Final();

public:
	/**
	 *@brief ��ӡ�ɾ����������Ϸ��
	 */
	bool AddServiceSession(ServiceSession* zone);
	void RemoveServiceSession(ServiceSession* zone);
	ServiceSession* FindServiceSession(UInt32 id);

	ServiceSession* FindServiceSessionByKey(ServiceSessionKey key);

	/**
	 *@brief �������з�����
	 */
	void VisitServiceSessions(ServiceSessionVisitor& visitor);

public://��Ϣ���
	/**
	*@brief ����Э��
	*/
	void SendProtocol(Avalon::Protocol& protocol);
	void SendPacket(Avalon::Packet* packet);

public://�¼�
	/**
	 *@brief ���ӽ���
	 */
	void OnConnected(UInt32 id);

	/**
	 *@brief ���ӶϿ�
	 */
	void OnDisconnected(UInt32 id);

private:
	//�淨������
	CLUIntObjIndex<ServiceSession>	m_Sessions;
	//������key
	ServiceSessionMap	m_KeyMapSessions;
};

#endif
