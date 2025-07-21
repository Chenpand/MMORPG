#ifndef _TC_SCENE_MGR_H_
#define _TC_SCENE_MGR_H_

#include <set>
#include <AvalonSingleton.h>

#include "TCScene.h"

class TCSPlayer;

/**
 *@brief 团本场景管理器                                                                     
 */
class TCSceneMgr : public Avalon::Singleton<TCSceneMgr>
{
public:
	TCSceneMgr();
	~TCSceneMgr();

public:
	bool Init();
	void Final();

public:
	/**
	 *@brief 事件                                                                     
	 */
	void OnTick(const Avalon::Time& now);

	/**
	*@brief 准备场景
	*/
	void AddToPrepareScene(UInt32 copyId, UInt64 playerId);
	void RemoveToPrepareScene(UInt32 copyId, UInt64 playerId);

	/**
	*@brief 同步消息
	*/
	void AddSyncPacket(UInt64 playerId, Avalon::Packet* packet);
	void RemoveSyncPacket(UInt64 playerId);

	/**
	*@brief 同步消息
	*/
	void SyncAllObjectToPlayer(TCSPlayer* player, UInt32 copyId);

	/**
	*@brief 广播消息
	*/
	Avalon::Packet* GetPlayerSyncPacket(UInt64 playerId);

	/**
	*@brief 广播消息
	*/
	void BroadcastPacket(Avalon::Packet* packet, UInt32 copyId);
	void BroadcastProtocol(Avalon::Protocol& protocol, UInt32 copyId);

private:

	/**
	*@brief 查找准备场景
	*/
	TCScene* _FindTCScene(UInt32 copyId);

private:

	// 准备场景列表(key->团本id)
	std::map<UInt32, TCScene> m_PrepareSceneMap;

	// 玩家同步消息(key->playerId)
	std::map<UInt64, Avalon::Packet*> m_SyncPacketMap;
};

#endif
