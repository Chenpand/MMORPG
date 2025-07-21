#ifndef __DUNGEON_TEAM_SERVICE_H__
#define __DUNGEON_TEAM_SERVICE_H__

/*
	组队打关卡
*/

#include "MatchService.h"
#include "DungeonMatchRace.h"

class DungeonTeamService : public MatchService
{
public:
    DungeonTeamService();

    virtual bool Init();
    virtual bool OnTeamStartMatch(MatchTeam* team);
    virtual bool OnJoinMatchPool(MatchTeam* team);
    virtual UInt32 OnTeamCancelMatch(ObjID_t teamId);
    virtual bool OnMatchSuccess(VMatchTeam allTeams);

    /**
    *@brief RelayServer开始战斗返回
    */
    virtual void OnRelayServerStartResponse(UInt32 result, ObjID_t gamesessionID, UInt32 abality, const Avalon::SockAddr& address);

protected:
    virtual MatchRace* CreateRace() { return new DungeonMatchRace(); }

};

#endif