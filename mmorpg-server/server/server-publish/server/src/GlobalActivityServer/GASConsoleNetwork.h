#ifndef _GAS_CONSOLE_NETWORK_H_
#define _GAS_CONSOLE_NETWORK_H_

#include <AvalonSingleton.h>
#include "CLNetwork.h"

class CLNetAddress;

/**
*@brief 控制台连接
*/
class ConsoleConnection : public CLNetConnection
{
public:
	explicit ConsoleConnection(CLNetwork* network);
	~ConsoleConnection();

public:
	void OnDataReached();

	void OnTick(const Avalon::Time& now);

	bool PreHandlePacket(Avalon::Packet* packet);

public:
	void EncodeHeader(UInt8* header, size_t len, UInt16 size, UInt32 id, UInt32 sequence);

	void DecodeHeader(UInt8* header, size_t len, UInt16& size, UInt32& id, UInt32& sequence);

private:
	static UInt32 GenID();
};

/**
*@brief 控制台网络
*/
class ConsoleNetwork : public CLNetwork, public Avalon::Singleton<ConsoleNetwork>
{
public:
	ConsoleNetwork();
	~ConsoleNetwork();

public:
	/**
	*@brief 创建一个连接
	*/
	Avalon::NetConnection* CreateConnection();

	/**
	*@brief 启动
	*/
	bool Startup(Avalon::PropertySet* properties, CLNetAddress* addresses = NULL);

	/**
	*@brief 关闭
	*/
	void Shutdown(UInt32 inputTimeout);

	/**
	*@brief 获取类型
	*/
	NetworkType GetType() const{ return NT_CONSOLE; }

public:
	/**
	*@brief 执行一个消息包
	*/
	bool ExecutePacket(Avalon::Packet* packet);

public://事件

	void OnConnected(UInt32 id);

	void OnDisconnected(UInt32 id);
};

#endif
