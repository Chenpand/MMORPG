#include "BSItemHandler.h"
#include "BSRouter.h"

void HandleProtocol(CLProtocol::SceneCheckItemNumRet& protocol)
{
	Router::SendToDungeon(protocol);
}