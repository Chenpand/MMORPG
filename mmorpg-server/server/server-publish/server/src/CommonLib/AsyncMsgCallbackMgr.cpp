#include "AsyncMsgCallbackMgr.h"
#include "AsyncMsgCallback.h"

AsyncMsgCallbackMgr::AsyncMsgCallbackMgr()
{
    m_idSeed = 0;
}

AsyncMsgCallbackMgr::~AsyncMsgCallbackMgr()
{

}

void AsyncMsgCallbackMgr::Tick(const Avalon::Time& now)
{
    auto itr = m_msgCbs.begin();
    auto next = m_msgCbs.begin();
    for (; itr != m_msgCbs.end(); itr = next)
    {
        ++next;

        auto cb = itr->second;
        if (!cb)
        {
            m_msgCbs.erase(itr);
            continue;
        }

        if (cb->IsTimeOut(now))
        {
            cb->OnFailed(MCE_TIMEOUT);
            m_msgCbs.erase(itr);
            delete cb;
        }
    }
}

bool AsyncMsgCallbackMgr::Add(AsyncMsgCallback* cb)
{
    if (!cb)
    {
        return false;
    }

    cb->SetID(_GenID());
    m_msgCbs[cb->GetID()] = cb;

    return true;
}

void AsyncMsgCallbackMgr::OnRecvReturnMsg(ObjID_t id, Avalon::Packet* packet)
{
    AsyncMsgCallback* cb = FindCallback(id);
    if (!cb)
    {
        ErrorLogStream << "recv msg but can't find callback(" << id << ")." << LogStream::eos;
        return;
    }

    cb->OnRecv(packet);

    m_msgCbs.erase(id);
    delete cb;
}

void AsyncMsgCallbackMgr::ExecutePacket(ObjID_t id, Avalon::Packet* packet)
{
    m_Executor.ExecutePacket(packet, id);
}

bool AsyncMsgCallbackMgr::SendBackReturnMsg(ObjID_t id, Avalon::Protocol& req, Avalon::Protocol& protocol)
{
    Avalon::Packet* packet = Avalon::Packet::Create();
    if (!protocol.Encode(packet))
    {
        Avalon::Packet::Destroy(packet);
        return NULL;
    }

    CLProtocol::SysAsyncWaitMsgRet ret(packet);
    ret.id = id;

    return req.SendBack(ret);
}

AsyncMsgCallback* AsyncMsgCallbackMgr::FindCallback(ObjID_t id)
{
    auto itr = m_msgCbs.find(id);
    return itr == m_msgCbs.end() ? NULL : itr->second;
}

ObjID_t AsyncMsgCallbackMgr::_GenID()
{
    return ++m_idSeed;
}