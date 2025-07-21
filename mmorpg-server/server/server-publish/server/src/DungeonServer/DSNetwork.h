#ifndef _DS_NETWORK_H_
#define _DS_NETWORK_H_

#include <AvalonSingleton.h>
#include "CLInternalNetwork.h"

/**
 *@brief 内网
 */
class DSNetwork : public CLInternalNetwork, public Avalon::Singleton<DSNetwork>
{
public:
	DSNetwork();
	~DSNetwork();

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
};

#endif
