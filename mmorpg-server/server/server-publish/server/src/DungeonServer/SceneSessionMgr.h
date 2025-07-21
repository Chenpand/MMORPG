#ifndef _SCENE_SESSION_MGR_H_
#define _SCENE_SESSION_MGR_H_

#include <CLObjectIndex.h>
#include <AvalonSingleton.h>
#include <AvalonProtocol.h>
#include "SceneSession.h"

/** 
 *@brief 场景服务器连接管理器
 */
class SceneSessionMgr : public Avalon::Singleton<SceneSessionMgr>
{
public:
	SceneSessionMgr();
	~SceneSessionMgr();

public:
	bool Init();
	void Final();

public:
	/**
	 *@brief 添加、删除、查找游戏区
	 */
	bool AddSceneSession(SceneSession* session);
	void RemoveSceneSession(SceneSession* session);
	SceneSession* FindSceneSession(UInt32 id);

	/**
	 *@brief 遍历所有服务器
	 */
	void VisitSceneSessions(SceneSessionVisitor& visitor);

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
	CLUIntObjIndex<SceneSession>	m_Sessions;
};

#endif
