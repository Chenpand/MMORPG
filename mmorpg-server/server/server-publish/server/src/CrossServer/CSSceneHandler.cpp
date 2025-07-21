#include "CSSceneHandler.h"

#include <CLErrorCode.h>
#include "CSNetwork.h"
#include "ChallengeMgr.h"
#include "CSRoomMgr.h"

void HandleProtocol(CLProtocol::WorldRegisterSceneReq& protocol)
{
	CLProtocol::WorldRegisterSceneRet ret;
	ret.sceneid = protocol.sceneid;
	ret.result = ErrorCode::SUCCESS;
	protocol.SendBack(ret);

	ChallengeMgr::Instance()->OnSceneCreated(protocol.GetConnID(), protocol.sceneid);
}

void HandleProtocol(CLProtocol::WorldUnregisterSceneReq& protocol)
{
	CLProtocol::WorldUnregisterSceneRet ret;
	ret.sceneid = protocol.sceneid;
	protocol.SendBack(ret);

	ChallengeMgr::Instance()->OnSceneDestroyed(protocol.sceneid);
}

void HandleProtocol(CLProtocol::CrossExitSceneRes& protocol)
{
	// 这种情况下直接广播给所有的wscene
	auto packet = Avalon::Packet::Create();
	if (protocol.Encode(packet))
	{
		CSNetwork::Instance()->Broadcast(ST_WSCENE, packet);
	}
	Avalon::Packet::Destroy(packet);
}

void HandleProtocol(CLProtocol::SceneNotifyWorldEnterScene& protocol)
{
	CSRoomMgr::Instance()->OnChangeScene(protocol.roleId, protocol.sceneId);
}
