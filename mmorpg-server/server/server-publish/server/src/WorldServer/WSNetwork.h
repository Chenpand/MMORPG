#ifndef _WS_NETWORK_H_
#define _WS_NETWORK_H_

#include <AvalonSingleton.h>
#include <CLInternalNetwork.h>
#include <CLTransfer.h>

/**
 *@brief ����
 */
class WSNetwork : public CLInternalNetwork, public CLTransfer, public Avalon::Singleton<WSNetwork>
{
public:
	WSNetwork();
	~WSNetwork();

public:
	bool PreHandlePacket(CLNetConnection* conn, Avalon::Packet* packet);
	void DispatchPacket(Avalon::Packet* packet);

	void RegisterProtocols();

public:
	/**
	*@brief ���ѡ��һ������������
	*/
	UInt32 SelectRandSceneServer();

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

public:
	/**
	*@brief ע����ת��Ϣ
	*/
	virtual void RegisterTransferProtocols();
};

#endif
