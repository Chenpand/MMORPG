#pragma once
#include <CLGameSessionProtocol.h>
#include <CLDungeonProtocol.h>

void HandleProtocol(CLProtocol::DungenServerTeamStartRaceReq& protocol);

void HandleProtocol(CLProtocol::DungenServerTeamStartRaceRes& protocol);

void HandleProtocol(CLProtocol::DungeonSvrDungeonRaceEnd& protocol);

void HandleProtocol(CLProtocol::WorldDungeonLeaveRace& protocol);

void HandleProtocol(CLProtocol::DungeonSvrReviveReq& protocol);


