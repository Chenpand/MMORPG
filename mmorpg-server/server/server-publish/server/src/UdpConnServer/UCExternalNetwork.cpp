#include "UCExternalNetwork.h"

#include <AvalonCodeEngine.h>
#include <AvalonSharedMemory.h>
#include <AvalonPacket.h>
#include <CLDefine.h>
#include <CLProtocolDefine.h>
#include <CLNetHeader.h>
#include <CLRelayServerProtocol.h>
#include <AvalonNetworkAPI.h>
#include <PerformanceCheck.h>

#include "UCPlayer.h"
#include "UCPlayerMgr.h"
#include "UCApplication.h"

Avalon::CShmMemory* g_pstCSChannelShm = NULL;
Avalon::CShmMemory* g_pstSCChannelShm = NULL;

UdpNetworks::UdpNetworks()
{
	m_pstENetHost = NULL;
	m_unPort = 0;
	m_unMaxClients = 0;

	//m_stPeerMap.clear();

	//
	init_event(m_stEvent);

	m_pstCSChannel = NULL;
	m_pstSCChannel = NULL;

}

UdpNetworks::~UdpNetworks()
{
	
}

bool UdpNetworks::make_channels(Avalon::PropertySet* prop)
{
	UInt32 unCSChannelKey = prop->GetProperty("Channels", "CSChannelKey");
	UInt32 unSCChannelKey = prop->GetProperty("Channels", "SCChannelKey");
	if (!unCSChannelKey || !unSCChannelKey)
	{
		ErrorLogStream << "invalid cs channel key: " << unCSChannelKey << ", sc channel key: " << unSCChannelKey << LogStream::eos;
		return false;
	}

	UInt32 unShmSize = Avalon::CCodeQueue::size(MAX_CS_CHANNEL_SIZE);

	g_pstCSChannelShm = Avalon::create_shm_memory(unCSChannelKey, unShmSize);
	if (NULL == g_pstCSChannelShm)
	{
		ErrorLogStream << "create cs channel memory failed, key= " << unCSChannelKey << LogStream::eos;
		return false;
	}

	g_pstSCChannelShm = Avalon::create_shm_memory(unSCChannelKey, unShmSize);
	if (NULL == g_pstSCChannelShm)
	{
		ErrorLogStream << "create sc channel memory failed, key= " << unSCChannelKey << LogStream::eos;
		return false;
	}

	//
	Avalon::CCodeQueue::set_sharedmemory(g_pstCSChannelShm);
	m_pstCSChannel = new Avalon::CCodeQueue(MAX_CS_CHANNEL_SIZE);
	if (NULL == m_pstCSChannel)
	{
		ErrorLogStream << "create cs channel queue failed, key= " << unCSChannelKey << LogStream::eos;
		return false;
	}

	Avalon::CCodeQueue::set_sharedmemory(g_pstSCChannelShm);
	m_pstSCChannel = new Avalon::CCodeQueue(MAX_CS_CHANNEL_SIZE);
	if (NULL == m_pstSCChannel)
	{
		ErrorLogStream << "create sc channel queue failed, key= " << unSCChannelKey << LogStream::eos;
		return false;
	}


	return true;
}

bool UdpNetworks::Startup(Avalon::PropertySet* prop)
{
	if (NULL == prop)
	{
		return false;
	}

	//make channels
	if (!make_channels(prop))
	{
		return false;
	}

	//open udp
	if (!open_udpnetwork(prop))
	{
		return false;
	}


	return true;
}

void UdpNetworks::Shutdown(Avalon::UInt32 inputTimeout /*= 100*/)
{
	//delete channels ?
	//TODO:

	//
	enet_host_destroy(m_pstENetHost);
	
	//should be called at last!
	enet_deinitialize();

	//
	DebugLogStream << "in UdpNetworks::Shutdown, shutdown udp network!" << LogStream::eos;
}

void UdpNetworks::OnTick(const Avalon::Time& now)
{
	auto timeoutMs = UCApplication::Instance()->GetUdpRecvTimeout();
	UInt64 tickEndTime = now.MSec() + timeoutMs;

	PERF_SEG_BEGIN(enet);
	init_event(m_stEvent);
	UInt32 packetNum = 0;
	while (CURRENT_TIME.MSec() <= tickEndTime && enet_host_service(m_pstENetHost, &m_stEvent, timeoutMs) > 0)
	{
		HandleEvent(m_stEvent);
		init_event(m_stEvent);

		if (++packetNum >= MAX_PACKET_PERTICK)
		{
			ErrorLogStream << "recv too many package(" << packetNum << ")." << LogStream::eos;
			break;
		}
	}
	PERF_SEG_END();

	PERF_SEG_BEGIN(channel);
	//check sc channel
	chech_channels(now);
	PERF_SEG_END();

	return;
}

