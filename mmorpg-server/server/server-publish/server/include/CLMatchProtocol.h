#ifndef _CL_MATCH_PROTOCOL_H_
#define _CL_MATCH_PROTOCOL_H_

#include <AvalonProtocol.h>
#include <AvalonSocket.h>
#include "CLProtocolDefine.h"
#include "CLItemDefine.h"
#include "CLMatchDefine.h"

namespace CLProtocol
{
	/**
	*@brief client->world server 请求开始匹配
	*/
	class SceneMatchStartReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_MATCH_START_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
            stream & type;
		}

	public:
        // 匹配类型，对应枚举PKType
        UInt8       type;
	};

    /**
    *@brief world server->client 开始匹配返回
    */
    class WorldMatchStartRes : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(WORLD_MATCH_START_RES)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & result;
        }

    public:
        UInt32 result;
    };

	/**
	*@brief scene server->client PK结算
	*/
	class SceneMatchPkRaceEnd : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_MATCH_PK_RACE_END)
	public:
		SceneMatchPkRaceEnd()
		{
			pkType = 0;
			result = 0;
			oldPkValue = 0;
			newPkValue = 0;
			oldMatchScore = 0;
			newMatchScore = 0;
			oldPkCoin = 0;
			addPkCoinFromRace = 0;
			addPkCoinFromActivity = 0;
			oldSeasonLevel = 0;
			newSeasonLevel = 0;
			oldSeasonStar = 0;
			newSeasonStar = 0;
			oldSeasonExp = 0;
			newSeasonExp = 0;
			changeSeasonExp = 0;
			getHonor = 0;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & pkType & result & oldPkValue & newPkValue & oldMatchScore & newMatchScore & oldPkCoin &
				addPkCoinFromRace & totalPkCoinFromRace & isInPvPActivity & addPkCoinFromActivity & totalPkCoinFromActivity &
				oldSeasonLevel & newSeasonLevel & oldSeasonStar & newSeasonStar & oldSeasonExp & newSeasonExp & changeSeasonExp & getHonor;
		}

	public:
		// PK类型，对应枚举(PkType)
		UInt8		pkType;
		UInt8		result;
		UInt32		oldPkValue;
		UInt32		newPkValue;
		UInt32		oldMatchScore;
		UInt32		newMatchScore;
		// 初始决斗币数量
		UInt32		oldPkCoin;
		// 战斗获得的决斗币
		UInt32		addPkCoinFromRace;
		// 今日战斗获得的全部决斗币
		UInt32		totalPkCoinFromRace;
		// 是否在PVP活动期间
		UInt8		isInPvPActivity;
		// 活动额外获得的决斗币
		UInt32		addPkCoinFromActivity;
		// 今日活动获得的全部决斗币
		UInt32		totalPkCoinFromActivity;
		// 原段位
		UInt32		oldSeasonLevel;
		// 现段位
		UInt32		newSeasonLevel;
		//原星
		UInt32		oldSeasonStar;
		//现星
		UInt32		newSeasonStar;
		//原经验
		UInt32		oldSeasonExp;
		//现经验
		UInt32		newSeasonExp;
		//改变的经验
		Int32		changeSeasonExp;
		//获得荣誉
		UInt32		getHonor;
	};

	/**
	*@brief client->scene 请求参加武道大会
	*/
	class SceneWudaoJoinReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_WUDAO_JOIN_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief scene->client 参加武道大会返回
	*/
	class SceneWudaoJoinRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_WUDAO_JOIN_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		UInt32		result;
	};

	/**
	*@brief client->scene 请求领取武道大会奖励
	*/
	class SceneWudaoRewardReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_WUDAO_REWARD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief scene->client 领取武道大会奖励返回
	*/
	class SceneWudaoRewardRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_WUDAO_REWARD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & getItems;
		}

		UInt32		result;
		std::vector<ItemReward> getItems;		//抽到的道具
	};

    /**
    *@brief world server->match server 请求开始匹配
    */
    class MatchServerMatchStartReq : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(MATCHSERVER_MATCH_START_REQ)
    public:
		MatchServerMatchStartReq()
		{
			matchScore = 0;
			level = 0;
			fightCount = 0;
			dailyCount = 0;
			winStreak = 0;
			loseStreak = 0;
			seasonLevel = 0;
		}

        AVALON_DEFINE_NETSERIALIZATION()
        {
			stream & info & type & matchScore & level & fightCount & dailyCount & winStreak & loseStreak & seasonLevel;
        }

    public:
		// 匹配基础信息
		MatchPlayerBaseInfo info;
        // 对应PkType
        UInt8           type;
		// 匹配积分
        UInt32          matchScore;
		// 等级
		UInt16			level;
		// 已经打过的次数
		UInt32			fightCount;
		// 今天第几次打
		UInt32			dailyCount;
		// 连胜数
		UInt32			winStreak;
		// 连败数
		UInt32			loseStreak;
		// 段位
		UInt32			seasonLevel;
    };

	/**
	*@brief match server->world server 返回开始匹配结果
	*/
	struct MatchServerMatchStartRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(MATCHSERVER_MATCH_START_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleid & type & result;
		}

		// 角色ID
		ObjID_t		roleid;
		// 对应PkType
		UInt8       type;
		// 结果
		UInt32		result;
	};

	/**
	*@brief world server->match server 地下城匹配
	*/
	class MatchServerMatchDungeonReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(MATCHSERVER_MATCH_DUNGEON_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & dungeonId & isHell & fighters & level;
		}

	public:
		UInt32								dungeonId;
		UInt8								isHell;
		std::vector<MatchPlayerBaseInfo>	fighters;
		UInt32								level;
	};
	
	/**
	*@brief match server -> world server 地下城匹配结果
	*/
	class MatchServerMatchDungeonRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(MATCHSERVER_MATCH_DUNGEON_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerIds & result;
		}

	public:
		std::vector<ObjID_t>		playerIds;
		UInt32						result;
	};

	/**
	*@brief client->server 请求取消匹配
	*/
	class SceneMatchCancelReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_MATCH_CANCEL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}

	public:

	};

    /**
    *@brief server->client 取消匹配返回
    */
    class WorldMatchCancelRes : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(WORLD_MATCH_CANCEL_RES)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
			stream & result;
        }

    public:
        UInt32 result;
    };

    /**
    *@brief world server->match server 请求取消匹配
    */
    class MatchServerMatchCancelReq : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(MATCHSERVER_MATCH_CANCEL_REQ)
    public:
		MatchServerMatchCancelReq() : id(0), needReturn(0) {}

        AVALON_DEFINE_NETSERIALIZATION()
        {
			stream & id & needReturn;
        }

    public:
        ObjID_t         id;
		UInt8			needReturn;
    };

	/**
	*@brief world server->match server 请求公会战匹配
	*/
	class MatchServerGuildBattleMatchReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(MATCHSERVER_MATCH_GUILD_BATTLE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info & guildId & terrId & winStreak & isCross;
		}

		// 匹配基础信息
		MatchPlayerBaseInfo			info;
		// 公会ID
		ObjID_t						guildId;
		// 领地ID
		UInt8						terrId;
		// 连胜数
		UInt32						winStreak;
		// 是否跨服
		UInt8						isCross;
	};

	/**
	*@brief world server->match server 请求赏金联赛匹配
	*/
	class MatchServerPremiumLeagueMatchReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(MATCHSERVER_MATCH_PREMIUM_LEAGUE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info & winStreak & score & seasonLevel;
		}

		// 匹配基础信息
		MatchPlayerBaseInfo			info;
		// 连胜数
		UInt32						winStreak;
		// 积分
		UInt32						score;
		// 段位
		UInt32						seasonLevel;
	};

	/**
	*@brief world server->match server 请求赏金联赛淘汰赛
	*/
	class MatchServerPremiumLeagueBattleReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(MATCHSERVER_MATCH_PREMIUM_LEAGUE_BATTLE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleId & battleStatus & roleIds;
		}
	public:
		// battle id
		ObjID_t							battleId;
		// 赏金联赛状态(对应枚举PremiumLeagueStatus)
		UInt8							battleStatus;
		// 参赛玩家
		std::vector<ObjID_t>            roleIds;
	};

	/**
	*@brief match server->world server 赏金联赛淘汰赛返回
	*/
	class MatchServerPremiumLeagueBattleRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(MATCHSERVER_MATCH_PREMIUM_LEAGUE_BATTLE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleId & sessionId & addr;
		}
	public:
		// battle id
		ObjID_t							battleId;
		// session id
		ObjID_t							sessionId;
		// relay地址
		Avalon::SockAddr				addr;
	};

	/**
	*@brief world server->match server 请求PK房间战斗
	*/
	class MatchServerPkRoomReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(MATCHSERVER_MATCH_PK_ROOM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roomId & players & pkType;
		}
	public:
		// 房间ID
		UInt32								roomId;
		std::vector<PkRoomMatchPlayerInfo>	players;
		// pk类型（对应枚举PkType）
		UInt8								pkType;
	};

	/**
	*@brief match server->world server 请求PK房间战斗返回
	*/
	class MatchServerPkRoomRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(MATCHSERVER_MATCH_PK_ROOM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roomId & playerIds & result;
		}
	public:
		// 房间ID
		UInt32					roomId;
		std::vector<UInt64>		playerIds;
		UInt32					result;
	};

    class WorldMatchRaceStart : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(WORLD_MATCH_RACE_START)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & roleId;
        }
    public:
        ObjID_t         roleId;
    };

    // 请求开始练习赛
    class WorldMatchPracticeReq : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(WORLD_MATCH_PRACTICE_REQ)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
			stream & roleIds & pkType;
        }
    public:
        std::vector<ObjID_t>            roleIds;
		UInt8                           pkType;
    };

	// 请求查询好友状态
	class WorldMatchQueryFriendStatusReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MATCH_QUERY_FRIEND_STATUS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	public:
	};

	// 请求查询好友状态返回
	class WorldMatchQueryFriendStatusRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MATCH_QUERY_FRIEND_STATUS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & infoes;
		}
	public:
		std::vector<FriendMatchStatusInfo>		infoes;
	};

    /**
    *@brief match server->world server 请求玩家信息
    */
    class MatchServerQueryPlayerInfoReq : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(MATCHSERVER_QUERY_PLAYER_INFO_REQ)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
			stream & sessionId & roleId & type & dungeonId;
        }

    public:
        ObjID_t         sessionId;
        ObjID_t         roleId;
		// 查询的类型（MatchQueryType）
		UInt8			type;
		// 地下城ID
		UInt32			dungeonId;
    };

    /**
    *@brief world server->match server 返回玩家信息
    */
    class MatchServerQueryPlayerInfoRes : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(MATCHSERVER_QUERY_PLAYER_INFO_RES)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & sessionId & roleId & result & type & racePlayerInfo & dungeonPlayerInfo;
        }

    public:
        ObjID_t					sessionId;
        ObjID_t					roleId;
        UInt8					result;
		UInt8					type;
        RacePlayerInfo			racePlayerInfo;
		DungeonRacePlayerInfo	dungeonPlayerInfo;
    };

	/**
	*@brief match server->world server 玩家匹配到的通知
	*/
	class MatchServerPlayerMatchSuccessNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(MATCHSERVER_PLAYER_MATCH_SUCCESS_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & zoneId & roleId & opponentId & type & seasonLevel & matchScore & usedTime;
		}

	public:
		UInt32		zoneId;
		ObjID_t		roleId;
		ObjID_t		opponentId;
		UInt8		type;
		UInt32		seasonLevel;
		UInt32		matchScore;
		UInt32		usedTime;
	};

	/**
	*@brief match server->world server 玩家取消匹配通知
	*/
	class MatchServerPlayerMatchCancelNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(MATCHSERVER_PLAYER_MATCH_CANCEL_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & type & matchTime & seasonLevel & matchScore;
		}

	public:
		ObjID_t		roleId;
		UInt8		type;
		UInt32		matchTime;
		UInt32		seasonLevel;
		UInt32		matchScore;
	};

	/**
	*@brief server->client 通知客户端比赛信息
	*/
	class WorldNotifyRaceStart : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_RACE_START)

	public:
		WorldNotifyRaceStart()
		{
			battleFlag = 0;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & session & addr & raceType & dungeonID & players & isRecordLog & isRecordReplay & battleFlag;
		}

	public:
        // 角色ID
        ObjID_t						roleId;

		//登录RelayServer的session
		UInt64						session;
		
		// RelayServer地址
		Avalon::SockAddr			addr;

        // 比赛类型（关卡，竞技）
        UInt8						raceType;

        // 地图ID
        UInt32						dungeonID;

		// 所有玩家信息
		std::vector<RacePlayerInfo> players;

		// 是否记录日志
		UInt8						isRecordLog;
		// 是否记录录像
		UInt8						isRecordReplay;
		// 战斗标记
		UInt64						battleFlag;
	};

	/**
	*@brief server->client 通知客户端比赛信息
	*/
	class WorldChangeMatchScore : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MATCH_CHANGE_SCORE)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & matchScore;
		}

	public:
		// 角色ID
		ObjID_t             roleId;

		// 匹配积分
		UInt32				matchScore;
	};

	/**
	*@brief world->Scene 通知赛季信息
	*/
	class SceneNotifySeason :public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_SEASON)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & seasonId & startTime & endTime & seasonAttrStartTime & seasonAttrEndTime & seasonSortListPlayerIds;
		}
	public:
		//赛季ID
		UInt32 seasonId;

		//开始时间
		UInt32 startTime;

		//结束时间
		UInt32 endTime;

		//赛季属性开始时间
		UInt32 seasonAttrStartTime;

		//赛季属性结束时间
		UInt32 seasonAttrEndTime;

		//赛季排行榜玩家列表
		std::vector<UInt64> seasonSortListPlayerIds;
	};

	/**
	*@brief Scene->client 通知段位信息
	*/
	class SceneSyncSeasonLevel : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_SEASON_LEVEL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & oldSeasonLevel & oldSeasonStar & seasonLevel & seasonStar;
		}
	public:
		//原段位
		UInt32 oldSeasonLevel;
		//原星级
		UInt32 oldSeasonStar;
		//现段位
		UInt32 seasonLevel;
		//现星级
		UInt32 seasonStar;
	};


	/**
	*@brief client->server 客户端通知赛季播放状态
	*/
	class SceneSeasonPlayStatus :public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SEASON_PLAY_STATUS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & seasonId;
		}
	public:
		UInt32 seasonId;
	};

	/**
	*@brief secne->client 通知客户端赛季信息
	*/
	class SceneSyncSeasonInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_SEASON_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & seasonId & endTime & seasonAttrEndTime & seasonAttrLevel & seasonAttr & seasonLevel & seasonStar & seasonExp & seasonStatus;
		}

	public:
		//赛季id
		UInt32 seasonId;
		//结束时间
		UInt32 endTime;
		//赛季属性结束时间
		UInt32 seasonAttrEndTime;
		//赛季属性结算段位
		UInt32 seasonAttrLevel;
		//赛季属性
		UInt8 seasonAttr;
		//现段位
		UInt32 seasonLevel;
		//现星级
		UInt32 seasonStar;
		//现在经验
		UInt32 seasonExp;
		//赛季状态
		UInt8 seasonStatus;
	};

	/**
	*@brief secne->match 指定PK机器人信息
	*/
	class MatchServerSpecifyPkRobot : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(MATCHSERVER_SEPCIFY_PK_ROBOT)
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

	/**
	*@brief union server->match server 请求冠军赛
	*/
	class MatchServerChampionBattleReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(MATCHSERVER_MATCH_CHAMPION_BATTLE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & groupID & order & battleStatus & roleIds;
		}
	public:
		// battle id
		UInt32							groupID;
		// orderid
		UInt32							order = 0;
		// 状态(对应枚举ChampionStatus)
		UInt8							battleStatus;
		// 参赛玩家
		std::vector<ObjID_t>            roleIds;
	};

	/**
	*@brief match server->union server 冠军赛返回
	*/
	class MatchServerChampionBattleRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(MATCHSERVER_MATCH_CHAMPION_BATTLE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & groupID & order & sessionId & addr;
		}
	public:
		// battle id
		ObjID_t							groupID;
		// orderid
		UInt32							order;
		// session id
		ObjID_t							sessionId;
		// relay地址
		Avalon::SockAddr				addr;
	};
	

	/**
	*@brief union-match server->world server 请求玩家信息
	*/
	class UnionMatchServerQueryPlayerInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(UNION_MATCHSERVER_QUERY_PLAYER_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & msg;
		}

	public:
		MatchServerQueryPlayerInfoReq         msg;
	};

	/**
	*@brief world server->union-match server 返回玩家信息
	*/
	class UnionMatchServerQueryPlayerInfoRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(UNION_MATCHSERVER_QUERY_PLAYER_INFO_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & msg;
		}

	public:
		MatchServerQueryPlayerInfoRes msg;
	};
}
#endif