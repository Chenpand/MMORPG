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
	*@brief client->world server ����ʼƥ��
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
        // ƥ�����ͣ���Ӧö��PKType
        UInt8       type;
	};

    /**
    *@brief world server->client ��ʼƥ�䷵��
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
	*@brief scene server->client PK����
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
		// PK���ͣ���Ӧö��(PkType)
		UInt8		pkType;
		UInt8		result;
		UInt32		oldPkValue;
		UInt32		newPkValue;
		UInt32		oldMatchScore;
		UInt32		newMatchScore;
		// ��ʼ����������
		UInt32		oldPkCoin;
		// ս����õľ�����
		UInt32		addPkCoinFromRace;
		// ����ս����õ�ȫ��������
		UInt32		totalPkCoinFromRace;
		// �Ƿ���PVP��ڼ�
		UInt8		isInPvPActivity;
		// ������õľ�����
		UInt32		addPkCoinFromActivity;
		// ���ջ��õ�ȫ��������
		UInt32		totalPkCoinFromActivity;
		// ԭ��λ
		UInt32		oldSeasonLevel;
		// �ֶ�λ
		UInt32		newSeasonLevel;
		//ԭ��
		UInt32		oldSeasonStar;
		//����
		UInt32		newSeasonStar;
		//ԭ����
		UInt32		oldSeasonExp;
		//�־���
		UInt32		newSeasonExp;
		//�ı�ľ���
		Int32		changeSeasonExp;
		//�������
		UInt32		getHonor;
	};

	/**
	*@brief client->scene ����μ�������
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
	*@brief scene->client �μ������᷵��
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
	*@brief client->scene ������ȡ�����ά��
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
	*@brief scene->client ��ȡ�����ά������
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
		std::vector<ItemReward> getItems;		//�鵽�ĵ���
	};

    /**
    *@brief world server->match server ����ʼƥ��
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
		// ƥ�������Ϣ
		MatchPlayerBaseInfo info;
        // ��ӦPkType
        UInt8           type;
		// ƥ�����
        UInt32          matchScore;
		// �ȼ�
		UInt16			level;
		// �Ѿ�����Ĵ���
		UInt32			fightCount;
		// ����ڼ��δ�
		UInt32			dailyCount;
		// ��ʤ��
		UInt32			winStreak;
		// ������
		UInt32			loseStreak;
		// ��λ
		UInt32			seasonLevel;
    };

	/**
	*@brief match server->world server ���ؿ�ʼƥ����
	*/
	struct MatchServerMatchStartRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(MATCHSERVER_MATCH_START_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleid & type & result;
		}

		// ��ɫID
		ObjID_t		roleid;
		// ��ӦPkType
		UInt8       type;
		// ���
		UInt32		result;
	};

	/**
	*@brief world server->match server ���³�ƥ��
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
	*@brief match server -> world server ���³�ƥ����
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
	*@brief client->server ����ȡ��ƥ��
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
    *@brief server->client ȡ��ƥ�䷵��
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
    *@brief world server->match server ����ȡ��ƥ��
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
	*@brief world server->match server ���󹫻�սƥ��
	*/
	class MatchServerGuildBattleMatchReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(MATCHSERVER_MATCH_GUILD_BATTLE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info & guildId & terrId & winStreak & isCross;
		}

		// ƥ�������Ϣ
		MatchPlayerBaseInfo			info;
		// ����ID
		ObjID_t						guildId;
		// ���ID
		UInt8						terrId;
		// ��ʤ��
		UInt32						winStreak;
		// �Ƿ���
		UInt8						isCross;
	};

	/**
	*@brief world server->match server �����ͽ�����ƥ��
	*/
	class MatchServerPremiumLeagueMatchReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(MATCHSERVER_MATCH_PREMIUM_LEAGUE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info & winStreak & score & seasonLevel;
		}

		// ƥ�������Ϣ
		MatchPlayerBaseInfo			info;
		// ��ʤ��
		UInt32						winStreak;
		// ����
		UInt32						score;
		// ��λ
		UInt32						seasonLevel;
	};

	/**
	*@brief world server->match server �����ͽ�������̭��
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
		// �ͽ�����״̬(��Ӧö��PremiumLeagueStatus)
		UInt8							battleStatus;
		// �������
		std::vector<ObjID_t>            roleIds;
	};

	/**
	*@brief match server->world server �ͽ�������̭������
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
		// relay��ַ
		Avalon::SockAddr				addr;
	};

	/**
	*@brief world server->match server ����PK����ս��
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
		// ����ID
		UInt32								roomId;
		std::vector<PkRoomMatchPlayerInfo>	players;
		// pk���ͣ���Ӧö��PkType��
		UInt8								pkType;
	};

	/**
	*@brief match server->world server ����PK����ս������
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
		// ����ID
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

    // ����ʼ��ϰ��
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

	// �����ѯ����״̬
	class WorldMatchQueryFriendStatusReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MATCH_QUERY_FRIEND_STATUS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	public:
	};

	// �����ѯ����״̬����
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
    *@brief match server->world server ���������Ϣ
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
		// ��ѯ�����ͣ�MatchQueryType��
		UInt8			type;
		// ���³�ID
		UInt32			dungeonId;
    };

    /**
    *@brief world server->match server ���������Ϣ
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
	*@brief match server->world server ���ƥ�䵽��֪ͨ
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
	*@brief match server->world server ���ȡ��ƥ��֪ͨ
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
	*@brief server->client ֪ͨ�ͻ��˱�����Ϣ
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
        // ��ɫID
        ObjID_t						roleId;

		//��¼RelayServer��session
		UInt64						session;
		
		// RelayServer��ַ
		Avalon::SockAddr			addr;

        // �������ͣ��ؿ���������
        UInt8						raceType;

        // ��ͼID
        UInt32						dungeonID;

		// ���������Ϣ
		std::vector<RacePlayerInfo> players;

		// �Ƿ��¼��־
		UInt8						isRecordLog;
		// �Ƿ��¼¼��
		UInt8						isRecordReplay;
		// ս�����
		UInt64						battleFlag;
	};

	/**
	*@brief server->client ֪ͨ�ͻ��˱�����Ϣ
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
		// ��ɫID
		ObjID_t             roleId;

		// ƥ�����
		UInt32				matchScore;
	};

	/**
	*@brief world->Scene ֪ͨ������Ϣ
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
		//����ID
		UInt32 seasonId;

		//��ʼʱ��
		UInt32 startTime;

		//����ʱ��
		UInt32 endTime;

		//�������Կ�ʼʱ��
		UInt32 seasonAttrStartTime;

		//�������Խ���ʱ��
		UInt32 seasonAttrEndTime;

		//�������а�����б�
		std::vector<UInt64> seasonSortListPlayerIds;
	};

	/**
	*@brief Scene->client ֪ͨ��λ��Ϣ
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
		//ԭ��λ
		UInt32 oldSeasonLevel;
		//ԭ�Ǽ�
		UInt32 oldSeasonStar;
		//�ֶ�λ
		UInt32 seasonLevel;
		//���Ǽ�
		UInt32 seasonStar;
	};


	/**
	*@brief client->server �ͻ���֪ͨ��������״̬
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
	*@brief secne->client ֪ͨ�ͻ���������Ϣ
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
		//����id
		UInt32 seasonId;
		//����ʱ��
		UInt32 endTime;
		//�������Խ���ʱ��
		UInt32 seasonAttrEndTime;
		//�������Խ����λ
		UInt32 seasonAttrLevel;
		//��������
		UInt8 seasonAttr;
		//�ֶ�λ
		UInt32 seasonLevel;
		//���Ǽ�
		UInt32 seasonStar;
		//���ھ���
		UInt32 seasonExp;
		//����״̬
		UInt8 seasonStatus;
	};

	/**
	*@brief secne->match ָ��PK��������Ϣ
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
		// �Ѷ�
		UInt8		hard;
		// ְҵ
		UInt8		occu;
		// ai
		UInt8		ai;
	};

	/**
	*@brief union server->match server ����ھ���
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
		// ״̬(��Ӧö��ChampionStatus)
		UInt8							battleStatus;
		// �������
		std::vector<ObjID_t>            roleIds;
	};

	/**
	*@brief match server->union server �ھ�������
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
		// relay��ַ
		Avalon::SockAddr				addr;
	};
	

	/**
	*@brief union-match server->world server ���������Ϣ
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
	*@brief world server->union-match server ���������Ϣ
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