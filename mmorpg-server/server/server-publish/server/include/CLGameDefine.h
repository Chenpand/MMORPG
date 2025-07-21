#ifndef _CL_GAMEDEFINE_H_
#define _CL_GAMEDEFINE_H_

#include "CLDefine.h"
#include <CLJobDataEntry.h>

//���ֳ���
#define NAME_LENGTH 32
#define NAME_SHOWWIDTH 14

//����ʱ�����Ϸ������
#define PARAM_NAME_GAME_START "gamestart_time"
#define PARAM_NAME_GAME_FIRST_START "game_first_start_time"

//��ҫ����ʱ��
#define HONOR_RANK_TIME "honor_rank_time"

//ȫ�񿳼��ۿ�
#define WHOLE_BARGAIN_DISCOUNT "whole_bargain_discount"

// Ǯ�����
#define MAX_MONEY 1000000000

//���sp
#define MAX_SP 99999

//���ȼ�
#define MAX_LEVEL 65

// �������ɾ���Ľ�ɫ����
const static UInt32 MAX_DELETING_PLAYER_NUM = 3;
// ɾ���Ľ�ɫ�ı���ʱ��
const static UInt32 DELETE_PLAYER_SAVE_TIME = 7 * Avalon::Time::SECS_OF_DAY;
// С�ڸõȼ��Ľ�ɫ��ֱ��ɾ��
const static UInt32 DIRECT_DELETE_PLAYER_LEVEL = 10;

/**
 *@brief �Ա�
 */
enum Sex
{
	SEX_MALE	= 0,	//��
	SEX_FEMALE	= 1,	//Ů
};

inline bool IsValidSex(int sex)
{
	return sex == SEX_MALE || sex == SEX_FEMALE;
}

/**
*@brief ������ɫʱ�ж��Ƿ�Ϸ�
*/
inline bool IsValidOccu(UInt8 mainOccu, UInt8 preOccu, UInt8 isRobot)
{
	JobDataEntry* mainOccuData = JobDataEntryMgr::Instance()->FindEntry(mainOccu);
	if (mainOccuData == NULL) return false;
	JobDataEntry* preOccuData = JobDataEntryMgr::Instance()->FindEntry(preOccu);
	if (preOccuData == NULL) return false;
	if (!mainOccuData->CheckTargetOccu(preOccu)) return false;

	if (mainOccuData->canCreateRole) return true;
	if (isRobot == 2) return true;
	return false;
}

#define TRANSFORM_OCCU_LEVEL 15
#define AWAKEN_ONE_LEVEL	45


/**
 *@brief ��������ϵ��
 */
#define BASEEXP_FACTOR_A 720
#define BASEEXP_FACTOR_B 1440

/**
 *@brief �ͷ����ͣ������ȼ��ӵ͵�������
 */
enum PunishType
{
	PUNISH_FORBIDTALK = 1,		//����
	PUNISH_FORBIDLOGIN = 2,		//��ֹ��½

	PUNISH_MAX = 3,
};

/**
 *@brief gmȨ�޼���
 */
enum GmAuthorityLevel
{
	GM_AUTHORITY_NONE	= 0,		//ûgmȨ��
	GM_AUTHORITY_NORMAL = 10,		//��ͨgmȨ��
	GM_AUTHORITY_SUPER	= 20,		//����gmȨ��
	GM_AUTHORITY_DEBUG	= 30,		//����Ȩ��
};

/**
 *@brief �ͻ��˽�����Ϸѡ��
 */
enum EnterGameOption
{
	ENTERGAME_OPTION_WALLOW	=	1 << 0,	//������ѡ��
};

/**
 *@brief ��ʱ�߼���ʱ���ͣ���󲻳���5����
 */
