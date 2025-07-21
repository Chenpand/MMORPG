#ifndef _CL_LOST_DUNGEON_PROTOCOL_H_
#define _CL_LOST_DUNGEON_PROTOCOL_H_

#include <AvalonSocket.h>
#include <CLProtocolDefine.h>
#include <CLLostDungeonDefine.h>
#include <CLLostDungeonDefine.h>
#include <CLMatchDefine.h>
#include <CLObjectDefine.h>

namespace CLProtocol
{
	/**
	*@brief client->scene 挑战迷失地牢请求
	*/
	class LostDungeonChallengeReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_CHAGE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & floor & hardType;
		}

		//层数
		UInt32  floor;
		//战场难度类型(LostDungeonBattleHardType)
		UInt8	hardType;
	};

	/**
	*@brief battle->client 挑战迷失地牢返回
	*/
	class LostDungeonChallengeRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_CHAGE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & battleId & battleDataId & sceneId & floor & hardType;
		}

		//结果
		UInt32  code;
		//匹配的战场id
		UInt32  battleId;
		//匹配的战场的表id
		UInt32  battleDataId;
		//匹配的战场的场景id
		UInt32	sceneId;
		//楼层
		UInt32	floor;
		//战场难度类型(LostDungeonBattleHardType)
		UInt8	hardType;
	};

	/**
	*brief scene->battle 匹配战场
	*/
	class LostDungeonMathBattleReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_MATH_BATTLE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & srcSceneNodeId & floor & hardType;
		}

		//匹配的玩家
		UInt64	playerId;
		//源场景id
		UInt32  srcSceneNodeId;
		//层数
		UInt32  floor;
		//匹配的战场难度(枚举LostDungeonBattleHardType)
		UInt8	hardType;
	};

	/**
	*@brief client->scene 开启迷失地牢请求
	*/
	class LostDungeonOpenReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_OPEN_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}

	};

	/**
	*@brief scene->client 开启迷失地牢返回
	*/
	class LostDungeonOpenRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_OPEN_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & floorDatas;
		}

		//结果
		UInt32  code;
		//生成的楼层数据
		std::vector<LostDungeonFloorData>	floorDatas;
	};

	/**
	*@brief client->scene 地牢开宝箱请求 
	*/
	class LostDungeonOpenBoxReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_OPENBOX_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream  & floor & boxId;
		}
		UInt32	floor;
		//宝箱id
		UInt32  boxId;
	};

	/**
	*@brief scene->client 地牢开宝箱返回
	*/
	class LostDungeonOpenBoxRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_OPENBOX_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & itemVec;
		}
		//结果
		UInt32  code;
		//备选道具
		std::vector<ItemReward> itemVec;
	};

	/**
	*@brief scene->client 同步楼层数据 
	*/
	class LostDungeonSyncFloor : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_SYNC_FLOOR)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & floorDatas;
		}
		//楼层数据
		std::vector<LostDungeonFloorData>	floorDatas;
	};

	/**
	*@brief client->battleScene 迷失地牢战场PK请求
	*/
	class SceneLostDungeonPkReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_LOSTDUNGEON_PK_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & dstId & battleID & dungeonID;
		}
		ObjID_t dstId;
		UInt32 battleID;
		UInt32 dungeonID;
	};

	/**
	*@brief battle->client 战场PK返回
	*/
	class SceneLostDungeonPkRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_LOSTDUNGEON_PK_RES)
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
	*@brief scene->battle 请求PK
	*/
	class BattleLostDungeonPkReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_PVP_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & attacker & attackeder & dungeonId & battleId;
		}

		RacePlayerInfo attacker;
		RacePlayerInfo attackeder;
		UInt32	dungeonId;
		UInt32  battleId;
	};

	/**
	*@brief client->scene 任务列表请求
	*/
	class SceneLostDungeonTaskListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_TASK_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & grade;
		}

		UInt32 grade;
	};

	/**
	*@brief scene->client 任务列表返回
	*/
	class SceneLostDungeonTaskListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_TASK_LIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & taskVec;
		}

		std::vector<UInt32> taskVec;
	};

	/**
	*@brief client->scene 选择任务请求
	*/
	class SceneLostDungeonChoiceTaskReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_CHOICE_TASK_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & taskId;
		}

		UInt32 taskId;
	};

	/**
	*@brief scene->client 选择任务返回
	*/
	class SceneLostDungeonChoiceTaskRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_CHOICE_TASK_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & taskId;
		}

		UInt32 retCode;
		UInt32 taskId;
	};

	/**
	*@brief scene->client 任务数据通知
	*/
	class SceneLostDungeonTaskDataNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_TASK_DATA_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & taskId & status & vars;
		}

		UInt32 taskId;
		UInt32 status;
		std::map<std::string, std::string> vars;	//任务变量
	};

	/**
	*@brief scene->client 任务结果通知
	*/
	class SceneLostDungeonTaskResultNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_TASK_RESULT_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & taskId & result;
		}

		UInt32 taskId;
		UInt32 result;
	};
		
	/**
	*@brief client->scene 选择地下城挑战模式请求
	*/
	class SceneLostDungeonSwitchChageModeReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_SWITCH_DUNGE_CHAGEMODE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & chageMode;
		}

		UInt8 chageMode;
	};

	/**
	*@brief scene->client 选择地下城挑战模式返回
	*/
	class SceneLostDungeonSwitchChageModeRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_SWITCH_DUNGE_CHAGEMODE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}

		UInt32 code;
	};


	/**
	*@brief scene->client 同步地下城组队信息
	*/
	class SceneLostDungeonSyncDungeonTeamInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_SYNC_DUNGEON_TEAM_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & challengeMode & teamMemebers & addMember & leavePlayerId & teamInfos;
		}
	
		SceneLostDungeonSyncDungeonTeamInfo()
		{
			type = 0;
			challengeMode = 0;
			leavePlayerId = 0;
		}
		
		UInt8   type; //SyncDungeonEnterTeamInfoType
		UInt8	challengeMode;
		std::vector<LostDungTeamMember> teamMemebers;
		LostDungTeamMember addMember;
		UInt64	leavePlayerId;
		//队伍状态
		std::vector<LostDungTeamInfo> teamInfos;
	};

	/**
	*@brief battle->client 通知开始地下城投票
	*/
	class BattleTeamRaceVoteNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_TEAM_RACE_VOTE_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & dungeonId & teamId;
		}

	public:
		// 地下城ID
		UInt32			dungeonId;
		UInt32			teamId;
	};

	/**
	*@brief client->battle  玩家上报投票选项
	*/
	class BattleTeamReportVoteChoice : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(BATTLE_TEAM_REPORT_VOTE_CHOICE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & agree;
		}

	public:
		// 玩家id
		UInt64  playerId;
		// 是否同意
		UInt8	agree;
	};

	/**
	*@brief battle->client 广播玩家玩家投票选项
	*/
	class BattleTeamVoteChoiceNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(BATTLE_TEAM_VOTE_CHOICE_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & agree;
		}

	public:
		// 角色ID
		ObjID_t	roleId;
		// 是否同意
		UInt8	agree;
	};

	/**
	*@breif scene->battle 玩家离开战场
	*/
	class BattleExitNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(BATTLE_EXIT_NOTIFY)

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & battleId & battleType & battleResult & score & btState;
		}
	public:
		// 角色ID
		ObjID_t	roleId;
		// 战场id
		UInt32	battleId;
		// 战场类型
		UInt8	battleType;
		// 结果(LostDungeonBattleReasult)
		UInt8	battleResult;
		// 奖励积分
		UInt32	score;
		// 离开时候战斗状态
		UInt8	btState;
	};

	/**
	*@brief scene->client 资源列表
	*/
	class SceneLostDungeonSyncResourcesList : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_SYNC_RES_LIST)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID & infoes;
		}

		UInt32 battleID;
		std::vector<SceneItemInfo>		infoes;
	};

	/**
	*@brief scene->client 新增资源
	*/
	class  SceneLostDungeonResourceAdd : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_SYNC_RES_ADD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID & data;
		}

		void AddItem(UInt32 sceneId, const SceneItem& item)
		{
			for (auto& info : data)
			{
				if (info.sceneId == sceneId)
				{
					info.items.push_back(item);
					return;
				}
			}

			SceneItemInfo info;
			info.sceneId = sceneId;
			info.items.push_back(item);
			data.push_back(info);
		}

		UInt32 battleID;
		std::vector<SceneItemInfo>		data;
	};

	/**
	*@brief scene->client 删除资源
	*/
	class SceneLostDungeonResourceDel : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_SYNC_RES_DEL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID & guids;
		}

		UInt32 battleID;
		std::vector<ObjID_t>		guids;
	};

	/**
	*@brief client->server 宝箱选择道具请求
	*/
	class SceneLostDungeonBoxChoiceItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_BOX_CHOICE_ITEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemId;
		}

		UInt32 itemId;
	};

	/**
	*@brief server->client 宝箱选择道具返还
	*/
	class SceneLostDungeonBoxChoiceItemRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_BOX_CHOICE_ITEM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & item;
		}

		UInt32 retCode;
		ItemReward item;
	};

	/**
	*@brief battle->scene 结算爬塔的一层
	*/
	class SceneLostDungeonSettleFloor : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_SETTLE_FLOOR)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & floor & battleResult & score & btState;
		}

		// 角色ID
		ObjID_t	roleId;
		// 楼层
		UInt32	floor;
		// 结果(LostDungeonBattleReasult)
		UInt8	battleResult;
		// 奖励积分
		UInt32	score;
		// 战斗状态
		UInt8   btState;
	};

	/**
	*@brief scene->client 通知客户端爬塔楼层通关
	*/
	class SceneLostDungeonSettleFlooorNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_SETTLE_FLOOR_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & floor & battleResult & addScore & decScore & score & againItemId;
		}
		// 楼层
		UInt32	floor;
		// 结果(LostDungeonBattleReasult)
		UInt8	battleResult;
		// 获得积分
		UInt32	addScore;
		// 扣除积分
		UInt32	decScore;
		// 当前积分
		UInt32  score;
		// 失败可以重新挑战道具id
		UInt32 againItemId;
	};

	/**
	*@brief client->scene 爬塔领取奖励
	*/
	class SceneLostDungeonGetRewardReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_GET_REWARD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief scene->client 爬塔领取奖励返回
	*/
	class SceneLostDungeonGetRewardRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_GET_REWARD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}
		UInt32 code;
	};

	/**
	*@brief client->scene 放弃爬塔请求
	*/
	class SceneLostDungeonGiveUpReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_GIVEUP_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief client->scene 放弃爬塔返回
	*/
	class SceneLostDungeonGiveUpRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_GIVEUP_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}
		UInt32 code;
	};

	/**
	*@brief scene->scene 退出战场需要同步的数据
	*/
	class BattleLostDungSyncDataToSelfScene : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(BATTLE_LOSTDUNG_SYNCDATA_TOSELFSCENE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & lostDungeonHp & lostDungeonMp;
		}

		// 角色ID
		ObjID_t	roleId;
		// 地牢剩余血量(千分比)
		Int32   lostDungeonHp;
		// 地牢剩余蓝量(千分比)
		Int32   lostDungeonMp;
	};

	/**
	*@brief scene->client 同步战场数据
	*/
	class BattleLostDungSyncBattleData : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(BATTLE_LOSTDUNGEON_SYNC_BATTLEDATA)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleId & playerNum;
		}
		//战场id
		UInt32	battleId;
		// 战场人数
		ObjID_t	playerNum;
	};

	/**
	*@brief client->battleScene  查看玩家情报
	*/
	class BattleLostDungSeeIntellReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(BATTLE_LOSTDUNGEON_SEE_INTELL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId;
		}
		//玩家id
		UInt64	playerId;
	};


	/**
	*@brief battleScene->client  查看玩家情报返回
	*/
	class BattleLostDungSeeIntellRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(BATTLE_LOSTDUNGEON_SEE_INTELL_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & playerId & artifacts;
		}
		UInt32 code;
		//玩家id
		UInt64	playerId;
		//神器
		std::vector<ItemReward>  artifacts;
	};

	/**
	*@brief battleScene->client  敌人是否在地下城
	*/
	class SceneLostDungeonEnemyInDungeon : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_ENEMY_IN_DUNGEON)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & enemyId & dungeonId;
		}

		// 敌方id
		UInt64 enemyId;
		// 地下城id
		UInt32 dungeonId;
	};

	/**
	*@brief battle->scene 同步地牢队伍状态
	*/
	class BattleSynLostDungTeamSt : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(BATTLE_LOSTDUNG_SYN_TEAM_ST)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleId & teamId & state & isEnd;
		}
		//战场id
		UInt32	battleId;
		//队伍id
		UInt32	teamId;
		//队伍状态
		UInt8	state;
		//是否结束比赛
		UInt8   isEnd;
	};

	/**
	*@brief scene->battle 战场销毁返回
	*/
	class BattleDestoryRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(BATTLE_DESTROY_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleType & battleId;
		}
		//战场类型
		UInt8   battleType;
		//战场id
		UInt32	battleId;
		
	};

	/**
	*@brief  battleScene->client  通知客户端离开战场
	*/
	class SceneNotifyClientLeaveBattle : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_CLIENT_LEAVE_BATTLE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	
	};

	/**
	*@brief client->scene 查询击杀目标坐标请求
	*/
	class SceneQueryKillTargetPosReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_QUERY_KILL_TAEGET_POS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerIds;
		}
		//玩家
		std::vector<UInt64>  playerIds;
	};

	/**
	*@brief scene->client 查询击杀目标坐标返回
	*/
	class SceneQueryKillTargetPosRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_QUERY_KILL_TAEGET_POS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerPos;
		}
		std::vector<LostDungeonPlayerPos> playerPos;
	};

	/**
	*@brief node->battle 查询一个玩家详细信息
	*/
	class BattleQuerylayerDetailOne : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(BATTLE_QUERY_PLAYER_DETAIL_ONE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & srcRoleId & targetRoleId & queryType;
		}

		UInt64 srcRoleId;
		UInt64 targetRoleId;
		UInt32 queryType;
	};

	/**
	*@brief battle->scene 通知一次队伍地下城比赛结束
	*/
	class BattleNotifyTeamRaceEnd : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(BATTLE_NOTIFY_TEAM_RACE_END)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream  & teamId & reason ;
		}
		//队伍id
		UInt32	teamId;
		//原因
		UInt8	reason;
	
	};

	/**
	*@brief scene->battle 同步玩家战斗数据
	*/
	class BattleSyncRacePlayerInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(BATTLE_SYNC_RACE_PLAYER_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream  & racePlayerInfo;
		}
		//玩家战斗数据
		RacePlayerInfo	racePlayerInfo;

	};

};

#endif