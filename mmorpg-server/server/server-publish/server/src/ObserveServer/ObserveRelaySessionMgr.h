#ifndef __WATCH_RELAY_SESSION_MGR_H__
#define __WATCH_RELAY_SESSION_MGR_H__

#include <AvalonSingleton.h>
#include <AvalonTime.h>
#include <CLObjectIndex.h>
#include "ObserveRelaySession.h"

typedef CLVisitor<ObserveRelaySession> ObserveRelaySessionVisitor;

class ObserveRelayServerShutdownChecker
{
public:
	ObserveRelayServerShutdownChecker(UInt32 id = 0)
    {
        nodeId = id;
        checkTime = Avalon::Time::CurrentTime().MSec() + Avalon::Time::MSECS_OF_SEC * 60;
    }

    bool isTimeToCheck(const Avalon::Time& now) const
    {
        return now.MSec() >= checkTime;
    }

    UInt32 nodeId;
    UInt64 checkTime;
};

/**
*@brief RelayServer会话管理器
*/
class ObserveRelaySessionMgr : public Avalon::Singleton<ObserveRelaySessionMgr>
{
public:
	ObserveRelaySessionMgr();
	~ObserveRelaySessionMgr();

	void Init();
	void Final();

public:
	/**
	*@brief 通过节点ID查找一个RelayServer
	*/
	ObserveRelaySession* FindSessionByNodeId(UInt32 nodeId);

	/**
	*@brief 选出一个RelayServer的网关
	*/
	ObserveRelaySession* SelectRelayServer();

	/**
	*@brief 更新RelayServer的负载
	*/
	void UpdateRelayServerAbality(UInt32 nodeID, UInt32 abality);

    /**
    *@brief 创建RelayServer的崩溃检测
    */
    void AddRelayServerShutdownChecker(UInt32 nodeID);

    /**
    *@brief 删除RelayServer的崩溃检测
    */
    void DelRelayServerShutdownChecker(UInt32 nodeID);

	/**
	*@brief 获取所有的RelayServer
	*/
	std::vector<ObserveRelaySession>& GetRelaySessiones() { return m_Sessions; }

public://事件

	void OnTick(const Avalon::Time& now);

	bool OnRelayServerConnected(Int32 id);

	void OnRelayServerDisconnected(UInt32 id);

private:
	//网关会话map
	std::vector<ObserveRelaySession>  m_Sessions;
    std::vector<ObserveRelayServerShutdownChecker> m_shutdownChecker;
};

#endif
