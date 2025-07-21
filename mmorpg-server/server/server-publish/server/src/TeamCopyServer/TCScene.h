#ifndef _TC_SCENE_H_
#define _TC_SCENE_H_

#include <set>
#include <CLDefine.h>

namespace Avalon{
	class Packet;
	class Protocol;
};

class TCScene
{
public:
	TCScene();
	~TCScene();

	/**
	*@brief ��������
	*/
	void SetSceneType(UInt32 type) { m_SceneType = type; }
	UInt32 GetSceneType() { return m_SceneType; }

	/**
	*@brief ���
	*/
	void AddPlayer(UInt64 playerId);
	void RemovePlayer(UInt64 playerId);

	/**
	*@brief �ѵ�ǰ������Ϣͬ�������
	*/
	std::vector<UInt64>& GetCurScenePlayer() { return m_PlayerVec; }

	/**
	*@brief �㲥��Ϣ
	*/
	void BroadcastPacket(Avalon::Packet* packet);
	void BroadcastProtocol(Avalon::Protocol& protocol);

private:

	// ��������
	UInt32 m_SceneType;

	// ����б�
	std::set<UInt64> m_PlayerSet;
	std::vector<UInt64> m_PlayerVec;
};

#endif
