#ifndef _CL_SCORE_WAR_PROTOCOL_H_
#define _CL_SCORE_WAR_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLScoreWarDefine.h"
#include <AvalonPacket.h>


namespace CLProtocol
{
	/**
	*@brief client->scene �������������
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
	*@brief scene->client ���������������Ӧ
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
	*@brief server->client ͬ����������Ϣ
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
	*@brief cross->scene ͬ����������Ϣ
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
	*@brief scene->cross ͬ����������Ϣ�ذ�
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
	*@brief scene->cross ͬ��GM����
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
	*@brief world->cross �������а���Ϣ
	*/
	class CrossScoreWarSortListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_SCORE_WAR_SORT_LIST_REQ)
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
	*@brief world->cross ���������״̬
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
	*@brief world->cross ͬ�����а���Ϣ
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
	*@brief cross->world ���������а���
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
	*@brief scene->world ���������а���
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
	*@brief world->cross ͬ�����а���Ϣ
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