enum LogicTickType
{
	LOGICTICK_SECOND =	0,	//1��
	LOGICTICK_FIVESEC = 1,	//5��
	LOGICTICK_HALFMIN = 2,	//�����	
	LOGICTICK_MIN	 =	3,	//1����
	LOGICTICK_FIVEMIN = 4,	//5����
	//TODO
	LOGICTICK_MAX,
};
//�߼���ʱ���
const static UInt64	LOGICTICK_INTERVAL[] = { 1000, 5000, 30000, 60000, 300000 };

/**
 *@brief ��������
 */
enum ReviveType
{
	REVIVE_INPOSITION = 0,	//ԭ�ظ���
	REVIVE_NORMAL = 1,		//�سǸ���
	REVIVE_SCENE = 2,		//��������ڸ�����ս����Ч��
	REVIVE_KICKOUT = 3,		//�߳���������
};

/**
 *@brief pkģʽ
 */
enum PkMode
{
	PK_MODE_PEACE = 0,		//��ƽ
	PK_MODE_TEAM =	1,		//���
	PK_MODE_TRIBE = 2,		//����
	PK_MODE_ALL = 3,		//ȫ��
	PK_MODE_EVIL = 4,		//���
	PK_MODE_MAX,
};

/**
 *@brief pk��ɫ
 */
enum PkColor
{
	PK_COLOR_WHITE = 0,		//��ɫ
	PK_COLOR_GREY = 1,		//��ɫ
	PK_COLOR_RED = 2,		//��
	PK_COLOR_DARKRED = 3,	//���
};

/**
 *@brief ����״̬
 */
enum TradeStatus
{
	TRADE_STATUS_UNLOCK = 0,	//δ����
	TRADE_STATUS_LOCK = 1,		//����
	TRADE_STATUS_CONFIRM = 2,	//ȷ��
	TRADE_STATUS_CANCEL = 3,	//ȡ��
};

/**
 *@brief ����������Чʱ��(��)
 */
#define TRADE_INVITE_VALID_TIME (5*60)
/**
 *@brief ���׸�����
 */
#define TRADE_GRID_NUM	7


// ս����������
#define BATTLE_PARAM_NUM 5


//���ط��������͵�udplog��uid
#define UDPLOG_GATE_UID		"gate"
// �������������͵�udplog��uid
#define UDPLOG_SCENE_UID	"scene"
//������������͵�udplog��uid
#define UDPLOG_WORLD_UID	"world"
//Global������͵�udplog��uid
#define UDPLOG_GLOBAL_ACTIVITY_UID	"global_activity"
//Dungeon���������͵�udplog��uid
#define UDPLOG_DUNGEON_UID  "dungeon"
//�ű����������͵�udplog��uid
#define UDPLOG_TEAM_COPY_UID  "team_copy"

//����������
struct EscortAward
{
	EscortAward():exp(0), contribution(0){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & exp & contribution;
	}

	//����
	UInt32	exp;
	//�ﹱ
	UInt32	contribution;
};

/**
 *@brief �����¼�
 */
enum EscortEvent
{
	ESCORTEVENT_KILLED	= 1,	//����Ŀ�걻ɱ
	ESCORTEVENT_FARAWAR = 2,	//Զ��
	ESCORTEVENT_CLOSETO = 3,	//����
	ESCORTEVNET_DIED	= 4,	//����
	ESCORTEVENT_ATTACKED = 5,	//������
	ESCORTEVNET_SUCC =	6,		//���ͳɹ�
};

//���ƺ���
#define MAX_TITLE_NUM 128


//��Ϸϵͳ�¼�����
enum SysEventType
{
	ST_NONE = 0,
	ST_CHARGE,
	ST_LVUP,
	ST_FINISH_DG,
	ST_USE_ITEM,

};


/**
 *@brief ��Ϊ��־
 */
enum ActionFlag
{
	ACTION_FLAG_GOLDBUY = 0,			//�𱴹���
	ACTION_FLAG_BINDGOLDBUY = 1,		//�����
	ACTION_FLAG_KILL = 2,				//Ұ���ͼɱ������
	ACTION_FLAG_KILLED = 3,				//Ұ���ͼ��ɱ��
	ACTION_FLAG_STRENGTHENEQUIP = 4,	//ǿ��װ��
	ACTION_FLAG_XIYUAN = 5,
	ACTION_FLAG_NEWQQVIP = 7,			//��ȡqq����������������
	ACTION_FLAG_POINTBUY = 8,			//��������

