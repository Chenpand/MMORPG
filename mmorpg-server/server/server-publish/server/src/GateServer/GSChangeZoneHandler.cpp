#include "GSChangeZoneHandler.h"

#include <CLErrorCode.h>
#include <AvalonSha1.h>
#include <AvalonRandom.h>
#include <sstream>

#include "GSAccountMgr.h"
#include "GSApplication.h"
#include "GSRouter.h"
#include "GSPlayerMgr.h"

void HandleProtocol(CLProtocol::GateChangeZoneCheckReq& protocol)
{
	if(protocol.srcZoneId == ZONE_ID) return;

	CLProtocol::GateChangeZoneCheckRet ret;
	ret.playerId = protocol.playerId;
	ret.zoneId = ZONE_ID;
	ret.mapId = protocol.mapId;
	ret.srcGateId = protocol.srcGateId;
	ret.result = 0;

	if(!GSApplication::Instance()->GetNetAddress()->FindExternalAddress(NODE_ID, ret.gateAddr))
	{
		ret.result = ErrorCode::LOGIN_SERVER_UNREADY;
		Router::SendToZone(protocol.srcZoneId, ret);
		return;
	}

	AccountInfo info;
	info.accid = protocol.accId;
	info.createTime = UInt32(CURRENT_TIME.Sec());
	info.gmauthority = protocol.gmAuthority;
	info.openid = protocol.openid;
	info.qqVipInfo = protocol.qqvipinfo;
	info.source1 = protocol.source1;
	info.source2 = protocol.source2;
	info.pf = protocol.pf;
	if(protocol.bReturn == 0) info.srcZoneId = protocol.srcZoneId;
	info.playerId = protocol.playerId;
	info.name = protocol.name;
	info.level = protocol.level;
	info.mapId = protocol.mapId;

	std::ostringstream oss;
	oss << info.accid << "|" << Avalon::Random::RandBetween(1, 100000) << "|" << CURRENT_TIME.MSec();
	Avalon::SHA1Engine sha1;
	sha1.update(oss.str());

	memmove(info.key, &sha1.digest().front(), 20);

	GSAccountMgr::Instance()->AddAccount(info);

	memmove(ret.key, info.key, 20);
	Router::SendToZone(protocol.srcZoneId, ret);
}

void HandleProtocol(CLProtocol::GateChangeZoneNotify& protocol)
{
	GSPlayer* player = GSPlayerMgr::Instance()->FindPlayerByID(protocol.playerId);
	if(player == NULL) return;

	player->CheckChangeZone(protocol.dstZoneId, protocol.dstMapId);
}
