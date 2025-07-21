#include "CLDataLogAppender.h"

#include <AvalonPacket.h>
#include "CLProtocolDefine.h"

CLDataLogAppender::CLDataLogAppender(const std::string& logname, const Avalon::SockAddr& addr)
	:m_Name(logname), m_PeerAddr(addr)
{
}

CLDataLogAppender::~CLDataLogAppender()
{
}

bool CLDataLogAppender::DoOpen()
{
	if(!m_Socket.Create(SOCK_DGRAM)) return false;

	return true;
}

bool CLDataLogAppender::DoAppend(const Avalon::LogEvent& logEvent)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::SYS_NET_LOGGER);
	Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
	stream & m_Name & logEvent.time.MSec() & UInt8(logEvent.priority);
	int len = strlen(logEvent.content);
	stream & UInt16(len);
	stream.SeriaArray(logEvent.content, size_t(len));
	packet->SetSize(stream.GetPos());
		
	UInt8* buffer = packet->GetBuffer() - Avalon::Packet::HEAD_SIZE;

	*(UInt16*)(buffer) = packet->GetSize();
	*(UInt32*)(buffer + sizeof(UInt16))= packet->GetID();
		
	Int32 ret = m_Socket.SendTo(buffer, packet->GetSize() + Avalon::Packet::HEAD_SIZE, m_PeerAddr);
	
	Avalon::Packet::Destroy(packet);
	return ret >= 0;
}

void CLDataLogAppender::DoClose()
{
	m_Socket.Close();
}
