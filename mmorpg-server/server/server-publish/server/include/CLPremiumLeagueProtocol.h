#ifndef __CL_PREMIUM_LEAGUE_PROTOCOL_H__
#define __CL_PREMIUM_LEAGUE_PROTOCOL_H__

#include <AvalonProtocol.h>
#include "CLProtocolDefine.h"
#include "CLPremiumLeagueDefine.h"

namespace CLProtocol
{
	/**
	*@brief world->client 同步赏金联赛状态
	*/
	class WorldPremiumLeagueSyncStatus : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PREMIUM_LEAGUE_SYNC_STATUS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

	public:
		PremiumLeagueStatusInfo		info;
	};

	/**
	*@brief client->world 请求赏金联赛奖金池信息
	*/
	class WorldPremiumLeagueRewardPoolReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PREMIUM_LEAGUE_REWARD_POOL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world->client 返回赏金联赛奖金
	*/
	class WorldPremiumLeagueRewardPoolRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PREMIUM_LEAGUE_REWARD_POOL_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & enrollPlayerNum & money;
			stream.SeriaArray(rewards, PL_REWARD_NUM);
		}

	public:
		// 报名人数
		UInt32			enrollPlayerNum;
		// 总奖金池
		UInt32			money;
		// 各排名获得的奖励
		UInt32			rewards[PL_REWARD_NUM];
	};

	/**
	*@brief client->world 赏金联赛报名请求
	*/
	class WorldPremiumLeagueEnrollReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PREMIUM_LEAGUE_ENROLL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world->client 赏金联赛报名返回
	*/
	class WorldPremiumLeagueEnrollRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PREMIUM_LEAGUE_ENROLL_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

	public:
		UInt32			result;
	};

	/**
	*@brief world->client 初始化淘汰赛玩家列表
	*/
	class WorldPremiumLeagueBattleGamerInit : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PREMIUM_LEAGUE_BATTLE_GAMER_INIT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & gamers;
		}

	public:
		std::vector<PremiumLeagueBattleGamer>			gamers;
	};

	/**
	*@brief world->client 更新淘汰赛玩家信息
	*/
	class WorldPremiumLeagueBattleGamerUpdate : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PREMIUM_LEAGUE_BATTLE_GAMER_UPDATE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & winNum & isLose;
		}

	public:
		ObjID_t			roleId;
		UInt32			winNum;
		UInt8			isLose;
	};

	/**
	*@brief world->client 玩家自己的赏金联赛信息
	*/
	class WorldPremiumLeagueSelfInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PREMIUM_LEAGUE_SELF_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

	public:
		PremiumLeagueSelfInfo	info;
	};

	/**
	*@brief client->world 请求赏金联赛战斗记录
	*/
	class WorldPremiumLeagueBattleRecordReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PREMIUM_LEAGUE_BTTTLE_RECORD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isSelf & startIndex & count;
		}

	public:
		UInt8 isSelf;
		Int32 startIndex;
		UInt32 count;
	};

	/**
	*@brief world->client 返回赏金联赛战斗记录
	*/
	class WorldPremiumLeagueBattleRecordRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PREMIUM_LEAGUE_BTTTLE_RECORD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & totalCount & records;
		}

	public:
		UInt32 totalCount;
		std::vector<PremiumLeagueRecordEntry> records;
	};

	/**
	*@brief world->client 同步赏金联赛战斗记录
	*/
	class WorldPremiumLeagueBattleRecordSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PREMIUM_LEAGUE_BTTTLE_RECORD_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & record;
		}

	public:
		PremiumLeagueRecordEntry record;
	};

	/**
	*@brief world->client 赏金联赛结算
	*/
	class WorldPremiumLeagueRaceEnd : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PREMIUM_LEAGUE_RACE_END)
	public:
		WorldPremiumLeagueRaceEnd() : isPreliminay(0), result(0), oldScore(0), newScore(0), preliminayRewardNum(0), getHonor(0){}
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isPreliminay & result & oldScore & newScore & preliminayRewardNum & getHonor;
		}

	public:
		// 是不是预选赛
		UInt8	isPreliminay;
		// 战斗结果
		UInt8	result;
		UInt32	oldScore;
		UInt32	newScore;
		// 奖励数量
		UInt32  preliminayRewardNum;
		//获得荣誉
		UInt32  getHonor;
	};

	/**
	*@brief world->client 初始化淘汰赛对战列表
	*/
	class WorldPremiumLeagueBattleInfoInit : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PREMIUM_LEAGUE_BATTLE_INFO_INIT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battles;
		}

	public:
		std::vector<CLPremiumLeagueBattle>	battles;
	};

	/**
	*@brief world->client 更新淘汰赛对战信息
	*/
	class WorldPremiumLeagueBattleInfoUpdate : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PREMIUM_LEAGUE_BATTLE_INFO_UPDATE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battle;
		}

	public:
		CLPremiumLeagueBattle	battle;
	};

	/**
	*@brief world->scene 赏金联赛结算到scene
	*/
	class WorldPremiumLeagueEndToScene : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PREMIUM_LEAGUE_END_TO_SCENE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & pvpType & honor & reason;
		}

		UInt64 roleId;
		UInt32 pvpType;
		UInt32 honor;
		std::string reason;
	};
};


#endif