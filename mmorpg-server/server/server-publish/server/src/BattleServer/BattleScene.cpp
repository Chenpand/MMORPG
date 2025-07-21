#include "BattleScene.h"

#include "BSNetwork.h"

BattleScene::BattleScene()
{
	m_Conn = NULL;
	m_SceneId = 0;
	m_Status = 0;
	m_NodeId = 0;
	m_SubType = SCENE_SUB_NULL;
}

BattleScene::~BattleScene()
{

}

bool BattleScene::Init(UInt32 nodeId, UInt32 sceneId)
{
	m_Conn = BSNetwork::Instance()->FindConnection(nodeId);
	if(m_Conn == NULL) return false;

	m_SceneId = sceneId;
	m_NodeId = nodeId;
	return true;
}

void BattleScene::SendToScene(Avalon::Protocol& protocol)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	if(protocol.Encode(packet))
	{
		SendToScene(packet);
	}
	Avalon::Packet::Destroy(packet);
}

void BattleScene::SendToScene(Avalon::Packet* packet)
{
	auto conn = BSNetwork::Instance()->FindConnection(m_NodeId);
	if (conn != NULL)
	{
		conn->SendPacket(packet);
	}
}
