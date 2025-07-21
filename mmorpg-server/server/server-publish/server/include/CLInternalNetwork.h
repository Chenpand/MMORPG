#ifndef _CL_INTERNAL_NETWORK_H_
#define _CL_INTERNAL_NETWORK_H_

#include <AvalonSimpleTimer.h>
#include "CLNetwork.h"
#include "CLNetAddress.h"

/**
 *@brief 内网连接
 */
class CLInternalConnection : public CLNetConnection
{
public:
	explicit CLInternalConnection(CLNetwork* network);
	~CLInternalConnection();

public:
	/**
	 *@brief 预处理消息包 进行消息验证
	 */
	bool PreHandlePacket(Avalon::Packet* packet);

	/**
	 *@brief 在此处理加密
	 */
	void ProcessSendPacket(Avalon::Packet *&packet);
};



/**
 *@brief 重连信息
 */
struct ReconnectTimer
{
	ReconnectTimer(){ nodeId = 0; }

	//定时器
	Avalon::SimpleTimer timer;
	//节点id
	UInt32	nodeId;
};

/**
 *@brief 内网网络
 */
class CLInternalNetwork : public CLNetwork
{
	typedef std::vector<ReconnectTimer> ReconnectTimerVec;

	//连接其他服务器等待时间
	const static UInt32	CONNECT_TIMEWAIT = 5;

public:
	CLInternalNetwork();
	~CLInternalNetwork();

public:
	/**
	 *@brief 启动
	 */
	bool Startup(Avalon::PropertySet* properties,CLNetAddress* addresses);

	/**
	 *@brief 关闭
	 */
	void Shutdown(UInt32 inputTimeout);

	/**
	 *@brief tick事件
	 */
	void OnTick(const Avalon::Time& now);

	/**
	 *@brief 获取类型
	 */
	NetworkType GetType() const { return NT_INTERVAL; }

	/**
	 *@brief 预处理消息包
	 */
	bool PreHandlePacket(CLNetConnection* conn,Avalon::Packet* packet);

	/**
	 *@brief 广播到每种类型的服务器
	 */
	void Broadcast(ServerType type,Avalon::Packet* packet);

public:
	/**
	 *@brief 创建连接对象
	 */
	Avalon::NetConnection* CreateConnection();

protected: //事件
	/**
	 *@brief 关闭服务
	 */
	virtual void OnShutdownService() = 0;

protected:
	/**
	 *@brief 检查关闭服务
	 */
	void CheckShutdownService();

	/**
	 *@brief 从管理服务器获取信息
	 */
	bool GetInfoFromAdminServer();

	/**
	 *@brief 连接到服务器
	 */
	void ConnectToServers();

	/**
	 *@brief 判断是否都建立连接了
	 */
	bool IsAllConnected();

	/**
	 *@brief 执行一个消息包
	 */
	bool ExecutePacket(Avalon::Packet* packet);

private:
	//网络地址
	CLNetAddress *	m_pAddress;   //其他服务器通过向Admin Server发送SysNetAddressReq 请求获得
	//adminServer地址
	Avalon::SockAddr m_AdminAddr;
	//连接的地址
	SockAddrMap		m_ConnectAddrs;
	//是否从adminserver获取到地址
	bool	m_bGotInfo;

	//重连定时器  根据断线后连接的处理策略(具体见static DisconnectHandleCfg DISCONNECT_HANDLE_CFG[] = )，若需要重连，则设置一个ReconnectTimer定时器
	ReconnectTimerVec	m_ReconnectTimers;
	//通知关闭定时器
	Avalon::SimpleTimer	m_ShutdownTimer;
	//是否在关闭服务器过程中
	bool	m_bWaitShutdown;
};


#endif
