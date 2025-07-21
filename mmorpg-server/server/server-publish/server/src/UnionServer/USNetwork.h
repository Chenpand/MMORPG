#ifndef _US_NETWORK_H_
#define _US_NETWORK_H_

#include <AvalonSingleton.h>
#include "CLInternalNetwork.h"

/**
 *@brief ����
 */
class USNetwork : public CLInternalNetwork, public Avalon::Singleton<USNetwork>
{
public:
	USNetwork();
	~USNetwork();

public:

	void RegisterProtocols();

	void DispatchPacket(Avalon::Packet* packet);

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
