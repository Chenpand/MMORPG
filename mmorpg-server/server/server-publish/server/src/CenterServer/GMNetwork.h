#ifndef _GM_NETWORK_H_
#define _GM_NETWORK_H_

#include <AvalonSingleton.h>
#include <AvalonPacket.h>
#include <AvalonIDGenerator.h>
#include <CLExternalNetwork.h>

/**
 *@brief 外网连接
 */
class GMConnection : public CLExternalConnection
{
public:
	GMConnection(CLNetwork* network);
	~GMConnection();

public:
	bool PreHandlePacket(Avalon::Packet* packet);

	virtual void OnTick(const Avalon::Time& now);

private:
};

/**
 *@brief 外网
 */
class GMNetwork : public CLExternalNetwork, public Avalon::Singleton<GMNetwork>
{
public:
	GMNetwork();
	~GMNetwork();

public:
	/**
	 *@brief 启动
	 */
	bool Startup(Avalon::PropertySet* properties,CLNetAddress* addresses = NULL);

	/**
	 *@brief 获取口令
	 */
	const std::string& GetPasswd() const{ return m_Passwd; }

	/**
	 *@brief 生成一个id
	 */
	UInt32 GenID(){ return m_IDGen.Generate(); }

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

private:
	/**
	 *@brief 登陆密码
	 */
	std::string m_Passwd;
	/**
	 *@brief id生成器
	 */
	Avalon::IDGenerator m_IDGen;
};

#endif
