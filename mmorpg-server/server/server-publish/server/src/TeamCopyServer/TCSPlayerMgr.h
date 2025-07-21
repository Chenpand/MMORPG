#ifndef _TCS_PLAYER_MGR_H_
#define _TCS_PLAYER_MGR_H_

#include <AvalonSingleton.h>
#include <CLPacketExecutor.h>
#include <AvalonSimpleTimer.h>

#include <CLDefine.h>
#include "TCSPlayer.h"

namespace CLProtocol
{
	class SysTransmitPlayerToTeamCopy;
}
/**
*@brief 团本玩家管理器
*/
class TCSPlayerMgr : public Avalon::Singleton<TCSPlayerMgr>
{
	typedef HashMap<UInt64, TCSPlayer*> TCSPlayerMap;
	typedef std::vector<TCSPlayer*> TCSPlayerVec;

public:
	TCSPlayerMgr();
	~TCSPlayerMgr();

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
	TCSPlayer* FindTCSPlayer(UInt64 id);

	/**
	*@brief 随机获得一个玩家
	*/
	TCSPlayer* RandTCSPlayer();
	void RandTCSPlayer(UInt32 num, std::vector<TCSPlayer*>& playerList, UInt32 param, bool isTeam = false);

	/**
	*@brief 增加玩家
	*/
	void AddTCSPlayer(TCSPlayer* tcsPlayer);

	/**
	*@brief 删除玩家
	*/
	void RemoveTCSPlayer(UInt64 playerId);

	/**
	*@brief 加入过期列表
	*/
	void AddExpire(UInt64 playerId, UInt64 leaveTime);

	/**
	*@brief 从过期列表删除
	*/
	void DelExpire(UInt64 playerId);

	/**
	*@brief 玩家是否离开
	*/
	bool PlayerIsExpire(UInt64 playerId);

	/**
	*@brief 玩家过期时间
	*/
	UInt64 PlayerExpireTime(UInt64 playerId);

	/**
	*@brief 玩家离线时间
	*/
	UInt64 PlayerLeaveTime(UInt64 playerId);

	/**
	*@brief 广播消息
	*/
	void BroadcastToPlayer(Avalon::Packet* packet);
	void BroadcastToPlayer(Avalon::Protocol& protocol);

public:
	void RegisterProtocols();
	void ExecuteProtocol(const CLProtocol::SysTransmitPlayerToTeamCopy& protocol);

private:
	//玩家消息包执行器
	CLParamPacketExecutor<TCSPlayer*>	m_Executor;
	
	// 玩家管理
	TCSPlayerMap m_PlayerMap;
	TCSPlayerVec m_PlayerVec;
	
	// 玩家过期时间
	std::map<UInt64, UInt64> m_PlayerExpireMap;

	Avalon::SimpleTimer m_60sTimer;
};

#endif
