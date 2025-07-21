#ifndef _GS_EXTERNAL_NETWORK_H_
#define _GS_EXTERNAL_NETWORK_H_


#include <AvalonSingleton.h>
#include <CLExternalNetwork.h>

/**
 *@brief ��������
 */
class GSExternalConnection : public CLExternalConnection
{
public:
	GSExternalConnection(CLNetwork* network);
	~GSExternalConnection();

public:
	bool PreHandlePacket(Avalon::Packet* packet);

public://�¼�
	/**
	 *@brief �����¼�
	 */
	virtual void OnTick(const Avalon::Time& now);

private:
	//�˺�id
	UInt32	m_AccId;
	//�Ƿ��ڵȴ����߹�����
	bool	m_bWaitOffine;
};

/**
 *@brief ����
 */
class GSExternalNetwork : public CLExternalNetwork, public Avalon::Singleton<GSExternalNetwork>
{
public:
	GSExternalNetwork();
	~GSExternalNetwork();

public:
	/**
	 *@brief �ַ���Ϣ��
	 */
	bool ExecutePacket(Avalon::Packet* packet);

	void RegisterProtocols();

	/**
	 *@brief ����һ������
	 */
	Avalon::NetConnection* CreateConnection();

public://�¼�
	/**
	 *@brief һ�����ӽ���
	 */
	virtual void OnConnected(UInt32 id);

	/**
	 *@brief һ�����ӶϿ�
	 */
	virtual void OnDisconnected(UInt32 id);
};


#endif