// ENetPeer* UdpNetworks::FindPeerByAccId(UInt32 accId)
// {
// 	ENetPeerMap::iterator iter = m_stPeerMap.find(accId);
// 	if (iter != m_stPeerMap.end())
// 	{
// 		return iter->second;
// 	}
// 
// 	return NULL;
// }
// 
// 
// 
// size_t UdpNetworks::GetPeerCount()
// {
// 	return m_stPeerMap.size();
// }
// 
// bool UdpNetworks::AddPeer(UInt32 accId, ENetPeer* peer)
// {
// 	if (NULL == peer)
// 	{
// 		return false;
// 	}
// 
// 	if (m_stPeerMap.size() >= m_unMaxClients)
// 	{
// 		return false;
// 	}
// 
// 	return(m_stPeerMap.insert(std::make_pair(accId, peer)).second);
// }
// 
// bool UdpNetworks::RemovePeer(ENetPeer* peer)
// {
// 	if (NULL == peer)
// 	{
// 		return false;
// 	}
// 
// 	return RemovePeer(peer->eventData); //ENetPeer::eventData 要求client 在connect时 data设置为AccoutId，enet会把data保存在ENetPeer::eventData中
// }
// 
// bool UdpNetworks::RemovePeer(UInt32 accId)
// {
// 	ENetPeerMap::iterator iter = m_stPeerMap.find(accId);
// 	if (iter != m_stPeerMap.end())
// 	{
// 		m_stPeerMap.erase(iter);
// 	}
// 
// 	return true;
// }

void UdpNetworks::init_event(ENetEvent& event)
{
	event.type = ENET_EVENT_TYPE_NONE;
	event.channelID = 0;
	event.data = 0;
	event.packet = NULL;
	event.peer = NULL;
}

void UdpNetworks::HandleEvent(ENetEvent& event)
{
	//event ready!
	switch (m_stEvent.type)
	{
	case ENET_EVENT_TYPE_RECEIVE:
		on_receive_data(event.peer, event.packet);
		enet_packet_destroy(event.packet);  //destroy the packet
		break;
	case ENET_EVENT_TYPE_CONNECT:
		on_connect(event.peer, event.data); //event.data 就是event.peer->eventData
		break;
	case ENET_EVENT_TYPE_DISCONNECT:
		on_disconnect(event.peer, event.data);
		break;

	default:
		break;
	}
}

