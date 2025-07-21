#ifndef _OBS_NETWORK_H_
#define _OBS_NETWORK_H_

#include <AvalonSingleton.h>
#include "CLInternalNetwork.h"

/**
*@brief 内网
*/
class OBSInternalNetwork : public CLInternalNetwork, public Avalon::Singleton<OBSInternalNetwork>
{
public:
	OBSInternalNetwork();
	~OBSInternalNetwork();

public:
	bool Startup(Avalon::PropertySet *properties, CLNetAddress *addresses);

public:
	void RegisterProtocols();

	/**
	*@brief 执行一个消息包
	*/
	bool ExecutePacket(Avalon::Packet* packet);

	bool PreHandlePacket(CLNetConnection* conn, Avalon::Packet* packet);

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

private:

};

#endif
