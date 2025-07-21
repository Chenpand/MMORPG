#ifndef __CL_MATCH_DEFINE_H__
#define __CL_MATCH_DEFINE_H__

#include "CLDefine.h"
#include "CLItemDefine.h"
#include "CLSkillDefine.h"
#include "CLWarpStoneDefine.h"
#include "CLRetinueDefine.h"
#include "CLDropItemDefine.h"
#include "CLPetDefine.h"
#include "CLEquipSchemeDefine.h"

const UInt32 SEASON_MIN_MATCH_SCORE = 1000;

enum PkType
{
    PK_NORMAL_1V1,  // ��ͳ������1V1
    PK_SEASON_1V1,  // ������1V1
	PK_GUILD_BATTLE,// ����ս
	PK_WUDAODAHUI,	// ������
	PK_PRACTICE,	// �����д�
	PK_PREMIUM_LEAGUE_PRELIMINAY,	// �ͽ�����Ԥѡ��
	PK_PREMIUM_LEAGUE_BATTLE,		// �ͽ������Ծ�����8ǿ�Ժ�
	PK_3V3_ROOM,					// 3v3����
	PK_3V3_MATCH,					// 3v3ƥ��
	PK_3V3_ACTIVITY,				// 3v3�
	PK_3V3_TUMBLE,					// 3v3�Ҷ�
	PK_1V1_CHIJI,					// 1v1�Լ�
	PK_3V3_CHIJI,					// 3v3�Լ�
	PK_EQUAL_1V1,                   // ��ƽ������
	PK_EQUAL_PRACTICE,              // ��ƽ�����������д�
	PK_2V2_ACTIVITY,                // 2V2�Ҷ��
	PK_LOST_DUNGEON,				// ��ʧ����PK
	PK_CHAMPION_BATTLE,				// �ھ���
	PK_5V5_MATCH,					// 5V5ƥ��
    PK_NUM,
};

/**
*@brief ������Ϣ����
*/
enum PkStatisticAttr
{
    PKIA_INVALID,
    PKIA_TYPE,		        // ����	UInt8
    PKIA_TOTAL_WIN_NUM,     // ��ʤ���� UInt32
    PKIA_TOTAL_LOSE_NUM,    // �ܸ����� UInt32
    PKIA_TOTAL_NUM,         // �ܳ��� UInt32
    PKIA_DETAIL_RECORD_INFO,// ��ս��ְͬҵ��ս��
    PKIA_RECENT_RECORD,     // ���ս��
    PKIA_MAX_WIN_STEAK,     // �����ʤ
    PKIA_CUR_WIN_STEAK,     // ��ǰ��ʤ

    PKIA_NUM,
};

// PK���
enum PkRaceResult
{
    PKRR_INVALID,       // ��Ч
    PKRR_WIN,           // ʤ��
    PKRR_LOSE,          // ʧ��
    PKRR_DOGFALL,       // ƽ��
    PKRR_NUM
};

// ��������
enum RaceType
{
	RT_DUNGEON,
	RT_PK,
	RT_GUILD_BATTLE,
	// �ͽ�����Ԥѡ��
	RT_PREMIUM_LEAGUE_PRELIMINAY,
	// �ͽ�������̭��
	RT_PREMIUM_LEAGUE_BATTLE,
	// 3V3
	RT_PK_3V3,
	// ������
	RT_SCORE_WAR,
	// 3V3�Ҷ�
	RT_3V3_TUMBLE,
	// �Լ�
	RT_CHIJI,
	// ������³�
	RT_EQULA_PK,
	// ������³�
	RT_DUNGEON_CROSS,
	// 2V2�Ҷ��
	RT_2V2_TUMNLE,
	// ��ʧ����pk
	RT_LOST_DUNGEON_PK,
	// �ھ���
	RT_CHAMPION = 13,
	// ��ս�ñ���
	RT_OBSERVE = 14,
	// 5V5
	RT_PK_5V5,
};

