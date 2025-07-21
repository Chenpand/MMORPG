#include "TCPlayer.h"
#include <AvalonPacket.h>
#include <AvalonStringUtil.h>
#include "TCPlayerMonitorMgr.h"

#ifdef WEB_SOCKET
TCPlayer::TCPlayer(UInt32 accId, WebSocketConnectionPtr conn)
#else
TCPlayer::TCPlayer(UInt32 accId, Avalon::NetConnectionPtr conn)
#endif
:m_unAccId(accId), m_Conn(conn)
{
	m_unRoleId = 0;
	m_IsMonitored = TCPlayerMonitorMgr::Instance()->IsPlayerMonitored(accId);
	m_NetSimpleFile = m_NetDetailFile = NULL;
}

void TCPlayer::RecordMsg(char* data, int length, bool isRecv)
{
	if (!m_IsMonitored || !data || length < Avalon::Packet::HEAD_SIZE)
	{
		return;
	}

	static Avalon::Packet* packet = Avalon::Packet::Create(Avalon::Packet::HEAD_SIZE);
	packet->DecodeHeader((UInt8*)data, length);

	if (!m_NetSimpleFile)
	{
		std::string filename = std::string("Monitor/Simple_") + Avalon::StringUtil::ConvertToString(m_unAccId) + ".txt";
		m_NetSimpleFile = fopen(filename.c_str(), "w");
	}

	if (m_NetSimpleFile)
	{
		fprintf(m_NetSimpleFile, "[%llu] player(%u,%llu) %s packet(%u) len(%d).\n", time(NULL), m_unAccId, m_unRoleId, isRecv ? "recv" : "send", packet->GetID(), length);
	}

	if (!m_NetDetailFile)
	{
		std::string filename = std::string("Monitor/Detail_") + Avalon::StringUtil::ConvertToString(m_unAccId) + ".txt";
		m_NetDetailFile = fopen(filename.c_str(), "wb");
	}

	if (m_NetDetailFile)
	{
		fwrite(data, length, 1, m_NetDetailFile);
	}
}

void TCPlayer::SetMonitored(bool monitored)
{
	m_IsMonitored = monitored;
	if (!m_IsMonitored)
	{
		CloseMonitorFile();
	}
}

void TCPlayer::CloseMonitorFile()
{
	if (m_NetDetailFile)
	{
		fclose(m_NetDetailFile);
		m_NetDetailFile = NULL;
	}

	if (m_NetSimpleFile)
	{
		fclose(m_NetSimpleFile);
		m_NetSimpleFile = NULL;
	}
}