#ifndef _CL_RL_PLAYERMGR_H_
#define _CL_RL_PLAYERMGR_H_

#include <AvalonSingleton.h>
#include <CLPacketExecutor.h>
#include <CLDefine.h>
#include <AvalonSimpleTimer.h>
#include <set>
#include "RLPlayer.h"

class RLPlayer;

namespace Avalon
{
	class PropertySet;
}

class RLPlayerMgr : public Avalon::Singleton<RLPlayerMgr>
{
public:
	typedef HashMap<ObjID_t, RLPlayer*> RLPlayerMap;
	typedef std::set<RLPlayer*> RLPlayerSet;

	enum
	{
		relaysvr_max_players		=	2000,			//最大支撑人数
		relaysvr_players_reserve	=	50,
	};
public:
	RLPlayerMgr();
	~RLPlayerMgr();

	bool Init(Avalon::PropertySet* prop);

	void OnTick(const Avalon::Time& now);

	void OnQuit();

	void ExecutePacket(RLPlayer* player, Avalon::Packet* packet);

	//void RegisterProtocols();

	//
	void FreePlayer(RLPlayer* player);
	//

	void VisitPlayers(RLPlayerVisitor& visitor);

public:
	//
	//void RegisterProtocols();
public:
	//
	RLPlayer*	find_player(ObjID_t roleId);
	RLPlayer*	findPlayerByAccount(UInt32 accid);
	bool		add_player(RLPlayer* player);
	bool		remove_player(RLPlayer* player);
	bool		remove_player(ObjID_t roleId);

	UInt32		get_player_count();
	UInt32		get_ablity();
	void		set_max_player_num(UInt32 num);

protected:
	void DeallocPlayer();
protected:

	CLParamPacketExecutor<RLPlayer*> m_Executor;
private:
	RLPlayerMap m_stPlayerMap;
	RLPlayerSet m_stWaitFreePlayerSet;

	UInt32 m_unMaxPlayer;		//最大玩家数量

	// 上报负载定时器
	Avalon::SimpleTimer m_abalityTimers;
};

#endif
