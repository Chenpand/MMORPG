#ifndef _CL_GUILD_DEFINE_H_
#define _CL_GUILD_DEFINE_H_

#include <CLDefine.h>
#include <AvalonNetStream.h>
#include <CLGameDefine.h>
#include <CLSortListDefine.h>
#include <CLItemDefine.h>

#define GUILD_BATTLE_REWARD_SIZE 64

#define GUILD_BATTLE_ACTIVITY_NAME "GuildBattle"
#define GUILD_BATTLE_ENROLL_ACTIVITY_NAME "GuildBattleEnroll"
#define GUILD_BATTLE_STATUS_KEY "guild_battle_status"
#define GUILD_BATTLE_CHALLENGE_PRICE "guild_battle_challenge_price"
#define GUILD_BATTLE_CHALLENGE_PLAYER "guild_battle_challenge_player"

//����ս�齱���������Сֵ
const static UInt8 GUILD_BATTLE_REWARD_PROBABILITY_MIN = 0;
const static UInt8 GUILD_BATTLE_REWARD_PROBABILITY_MAX = 100;

//�����˳������´μ��빫�����ȴʱ��
const static UInt32	GUILD_ENTER_COLDDOWN_LEAVE = 2 * 60 * 60;
//���߳������´μ��빫�����ȴʱ��
const static UInt32	GUILD_ENTER_COLDDOWN_KICK = 2 * 60 * 60;
//����һ�CD
const static UInt32	GUILD_EXCHANGE_COLDDOWN = 30 * 60;
//�����ɢ�ȴ�ʱ��
const static UInt32	GUILD_DISSMISS_WAIT_TIME = 24 * 60 * 60;
//����������(���)
const static UInt32 GUILD_NAME_MAX_WIDTH = 14;
//���ṫ�泤��(���)
const static UInt32 GUILD_ANNOUNCEMENT_MAX_WIDTH = 200;
//�������Գ���(���)
const static UInt32 GUILD_DECLARATION_MAX_WIDTH = 100;
//�����ʼ�����(���)
const static UInt32 GUILD_MAIL_MAX_WIDTH = 200;
// ������������
const static UInt32 GUILD_AUCTION_COIN = 600000002;


//�����¼�: װ��ǿ���ɹ��ȼ� >= 12
const static UInt32  GUILD_EVENT_STRENGTH_LEVEL_SUCCESS = 12;
//�����¼�: װ��ǿ��ʧ�ܵȼ� >= 13
const static UInt32  GUILD_EVENT_STRENGTH_LEVEL_FAIL = 13;

//�����¼�: װ�������ɹ��ȼ� >= 12
const static UInt32  GUILD_EVENT_ENHANCE_LEVEL_SUCCESS = 12;
//�����¼�: װ������ʧ�ܵȼ� >= 13
const static UInt32  GUILD_EVENT_ENHANCE_LEVEL_FAIL = 12;

//�����¼�: �����ȼ����͵ȼ�
const static UInt32  GUILD_EVENT_HONOR_LEVEL = 9;

/**
*@brief ��������
*/
enum GuildAttr
{
	GA_INVALID,				// ��Ч����
	GA_NAME,				// ����	string
	GA_LEVEL,				// �ȼ�	UInt8
	GA_DECLARATION,			// ���� string
	GA_FUND,				// �����ʽ� UInt32
	GA_ANNOUNCEMENT,		// ���� string
	GA_BUILDING,			// ���Ὠ�� GuildBuilding
	GA_DISMISS_TIME,		// ��ɢʱ�� UInt32
	GA_MEMBER_NUM,			// ��Ա���� UInt16
	GA_LEADER_NAME,			// �᳤���� string
	GA_ENROLL_TERRID,		// �������ID UInt8
	GA_BATTLE_SCORE,		// ����ս���� UInt32
	GA_OCCUPY_TERRID,		// ����ռ����� UInt8
	GA_INSPIRE,				// ����ս������� UInt8
	GA_WIN_PROBABILITY,		// ����սʤ���齱���� UInt8
	GA_LOSE_PROBABILITY,	// ����սʧ�ܳ齱���� UInt8
	GA_STORAGE_ADD_POST,	// ����ս�ֿ������Ʒ UInt8
	GA_STORAGE_DEL_POST,	// ����ս�ֿ�ɾ����Ʒ UInt8
	GA_OCCUPY_CROSS_TERRID,	// ����ռ������� UInt8
	GA_HISTORY_TERRID,		// ������ʷռ����� UInt8
	GA_JOIN_LEVEL,			// ���빫��ȼ� UInt32
	GA_DUNGEON_TYPE,		// ���ḱ���Ѷ� UInt32
	GA_LAST_WEEK_ADD_FUND,  // �������ӵķ��ٶ� UInt32
	GA_THIS_WEEK_ADD_FUND,  // �������ӵķ��ٶ� UInt32
};

