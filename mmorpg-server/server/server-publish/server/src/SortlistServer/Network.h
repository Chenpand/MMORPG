#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <AvalonSingleton.h>
#include "CLInternalNetwork.h"

/**
 *@brief 内网
 */
class Network : public CLInternalNetwork, public Avalon::Singleton<Network>
{
public:
	Network();
	~Network();

public:

	void RegisterProtocols();

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

public:
	/**
	 *@brief 处理玩家消息                                                                     
	 */
	bool ExecutePlayerPacket(Avalon::Packet* packet, ObjID_t playerId) const;

	/**
	 *@brief 注册玩家协议                                                                   
	 */
	void RegisterPlayerProtocols();

private:
	//玩家消息处理
	CLParamPacketExecutor<ObjID_t>	m_Executor;
};

#endif
