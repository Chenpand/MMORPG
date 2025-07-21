#ifndef _SERVICESESSION_MGR_H_
#define _SERVICESESSION_MGR_H_

#include <CLObjectIndex.h>
#include <AvalonSingleton.h>
#include <AvalonProtocol.h>
#include "ServiceSession.h"

/** 
 *@brief 玩法服务器连接管理器
 */
class ServiceSessionMgr : public Avalon::Singleton<ServiceSessionMgr>
{
	typedef std::map<ServiceSessionKey, ServiceSession*>	ServiceSessionMap;

public:
	ServiceSessionMgr();
	~ServiceSessionMgr();

public:
	bool Init();
	void Final();

public:
	/**
	 *@brief 添加、删除、查找游戏区
	 */
	bool AddServiceSession(ServiceSession* zone);
	void RemoveServiceSession(ServiceSession* zone);
	ServiceSession* FindServiceSession(UInt32 id);

	ServiceSession* FindServiceSessionByKey(ServiceSessionKey key);

	/**
	 *@brief 遍历所有服务器
	 */
	void VisitServiceSessions(ServiceSessionVisitor& visitor);

public://消息相关
	/**
	*@brief 发送协议
	*/
	void SendProtocol(Avalon::Protocol& protocol);
	void SendPacket(Avalon::Packet* packet);

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
	//玩法服务器
	CLUIntObjIndex<ServiceSession>	m_Sessions;
	//服务器key
	ServiceSessionMap	m_KeyMapSessions;
};

#endif
