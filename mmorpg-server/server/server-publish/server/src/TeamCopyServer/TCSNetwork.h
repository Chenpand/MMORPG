#ifndef _TCS_NETWORK_H_
#define _TCS_NETWORK_H_

#include <AvalonSingleton.h>
#include "CLInternalNetwork.h"

/**
 *@brief ����
 */
class TCSNetwork : public CLInternalNetwork, public Avalon::Singleton<TCSNetwork>
{
public:
	TCSNetwork();
	~TCSNetwork();

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