	ACTION_FLAG_NEWER = 10,				//�����
	ACTION_FLAG_COUPON_GIFTBAG1	= 11,	//�ֿ�ȯ���1
	ACTION_FLAG_COUPON_GIFTBAG2 = 12,	//�ֿ�ȯ���2
	ACTION_FLAG_COUPON_GIFTBAG3 = 13,	//�ֿ�ȯ���3
	ACTION_FLAG_FASHION_SHOW = 14,		//ʱװ��ʾ 0:����ʾ 1:��ʾ
	ACTION_FLAG_TRANSWOOD = 15,			//����ľ��
	ACTION_FLAG_VIP_TIANWAIYUNTIE = 16,	//vip������������
	ACTION_FLAG_VIP_DIXINYUNTIE = 17,	//vip���͵�������
	ACTION_FLAG_VIP_SHENYUANYUNIIE = 18,//vip������Ԩ����
	ACTION_FLAG_APPONPANEL = 19,		//
	ACTION_FLAG_ROBOT = 20,				//������

	ACTION_FLAG_MAX,
};


/**
 *@brief ���ݵ�½������ȡ���ӽ���
 */
inline UInt32 GetSigninBoxByDays(UInt32 days)
{
	if(days >= 1 && days <= 7) return 10 + days;

	return 11;
}

/**
 *@brief ���ݵ�½������ȡvip���ӽ���
 */
inline UInt32 GetVipSigninBoxByDays(UInt32 days)
{
	if(days >= 1 && days <= 7) return 20 + days;

	return 21;
}

/**
 *@brief ÿ�������־����
 */
enum DailyGiftBagFlag
{
	DAILY_GIFTBAG_ACTIVE1 = 1,			//��Ծ��1
	DAILY_GIFTBAG_ACTIVE2 = 2,			//��Ծ��2
	DAILY_GIFTBAG_ACTIVE3 = 3,			//��Ծ��3
	DAILY_GIFTBAG_ACTIVE4 = 4,			//��Ծ��4
	DAILY_GIFTBAG_SIGNIN	= 11,		//ǩ��
	DAILY_GIFTBAG_VIPGIFTBAG = 12,		//vip���
	DAILY_GIFTBAG_QQVIP	= 13,			//qq����
	DAILY_GIFTBAG_QQVIP_RECOMMEND1 = 14, //QQ�����Ƽ�λ���1
	DAILY_GIFTBAG_QQVIP_RECOMMEND2 = 15, //QQ�����Ƽ�λ���2
	DAILY_GIFTBAG_QQVIP_KLWELCOME1 = 16, //������������
	DAILY_GIFTBAG_QQVIP_KLWELCOME2 = 17, //������������
	DAILY_GIFTBAG_3366 = 18,		

	DAILY_GIFTBAG_INVITE = 20,			//�������
	DAILY_GIFTBAG_INVITEE_ACTIVE1 = 21,	//�������߻�Ծ���1
	DAILY_GIFTBAG_INVITEE_ACTIVE2 = 22,	//�������߻�Ծ���2
	DAILY_GIFTBAG_INVITEE_ACTIVE3 = 23,	//�������߻�Ծ���3
	DAILY_GIFTBAG_INVITEE_ACTIVE4 = 24,	//�������߻�Ծ���4
	DAILY_GIFTBAG_INVITEE_ACTIVE5 = 25,	//�������߻�Ծ���5
};


//����������
#define MAX_ONCE_STORY_NUM 200

//����ܿ�����
#define MAX_FUNCTION_OPEN_NUM 128

//�������������
#define MAX_BOOT_FLAG_NUM 128

//��һ��ս��ID
#define NEW_BOOT_FIRST_BATTLE_ID 1


