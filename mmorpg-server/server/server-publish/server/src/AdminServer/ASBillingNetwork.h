#ifndef _AS_BILLING_NETWORK_H_
#define _AS_BILLING_NETWORK_H_

#include <AvalonSingleton.h>
#include "CLNetwork.h"

class CLNetAddress;

/**
 *@brief ֧������
 */
class BillingConnection : public CLNetConnection
{
public:
	explicit BillingConnection(CLNetwork* network);
	~BillingConnection();

public:
	void OnDataReached();

	void OnTick(const Avalon::Time& now);

	bool PreHandlePacket(Avalon::Packet* packet);

public:
	void EncodeHeader(UInt8* header, size_t len, UInt16 size, UInt32 id, UInt32 sequence);
		
	void DecodeHeader(UInt8* header, size_t len, UInt16& size, UInt32& id, UInt32& sequence);

private:
	static UInt32 GenID();
};

/**
 *@brief ֧������
 */
class BillingNetwork : public CLNetwork, public Avalon::Singleton<BillingNetwork>
{
public:
	BillingNetwork();
	~BillingNetwork();

public:
	/**
	 *@brief ����һ������
	 */
	Avalon::NetConnection* CreateConnection();

	/**
	 *@brief ����
	 */
	bool Startup(Avalon::PropertySet* properties, CLNetAddress* addresses = NULL);

	/**
	 *@brief �ر�
	 */
	void Shutdown(UInt32 inputTimeout);

	/**
	 *@brief ��ȡ����
	 */
	NetworkType GetType() const{ return NT_BILLING; }

public:
	/**
	 *@brief ִ��һ����Ϣ��
	 */
	bool ExecutePacket(Avalon::Packet* packet);

public://�¼�
	
	void OnConnected(UInt32 id);

	void OnDisconnected(UInt32 id);
};

#endif
