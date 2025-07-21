#ifndef _CL_RL_INTERNALNETWORK_H_
#define _CL_RL_INTERNALNETWORK_H_

#include <AvalonSingleton.h>
#include <CLInternalNetwork.h>



class RLInternalConnection : public CLInternalConnection
{
public:
	RLInternalConnection(CLNetwork* network);
	~RLInternalConnection();

	//预处理RelayServer与Match Server以及其他服务器的通讯， 比如：处理与Match Server之间的注册消息
	bool PreHandlePacket(Avalon::Packet* packet);
protected:
private:
};

class RLInternalNetwork : public CLInternalNetwork, public Avalon::Singleton<RLInternalNetwork>
{
public:
	RLInternalNetwork();
	~RLInternalNetwork();


	Avalon::NetConnection* CreateConnection();

public://事件
	/**
	*@brief 一个连接建立 处理网络层net--> 逻辑层logic的 SYS_NOTIFY_CONNECTED 链接建立协议 (实现CLNetwork相应的虚接口, 此处表示nodeid 为id的 服务器已经链接成功)
	*/
	virtual void OnConnected(UInt32 id);

	/**
	*@brief 一个连接断开 处理网络层net -> 逻辑层logic的 SYS_NOTIFY_DISCONNECTED 链接断开协议
	*/
	virtual void OnDisconnected(UInt32 id);

	/**
	*@brief 停止服务  处理收到的停服通知协议 SYS_NOTIFY_SHUTDOWN
	*/
	virtual void OnShutdownService();
	/*
	*/
	bool PreHandlePacket(CLNetConnection* conn, Avalon::Packet* packet);

	/*
	*/
	void RegisterProtocols();
protected:

	void register2matchsvr();

private:
	bool m_reconnect;				//是否是断线重连
};



#endif
