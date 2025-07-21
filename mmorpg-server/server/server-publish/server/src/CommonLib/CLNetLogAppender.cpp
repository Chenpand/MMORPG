#include "CLNetLogAppender.h"
#include "CLSysProtocol.h"

CLNetLogAppender::CLNetLogAppender(const std::string& logname, Avalon::NetConnectionPtr conn)
	:m_Name(logname), m_Conn(conn)
{
}

CLNetLogAppender::~CLNetLogAppender()
{
}

bool CLNetLogAppender::DoOpen()
{
	return true;
}

bool CLNetLogAppender::DoAppend(const Avalon::LogEvent& logEvent)
{
	if(m_Conn == NULL) return false;

	Avalon::Packet* packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::SYS_NET_LOGGER);
	Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
	stream & m_Name & logEvent.time.MSec() & UInt8(logEvent.priority);
	int len = strlen(logEvent.content);
	stream & UInt16(len);
	stream.SeriaArray(logEvent.content, size_t(len));
	packet->SetSize(stream.GetPos());
	if(!m_Conn->SendPacket(packet))
	{
		Avalon::Packet::Destroy(packet);
		return false;
	}
	Avalon::Packet::Destroy(packet);
	return true;
}

void CLNetLogAppender::DoClose()
{
}
