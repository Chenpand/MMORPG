#ifndef _CL_GAMESESSIONPROTOCOL_H_
#define _CL_GAMESESSIONPROTOCOL_H_

#include <AvalonPacket.h>
#include <AvalonProtocol.h>
#include <AvalonSocket.h>
#include "CLProtocolDefine.h"
#include "CLGameSessionDefine.h"
#include "CLDungeonDefine.h"
#include "CLGuildDefine.h"
#include "CLMatchDefine.h"
#include "CLObjectDefine.h"

namespace CLProtocol
{
	/* Match Server相关协议 */

    /*
        RelayServer向MatchServer的注册消息
    */
    class MatchSvrRelayRegister : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(MATCHSERVER_RELAY_REGISTER)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & ability & isRestart;
        }

    public:
        UInt32 ability;         // RelayServer负载
        UInt8 isRestart;        // 是不是重新启动
    };
    
	class Fighter 
	{
		public:
			AVALON_DEFINE_NETSERIALIZATION()
			{
				stream & pos & accId & roleId & wsId;
			}
		public:

			UInt8 pos;						//占位0-9  最多支持5V5 PK
			UInt32 accId;					//帐号id
			ObjID_t roleId;					//角色id
            UInt32  wsId;                   //world server id
	protected:
	private:
	};

	class DungeonFighter : public Fighter
	{
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & pos & accId & roleId & wsId & serviceId & sourceSceneId;
		}
		DungeonFighter() : serviceId(0), sourceSceneId(0)
		{
		}
	public:

		UInt32  serviceId;				//service id
		UInt32  sourceSceneId;          //source secene id

	protected:
	private:
	};

	class MatchSvrStartGameReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(MATCHSERVER_START_GAME_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & raceType & gamesessionID & gamesessionFighter;
		}

	public:
        UInt8   raceType;
		ObjID_t gamesessionID;				//Match Server生成的，唯一标识一场PK的ID

		//PK双方信息
		std::vector<Fighter> gamesessionFighter;
		//std::vector<Fighter> fightersRight;
	protected:
	private:
	};

    // MatchServer通知RelayServer结束某场比赛
    class MatchSvrTerminateGame : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(MATCHSERVER_TERMINATE_GAME)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & gamesessionID;
        }
    public:
        ObjID_t gamesessionID;
    };

	// MatchServer通知RelayServer结束某场比赛
	class MatchSvrNotifyRacePlayerInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(MATCHSERVER_NOTIFY_RACE_PLAYER_INFO)
	public:
		MatchSvrNotifyRacePlayerInfo()
		{
			battleFlag = 0;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & gamesessionID & pkType & pkDungeonId & players & battleFlag;
		}
	public:
		ObjID_t							gamesessionID;
		UInt8							pkType;
		UInt32							pkDungeonId;
		std::vector<RacePlayerInfo>		players;
		UInt64							battleFlag;
	};
	
    class WorldServerTeamCreateRaceReq : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(WORLDSERVER_TEAM_CREATE_RACE_REQ)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
			stream & teamId & dungeonId & isHell & fighters & cityMonster;
        }

    public:
        UInt32                  teamId;
        UInt32                  dungeonId;
        UInt8                   isHell;
        std::vector<Fighter>    fighters;
		SceneNpc				cityMonster;
    };

    class WorldServerTeamCreateRaceRes : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(WORLDSERVER_TEAM_CREATE_RACE_RES)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
			stream & teamId & dungeonId & isHell & cityMonster & result & relayServerID & gamesessionID & address;
        }

    public:
        UInt32                  teamId;
        UInt32                  dungeonId;
        UInt8                   isHell;
		SceneNpc				cityMonster;

        UInt32                  result;             //取值：_MatchSvrRlaySvrResultCode
        UInt32                  relayServerID;
        ObjID_t                 gamesessionID;
        Avalon::SockAddr        address;            //relay server地址信息
    };

	class WorldServerTeamStartDungeonReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLDSERVER_TEAM_START_DUNGEON_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & teamId & dungeonId & cityMonster;
		}

	public:
		UInt32                  teamId;
		UInt32                  dungeonId;
		SceneNpc				cityMonster;
	};

	class WorldServerTeamStartDungeonRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLDSERVER_TEAM_START_DUNGEON_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & teamId & dungeonId & cityMonster;
		}

	public:
		UInt32                  teamId;
		UInt32                  dungeonId;
		SceneNpc				cityMonster;
	};

	enum _MatchSvrRlaySvrResultCode
	{
		MatchSvrRlaySvrResultCode_success					=		0,				//成功
		MatchSvrRlaySvrResultCode_duplicatedSessionId		=		1,				//重复的SessionID
		MatchSvrRlaySvrResultCode_fighter_alreadyhere		=		2,				//参战者已存在（已处于对战中）
		MatchSvrRlaySvrResultCode_invalid_race_type			=		3,				//无效的比赛类型
		MatchSvrRlaySvrResultCode_add_gamer_failed			=		4,				//添加玩家失败
		//to be inserted here!
		//.....
	};

	class MatchSvrStartGameRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(MATCHSERVER_START_GAME_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & gamesessionID & ability & address & raceType;
		}

	public:
		UInt32 result;							//取值：_MatchSvrRlaySvrResultCode
		ObjID_t gamesessionID;

		//
		UInt32 ability;							//relay server的承载能力
		Avalon::SockAddr address;				//relay server地址信息

		UInt8	raceType;

	protected:
	private:
	};

	//
	

