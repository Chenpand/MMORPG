#ifndef __PK_ROOM_MATCH_SERVICE_H__
#define __PK_ROOM_MATCH_SERVICE_H__

#include "PvPMatchService.h"
#include "PkRoomMatchRace.h"

class PkRoomMatchService : public PvPMatchService
{
	const UInt32 MATCH_MAX_WAIT_TIME = 2 * 60;
public:
	PkRoomMatchService();
	virtual ~PkRoomMatchService();

	virtual bool Init();

	virtual bool OnTeamStartMatch(MatchTeam* team);

	virtual UInt32 OnTeamCancelMatch(ObjID_t teamId);

	virtual bool OnMatchSuccess(VMatchTeam allTeams);

	virtual bool OnJoinMatchPool(MatchTeam* team);

	virtual void OnRelayServerStartResponse(UInt32 result, ObjID_t gamesessionID, UInt32 abality, const Avalon::SockAddr& address);

	virtual void OnQueryPlayerRaceInfoRes(ObjID_t sessionId, ObjID_t roleId, UInt8 result, RacePlayerInfo& info);

protected:
	virtual MatchRace* CreateRace() { return new PkRoomMatchRace(PK_3V3_ROOM); }
	virtual RaceType GetRaceType() { return RT_PK_3V3; }
};

#endif