// ����ְ��
enum GuildPost
{
	GUILD_POST_INVALID = 0,
	// ��Ա
	GUILD_MEMBER_BEGIN = 0,	
	GUILD_POST_NORMAL = 1,		// ��ͨ��Ա
	GUILD_POST_ELITE = 2,		// ��Ӣ
	GUILD_MEMBER_END,
	// �����
	GUILD_MANAGER_BEGIN = 10,			
	GUILD_POST_ELDER = 11,		// ����
	GUILD_POST_ASSISTANT = 12,	// ���᳤
	GUILD_POST_LEADER = 13,		// �᳤
	GUILD_MANAGER_END,

};

/**
*@brief ����Ȩ������
*/
enum GuildPowerFlag
{
	GUILD_POWER_SETMEMBER					= 1 << 0,		// ������Ա
	GUILD_POWER_SETPOST						= 1 << 1,		// ����ְλ
	GUILD_POWER_KICK						= 1 << 2,		// ������Ա
	GUILD_POWER_CHANGE_DECLAR				= 1 << 3,		// �޸�����
	GUILD_POWER_UPDATE_BUILDING				= 1 << 4,		// ��������
	GUILD_POWER_CHANGE_NAME					= 1 << 5,		// �޸Ĺ�����
	GUILD_POWER_CHANGE_ANNOUNCE				= 1 << 6,		// �޸Ĺ��ṫ��
	GUILD_POWER_MAIL						= 1 << 7,		// �����ʼ�
	GUILD_POWER_DISMISS						= 1 << 8,		// ��ɢ���
	GUILD_SET_JOIN_LEVEL					= 1 << 9,		// ���ü���ȼ�
	GUILD_SET_DUNGEON_BOSS_TYPE				= 1 << 10,		// ���ù��ḱ���Ѷ�

	// ��ְͬ���Ȩ��
	GUILD_POWER_NORMAL	= 0,		// ��ͨ��Ա
	GUILD_POWER_ELITE = 0,			// ��Ӣ
	GUILD_POWER_ELDER = GUILD_POWER_SETMEMBER | GUILD_POWER_KICK | GUILD_POWER_MAIL | GUILD_SET_JOIN_LEVEL,
	// ���᳤
	GUILD_POWER_ASSISTANT = GUILD_POWER_ELDER | GUILD_POWER_CHANGE_DECLAR | GUILD_POWER_SETPOST | GUILD_POWER_CHANGE_NAME | GUILD_POWER_CHANGE_ANNOUNCE | GUILD_POWER_MAIL | GUILD_SET_JOIN_LEVEL | GUILD_SET_DUNGEON_BOSS_TYPE,
	// �᳤
	GUILD_POWER_LEADER = GUILD_POWER_ASSISTANT | GUILD_POWER_DISMISS | GUILD_POWER_UPDATE_BUILDING | GUILD_SET_JOIN_LEVEL | GUILD_SET_DUNGEON_BOSS_TYPE,
};

