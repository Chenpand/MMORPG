#include "CSChallengeHandler.h"

#include "ChallengeMgr.h"
#include "GameZoneMgr.h"

void WorldChallengeSigninFunction::operator()(Avalon::Packet* packet) const
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(packet->GetConnID());
	if(zone == NULL) return;

	ObjID_t	id = 0;
	std::string name;
	UInt32  gateId = 0;
	UInt16  zoneId = 0;
	UInt16	level = 0;
	UInt8	occu = 0;
	UInt32	power = 0;

	Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetSize());
	try
	{
		stream & id & name & gateId & zoneId & level & occu;
	}
	catch (Avalon::NetException&)
	{
		return;
	}

	ChallengeMgr::Instance()->Signin(zone, id, name, gateId, zoneId, level, occu, power, packet->GetBuffer() + stream.GetPos(), packet->GetSize() - stream.GetPos());
}

void HandleProtocol(Protocol::ChallengeWorldChallengeSignout& protocol)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(protocol.GetConnID());
	if(zone == NULL) return;

	ChallengeMgr::Instance()->Signout(zone, protocol.id);
}


void HandleProtocol(Protocol::ChallengeWorldChallengeInfoReq& protocol)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(protocol.GetConnID());
	if(zone == NULL) return;

	ChallengeMgr::Instance()->SendInfo(zone, protocol.playerId, protocol.level);
}

void HandleProtocol(Protocol::ChallengeWorldChallengeSortListReq& protocol)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(protocol.GetConnID());
	if(zone == NULL) return;

	ChallengeMgr::Instance()->SendSortList(zone, protocol.playerId, protocol.group, protocol.start, protocol.num);
}

void HandleProtocol(Protocol::ChallengeWorldChallengeGetAwardReq& protocol)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(protocol.GetConnID());
	if(zone == NULL) return;

	ChallengeMgr::Instance()->GetAward(zone, protocol.playerId, protocol.type, protocol.index, protocol.minVal);
}
