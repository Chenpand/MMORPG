#include "SceneRoomHandler.h"
#include <CLMatchProtocol.h>
#include "PlayerMgr.h"
#include "SSRouter.h"
#include "Player.h"
#include "Scene.h"

void HandleProtocol(CLProtocol::SceneRoomStartMatch& protocol)
{
	CLProtocol::MatchServerPkRoomReq req;
	req.pkType = protocol.pkType;
	req.roomId = protocol.roomId;

	for (UInt32 i = 0; i < protocol.playerIds.size(); ++i)
	{
		PkRoomMatchPlayerInfo info;
		if (protocol.playerIds[i] != 0)
		{
			Player* player = PlayerMgr::Instance()->FindLoadingFinishPlayer(protocol.playerIds[i]);
			if (player != NULL)
			{
				info.accid = player->GetAccID();
				info.wsId = CL_MAKE_NODEID(ZONE_ID, ST_WORLD, 1);
				info.id = player->GetID();
				info.level = player->GetLevel();
				info.loseStreak = player->GetSeasonPkLoseStraight();
				info.winStreak = player->GetSeasonPkWinStraight();
				info.name = player->GetName();
				info.score = player->GetMatchScore();
				info.seasonLevel = player->GetSeasonLevel();
				info.seasonStar = player->GetSeasonStar();
				info.seasonExp = player->GetSeasonExp();
				info.seat = i;
				req.players.push_back(info);
			}
			else
			{
				ErrorLogStream << "playerId("<< protocol.playerIds[i] <<") Is not find " << LogStream::eos;
			}
		}
	}

	Router::SendToWorld(req);
}

void HandleProtocol(CLProtocol::CrossRoomMatchInfoReq& req)
{
	CLProtocol::CrossRoomMatchInfoRes res;

	Player* player = PlayerMgr::Instance()->FindLoadingFinishPlayer(req.playerId);
	if (player == NULL)
	{
		res.result = ErrorCode::ROOM_SYSTEM_NOT_PLAYER;
	}

	res.accId = player->GetAccID();
	res.loseStreak = player->GetSeasonPkLoseStraight();
	res.matchScore = player->GetMatchScore();

	res.playerId = player->GetID();
	res.roomId = req.roomId;
	res.seasonLevel = player->GetSeasonLevel();
	res.seasonExp = player->GetSeasonExp();
	res.seasonStar = player->GetSeasonStar();
	res.winStreak = player->GetSeasonPkWinStraight();
	res.battleCount = player->GetScoreWarBattleCount();
	Router::SendToCross(res);
}

void HandleProtocol(CLProtocol::MatchServerPkRoomRaceEnd& protocol)
{
	for (UInt32 i = 0; i < protocol.results.size(); ++i)
	{
		Player* player = PlayerMgr::Instance()->FindLoadingFinishPlayer(protocol.results[i].info.roleId);
		if (player != NULL)
		{
			player->OnRoomPvp(protocol, protocol.results[i]);
		}
		else
		{
			ErrorLogStream << "MatchServerPkRoomRaceEnd(" << protocol.results[i].info.roleId << ") player not find" << LogStream::eos;
		}
	}
}

void HandleProtocol(CLProtocol::SceneRoomSyncInfo& protocol)
{
	InfoLogStream << "SceneRoomSyncInfo(" << protocol.playerId << "|" << protocol.roomId << "|" << protocol.roomEvent << ")" << LogStream::eos;
	Player* player = PlayerMgr::Instance()->FindLoadingFinishPlayer(protocol.playerId);
	if (player == NULL)
	{
		return;
	}

	if (protocol.roomEvent == RoomEvent::ROOM_EVENT_JOIN)
	{
		player->SetRoomId(protocol.roomId);
	}
	else if (protocol.roomEvent == RoomEvent::ROOM_EVENT_QUIT)
	{
		player->SetRoomId(0);
		//player->DelAllAroundPlayers();
		if (player->GetScene() && player->GetScene()->GetDataEntry() && player->GetScene()->GetDataEntry()->subType == SCENE_SUB_ROOM)
		{
			Avalon::Packet* delpacket = player->MakeDelObjectPacket();
			player->BroadcastExceptMe(delpacket);
			Avalon::Packet::Destroy(delpacket);
		}
	}
	else
	{
		ErrorLogStream << PLAYERINFO(player) << "SceneRoomSyncInfo(" << protocol.roomId << "|" << protocol.roomEvent <<")" << LogStream::eos;
	}
}

