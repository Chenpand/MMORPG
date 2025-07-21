#ifndef _GAS_PLAYER_H_
#define _GAS_PLAYER_H_

#include <AvalonObjectPool.h>
#include <AvalonProtocol.h>
#include <CLObject.h>
#include <CLGameDefine.h>


#define PLAYERINFO(T) "uid(" << T->GetUId() << "), serverName(" << T->GetServerName() << ")"

class GASItemTransaction;

class GASPlayer : public CLObject
{
	AVALON_DEFINE_OBJECTPOOL(GASPlayer, Avalon::NullMutex)

	//道具事务超时时间
	const static UInt32 ITEM_TRANS_TIMEOUT = 5;

public:
	GASPlayer();
	~GASPlayer();

public:
	/**
	*@brief 获取唯一id
	*/
	const std::string& GetUId() const { return GetName(); }

	/**
	*@brief 设置获取账号id
	*/
	void SetAccId(UInt32 id) { m_AccId = id; }
	UInt32 GetAccId() const { return m_AccId; }

	/**
	*@brief 设置获取玩家姓名
	*/
	void SetPlayerName(const std::string& name) { m_PlayerName = name; }
	const std::string GetPlayerName() const { return m_PlayerName; }

	/**
	*@brief 设置获取平台名
	*/
	void SetPlatform(const std::string& pf) { m_Platform = pf; }
	const std::string GetPlatform() const { return m_Platform; }

	/**
	*@brief 设置获取所在服区id
	*/
	void SetServerZoneId(UInt32 id) { m_ServerZoneId = id; }
	UInt32 GetServerZoneId() const { return m_ServerZoneId; }

	/**
	*@brief 设置获取所在服名字
	*/
	void SetServerName(const std::string& server) { m_ServerName = server; }
	const std::string GetServerName() const { return m_ServerName; }

	/**
	*@brief 设置获取玩家归属节点
	*/
	void SetSourceNode(ServerType serverType, UInt32 node);
	UInt32 GetSourceNode(ServerType serverType) const;

	/**
	*@brief 设置删除时间
	*/
	void SetRemoveTime(UInt64 time) { m_RemoveTime = time; }
	UInt64 GetRemoveTime() const { return m_RemoveTime; }

public:
	void OnTick(const Avalon::Time& now);

public://道具相关
	/**
	*@brief 准备道具事务
	*/
	bool BeginItemTransaction(GASItemTransaction* transaction);

	/**
	*@brief 获取当前的道具事务
	*/
	GASItemTransaction* GetItemTransaction() const{ return m_pItemTrans; }

	/**
	*@brief 解锁财产流程锁
	*/
	void UnLockAsserts();

	/**
	*@brief 移除指定道具，在道具使用流程中用
	*/
	void RemoveItem(ObjID_t itemId);

	void OnCheckItemRet(UInt16 serial, UInt32 result, UInt32 counterTimes);

public:
	/**
	*@brief 发送到客户端
	*/
	void SendProtocol(Avalon::Protocol& protocol);
	void SendPacket(Avalon::Packet* packet);

	/**
	*@brief 发送到center
	*/
	void SendToCenter(Avalon::Protocol& protocol);
	void SendToCenter(Avalon::Packet* packet);

	/**
	*@brief 发送到admin
	*/
	void SendToAdmin(Avalon::Protocol& protocol);
	void SendToAdmin(Avalon::Packet* packet);

	/**
	*@brief 发送到world
	*/
	void SendToWorld(Avalon::Protocol& protocol);
	void SendToWorld(Avalon::Packet* packet);

	/**
	*@brief 发送到gate
	*/
	void SendToGate(Avalon::Protocol& protocol);
	void SendToGate(Avalon::Packet* packet);

	/**
	*@brief 发送到scene
	*/
	void SendToScene(Avalon::Protocol& protocol);
	void SendToScene(Avalon::Packet* packet);

private:
	UInt32 m_AccId;
	std::string m_PlayerName;
	std::string m_Platform;
	UInt32 m_ServerZoneId;
	std::string m_ServerName;
	UInt64 m_RemoveTime;

	//逻辑定时事件
	UInt64		m_TickTime[LOGICTICK_MAX];

	// 道具事务
	GASItemTransaction* m_pItemTrans;
	//道具事务超时计时
	UInt8			m_ItemTransTimeout;

	// 归属节点
	UInt32 m_SourceNode[SERVER_TYPE_MAX];
};

typedef CLVisitor<GASPlayer> GASPlayerVisitor;

#endif