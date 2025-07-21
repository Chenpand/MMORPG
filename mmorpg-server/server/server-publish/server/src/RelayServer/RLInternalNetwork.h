#ifndef _CL_RL_INTERNALNETWORK_H_
#define _CL_RL_INTERNALNETWORK_H_

#include <AvalonSingleton.h>
#include <CLInternalNetwork.h>



class RLInternalConnection : public CLInternalConnection
{
public:
	RLInternalConnection(CLNetwork* network);
	~RLInternalConnection();

	//Ԥ����RelayServer��Match Server�Լ�������������ͨѶ�� ���磺������Match Server֮���ע����Ϣ
	bool PreHandlePacket(Avalon::Packet* packet);
protected:
private:
};

class RLInternalNetwork : public CLInternalNetwork, public Avalon::Singleton<RLInternalNetwork>
{
public:
	RLInternalNetwork();
	~RLInternalNetwork();


	Avalon::NetConnection* CreateConnection();

public://�¼�
	/**
	*@brief һ�����ӽ��� ���������net--> �߼���logic�� SYS_NOTIFY_CONNECTED ���ӽ���Э�� (ʵ��CLNetwork��Ӧ����ӿ�, �˴���ʾnodeid Ϊid�� �������Ѿ����ӳɹ�)
	*/
	virtual void OnConnected(UInt32 id);

	/**
	*@brief һ�����ӶϿ� ���������net -> �߼���logic�� SYS_NOTIFY_DISCONNECTED ���ӶϿ�Э��
	*/
	virtual void OnDisconnected(UInt32 id);

	/**
	*@brief ֹͣ����  �����յ���ͣ��֪ͨЭ�� SYS_NOTIFY_SHUTDOWN
	*/
	virtual void OnShutdownService();
	/*
	*/
	bool PreHandlePacket(CLNetConnection* conn, Avalon::Packet* packet);

	/*
	*/
	void RegisterProtocols();
protected:

	void register2matchsvr();

private:
	bool m_reconnect;				//�Ƿ��Ƕ�������
};



#endif
