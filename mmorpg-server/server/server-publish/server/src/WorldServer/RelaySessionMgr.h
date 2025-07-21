#ifndef __RELAY_SESSION_MGR_H__
#define __RELAY_SESSION_MGR_H__

#include <AvalonSingleton.h>
#include <AvalonTime.h>
#include <CLObjectIndex.h>
#include "RelaySession.h"

typedef CLVisitor<RelaySession> RelaySessionVisitor;

/**
*@brief RelayServer�Ự������
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
	*@brief ͨ���ڵ�ID����һ��RelayServer
	*/
	RelaySession* FindSessionByNodeId(UInt32 nodeId);

	/**
	*@brief ѡ��һ��RelayServer������
	*/
	RelaySession* SelectRelayServer();

	/**
	*@brief ����RelayServer�ĸ���
	*/
	void UpdateRelayServerAbality(UInt32 nodeID, UInt32 abality);

public://�¼�

	void OnTick(const Avalon::Time& now);

	bool OnRelayServerConnected(Int32 id);

	void OnRelayServerDisconnected(UInt32 id);

private:
	//���ػỰmap
	std::vector<RelaySession>  m_Sessions;
};

#endif
