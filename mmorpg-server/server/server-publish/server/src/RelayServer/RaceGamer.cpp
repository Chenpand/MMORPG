#include "RaceGamer.h"
#include <AvalonPacket.h>
#include <CLChatProtocol.h>
#include "RLPlayerMgr.h"
#include "RaceDefine.h"
#include "RaceMgr.h"


RaceGamer::RaceGamer()
{
    m_offline = false;
    m_roleId = 0;
    m_wsId = 0;
    m_status = GAMER_STATUS_WAIT_LOGIN;
    m_accId = 0;
    m_pos = 0;
    m_player = NULL;
    m_raceResult = resultBitmask_invalid;
	m_reconnecting = false;
	m_syncReconnectFrame = 0;
	m_reconnEndTime = 0;
	m_netQuality = 3;
	m_loadProgress = 0;
	m_sentPacketNum = 0;
	m_recvPacketNum = 0;
	m_lastRecvChecksumTime = 0;
	m_lastObserveFrame = 0;
	m_lastRecvHeartbeatTime = 0;
	m_lastSendHeartbeatTime = 0;
}

RaceGamer::~RaceGamer()
{

}

void RaceGamer::Online(RLPlayer* player)
{
	m_offline = false;
	m_player = player;
}

void RaceGamer::Offline(ObjID_t raceId, UInt32 waitReconnTime)
{
    m_offline = true;
    if (m_player)
    {
        // 如果没有断开连接，强制断开连接
        if (!m_player->is_offline())
        {
            m_player->disconnect();
        }

        RLPlayerMgr::Instance()->remove_player(m_player->GetID());
        RLPlayerMgr::Instance()->FreePlayer(m_player);
        SetPlayer(NULL);
    }

	m_reconnEndTime = (UInt32)CURRENT_TIME.Sec() + waitReconnTime;

	if (waitReconnTime > 0)
	{
		RaceMgr::Instance()->AddReconnectInfo(GetID(), raceId, m_reconnEndTime);
	}
}

void RaceGamer::ChangeStatus(RaceGamerStatus status)
{
    DebugLogStream << "gamer(" << GetAccountID() << ", " << GetID() << ") change status (" 
                    << (UInt8)GetStatus() << ") to (" << (UInt8)status << ")." << LogStream::eos;

    m_status = status;
}

void RaceGamer::SendEventToPlayer(const GameSessionEvent& event)
{
    if (m_player)
    {
        m_player->on_gamesession_event(event);

		if (event.id == gamesessionEventId_dismiss)
		{
			m_player = NULL;
		}
    }
}

void RaceGamer::SendProtocol(Avalon::Protocol& protocol, bool disconnect)
{
    if (!m_player)
    {
        return;
    }

    Avalon::Packet* packet = Avalon::Packet::Create();
    if (NULL == packet)
    {
        return;
    }

    if (!protocol.Encode(packet))
    {
        ErrorLogStream << "gamer(" << GetID() << ") protocol(" << protocol.GetProtocolID() << ") encode fail!" << LogStream::eos;
        Avalon::Packet::Destroy(packet);
        return;
    }

    SendPacket(packet, disconnect);
    Avalon::Packet::Destroy(packet);
}

void RaceGamer::SendPacket(Avalon::Packet* packet, bool disconnect)
{
    if (!m_player || !packet)
    {
        return;
    }

    m_player->send_packet(*packet, disconnect);
	m_sentPacketNum++;
}

bool RaceGamer::PushFrameChecksum(UInt32 frame, UInt32 checksum)
{
    // 所有的校验帧都保存下来
    UInt32 index = frame / FRAME_CHECKSUM_INTERVAL - 1;
    if (index > m_frameChecksums.size())
    {
        m_frameChecksums.resize(index);
    }
    else if (index < m_frameChecksums.size())
    {
        // 说明已经有了
        return false;
    }

	SetLastRecvChecksumTime();
    m_frameChecksums.push_back(checksum);
    return true;
}

bool RaceGamer::HaveFrameChecksum(UInt32 frame)
{
    UInt32 index = frame / FRAME_CHECKSUM_INTERVAL - 1;
    if (index >= m_frameChecksums.size())
    {
        return false;
    }

    return true;
}

void RaceGamer::SetLastRecvChecksumTime()
{
	m_lastRecvChecksumTime = (UInt32)CURRENT_TIME.Sec();
}

UInt32 RaceGamer::GetFrameChecksum(UInt32 frame)
{
    UInt32 index = frame / FRAME_CHECKSUM_INTERVAL - 1;
    if (index >= m_frameChecksums.size())
    {
        return 0;
    }

    return m_frameChecksums[index];
}

void RaceGamer::Notify(UInt16 type, UInt8 color, const char* word, bool disconnect)
{
	if (word == NULL) return;

	CLProtocol::SysNotify notify;
	notify.type = type;
	notify.channel = 0;
	notify.color = color;
	notify.word = word;
	SendProtocol(notify, disconnect);
}