#ifndef _SCENE_MAIL_HANDLER_H_
#define _SCENE_MAIL_HANDLER_H_

#include <CLNetwork.h>
#include <CLMailProtocol.h>

void HandleProtocol(CLProtocol::SceneMailCheckPackageReq& req);

class MailGiveItemsFunction : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const;
};

void HandleProtocol(CLProtocol::SceneMailRemoveCostReq& req);

#endif