void UdpNetworks::on_receive_data(ENetPeer* peer, ENetPacket* packet)
{
	//DebugLogStream << "in UdpNetworks::on_receive_data, receive data from client:" << LogStream::eos;
	if (NULL == peer || NULL == packet)
	{
		return;
	}

	//
	UInt32 accId = (UInt32)peer->eventData;
	UCPlayer* player = UCPlayerMgr::Instance()->find_player(accId);
	if (NULL == player)
	{
		return;
	}

    //DebugLogStream << "recv packet(" << (UInt16)packet->dataLength << ") from player(" << accId << ")." << LogStream::eos;

	//DebugLogStream << "in UdpNetworks::on_receive_data, player(accId:" << player->m_unAccId << ", roleId:" <<player->m_unRoleId <<"), packet(size:" << (UInt16)packet->dataLength << ")" << LogStream::eos;

	//
	UInt16 receivedSize = (UInt16)packet->dataLength;
	char* pszBuff = (char*) packet->data;
	if (receivedSize < cs_message_min_size || !pszBuff)
	{
		disconnect_client(player, enmDisconnectReason_system);
		return;
	}

	UInt16 packetSize = 0; 
	Avalon::CCodeEngine::decode_int16(&pszBuff, (uint16_t*)&packetSize);

	if ((packetSize + Avalon::Packet::HEAD_SIZE) != receivedSize) //packetSize不包括协议的头部字段长度（2+4）
	{
		//
		DebugLogStream << "in UdpNetworks::on_receive_data, received size(" << receivedSize << "), was not equal to packet size(" << packetSize << "), so discard it!" << LogStream::eos;
		//something wrong! disconnect the client ? TODO:
		return;
	}
	
	pszBuff = (char*)(packet->data + cs_message_id_offset);
	UInt32 msgID = 0;
	Avalon::CCodeEngine::decode_int32(&pszBuff, (uint32_t*)&msgID);

	//DebugLogStream << "in UdpNetworks::on_receive_data, received message(id:" << msgID << ")" << LogStream::eos;

	//first packet
	if (CLProtocol::RELAYSERVER_LOGIN_REQ == msgID)
	{
		//modified by aprilliu, 2016.03.07
		Avalon::Packet* pstPacket = Avalon::Packet::Create(packetSize);
		if (NULL == pstPacket)
		{
            ErrorLogStream << "player(" << accId << ") create packet failed, size(" << packetSize << ")." << LogStream::eos;
			return;
		}

		pstPacket->SetSize(packetSize);
		pstPacket->SetID(msgID);

		memcpy(pstPacket->GetBuffer(), packet->data + Avalon::Packet::HEAD_SIZE,  (size_t)packetSize);

		CLProtocol::RelaySvrLoginReq login;
		if (!login.Decode(pstPacket))
		{
            ErrorLogStream << "player(" << accId << ", " << player->m_unRoleId << ") decode login protocol failed." << LogStream::eos;
			return;
		}

		player->m_unRoleId = login.m_unRoleId;

        DebugLogStream << "player(" << accId << ", " << player->m_unRoleId << ") send login request." << LogStream::eos;
	}
	else if (CLProtocol::RELAYSERVER_RECONNECT_REQ == msgID)
	{
		Avalon::Packet* pstPacket = Avalon::Packet::Create(packetSize);
		if (NULL == pstPacket)
		{
			return;
		}

		pstPacket->SetSize(packetSize);
		pstPacket->SetID(msgID);

		memcpy(pstPacket->GetBuffer(), packet->data + Avalon::Packet::HEAD_SIZE, (size_t)packetSize);

		CLProtocol::RelaySvrReconnectReq reconnect;
		if (!reconnect.Decode(pstPacket))
		{
            ErrorLogStream << "player(" << accId << ", " << player->m_unRoleId << ") decode reconnect protocol failed." << LogStream::eos;
			return;
		}

		player->m_unRoleId = reconnect.roleId;

        DebugLogStream << "player(" << accId << ", " << player->m_unRoleId << ") send reconnect request." << LogStream::eos;
	}
	else if (CLProtocol::RELAYSERVER_OBSERVE_REQ == msgID)
	{
		Avalon::Packet* pstPacket = Avalon::Packet::Create(packetSize);
		if (NULL == pstPacket)
		{
			return;
		}

		pstPacket->SetSize(packetSize);
		pstPacket->SetID(msgID);

		memcpy(pstPacket->GetBuffer(), packet->data + Avalon::Packet::HEAD_SIZE, (size_t)packetSize);

		CLProtocol::RelaySvrObserveReq reconnect;
		if (!reconnect.Decode(pstPacket))
		{
			ErrorLogStream << "player(" << accId << ", " << player->m_unRoleId << ") decode observe protocol failed." << LogStream::eos;
			return;
		}

		player->m_unRoleId = reconnect.roleId;

		DebugLogStream << "player(" << accId << ", " << player->m_unRoleId << ") send observe request." << LogStream::eos;
	}

	//transmit packet to service
	//CNetHead stNetHead;

	transmit_packet(accId, peer, packet);
	
}

