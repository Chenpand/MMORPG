#include "CLExternalNetwork.h"

#include <AvalonLogger.h>
#include <AvalonCompress.h>
extern "C"
{
//#include <zlib.h>
}

#include "CLNetAddress.h"
#include "CLNetworkPerformance.h"

UInt32 CLExternalConnection::MAX_BYTES_PER_MIN = 120000;
UInt32 CLExternalConnection::MAX_PACKETS_PER_MIN = 600;  //noted by aprilliu, 600是默认值，可以在配置文件中进行配置 该值是指一个链接上的包数量（如果取600，则意味着限制了60000ms/600 = 100ms一个包）

CLExternalConnection::CLExternalConnection(CLNetwork* network):CLNetConnection(network)
{
	m_BytesRecved = 0;
	m_PacketsRecved = 0;
}

CLExternalConnection::~CLExternalConnection()
{
}

void CLExternalConnection::DecodeHeader(UInt8* header, size_t len, UInt16& size, UInt32& id)
{
	if(strncmp((char*)header, "<policy-file-request/>", len) == 0)
	{
		static char sndbuf[]=   
			"<cross-domain-policy> "     
			"<allow-access-from domain=\"*\" to-ports=\"*\"/>"     
			"</cross-domain-policy> ";
		Send(sndbuf, strlen(sndbuf) + 1);
		Close();
		return;
	}
	else if(strncmp((char*)header, "tgw_17_forward\r\n", len) == 0)
	{
		size = TGW_HEADER_LENGTH - Avalon::Packet::HEAD_SIZE;
		id = TGW_PACKET_ID;
		return;
	}

	UInt32 sequence = 0;
	CLNetConnection::DecodeHeader(header, len, size, id, sequence);
}

void CLExternalConnection::OnDataReached()  //noted by aprilliu 此处的处理逻辑 跟 CLNetConnection::OnDataReached()重复
{
	do
	{
		Avalon::Packet* packet = RecvPacket();
		if(packet == NULL) break;

		if(GetNetwork()->PreHandlePacket(this,packet))
		{
			Avalon::Packet::Destroy(packet);
		}
		else
		{
			GetNetwork()->DispatchPacket(packet);
		}
		
		if(m_BytesRecved >= MAX_BYTES_PER_MIN || m_PacketsRecved >= MAX_PACKETS_PER_MIN) break;
	}
	while(true);
}

void CLExternalConnection::OnTick(const Avalon::Time& now)
{
	if(now > m_CountTime + 60000)
	{
		m_BytesRecved = 0;
		m_PacketsRecved = 0; //超过1分钟，则重新开始计数

		m_CountTime = now;
	}

	CLNetConnection::OnTick(now);
}

bool CLExternalConnection::PreHandlePacket(Avalon::Packet *packet)
{		
	++m_PacketsRecved;  //增加收到的包个数
	m_BytesRecved = m_BytesRecved + packet->GetSize() + Avalon::Packet::HEAD_SIZE;
	
	if(packet->GetID() == TGW_PACKET_ID) return true;	//忽略掉
	if(CLNetConnection::PreHandlePacket(packet)) return true;

	return false;
}

void CLExternalConnection::ProcessRecvPacket(Avalon::Packet *&packet)
{
}

void CLExternalConnection::ProcessSendPacket(Avalon::Packet *&packet)
{
	if(packet->GetID() & PACKET_COMPRESS_FLAG)
	{
		UInt32 packetid = packet->GetID() & ~PACKET_COMPRESS_FLAG;
		if(packet->GetSize() < MIN_COMPRESS_SIZE)
		{
			packet->SetID(packetid);
			return;
		}

		unsigned char buf[Avalon::Packet::MAX_BODY_SIZE];
		size_t bufLen = sizeof(buf);
        
        Avalon::snappy_compress((char*)buf, &bufLen, (const char*)packet->GetBuffer(), packet->GetSize());
		
		memmove(packet->GetBuffer(), buf, bufLen);
		packet->SetSize(UInt16(bufLen));
	}

	CLNetworkPerfMgr::Instance()->OnSendPacket(GetID(), packet->GetID(), packet->GetSize());
}



CLExternalNetwork::CLExternalNetwork()
{
}

CLExternalNetwork::~CLExternalNetwork()
{
}

bool CLExternalNetwork::Startup(Avalon::PropertySet *properties, CLNetAddress *addresses)
{
	if(properties == NULL || addresses == NULL) return false;

	UInt32 value = properties->GetProperty(GetName(), "max_bytes_per_min");
	if(value != 0) CLExternalConnection::MAX_BYTES_PER_MIN = value;
	value = properties->GetProperty(GetName(), "max_packets_per_min");
	if(value != 0) CLExternalConnection::MAX_PACKETS_PER_MIN = value;

	if(!CLNetwork::Startup(properties))
	{
		ErrorLogStream << "startup network failed!" << LogStream::eos;
		return false;
	}

	Avalon::SockAddr addr;
	if(!addresses->FindExternalAddress(NODE_ID, addr))
	{
		//如果是自己配置
		if(!IsSelfConfigAddress(SERVER_TYPE)) return false;
		
		if(!addr.FromString(properties->GetProperty(GetName(),"bind_address").GetString())) return false;
		
		addresses->AddExternalAddress(NODE_ID, addr);
	}

	addr.SetIP("0.0.0.0");
	if(!Bind(addr))
	{
		CLNetwork::Shutdown(0);
		return false;
	}

	return true;
}

void CLExternalNetwork::Shutdown(UInt32 inputTimeout)
{
	CLNetwork::Shutdown(inputTimeout);
}

bool CLExternalNetwork::PreHandlePacket(CLNetConnection *conn, Avalon::Packet *packet)
{
	return false;
}

Avalon::NetConnection* CLExternalNetwork::CreateConnection()
{
	return new CLExternalConnection(this);
}

