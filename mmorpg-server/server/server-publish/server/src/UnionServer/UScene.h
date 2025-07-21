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
	*@brief 场景ID
	*/
	void SetSceneID(UInt32 id) { m_SceneId = id; }
	UInt32 GetSceneID() { return m_SceneId; }

	/**
	*@brief 场景类型
	*/
	void SetSceneType(UInt32 type) { m_SceneType = type; }
	UInt32 GetSceneType() { return m_SceneType; }

	/**
	*@brief 玩家
	*/
	USPlayer* AddPlayer(USPlayer* player);
	void RemovePlayer(USPlayer* player);
	void ClearPlayer();
	UInt32 GetPlayerNum();

	/**
	*@brief 把当前场景消息同步到玩家
	*/
	std::set<USPlayer*>& GetCurScenePlayer() { return m_PlayerSet; }

	/**
	*@brief 广播消息
	*/
	void BroadcastPacket(Avalon::Packet* packet);
	void BroadcastProtocol(Avalon::Protocol& protocol);

protected:
	// 场景ID
	UInt32 m_SceneId;
	// 场景类型
	UInt32 m_SceneType;

	// 玩家列表
	std::set<USPlayer*> m_PlayerSet;
};

#endif
