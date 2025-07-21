#ifndef _TFS_NET_WORK_H_
#define _TFS_NET_WORK_H_

#include <AvalonSingleton.h>
#include "CLInternalNetwork.h"

/**
 *@brief 内网
 */
class TFSNetwork : public CLInternalNetwork, public Avalon::Singleton<TFSNetwork>
{
public:
	TFSNetwork();
	~TFSNetwork();

public:

	void RegisterProtocols();

	void DispatchPacket(Avalon::Packet* packet);

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
