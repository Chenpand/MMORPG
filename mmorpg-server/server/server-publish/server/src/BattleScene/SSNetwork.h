#ifndef _SS_NETWORK_H_
#define _SS_NETWORK_H_

#include <AvalonSingleton.h>
#include <CLInternalNetwork.h>

/**
 *@brief 内网
 */
class SSNetwork : public CLInternalNetwork, public Avalon::Singleton<SSNetwork>
{
public:
	SSNetwork();
	~SSNetwork();

public:
	void DispatchPacket(Avalon::Packet* packet);

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
};

#endif
