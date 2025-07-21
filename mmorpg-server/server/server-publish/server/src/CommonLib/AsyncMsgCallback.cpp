#include "AsyncMsgCallback.h"
#include <AvalonPacket.h>
#include <CLSysProtocol.h>

AsyncMsgCallback::AsyncMsgCallback(Avalon::Protocol* waitMsg)
{
    m_id = 0;
    m_timeout = DEFAULT_TIMEOUT_MS;
    m_startTime = Avalon::Time::CurrentTime();
    m_waitMsg = waitMsg;
}

AsyncMsgCallback::~AsyncMsgCallback()
{
    if (m_waitMsg)
    {
        delete m_waitMsg;
		m_waitMsg = NULL;
    }
}

bool AsyncMsgCallback::IsTimeOut(const Avalon::Time& now) const
{
    if (m_timeout <= 0)
    {
        return false;
    }

    if (now < m_startTime)
    {
        return false;
    }

    if (now.Sec() < (m_startTime.Sec() + m_timeout))
    {
        return false;
    }

    return true;
}

UInt32 AsyncMsgCallback::GetWaitMsgID() const
{
    if (m_waitMsg)
    {
        return m_waitMsg->GetProtocolID();
    }

    return 0;
}

Avalon::Packet* AsyncMsgCallback::GetRequestPacket(Avalon::Protocol& protocol)
{
    Avalon::Packet* packet = Avalon::Packet::Create();
    if (!protocol.Encode(packet)) 
    {
        Avalon::Packet::Destroy(packet);
        return NULL;
    }

    CLProtocol::SysAsyncWaitMsg req(packet);
    req.id = GetID();

    Avalon::Packet* packet2 = Avalon::Packet::Create();
    if (!req.Encode(packet2))
    {
        Avalon::Packet::Destroy(packet2);
        return NULL;
    }

    return packet2;
}

void AsyncMsgCallback::OnRecv(Avalon::Packet* packet)
{
    if (!packet)
    {
        OnFailed(MCE_RECV_INVALID_PACKET);
        return;
    }

    if (!m_waitMsg)
    {
        OnFailed(MCE_NOT_SET_WAIT_MSG);
        return;
    }

    Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetSize());
    try
    {
        m_waitMsg->Input(stream);
    }
    catch (const Avalon::NetException&)
    {
        ErrorLogStream << "sync msg task(" << GetID() << ") faile to encode when wait msg(" 
                        << m_waitMsg->GetProtocolID() << ")." << LogStream::eos;
        OnFailed(MCE_DECODE_MSG_FAILED);
        return;
    }

    OnSuccess(m_waitMsg);
}