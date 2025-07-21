#include "CSChallengeHandler.h"

#include "ChallengeMgr.h"
#include "GameZoneMgr.h"
#include "ChallengeScene.h"
#include "CSNetwork.h"
#include "CSRoomMgr.h"

void WorldChallengeEnterSceneReqFunction::operator()(Avalon::Packet* packet) const
{
	ObjID_t roleId;
	UInt32 gateId = 0;
	UInt32 srcSceneNodeId = 0;
	UInt32 dstSceneId = 0;

	try
	{
		Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetSize());

		stream & roleId & gateId & srcSceneNodeId & dstSceneId;

		auto challengeScene = ChallengeMgr::Instance()->FindChallengeScene(dstSceneId);
		if (!challengeScene)
		{
			// 发送返回,ToDo...
			return;
		}

		challengeScene->SendToScene(packet);

		//CSRoomMgr::Instance()->OnChangeScene(roleId, dstSceneId);
	} 
	catch (Avalon::NetSerializeException* e)
	{
		// 发送返回,ToDo...
	}
}

