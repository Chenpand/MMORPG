#ifndef _GS_EXTERNAL_NETWORK_H_
#define _GS_EXTERNAL_NETWORK_H_


#include <AvalonSingleton.h>
#include <CLExternalNetwork.h>

/**
 *@brief 外网连接
 */
class GSExternalConnection : public CLExternalConnection
{
public:
	GSExternalConnection(CLNetwork* network);
	~GSExternalConnection();

public:
	bool PreHandlePacket(Avalon::Packet* packet);

public://事件
	/**
	 *@brief 心跳事件
	 */
	virtual void OnTick(const Avalon::Time& now);

private:
	//账号id
	UInt32	m_AccId;
	//是否处于等待下线过程中
	bool	m_bWaitOffine;
};

/**
 *@brief 外网
 */
class GSExternalNetwork : public CLExternalNetwork, public Avalon::Singleton<GSExternalNetwork>
{
public:
	GSExternalNetwork();
	~GSExternalNetwork();

public:
	/**
	 *@brief 分发消息包
	 */
	bool ExecutePacket(Avalon::Packet* packet);

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
};


#endif
