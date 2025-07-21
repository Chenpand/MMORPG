#ifndef __VERIFY_CLIENT_MGR_H__
#define __VERIFY_CLIENT_MGR_H__

#include <AvalonSingleton.h>
#include <AvalonTime.h>
#include <CLObjectIndex.h>
#include "VerifyClient.h"

/**
*@brief 验证客户端管理器
*/
class VerifyClientMgr : public Avalon::Singleton<VerifyClientMgr>
{
public:
	/**
	*@brief 通过节点ID查找一个Client
	*/
	VerifyClient* FindClient(UInt32 id);

	/**
	*@brief 新增Client
	*/
	void AddClient(VerifyClient* client);

	/**
	*@brief 删除Client
	*/
	void DelClient(UInt32 id);

public:
	/**
	*@brief 广播消息
	*/
	void Broadcast(Avalon::Protocol& protocol);
	void Broadcast(Avalon::Packet* packet);

private:
	//网关会话map
	std::map<UInt32, VerifyClient*>		m_Clients;
};

#endif
