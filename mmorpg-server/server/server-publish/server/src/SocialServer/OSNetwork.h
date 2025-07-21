#ifndef _OS_NETWORK_H_
#define _OS_NETWORK_H_

#include <AvalonSingleton.h>
#include "CLInternalNetwork.h"

/**
 *@brief 内网
 */
class OSNetwork : public CLInternalNetwork, public Avalon::Singleton<OSNetwork>
{
public:
	OSNetwork();
	~OSNetwork();

public:

	void RegisterProtocols();

	void TransmitProtocol(UInt32 connId, UInt32 nodeId, Avalon::Protocol& protocol);

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
};

#endif
