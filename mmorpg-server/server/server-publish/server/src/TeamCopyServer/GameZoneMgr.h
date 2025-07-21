#ifndef _GAMEZONE_MGR_H_
#define _GAMEZONE_MGR_H_

#include <CLObjectIndex.h>
#include <AvalonSingleton.h>
#include <AvalonProtocol.h>
#include "GameZone.h"

#include <set>
/** 
 *@brief 游戏区管理器
 */
class GameZoneMgr : public Avalon::Singleton<GameZoneMgr>
{
	typedef std::map<UInt32, GameZone*>	GameZoneMap;

public:
	GameZoneMgr();
	~GameZoneMgr();

public:
	bool Init();
	void Final();

public:
	/**
	 *@brief 添加、删除、查找游戏区
	 */
	bool AddGameZone(GameZone* zone);
	void RemoveGameZone(GameZone* zone);
	GameZone* FindGameZone(UInt32 id);

	GameZone* FindGameZoneByID(UInt32 id);

	/**
	 *@brief 遍历所有游戏区
	 */
	void VisitGameZones(GameZoneVisitor& visitor);

public://消息相关
	/** 
	 *@brief 广播消息
	 */
	void BroadcastToGameZone(Avalon::Packet* packet);
	void BroadcastToGameZone(Avalon::Protocol& protocol);

	void BroadcastToGameZoneWorldByZoneId(Avalon::Packet* packet,std::set<UInt32>& st);
	void BroadcastToGameZoneWorldByZoneId(Avalon::Protocol& protocol,std::set<UInt32>& st);

public://事件
	/**
	 *@brief 心跳
	 */
	void OnTick(const Avalon::Time& now);

	/**
	 *@brief 连接建立
	 */
	void OnConnected(UInt32 id);

	/**
	 *@brief 连接断开
	 */
	void OnDisconnected(UInt32 id);

private:
	//游戏区索引
	CLUIntObjIndex<GameZone>	m_Zones;
	//区id
	GameZoneMap					m_IdZones;
};

#endif