//����ֿ���������
enum GuildStorageSetting
{
	GUILD_STORAGE_SETTING_INVALID = 0,
	//ʤ���齱����
	GSS_WIN_PROBABILITY = 1,
	//ʧ�ܳ齱����
	GSS_LOSE_PROBABILITY = 2,
	//�ֿ�����Ȩ��
	GSS_STORAGE_ADD_POST = 3,
	//�ֿ�ɾ��Ȩ��
	GSS_STORAGE_DEL_POST = 4,
	GSS_MAX,
};

//�����Ա�齱״̬
enum GuildBattleLotteryStatus
{
	// ��Ч
	GBLS_INVALID = 0,
	// ���ܳ齱
	GBLS_NOT = 1,
	// ���Գ齱
	GBLS_CAN = 2,
	// �Ѿ��齱
	GBLS_FIN = 3,

	GBLS_MAX,
};

/**
*@brief ��ȡְλ��Ȩ��
*/
inline UInt32 GetGuildPostPower(GuildPost post)
{
	switch (post)
	{
	case GUILD_POST_INVALID:
	case GUILD_POST_NORMAL:
		return GUILD_POWER_NORMAL;
	case GUILD_POST_ELITE:
		return GUILD_POWER_ELITE;
	case GUILD_POST_ELDER:
		return GUILD_POWER_ELDER;
	case GUILD_POST_ASSISTANT:
		return GUILD_POWER_ASSISTANT;
	case GUILD_POST_LEADER:
		return GUILD_POWER_LEADER;
	default:
		return GUILD_POWER_NORMAL;
	}
	return GUILD_POWER_NORMAL;
}

inline bool IsEnrollGuildBattlePost(GuildPost post)
{
	return (post == GUILD_POST_ASSISTANT) || (post == GUILD_POST_LEADER);
}

inline bool HasGuildPower(GuildPost post, GuildPowerFlag power)
{
	return (GetGuildPostPower(post) & power) != 0;
}

struct GuildEntry
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & name & level & memberNum & leaderName & declaration & isRequested 
			& occupyCrossTerrId & occupyTerrId & joinLevel & leaderId;
	}

	// id
	ObjID_t			id;
	// name
	std::string		name;
	// ����ȼ�
	UInt8			level;
	// ��������
	UInt8			memberNum;
	// �᳤����
	std::string		leaderName;
	// ����
	std::string		declaration;
	// �Ƿ��Ѿ�����
	UInt8			isRequested;
	// ������
	UInt8			occupyCrossTerrId;
	// �������
	UInt8			occupyTerrId;
	// ���ȼ�
	UInt32			joinLevel;
	// �᳤id
	UInt64			leaderId;
};

// �����Ա
struct GuildMemberEntry
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & name & level & occu & post & contribution & logoutTime & activeDegree & vipLevel 
			& seasonLevel  & playerLabelInfo;
	}

	// id
	ObjID_t			id;
	// name
	std::string		name;
	// �ȼ�
	UInt16			level;
	// ְҵ
	UInt8			occu;
	// ְ��(��Ӧö��GuildPost)
	UInt8			post;
	// ��ʷ����
	UInt32			contribution;
	// ����ʱ��(0��������)
	UInt32			logoutTime;
	// ��Ծ��
	UInt32			activeDegree;
	// vip�ȼ�
	UInt8			vipLevel;
	// ��Ҷ�λ
	UInt32			seasonLevel;
	//��ұ�ǩ��Ϣ
	PlayerLabelInfo playerLabelInfo;
};

struct GuildRequesterInfo
{
public:
	GuildRequesterInfo() : id(0), level(0), occu(0), requestTime(0), seasonLevel(0){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & name & level & occu & vipLevel & requestTime & seasonLevel & playerLabelInfo;
	}
	//id
	ObjID_t		id;
	//����
	std::string	name;
	//�ȼ�
	UInt16		level;
	//ְҵ
	UInt8		occu;
	//vip�ȼ�
	UInt8		vipLevel;
	//����ʱ��
	UInt32		requestTime;
	// ��Ҷ�λ
	UInt32		seasonLevel;
	//��ұ�ǩ��Ϣ
	PlayerLabelInfo playerLabelInfo;
};
typedef std::map<ObjID_t, GuildRequesterInfo> MapGuildRequesterInfo;

