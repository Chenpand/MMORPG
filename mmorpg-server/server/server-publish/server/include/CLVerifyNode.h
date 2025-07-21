#ifndef __CL_VERIFY_NODE_H__
#define __CL_VERIFY_NODE_H__

#include <CLDefine.h>
#include <AvalonProtocol.h>
#include <AvalonNetConnection.h>
#include <CLVerifyDefine.h>

class CLVerifyNode
{
public:
	CLVerifyNode();
	~CLVerifyNode();

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_Id & m_Abality & m_Addr;
	}

public:
	/**
	*@brief 初始化
	*/
	bool Init(Int32 id, Avalon::SockAddr& addr, const Avalon::NetConnectionPtr conn);

	/**
	*@brief 获取节点id
	*/
	inline UInt32 GetID() const { return m_Id; }

	inline UInt32 GetAbality() const { return m_Abality; }
	inline void SetAbality(int abality) { m_Abality = abality; }
	inline Avalon::SockAddr& GetAddr() { return m_Addr; }

public:
	/**
	*@brief 发送协议
	*/
	void SendProtocol(Avalon::Protocol& protocol);
	void SendPacket(Avalon::Packet* packet);

private:
	// 节点id
	UInt32						m_Id;

	// 连接
	Avalon::NetConnectionPtr	m_Conn;

	// 负载
	UInt32						m_Abality;

	// 地址
	Avalon::SockAddr			m_Addr;
};

#endif