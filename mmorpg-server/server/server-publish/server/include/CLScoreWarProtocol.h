#ifndef _CL_SCORE_WAR_PROTOCOL_H_
#define _CL_SCORE_WAR_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLScoreWarDefine.h"
#include <AvalonPacket.h>


namespace CLProtocol
{
	/**
	*@brief client->scene 请求积分赛奖励
	*/
	class SceneScoreWarRewardReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SCORE_WAR_REWARD_REQ)
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
	class SceneScoreWarRewardRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SCORE_WAR_REWARD_RES)
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
	class SceneSyncScoreWarInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_SCORE_WAR_INFO)
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
	class SceneCrossSyncScoreWarInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CROSS_SYNC_SCORE_WAR_INFO)
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
	class SceneCrossSyncScoreWarInfoRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CROSS_SYNC_SCORE_WAR_INFO_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}
	public:
		UInt32 result;
	};

	/**
	*@brief scene->cross 同步GM命令
	*/
	class SceneCrossGMSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CROSS_GM_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isByTime & targetStatus;
		}
	public:
		UInt8 isByTime;
		UInt8 targetStatus;
	};


	/**
	*@brief world->cross 请求排行榜信息
	*/
	class CrossScoreWarSortListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_SCORE_WAR_SORT_LIST_REQ)
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
	class CrossScoreWarSortListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_SCORE_WAR_SORT_LIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & (*packet);
		}

		CrossScoreWarSortListRes() { packet = Avalon::Packet::Create(); }
		CrossScoreWarSortListRes(Avalon::Packet* pack){ packet = pack; }
		~CrossScoreWarSortListRes() { Avalon::Packet::Destroy(packet); }

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
	class CrossScoreWarInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_SCORE_WAR_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	};
	

	/**
	*@brief world->cross 同步排行榜信息
	*/
	class CrossScoreWarSortListSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_SCORE_WAR_SORT_LIST_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & entry;
		}

		ScoreWarSortListEntry entry;
	};

	/**
	*@brief cross->world 积分赛排行榜奖励
	*/
	class WorldScoreWarRankingReward : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SCORE_WAR_RANKING_REWARD)
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
	class WorldScoreWarSyncBattleInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SCORE_WAR_SYNC_BATTLE_INFO)
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
	class CrossScoreWarBattleInfoSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_SCORE_WAR_BATTLE_INFO_SYNC)
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

}



#endif //_CL_SCORE_WAR_PROTOCOL_H_

