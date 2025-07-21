#ifndef _CL_2V2_SCORE_WAR_PROTOCOL_H_
#define _CL_2V2_SCORE_WAR_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CL2V2ScoreWarDefine.h"
#include <AvalonPacket.h>


namespace CLProtocol
{
	/**
	*@brief client->scene �������������
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
	*@brief scene->client ���������������Ӧ
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
	*@brief server->client ͬ����������Ϣ
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
	*@brief cross->scene ͬ����������Ϣ
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
	*@brief scene->cross ͬ����������Ϣ�ذ�
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
	*@brief world->cross �������а���Ϣ
	*/
	class Cross2V2ScoreWarSortListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_2V2_SCORE_WAR_SORT_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & type & start & num;
		}
		//���ID
		ObjID_t playerId;
		//���а�����
		UInt8 type;
		//��ʼλ�� 0��ʼ
		UInt16 start;
		//����
		UInt16 num;
	};

	/**
	*@brief cross->world �������а���Ϣ
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
	*@brief world->cross ���������״̬
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
	*@brief world->cross ͬ�����а���Ϣ
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
	*@brief cross->world ���������а���
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
	*@brief scene->world ���������а���
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
	*@brief world->cross ͬ�����а���Ϣ
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
	*@brief scene->cross ͬ��GM����
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

