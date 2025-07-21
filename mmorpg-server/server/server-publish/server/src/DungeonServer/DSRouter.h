#ifndef _DS_ROUTER_H_
#define _DS_ROUTER_H_

#include <CLDefine.h>
#include <AvalonDefine.h>
#include <AvalonProtocol.h>
#include <AvalonSingleton.h>
#include <AvalonNetConnection.h>

class DSRouter : public Avalon::Singleton<DSRouter>
{
public:
	/**
	*@brief ���͵�Service
	*/
	bool SendToService(UInt32 id, Avalon::Packet* packet);
	bool SendToService(UInt32 id, Avalon::Protocol& protocol);

	/**
	*@brief ���͵�RelayServer
	*/
	bool SendToRelay(UInt32 nodeid, Avalon::Protocol& protocol);

	/**
	*@brief ���͵�scene
	*/
	bool SendToScene(UInt32 serviceId, UInt32 sceneId, Avalon::Protocol& protocol);

	/**
	*@brief ֱ�ӷ��͵�scene
	*/
	bool SendToScene(UInt32 sceneSessionId, Avalon::Protocol& protocol);

	/**
	*@brief ������Ϣ��world
	*/
	bool SendToWorld(UInt32 serviceId, UInt32 sceneWorldId, Avalon::Protocol& protocol);

private:
};

#endif