/**
 *@brief ��ȡ�ֿ�ȯ�����Ʒ��Ӧ��action����                                                                     
 */
inline UInt32 GetCouponMallItemActionFlag(UInt32 mallid)
{
	switch(mallid)
	{
	case 1001: return ACTION_FLAG_COUPON_GIFTBAG1;
	case 1002: return ACTION_FLAG_COUPON_GIFTBAG2;
	case 1003: return ACTION_FLAG_COUPON_GIFTBAG3;
	}
	return 0;
}

/**
 *@brief �ƶ������                                                                     
 */
#define MOVE_CHECK_INTERVAL 5000

/**
*@brief ������������
*/
enum LastActionType
{
	LAT_INVALID = 0,	//��Ч
	LAT_GATHER = 1,		//�ɼ�
	LAT_FISH = 2,		//����
};

// ��������Ϣ
struct PlayerAvatar
{
	PlayerAvatar() : weaponStrengthen(0), isShoWeapon(0) {}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & equipItemIDs & weaponStrengthen & isShoWeapon;
	}

	bool operator!=(const PlayerAvatar& other) const
	{
		return other.equipItemIDs != equipItemIDs || other.weaponStrengthen != weaponStrengthen || other.isShoWeapon != isShoWeapon;
	}

	bool operator==(const PlayerAvatar& other) const
	{
		return !(other != *this);
	}

	std::vector<UInt32>		equipItemIDs;
	// ����ǿ���ȼ�
	UInt8					weaponStrengthen;
	// �Ƿ���ʾʱװ����
	UInt8					isShoWeapon;
};


struct SceneRetinue
{
public:
	SceneRetinue() : m_DataId(0), m_Level(1) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_DataId & m_Level;
	}

	bool operator!=(const SceneRetinue& other) const
	{
		return m_DataId != other.m_DataId || m_Level != other.m_Level;
	}

	bool operator==(const SceneRetinue& other) const
	{
		return m_DataId == other.m_DataId && m_Level == other.m_Level;
	}

	bool operator=(const SceneRetinue& other)
	{
		m_DataId = other.m_DataId;
		m_Level = other.m_Level;
		return true;
	}

	//��ӱ�ID
	UInt32 m_DataId;

	//�ȼ�
	UInt8 m_Level;
};

struct ScenePet
{
public:
	ScenePet() :m_Id(0), m_DataId(0), m_Level(1){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_Id & m_DataId & m_Level;
	}

	bool operator!=(const ScenePet& other) const
	{
		return m_Id != other.m_Id || m_DataId != other.m_DataId || m_Level != other.m_Level;
	}

	bool operator==(const ScenePet& other) const
	{
		return m_Id == other.m_Id && m_DataId == other.m_DataId && m_Level == other.m_Level;
	}

	bool operator=(const ScenePet& other)
	{
		m_Id = other.m_Id;
		m_DataId = other.m_DataId;
		m_Level = other.m_Level;
		return true;
	}

	//����id
	ObjID_t m_Id;

	//��ӱ�ID
	UInt32 m_DataId;

	//�ȼ�
	UInt16 m_Level;
};

/**
*@brief ���ܽ�������
*/
enum UnlockType
{
	UNLOCK_INVALID = 0,
	UNLOCK_FUNCTION = 1,	//���ܽ���
	UNLOCK_AREA = 2,		//�������

	UNLOCK_NUM = 3,
};

// �����
enum RedPointFlag
{
	// ������
	RED_POINT_GUILD_REQUESTER = 0,
	// �̵�
	RED_POINT_GUILD_SHOP = 1,
	// ����Ĥ��
	RED_POINT_GUILD_ORZ = 2,
	// Բ������
	RED_POINT_GUILD_TABLE = 3,
	// ����沢
	RED_POINT_GUILD_MERGER = 4,
	// �������ÿ�ս���
	RED_POINT_GUILD_TERR_ID = 5,
};

