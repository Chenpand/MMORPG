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
	*@brief 场景类型
	*/
	void SetSceneType(UInt32 type) { m_SceneType = type; }
	UInt32 GetSceneType() { return m_SceneType; }

	/**
	*@brief 玩家
	*/
	void AddPlayer(UInt64 playerId);
	void RemovePlayer(UInt64 playerId);

	/**
	*@brief 把当前场景消息同步到玩家
	*/
	std::vector<UInt64>& GetCurScenePlayer() { return m_PlayerVec; }

	/**
	*@brief 广播消息
	*/
	void BroadcastPacket(Avalon::Packet* packet);
	void BroadcastProtocol(Avalon::Protocol& protocol);

private:

	// 场景类型
	UInt32 m_SceneType;

	// 玩家列表
	std::set<UInt64> m_PlayerSet;
	std::vector<UInt64> m_PlayerVec;
};

#endif
