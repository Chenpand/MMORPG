#ifndef _CS_NETWORK_H_
#define _CS_NETWORK_H_

#include <AvalonSingleton.h>
#include "CLInternalNetwork.h"

/**
 *@brief ����
 */
class CSNetwork : public CLInternalNetwork, public Avalon::Singleton<CSNetwork>
{
public:
	CSNetwork();
	~CSNetwork();

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
