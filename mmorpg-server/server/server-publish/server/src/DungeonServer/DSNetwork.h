#ifndef _DS_NETWORK_H_
#define _DS_NETWORK_H_

#include <AvalonSingleton.h>
#include "CLInternalNetwork.h"

/**
 *@brief ����
 */
class DSNetwork : public CLInternalNetwork, public Avalon::Singleton<DSNetwork>
{
public:
	DSNetwork();
	~DSNetwork();

public:

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
