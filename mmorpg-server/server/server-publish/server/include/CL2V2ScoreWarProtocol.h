#ifndef _CL_2V2_SCORE_WAR_PROTOCOL_H_
#define _CL_2V2_SCORE_WAR_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CL2V2ScoreWarDefine.h"
#include <AvalonPacket.h>


namespace CLProtocol
{
	/**
	*@brief client->scene 请求积分赛奖励
	*/
	class Scene2V2ScoreWarRewardReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_2V2_SCORE_WAR_REWARD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & rewardId;
		}

	public:
		UInt8 rewardId;
	};

	/**
	*@brief scene->client 请求积分赛奖励响应
	*/
	class Scene2V2coreWarRewardRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_2V2_SCORE_WAR_REWARD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

	public:
		UInt32 result;
	};

	/**
	*@brief server->client 同步积分赛信息
	*/
	class Scene2V2SyncScoreWarInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_2V2_SYNC_SCORE_WAR_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & status & statusEndTime;
		}
	public:
		UInt8 status;
		UInt32 statusEndTime;
	};

	/**
	*@brief cross->scene 同步积分赛信息
	*/
	class Scene2V2CrossSyncScoreWarInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_2V2_CROSS_SYNC_SCORE_WAR_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & status & statusEndTime & startTime;
		}
	public:
		UInt8 status;
		UInt32 statusEndTime;
		UInt32 startTime;
	};

	/**
	*@brief scene->cross 同步积分赛信息回包
	*/
	class Scene2V2CrossSyncScoreWarInfoRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_2V2_CROSS_SYNC_SCORE_WAR_INFO_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}
	public:
		UInt32 result;
	};

	/**
	*@brief world->cross 请求排行榜信息
	*/
	class Cross2V2ScoreWarSortListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_2V2_SCORE_WAR_SORT_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & type & start & num;
		}
		//玩家ID
		ObjID_t playerId;
		//排行榜类型
		UInt8 type;
		//开始位置 0开始
		UInt16 start;
		//数量
		UInt16 num;
	};

	/**
	*@brief cross->world 请求排行榜信息
	*/
	class Cross2V2ScoreWarSortListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_2V2_SCORE_WAR_SORT_LIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & (*packet);
		}

		Cross2V2ScoreWarSortListRes() { packet = Avalon::Packet::Create(); }
		Cross2V2ScoreWarSortListRes(Avalon::Packet* pack){ packet = pack; }
		~Cross2V2ScoreWarSortListRes() { Avalon::Packet::Destroy(packet); }

		void Detach()
		{
			packet = NULL;
		}

		ObjID_t playerId;
		Avalon::Packet* packet;
	};

	/**
	*@brief world->cross 请求积分赛状态
	*/
	class Cross2V2ScoreWarInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_2V2_SCORE_WAR_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	};
	

	/**
	*@brief world->cross 同步排行榜信息
	*/
	class Cross2V2ScoreWarSortListSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_2V2_SCORE_WAR_SORT_LIST_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & entry;
		}

		ScoreWar2V2SortListEntry entry;
	};

	/**
	*@brief cross->world 积分赛排行榜奖励
	*/
	class World2V2ScoreWarRankingReward : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_2V2_SCORE_WAR_RANKING_REWARD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & ranking;
		}

		ObjID_t playerId;
		UInt32 ranking;
	};

	/**
	*@brief scene->world 积分赛排行榜奖励
	*/
	class World2V2ScoreWarSyncBattleInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_2V2_SCORE_WAR_SYNC_BATTLE_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & loseStreak & winStreak & matchScore & battleCount;
		}

		ObjID_t playerId;
		UInt32 loseStreak;
		UInt32 winStreak;
		UInt32 matchScore;
		UInt32 battleCount;
	};

	/**
	*@brief world->cross 同步排行榜信息
	*/
	class Cross2V2ScoreWarBattleInfoSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_2V2_SCORE_WAR_BATTLE_INFO_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & roomId & loseStreak & winStreak & matchScore & battleCount;
		}

		ObjID_t playerId;
		UInt32 roomId;
		UInt32 loseStreak;
		UInt32 winStreak;
		UInt32 matchScore;
		UInt32 battleCount;
	};

	/**
	*@brief scene->cross 同步GM命令
	*/
	class SceneCross2V2GMSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CROSS_2V2_GM_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isByTime & targetStatus;
		}
	public:
		UInt8 isByTime;
		UInt8 targetStatus;
	};

}



#endif //_CL_SCORE_WAR_PROTOCOL_H_

