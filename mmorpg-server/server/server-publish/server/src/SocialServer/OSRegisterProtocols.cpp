#include "OSNetwork.h"
#include <CLProtocolDefine.h>

#include "SocialInviteHandler.h"

template<typename ProtocolT>
class OSPacketFunction : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const
	{
		ProtocolT protocol;
		if(protocol.Decode(packet))
		{
			HandleProtocol(protocol);
		}
	}
};

#define OS_REGISTER_PROTOCOL(ProtocolT) \
	RegisterProtocol(ProtocolT::__ID,new OSPacketFunction<ProtocolT>());


void OSNetwork::RegisterProtocols()
{
	OS_REGISTER_PROTOCOL(CLProtocol::SocialInviteReportOnline)
	OS_REGISTER_PROTOCOL(CLProtocol::SocialInviteNotifyLevelup)
	OS_REGISTER_PROTOCOL(CLProtocol::SocialInviteNotifyConsume)
	OS_REGISTER_PROTOCOL(CLProtocol::SocialNotifyInviteeEnterGame)
	OS_REGISTER_PROTOCOL(CLProtocol::SocialInviteInviteeListReq)
	OS_REGISTER_PROTOCOL(CLProtocol::SocialInviteCheckGiftbagReq)
	OS_REGISTER_PROTOCOL(CLProtocol::SocialInviteCheckRecallAwardReq)
	OS_REGISTER_PROTOCOL(CLProtocol::SocialInviteCheckConsumeAwardReq)
	OS_REGISTER_PROTOCOL(CLProtocol::SocialInviteBindKLActivity)
	OS_REGISTER_PROTOCOL(CLProtocol::SocialInviteCheckKLActivityReq)
	OS_REGISTER_PROTOCOL(CLProtocol::SceneInviteBeginExchangeCDKeyReq)
	OS_REGISTER_PROTOCOL(CLProtocol::SceneInviteEndExchangeCDKeyReq)
}

