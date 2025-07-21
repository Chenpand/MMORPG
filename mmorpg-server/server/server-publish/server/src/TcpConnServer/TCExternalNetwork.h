#ifndef _UC_EXTERNALNETWORK_H_
#define _UC_EXTERNALNETWORK_H_

#include <AvalonSingleton.h>
#include <AvalonPropertySet.h>
#include <CLDefine.h>
#include <AvalonTime.h>
#include <AvalonCodeQueue.h>
#include "enet/enet.h"
#include "CLNetHeader.h"

#ifdef WEB_SOCKET
#include "CLWebSocketNetwork.h"
#else
#include <CLExternalNetwork.h>
#endif

//class Avalon::CCodeQueue;
class TCPlayer;

#ifdef WEB_SOCKET
typedef CLWebSocketConnection TcpConnection;
typedef CLWebSocketNetwork TcpNetwork;
#else
typedef CLNetConnection TcpConnection;
typedef CLNetwork TcpNetwork;
#endif


class TCExternalConnection : public TcpConnection
{
public:
	TCExternalConnection(TcpNetwork* network);
	~TCExternalConnection();

public:

	/**
	*@brief ?????????? ??????????
	*/
	bool PreHandlePacket(Avalon::Packet* packet);

	void SetPacket(Avalon::Packet* packet);

	Avalon::Packet* GetPacket() { return m_Packet; }

	void DestroyPacket(){ Avalon::Packet::Destroy(m_Packet); }

public:
	virtual void OnTick(const Avalon::Time& now);

private:

	Avalon::Packet* m_Packet;

	//???id
	UInt32 m_AccId;

	//???id
	UInt64 m_RoleId;

	//????????????????
	bool	m_bWaitOffine;
};

class TCExternalNetwork : public TcpNetwork, public Avalon::Singleton<TCExternalNetwork>
{
public:
	enum
	{
		MAX_CLIENT_COUNT = 3000,		//???????client????

		wait_timeout = 5,               //ms
		//cs_message_id_offset = 2,			//Byte Э??id?????????????buffer?У?
		//cs_message_min_size = 6,			//csЭ?????С????(2Byte??????? + 4Byte Э??ID)

		//MAX_CS_CHANNEL_SIZE = 0x2000000,			//32Mb

		//max_relaysvr_cs_packet_size	= 8192,		//relay client??service ??csЭ??????????

		max_packet_once_receive = 64000,		//??ν???service???????
		//max_relaysvr_sc_packet_size = 64000,	//64k  service??client??????????

		enet_default_channel_id = 0,
		enet_default_channel_count = 1,
	};

public:
	TCExternalNetwork();
	~TCExternalNetwork();

public:

	/**
	*@brief ????
	*/
	bool Startup(Avalon::PropertySet* properties);

	/**
	*@brief ???
	*/
	void Shutdown(UInt32 inputTimeout);

	/**
	*@brief ???????
	*/
	NetworkType GetType() const{ return NT_EXTERVAL; }

	/**
	*@brief ????????
	*/
	bool ExecutePacket(Avalon::Packet* packet);

	void RegisterProtocols() { };

#ifdef WEB_SOCKET
	/**
	*@brief ???????????
	*/
	CLWebSocketConnection* CreateConnection();
#else
	/**
	*@brief ???????????
	*/
	Avalon::NetConnection* CreateConnection();
#endif

	/**
	*@brief ???????
	*/
	bool PreHandlePacket(CLNetConnection* conn, Avalon::Packet* packet);

public://???
	/**
	*@brief ??????????
	*/
	virtual void OnConnected(UInt32 id);

	/**
	*@brief ?????????
	*/
	virtual void OnDisconnected(UInt32 id);

	/**
	*@brief tick???
	*/
	virtual void OnTick(const Avalon::Time& now);

public:
	bool MakeChannels(Avalon::PropertySet* prop);

	bool NotifyClientConnected(TCPlayer* player);

	bool DisconnectClient(TCPlayer* player, enmDisconnectReason reason);

	bool NotifyClientDisconnected(TCPlayer* player, enmDisconnectReason reason);

	bool TransmitPacket(UInt32 accId, Avalon::Packet* packet);

	void ChechChannels(const Avalon::Time& now);

	Int32 ProcessServicePacket(char* packet, const Int32 size);

	Int32 ProcessServiceTransmitRequest(TCPlayer* player, char* packet, Int32 size);

private:


	//
	Avalon::CCodeQueue* m_pstCSChannel;  //c -> s channel
	Avalon::CCodeQueue* m_pstSCChannel;  //s -> c channel
};

extern Avalon::CShmMemory* g_pstCSChannelShm;
extern Avalon::CShmMemory* g_pstSCChannelShm;



#endif
