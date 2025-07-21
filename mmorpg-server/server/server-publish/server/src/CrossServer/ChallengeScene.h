#ifndef _CHALLENGE_SCENE_H_
#define _CHALLENGE_SCENE_H_

#include <AvalonNetConnection.h>
#include <AvalonProtocol.h>
#include <CLDefine.h>

class ChallengeScene
{
public:
	ChallengeScene();
	~ChallengeScene();

public:

	bool Init(UInt32 nodeId, UInt32 sceneId);

	UInt32 GetSceneID() const{ return m_SceneId; }
	UInt32 GetNodeId() const{ return m_NodeId; }
	
	void SetStatus(UInt8 status){ m_Status = status; }
	UInt8 GetStatus() const{ return m_Status; }

public:
	void SendToScene(Avalon::Protocol& protocol);
	void SendToScene(Avalon::Packet* packet);

private:
	//����
	Avalon::NetConnectionPtr m_Conn;
	//����id
	UInt32					m_SceneId;
	//״̬ 0δʹ�� 1ʹ����
	UInt8					m_Status;
	// NodeID
	UInt32					m_NodeId;
};

#endif
