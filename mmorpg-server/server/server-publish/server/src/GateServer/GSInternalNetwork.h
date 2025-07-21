#ifndef _GS_INTERNAL_NETWORK_H_
#define _GS_INTERNAL_NETWORK_H_

#include <AvalonSingleton.h>
#include <CLInternalNetwork.h>

/**
 *@brief ��������
 */
class GSInternalConnection : public CLInternalConnection
{
public:
	GSInternalConnection(CLNetwork* network);
	~GSInternalConnection();

public:
	bool PreHandlePacket(Avalon::Packet* packet);
};

/**
 *@brief ����
 */
class GSInternalNetwork : public CLInternalNetwork, public Avalon::Singleton<GSInternalNetwork>
{
public:
	GSInternalNetwork();
	~GSInternalNetwork();

public:
	void DispatchPacket(Avalon::Packet* packet);

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

	/**
	 *@brief ֹͣ����
	 */
	virtual void OnShutdownService();
};

#endif
