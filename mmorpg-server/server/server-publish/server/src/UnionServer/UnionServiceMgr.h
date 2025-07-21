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
	*@brief �յ�PlayerЭ��ַ�����Ӧ��Service
	*/
	void OnReceivePlayerProtocol(const CLProtocol::SysTransmitPlayerToUnion& protocol);

	/**
	*@brief �յ�Player���߷ַ������е�Service
	*/
	void OnPlayerDisconnect(UInt64 roleId);

	/**
	*@brief �յ�Playerͬ��sceneobject
	*/
	void OnPlayerSyncSceneObject(UInt64 roleID, Avalon::Packet* packet);

	/**
	*@brief �յ�Playerͬ��ɾ��sceneobject
	*/
	void OnPlayerDeleteSceneObject(UInt64 roleID, Avalon::Packet* packet);

	/**
	*@brief �յ�Playerͬ��ObjectProperty
	*/
	void OnPlayerSyncObjectProperty(UInt64 roleID, Avalon::Packet* packet);
	
	/**
	*@brief �յ�PlayerMove�ַ������е�Service
	*/
	void OnPlayerMove(UInt64 roleID, Avalon::Packet* packet);

	/**
	*@brief �յ�SceneReady�ַ������е�Service
	*/
	void OnSceneReady(CLProtocol::UnionSceneIsReady & protocol);

	/**
	*@brief �յ�World�ı�ZoneID�ַ������е�Service
	*/
	void OnWorldChgNewZoneID(UInt32 oldZoneID, UInt32 newZoneID);
	
	/**
	*@brief �յ����Ϣ
	*/
	void OnActivityNotify(CLProtocol::UnionNotifyActivityInfo& protocol);

	/**
	*@brief ��ȡ�������
	*/
	UnionService* _GetUnionService(UnionServerFunctionType type);

private:

	/**
	*@brief �����������
	*/
	UnionService* _CreateUnionService(UnionServerFunctionType type);

private:
	std::map<UnionServerFunctionType, UnionService*> m_UnionServieces;

};


#endif