#ifndef _GS_INTERNAL_NETWORK_H_
#define _GS_INTERNAL_NETWORK_H_

#include <AvalonSingleton.h>
#include <CLInternalNetwork.h>

/**
 *@brief 内网连接
 */
class GSInternalConnection : public CLInternalConnection
{
public:
	GSInternalConnection(CLNetwork* network);
	~GSInternalConnection();

public:
	bool PreHandlePacket(Avalon::Packet* packet);
};

/**
 *@brief 内网
 */
class GSInternalNetwork : public CLInternalNetwork, public Avalon::Singleton<GSInternalNetwork>
{
public:
	GSInternalNetwork();
	~GSInternalNetwork();

public:
	void DispatchPacket(Avalon::Packet* packet);

	void RegisterProtocols();

	/**
	 *@brief 创建一个连接
	 */
	Avalon::NetConnection* CreateConnection();

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
