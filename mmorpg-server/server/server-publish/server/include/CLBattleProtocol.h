#ifndef _CL_BATTLE_PROTOCOL_H_
#define _CL_BATTLE_PROTOCOL_H_

#include <AvalonSocket.h>
#include "CLProtocolDefine.h"
#include "CLItemDefine.h"
#include "CLSkillDefine.h"

namespace CLProtocol
{

	/**
	*@brief scene->bscene 请求进入战场场景
	*/
	class BattleEnterSceneReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_ENTER_SCENE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief bscene->scene 进入战场场景返回
	*/
	class BattleEnterSceneRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_ENTER_SCENE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & result & battleSceneNodeId & battleId & battleType;
		}

		ObjID_t roleId;
		UInt32 result;
		UInt32 battleSceneNodeId;
		UInt32 battleId;
		UInt8	battleType;
	};

	/**
	*@brief bscene->scene 退出战场场景
	*/
	class BattleExitSceneReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_EXIT_SCENE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & battleType & result & battleId & floor & battleDead;
		}

		BattleExitSceneReq()
		{
			roleId = 0;
			battleType = 0;
			result = 0;
			battleId = 0;
			floor = 0;
			battleDead = 0;
		}

		ObjID_t roleId;
		UInt8	battleType;

		UInt8   result;
		UInt32	battleId;
		UInt32	floor;
		UInt8	battleDead;
	};

	/**
	*@brief scene->bscene 退出战场场景
	*/
	class BattleExitSceneRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_EXIT_SCENE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & result & battleType & battleResult & battleId & revive & reviveItem;
		}

		BattleExitSceneRes()
		{
			revive = 0;
		}

		ObjID_t roleId;
		UInt32 result;

		UInt8	battleType;
		UInt8   battleResult;
		UInt32	battleId;
		UInt8   revive; //(复活到战场)
		UInt32	reviveItem; //复活道具id
	};

	/**
	*@brief client->battle 战场PK请求
	*/
	class BattlePkSomeOneReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_PK_SOMEONE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & dstId & battleID & dungeonID;
		}

		ObjID_t roleId;
		ObjID_t dstId;
		UInt32 battleID;
		UInt32 dungeonID;
	};

	/**
	*@brief battle->client 战场PK返回
	*/
	class BattlePkSomeOneRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_PK_SOMEONE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & roleId & dstId;
		}

		UInt32 retCode;
		ObjID_t roleId;
		ObjID_t dstId;
	};

	/**
	*@brief client->battle 吃鸡锁定目标请求
	*/
	class BattleLockSomeOneReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_LOCK_SOMEONE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & dstId & battleID;
		}

		ObjID_t roleId;
		ObjID_t dstId;
		UInt32 battleID;
	};

	/**
	*@brief battle->client 吃鸡锁定目标返回
	*/
	class BattleLockSomeOneRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_LOCK_SOMEONE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & roleId & dstId;
		}

		UInt32 retCode;
		ObjID_t roleId;
		ObjID_t dstId;
	};

	/**
	*@brief battle->client 吃鸡结算
	*/
	class SceneBattleBalanceEnd : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_BALANCE_END)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & rank & playerNum & kills & survivalTime & score & battleID & getHonor;
		}

	public:
		UInt64 roleId;
		UInt32 rank;
		UInt32 playerNum;
		UInt32 kills;
		UInt32 survivalTime;
		UInt32 score;
		UInt32 battleID;
		UInt32 getHonor;
	};

	/**
	*@brief client->battle 战场报名请求
	*/
	class BattleEnrollReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_ENROLL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isMatch & accId & roleId & playerName & playerOccu;
		}

		UInt32 isMatch;
		UInt32 accId;
		ObjID_t roleId;
		std::string playerName;
		UInt8 playerOccu;
	};

	/**
	*@brief battle->client 战场报名返回
	*/
	class BattleEnrollRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_ENROLL_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isMatch & retCode;
		}

		// 0取消报名，非0报名
		UInt32 isMatch;
		UInt32 retCode;
	};

	/**
	*@brief client->bscene 拾取场景道具请求
	*/
	class SceneBattlePickUpItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_PICK_UP_ITEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemid & useAll & param1 & param2 & battleID;
		}

	public:
		//道具guid
		ObjID_t	itemid;
		//是否一键使用全部
		UInt8 useAll;
		//预留参数1
		UInt32 param1;
		//预留参数2
		UInt32 param2;
		//
		UInt32 battleID;
	};

	// 拾取场景道具返回
	class SceneBattlePickUpItemRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_PICK_UP_ITEM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & errcode;
		}

	public:
		UInt32 errcode;
	};

	class SceneBattleChangeSkillsReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_USE_SKILL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & configType & changeSkills;
		}

	public:

		// 技能配置类型
		UInt8 configType;

		//变化的技能列表
		std::vector<ChangeSkill> changeSkills;
	};

	class SceneBattleChangeSkillsRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_USE_SKILL_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

	public:
		UInt32	result;
	};

	struct PlayerSubjectInfo
	{
		PlayerSubjectInfo()
		{
			m_AccId = 0;
			m_PlayerId = 0;
			m_PlayerOccu = 0;
		}

		~PlayerSubjectInfo()
		{
			
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & m_AccId & m_PlayerId & m_PlayerName & m_PlayerOccu;
		}

		//账号ID
		UInt32 m_AccId;
		//玩家ID
		UInt64 m_PlayerId;
		//玩家名字
		std::string m_PlayerName;
		//玩家职业
		UInt8 m_PlayerOccu;
	};

	class SceneBattleMatchSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_MATCH_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID & suvivalNum & players & sceneNodeID;
		}

	public:
		UInt32 battleID;
		UInt32 suvivalNum;
		std::vector<PlayerSubjectInfo> players;
		UInt32 sceneNodeID;
	};

	class SceneBattleCreateBattleReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_CREATE_BATTLE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleType & battleID & playerIDs & sceneId & battleDataId & lostDungeonFloor;
		}

	public:
		UInt32				battleType;
		UInt32				battleID;
		std::vector<UInt64> playerIDs;
		UInt32				sceneId;
		UInt32				battleDataId;
		UInt32				lostDungeonFloor;
	};

	class SceneBattleCreateBattleRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_CREATE_BATTLE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleType & battleID & sceneId & result;
		}

	public:
		UInt32				battleType;
		UInt32				battleID;
		UInt32				sceneId;
		UInt32				result;
	};

	class SceneBattleDestroyBattle : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_DESTROY_BATTLE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleType & battleID & reason;
		}

	public:
		UInt32		battleType;
		UInt32		battleID;
		UInt8		reason;
	};

	class BattleNotifyPrepareInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_NOTIFY_PREPARE_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerNum & totalNum;
		}

		UInt32 playerNum;
		UInt32 totalNum;
	};

	class BattleNotifyBattleInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_NOTIFY_BATTLE_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID & playerNum;
		}

	public:
		UInt32 battleID;
		UInt32 playerNum;
	};

	class BattleServerSpecifyPkRobot : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_SEPCIFY_PK_ROBOT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & hard & occu & ai;
		}

	public:
		// 难度
		UInt8		hard;
		// 职业
		UInt8		occu;
		// ai
		UInt8		ai;
	};

	class BattleServerAddPkRobot : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_ADD_PK_ROBOT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	class SceneBattleKillSomeOne : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_KILL_SOMEONE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID & playerID & killerID & remainHp & remainMp & otherID;
		}

	public:
		UInt32 battleID;
		UInt64 playerID;
		UInt64 killerID;
		// 胜者剩余血量
		Int32  remainHp;
		// 胜者剩余蓝量
		Int32  remainMp;
		//
		UInt64 otherID;
	};

	class SceneBattleSyncPoisonRing : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_SYNC_POISON_RING)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID & x & y & radius & beginTimestamp & interval & x1 & y1 & radius1;
		}

	public:
		UInt32  battleID;
		UInt32	x;
		UInt32	y;
		UInt32	radius;
		UInt32	beginTimestamp;
		UInt32	interval;
		UInt32	x1;
		UInt32	y1;
		UInt32	radius1;
	};

	class SceneBattlePickUpSpoilsReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_PICKUP_SPOILS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID & playerID & itemGuid;
		}

	public:
		UInt32 battleID;
		UInt64 playerID;
		UInt64 itemGuid;
	};

	class SceneBattlePickUpSpoilsRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_PICKUP_SPOILS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & itemGuid;
		}

	public:
		UInt32 retCode;
		UInt64 itemGuid;
	};

	class SceneBattleSelectOccuReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_SELECT_OCCU_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & occu;
		}

	public:
		UInt8 occu;
	};

	class SceneBattleSelectOccuRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_SELECT_OCCU_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode;
		}

	public:
		UInt32 retCode;
	};

	class SceneBattleNotifySpoilsItem : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_NOTIFY_SPOILS_ITEM)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID & playerID & detailItems;
		}

	public:
		UInt32 battleID;
		UInt64 playerID;
		//具体的道具
		std::vector<DetailItem>	detailItems;
	};

	/**
	*@brief client->scene 出生传送
	*/
	class SceneBattleBirthTransfer : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_BIRTH_TRANSFER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID & regionID;
		}

	public:
		UInt32 battleID;
		UInt32 regionID;
	};

	class SceneBattleBirthTransferNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_BIRTH_TRANSFER_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID & playerID & birthPosX & birthPosY;
		}

	public:
		UInt32 battleID;
		UInt64 playerID;
		UInt32 birthPosX;
		UInt32 birthPosY;
	};

	class SceneBattleNotifyWaveInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_NOTIFY_WAVE_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID & waveID;
		}

	public:
		UInt32 battleID;
		UInt32 waveID;
	};

	class SceneBattleNotifySomeoneDead : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_NOTIFY_SOMEONE_DEAD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID & playerID & killerID & reason & kills & suvivalNum;
		}

	public:
		UInt32 battleID;
		UInt64 playerID;
		UInt64 killerID;
		UInt32 reason;
		UInt32 kills;
		UInt32 suvivalNum;
	};

	class SceneBattleThrowSomeoneItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_THROW_SOMEONE_TIEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID & targetID & itemGuid;
		}

	public:
		UInt32 battleID;
		UInt64 targetID;
		UInt64 itemGuid;
	};

	class SceneBattleThrowSomeoneItemRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_THROW_SOMEONE_TIEM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & attackID & targetID & itemGuid & itemDataID & targetName & param;
		}

		SceneBattleThrowSomeoneItemRes() : param(0) {}
	public:
		UInt32 retCode;
		UInt64 attackID;
		UInt64 targetID;
		UInt64 itemGuid;
		UInt32 itemDataID;
		std::string targetName;
		UInt32 param;
	};

	/**
	*@brief world->battle 请求吃鸡排行榜
	*/
	class BattleSortListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_SORT_LIST_REQ)
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
	*@brief battle->scene 战场
	*/
	class SceneBattleEnd : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_END)
	public:
		SceneBattleEnd()
		{
			playerID = 0;
			score = 0;
			getHonor = 0;
		}
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerID & score & getHonor;
		}

	public:
		UInt64 playerID;
		UInt32 score;
		UInt32 getHonor;
	};

	/**
	*@brief client->scene 删除物品请求
	*/
	class SceneBattleDelItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_DEL_ITEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemGuid;
		}

	public:
		UInt64 itemGuid;
	};

	/**
	*@brief scene->client 删除物品返回
	*/
	class SceneBattleDelItemRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_DEL_ITEM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode;
		}

	public:
		UInt32 retCode;
	};

	/**
	*@brief client->scene 进入吃鸡战场
	*/
	class SceneBattleEnterBattleReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_ENTER_BATTLE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID;
		}

	public:
		UInt32 battleID;
	};

	class SceneBattleEnterBattleRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_ENTER_BATTLE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID & retCode;
		}

	public:
		UInt32 battleID;
		UInt32 retCode;
	};

	/**
	*@brief battle->scene 吃鸡结算日志
	*/
	class SceneBattleBalanceLog : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_BALANCE_LOG)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & rank & playerNum & kills & survivalTime & score & reason & roleId;
		}

	public:
		UInt32 rank;
		UInt32 playerNum;
		UInt32 kills;
		UInt32 survivalTime;
		UInt32 score;
		UInt32 reason;
		UInt64 roleId;
	};

	struct BattleNpc
	{
		BattleNpc() : npcGuid(0), npcId(0), opType(0) {}
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & npcGuid & npcId & opType & scenePos;
		}

		UInt64 npcGuid;
		// npcId
		UInt32 npcId;
		// 1刷出，0清除
		UInt32 opType;
		// 位置
		ScenePos scenePos;
	};

	/**
	*@brief scene->client 刷出NPC
	*/
	class SceneBattleNpcNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_NPC_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleNpcList;
		}

	public:

		std::vector<BattleNpc> battleNpcList;
	};

	/**
	*@brief client->scene 与NPC交易请求
	*/
	class SceneBattleNpcTradeReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_NPC_TRADE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & npcGuid & npcId & paramVec;
		}

	public:
		UInt64 npcGuid;
		UInt32 npcId;
		std::vector<UInt64> paramVec;
	};

	/**
	*@brief scene->client 与NPC交易返回
	*/
	class SceneBattleNpcTradeRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_NPC_TRADE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & npcId;
		}

	public:
		UInt32 retCode;
		UInt32 npcId;
	};

	/**
	*@brief client->scene 吃鸡通知踩中陷阱
	*/
	class SceneBattleNotifyBeTraped : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_NOTIFY_BE_TRAPED)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID & playerID & ownerID & x & y;
		}

	public:
		UInt32 battleID;
		UInt64 playerID;
		UInt64 ownerID;
		UInt32 x;
		UInt32 y;
	};

	/**
	*@brief client->scene 吃鸡放置陷阱请求
	*/
	class SceneBattlePlaceTrapsReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_PLACE_TRAPS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemGuid & itemCount & x & y;
		}

	public:
		UInt64 itemGuid;
		UInt32 itemCount;
		UInt32 x;
		UInt32 y;
	};

	/**
	*@brief scene->client 吃鸡放置陷阱返回
	*/
	class SceneBattlePlaceTrapsRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_PLACE_TRAPS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode;
		}

	public:
		UInt32 retCode;
	};

	/**
	*@brief client->scene 吃鸡开宝箱请求
	*/
	class SceneBattleOpenBoxReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_OPEN_BOX_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemGuid & param;
		}

	public:
		UInt64 itemGuid;
		// 参数 1：打开；2：取消；3：拾取
		UInt32 param;
	};

	/**
	*@brief scene->client 吃鸡开宝箱返回
	*/
	class SceneBattleOpenBoxRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_OPEN_BOX_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemGuid & param & retCode & openTime;
		}

	public:
		UInt64 itemGuid;
		UInt32 param;
		UInt32 retCode;
		UInt32 openTime;
	};

	/**
	*@brief battle->client 最佳排名通知
	*/
	class BattleNotifyBestRank : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_NOTIFY_BEST_RANK)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & rank;
		}

	public:
		UInt32 rank;
	};

	/**
	*@brief battle->scene 最佳排名奖励通知
	*/
	class BattleNotifyRankAward : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_NOTIFY_RANK_AWARD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & rank;
		}

	public:
		UInt64 playerId;
		UInt32 rank;
	};

	/**
	*@brief world->battle 吃鸡通知活动时间
	*/
	class BattleNotifyActivityInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_NOTIFY_ACTIVITY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & actName & preTime & startTime & endTime;
		}

	public:
		std::string actName;
		UInt32 preTime;
		UInt32 startTime;
		UInt32 endTime;
	};

	struct ChiJiSkill
	{
		ChiJiSkill() : skillId(0), skillLvl(0){}
		ChiJiSkill(UInt32 _id, UInt32 _lvl) : skillId(_id), skillLvl(_lvl) {}
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & skillId & skillLvl;
		}
		UInt32 skillId;	
		UInt32 skillLvl;
	};

	/**
	*@brief scene->client 下发技能选择列表
	*/
	class BattleSkillChoiceListNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_SKILL_CHOICE_LIST_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & chiJiSkillVec;
		}

	public:
		std::vector<ChiJiSkill> chiJiSkillVec;
	};

	/**
	*@brief client->scene 选择技能请求
	*/
	class BattleChoiceSkillReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_CHOICE_SKILL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & skillId;
		}

	public:
		UInt32 skillId;
	};

	/**
	*@brief scene->client 选择技能返回
	*/
	class BattleChoiceSkillRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_CHOICE_SKILL_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & skillId & skillLvl;
		}

	public:

		UInt32 retCode;
		UInt32 skillId;
		UInt32 skillLvl;
	};

	/**
	*@brief client->battle 吃鸡观战请求
	*/
	class BattleObserveReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_OBSERVE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & dstId & battleId;
		}

	public:
		UInt64 playerId;
		UInt64 dstId;
		UInt32 battleId;
	};

	/**
	*@brief battle->client 吃鸡观战返回
	*/
	class BattleObserveRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_OBSERVE_RES)
	public:

		BattleObserveRes()
		{
			retCode = 0;
			playerId = 0;
			dstId = 0;
			raceId = 0;
		}
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & playerId & dstId & raceId & addr;
		}

	public:
		UInt32 retCode;
		UInt64 playerId;
		UInt64 dstId;
		UInt64 raceId;
		Avalon::SockAddr addr;
	};

	/**
	*@brief battle->scene 吃鸡免战检查
	*/
	class BattleCheckNoWarReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_CHECK_NO_WAR_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & dstId & battleID & dungeonID;
		}

		ObjID_t roleId;
		ObjID_t dstId;
		UInt32 battleID;
		UInt32 dungeonID;
	};

	/**
	*@brief scene->battle 吃鸡免战检查返回
	*/
	class BattleCheckNoWarRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_CHECK_NO_WAR_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isNoWar & roleId & dstId & battleID & dungeonID;
		}

		UInt32 isNoWar;
		UInt64 roleId;
		UInt64 dstId;
		UInt32 battleID;
		UInt32 dungeonID;
	};

	/**
	*@brief battle->client 通知吃鸡杂项数据
	*/
	class BattleNotifyChijiMisc : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_NOTIFY_CHIJI_MISC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & moveIntervalMs;
		}

		// 移动包发送间隔(ms)
		UInt32 moveIntervalMs;
	};

	/**
	*@brief scene->client 免战选项
	*/
	class SceneBattleNoWarOption : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_NO_WAR_OPTION)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & enemyRoleId & enemyName;
		}

		UInt64 enemyRoleId;
		std::string enemyName;
	};

	/**
	*@brief client->scene 免战选择请求
	*/
	class SceneBattleNoWarChoiceReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_NO_WAR_CHOICE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isNoWar;
		}
		 
		UInt32 isNoWar;
	};

	/**
	*@brief scene->client 免战选择返回
	*/
	class SceneBattleNoWarChoiceRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_NO_WAR_CHOICE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode;
		}
		UInt32 retCode;
	};

	/**
	*@brief scene->client 免战通知
	*/
	class SceneBattleNoWarNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_NO_WAR_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	};

	/**
	*@brief scene->client 免战等待通知
	*/
	class SceneBattleNoWarWait : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_NO_WAR_WAIT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	};

	/**
	*@brief client->scene 职业列表请求
	*/
	class SceneBattleOccuListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_OCCU_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief scene->client 职业列表返回
	*/
	class SceneBattleOccuListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_OCCU_LIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & occuVec;
		}

		std::vector<UInt32> occuVec;
	};

	/**
	*@brief battle->bscene 吃鸡商店通知
	*/
	class BattleShopNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_SHOP_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleId & shopWave;
		}

		UInt32 battleId;
		UInt32 shopWave;
	};
	
	/**
	*@brief scene->client 下发装备选择列表
	*/
	class BattleEquipChoiceListNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_EQUIP_CHOICE_LIST_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & equipVec;
		}

	public:
		std::vector<UInt32> equipVec;
	};

	/**
	*@brief client->scene 选择装备请求
	*/
	class BattleChoiceEquipReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_CHOICE_EQUIP_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & equipId;
		}

	public:
		UInt32 equipId;
	};

	/**
	*@brief scene->client 选择装备返回
	*/
	class BattleChoiceEquipRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_CHOICE_EQUIP_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & equipId;
		}

	public:

		UInt32 retCode;
		UInt32 equipId;
	};

	/**
	*@brief battle->scene 吃鸡开关通知
	*/
	class BattleOpenNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_OPEN_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & openTime;
		}

	public:

		UInt32 openTime;
	};

}

#endif
