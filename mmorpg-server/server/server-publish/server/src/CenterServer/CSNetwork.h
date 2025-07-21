#ifndef _CS_NETWORK_H_
#define _CS_NETWORK_H_

#include <AvalonSingleton.h>
#include "CLInternalNetwork.h"

/**
 *@brief 内网
 */
class CSNetwork : public CLInternalNetwork, public Avalon::Singleton<CSNetwork>
{
public:
	CSNetwork();
	~CSNetwork();

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
