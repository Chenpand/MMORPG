#ifndef _US_CONSOLE_NETWORK_H_
#define _US_CONSOLE_NETWORK_H_

#include <AvalonSingleton.h>
#include "CLNetwork.h"

class CLNetAddress;

/**
*@brief ����̨����
*/
class USConsoleConnection : public CLNetConnection
{
public:
	explicit USConsoleConnection(CLNetwork* network);
	~USConsoleConnection();

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
class USConsoleNetwork : public CLNetwork, public Avalon::Singleton<USConsoleNetwork>
{
public:
	USConsoleNetwork();
	~USConsoleNetwork();

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
