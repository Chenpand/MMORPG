#ifndef _TC_SCENE_H_
#define _TC_SCENE_H_

#include <set>
#include <CLDefine.h>

namespace Avalon{
	class Packet;
	class Protocol;
};

class USPlayer;

class UScene
{
public:
	UScene();
	virtual ~UScene();

	/**
	*@brief ����ID
	*/
	void SetSceneID(UInt32 id) { m_SceneId = id; }
	UInt32 GetSceneID() { return m_SceneId; }

	/**
	*@brief ��������
	*/
	void SetSceneType(UInt32 type) { m_SceneType = type; }
	UInt32 GetSceneType() { return m_SceneType; }

	/**
	*@brief ���
	*/
	USPlayer* AddPlayer(USPlayer* player);
	void RemovePlayer(USPlayer* player);
	void ClearPlayer();
	UInt32 GetPlayerNum();

	/**
	*@brief �ѵ�ǰ������Ϣͬ�������
	*/
	std::set<USPlayer*>& GetCurScenePlayer() { return m_PlayerSet; }

	/**
	*@brief �㲥��Ϣ
	*/
	void BroadcastPacket(Avalon::Packet* packet);
	void BroadcastProtocol(Avalon::Protocol& protocol);

protected:
	// ����ID
	UInt32 m_SceneId;
	// ��������
	UInt32 m_SceneType;

	// ����б�
	std::set<USPlayer*> m_PlayerSet;
};

#endif
