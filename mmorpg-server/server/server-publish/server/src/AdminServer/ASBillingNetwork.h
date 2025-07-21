#ifndef _AS_BILLING_NETWORK_H_
#define _AS_BILLING_NETWORK_H_

#include <AvalonSingleton.h>
#include "CLNetwork.h"

class CLNetAddress;

/**
 *@brief 支付连接
 */
class BillingConnection : public CLNetConnection
{
public:
	explicit BillingConnection(CLNetwork* network);
	~BillingConnection();

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
 *@brief 支付网络
 */
class BillingNetwork : public CLNetwork, public Avalon::Singleton<BillingNetwork>
{
public:
	BillingNetwork();
	~BillingNetwork();

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
	NetworkType GetType() const{ return NT_BILLING; }

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
