#ifndef _RS_NETWORK_H_
#define _RS_NETWORK_H_

#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>
#include "CLInternalNetwork.h"

/**
 *@brief ����
 */
class RSNetwork : public CLInternalNetwork, public Avalon::Singleton<RSNetwork>
{
public:
	RSNetwork();
	~RSNetwork();

public:

	void RegisterProtocols();

public://�¼�
	/**
	 *@brief �����¼�
	 */
	virtual void OnTick(const Avalon::Time& now);

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

protected:
	/**
	 *@brief ִ��һ����Ϣ��
	 */
	bool ExecutePacket(Avalon::Packet* packet);

private:
	//���յİ���
	UInt32	m_PacketNum;
	//���յ�����
	UInt32	m_ByteNum;
	//5�ֶ�ʱ��
	Avalon::SimpleTimer	m_Timer;
};


#endif
