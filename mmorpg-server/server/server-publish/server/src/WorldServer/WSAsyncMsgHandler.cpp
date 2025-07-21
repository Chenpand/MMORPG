#include "WSAsyncMsgHandler.h"
#include "AsyncMsgCallbackMgr.h"
#include "WorldDungeonHandler.h"
#include "WSEnterGameHandler.h"

void HandleProtocol(CLProtocol::SysAsyncWaitMsg& protocol)
{
    if (!protocol.packet)
    {
        return;
    }

    protocol.packet->SetConnID(protocol.GetConnID());
    protocol.packet->SetNetService(protocol.GetNetService());

    AsyncMsgCallbackMgr::Instance()->ExecutePacket(protocol.id, protocol.packet);
}

template<typename ProtocolT>
class AsyncMsgFunction : public CLParamPacketFunction<ObjID_t>
{
public:
    void operator()(Avalon::Packet* packet, ObjID_t id) const
    {
        ProtocolT protocol;
        if (protocol.Decode(packet))
        {
            HandleProtocol(id, protocol);
        }
    }
};

#define REGISTER_ASYNC_PROTOCOL(ProtocolT) \
	AsyncMsgCallbackMgr::Instance()->RegisterMsgHandler(ProtocolT::__ID,new AsyncMsgFunction<ProtocolT>)

void RegisterAsyncMsgHandlers()
{
    REGISTER_ASYNC_PROTOCOL(CLProtocol::SceneRareItemReq);
	REGISTER_ASYNC_PROTOCOL(CLProtocol::SceneQueryAccountReq);
}