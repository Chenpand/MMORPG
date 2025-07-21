#ifndef _OBS_NETWORK_H_
#define _OBS_NETWORK_H_

#include <AvalonSingleton.h>
#include "CLInternalNetwork.h"

/**
*@brief ����
*/
class OBSInternalNetwork : public CLInternalNetwork, public Avalon::Singleton<OBSInternalNetwork>
{
public:
	OBSInternalNetwork();
	~OBSInternalNetwork();

public:
	bool Startup(Avalon::PropertySet *properties, CLNetAddress *addresses);

public:
	void RegisterProtocols();

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