//处理ENET_EVENT_TYPE_CONNECT 新连接事件
void UdpNetworks::on_connect(ENetPeer* peer, UInt32 accId)
{
	char ip[32];
	UInt16 port = 0;
	Avalon::sockaddr_t addr;
	addr.sin_addr.s_addr = peer->address.host;
	addr.sin_port = htons(peer->address.port);
	Avalon::sockaddr_decode(addr, ip, port);
	//
	DebugLogStream << "In UdpNetworks::on_connect, player(accid:" << accId << "), peer(" << (UInt64)peer << ") address(" << ip << ":" << port << ")" << LogStream::eos;
	
	//
	if (NULL == peer )
	{
		ErrorLogStream << "In UdpNetworks::on_connect, peer is invalid ,just return" << LogStream::eos;
		return;
	}

	if (!accId)
	{
		ErrorLogStream << "In UdpNetworks::on_connect, accId is invalid ,just reset client" << LogStream::eos;
		//enet_peer_reset(peer);
		enet_peer_disconnect_now(peer, accId);
		return;
	}

	UCPlayer* player = UCPlayerMgr::Instance()->find_player(accId);
	if (player)
	{
		//what ? some error
		ErrorLogStream << "UdpNetworks::on_connect, player(accId:" << accId << ") was duplicated login, just disconnect it!" << LogStream::eos;
		disconnect_client(player, enmDisconnectReason_system);
		//
		//return;
	}

	//
	UInt32 currentPlayers = UCPlayerMgr::Instance()->get_player_count();
	if (currentPlayers >= m_unMaxClients)
	{
		ErrorLogStream << "UdpNetworks::on_connect, refuse the client(accId:" << player->m_unAccId << "), because so many clients(count:" << currentPlayers << "), and capacity:" << m_unMaxClients << LogStream::eos;
		//some many clients, just reset peer
		//enet_peer_reset(peer);
		enet_peer_disconnect_now(peer, accId);
		return;
	}

	//then, create player, noted: role was not setted
	player = new UCPlayer(accId, peer);
	if (NULL == player)
	{
		enet_peer_disconnect_now(peer, accId);
		return;
	}

	// 设置超时信息
	{
		enet_peer_timeout(peer, UCApplication::Instance()->GetTimeoutLimit(), UCApplication::Instance()->GetTimeoutMinimum(), UCApplication::Instance()->GetTimeoutMaximum());
		DebugLogStream << "player(" << player->m_unAccId << ") enet_peer_timeout(" << UCApplication::Instance()->GetTimeoutLimit() << ", "
					<< UCApplication::Instance()->GetTimeoutMinimum() << ", " << UCApplication::Instance()->GetTimeoutMaximum() << ")." << LogStream::eos;
	}

	DebugLogStream << "UdpNetworks::on_connect, create UCPlayer success! player(accId:" << player->m_unAccId << ")" << LogStream::eos;

	if (!UCPlayerMgr::Instance()->add_player(player))
	{
		DebugLogStream << "UdpNetworks::on_connect, add_player failed, player(accId:" << player->m_unAccId << ")" << LogStream::eos;
		disconnect_client(player, enmDisconnectReason_system);
		return;
	}

	//notify new client to service
	notify_client_connected(player);

	return;
}

//检测到client断开连接
void UdpNetworks::on_disconnect(ENetPeer* peer, UInt32 accId)
{
	DebugLogStream << "in UdpNetworks::on_disconnect, detect client was disconnected!, peer(" << (UInt64)peer << ")" << LogStream::eos;
	//
	if (NULL == peer || ! accId)
	{
		return;
	}

	//
	UCPlayer* player = UCPlayerMgr::Instance()->find_player(accId);
	if (NULL == player)
	{
		//just reset ENetPeer
		enet_peer_reset(peer);
		return;
	}

	DebugLogStream << "in UdpNetworks::on_disconnect, detect client was disconnected! player(accId:" << player->m_unAccId << ", roleId:" << player->m_unRoleId << ")" << LogStream::eos;

	//
	if (player->m_pstPeer != peer) //不太可能
	{
		DebugLogStream << "player(" << accId << ")'s connection disconnected, but the peer is not current peer." << LogStream::eos;
		//
		enet_peer_reset(peer);
		return;
		//player->m_pstPeer = peer;
	}

	disconnect_client(player, enmDisconnectReason_client_close);
	return;
}

bool UdpNetworks::open_udpnetwork(Avalon::PropertySet* prop)
{
	//
	m_szIP = prop->GetProperty("NetWork", "IP").GetString();
	if (m_szIP.empty())
	{
		return false;
	}

	m_unPort = prop->GetProperty("NetWork", "Port");
	if (!m_unPort)
	{
		return false;
	}

	m_unMaxClients = prop->GetProperty("NetWork", "MaxClients");
	if (m_unMaxClients > MAX_CLIENT_COUNT || !m_unMaxClients)
	{
		m_unMaxClients = MAX_CLIENT_COUNT;
	}

	if (0 != enet_initialize())
	{
		ErrorLogStream << "enet library initialize fail!" << LogStream::eos;
		return false;
	}

	//只创建一个channel
	ENetAddress address;
	address.port = m_unPort;
	address.host = ENET_HOST_ANY;

	m_pstENetHost = enet_host_create(&address, m_unMaxClients, enet_default_channel_count, 0, 0);
	if (NULL == m_pstENetHost)
	{
		ErrorLogStream << "enet_host_create fail! ip= " << m_szIP << "port = " << m_unPort << LogStream::eos;
		return false;
	}

	//enet_host_compress_with_range_coder(m_pstENetHost);  //enet_host_destory中会释放相应的资源
	//m_pstENetHost->checksum = enet_crc32;

	DebugLogStream << "ip = " << m_szIP.c_str() << " in UdpNetworks::open_udpnetwork, open upd network(port:" << m_unPort << ") success!" << LogStream::eos;

	return true;
}

