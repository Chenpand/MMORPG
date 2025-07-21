#ifndef _AS_NETWORK_H_
#define _AS_NETWORK_H_

#include <AvalonSingleton.h>
#include "CLInternalNetwork.h"
#include <CLProtocolDefine.h>

/**
 *@brief 内网
 */
class ASInternalNetwork : public CLInternalNetwork, public Avalon::Singleton<ASInternalNetwork>
{
public:
	ASInternalNetwork();
	~ASInternalNetwork();

public:
	bool Startup(Avalon::PropertySet *properties, CLNetAddress *addresses);

public:
	void RegisterProtocols();

	/**
	 *@brief 执行一个消息包
	 */
	bool ExecutePacket(Avalon::Packet* packet);

	bool PreHandlePacket(CLNetConnection* conn,Avalon::Packet* packet);


	/**
	 *@brief 获取一个指定服务器类型和功能的连接
	 */
	Avalon::NetConnectionPtr FindUnionConnectionByFun(UnionServerFunctionType funType);

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

	/**
	 *@brief 接到UnionServer通知其已开启的功能
	 */
	void OnUnionServerNotifyOpenFunction(UInt32 connID, std::vector<UInt32> funVec);

private:
	//是否连到center
	bool m_bCenterConnected;
	//功能对应的UnionServer连接id
	std::map<UnionServerFunctionType, UInt32> m_UnionFunctionConnID;
	UInt64 m_addID = 0;
};

#endif
