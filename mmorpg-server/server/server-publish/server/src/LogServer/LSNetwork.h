#ifndef _LS_NETWORK_H_
#define _LS_NETWORK_H_

#include <AvalonSingleton.h>
#include <AvalonSocket.h>
#include "CLNetwork.h"

/**
 *@brief ��־����
 */
class LogConnection : public CLNetConnection
{
public:
	explicit LogConnection(CLNetwork* network);
	~LogConnection();

public:
	void OnDataReached();

	void OnTick(const Avalon::Time& now);

	bool PreHandlePacket(Avalon::Packet* packet);

private:
	static UInt32 GenID();
};

/**
 *@brief ��־����
 */
class LogNetwork : public CLNetwork, public Avalon::Singleton<LogNetwork>
{
public:
	LogNetwork();
	~LogNetwork();

public:
	/**
	 *@brief ����һ������
	 */
	Avalon::NetConnection* CreateConnection();

	/**
	 *@brief ����
	 */
	bool Startup(Avalon::PropertySet* properties);

	/**
	 *@brief �ر�
	 */
	void Shutdown(UInt32 inputTimeout);

	NetworkType GetType() const{ return NT_EXTERVAL; }


public://�¼�
	void OnTick(const Avalon::Time& now);
	
	void OnConnected(UInt32 id);

	void OnDisconnected(UInt32 id);

private:
	void ProcessDataLogPacket(Avalon::Packet* packet, const Avalon::SockAddr& addr);

private:
	//udp�׽���
	Avalon::Socket m_UdpSock;
};

#endif
