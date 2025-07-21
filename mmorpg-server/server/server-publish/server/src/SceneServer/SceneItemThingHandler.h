#ifndef _SCENE_ITEM_THING_HANDLER_H_
#define _SCENE_ITEM_THING_HANDLER_H_

#include <CLItemThingProtocol.h>

void HandleProtocol(CLProtocol::SceneItemThingStartReq& req);

void HandleProtocol(CLProtocol::SceneItemThingStartCancel& req);

void HandleProtocol(CLProtocol::SceneItemThingCommitReq& req);


#endif // !_SCENE_ITEM_THING_HANDLER_H_