// ����ս����
enum GuildBattleType
{
	//��Ч
	GBT_INVALID,

	//��ͨ
	GBT_NORMAL,
	//��ս
	GBT_CHALLENGE,
	//���
	GBT_CROSS,

	GBT_MAX,
};

//����ս�������
enum GuildTerritoryType
{
	//��Ч
	GTT_INVALID,

	//��ͨ
	GTT_NORMAL,
	//���
	GTT_CROSS,
	
	GTT_MAX
};

// ����ս״̬
enum GuildBattleStatus
{
	//��Ч
	GBS_INVALID,
	//����
	GBS_ENROLL,
	//׼��
	GBS_PREPARE,
	//ս��
	GBS_BATTLE,
	//�콱
	GBS_REWARD,

	GBS_MAX,
};

// ���Ὠ������
enum GuildBuildingType
{
	GUILD_BUILDING_MAIN = 0,	// ����
	GUILD_BUILDING_SHOP = 1,	// �̵�
	GUILD_BUILDING_TABLE = 2,	// Բ������
	GUILD_BUILDING_DUNGEON = 3,	// ���³�
	GUILD_BUILDING_STATUE = 4,	// ����
	GUILD_BUILDING_BATTLE = 5,	// ս����
	GUILD_BUILDING_WELFARE = 6,	// ������
	GUILD_BUILDING_HONOUR = 7,	// ��ҫʥ��
	GUILD_BUILDING_ALTAR = 8,	// ս����̳
	GUILD_BUILDING_HONOR = 9,	// ��������
	GUILD_BUILDING_NUM,
};

// ���Ὠ��
struct GuildBuilding
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & type & level;
	}

	// ��������(��Ӧö��GuildBuildingType)
	UInt8				type;
	// �ȼ�
	UInt8				level;
};

// Բ�������Ա��Ϣ
struct GuildTableMember
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & level & occu & name & seat & type & playerLabelInfo;
	}
	// ��ɫID
	ObjID_t		id;
	// �ȼ�
	UInt16		level;
	// ְҵ
	UInt8		occu;
	// ����
	std::string	name;
	// λ��
	UInt8		seat;
	// ��������
	UInt8		type;
	//��ұ�ǩ��Ϣ
	PlayerLabelInfo playerLabelInfo;
};


// ����ս��Ա
struct GuildBattleMember
{
	GuildBattleMember() :id(0), winStreak(0), gotScore(0), totalScore(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & name & serverName & guildName & winStreak & gotScore & totalScore;
	}

	// ID
	ObjID_t			id;
	// ����
	std::string		name;
	// ����������
	std::string		serverName;
	// ��������
	std::string		guildName;
	// ��:ս��ǰ����ʤ�� Ӯ:ս������ʤ
	UInt8			winStreak;
	// ��û���
	UInt16			gotScore;
	//�ܻ���
	UInt16			totalScore;
};

// ����ս��¼
struct GuildBattleRecord
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & index & winner & loser & time;
	}

	UInt32				index;
	// ʤ����
	GuildBattleMember	winner;
	// ʧ����
	GuildBattleMember	loser;
	// ʱ��
	UInt32				time;
};

//�����Ϣ
struct GuildTerritoryBaseInfo
{
	GuildTerritoryBaseInfo() :terrId(0), enrollSize(0){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & terrId & serverName & guildName & enrollSize;
	}

	//���ID
	UInt8 terrId;
	//����������
	std::string serverName;
	//��������
	std::string guildName;
	//��������
	UInt32 enrollSize;

};

struct GuildBattleBaseInfo
{
	GuildBattleBaseInfo()
	{
		enrollTerrId = 0;
		guildBattleScore = 0;
		occupyTerrId = 0;
		occupyCrossTerrId = 0;
		historyTerrId = 0;
		inspire = 0;
		guildBattleType = (UInt8)GuildBattleType::GBT_INVALID;
		guildBattleStatus = (UInt8)GuildBattleStatus::GBS_INVALID;
		guildBattleStatusEndTime = 0;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & enrollTerrId & guildBattleScore & occupyTerrId & occupyCrossTerrId & historyTerrId & inspire
			& selfGuildBattleRecord & terrInfos & guildBattleType & guildBattleStatus & guildBattleStatusEndTime;
	}

