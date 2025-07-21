#ifndef __CHAMPION_PROTOCOL_H__
#define __CHAMPION_PROTOCOL_H__


#include "CLProtocolDefine.h"
#include <AvalonPacket.h>
#include <CLChampionDefine.h>

namespace CLProtocol
{
	/**
	*@brief Union->Scene 同步冠军赛状态
	*/
	class SceneReceiveSyncChampionStatus : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RECEIVE_SYNC_CHAMPION_STATUS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & status;
		}

	public:
		ChampionStatusInfo status;
	};


	/**
	*@brief Client->Scene 冠军赛报名
	*/
	class SceneChampionEnrollReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_ENROLL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	};

	/**
	*@brief Scene ->Client 冠军赛报名
	*/
	class SceneChampionEnrollRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_ENROLL_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & errorCode;
		}
		UInt32 errorCode;
	};

	/**
	*@brief Scene->Union 冠军赛报名检查
	*/
	class UnionCheckChampionEnrollReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(UNION_CHECK_CHAMPION_ENROLL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & accid;
		}
		UnionCheckChampionEnrollReq() = default;
		UnionCheckChampionEnrollReq(UInt64 roleId, UInt32 accid) : roleId(roleId),accid(accid) {}
	public:
		UInt64 roleId;
		UInt32 accid;
	};

	/**
	*@brief Union->Scene 冠军赛报名检查返回
	*/
	class UnionCheckChampionEnrollRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(UNION_CHECK_CHAMPION_ENROLL_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream  & errorCode & roleId;
		}
	public:
		UInt32 errorCode;
		UInt64 roleId;
	};

	/**
	*@brief Clinet->Scene 请求进入比赛准备区域
	*/
	class SceneChampionJoinPrepareReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_JOIN_PREPARE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	public:

	};

	/**
	*@brief Scene->Client 请求进入比赛准备区域返回
	*/
	class SceneChampionJoinPrepareRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_JOIN_PREPARE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & errorCode;
		}
	public:
		UInt32 errorCode;
	};
	
	/**
	*@brief Scene->Union 请求进入比赛准备区域
	*/
	class UnionChampionJoinPrepareReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(UNION_CHAMPION_JOIN_PREPARE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sceneNode & worldNode & accId & roleId & name & occu & level & vipLvl & guildId & awaken & dstSceneId & dstDoorId & avatar & server;
		}
	public:
		UInt32 sceneNode = 0;
		UInt32 worldNode = 0;
		UInt32 accId = 0;
		ObjID_t roleId = 0;
		std::string name;
		UInt8 occu = 0;
		UInt16 level = 0;
		UInt8 vipLvl = 0;
		UInt64 guildId = 0;
		UInt8 awaken = 0;
		UInt32 dstSceneId = 0;
		UInt32 dstDoorId = 0;
		PlayerAvatar avatar;
		std::string server;
	};

	/**
	*@brief Union->Scene 请求进入比赛准备区域返回
	*/
	class UnionChampionJoinPrepareRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(UNION_CHAMPION_JOIN_PREPARE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & errorCode & dstSceneId & dstDoorId;
		}
	public:
		UInt64 roleId;
		UInt32 errorCode;
		UInt32 dstSceneId;
		UInt32 dstDoorId;
	};

	/**
	*@brief Clinet->Scene 请求复活
	*/
	class SceneChampionReliveReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_RELIVE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	public:

	};

	/**
	*@brief Scene->Client 请求复活返回
	*/
	class SceneChampionReliveRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_RELIVE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & errorCode;
		}
	public:
		UInt32 errorCode;
	};

	/**
	*@brief Scene->Union 请求检查复活
	*/
	class UnionChampionReliveReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(UNION_CHAMPION_CHECK_RELIVE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	public:

	};

	/**
	*@brief Union->Scene 请求检查复活返回
	*/
	class UnionChampionReliveRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(UNION_CHAMPION_CHECK_RELIVE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & errorCode;
		}
	public:
		UInt32 errorCode;
	};



	/**
	*@brief Clinet->Scene 请求16强表
	*/
	class SceneChampion16TableReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_16_TABLE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	public:

	};

	/**
	*@brief Scene->Client 请求16强表返回
	*/
	class SceneChampion16TableRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_16_TABLE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerVec;
		}
	public:
		std::vector<ChampionTop16Player> playerVec;
	};


	/**
	*@brief Union->Scene 同步16强表
	*/
	class SceneChampion16TableSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_16_TABLE_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerVec;
		}
	public:
		std::vector<ChampionTop16Player> playerVec;
	};


	/**
	*@brief Clinet->Scene 冠军赛押注
	*/
	class SceneChampionGambleReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_GAMBLE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleID & id & option & coin & accid;
		}
	public:
		UInt64 roleID;
		UInt32 id;
		UInt64 option;
		UInt32 coin;
		UInt32 accid;
	};


	/**
	*@brief Scene->Client 冠军赛押注返回
	*/
	class SceneChampionGambleRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_GAMBLE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & option & errorCode & num;
		}
	public:
		UInt32 id;
		UInt64 option;
		UInt32 errorCode;
		UInt32 num;
	};

	/**
	*@brief Union->Scene 冠军赛押注返回
	*/
	class SceneChampionGambleUnionRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_GAMBLE_UNION_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleID & res;
		}
	public:
		UInt64 roleID;
		SceneChampionGambleRes res;
	};


	/**
	*@brief Union->Scene 冠军赛竞猜信息同步
	*/
	class SceneChampionGambleInfoSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_GAMBLE_INFO_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & infoVec;
		}
	public:
		std::vector<ChampionGambleInfo> infoVec; 
	};



	class SceneChampionStatusReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_STATUS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	public:

	};

	class SceneChampionStatusRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_STATUS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & status;
		}
	public:
		ChampionStatusInfo status;
	};

	class SceneChampionSelfStatusReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_SELF_STATUS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	public:

	};

	class SceneChampionSelfStatusRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_SELF_STATUS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & status;
		}
	public:
		UInt32 status;
	};


	class SceneChampionSelfBattleRecordReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_SELF_BATTLE_RECORD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	public:

	};

	class SceneChampionSelfBattleRecordRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_SELF_BATTLE_RECORD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & win & lose & relive;
		}
	public:
		UInt32 win;
		UInt32 lose;
		UInt8 relive;
	};

	class SceneChampionGroupRankReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_GROUP_RANK_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	public:

	};



	class SceneChampionGroupRankRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_GROUP_RANK_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & rankList;
		}
	public:
		std::vector<RankRole> rankList;
	};

	class SceneChampionGroupRecordReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_GROUP_RECORD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & groupID;
		}
	public:
		UInt32 groupID;
	};

	class SceneChampionGroupRecordRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_GROUP_RECORD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & groupID & records;
		}
	public:
		UInt32 groupID;
		std::vector<ChampionBattleRecord> records;
	};

	class SceneChampionTotalStatusReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_TOTAL_STATUS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accid;
		}
	public:
		UInt32 accid;
	};

	class SceneChampionTotalStatusRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_TOTAL_STATUS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & status & slefStatus & roleID;
		}
	public:
		ChampionStatusInfo status;
		UInt32 slefStatus;
		UInt64 roleID;
	};

	class UnionChampionPlayerStatusReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(UNION_CHAMPION_PLAYER_STATUS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleID & param;
		}
	public:
		UInt64 roleID;
		UInt64 param;
	};

	class UnionChampionPlayerStatusRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(UNION_CHAMPION_PLAYER_STATUS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleID & param & status & win & lose;
		}
	public:
		UInt64 roleID;
		UInt64 param;
		UInt8 status;
		UInt32 win;
		UInt32 lose;
	};

	
	class UnionChampionBattleCountdownSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(UNION_CHAMPION_BATTLE_COUNTDOWN_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & endTime;
		}
	public:
		UInt32 endTime;
	};


	class UnionChampionScoreRankTopSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(UNION_CHAMPION_SCORE_RANK_TOP_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & rankList;
		}
	public:
		std::vector<RankRole> rankList;
	};

	class UnionChampionGroupIDSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(UNION_CHAMPION_GROUPID_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}
	public:
		UInt32 id;
	};

	class UnionChampionKnockoutScore : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(UNION_CHAMPION_KNOCKOUT_SCORE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roles;
		}
	public:
		std::vector<ChampionKnockoutPlayer> roles;
	};

	class SceneChampionScoreBattleRecordsReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_SCORE_BATTLE_RECORDS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	public:

	};

	class SceneChampionScoreBattleRecordsRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_SCORE_BATTLER_ECORDS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & records;
		}
	public:
		std::vector<ChampionScoreBattleRecord> records;
	};

	class SceneChampionGroupStatusSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_GROUP_STATUS_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & group;
		}
	public:
		ChampionGroupStatus group;
	};

	class SceneChampionAllGroupReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_ALL_GROUP_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	public:

	};

	class SceneChampionAllGroupRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_ALL_GROUP_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & groups;
		}
	public:
		std::vector<ChampionGroupStatus> groups;
	};

	/**
	 *@brief Union->Scene同步所有比赛信息
	 *		
	 */
	class SceneChampionAllRaceInfoSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_ALL_RACE_INFO_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};


	class SceneChampionAllGambleInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_ALL_GAMBLE_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	public:
	};

	class SceneChampionAllGambleInfoRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_ALL_GAMBLE_INFO_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & infos;
		}
	public:
		std::vector<ChampionGambleInfo> infos;
	};


	class SceneChampionGambleRecordReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_GAMBLE_RECORD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	public:
	};

	class SceneChampionGambleRecordRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_GAMBLE_RECORD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & infos;
		}
	public:
		std::vector<ChampionGambleRecord> infos;
	};


	class UnionChampionNullTrunNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(UNION_CHAMPION_NULL_TRUN_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	public:

	};	
	
	class SceneChampionGmableAlreadyBetReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_GEMBLE_ALREADY_BET_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & option;
		}
	public:
		UInt32 id;
		UInt64 option;
	};

	class SceneChampionGmableAlreadyBetRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAMPION_GEMBLE_ALREADY_BET_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & option & num;
		}
	public:
		UInt32 id;
		UInt64 option;
		UInt64 num;
	};
}




#endif // 
