#ifndef _SS_PUSH_NOTIFY_H_
#define _SS_PUSH_NOTIFY_H_

#include "SSRouter.h"
#include "SSApplication.h"

#include <CLSceneProtocol.h>
#include <CLDefine.h>

inline void pushNotifyToWorld(UInt32 accid, UInt64 roleid, std::string title, std::string content)
{
	CLProtocol::ScenePushNotifyToWorld protocol;

	protocol.platform = PLATFORM;
	protocol.accid = accid;
	protocol.roleid = roleid;
	protocol.title = title;
	protocol.content = content;

	Router::SendToWorld(protocol);
}

#endif