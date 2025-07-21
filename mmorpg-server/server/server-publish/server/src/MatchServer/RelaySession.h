#ifndef __RELAY_SESSION_H__
#define __RELAY_SESSION_H__

#include <AvalonNetConnection.h>
#include <AvalonProtocol.h>
#include <AvalonTime.h>
#include <CLDefine.h>
#include <CLSysProtocol.h>

/**
*@brief RelayServer�Ự
*/
class RelaySession
{
public:
	RelaySession();
	~RelaySession();

public:
	/**
	*@brief ��ʼ��
	*/
	bool Init(Int32 id);

	/**
	*@brief ��ȡ�ڵ�id
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
	*@brief ����Э��
	*/
	void SendProtocol(Avalon::Protocol& protocol);
	void SendPacket(Avalon::Packet* packet);

private:
	// �ڵ�id
	UInt32						m_NodeId;

	// ����
	Avalon::NetConnectionPtr	m_Conn;

	// ����
	UInt32						m_Abality;

    
};

#endif