// ������Ʒ
struct ConsumeItem
{
public:
	ConsumeItem() : itemId(0), num(0){}

public:
	UInt32 itemId;
	UInt32 num;
};

// �˺���Ϣ����
enum AccountInfoType
{
	// �ㄻ
	ACCOUNT_POINT,
	// VIP�ȼ�
	ACCOUNT_VIP_LEVEL,
	// VIP����
	ACCOUNT_VIP_EXP,
	// �ܵĳ�ֵ���
	ACCOUNT_TOTAL_CHARGE_NUM,
	// ��Ƽƻ�
	ACCOUNT_MONEY_MANAGE_STATUS,
	// �������޺��˷�����
	ACCOUNT_WEAPON_LEASE_TICKETS,
	// �̳ǻ���
	ACCOUNT_MALL_POINT,
	// �ջ�Ծ��
	ACCOUNT_ACTIVITY,
	// ð�ձ�
	ACCOUNT_ADVENTURE,
	// ð��ͨ��֤����
	ACCOUNT_ADVENTURE_EXP,
	// ð��ͨ��֤���߰�
	ACCOUNT_ADVENTURE_KING,
	// ��ļӲ��
	ACCOUNT_HIRE_COIN,
	// ������Ȩ
	ACCOUNT_GIFT_RIGHT,
	// ���õㄻ
	ACCOUNT_CREDIT_POINT,
	// ����Ӳ��
	ACCOUNT_MONOPOLY_COIN,
	// ����ף����
	ACCOUNT_MONOPOLY_CARD,	
	// ��������
	ACCOUNT_MONOPOLY_ROLL,
	// ��Ԩ��Ȩ
	ACCOUNT_DUNGEON_RIGHT,
};

enum NotifyType
{
	NT_NONE = 0,

	// ����ս
	NT_GUILD_BATTLE = 1,

	// ������ 		
	NT_BUDO = 2,

	//���ӿ���				
	NT_JAR_OPEN = 3,

	//�����ۿ�����			
	NT_JAR_SALE_RESET = 4,

	//ʱ�޵���
	NT_TIME_ITEM = 5,

	//�ͽ�������
	NT_GOLD_BONUS_DAHUI = 6,

	//ħ�޻������
	NT_MAGJAR_RESET_POINT = 7,
	NT_MAX,
};

struct NotifyInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & type & param;
	}

	UInt32		type;	//NotifyType
	UInt64		param;
};

/**
*@brief ��¼������Ϣ
*/
struct LoginPushInfo
{
	LoginPushInfo()
	{
		id = 0;
		unlockLevel = 0;
		startTime = 0;
		endTime = 0;
		sortNum = 0;
		isShowTime = 0;
		isSetNative = 0;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & name & unlockLevel & iconPath & linkInfo & startTime & \
			endTime & loadingIconPath & sortNum & openInterval & closeInterval & \
			isShowTime & isSetNative;
	}

	// id
	UInt8				id;
	// ����
	std::string			name;
	// �����ȼ�
	UInt16				unlockLevel;
	// ͼ��·��
	std::string			iconPath;
	// ����λ��
	std::string			linkInfo;
	// ��Ч��ʼʱ��
	UInt32				startTime;
	// ��Ч����ʱ��
	UInt32				endTime;
	// loadingԤ����·��
	std::string			loadingIconPath;
	// �������
	UInt8				sortNum;
	// �������
	std::string			openInterval;
	// �رռ��
	std::string			closeInterval;
	// �Ƿ���ʾʱ��
	UInt8				isShowTime;
	// �Ƿ����ñ���ͼƬԭ������С
	UInt8				isSetNative;
};

/**
*@brief ��ҵ�½״̬
*/
enum PlayerLoginStatus
{
	PLS_NONE,
	PLS_FIRST_LOGIN_DAILY, // ÿ���һ�ε�¼
};

/**
*@brief ��ͳ��
*/
struct PacketStatistics
{
	PacketStatistics() : num(0), totalSize(0) {}
	UInt32 num;
	UInt32 totalSize;
};

#endif

