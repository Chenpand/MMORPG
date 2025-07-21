#ifndef _WORLD_MAIL_HANDLER_H_
#define _WORLD_MAIL_HANDLER_H_

#include <CLNetwork.h>
#include <CLMailProtocol.h>

class WSPlayer;

void HandleProtocol(WSPlayer* player, CLProtocol::WorldMailListReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldReadMailReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGetMailItems& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldSendMail& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldDeleteMail& req);

void HandleProtocol(CLProtocol::SceneMailCheckPackageRet& ret);

class MailRemoveCostRetFunction : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const;
};

//void HandleProtocol(CLProtocol::WorldSendSysMail& req);

class WorldSendSysMailFunction : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const;
};

void HandleProtocol(CLProtocol::WorldSendSysMailToTribeLeader& req);

void HandleProtocol(CLProtocol::WorldTeamCopySendMail& req);

void HandleProtocol(CLProtocol::WorldUnionSendMail& req);


class TestItemMailFunction : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const;
};

#endif
