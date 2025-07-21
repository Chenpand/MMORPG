#ifndef _GAS_GAMEZONE_MGR_H_
#define _GAS_GAMEZONE_MGR_H_

#include <CLObjectIndex.h>
#include <AvalonSingleton.h>
#include <AvalonProtocol.h>
#include "GASGameZone.h"

/** 
 *@brief 游戏区管理器
 */
class GASGameZoneMgr : public Avalon::Singleton<GASGameZoneMgr>
{
	typedef std::map<UInt32, GASGameZone*>	GASGameZoneMap;

public:
	GASGameZoneMgr();
	~GASGameZoneMgr();

public:
	void Final();

public:
	/**
	 *@brief 添加、删除、查找游戏区
	 */
	bool AddGASGameZone(GASGameZone* zone);
	void RemoveGASGameZone(GASGameZone* zone);
	GASGameZone* FindGASGameZone(UInt32 id);

	/**
	 *@brief 遍历所有游戏区
	 */
	void VisitGASGameZones(GASGameZoneVisitor& visitor);

public://消息相关
	/** 
	 *@brief 广播消息
	 */
	void BroadcastToGASGameZone(Avalon::Packet* packet);
	void BroadcastToGASGameZone(Avalon::Protocol& protocol);

	/**
	*@brief 广播消息到word
	*/
	void BroadcastToWorld(Avalon::Protocol& protocol);

	/**
	*@brief 广播消息到所有gate
	*/
	void BroadcastToGate(Avalon::Protocol& protocol);


public://事件
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
	CLUIntObjIndex<GASGameZone>	m_Zones;
};

#endif
