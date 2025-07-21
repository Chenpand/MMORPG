#ifndef __VERIFY_CLIENT_H__
#define __VERIFY_CLIENT_H__

#include <CLDefine.h>
#include <AvalonProtocol.h>
#include <AvalonNetConnection.h>

class VerifyClient
{
public:
	/**
	*@brief ��ʼ��
	*/
	bool Init(Int32 id, ServerType type);

	/**
	*@brief ��ȡ�ڵ�id
	*/
	inline UInt32 GetID() const { return m_Id; }

public:
	/**
	*@brief ����Э��
	*/
	void SendProtocol(Avalon::Protocol& protocol);
	void SendPacket(Avalon::Packet* packet);

private:
	// �ڵ�id
	UInt32						m_Id;

	// ����������
	ServerType					m_ServerType;

	// ����
	Avalon::NetConnectionPtr	m_Conn;
};

#endif