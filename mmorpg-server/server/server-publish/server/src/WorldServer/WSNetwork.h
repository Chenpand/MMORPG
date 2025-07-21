#ifndef _WS_NETWORK_H_
#define _WS_NETWORK_H_

#include <AvalonSingleton.h>
#include <CLInternalNetwork.h>
#include <CLTransfer.h>

/**
 *@brief 内网
 */
class WSNetwork : public CLInternalNetwork, public CLTransfer, public Avalon::Singleton<WSNetwork>
{
public:
	WSNetwork();
	~WSNetwork();

public:
	bool PreHandlePacket(CLNetConnection* conn, Avalon::Packet* packet);
	void DispatchPacket(Avalon::Packet* packet);

	void RegisterProtocols();

public:
	/**
	*@brief 随机选择一个场景服务器
	*/
	UInt32 SelectRandSceneServer();

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
	*@brief 注册中转消息
	*/
	virtual void RegisterTransferProtocols();
};

#endif
