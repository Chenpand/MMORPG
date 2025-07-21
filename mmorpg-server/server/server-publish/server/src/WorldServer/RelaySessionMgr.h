#ifndef __RELAY_SESSION_MGR_H__
#define __RELAY_SESSION_MGR_H__

#include <AvalonSingleton.h>
#include <AvalonTime.h>
#include <CLObjectIndex.h>
#include "RelaySession.h"

typedef CLVisitor<RelaySession> RelaySessionVisitor;

/**
*@brief RelayServer会话管理器
*/
class RelaySessionMgr : public Avalon::Singleton<RelaySessionMgr>
{
public:
	RelaySessionMgr();
	~RelaySessionMgr();

	void Init();
	void Final();

public:
	/**
	*@brief 通过节点ID查找一个RelayServer
	*/
	RelaySession* FindSessionByNodeId(UInt32 nodeId);

	/**
	*@brief 选出一个RelayServer的网关
	*/
	RelaySession* SelectRelayServer();

	/**
	*@brief 更新RelayServer的负载
	*/
	void UpdateRelayServerAbality(UInt32 nodeID, UInt32 abality);

public://事件

	void OnTick(const Avalon::Time& now);

	bool OnRelayServerConnected(Int32 id);

	void OnRelayServerDisconnected(UInt32 id);

private:
	//网关会话map
	std::vector<RelaySession>  m_Sessions;
};

#endif
