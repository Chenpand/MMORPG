#ifndef _SERVICE_SESSION_H_
#define _SERVICE_SESSION_H_

#include <CLObject.h>
#include <AvalonNetConnection.h>
#include <AvalonProtocol.h>

struct ServiceSessionKey
{
	ServiceSessionKey()
	: serviceType(0), serviceId(0) {}
	ServiceSessionKey(UInt8 serviceType_, UInt8 serverId_)
	: serviceType(serviceType_), serviceId(serverId_) {}

	bool operator<(const ServiceSessionKey& other) const
	{
		if (other.serviceType != serviceType)
		{
			return serviceType < other.serviceType;
		}
		return serviceId < other.serviceId;
	}

	UInt8 serviceType;
	UInt8 serviceId;
};

/**
 *@brief �淨������
 */
class ServiceSession
{
public:
	ServiceSession();
	~ServiceSession();

public:
	/**
	 *@brief ���û�ȡid
	 */
	bool Init(UInt32 id);
	UInt32 GetID() const{ return m_Id; }

	const ServiceSessionKey& GetKey() const { return m_Key; };
public: //��Ϣ���
	/**
	 *@brief ������Ϣ
	 */
	void SendPacket(Avalon::Packet* packet);
	void SendProtocol(Avalon::Protocol& protocol);

private:
	//id
	UInt32	m_Id;
	//����
	ServiceSessionKey	m_Key;
	//�淨����������
	Avalon::NetConnectionPtr m_Conn;
};

typedef CLVisitor<ServiceSession>	ServiceSessionVisitor;

#endif
