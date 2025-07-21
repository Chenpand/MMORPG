#include "WSScene.h"

#include <CLSceneDefine.h>
#include <CLMoveProtocol.h>
#include <CLSceneProtocol.h>

#include "WSNetwork.h"
#include "WSPlayer.h"
#include "WSTeam.h"

WSScene::WSScene()
{
	m_Id = 0;
	m_MapID = 0;
	m_Type = 0;
	m_Owner = 0;
	m_Level = 0;
	m_CreateTime = 0;
	m_pDataEntry = NULL;

	m_PlayerNum = 0;
}

WSScene::~WSScene()
{
}

bool WSScene::Init(UInt32 id, UInt32 mapid, const std::string &name, UInt32 nodeid)
{
	m_Conn = WSNetwork::Instance()->FindConnection(nodeid);
	if(m_Conn == NULL) return false;

	m_Id = id;
	m_Name = name;
	m_MapID = mapid;
	m_pDataEntry = SceneDataEntryMgr::Instance()->FindEntry(m_MapID);
	if(m_pDataEntry == NULL)
	{
		return false;
	}
	m_CreateTime = UInt32(CURRENT_TIME.Sec());

	return true;
}

void WSScene::SetParam(const char* name, UInt64 value)
{
	CLProtocol::SceneSetParam protocol;
	protocol.sceneId = GetID();
	protocol.name = name;
	protocol.value = value;
	SendToScene(protocol);
}

void WSScene::OnCreated(WSPlayer* creater)
{
	PullPlayer(creater);
}

void WSScene::OnDestroyed()
{
}

void WSScene::PullPlayer(WSPlayer* player)
{
	if(player == NULL) return;

	PullPlayerToPos(player, m_RevivePos);
}

void WSScene::PullPlayerToPos(WSPlayer* player, const CLPosition& pos)
{
	CLProtocol::SceneEnterScenePullPlayer req;
	req.playerId = player->GetID();
	req.sceneId = GetID();
	req.mapId = GetMapID();
	req.pos = pos;
	req.bFirstEnter = IsFirstEnter(player->GetID()) ? 1 : 0;
	req.level = GetLevel();
	player->SendToScene(req);

	if(GetType() == SCENE_TYPE_TEAMCOPY || GetType() == SCENE_TYPE_PRIVATESCENE)
	{
		m_EnterPlayers.insert(player->GetID());
	}
}

bool WSScene::IsFirstEnter(ObjID_t playerId)
{
	if(GetType() != SCENE_TYPE_TEAMCOPY && GetType() != SCENE_TYPE_PRIVATESCENE) return false;

	return m_EnterPlayers.find(playerId) == m_EnterPlayers.end();
}

void WSScene::SendToScene(Avalon::Protocol& protocol)
{
	if(m_Conn != NULL)
	{
		CL_SEND_PROTOCOL(m_Conn, protocol);
	}
}

void WSScene::SendToScene(Avalon::Packet* packet)
{
	if(m_Conn != NULL)
	{
		m_Conn->SendPacket(packet);
	}
}

