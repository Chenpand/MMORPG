#ifndef __VC_INTERNAL_NETWORK_H__
#define __VC_INTERNAL_NETWORK_H__

#include <AvalonSingleton.h>
#include "CLInternalNetwork.h"

/**
*@brief ����
*/
class VCInternalNetwork : public CLInternalNetwork, public Avalon::Singleton<VCInternalNetwork>
{
public:
	VCInternalNetwork();
	~VCInternalNetwork();

public:
	bool Startup(Avalon::PropertySet *properties, CLNetAddress *addresses);

public:
	void RegisterProtocols();

	void DispatchPacket(Avalon::Packet* packet);

	/**
	*@brief ִ��һ����Ϣ��
	*/
	bool ExecutePacket(Avalon::Packet* packet);

	bool PreHandlePacket(CLNetConnection* conn, Avalon::Packet* packet);

public://�¼�
	/**
	*@brief һ�����ӽ���
	*/
	virtual void OnConnected(UInt32 id);

	/**
	*@brief һ�����ӶϿ�
	*/
	virtual void OnDisconnected(UInt32 id);

	/**
	*@brief ֹͣ����
	*/
	virtual void OnShutdownService();

private:

};


#endif