	// �������ID
	UInt8			enrollTerrId;
	// ����ս����
	UInt32			guildBattleScore;
	// �Ѿ�ռ������ID
	UInt8			occupyTerrId;
	// �Ѿ�ռ��Ŀ�����ID
	UInt8			occupyCrossTerrId;
	// ��ʷռ������ID
	UInt8			historyTerrId;
	// �������
	UInt8			inspire;

	//�Լ��Ĺ���ս��¼
	std::vector<GuildBattleRecord> selfGuildBattleRecord;
	//����ս�����Ϣ
	std::vector<GuildTerritoryBaseInfo> terrInfos;
	//����ս����
	UInt8 guildBattleType;
	//����ս״̬
	UInt8 guildBattleStatus;
	//����ս״̬����ʱ��
	UInt32 guildBattleStatusEndTime;
};

// ���������Ϣ
struct GuildBaseInfo
{
	GuildBaseInfo()
	{
		id = 0;
		level = 0;
		fund = 0;
		dismissTime = 0;
		memberNum = 0;
		joinLevel = 0;
		emblemLevel = 0;
		guildDungeonType = 0;
		weekAddedFund = 0;
		lastWeekAddedFund = 0;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & name & level & fund & declaration & announcement & dismissTime & memberNum 
			& leaderName & winProbability & loseProbability & storageAddPost & storageDelPost & buildings & hasRequester & tableMembers & guildBattleInfo & joinLevel & emblemLevel & guildDungeonType & weekAddedFund & lastWeekAddedFund;
	}

	// ����ID
	ObjID_t			id;
	// ������
	std::string		name;
	// ����ȼ�
	UInt8			level;
	// �����ʽ�
	UInt32			fund;
	// ��������
	std::string		declaration;
	// ����
	std::string		announcement;
	// ��ɢʱ��
	UInt32			dismissTime;
	// ��Ա����
	UInt16			memberNum;
	// �᳤����
	std::string		leaderName;
	// ����սʤ���齱����
	UInt8			winProbability;
	// ����սʧ�ܳ齱����
	UInt8			loseProbability;
	// ����ֿ����Ȩ��
	UInt8			storageAddPost;
	// ����ֿ����Ȩ��
	UInt8			storageDelPost;
	// ������Ϣ
	std::vector<GuildBuilding> buildings;
	// ��û��������빫�����
	UInt8			hasRequester;
	// Բ�������Ա��Ϣ
	std::vector<GuildTableMember> tableMembers;
	//����ս�����Ϣ
	GuildBattleBaseInfo guildBattleInfo;
	// ������Ƶȼ�
	UInt32			joinLevel;
	// �ռ�
	UInt32			emblemLevel;
	// ���ḱ��boss�Ѷ�
	UInt32			guildDungeonType;
	//���᱾�����ӵķ��ٶ�
	UInt32       weekAddedFund;
	//�����������ӵķ��ٶ�
	UInt32       lastWeekAddedFund;
};

// �����������
enum GuildOperation
{
	GUILD_OP_MODIFY_DECLAR,		// �޸Ĺ�������
	GUILD_OP_MODIFY_NAME,		// �޸Ĺ�����
	GUILD_OP_MODIFY_ANNOUNCE,	// �޸Ĺ��ṫ��
	GUILD_OP_SEND_MAIL,			// ���͹����ʼ�
	GUILD_OP_UPGRADE_BUILDING,	// ��������
	GUILD_OP_DONATE,			// ����
	GUILD_OP_EXCHANGE,			// �һ�
	GUILD_OP_UPGRADE_SKILL,		// ��������
	GUILD_OP_DISMISS,			// ��ɢ����
	GUILD_OP_CANCEL_DISMISS,	// ȡ����ɢ����
	GUILD_OP_ORZ,				// Ĥ��
	GUILD_OP_TABLE,				// Բ������
	GUILD_OP_PAY_REDPACKET,		// �����ԷѺ��
};

