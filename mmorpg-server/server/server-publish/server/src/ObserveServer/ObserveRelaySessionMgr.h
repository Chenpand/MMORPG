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
*@brief RelayServer�Ự������
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
	*@brief ͨ���ڵ�ID����һ��RelayServer
	*/
	ObserveRelaySession* FindSessionByNodeId(UInt32 nodeId);

	/**
	*@brief ѡ��һ��RelayServer������
	*/
	ObserveRelaySession* SelectRelayServer();

	/**
	*@brief ����RelayServer�ĸ���
	*/
	void UpdateRelayServerAbality(UInt32 nodeID, UInt32 abality);

    /**
    *@brief ����RelayServer�ı������
    */
    void AddRelayServerShutdownChecker(UInt32 nodeID);

    /**
    *@brief ɾ��RelayServer�ı������
    */
    void DelRelayServerShutdownChecker(UInt32 nodeID);

	/**
	*@brief ��ȡ���е�RelayServer
	*/
	std::vector<ObserveRelaySession>& GetRelaySessiones() { return m_Sessions; }

public://�¼�

	void OnTick(const Avalon::Time& now);

	bool OnRelayServerConnected(Int32 id);

	void OnRelayServerDisconnected(UInt32 id);

private:
	//���ػỰmap
	std::vector<ObserveRelaySession>  m_Sessions;
    std::vector<ObserveRelayServerShutdownChecker> m_shutdownChecker;
};

#endif
