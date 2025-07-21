#ifndef __VC_INTERNAL_NETWORK_H__
#define __VC_INTERNAL_NETWORK_H__

#include <AvalonSingleton.h>
#include "CLInternalNetwork.h"

/**
*@brief 内网
*/
class VCInternalNetwork : public CLInternalNetwork, public Avalon::Singleton<VCInternalNetwork>
{
public:
	VCInternalNetwork();
	~VCInternalNetwork();

public:
	bool Startup(Avalon::PropertySet *properties, CLNetAddress *addresses);

public:
	void RegisterProtocols();

	void DispatchPacket(Avalon::Packet* packet);

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