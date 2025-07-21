#ifndef _TC_SCENE_MGR_H_
#define _TC_SCENE_MGR_H_

#include <set>
#include <AvalonSingleton.h>

#include "UScene.h"

class USPlayer;
class UnionService;

/**
 *@brief 跨服场景管理器                                                                     
 */
class USceneMgr
{
public:
	USceneMgr(UnionService& service);
	~USceneMgr();

public:
	bool Init();
	void Final();

public:
	/**
	 *@brief 事件                                                                     
	 */
	void OnTick(const Avalon::Time& now);

	/**
	*@brief 场景
	*/
	UScene* AddScene(UInt32 id);
	void RemoveScene(UInt32 Id);
	void ClearScene();

	std::map<UInt32, UScene*>& GetAllScene();
	/**
	*@brief 查找场景
	*/
	UScene* FindUScene(UInt32 Id);

	/**
	*@brief 同步消息
	*/
	void AddSyncPacket(UInt64 playerId, Avalon::Packet* packet);
	void RemoveSyncPacket(UInt64 playerId);

	/**
	*@brief 同步消息
	*/
	void SyncAllObjectToPlayer(USPlayer* player, UInt32 Id);

	/**
	*@brief 广播消息
	*/
	Avalon::Packet* GetPlayerSyncPacket(UInt64 playerId);

	/**
	*@brief 广播消息
	*/
	void BroadcastPacket(Avalon::Packet* packet);
	void BroadcastProtocol(Avalon::Protocol& protocol);


private:

	// 场景列表
	std::map<UInt32, UScene*> m_SceneMap;

	// 玩家同步消息(key->playerId)
	std::map<UInt64, Avalon::Packet*> m_SyncPacketMap;

	//被持有的跨服服务
	UnionService& m_UnionService;
};

#endif
