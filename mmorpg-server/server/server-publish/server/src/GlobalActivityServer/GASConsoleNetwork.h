#ifndef _GAS_CONSOLE_NETWORK_H_
#define _GAS_CONSOLE_NETWORK_H_

#include <AvalonSingleton.h>
#include "CLNetwork.h"

class CLNetAddress;

/**
*@brief ����̨����
*/
class ConsoleConnection : public CLNetConnection
{
public:
	explicit ConsoleConnection(CLNetwork* network);
	~ConsoleConnection();

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
*@brief ����̨����
*/
class ConsoleNetwork : public CLNetwork, public Avalon::Singleton<ConsoleNetwork>
{
public:
	ConsoleNetwork();
	~ConsoleNetwork();

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
	NetworkType GetType() const{ return NT_CONSOLE; }

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