// ����
enum GuildDonateType
{
	GUILD_DONATE_GOLD,		// ��Ҿ���
	GUILD_DONATE_POINT,		// �ㄻ����
	GUILD_DONATE_NUM,
};

enum GuildBattleMemberStatus
{
	GBS_NORMAL,		//����״̬
	GBS_START,		//��ƥ�����ս��
};

// ������־
struct GuildDonateLog
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & name & type & num & contri;
	}
	// id
	ObjID_t		id;
	// ����
	std::string name;
	// �������ͣ���Ӧö��GuildDonateType��
	UInt8		type;
	// ����
	UInt8		num;
	// ��ù���
	UInt32		contri;
};

// ����᳤��Ϣ
struct GuildLeaderInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & name & occu & avatar & popularoty & playerLabelInfo;
	}

	// ID
	ObjID_t			id;
	// ����
	std::string		name;
	// ְҵ
	UInt8			occu;
	// ���
	PlayerAvatar	avatar;
	// ����
	UInt32			popularoty;
	//��ұ�ǩ��Ϣ
	PlayerLabelInfo playerLabelInfo;
};

// Ĥ������
enum GuildOrzType
{
	// ��ͨĤ��
	GUILD_ORZ_LOW,
	// �м�Ĥ��
	GUILD_ORZ_MID,
	// �߼�Ĥ��
	GUILD_ORZ_HIGH,
	GUILD_ROZ_NUM,
};

struct GuildBattleEndInfo
{
	GuildBattleEndInfo() :terrId(0), terrName(""), guildId(0), guildName(""), guildLeaderName(""), serverName(""){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & terrId & terrName & guildId & guildName & guildLeaderName & serverName;
	}

	UInt8 terrId;
	std::string terrName;
	ObjID_t guildId;
	std::string guildName;
	std::string guildLeaderName;
	std::string serverName;
};

//�������ս������Ϣ
struct CSEnrollInfo
{
	CSEnrollInfo() :terrId(0), guildId(0), guildName(""), guildLeaderName(""), serverName(""){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & terrId & guildId & guildName & guildLeaderName & serverName;
	}

	inline bool operator==(const CSEnrollInfo &info)
	{
		return guildId == info.guildId;
	}

	UInt8 terrId;
	ObjID_t guildId;
	std::string guildName;
	std::string guildLeaderName;
	std::string serverName;
};

//����ս������Ϣ
struct GuildBattleInspireInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & playerId & playerName & inspireNum;
	}

	//���ID
	ObjID_t playerId;
	//�������
	std::string playerName;
	//�������
	UInt32 inspireNum;

};

struct GuildBattleTimeInfo
{
	GuildBattleTimeInfo() :type(GuildBattleType::GBT_INVALID), enrollTime(0), preTime(0), battleTime(0), rewardTime(0), endTime(0){}

	GuildBattleType type;
	UInt32 enrollTime;
	UInt32 preTime;
	UInt32 battleTime;
	UInt32 rewardTime;
	UInt32 endTime;
};

struct GuildStorageItemInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & uid & dataId & num & str & equipType;
	}

	void DecodeString(std::istringstream& is)
	{
		char split = 0;
		UInt16 str16;
		is >> dataId >> split >> num >> split >> str16;
		str = (UInt8)str16;
	}
	void EncodeString(std::ostringstream& os, char split = '_') const
	{
		os << dataId << split << num << split << (UInt32)str;
	}

	UInt64	uid;
	UInt32	dataId;
	UInt16	num;
	UInt8	str;
	UInt8   equipType;
};

struct GuildStorageDelItemInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & uid & num;
	}

	UInt64	uid;
	UInt16	num;
};

