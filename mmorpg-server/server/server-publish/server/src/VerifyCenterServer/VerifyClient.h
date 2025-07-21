#ifndef __VERIFY_CLIENT_H__
#define __VERIFY_CLIENT_H__

#include <CLDefine.h>
#include <AvalonProtocol.h>
#include <AvalonNetConnection.h>

class VerifyClient
{
public:
	/**
	*@brief 初始化
	*/
	bool Init(Int32 id, ServerType type);

	/**
	*@brief 获取节点id
	*/
	inline UInt32 GetID() const { return m_Id; }

public:
	/**
	*@brief 发送协议
	*/
	void SendProtocol(Avalon::Protocol& protocol);
	void SendPacket(Avalon::Packet* packet);

private:
	// 节点id
	UInt32						m_Id;

	// 服务器类型
	ServerType					m_ServerType;

	// 连接
	Avalon::NetConnectionPtr	m_Conn;
};

#endif