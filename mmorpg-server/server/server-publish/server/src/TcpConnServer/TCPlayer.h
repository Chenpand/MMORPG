#ifndef _UC_PLAYER_H_
#define _UC_PLAYER_H_

#ifdef WEB_SOCKET
#include <CLWebSocketNetwork.h>
#else
#include <AvalonNetConnection.h>
#endif

#include <CLDefine.h>
#include <AvalonObjectPool.h>
#include <AvalonPacket.h>

//#include "enet/enet.h"

struct _ENetPeer;
typedef _ENetPeer ENetPeer;
class TCPlayer 
{
	AVALON_DEFINE_OBJECTPOOL(TCPlayer, Avalon::NullMutex)
public:

#ifdef WEB_SOCKET
	TCPlayer(UInt32 accId, WebSocketConnectionPtr conn);
#else
	TCPlayer(UInt32 accId, Avalon::NetConnectionPtr conn);
#endif


	~TCPlayer()
	{
		CloseMonitorFile();
	}

public:
	// 监控相关
	void RecordMsg(char* data, int length, bool isRecv);
	void SetMonitored(bool monitored);
	bool IsMonitored() const { return m_IsMonitored; }
	void CloseMonitorFile();

public:

	UInt32 m_unAccId;
	ObjID_t m_unRoleId;
	//ENetPeer * m_pstPeer;

#ifdef WEB_SOCKET
	WebSocketConnectionPtr	m_Conn;
#else
	//连接
	Avalon::NetConnectionPtr	m_Conn;
#endif

	bool		m_IsMonitored;
	//记录玩家消息的基本信息的文件
	FILE*						m_NetSimpleFile;
	//记录玩家具体消息的文件
	FILE*						m_NetDetailFile;

private:
};

#endif
