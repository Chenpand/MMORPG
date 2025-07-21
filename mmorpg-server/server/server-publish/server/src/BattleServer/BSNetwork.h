#ifndef _BS_NETWORK_H_
#define _BS_NETWORK_H_

#include <AvalonSingleton.h>
#include "CLInternalNetwork.h"

/**
 *@brief 内网
 */
class BSNetwork : public CLInternalNetwork, public Avalon::Singleton<BSNetwork>
{
public:
	BSNetwork();
	~BSNetwork();

public:

	void RegisterProtocols();

public://事件
	/**
	 *@brief 一个连接建立
	 */
	virtual void OnConnected(UInt32 id);

	/**
	 *@brief 一个连接断开
	 */
	virtual void OnDisconnected(UInt32 id);

	/**
	 *@brief 停止服务
	 */
	virtual void OnShutdownService();

public:
	/**
	*@brief 发送到relay
	*/
	//void SendToRelay(Avalon::Protocol& protocol);
	//void SendToRelay(Avalon::Packet* packet);
	/**
	*@brief 发送到bscene
	*/
	void SendToBScene(UInt32 nodeId, Avalon::Protocol& protocol);
	void SendToBScene(UInt32 nodeId, Avalon::Packet* packet);

private:
	//Avalon::NetConnectionPtr m_RelayConn;
};

#endif
