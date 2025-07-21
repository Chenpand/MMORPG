#ifndef __VERIFY_CLIENT_MGR_H__
#define __VERIFY_CLIENT_MGR_H__

#include <AvalonSingleton.h>
#include <AvalonTime.h>
#include <CLObjectIndex.h>
#include "VerifyClient.h"

/**
*@brief ��֤�ͻ��˹�����
*/
class VerifyClientMgr : public Avalon::Singleton<VerifyClientMgr>
{
public:
	/**
	*@brief ͨ���ڵ�ID����һ��Client
	*/
	VerifyClient* FindClient(UInt32 id);

	/**
	*@brief ����Client
	*/
	void AddClient(VerifyClient* client);

	/**
	*@brief ɾ��Client
	*/
	void DelClient(UInt32 id);

public:
	/**
	*@brief �㲥��Ϣ
	*/
	void Broadcast(Avalon::Protocol& protocol);
	void Broadcast(Avalon::Packet* packet);

private:
	//���ػỰmap
	std::map<UInt32, VerifyClient*>		m_Clients;
};

#endif
