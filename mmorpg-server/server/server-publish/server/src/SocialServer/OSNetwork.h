#ifndef _OS_NETWORK_H_
#define _OS_NETWORK_H_

#include <AvalonSingleton.h>
#include "CLInternalNetwork.h"

/**
 *@brief ����
 */
class OSNetwork : public CLInternalNetwork, public Avalon::Singleton<OSNetwork>
{
public:
	OSNetwork();
	~OSNetwork();

public:

	void RegisterProtocols();

	void TransmitProtocol(UInt32 connId, UInt32 nodeId, Avalon::Protocol& protocol);

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