// ����״̬
enum FriendMatchStatus
{
	// ����
	FRIEND_MATCH_STATUS_IDLE,
	// æµ
	FRIEND_MATCH_STATUS_BUSY,
	// ����
	FRIEND_MATCH_STATUS_OFFLINE,
};

// ����״̬��Ϣ
struct FriendMatchStatusInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & roleId & status;
	}
	ObjID_t		roleId;
	// ״̬����Ӧö�٣�FriendMatchStatus��
	UInt8		status;
};

// ƥ���������ѯ������
enum MatchQueryType
{
	// ����
	MATCH_QUERY_RACE_INFO,
	// ���ս
	MATCH_QUERY_GUILD_RACE_INFO,
	// �ͽ�����Ԥѡ��
	MATCH_PREMIUM_LEAGUE_PRELIMINAY_INFO,
	// ���³�
	MATCH_QUERY_DUNGEON_INFO,
	//��ƽ������
	MATCH_QUERY_EQUAL_RACE_INFO,
};

// ƥ���������ѯ�����Ϣ����
enum MatchQueryPlayerInfoResult
{
	MATCH_QUERY_SUCCESS,
	MATCH_QUERY_PLAYER_OFFLINE,
};

// ������AI�Ѷ�
enum RobotAIHardType
{
	ROBOT_AI_INVALID,	// ��Чֵ
	ROBOT_AI_FISH,		// ����
	ROBOT_AI_EASY,		// ��
	ROBOT_AI_NORMAL,	// ��ͨ
	ROBOT_AI_HARD,		// ����
	ROBOT_AI_MASTER,	// ��ʦ
	ROBOT_AI_GUILD_BATTLE,	// ����ս
};

// �����еļ�����Ϣ
struct RaceSkillInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & level & slot;
	}

	// ����ID
	UInt16      id;
	// ���ܵȼ�
	UInt8       level;
	// ��λ
	UInt8       slot;
};

struct MatchPlayerBaseInfo
{
	MatchPlayerBaseInfo() : id(0), accid(0), serverCreateTime(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & accid & name & serverCreateTime;
	}

	ObjID_t         id;
	UInt32          accid;
	std::string     name;
	UInt32			serverCreateTime;
};

struct PkRoomMatchPlayerInfo
{
	PkRoomMatchPlayerInfo()
	{
		//memset(this, 0, sizeof(*this));
		id = 0;
		wsId = 0;
		accid = 0;
		score = 0;
		level = 0;
		winStreak = 0;
		loseStreak = 0;
		seasonLevel = 0;
		seasonStar = 0;
		seasonExp = 0;
		seat = 0;
		serverCreateTime = 0;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & wsId & accid & name & score & level & winStreak & loseStreak & seasonLevel & seasonStar & seasonExp & seat & serverCreateTime;
	}
	ObjID_t         id;
	UInt32			wsId;
	UInt32          accid;
	std::string     name;
	UInt32			score;
	UInt16			level;
	// ��ʤ��
	UInt32			winStreak;
	// ������
	UInt32			loseStreak;
	// ��λ
	UInt32			seasonLevel;
	// ��������
	UInt32			seasonStar;
	// ��������
	UInt32			seasonExp;
	// ��λ��
	UInt8			seat;
	// ����ʱ��
	UInt32			serverCreateTime;
};

