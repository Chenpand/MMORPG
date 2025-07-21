#ifndef _UC_EXTERNALNETWORK_H_
#define _UC_EXTERNALNETWORK_H_

#include "enet/enet.h"
#include <AvalonSingleton.h>
#include <AvalonPropertySet.h>
#include <CLDefine.h>
#include <AvalonTime.h>
#include <AvalonCodeQueue.h>
#include "CLNetHeader.h"


//class Avalon::CCodeQueue;
class UCPlayer;

class UdpNetworks : public Avalon::Singleton<UdpNetworks>
{
public:
	enum 
	{
		MAX_CLIENT_COUNT = 3000,		//最多支持的client数量

		wait_timeout = 1,               //ms

		//cs_message_id_offset = 2,			//Byte 协议id的偏移（在编码后的buffer中）
		//cs_message_min_size = 6,			//cs协议的最小长度(2Byte包长字段 + 4Byte 协议ID)

		//MAX_CS_CHANNEL_SIZE = 0x2000000,			//32Mb

		//max_relaysvr_cs_packet_size	= 8192,		//relay client到service 的cs协议最大包长度

		max_packet_once_receive		= 64000,		//一次接收service的包数量
		//max_relaysvr_sc_packet_size = 64000,	//64k  service到client的最大包长度

		enet_default_channel_id		= 0,
		enet_default_channel_count  = 1,
	};

	

public:
	UdpNetworks();
	~UdpNetworks();

	bool Startup(Avalon::PropertySet* prop);
	void Shutdown(Avalon::UInt32 inputTimeout = 100);
	virtual void OnTick(const Avalon::Time& now);

	size_t GetPeerCount();

	bool NotifyClientPing(UCPlayer* player);

protected:
// 	ENetPeer* FindPeerByAccId(UInt32 accId);
// 	bool AddPeer(UInt32 accId, ENetPeer* peer);
// 	bool RemovePeer(ENetPeer* peer);
// 	bool RemovePeer(UInt32 accId);

	bool make_channels(Avalon::PropertySet* prop);
	bool open_udpnetwork(Avalon::PropertySet* prop);

	void init_event(ENetEvent& event);
	void HandleEvent(ENetEvent& event);
	void on_receive_data(ENetPeer* peer, ENetPacket* packet);
	void on_connect(ENetPeer* peer, UInt32 accId);
	void on_disconnect(ENetPeer* peer, UInt32 accId);

	bool transmit_packet(UInt32 accId, ENetPeer* peer, ENetPacket* packet);

	//
	bool disconnect_client(UCPlayer *player, EDisconnectReason reason);
	bool notify_client_disconnected(UCPlayer *player, EDisconnectReason reason);
	bool notify_client_connected(UCPlayer* player);

	//检查cs以及sc channel
	void chech_channels(const Avalon::Time& now);
	Int32 process_service_packet(char* packet, const Int32 size);
	Int32 process_service_transmit_request(UCPlayer* player, char* packet, Int32 size, Int32 flag);
	Int32 process_service_disconnect_request(UCPlayer* player);

	Int32 send_packet_reliable(UCPlayer* player, char* packet, Int32 size);
	Int32 send_packet_reliable_immediately(UCPlayer* player, char* packet, Int32 size);
protected:
	 
private:
	ENetHost* m_pstENetHost;
	std::string m_szIP;
	UInt16 m_unPort;
	UInt16 m_unMaxClients;

	//ENetPeerMap m_stPeerMap;

	ENetEvent m_stEvent;

	//
	Avalon::CCodeQueue* m_pstCSChannel;  //c -> s channel
	Avalon::CCodeQueue* m_pstSCChannel;  //s -> c channel
};

extern Avalon::CShmMemory* g_pstCSChannelShm;
extern Avalon::CShmMemory* g_pstSCChannelShm;

#endif
