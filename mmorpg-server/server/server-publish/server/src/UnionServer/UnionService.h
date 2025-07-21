#ifndef __UNION_SERVICE__
#define __UNION_SERVICE__

#include "USPlayerMgr.h"
#include "USceneMgr.h"
#include <CLUnionServerProtocol.h>

class GameZone;

class UnionService
{
public:
	UnionService();
	virtual ~UnionService();
public:
	virtual bool Init();
	virtual void OnTick(const Avalon::Time& now);
	virtual void OnConnected(GameZone* zone);
	virtual void OnDayChanged() {}
	virtual USPlayer* CreatePlayer() = 0;
	virtual UScene* CreateScene() { return new UScene; }
	virtual void OnAddNewPlayer(USPlayer* player);
	virtual void OnRemovePlayer(UInt64 roleId);
	virtual bool OnCheckAddNewPlayer(UInt64 roleId, UInt32 accid);
	/**
	*@brief 返回自身服务的类型枚举
	*/
	static UnionServerFunctionType GetFunctionType() { return USFT_NONE; }

	/**
	*@brief 收到Player断线
	*/
	virtual void OnPlayerDisconnect(UInt64 roleId);
	virtual void OnPlayerMove(UInt64 roleID, Avalon::Packet* packet);
	/**
	*@brief 收到Player同步sceneobject
	*/
	virtual void OnPlayerSyncSceneObject(UInt64 roleID, Avalon::Packet* packet);

	/**
	*@brief 收到Player同步删除sceneobject
	*/
	virtual void OnPlayerDeleteSceneObject(UInt64 roleID, Avalon::Packet* packet);

	/**
	*@brief 收到Player同步ObjectProperty
	*/
	virtual void OnPlayerSyncObjectProperty(UInt64 roleID, Avalon::Packet* packet);

	/**
	*@brief 收到SceneReady
	*/
	virtual void OnSceneReady(CLProtocol::UnionSceneIsReady & protocol);

	/**
	*@brief 收到World改变ZoneID分发到所有的Service
	*/
	virtual void OnWorldChgNewZoneID(UInt32 oldZoneID, UInt32 newZoneID);

	/**
	 *@brief 收到活动消息	
	 */
	virtual void OnActivityNotify(CLProtocol::UnionNotifyActivityInfo& protocol);

public:
	void OnReceivePlayerProtocol(const CLProtocol::SysTransmitPlayerToUnion& protocol) { m_PlayerMgr.ExecuteProtocol(protocol); }
	void AddNewPlayer(CLProtocol::UnionServerAddPlayerReq& protocol);
	void RemovePlayer(CLProtocol::UnionServerDelPlayerReq& protocol);

	//目前支持发向admin,world,scene的
	void BroadcastToZone(Avalon::Packet* packet, ServerType type = ST_INVALID);
	void BroadcastToZone(Avalon::Protocol& protocol, ServerType type = ST_INVALID);

protected:
	USPlayerMgr m_PlayerMgr;
	USceneMgr m_SceneMgr;
};



#endif