struct GuildStorageOpRecord
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & name & opType & items & time;
	}

	UInt64				guid;
	std::string			name;
	UInt32				opType;					// GuildStorageOpType
	std::vector<GuildStorageItemInfo>	items;
	UInt32				time;
};


struct TerritorySortListEntry
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & sortType & serverName & playerId & playerName & guildId & guildName & score;
	}

	UInt8			sortType;
	std::string		serverName;
	UInt64			playerId;
	std::string		playerName;
	UInt64			guildId;
	std::string		guildName;
	UInt32			score;

	void SerializeForClient(Avalon::NetOutputStream& stream)
	{
		if (IsGuildSortList(sortType))
			stream & guildId & guildName & serverName & score;
		else
			stream & playerId & playerName & serverName & score & guildName;
	}

	UInt64 GetSortFactor() const
	{
		return GetSortFactor(sortType, playerId, score);
	}

	static UInt64 GetSortFactor(UInt8 sortType, UInt64 id, UInt32 score)
	{
		switch ((SortListType)sortType)
		{
		case SORTLIST_GUILD_BATTLE_SCORE:return score;
		case SORTLIST_GUILD_BATTLE_MEMBER:return score;
		case SORTLIST_GUILD_MEMBER_SCORE:return score;
		default: return score;
		}
	}
};

//********************������³�*********************

// �����buff��Ϣ
struct GuildDungeonBuff
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & buffId & buffLvl;
	}
	// id
	UInt32 buffId;
	// �ȼ�
	UInt32 buffLvl;

	GuildDungeonBuff() : buffId(0), buffLvl(0) {}
};


struct GuildDungeonBattleRecord
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & dungeonId & battleCnt & oddBlood & buffVec;
	}

	// ���³�id
	UInt32 dungeonId;
	// ս������
	UInt32 battleCnt;
	// ʣ��Ѫ��
	UInt64 oddBlood;
	// buff�б�
	std::vector<GuildDungeonBuff> buffVec;

	GuildDungeonBattleRecord() : dungeonId(0), battleCnt(0), oddBlood(0){}
};

typedef std::map<UInt32, GuildDungeonBattleRecord*> GuildDungeonBattleMap;

// �˺�
struct GuildDungeonPlayerDamage
{
	// ���ID
	UInt64 playerId;
	// �˺�ֵ
	UInt64 damagePoint;
	// �������
	std::string playerName;

	GuildDungeonPlayerDamage() : playerId(0), damagePoint(0) {}

	bool operator < (const GuildDungeonPlayerDamage& info) const
	{
		return damagePoint < info.damagePoint;
	}
};

typedef std::vector<GuildDungeonPlayerDamage*> GuildDungeonDamageVec;

// ������³ǽ���
struct GuildDungeonReward
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & itemId & itemNum & isHighVal;
	}

	// ����Id
	UInt32 itemId;
	// ��������
	UInt32 itemNum;
	// �Ƿ�߼�ֵ
	UInt32 isHighVal;

	GuildDungeonReward() : itemId(0), itemNum(0), isHighVal(0) {}
};

typedef std::vector<GuildDungeonReward> GuildDungeonRewardVec;
typedef std::map<UInt32, GuildDungeonReward> GuildDungeonRewardMap;

struct GuildDungeonGateTime
{
	GuildDungeonGateTime() : guildId(0), spendTime(0) {}

	// ��������
	UInt64 guildId;
	// ͨ����ʱ
	UInt64 spendTime;
};

enum GuildAuctionType
{
	// ��Чֵ
	G_AUCTION_INVALID = 0,
	// ������
	G_AUCTION_GUILD = 1,
	// ������
	G_AUCTION_WORLD = 2,
};

enum GuildAuctionItemState
{
	// ��Чֵ
	GAI_STATE_INVALID = 0,
	// ����׼��
	GAI_STATE_PREPARE = 1,
	// ������
	GAI_STATE_NORMAL = 2,
	// �ɽ�
	GAI_STATE_DEAL = 3,
	// ����
	GAI_STATE_ABORATION = 4,
};