bool UdpNetworks::transmit_packet(UInt32 accId, ENetPeer* peer, ENetPacket* packet)
{
	//InfoLogStream << "in UdpNetworks::transmit_packet, to transfer packet to service, player(accid:" << accId << ")" << LogStream::eos;
	UCPlayer* player = UCPlayerMgr::Instance()->find_player(accId);
	if (NULL == player)
	{
		ErrorLogStream << "in UdpNetworks::transmit_packet, can't find player(accid:" << accId << ")" << LogStream::eos;
		return false;
	}

	
	UInt16 receivedSize = (UInt16)packet->dataLength;
	char* pszBuff = (char*)packet->data;
	if (receivedSize < cs_message_min_size || receivedSize > max_relaysvr_cs_packet_size || !pszBuff)
	{
		ErrorLogStream << "in UdpNetworks::transmit_packet, received size:" << receivedSize << "was invalid, so discard the packet!" << LogStream::eos;
		return false;
	}

	CNetHead stNetHead;

	stNetHead.m_cCtrlId = enmControlId_request_transmit;
	stNetHead.m_unAccId = accId;
	stNetHead.m_RoleId = player->m_unRoleId;

	//stNetHead.m_stControlInfo.m_stSourceIPInfo.m_iSourceIP = (int32_t)peer->address.host;
	//stNetHead.m_stControlInfo.m_stSourceIPInfo.m_nSourcePort = peer->address.port;
	stNetHead.m_stControlInfo.m_iPing = (int32_t)peer->roundTripTime;

	static char szCode[max_relaysvr_cs_packet_size];

	//
	//set packet size
	stNetHead.m_iPackageSize = stNetHead.size() + receivedSize;

	Int32 encodeSize = (Int32)sizeof(szCode);
	pszBuff = szCode;
	if (stNetHead.encode(pszBuff, encodeSize) != success)
	{
		ErrorLogStream << "in UdpNetworks::transmit_packet, Net Head encode fail! " << " so discard the packet!" << LogStream::eos;
		return false;
	}

	if (sizeof(szCode) - encodeSize < receivedSize)
	{
		ErrorLogStream << "in UdpNetworks::transmit_packet, packet size was so larg! size:" << receivedSize << " so discard the packet!" << LogStream::eos;
		return false;
	}

	pszBuff = szCode + encodeSize;
	memcpy(pszBuff, packet->data, packet->dataLength);

	player->RecordMsg((char*)packet->data, packet->dataLength, true);

	if (m_pstCSChannel->append((const char*)szCode, stNetHead.m_iPackageSize) != success)
	{
		//close the peer ? TODO:
		ErrorLogStream << "in UdpNetworks::transmit_packet, append packet(size:" << stNetHead.m_iPackageSize << ") to CS Channel failed, so discard the packet, and disconnect client" << LogStream::eos;
		disconnect_client(player, enmDisconnectReason_system);
		return false;
	}

	//InfoLogStream << "in UdpNetworks::transmit_packet, append packet(size:" << stNetHead.m_iPackageSize << ") success!" << LogStream::eos;
	//InfoLogStream << "in UdpNetworks::transmit_packet, cs channel code size; " << m_pstCSChannel->get_codesize() << LogStream::eos;

	return true;
}

//player was invalid after called the routine
bool UdpNetworks::disconnect_client(UCPlayer *player, enmDisconnectReason reason)
{
	//
	if (NULL == player || !player->m_pstPeer)
	{
		return false;
	}

	DebugLogStream << "in UdpNetworks::disconnect_client, to disconnect client(accId:" << player->m_unAccId << ", roleId:" << player->m_unRoleId << "), reason:" << (UInt16)reason << LogStream::eos;
	//
	if (enmDisconnectReason_service != reason && enmDisconnectReason_invalid_newclient != reason)
	{
		//通知service , client 断开
		DebugLogStream << "in UdpNetworks::disconnect_client, to notify service that client was disconnected, reason:" << (UInt16)reason << LogStream::eos;
		notify_client_disconnected(player, reason);
	}

	//and free player's resource
	//enet_peer_reset(player->m_pstPeer);
	//enet_peer_disconnect_now(player->m_pstPeer, player->m_unAccId);
	enet_peer_disconnect_later(player->m_pstPeer, player->m_unAccId);

	UCPlayerMgr::Instance()->remove_player(player);

	//
	DebugLogStream << "in UdpNetworks::disconnect_client, delete player(accId:" << player->m_unAccId << ", roleId:" << player->m_unRoleId << ")" << LogStream::eos;

	CL_SAFE_DELETE(player);  //删除player对象

	//
	return true;
}

