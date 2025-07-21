#ifndef _BATTLE_SCENE_H_
#define _BATTLE_SCENE_H_

#include <AvalonNetConnection.h>
#include <AvalonProtocol.h>
#include <CLDefine.h>
#include <CLSceneDataEntry.h>

class BattleScene
{
public:
	BattleScene();
	~BattleScene();

public:
	bool Init(UInt32 nodeId, UInt32 sceneId);

	UInt32 GetSceneID() const{ return m_SceneId; }
	UInt32 GetNodeID() const{ return m_NodeId; }
	
	void SetStatus(UInt8 status){ m_Status = status; }
	UInt8 GetStatus() const{ return m_Status; }

	void SetSubType(SceneSubType subType){ m_SubType = subType; }
	SceneSubType GetSubType() const{ return m_SubType; }

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
	//������
	SceneSubType			m_SubType;
};

#endif
