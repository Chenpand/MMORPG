#ifndef _SS_NETWORK_H_
#define _SS_NETWORK_H_

#include <AvalonSingleton.h>
#include <CLInternalNetwork.h>

/**
 *@brief ����
 */
class SSNetwork : public CLInternalNetwork, public Avalon::Singleton<SSNetwork>
{
public:
	SSNetwork();
	~SSNetwork();

public:
	void DispatchPacket(Avalon::Packet* packet);

	void RegisterProtocols();

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