bool UdpNetworks::notify_client_disconnected(UCPlayer *player, enmDisconnectReason reason)
{
	DebugLogStream << "in UdpNetworks::notify_client_disconnected,  to notify service that client disconnected" << LogStream::eos;
	if (NULL == player)
	{
		return false;
	}

	CNetHead stNetHead;
	stNetHead.m_cCtrlId = enmControlId_notify_disconnect;
	stNetHead.m_unAccId = player->m_unAccId;
	stNetHead.m_RoleId = player->m_unRoleId;
	stNetHead.m_stControlInfo.m_stClientDisconnect.m_cDisconnectReason = reason;
	
	stNetHead.m_iPackageSize = stNetHead.size();

	char szBuff[256];
	Int32 iCodeSize = (Int32)sizeof(szBuff);
	if (success != stNetHead.encode(szBuff, iCodeSize) || success != m_pstCSChannel->append((const char*)szBuff, iCodeSize))
	{
		DebugLogStream << "in UdpNetworks::notify_client_disconnected, encode or append packet(size: " << iCodeSize << ") failed, discard the packet" << LogStream::eos;
		return false;
	}

	InfoLogStream << "in UdpNetworks::notify_client_disconnected, append packet(size:" << iCodeSize << ", ctrlId:" << (UInt16)stNetHead.m_cCtrlId << ") success!" << LogStream::eos;
	InfoLogStream << "in UdpNetworks::notify_client_disconnected, cs channel code size; " << m_pstCSChannel->get_codesize() << LogStream::eos;

	return true;
}

bool UdpNetworks::notify_client_connected(UCPlayer* player)
{
	if (NULL == player)
	{
		return false;
	}

	DebugLogStream << "in UdpNetworks::notify_client_connected, notify service player(accId:" << player->m_unAccId << ",roleId:" << player->m_unRoleId << ") was connected!" << LogStream::eos;
	//
	CNetHead stNetHead;
	stNetHead.m_cCtrlId = enmControlId_notify_start;
	stNetHead.m_unAccId = player->m_unAccId;
	stNetHead.m_RoleId = player->m_unRoleId;		//now, role id is still invalid
	stNetHead.m_stControlInfo.m_stSourceIPInfo.m_iSourceIP = (int32_t)player->m_pstPeer->address.host;
	stNetHead.m_stControlInfo.m_stSourceIPInfo.m_nSourcePort = (int16_t)player->m_pstPeer->address.port;

	stNetHead.m_iPackageSize = stNetHead.size();

	//
	char szBuff[256];
	Int32 iCodeSize = (Int32)sizeof(szBuff);
	if (success != stNetHead.encode(szBuff, iCodeSize) || success != m_pstCSChannel->append((const char*)szBuff, iCodeSize))
	{
		ErrorLogStream << " in UdpNetworks::notify_client_connected, encode or append packet to service failed, size:" << iCodeSize << "), ctrlId:" << (UInt16)stNetHead.m_cCtrlId << LogStream::eos;
		disconnect_client(player, enmDisconnectReason_invalid_newclient); //for one new client, we don't need notify the service
		return false;
	}

	InfoLogStream << "in UdpNetworks::notify_client_connected, append packet(size:" << iCodeSize << ") to notify player(accId:" << player->m_unAccId << ", roleId:" << player->m_unRoleId << ") was connected successfully!" << LogStream::eos;
	InfoLogStream << "in UdpNetworks::notify_client_connected, cs channel code size; " << m_pstCSChannel->get_codesize() << LogStream::eos;

	return true;

}

