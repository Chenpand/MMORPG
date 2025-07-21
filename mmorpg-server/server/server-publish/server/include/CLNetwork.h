#ifndef _CL_NETWORK_H_
#define _CL_NETWORK_H_

#include <AvalonNetService.h>
#include <AvalonPropertySet.h>
#include <AvalonProtocol.h>
#include "CLDefine.h"
#include "CLPacketExecutor.h"

/**
 *@brief 网络类型
 */
enum NetworkType
{
	NT_INTERVAL,	//内网
	NT_EXTERVAL,	//外网
	NT_CONSOLE,		//控制台
	NT_BILLING,		//支付
};

/**
 *@brief 获取网络名
 */
inline const char* GetNetworkNameByType(int type)
{
	switch(type)
	{
	case NT_INTERVAL:	return "InternalNetwork";
	case NT_EXTERVAL:	return "ExternalNetwork";
	case NT_CONSOLE:	return "ConsoleNetwork";
	case NT_BILLING:	return "BillingNetwork";
	}
	return "";
}


class CLNetwork;

extern bool CL_SEND_PROTOCOL(Avalon::NetConnectionPtr Conn, Avalon::Protocol& Req);
/**
 *@brief 网络连接
 */
class CLNetConnection : public Avalon::NetConnection
{
public:
	explicit CLNetConnection(CLNetwork* network);
	~CLNetConnection();

	void OnDataReached();  //在 NetConnection::HandleInput中被调用， reactor 检测到可读事件

	void OnTick(const Avalon::Time& now);

	CLNetwork* GetNetwork(){ return m_pNetwork; }

	bool SendProtocol(Avalon::Protocol& protocol);

private:
	//网络
	CLNetwork *m_pNetwork;
};


/**
 *@brief 网络
 */
class CLNetwork : public Avalon::NetService
{
public:
	CLNetwork();
	virtual ~CLNetwork();

public:
	/**
	 *@brief 启动网络服务器
	 */
	bool Startup(Avalon::PropertySet* properties);

	/**
	 *@brief 关闭网络服务
	 */
	void Shutdown(UInt32 inputTimeout);

	/**
	 *@brief 网络网络类型
	 */
	virtual NetworkType GetType() const = 0;

	/**
	 *@brief 获取网络名
	 */
	virtual const std::string GetName() const;

	/**
	 *@brief 获取一个指定类型的连接
	 */
	Avalon::NetConnectionPtr FindConnectionByType(ServerType type);

	/**
	 *@brief 获取一个指定类型的连接列表
	 */
	void FindConnectionByType(ServerType type, std::vector<Avalon::NetConnectionPtr>& connVec);

	/**
	 *@brief 预处理消息
	 */
	virtual bool PreHandlePacket(CLNetConnection* conn,Avalon::Packet* packet);

	/**
	 *@brief 分发消息
	 */
	virtual void DispatchPacket(Avalon::Packet* packet);

	/**
	 *@brief 获取消息处理器                                                                     
	 */
	CLPacketExecutor& GetPacketExecutor() { return m_Executor; }

public://事件
	/**
	 *@brief 创建一个网络连接
	 */
	Avalon::NetConnection* CreateConnection();

	/**
	 *@brief 一个连接建立(已验证)
	 */
	void OnAsynConnected(UInt32 id, Avalon::NetConnectionType type);

	/**
	 *@brief 一个连接断开
	 */
	void OnAsynDisconnected(UInt32 id, Avalon::NetConnectionType type);

	/**
	 *@brief 一个连接建立  网络层通知逻辑层  已建立了链接 具体的服务器的网络层需要实现该接口
	 */
	virtual void OnConnected(UInt32 id)  = 0;

	/**
	 *@brief 一个连接断开
	 */
	virtual void OnDisconnected(UInt32 id) = 0;

	/**
	 *@brief tick事件
	 */
	virtual void OnTick(const Avalon::Time& now);

protected:
	/**
	 *@brief 注册协议
	 */
	virtual void RegisterProtocols(){}

	/**
	 *@brief 绑定一个协议  也就是把协议id和对该协议的处理函数绑定一起
	 */
	void RegisterProtocol(UInt32 id,CLPacketFunction* func){ m_Executor.Bind(id,func); }

	/**
	 *@brief 执行一个消息包
	 */
	virtual bool ExecutePacket(Avalon::Packet* packet);

private:
	//默认的消息队列  用此消息队列实现了一个消息派发机制（异步），在OnTick时进行实际的消息处理
	Avalon::PacketQueue m_Queue;
	//消息包执行器
	CLPacketExecutor	m_Executor;
};

#endif
