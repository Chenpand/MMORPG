#ifndef _GM_CLIENT_MGR_H_
#define _GM_CLIENT_MGR_H_

#include <CLObjectIndex.h>
#include <AvalonSingleton.h>
#include "GMClient.h"

/**
 *@brief gm客户端管理器
 */
class GMClientMgr : public Avalon::Singleton<GMClientMgr>
{
public:
	GMClientMgr();
	~GMClientMgr();

public:
	bool Init();
	void Final();

public:
	/**
	 *@brief 添加、删除、查找gm客户端
	 */
	bool AddGMClient(GMClient* client);
	void RemoveGMClient(GMClient* client);
	GMClient* FindGMClient(UInt32 id);

	/**
	 *@brief 遍历所有gm客户端
	 */
	void VisitGMClients(GMClientVisitor& visitor);

public://消息相关
	/** 
	 *@brief 广播消息
	 */
	void BroadcastToGMClient(Avalon::Packet* packet);
	void BroadcastToGMClient(Avalon::Protocol& protocol);

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
	//gm客户端索引
	CLUIntObjIndex<GMClient>	m_Clients;
};

#endif
