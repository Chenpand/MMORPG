#ifndef __RELAY_SESSION_H__
#define __RELAY_SESSION_H__

#include <AvalonNetConnection.h>
#include <AvalonProtocol.h>
#include <AvalonTime.h>
#include <CLDefine.h>
#include <CLSysProtocol.h>

/**
*@brief RelayServer会话
*/
class RelaySession
{
public:
	RelaySession();
	~RelaySession();

public:
	/**
	*@brief 初始化
	*/
	bool Init(Int32 id);

	/**
	*@brief 获取节点id
	*/
	inline UInt32 GetNodeID() const { return m_NodeId; }

	inline UInt32 GetAbality() const { return m_Abality; }
	inline void SetAbality(int abality) { m_Abality = abality; }

public:
	/**
	*@brief tick
	*/
	void OnTick(const Avalon::Time& now);

public:
	/**
	*@brief 发送协议
	*/
	void SendProtocol(Avalon::Protocol& protocol);
	void SendPacket(Avalon::Packet* packet);

private:
	// 节点id
	UInt32						m_NodeId;

	// 连接
	Avalon::NetConnectionPtr	m_Conn;

	// 负载
	UInt32						m_Abality;

    
};

#endif