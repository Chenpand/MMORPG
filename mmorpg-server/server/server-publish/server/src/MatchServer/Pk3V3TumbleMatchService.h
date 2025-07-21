#ifndef __PK_3V3_TUMBLE_MATCH_SERVICE_H__
#define __PK_3V3_TUMBLE_MATCH_SERVICE_H__

#include "PKRoomMatchService.h"
#include "Pk3V3TumbleMatchRace.h"

class Pk3V3TumbleMatchService : public PkRoomMatchService
{
public:
	Pk3V3TumbleMatchService();
	virtual ~Pk3V3TumbleMatchService();

protected:
	virtual MatchRace* CreateRace() { return new Pk3V3TumbleMatchRace(PK_3V3_TUMBLE); }
	virtual RaceType GetRaceType() { return RT_3V3_TUMBLE; }
};

#endif