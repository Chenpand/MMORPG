#ifndef _RS_NETWORK_H_
#define _RS_NETWORK_H_

#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>
#include "CLInternalNetwork.h"

/**
 *@brief 内网
 */
class RSNetwork : public CLInternalNetwork, public Avalon::Singleton<RSNetwork>
{
public:
	RSNetwork();
	~RSNetwork();

public:

	void RegisterProtocols();

public://事件
	/**
	 *@brief 心跳事件
	 */
	virtual void OnTick(const Avalon::Time& now);

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

protected:
	/**
	 *@brief 执行一个消息包
	 */
	bool ExecutePacket(Avalon::Packet* packet);

private:
	//接收的包数
	UInt32	m_PacketNum;
	//接收的流量
	UInt32	m_ByteNum;
	//5分定时器
	Avalon::SimpleTimer	m_Timer;
};


#endif
