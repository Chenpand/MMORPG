#ifndef _BS_NETWORK_H_
#define _BS_NETWORK_H_

#include <AvalonSingleton.h>
#include "CLInternalNetwork.h"

/**
 *@brief ����
 */
class BSNetwork : public CLInternalNetwork, public Avalon::Singleton<BSNetwork>
{
public:
	BSNetwork();
	~BSNetwork();

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

public:
	/**
	*@brief ���͵�relay
	*/
	//void SendToRelay(Avalon::Protocol& protocol);
	//void SendToRelay(Avalon::Packet* packet);
	/**
	*@brief ���͵�bscene
	*/
	void SendToBScene(UInt32 nodeId, Avalon::Protocol& protocol);
	void SendToBScene(UInt32 nodeId, Avalon::Packet* packet);

private:
	//Avalon::NetConnectionPtr m_RelayConn;
};

#endif
