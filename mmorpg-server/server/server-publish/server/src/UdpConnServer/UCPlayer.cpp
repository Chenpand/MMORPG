#include "UCPlayer.h"
#include <AvalonPacket.h>
#include <AvalonStringUtil.h>
#include "UCPlayerMonitorMgr.h"

UCPlayer::UCPlayer(UInt32 accId, ENetPeer* peer) :m_unAccId(accId), m_pstPeer(peer)
{
	m_unRoleId = 0;
	m_IsMonitored = UCPlayerMonitorMgr::Instance()->IsPlayerMonitored(accId);
	m_NetSimpleFile = m_NetDetailFile = NULL;
}

void UCPlayer::RecordMsg(char* data, int length, bool isRecv)
{
	if (!m_IsMonitored || !data || length < (int)Avalon::Packet::HEAD_SIZE)
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
#if defined(WIN32) || defined(_WIN32)
		fprintf(m_NetSimpleFile, "[%llu] player(%u,%llu) %s packet(%u) len(%d).\n", time(NULL), m_unAccId, m_unRoleId, isRecv ? "recv" : "send", packet->GetID(), length);
#else
		fprintf(m_NetSimpleFile, "[%lu] player(%u,%llu) %s packet(%u) len(%d).\n", time(NULL), m_unAccId, m_unRoleId, isRecv ? "recv" : "send", packet->GetID(), length);
#endif
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

void UCPlayer::SetMonitored(bool monitored)
{
	m_IsMonitored = monitored;
	if (!m_IsMonitored)
	{
		CloseMonitorFile();
	}
}

void UCPlayer::CloseMonitorFile()
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