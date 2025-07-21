#ifndef _UC_PLAYER_H_
#define _UC_PLAYER_H_
#include <CLDefine.h>
#include <AvalonObjectPool.h>
#include <AvalonPacket.h>
//#include "enet/enet.h"

struct _ENetPeer;
typedef _ENetPeer ENetPeer;
class UCPlayer 
{
	AVALON_DEFINE_OBJECTPOOL(UCPlayer, Avalon::NullMutex)
public:
	UCPlayer(UInt32 accId, ENetPeer* peer = NULL);

	~UCPlayer()
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

	UInt32		m_unAccId;
	ObjID_t		m_unRoleId;
	ENetPeer *	m_pstPeer;
	bool		m_IsMonitored;
	//记录玩家消息的基本信息的文件
	FILE*						m_NetSimpleFile;
	//记录玩家具体消息的文件
	FILE*						m_NetDetailFile;

private:
};

#endif
