#ifndef _GM_CLIENT_MGR_H_
#define _GM_CLIENT_MGR_H_

#include <CLObjectIndex.h>
#include <AvalonSingleton.h>
#include "GMClient.h"

/**
 *@brief gm�ͻ��˹�����
 */
class GMClientMgr : public Avalon::Singleton<GMClientMgr>
{
public:
	GMClientMgr();
	~GMClientMgr();

public:
	bool Init();
	void Final();

public:
	/**
	 *@brief ��ӡ�ɾ��������gm�ͻ���
	 */
	bool AddGMClient(GMClient* client);
	void RemoveGMClient(GMClient* client);
	GMClient* FindGMClient(UInt32 id);

	/**
	 *@brief ��������gm�ͻ���
	 */
	void VisitGMClients(GMClientVisitor& visitor);

public://��Ϣ���
	/** 
	 *@brief �㲥��Ϣ
	 */
	void BroadcastToGMClient(Avalon::Packet* packet);
	void BroadcastToGMClient(Avalon::Protocol& protocol);

public://�¼�
	/**
	 *@brief ����
	 */
	void OnTick(const Avalon::Time& now);

	/**
	 *@brief ���ӽ���
	 */
	void OnConnected(UInt32 id);

	/**
	 *@brief ���ӶϿ�
	 */
	void OnDisconnected(UInt32 id);

private:
	//gm�ͻ�������
	CLUIntObjIndex<GMClient>	m_Clients;
};

#endif
