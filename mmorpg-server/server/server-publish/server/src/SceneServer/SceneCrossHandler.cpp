#include "SceneCrossHandler.h"

#include <CLGiftBagDataEntry.h>
#include <CLSysNotify.h>
#include "SceneMgr.h"
#include "PlayerMgr.h"
#include "SSRouter.h"
#include "SysInterface.h"
#include <CLSceneProtocol.h>

/*
void HandleProtocol(CLProtocol::SceneWorldChallengeSigninReq& protocol)
{
}

void SendChallengersFunction::operator()(Avalon::Packet* packet) const
{
}

void HandleProtocol(CLProtocol::ChallengeWorldChallengeSyncResult& protocol)
{
}

void HandleProtocol(CLProtocol::ChallengeWorldChallengeNotifyBattle& protocol)
{
}
*/

void WorldCrossEnterSceneFunction::operator()(Avalon::Packet* packet) const
{
	ObjID_t roleId = 0;
	Player* player = NULL;
	Scene* scene = NULL;
	UInt32 gateId = 0;
	UInt32 srcSceneNodeId = 0;
	UInt32 dstSceneId = 0;

	try
	{
		Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetSize());

		stream & roleId & gateId & srcSceneNodeId & dstSceneId;

		scene = SceneMgr::Instance()->FindNormalScene(dstSceneId);
		if (!scene)
		{
			// 发送返回,ToDo...
			return;
		}

		bool newPlayer = false;
		player = PlayerMgr::Instance()->FindPlayer(roleId);
		if (!player)
		{
			player = new Player();
			newPlayer = true;
		}

		auto dir = player->GetDir();
		auto scenePos = player->GetScenePos();
		
		player->Decode(stream);
		player->SetID(roleId);
		player->SetSrcSceneNodeID(srcSceneNodeId);

		auto avatar = player->GetAvatar();

		if (!player->SetupBase() || !player->SetGateID(gateId))
		{
			// 发送返回,ToDo...
			return;
		}
		player->SetAvatar(avatar);

		if (newPlayer)
		{
			PlayerMgr::Instance()->AddPlayer(player);

			CLPosition pos = scene->GetBirthPoint();
			player->SetPos(pos);

			if (!scene->AddObject(player))
			{
				scene->RemoveObject(player);
				PlayerMgr::Instance()->RemovePlayer(player, true);

				// 发送返回,ToDo...
				return;
			}

			auto scenePos = player->GetScenePos();
			InfoLogStream << "player(" << player->GetID() << ") birth pos(" << scenePos.GetX() << ", " << scenePos.GetY() << ")." << LogStream::eos;
		}
		else
		{
			if (player->GetSceneID() != dstSceneId)
			{
				CLPosition tmPos;
				player->ChangeScene(dstSceneId, tmPos, 0);
			}

			player->SetSceneDir(dir);
			player->SetScenePos(scenePos.GetX(), scenePos.GetY());
			player->SetPos(scenePos.GetGridePos(scene->GetGridWidth(), scene->GetGridHeight()));

			scenePos = player->GetScenePos();
			InfoLogStream << "player(" << player->GetID() << ") pos(" << scenePos.GetX() << ", " << scenePos.GetY() << ")." << LogStream::eos;

			//通知网关进入场景
			player->NotifyGateEnterScene();

			//通知客户端加载场景了
			player->NotifyLoadScene();

			//同步场景对象
			player->SyncAroundToMe(true);
		}
	}
	catch (Avalon::NetSerializeException* e)
	{
		// 发送返回,ToDo...
	}

	player->SetGameStatus(PS_NORMAL);
	player->SetStatus(CREATURE_STATUS_NORMAL);

	{
		auto syncPacket = player->MakeSyncPropertyPacket(SST_AROUND);
		if (syncPacket != NULL)
		{
			player->SendPacket(syncPacket);
			Avalon::Packet::Destroy(syncPacket);
		}
	}
	
	UInt32 playerNum = scene ? scene->GetPlayerNum() : 0;
	InfoLogStream << "player(" << roleId << ") enter scene(" << dstSceneId << ") from scenenode(" << srcSceneNodeId << ") scene player num(" << playerNum << ")." << LogStream::eos;

	CLProtocol::CrossEnterSceneRes res;
	res.roleId = roleId;
	res.result = ErrorCode::SUCCESS;
	res.crossSceneNodeId = NODE_ID;
	Router::SendToSelfScene(srcSceneNodeId, res);
}

void HandleProtocol(CLProtocol::CrossEnterSceneRes& protocol)
{
	auto player = PlayerMgr::Instance()->FindNormalPlayer(protocol.roleId);
	if (!player)
	{
		// 发送返回,ToDo...
		return;
	}

	if (protocol.result != ErrorCode::SUCCESS)
	{
		// 发送返回,ToDo...
		return;
	}

	player->SetCrossSceneNodeID(protocol.crossSceneNodeId);
	
	/*
		进入跨服场景后，将玩家切到赛利亚房间，这种情况下不能通知gate和client,因为如果通知了gate，那么gate上的该玩家就会进入赛利亚房间了
	*/
	CLPosition pos;
	UInt32 majorCity = player->GetMajorCityID();
	player->SetNotifyChangeScene(false);
	player->ChangeScene(majorCity, pos, 0);
	player->SetNotifyChangeScene(true);

	InfoLogStream << "player(" << player->GetID() << ") enter wscene, nodeid(" << protocol.crossSceneNodeId << ")." << LogStream::eos;

	// 同步到world
	player->SyncEventToWorld(SET_ENTER_CROSS_SCENE, 0);
}

void HandleProtocol(CLProtocol::CrossExitSceneReq& protocol)
{
	auto player = PlayerMgr::Instance()->FindNormalPlayer(protocol.roleId);
	if (!player)
	{
		// 发送返回,ToDo...
		return;
	}

	UInt32 nodeId = player->GetCrossSceneNodeID();
	player->SetCrossSceneNodeID(0);

	//通知网关进入场景
	player->NotifyGateEnterScene();

	//通知客户端加载场景了
	player->NotifyLoadScene();

	//同步场景对象
	player->SyncAroundToMe(true);

	CLProtocol::CrossExitSceneRes res;
	res.roleId = player->GetID();
	res.result = ErrorCode::SUCCESS;
	Router::SendToWScene(nodeId, res);

	InfoLogStream << "player(" << player->GetID() << ") return to scene from wscene(" << nodeId << ")." << LogStream::eos;

	// 同步到world
	player->SyncEventToWorld(SET_EXIT_CROSS_SCENE, 0);

	// 通知world玩家当前场景
	if (player->GetScene())
	{
		CLProtocol::SceneNotifyWorldEnterScene notify;
		notify.roleId = player->GetID();
		notify.sceneId = player->GetScene()->GetID();
		Router::SendToWorld(notify);
	}
}

void HandleProtocol(CLProtocol::CrossExitSceneRes& protocol)
{
	auto player = PlayerMgr::Instance()->FindNormalPlayer(protocol.roleId);
	if (!player)
	{
		return;
	}

	if (protocol.result != ErrorCode::SUCCESS)
	{
		// 发送返回,ToDo...
		return;
	}

	player->LeaveGame();

	InfoLogStream << "player(" << player->GetID() << ") exit cross scene." << LogStream::eos;
}

void HandleProtocol(CLProtocol::SysTransmitPlayerProtocolInNode& trans)
{
	PlayerMgr::Instance()->ExecutePacket(trans.playerId, trans.packet);
}
