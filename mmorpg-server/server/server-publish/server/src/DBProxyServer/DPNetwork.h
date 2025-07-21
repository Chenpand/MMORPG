#ifndef _DP_NETWORK_H_
#define _DP_NETWORK_H_

#include <AvalonSingleton.h>
#include "CLInternalNetwork.h"

/**
 *@brief ����
 */
class Network : public CLInternalNetwork, public Avalon::Singleton<Network>
{
public:
	Network();
	~Network();

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

private:
	//�����Ϣ����
	CLParamPacketExecutor<ObjID_t>	m_Executor;
};

#endif