bool UdpNetworks::NotifyClientPing(UCPlayer* player)
{
	if (NULL == player || player->m_unRoleId == 0)
	{
		return false;
	}

	CNetHead stNetHead;
	stNetHead.m_cCtrlId = enmControlId_notify_ping;
	stNetHead.m_unAccId = player->m_unAccId;
	stNetHead.m_RoleId = player->m_unRoleId;		//now, role id is still invalid
	stNetHead.m_stControlInfo.m_iPing = (int32_t)(player->m_pstPeer ? player->m_pstPeer->roundTripTime : 0);

	stNetHead.m_iPackageSize = stNetHead.size();

	//
	char szBuff[256];
	Int32 iCodeSize = (Int32)sizeof(szBuff);
	if (success != stNetHead.encode(szBuff, iCodeSize) || success != m_pstCSChannel->append((const char*)szBuff, iCodeSize))
	{
		ErrorLogStream << "encode or append packet to service failed, size:" << iCodeSize << "), ctrlId:" << (UInt16)stNetHead.m_cCtrlId << LogStream::eos;
		disconnect_client(player, enmDisconnectReason_service); 
		return false;
	}

	return true;
}

void UdpNetworks::chech_channels(const Avalon::Time& now)
{
	//
	Int32  iDataSize = 0;
	static char szBuff[max_relaysvr_sc_packet_size];
	for (Int32 i = 0; i < max_packet_once_receive; ++i)
	{
		iDataSize = (Int32)sizeof(szBuff);
		if (success != m_pstSCChannel->pop(szBuff, iDataSize))
		{
			break;
		}

		if (iDataSize <= 0)
		{
			DebugLogStream << "in UdpNetworks::chech_channels, pop invalid code size:" << iDataSize << LogStream::eos;
			break;
		}

		//
		process_service_packet(szBuff, iDataSize);
	}

}

Int32 UdpNetworks::process_service_packet(char* packet, const Int32 size)
{
	//InfoLogStream << "in UdpNetworks::process_service_packet, to process service packet(size:" << (Int32)size << ")" << LogStream::eos;
	if (NULL == packet || size <= 0)
	{
		return fail;
	}

	CNetHead stNetHead;
	Int16 headSize = 0;
	if (success != stNetHead.decode((const char*)packet, size, &headSize))
	{
		DebugLogStream << "in UdpNetworks::process_service_packet, CNetHead::decode fail!" << LogStream::eos;
		return fail;
	}
	
	//DebugLogStream << "in UdpNetworks::process_service_packet, to process service packet(recv size:" << (Int32)size << ", packet size: " << (UInt32)stNetHead.m_iPackageSize << ") ctrlId:" << (UInt16) stNetHead.m_cCtrlId  << " for player(accId:" << (UInt32)stNetHead.m_unAccId << ",roleId:" << (UInt64)stNetHead.m_RoleId << ")" << LogStream::eos;

	//
	if (size != stNetHead.m_iPackageSize)
	{
		DebugLogStream << "in UdpNetworks::process_service_packet, received packet(size:" << (Int32)size << ") was not equal to CNetHead.Packetsize(" << (Int32)stNetHead.m_iPackageSize <<"), just discard!" << LogStream::eos;
		return fail;
	}

	Int32 iBodySize = size - headSize;  //cs protocol size
	char* pszBody = packet + headSize;

	//
	UCPlayer *player = UCPlayerMgr::Instance()->find_player(stNetHead.m_unAccId);
	if (NULL == player)
	{
		DebugLogStream << "in UdpNetworks::process_service_packet, can't find player(accId:" << (UInt32)stNetHead.m_unAccId << "), so just discard!" << LogStream::eos;
		return fail;
	}

	if (player->m_unRoleId != stNetHead.m_RoleId)
	{
		DebugLogStream << "in UdpNetworks::process_service_packet, player(roleId:" << player->m_unRoleId << ") was not equal to CNetHead.RoleId(" << (UInt64)stNetHead.m_RoleId << "), just discard!" << LogStream::eos;
		return fail;
	}

	//
	//DebugLogStream << "in UdpNetworks::process_service_packet, to process service packet(size:" << (Int32)size << ",type:" << (Int16)stNetHead.m_cCtrlId << ") player(accId:" << player->m_unAccId << ", roleId:" << player->m_unRoleId <<")" << LogStream::eos;
	Int32 result = success;
	switch (stNetHead.m_cCtrlId)
	{
		case enmControlId_request_transmit:
			result = process_service_transmit_request(player, pszBody, iBodySize, stNetHead.m_stControlInfo.m_iPing);
			break;
		case enmControlId_request_disconnect_client:
			result = process_service_disconnect_request(player);
			break;
		default:
			result = fail;
			break;
	}

	return result;
}

