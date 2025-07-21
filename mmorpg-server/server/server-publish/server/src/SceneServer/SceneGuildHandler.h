#ifndef __SCENE_GUILD_HANDLER_H__
#define __SCENE_GUILD_HANDLER_H__

#include <CLGuildProtocol.h>
#include <CLPacketExecutor.h>
class Player;

void HandleProtocol(CLProtocol::WorldGuildSyncToScene& protocol);

void HandleProtocol(CLProtocol::WorldGuildSyncContribution& protocol);

void HandleProtocol(CLProtocol::WorldGuildUpdateSkill& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneGuildExchangeReq& req);

void HandleProtocol(CLProtocol::WorldGuildGiveItemReq& req);

void HandleProtocol(CLProtocol::WSAddGuildStorageItemReq& req);

class GetGuildStorageItemFunction : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const;
};

void HandleProtocol(CLProtocol::WSShopBuyAddGuildStorageRes& res);

void HandleProtocol(CLProtocol::WorldGuildEmblemSync& sync);


#endif