struct GuildAuctionItem
{
	GuildAuctionItem() : guildId(0), rewardGroup(0), bidNum(0), guid(0), curPrice(0), bidPrice(0), fixPrice(0),
		addPrice(0), state(0), endTime(0), auctionType(0), bidRoleId(0), bidTime(0), vipLevel(0), level(0), occu(0), isBonus(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & guid & bidRoleId & curPrice & bidPrice & fixPrice & endTime & state & auctionRewardVec;
	}

	std::string SerializeToString()
	{
		std::stringstream ss;
		ss << " guid:" << guid;
		ss << ", rewardGroup:" << rewardGroup;
		ss << ", bidNum:" << bidNum;
		ss << ", guildId:" << guildId;
		ss << ", curPrice:" << curPrice;
		ss << ", bidPrice:" << bidPrice;
		ss << ", fixPrice:" << fixPrice;
		ss << ", state:" << state;
		ss << ", endTime:" << endTime;
		ss << ", auctionType:" << auctionType;
		ss << ", bidRoleId:" << bidRoleId;
		ss << ", bidTime:" << bidTime;
		ss << ", vipLevel:" << vipLevel;
		ss << ", level:" << level;
		ss << ", occu:" << occu;
		ss << ", isBonus:" << isBonus;

		ss << ", Reward( ";
		for (auto& iter : auctionRewardVec)
		{
			ss << " id:" << iter.id << ", num:" << iter.num;
		}
		ss << " )";
		return ss.str();
	};

	// ����id
	UInt64 guildId;
	// ������
	UInt32 rewardGroup;
	// ���۴���
	UInt32 bidNum;
	// guid
	UInt64 guid;
	// ��ǰ����
	UInt32 curPrice;
	// ���ļ�
	UInt32 bidPrice;
	// һ�ڼ�
	UInt32 fixPrice;
	// �Ӽ�
	UInt32 addPrice;
	// ��Ʒ״̬
	UInt32 state;
	// ��������ʱ��
	UInt32 endTime;
	// ��������
	UInt32 auctionType;
	// ������
	UInt64 bidRoleId;
	// ����ʱ��
	UInt32 bidTime;
	// vip
	UInt32 vipLevel;
	// �ȼ�
	UInt32 level;
	// ְҵ
	UInt32 occu;
	// �Ƿ�ֳ�
	UInt32 isBonus;
	// ������Ʒ
	ItemRewardVec auctionRewardVec;
};

/**
 *@brief  �����¼���Ϣ
 */
struct GuildEvent
{

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & eventInfo & addTime;
	}

	//Ψһid
	UInt64			guid;

	//�¼���Ϣ
	std::string		eventInfo;

	//����ʱ��
	UInt32			addTime;
};
struct GuildHonor
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & groupId & cnt & time;
	}
	//����id
	UInt32 id;
	//������id
	UInt32 groupId;
	//��ɴ���
	UInt32 cnt;
	//���ʱ��
	UInt32 time;
};

enum GuildProsperityType
{
	GUILD_PROSPERITY_NULL = 0,
	GUILD_PROSPERITY_DISMISS = 1,
	GUILD_PROSPERITY_LOW = 2,
	GUILD_PROSPERITY_MID = 3,
	GUILD_PROSPERITY_HIGH = 4,
	GUILD_PROSPERITY_IN_DISMISS = 5,
	GUILD_PROSPERITY_IN_MERGER = 6,
	GUILD_PROSPERITY_IN_BY_MERGER = 7,
};

enum GuildMergerRequestStatus
{
	GUILD_MERGER_REQUEST_NULL = 0,
	GUILD_MERGER_REQUEST_SEND = 1,
	GUILD_MERGER_REQUEST_ACCEPTED = 2,
	GUILD_MERGER_REQUEST_REFUSED = 3,
};

enum GuildDismissType
{
	GUILD_DISMISS_NORMAL = 0,
	GUILD_DISMISS_LOW = 1,
	GUILD_DISMISS_MERGER = 2,
};

//*****************************************


#endif //_CL_GUILD_DEFINE_H_