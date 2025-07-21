#include "ChallengeScene.h"

#include "CSNetwork.h"

ChallengeScene::ChallengeScene()
{
	m_SceneId = 0;
	m_Status = 0;
}

ChallengeScene::~ChallengeScene()
{

}

bool ChallengeScene::Init(UInt32 nodeId, UInt32 sceneId)
{
	m_Conn = CSNetwork::Instance()->FindConnection(nodeId);
	if(m_Conn == NULL) return false;

	m_SceneId = sceneId;
	return true;
}

void ChallengeScene::SendToScene(Avalon::Protocol& protocol)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	if(protocol.Encode(packet))
	{
		SendToScene(packet);
	}
	Avalon::Packet::Destroy(packet);
}

void ChallengeScene::SendToScene(Avalon::Packet* packet)
{
	if(m_Conn != NULL)
	{
		m_Conn->SendPacket(packet);
	}
}
