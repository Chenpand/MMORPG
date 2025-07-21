#ifndef _CL_INTERNAL_NETWORK_H_
#define _CL_INTERNAL_NETWORK_H_

#include <AvalonSimpleTimer.h>
#include "CLNetwork.h"
#include "CLNetAddress.h"

/**
 *@brief ��������
 */
class CLInternalConnection : public CLNetConnection
{
public:
	explicit CLInternalConnection(CLNetwork* network);
	~CLInternalConnection();

public:
	/**
	 *@brief Ԥ������Ϣ�� ������Ϣ��֤
	 */
	bool PreHandlePacket(Avalon::Packet* packet);

	/**
	 *@brief �ڴ˴������
	 */
	void ProcessSendPacket(Avalon::Packet *&packet);
};



/**
 *@brief ������Ϣ
 */
struct ReconnectTimer
{
	ReconnectTimer(){ nodeId = 0; }

	//��ʱ��
	Avalon::SimpleTimer timer;
	//�ڵ�id
	UInt32	nodeId;
};

/**
 *@brief ��������
 */
class CLInternalNetwork : public CLNetwork
{
	typedef std::vector<ReconnectTimer> ReconnectTimerVec;

	//���������������ȴ�ʱ��
	const static UInt32	CONNECT_TIMEWAIT = 5;

public:
	CLInternalNetwork();
	~CLInternalNetwork();

public:
	/**
	 *@brief ����
	 */
	bool Startup(Avalon::PropertySet* properties,CLNetAddress* addresses);

	/**
	 *@brief �ر�
	 */
	void Shutdown(UInt32 inputTimeout);

	/**
	 *@brief tick�¼�
	 */
	void OnTick(const Avalon::Time& now);

	/**
	 *@brief ��ȡ����
	 */
	NetworkType GetType() const { return NT_INTERVAL; }

	/**
	 *@brief Ԥ������Ϣ��
	 */
	bool PreHandlePacket(CLNetConnection* conn,Avalon::Packet* packet);

	/**
	 *@brief �㲥��ÿ�����͵ķ�����
	 */
	void Broadcast(ServerType type,Avalon::Packet* packet);

public:
	/**
	 *@brief �������Ӷ���
	 */
	Avalon::NetConnection* CreateConnection();

protected: //�¼�
	/**
	 *@brief �رշ���
	 */
	virtual void OnShutdownService() = 0;

protected:
	/**
	 *@brief ���رշ���
	 */
	void CheckShutdownService();

	/**
	 *@brief �ӹ����������ȡ��Ϣ
	 */
	bool GetInfoFromAdminServer();

	/**
	 *@brief ���ӵ�������
	 */
	void ConnectToServers();

	/**
	 *@brief �ж��Ƿ񶼽���������
	 */
	bool IsAllConnected();

	/**
	 *@brief ִ��һ����Ϣ��
	 */
	bool ExecutePacket(Avalon::Packet* packet);

private:
	//�����ַ
	CLNetAddress *	m_pAddress;   //����������ͨ����Admin Server����SysNetAddressReq ������
	//adminServer��ַ
	Avalon::SockAddr m_AdminAddr;
	//���ӵĵ�ַ
	SockAddrMap		m_ConnectAddrs;
	//�Ƿ��adminserver��ȡ����ַ
	bool	m_bGotInfo;

	//������ʱ��  ���ݶ��ߺ����ӵĴ������(�����static DisconnectHandleCfg DISCONNECT_HANDLE_CFG[] = )������Ҫ������������һ��ReconnectTimer��ʱ��
	ReconnectTimerVec	m_ReconnectTimers;
	//֪ͨ�رն�ʱ��
	Avalon::SimpleTimer	m_ShutdownTimer;
	//�Ƿ��ڹرշ�����������
	bool	m_bWaitShutdown;
};


#endif
