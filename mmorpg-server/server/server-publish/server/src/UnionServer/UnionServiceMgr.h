#ifndef __UNION_SERVICE_MGR__
#define __UNION_SERVICE_MGR__

#include <map>
#include <AvalonSingleton.h>
#include <AvalonTime.h>
#include <CLDefine.h>
#include <CLSysProtocol.h>
#include <CLUnionServerProtocol.h>
#include "UnionService.h"

class GameZone;

#define GET_UNION_SERVICE(T) static_cast<T##*>(UnionServiceMgr::Instance()->_GetUnionService(T##::GetFunctionType()));

class UnionServiceMgr : public Avalon::Singleton<UnionServiceMgr>
{
public:
	UnionServiceMgr() {};
	~UnionServiceMgr() {};
public:
	bool Init(std::set<UnionServerFunctionType> &openFun);
	void OnTick(const Avalon::Time& now);
	void OnQuit();
	void OnConnected(GameZone* zone);
	void CheckTime(const Avalon::Time& now);

	template<typename T>
	T* GetUnionService()
	{
		return static_cast<T*>(_GetUnionService(T::GetFunctionType()));
	}

	/**
	*@brief 收到Player协议分发到对应的Service
	*/
	void OnReceivePlayerProtocol(const CLProtocol::SysTransmitPlayerToUnion& protocol);

	/**
	*@brief 收到Player断线分发到所有的Service
	*/
	void OnPlayerDisconnect(UInt64 roleId);

	/**
	*@brief 收到Player同步sceneobject
	*/
	void OnPlayerSyncSceneObject(UInt64 roleID, Avalon::Packet* packet);

	/**
	*@brief 收到Player同步删除sceneobject
	*/
	void OnPlayerDeleteSceneObject(UInt64 roleID, Avalon::Packet* packet);

	/**
	*@brief 收到Player同步ObjectProperty
	*/
	void OnPlayerSyncObjectProperty(UInt64 roleID, Avalon::Packet* packet);
	
	/**
	*@brief 收到PlayerMove分发到所有的Service
	*/
	void OnPlayerMove(UInt64 roleID, Avalon::Packet* packet);

	/**
	*@brief 收到SceneReady分发到所有的Service
	*/
	void OnSceneReady(CLProtocol::UnionSceneIsReady & protocol);

	/**
	*@brief 收到World改变ZoneID分发到所有的Service
	*/
	void OnWorldChgNewZoneID(UInt32 oldZoneID, UInt32 newZoneID);
	
	/**
	*@brief 收到活动消息
	*/
	void OnActivityNotify(CLProtocol::UnionNotifyActivityInfo& protocol);

	/**
	*@brief 获取跨服服务
	*/
	UnionService* _GetUnionService(UnionServerFunctionType type);

private:

	/**
	*@brief 创建跨服服务
	*/
	UnionService* _CreateUnionService(UnionServerFunctionType type);

private:
	std::map<UnionServerFunctionType, UnionService*> m_UnionServieces;

};


#endif