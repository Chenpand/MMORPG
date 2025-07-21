#include <CLProtocolDefine.h>
#include <CLRelationProtocol.h>

#include "GMNetwork.h"
#include "CSNetwork.h"

#include "CenterHandler.h"
#include "ChatMonitorHandler.h"
#include "CenterSysHandler.h"


template<typename ProtocolT>
class CSPacketFunction : public CLPacketFunction
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

#define CS_REGISTER_PROTOCOL(ProtocolT) \
	RegisterProtocol(ProtocolT::__ID,new CSPacketFunction<ProtocolT>());


void CSNetwork::RegisterProtocols()
{
	CS_REGISTER_PROTOCOL(CLProtocol::CenterForbidTalkListReq)
	CS_REGISTER_PROTOCOL(CLProtocol::CenterForbidTalkReq)
	CS_REGISTER_PROTOCOL(CLProtocol::CenterUnForbidTalkReq)
	CS_REGISTER_PROTOCOL(CLProtocol::CenterSyncChatInfo)
	
	
	CS_REGISTER_PROTOCOL(CLProtocol::CenterTransmitToZone)
	CS_REGISTER_PROTOCOL(CLProtocol::CenterBroadcastToZones)

	CS_REGISTER_PROTOCOL(CLProtocol::WCCDKVerifyReq)
	CS_REGISTER_PROTOCOL(CLProtocol::CenterPushNotify)
	CS_REGISTER_PROTOCOL(CLProtocol::CenterCustomServiceSignReq)
	CS_REGISTER_PROTOCOL(CLProtocol::CenterForbidAccountReq)
	CS_REGISTER_PROTOCOL(CLProtocol::CenterReportPlayerCheat)
	CS_REGISTER_PROTOCOL(CLProtocol::CenterReportPlayerRoleList)
	CS_REGISTER_PROTOCOL(CLProtocol::CenterGlobalServerInfoReq)
	CS_REGISTER_PROTOCOL(CLProtocol::CenterPlayerAddictTimeSync)
	CS_REGISTER_PROTOCOL(CLProtocol::CenterActivityYearSortListSync)
	CS_REGISTER_PROTOCOL(CLProtocol::CenterQueryConvertAccountReq)
	CS_REGISTER_PROTOCOL(CLProtocol::SysTransmitPlayerFormNodesToGlobal)
	CS_REGISTER_PROTOCOL(CLProtocol::SysTransmitFromGlobalToNode)
	CS_REGISTER_PROTOCOL(CLProtocol::SysTransmitInNodes)
	CS_REGISTER_PROTOCOL(CLProtocol::CenterPunishInfoReq)
	CS_REGISTER_PROTOCOL(CLProtocol::WorldGmOpActivtyReq)
	CS_REGISTER_PROTOCOL(CLProtocol::CenterSyncRedPacketInfo)
	CS_REGISTER_PROTOCOL(CLProtocol::CenterDeleteRedPacketInfo)
	CS_REGISTER_PROTOCOL(CLProtocol::WorldCheckRechargePushReq)
	CS_REGISTER_PROTOCOL(CLProtocol::CenterAccountLoginTimeSync)
	CS_REGISTER_PROTOCOL(CLProtocol::GASCheckHireBindRet)
	CS_REGISTER_PROTOCOL(CLProtocol::WorldQueryHireAlreadyBindRes)
}

void GMNetwork::RegisterProtocols()
{
	CS_REGISTER_PROTOCOL(CLProtocol::CenterChatMonitorSetServers)
}
