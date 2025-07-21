#include "CSSceneHandler.h"

#include <CLErrorCode.h>

#include "ChallengeMgr.h"

void HandleProtocol(Protocol::WorldRegisterSceneReq& protocol)
{
	Protocol::WorldRegisterSceneRet ret;
	ret.sceneid = protocol.sceneid;
	ret.result = ErrorCode::SUCCESS;
	protocol.SendBack(ret);

	ChallengeMgr::Instance()->OnSceneCreated(protocol.GetConnID(), protocol.sceneid);
}

void HandleProtocol(Protocol::WorldUnregisterSceneReq& protocol)
{
	Protocol::WorldUnregisterSceneRet ret;
	ret.sceneid = protocol.sceneid;
	protocol.SendBack(ret);

	ChallengeMgr::Instance()->OnSceneDestroyed(protocol.sceneid);
}