#ifndef _LS_NETWORK_H_
#define _LS_NETWORK_H_

#include <AvalonSingleton.h>
#include <AvalonSocket.h>
#include "CLNetwork.h"

/**
 *@brief 日志连接
 */
class LogConnection : public CLNetConnection
{
public:
	explicit LogConnection(CLNetwork* network);
	~LogConnection();

public:
	void OnDataReached();

	void OnTick(const Avalon::Time& now);

	bool PreHandlePacket(Avalon::Packet* packet);

private:
	static UInt32 GenID();
};

/**
 *@brief 日志网络
 */
class LogNetwork : public CLNetwork, public Avalon::Singleton<LogNetwork>
{
public:
	LogNetwork();
	~LogNetwork();

public:
	/**
	 *@brief 创建一个连接
	 */
	Avalon::NetConnection* CreateConnection();

	/**
	 *@brief 启动
	 */
	bool Startup(Avalon::PropertySet* properties);

	/**
	 *@brief 关闭
	 */
	void Shutdown(UInt32 inputTimeout);

	NetworkType GetType() const{ return NT_EXTERVAL; }


public://事件
	void OnTick(const Avalon::Time& now);
	
	void OnConnected(UInt32 id);

	void OnDisconnected(UInt32 id);

private:
	void ProcessDataLogPacket(Avalon::Packet* packet, const Avalon::SockAddr& addr);

private:
	//udp套接字
	Avalon::Socket m_UdpSock;
};

#endif