Int32 UdpNetworks::process_service_transmit_request(UCPlayer* player, char* packet, Int32 size, Int32 flag)
{
	//InfoLogStream << "in  UdpNetworks::process_service_transmit_request " << LogStream::eos;
	if (NULL == player || NULL == packet || !size)
	{
		return fail;
	}

	//InfoLogStream << "in  UdpNetworks::process_service_transmit_request, to process player(accId:" << player->m_unAccId << ", roleId:" << player->m_unRoleId << ")'s packet(size: " << size << "), flag:" << flag << LogStream::eos;

	if (NULL == player->m_pstPeer)
	{
		DebugLogStream << "in  UdpNetworks::process_service_transmit_request peer is null" << LogStream::eos;
		return fail;
	}

	player->RecordMsg(packet, size, false);

	//
	if (!flag) //service  do not disconnect the client
	{
		send_packet_reliable(player, packet, size);
	}
	else
	{
		//first send the packet, then disconnect the client
		send_packet_reliable_immediately(player, packet, size);
		disconnect_client(player, enmDisconnectReason_service); //disconnect the client, and deallocate the UCPlayer object, player was invalid after called the routine
	}

	return success;
}

Int32 UdpNetworks::process_service_disconnect_request(UCPlayer* player)
{
	return disconnect_client(player, enmDisconnectReason_service);
}

Int32 UdpNetworks::send_packet_reliable(UCPlayer* player, char* packet, Int32 size)
{
	if (NULL == packet || NULL == player || !size)
	{
		return fail;
	}

    //DebugLogStream << "send packet(" << size << ") to player(" << player->m_unAccId << ")." << LogStream::eos;

	if (NULL == player->m_pstPeer)
	{
		return fail;
	}

	//InfoLogStream << "in UdpNetworks::send_packet_reliable, send reliably packet(size:" << size << ") to player(accId" << player->m_unAccId << ", roleId:" << player->m_unRoleId << ")" << LogStream::eos;

	enet_uint32 flag = 0;
	flag |= ENET_PACKET_FLAG_RELIABLE;
	ENetPacket* toSendPacket = enet_packet_create((const void*)packet, (size_t)size, flag); //创建的ENetPacket并不需要自己释放, enet会自动释放
	if (NULL == toSendPacket)
	{
		DebugLogStream << "in UdpNetworks::send_packet_reliable, create packet fail! " << LogStream::eos;
		return fail;
	}

	//InfoLogStream << "in UdpNetworks::send_packet_reliable, to send packet(size: " << size << ") to player(accId:" << player->m_unAccId << ",roleId:" << player->m_unRoleId << ")" << LogStream::eos;
	if (enet_peer_send(player->m_pstPeer, enet_default_channel_id, toSendPacket) < 0)
	{
		DebugLogStream << "in UdpNetworks::send_packet_reliable, send packet(size: " << size << ") to player(accId:" << player->m_unAccId << ",roleId:" << player->m_unRoleId << ") failed!" << LogStream::eos;
		return fail;
	}
	
	//InfoLogStream << "in UdpNetworks::send_packet_reliable, to send packet(size: " << size << ") to player(accId:" << player->m_unAccId << ",roleId:"
	//	<< player->m_unRoleId << ") successfully! at time(" << Avalon::Time::CurrentTime().MSec() << ")" << LogStream::eos;

	return success;
}

Int32 UdpNetworks::send_packet_reliable_immediately(UCPlayer* player, char* packet, Int32 size)
{
	if (success != send_packet_reliable(player, packet, size))
	{
		if (player)
		{
			DebugLogStream << "in UdpNetworks::send_packet_reliable_immediately, send packet(size:" << size << ") to player(accId:" << player->m_unAccId << ", roleId:" << player->m_unRoleId << ") failed" << LogStream::eos;
		}
		else
		{
			DebugLogStream << "in UdpNetworks::send_packet_reliable_immediately, send packet(size:" << size << ") to client failed!" << LogStream::eos;
		}
		
		return fail;
	}

	//flush now
	enet_host_flush(m_pstENetHost);

	//InfoLogStream << "in UdpNetworks::send_packet_reliable_immediately, send packet(size:" << size << ") to player(accId:" << player->m_unAccId << ", roleId:" 
	//	<< player->m_unRoleId << ") successfully at time(" << Avalon::Time::CurrentTime().MSec() <<")" << LogStream::eos;


	return success;
}
