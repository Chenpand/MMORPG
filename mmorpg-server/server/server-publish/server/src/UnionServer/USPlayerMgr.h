#ifndef _US_PLAYER_MGR_H_
#define _US_PLAYER_MGR_H_

#include <AvalonSingleton.h>
#include <CLPacketExecutor.h>
#include <functional>

#include <CLDefine.h>
#include "USPlayer.h"

namespace CLProtocol
{
	class SysTransmitPlayerToUnion;
}

class UnionService;

using USPlayerMap = HashMap<UInt64, USPlayer*>;
using USPlayerVec = std::vector<USPlayer*>;

struct RoleIDConnID
{
	UInt32 accID;
	UInt64 roleID;
	UInt32 connId;
	std::map<UInt32, UInt32> sourceNode;
};

/**
*@brief 跨服玩家管理器
*/
class USPlayerMgr
{


public:
	USPlayerMgr(UnionService& service);
	~USPlayerMgr();

	/**
	*@brief 初始化
	*/
	void Init();

	/**
	*@brief tick
	*/
	void OnTick(const Avalon::Time& now);

	/**
	*@brief 查找玩家
	*/
	USPlayer* FindUSPlayer(UInt64 id);

	/**
	*@brief 查找玩家
	*/
	USPlayer* FindUSPlayerByAccid(UInt32 accid);


	/**
	*@brief 增加玩家
	*/
	USPlayer* AddUSPlayer(UInt64 id);

	/**
	*@brief 删除玩家
	*/
	void RemoveUSPlayer(UInt64 id);

	/**
	*@brief 清空玩家
	*/
	void ClearUSPlayer();

	/**
	*@brief 返回所有玩家
	*/
	USPlayerVec& GetAllUSPlayer();

	/**
	*@brief 返回所有玩家
	*/
	USPlayerVec GetSuitUSPlayer(std::function<bool (USPlayer* )> suit);

	/**
	*@brief 广播消息
	*/
	void BroadcastToPlayer(Avalon::Packet* packet);
	void BroadcastToPlayer(Avalon::Protocol& protocol);

public:
	void RegisterProtocols();
	void ExecuteProtocol(const CLProtocol::SysTransmitPlayerToUnion& protocol);

private:
	//玩家消息包执行器
	CLParamPacketExecutor<USPlayer*>	m_Executor;

	//不需要具体对象的玩家消息包执行器
	CLParamPacketExecutor<RoleIDConnID>	m_IDExecutor;
	
	// 玩家管理
	USPlayerMap m_PlayerMap;
	USPlayerVec m_PlayerVec;
	
	//被持有的跨服服务
	UnionService& m_UnionService;
};

#endif
