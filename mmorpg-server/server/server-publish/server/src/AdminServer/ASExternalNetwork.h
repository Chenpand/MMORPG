#ifndef _AS_EXTERNAL_NETWORK_H_
#define _AS_EXTERNAL_NETWORK_H_

#include <AvalonSingleton.h>
#include <CLExternalNetwork.h>

/**
 *@brief ��������
 */
class ASExternalConnection : public CLExternalConnection
{
public:
	ASExternalConnection(CLNetwork* network);
	~ASExternalConnection();

public:
	bool PreHandlePacket(Avalon::Packet* packet);

	void OnTick(const Avalon::Time& now);

private:
	/**
	 *@brief ��֤�˺�
	 */
	bool VerifyAccount(const UInt8* _hashval, const std::string& uparams, const std::string& key1, const std::string& key2);

private:
	//��֤ͨ��ʱ��
	UInt32	m_VerifyTime;

	//�˺�id
	UInt32		m_AccId;
	//openid
	std::string	m_OpenId;
	//qq��Ȩ��Ϣ
	QQVipInfo	m_qqVipInfo;
	//hashʱ���
	UInt32		m_HashTimestamp;
	//gmȨ��
	UInt8		m_GmAuthority;
	//ƽ̨
	std::string	m_Pf;
	//�豸ID
	std::string m_DeviceId;
	//���ֻ��Ľ�ɫID
	ObjID_t		m_PhoneBindRoleId;
	//�Ƿ���֤���(δ��֤���ο�)
	UInt32		m_IsIDAuth;
	//����ʱ��
	UInt32		m_AddictSecTime;
	//����
	UInt8		m_Age;
	//����¼ʱ��
	UInt32		m_LastLoginTime;
	//�豸�ͺ�
	std::string	m_Model;
	//�豸�汾
	std::string m_DeviceVersion;
};

/**
 *@brief ����
 */
class ASExternalNetwork : public CLExternalNetwork, public Avalon::Singleton<ASExternalNetwork>
{
public:
	ASExternalNetwork();
	~ASExternalNetwork();

public:

	void RegisterProtocols();

	/**
	 *@brief ����һ������
	 */
	Avalon::NetConnection* CreateConnection();

public://�¼�
	/**
	 *@brief һ�����ӽ���
	 */
	virtual void OnConnected(UInt32 id);

	/**
	 *@brief һ�����ӶϿ�
	 */
	virtual void OnDisconnected(UInt32 id);
};

#endif