// 	class FightergResult
// 	{
// 	public:
// 		AVALON_DEFINE_NETSERIALIZATION()
// 		{
// 			stream & resultFlag & pos & accId & roleId;
// 		}
// 
// 		UInt8			resultFlag;					//取值：_resultBitmask
// 		UInt8			pos;						//占位0-9  最多支持5V5 PK
// 		UInt32			accId;						//帐号id
// 		ObjID_t			roleId;						//角色id
// 
// 
// 	protected:
// 	private:
// 	};



	//RELAYSERVER_END_GAME_REQ  PK结束，请求结算 relay server --> match server   Match server暂时不返回响应（ 可以当作Notify消息）
	class MatchSvrEndGameReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RELAYSERVER_END_GAME_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & endReason & gamesessionID & raceEndInfo & raceType;
		}

		UInt8			endReason;						//结束原因：取值EGameSessionDismissType
		ObjID_t			gamesessionID;
		PkRaceEndInfo	raceEndInfo;
		UInt8			raceType;
	protected:
	private:
	};

    //RELAYSERVER_END_GAME_REQ  PK结束，请求结算 relay server --> match server   Match server暂时不返回响应（ 可以当作Notify消息）
    class MatchSvrDungeonRaceEnd : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(RELAYSERVER_DUNGEON_RACE_END)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & reason & sessionID & raceEndInfo;
        }

        UInt8	            reason;						//结束原因：取值EGameSessionDismissType
        ObjID_t             sessionID;

        DungeonRaceEndInfo  raceEndInfo;
    protected:
    private:
    };

	//RELAYSERVER_END_GAME_RES   relay server <-- match server   暂时不用
	class MatchSvrEndGameRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RELAYSERVER_END_GAME_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	protected:
	private:
	};

    struct MatchOpponent
    {
		MatchOpponent()
		{
			zoneId = 0;
			roleId = 0;
			occu = 0;
			level = 0;
			seasonLevel = 0;
			seasonStars = 0;
		}

        AVALON_DEFINE_NETSERIALIZATION()
        {
			stream & zoneId & roleId & name & occu & level & seasonLevel & seasonStars;
        }

		UInt32		zoneId;
        ObjID_t     roleId;
		std::string	name;
        UInt8       occu;
		UInt16		level;
		UInt32		seasonLevel;
		UInt8		seasonStars;
    };

    class MatchServerRaceEnd : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(MATCHSERVER_RACE_END)
    public:
        MatchServerRaceEnd()
        {
            seasonScoreChange = 0;
			dungeonID = 0;
			usedTime = 0;
        }

        AVALON_DEFINE_NETSERIALIZATION()
        {
			stream & type & opponent & result & seasonScoreChange & dungeonID & usedTime;
        }

        UInt8           type;
        MatchOpponent   opponent;
		PkPlayerRaceEndInfo  result;

        // 赛季积分变化
        Int32           seasonScoreChange;
		// pk地图的id
		UInt32          dungeonID;
		// 使用时间
		UInt32			usedTime;
    };

    class MatchServerDungeonRaceEnd : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(MATCHSERVER_DUNGEON_RACE_END)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & teamId & result & endInfo;
        }

        UInt32                  teamId;
        UInt8                   result;
        DungeonRaceEndInfo      endInfo;
    };

	class MatchServerGuildBattleRaceEnd : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(MATCHSERVER_GUILD_BATTLE_RACE_END)
	public:
		MatchServerGuildBattleRaceEnd()
		{
			sessionId = 0;
			terrId = 0;
			result = 0;
			winerRemainHp = 0;
			winerRemainMp = 0;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sessionId & terrId & result & winner & loser & winerRemainHp & winerRemainMp;
		}

		// 比赛ID
		ObjID_t				sessionId;
		// 领地ID
		UInt32				terrId;
		// pk结果（正常结算、平局、无效局）
		UInt8				result;
		GuildBattleMember	winner;
		GuildBattleMember	loser;
		UInt32				winerRemainHp;
		UInt32				winerRemainMp;
	};
	
	// 赏金联赛成员信息
	struct PremiumLeagueFighter
	{
		PremiumLeagueFighter()
		{
			id = 0;
			remainHp = 0;
			remainMp = 0;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & name & remainHp & remainMp;
		}

		// 角色ID
		ObjID_t id;
		// 名字
		std::string name;
		// 剩余血量
		UInt32 remainHp;
		// 剩余蓝量
		UInt32 remainMp;
	};

	// 赏金联赛结算
	class MatchServerPremiumLeagueRaceEnd : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(MATCHSERVER_PREMIUM_LEAGUE_RACE_END)
	public:
		MatchServerPremiumLeagueRaceEnd()
		{
			raceId = 0;
			result = 0;
			dungeonID = 0;
			usedTime = 0;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & raceId & result & winner & loser & dungeonID & usedTime;
		}

		// pk结果（正常结算、平局、无效局）
		ObjID_t					raceId;
		UInt8					result;
		PremiumLeagueFighter	winner;
		PremiumLeagueFighter	loser;
		UInt32                  dungeonID;
		UInt32					usedTime;
	};

	// 冠军赛成员信息
	struct ChampionFighter
	{
		ChampionFighter()
		{
			id = 0;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & name ;
		}

		// 角色ID
		ObjID_t id;
		// 名字
		std::string name;
	};

	// 冠军赛结算
	class MatchServerChampionRaceEnd : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(MATCHSERVER_CHAMPION_RACE_END)
	public:
		MatchServerChampionRaceEnd()
		{
			groupID = 0;
			raceId = 0;
			result = 0;
			dungeonID = 0;
			order = 0;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & groupID & raceId & order & result & winner & loser & dungeonID;
		}

		// pk结果（正常结算、平局、无效局）
		UInt32					groupID;
		ObjID_t					raceId;
		UInt32					order;
		UInt8					result;
		ChampionFighter			winner;
		ChampionFighter			loser;
		UInt32                  dungeonID;
	};

	class MatchServerPkRoomRaceEnd : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(MATCHSERVER_PK_ROOM_RACE_END)
	public:
		MatchServerPkRoomRaceEnd()
		{
			dungeonID = 0;
			usedTime = 0;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & raceId & results & dungeonID & usedTime;
		}

		UInt8								type;
		UInt64								raceId;
		std::vector<Pk3V3PlayerRaceEndInfo>	results;
		UInt32                              dungeonID;
		UInt32								usedTime;
	};
	

    class SceneServerPlayerLeaveRace : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENESERVER_PLAYER_LEAVE_RACE)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & raceId & roleId;
        }

        ObjID_t raceId;
        ObjID_t roleId;
    };

	class SceneServerSyncPlayerEvent : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENESERVER_SYNC_PLAYER_EVENT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & raceId & roleId & type & param1 & param2;
		}

		ObjID_t raceId;
		ObjID_t roleId;

		// SceneServerSyncPlayerEvent
		UInt8	type;
		ObjID_t	param1;
		ObjID_t param2;
	};

	class SceneServerGiveupReconn : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENESERVER_GIVEUP_RECONN)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId;
		}

		ObjID_t roleId;
	};
	
	class RelayServerNotifyRaceStart : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RELAYSERVER_RACE_START)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & raceId & addr & roleId & raceType;
		}

		ObjID_t raceId;
		Avalon::SockAddr addr;
		ObjID_t	roleId;
		// 比赛类型 RaceType
		UInt8	raceType;
	};

    /**
    *@brief scene、world、match、relay 之间转发的消息
            目前提供 scene --> match
                     scene --> relay
                     world --> match
                     world --> relay
                     relay --> world
    */
    class SysTransmitRaceProtocolInNode : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SYS_TRANSMIT_RACE_RPROTOCOL_INNODES)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & nodeId & (*packet);
        }

        SysTransmitRaceProtocolInNode() { packet = Avalon::Packet::Create(); }
        SysTransmitRaceProtocolInNode(Avalon::Packet* pack) { packet = pack; }
        ~SysTransmitRaceProtocolInNode() { Avalon::Packet::Destroy(packet); }

        void Detach() { packet = NULL; }

        //节点id
        UInt32	            nodeId;
        //消息包
        Avalon::Packet*     packet;
    };

    /**
    *@brief relay -> scene 发送消息到scene server上
    */
    class SysTransmitRaceProtocolToScenePlayer : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SYS_TRANSMIT_RACE_RPROTOCOL_TO_SCENE_PLAYER)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & roleId & (*packet);
        }

        SysTransmitRaceProtocolToScenePlayer() { packet = Avalon::Packet::Create(); }
        SysTransmitRaceProtocolToScenePlayer(Avalon::Packet* pack) { packet = pack; }
        ~SysTransmitRaceProtocolToScenePlayer() { Avalon::Packet::Destroy(packet); }

        void Detach() { packet = NULL; }

        //角色id
        ObjID_t	            roleId;
        //消息包
        Avalon::Packet*     packet;
    };

	///**
	//*@   service -> dungeon 队伍请求地下城服务器创建比赛
	//*/
	//class DungenServerTeamCreateRaceReq : public Avalon::Protocol
	//{
	//	AVALON_DEFINE_PROTOCOLID(DUNGEON_SVR_TEAM_CREATE_RACE_REQ)
	//public:
	//	AVALON_DEFINE_NETSERIALIZATION()
	//	{
	//		stream & teamId & dungeonId  & fighters;
	//	}

	//	DungenServerTeamCreateRaceReq() : teamId(0), dungeonId(0)
	//	{
	//	}

	//public:
	//	UInt32						   teamId;
	//	UInt32						   dungeonId;
	//	std::vector<DungeonFighter>    fighters;
	//};

	///**
	//*@   dungeon -> service 队伍请求地下城服务器创建比赛返回
	//*/
	//class DungenServerTeamCreateRaceRes : public Avalon::Protocol
	//{
	//	AVALON_DEFINE_PROTOCOLID(DUNGEON_SVR_TEAM_CREATE_RACE_RES)
	//public:
	//	AVALON_DEFINE_NETSERIALIZATION()
	//	{
	//		stream & teamId & dungeonId  & result & relayServerID & gamesessionID & address;
	//	}

	//public:
	//	UInt32                  teamId;
	//	UInt32                  dungeonId;

	//	UInt32                  result;             //取值：
	//	UInt32                  relayServerID;
	//	ObjID_t                 gamesessionID;
	//	Avalon::SockAddr        address;            //relay server地址信息
	//};

	/**
	*@brief service->dungeon 请求地下城战斗
	*/
	class DungenServerTeamStartRaceReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(DUNGEON_SVR_TEAM_START_RACE_REQ)
	public:
		DungenServerTeamStartRaceReq()
		{
			teamId = 0;
			dungeonId = 0;
			passedAreaIndex = 0;
			sourceSceneNodeId = 0;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & teamId  & dungeonId & passedAreaIndex & fighters & playerInfoes 
				& clearedDungeonIds & battleTempRaceId & monster & sourceSceneNodeId;
		}

		UInt32									teamId;
		UInt32									dungeonId;
		UInt32									passedAreaIndex;
		std::vector<DungeonFighter>				fighters;

		std::vector<DungeonRacePlayerInfo>		playerInfoes;

		// relayserver相关
		//UInt32									relayId;
		//UInt64									session;
		//Avalon::SockAddr						relayAddr;
		std::vector<UInt32>						clearedDungeonIds; //已经通过地下城id
		UInt64									battleTempRaceId;	//battle上分配的临时raceid
		SceneNpc								monster;//小怪
		UInt32									sourceSceneNodeId;
	};

	class DungenServerTeamStartRaceRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(DUNGEON_SVR_TEAM_START_RACE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & teamId & dungeonId & result & isContinue & relayServerID & gamesessionID & address & battleTempRaceId;
		}

	public:
		UInt32                  teamId;
		UInt32                  dungeonId;
		UInt32					result;
		UInt8					isContinue;

		// relayserver相关
		UInt32                  relayServerID;
		ObjID_t                 gamesessionID;
		Avalon::SockAddr        address;            //relay server地址信息
		UInt64					battleTempRaceId;	//战场临时raceid
	};

	//地下城结束，请求结算 dungeon server --> server  
	class DungeonSvrDungeonRaceEnd : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(DUNGEON_SVR_DUNGEON_RACE_END)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & teamId & result & endInfo & gamesessionID;
		}

		UInt32                  teamId;
		UInt8                   result;
		DungeonRaceEndInfo      endInfo;
		ObjID_t					gamesessionID;
	};

	// 同步relay负载
	class RelayAbalitySync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RELAYSERVER_ABALITY_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & abality;
		}
		UInt32 abality;
	};

}//end of namespace Protocol


#endif