// �����е������Ϣ
struct RacePlayerInfo
{
	RacePlayerInfo() : zoneId(0), robotAIType(0), robotHard(0), roleId(0), accid(0), occu(0), level(0), pkValue(0), pos(0), remainHp(HP_MP_FULL_VALUE), remainMp(HP_MP_FULL_VALUE), seasonLevel(0), seasonStar(0), seasonAttr(0), monthcard(0){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & zoneId & serverName & robotAIType & robotHard & roleId & accid & name & guildName & occu & level & pkValue & matchScore & pos & remainHp & remainMp & seasonLevel & seasonStar & seasonAttr & monthcard & skills &
			equips & raceItems & buffs & warpStones & retinues & pets & potionPos & secondWeapons & avatar & playerLabelInfo & equipScheme & wearingEqIds
			& lostDungArtifacts;
	}
	// ������ID
	UInt32 zoneId;
	// ��������
	std::string serverName;
	// RobotAI�Ѷȣ���Ӧö��RobotAIHardType��,�������Чֵ��˵�����ǻ�����
	UInt8			robotAIType;
	// �ڼ��δ�(0������Ѫ������ֶ����ڻ��������ϣ������ǻ������Ѷȣ�Ѫ����ͬ��)
	UInt8			robotHard;
	//��ɫid
	ObjID_t			roleId;
	//�˺�id
	UInt32          accid;
	//����
	std::string		name;
	//������
	std::string		guildName;
	//ְҵ
	UInt8			occu;
	//�ȼ�
	UInt16			level;
	//PKֵ
	UInt32          pkValue;
	//ƥ�����
	UInt32			matchScore;
	//λ��
	UInt8			pos;
	// ʣ��Ѫ��
	UInt32			remainHp;
	// ʣ������
	UInt32			remainMp;
	// ������λ
	UInt32			seasonLevel;
	// ��������
	UInt32			seasonStar;
	// ��������
	UInt8			seasonAttr;
	// �ǲ����¿��û�
	UInt8			monthcard;

	// �����б�
	std::vector<RaceSkillInfo>  skills;

	// װ���б�
	std::vector<RaceEquip>      equips;

	// ���ֵ���
	std::vector<RaceItem>		raceItems;
	// buff
	std::vector<RaceBuffInfo>	buffs;

	//��Ԫʯ
	std::vector<RaceWarpStone>	warpStones;
	//���
	std::vector<RaceRetinue>	retinues;
	//����
	std::vector<RacePet>		pets;
	//ҩƷ����
	std::vector<UInt32>			potionPos;
	//����������
	std::vector<RaceEquip>		secondWeapons;
	//���
	PlayerAvatar				avatar;
	//��ұ�ǩ��Ϣ
	PlayerLabelInfo				playerLabelInfo;
	//װ��������Ϣ
	std::vector<RaceEquipScheme>	equipScheme;
	//��ǰ����װ��,ʱװid
	std::vector<UInt64>			wearingEqIds;
	// ��������
	std::vector<RaceItem>		lostDungArtifacts;
};

// ���³������Ϣ
struct DungeonRacePlayerInfo
{
	DungeonRacePlayerInfo()
	{
		accId = 0;
		usedTime = 0;
		remainHp = 0;
		remainMp = 0;
		freeGoldChest = 0;
		isAssist = 0;
		addedMonsterDropNum = 0;
		soureSceneId = 0;
		vipLvl = 0;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & accId & zoneId & raceInfo & taskList & addition & dailyCount & totalCount & freeGoldChest & addedMonsterDropNum
			& usedTime & remainHp & remainMp & isAssist & soureSceneId & vipLvl;
	}

	// accid
	UInt32						accId;
	// zoneid
	UInt32						zoneId;
	// ս����Ϣ
	RacePlayerInfo				raceInfo;
	// ��ǰ�����б�
	std::vector<UInt32>			taskList;
	// ���ּӳ�
	DungeonAddition				addition;
	// ���յڼ��δ�
	UInt32						dailyCount;
	// �Ѿ�����Ĵ���
	UInt32						totalCount;
	// �ܷ���Ѵ򿪻ƽ���
	UInt8						freeGoldChest;
	// ������ӵĹ���������
	UInt32						addedMonsterDropNum;

	// ������һ�εĽ���ʱ��Ҫ������
	// �Ѿ����˵�ʱ��
	UInt32						usedTime;
	// ʣ��Ѫ��
	UInt32						remainHp;
	// ʣ������
	UInt32						remainMp;
	// �Ƿ�����ս
	UInt8						isAssist;
	// ���ԴsceneId
	UInt32						soureSceneId;
	// ���vip
	UInt8						vipLvl;
};

#endif
