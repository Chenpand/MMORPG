#ifndef _CL_OBJECT_DEFINE_H_
#define _CL_OBJECT_DEFINE_H_

#include "CLDefine.h"

/**
 *@brief npc����ҵ�״̬
 */
enum CreatureStatus
{
	CREATURE_STATUS_NORMAL = 0,		//����״̬
	CREATURE_STATUS_DEAD =	 1,		//����״̬
    CREATURE_STATUS_WALK =   2,     //��·
    CREATURE_STATUS_RUN  =   3,     //�ܲ�
	CREATURE_STATUS_PK   =   4,     //PK��
};

/**
 *@brief ����obj����
 */
enum SceneObjectType
{
	SOT_INVALID,	//��Ч
	SOT_NPC,		//npc
	SOT_PLAYER,		//���
	SOT_ITEM,		//����
	SOT_TRAP,		//����
	SOT_PET,		//����
	SOT_FIREWORKS,	//�̻�
	SOT_SUMMONNPC,	//��ҵ��ٻ�npc
	SOT_VEHICLE,	//��ͨ����,
	SOT_CITYMONSTER,	//����
};

/**
 *@brief �ж��ǲ�������
 */
inline bool IsCreature(SceneObjectType type)
{
	switch(type)
	{
	case SOT_PLAYER:
	case SOT_NPC:
	case SOT_PET:
	case SOT_SUMMONNPC:
	case SOT_VEHICLE:
		return true;
	default:
		return false;
	}
}

/**
 *@brief �ж��ǲ����ٻ���
 */
inline bool IsSummonObj(SceneObjectType type)
{
	switch(type)
	{
	case SOT_SUMMONNPC:
	case SOT_PET:
		return true;
	default:
		return false;
	}
	return false;
}

/**
 *@brief ����object������ö��
 */
enum SceneObjectAttr
{
	SOA_INVALID,	//��Ч
	SOA_NAME,		//����	string
	SOA_LEVEL,		//�ȼ�	UInt16
	SOA_EXP,		//����	UInt64
	SOA_BLESSEXP,	//ף������	UInt32
	SOA_SEX,		//�Ա�	UInt8
	SOA_OCCU,		//ְҵ	UInt8
	SOA_PKMODE,		//pkģʽ	UInt8
	SOA_PKCOLOR,	//pk��ɫ	UInt8
	SOA_VIPLVL,		//vip�ȼ�   UInt8
	SOA_QQVIPINFO,	//qqƽ̨��Ȩ��Ϣ QQVipInfo
	SOA_EVIL,		//���	UInt16;
	SOA_ATTACK,		//����	UInt32
	SOA_DEFENSE,	//����	UInt32
	SOA_HIT,		//����	UInt8
	SOA_DODGE,		//����	UInt8
	SOA_THUMP,		//����	UInt32
	SOA_HOLDOFF,	//�ֵ�	UInt32
	SOA_THUMPEFFECT,	//����Ч�� UInt32
	SOA_CHAOSATTACK,	//���繥�� UInt32
	SOA_CHAOSDEFENSE,	//������� UInt32
	SOA_SWIM,			//ѣ��	   UInt32
	SOA_SPEEDDOWN,		//����     UInt32
	SOA_IMMOBILE,		//����	   UInt32
	SOA_SILENT,			//��Ĭ	   UInt32
	SOA_SWIM_DEF,		//ѣ�ο���	UInt32
	SOA_SPEEDDOWN_DEF,	//���ٿ���	UInt32
	SOA_IMMOBILE_DEF,	//������	UInt32
	SOA_SILENT_DEF,		//��Ĭ����	UInt32
	SOA_HP,				//����ֵ	UInt32
	SOA_MAXHP,			//�������ֵ	UInt32
	SOA_POWER,			//ս����	UInt32
	SOA_MOVESPEED,		//�ƶ��ٶ�	UInt16
	SOA_POSITION,		//��ǰλ��	CLPosition, UInt16+UInt16
	SOA_DIR,			//����		UInt8
	SOA_VEHICLEID,		//��id		ObjID_t
	SOA_SKILLS,			//�����б�  ֻ���͸����� ��ʽ:id(UInt16)  + level(UInt8) ... 0(UInt16)
	SOA_BUFFS,			//buff�б� ͬ��������Ϳɼ���� ��ʽ:id(UInt16) + value(Int32) + time(UInt32) ... 0(UInt16)
	SOA_ITEMCD,			//����cd	ֻͬ�������� ��ʽ:cd��(UInt8) + cdʱ��(UInt32) ... 0(UInt8)
	SOA_GOLD,			//����	UInt32
	SOA_BINDGOLD,		//�󶨽�� UInt32
	SOA_POINT,			//��� UInt32
	SOA_BINDPOINT,		//�󶨵�ȯ UInt32
	SOA_SPIRITSTONE,	//��ʯ UInt32
	SOA_HONOUR,			//���� UInt32
	SOA_PACKSIZE,		//������ͨ������С UInt8
	SOA_STORAGESIZE,	//�ֿ��С		   UInt8
	SOA_EQUIPS,			//��Χ�ɼ���װ��  �·�(UInt32) + װ��(UInt32)
	SOA_SUITEFFECT,		//��װЧ�� UInt32 |ȫ��ǿ���ȼ�UInt4|ȫ��6����ʯ��UInt12(��ʯ�ȼ�UInt4 + ��ʯ����UInt8)|ȫ��Ʒ��UInt8|ȫ����������Ǽ�UInt8|
	SOA_BUFFCONTROL,	//buff���Ʊ�ʶ	UInt16(BuffControlFlag���)
	SOA_DATAID,			//������id UInt32
	SOA_STATUS,			//״̬     UInt8   
	SOA_NPCTASKSTATUS,	//npc����״̬ UInt8 NpcTaskStatus
	SOA_SKILLBAR,		//������  ֻ�������� ��ʽ: pos(UInt8 0��ʼ) + id(UInt16) ... 0xFF(UInt8)
	SOA_GAMEOPTIONS,	//��Ϸѡ��
	SOA_SUBNAME,		//������ string
	SOA_TEAMMASTER,		//�Ƿ�ӳ�	UInt8 0��ʾ���� 1��ʾ�Ƕӳ�
	SOA_TEAMSIZE,		//�����С  UInt8 0��ʾû����
	SOA_TRIBEID,		//��������id	UInt64
	SOA_TRIBEPOST,		//����ְ��	UInt8
	SOA_ITEMBIND,		//���߰�  UInt8
	SOA_ITEMQUALITY,	//����Ʒ��	UInt8
	SOA_ITEMNUM,		//��������	UInt16
	SOA_TRAPID,			//����id UInt32
	SOA_MOUNTID,		//��ǰ��˵����� id(UInt32)
	SOA_BATTLEGROUP,	//ս����Ӫ	UInt8
	SOA_OWNERID,		//���ߡ��ٻ����������ID UInt64
	SOA_SUMMONAIMODE,	//�ٻ���aiģʽ	UInt8	
	SOA_TEAMID,			//������������ID UInt32
	SOA_AUTOHOOK,		//�Զ��һ���Ϣ��ֻ�ڳ���������ͬ��
	SOA_ESCORTERID,		//������id ObjID_t
	SOA_TITLEMASK,		//�ƺ����� 16���ȵ��ֽ�����
	SOA_TITLE,			//��ǰʹ�óƺ�  UInt32
	SOA_COMPLETETIME,	//������NPC�Ľ���ʱ�� UInt32
	SOA_DEATHOWNER,		//���������� ObjID_t
	SOA_POWERDETAIL,	//ս������ϸ��Ϣ ֻ���͸�������ʽ��id(UInt8) + ֵ(UInt32) ... 0(UInt8)
	SOA_UPDATEBULLETIN,	//���°汾�� UInt32
	SOA_FUNCNOTIFY,		//Ԥ�湦�� UInt8
	SOA_STORY,			//��ǰ���� UInt16
	SOA_ACTIONFLAG,		//��Ϊ��� UInt32
	SOA_FUNCFLAG,		//���ܿ�����־ char[8]
	SOA_LOVERNAME,		//�������� string
	SOA_SRCZONEID,		//ԭ����	 UInt16
	SOA_PETDATAID,		//��������id UInt32
	S0A_PETTRANSLIST,	//����û��б� char[25]
	SOA_TRANSID,		//�û�id UInt32
	SOA_DAYCHARGENUM,      //�ճ�ֵ�ۼ�   UInt32
	SOA_GATHERSOULVAL,	//�ۻ�ֵ UInt32
	SOA_SOULPACK,		//�޻걳�� PetSoul[12]
	SOA_ZONEID,			//����	UInt16
	SOA_TRANSFIGUREID,	//��ǰ����ID		UInt32
    SOA_SCENE_POS,      //�����е�λ�� ScenePos, UInt32+UInt32
    SOA_SCENE_DIR,      //�����еķ��� UInt16(360��)
	SOA_SP,				//SP	UInt32
	SOA_AWAKEN,			//�Ƿ����  UInt8
	SOA_ITEMBAR,		//��Ʒ��  ֻ�������� ��ʽ: pos(UInt8 0��ʼ) + id(UInt32) ... 0(UInt8)
    SOA_PKVALUE,		//����������  UInt32
	SOA_FATIGUE,		//ƣ��ֵ	UInt16
    SOA_WARRIOR_SOUL,   //����֮��   UInt32
    SOA_MATCH_SCORE,    //ƥ����� UInt32
    SOA_WARP_STONE,		//��Ԫʯ	��ʽ:  id(UInt32) + level(UInt8) + energy(UInt32) ... 0(UInt32)
    SOA_COUNTER,        //���ּ��� CounterMgr
	SOA_RETINUE,		//��� UInt32
	SOA_PK_COIN,		//������ UInt32
	SOA_AVATAR,			//��� PlayerAvatar
	SOA_REVIVE_COIN,	//����� UInt32
	SOA_VIP,			//vip
	SOA_CREATE_TIME,	//����ʱ��
	SOA_NEW_BOOT,		//��������
	SOA_TOWER_WIPEOUT_END_TIME,	//����֮��ɨ������ʱ�� UInt32
	SOA_GUILD_POST,		//����ְ�� UInt8
	SOA_GUILD_CONTRI,	//�ﹱ UInt32
	SOA_GUILD_SHOP,		//�����̵�ID UInt32
	SOA_RED_POINT,		//���
	SOA_GUILD_NAME,		//������ string
	SOA_GUILD_BATTLE_NUMBER,	//����ս����	UInt32
	SOA_GUILD_BATTLE_SCORE,		//����ս����	UInt32
	SOA_GUILD_BATTLE_REWARD,	//����ս����	
	SOA_DAILY_TASK_SCORE,		//ÿ���������	UInt32
	SOA_DAILY_TASK_REWARD_MASK,	//ÿ��������ֽ���
	SOA_WUDAO_STATUS,	// ������״̬ UInt8
	SOA_MONTH_CARD_EXPIRE_TIME,	//�¿�����ʱ�� UInt32
	SOA_CUSTOM_FIELD,			//�Զ����ֶ� UInt32
	SOA_SEASON_LEVEL,			//�����ȼ�	UInt32
	SOA_SEASON_STAR,			//��������	UInt32
	SOA_SEASON_UPLEVEL_RECORD,	//������������¼	PkRaceResult + ... + 0(UInt8)
	SOA_SEASON_ATTR,			//��������id		UInt8
	SOA_SEASON_KING_MARK,		//����ӡ��			UInt16
	SOA_AUCTION_REFRESH_TIME,	//��һ��ˢ��������ʱ��
	SOA_AUCTION_ADDITION_BOOTH,	//�����ж�����λ
	SOA_GOLDJAR_POINT,			//��޻���
	SOA_MAGJAR_POINT,			//ħ�޻���
	SOA_BOOT_FLAG,				//������
	SOA_SEASON_EXP,				//��������	UInt32
	SOA_PET_FOLLOW,				//����ĳ��� UInt64
	SOA_POTION_SET,				//ҩˮ����
	SOA_PRE_OCCU,				//Ԥתְְҵ UInt8
	SOA_GUILD_BATTLE_LOTTERY_STATUS,	//����ս�齱״̬ UInt8
	SOA_PVP_SKILLS,						//PVP�����б�  ֻ���͸����� ��ʽ:id(UInt16)  + level(UInt8) ... 0(UInt16)
	SOA_PVP_SKILLBAR,					//������  ֻ�������� ��ʽ: pos(UInt8 0��ʼ) + id(UInt16) ... 0xFF(UInt8)
	SOA_PVP_SP,					//SP	UInt32
	SOA_ACHIEVEMENT_SCORE,		//�ɾͻ��� UInt32
	SOA_ACHIEVEMENT_TASK_REWARD_MASK, //�ɾͻ��ֽ���
	SOA_WEAPON_BAR,						//������
	SOA_APPOINTMENT_OCCU,				//�Ƿ���ԤԼ��ɫ
	SOA_PACKAGE_SIZE_ADDITION,	//���������ӳ�
	SOA_MONEY_MANAGE_STATUS,			//��ƹ���״̬
	SOA_MONEY_MANAGE_REWARD_MASK,		//��ƹ�����
	SOA_SCORE_WAR_SCORE,				//����������
	SOA_SCORE_WAR_BATTLE_COUNT,			//������ս������
	SOA_SCORE_WAR_REWARD_MASK,			//���������뽱
	SOA_SCORE_WAR_WIN_BATTLE_COUNT,		//������ս��ʤ������
	SOA_ACADEMIC_TOTAL_GROWTH_VALUE,	//ͽ��ѧҵ�ɳ�ֵ
	SOA_MASTER_DAILYTASK_GROWTH,		//ʦ���ճ�����ɳ���ֵ
	SOA_MASTER_ACADEMICTASK_GROWTH,	    //ʦ��ѧҵ�ɳ���ֵ
	SOA_MASTER_UPLEVEL_GROWTH,			//ʦ�������ɳ���ֵ
	SOA_MASTER_GIVEEQUIP_GROWTH,		//ʦ������װ���ɳ���ֵ
	SOA_MASTER_GIVEGIFT_GROWTH,			//ʦ����������ɳ���ֵ
	SOA_MASTER_TEAMCLEARDUNGON_GROWTH,	//ʦ��ʦͽ���ͨ�����³ǳɳ���ֵ
	SOA_MASTER_GOODTEACHER_VALUE,		//��ʦֵ
	SOA_ROOM_ID,						//����ID
	SOA_SHOW_FASHION_WEAPON,			//��ʾʱװ����
	SOA_WEAPON_LEASE_TICKETS,			//�������޺��˷�
	SOA_GAME_SET,						//��Ϸ����
	SOA_NOVICE_GUIDE_CHOOSE_FLAG,		//��������ѡ���־
	SOA_ADVENTURE_TEAM_NAME,            //ð�ն�(Ӷ����)��
	SOA_HEAD_FRAME,					    //ͷ���
	SOA_NEW_TITLE_NAME,					//����ͷ����
	SOA_NEW_TITLE_GUID,					//����ͷ��GUID
	SOA_CHIJI_HP		= 169,			//�Լ�HP
	SOA_CHIJI_MP		= 170,			//�Լ�MP
	SOA_PACKAGE_TYPE	= 171,			//�������͵Ĵ�С
	SOA_GUILD_EMBLEM	= 172,			//����ռ�
	SOA_OPPO_VIP_LEVEL	= 173,			//oppo vip�ȼ�
	SOA_CHIJI_SCORE		= 174,			//�Լ�����
	SOA_EQUAL_PVP_SKILLS,				//��ƽ������PVP�����б�  ֻ���͸����� ��ʽ:id(UInt16)  + level(UInt8) ... 0(UInt16)
	SOA_EQUAL_PVP_SKILLBAR,				//��ƽ������������  ֻ�������� ��ʽ: pos(UInt8 0��ʼ) + id(UInt16) ... 0xFF(UInt8)
	SOA_EQUAL_PVP_SP,					//��ƽ������SP	UInt32
	SOA_ACCOUNT_ACHIEVEMENT_SCORE,	   //�˺ųɾͻ���
	SOA_MALL_POINT = 179,			    //�̳ǻ��� UInt32
	SOA_TOTAL_EQUIP_SCORE = 180,		//ȫ��װ������ UInt32
	SOA_ADVENTURE_COIN = 181,			//ð�ձ�
	SOA_LOSTDUNGEON_HP,					//����HP
	SOA_LOSTDUNGEON_MP,					//����MP
	SOA_LOSTDUNGEON_SCORE,				//���λ���
	SOA_LOSTDUNGEON_BATTLE_ST,			//����ս��״̬
	SOA_CREDIT_POINT,					//���õ�ȯ
	SOA_CREDIT_POINT_MONTH_GET,			//���õ�ȯ�»��
}; 


/**
 *@brief �г�������
 */
struct ChangeSceneInfo
{
	ChangeSceneInfo():id(0),accid(0),gateid(0),srcnode(0),srcscene(0)
		,destnode(0),destscene(0){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & accid & gateid & srcnode & srcscene
			& destnode & destscene;
	}
		
	//Ψһid
	ObjID_t	id;
	//�˺�id
	UInt32	accid;
	//����id
	UInt32	gateid;
	//Դ�ڵ�
	UInt32	srcnode;
	//Դ����id
	UInt32	srcscene;
	//Ŀ�������
	UInt32	destnode;
	//Ŀ�곡��id
	UInt32	destscene;
};


/** 
 *@brief ������״
 */
enum TrapShape
{
	TRAP_SHAPE_CIRCLE = 0,	//Բ��
	TRAP_SHAPE_RECT = 1,	//����
};

/**
 *@brief ��������
 */
enum TrapType
{
	TRAP_TYPE_NORMAL =	0,	//��ͨ����
	TRAP_TYPE_BLOCK =	1,	//�赲����
	TRAP_TYPE_TRANS =	2,	//��������
	TRAP_TYPE_EVENT =	3,	//�����ű��¼�������
	TRAP_TYPE_EFFECT =	4,	//��Ч����
	TRAP_TYPE_LAST	= 5,    //��������
};

/**
 *@brief �Ƿ���Ҫ���������赲
 */
inline bool IsTrapSetBlock(int type)
{
	return type != TRAP_TYPE_EFFECT;
}

/**
 *@brief ����ٿ�buff
 */
#define TRAP_HANDLE_BUFF 2102

/**
*@brief ͬ���¼�����
*/
enum SyncEventType
{
	SET_INVALID,
	SET_FRIEND_GIVE,	//��������
	SET_REVIVE,			// ����
	SET_ADD_AUCTION,	//�������ϼ�
	SET_SELL_AUCTION,	//�������۳�
	SET_ADD_REDPOINT,	//��Ӻ��
	SET_CLR_REDPOINT,	//������
	SET_OPEN_FREE_GOLD_CHEST, //����ѵĻƽ���
	SET_BUY_DUNGEON_TIMES,	// ������³Ǵ���
	SET_AUCTION_REFRESH_TIME,// ������ˢ��ʱ��
	SET_NEW_YEAR_RED_PACKET, //������
	SET_PREMIUM_LEAGUE_ENROLL,// �ͽ���������
	SET_BUY_AUCTION,		//�����й���
	SET_JOIN_GUILD,			//���빤��
	SET_GUILD_TERR_CHEST_DOUBLE,	// ������ط��Ƽӱ�
	SET_GUILD_BATTLE_SCORE,			//����ս����
	SET_GUILD_OCCUPY_TERRITORY,		//ռ�����
	SET_DUNGEON_USE_POTION,		// ���³��г�ҩ
	SET_ENTER_CROSS_SCENE,		// ����������
	SET_EXIT_CROSS_SCENE,		// �˳��������
	SET_RELATION_RELIEVE_MASTER, //���ʦͽ��ϵ
	SET_RELATION_DISCIPLE_FINISHSCHOOL, //ͽ�ܳ�ʦ
	SET_RELATION_DISCIPLE_APPRENTICE,  //ͽ�ܰ�ʦ�ɹ�
	SET_ADD_GOODTEACH_VALUE,	//�����ʦֵ
	SET_REMOVE_FATIGUE,			//����ƣ��
	SET_OPPO_VIP_LEVEL,			//oppo vip�ȼ�
	SET_ACCOUT_ACHIVE_SCORE,	//�����˺ųɾͻ���
	SET_LOST_DUNG_HP_MP,		//���õ���Ѫ��������
	SET_LOST_DUNG_BATTLE_ST,	//������ҵ���ս��״̬
	SET_LOST_DUNG_PROT_TIME,	//������ҵ���ս������ʱ���
	SET_TEAM_COPY_CLEAR_DUNGEON,	//�ű�ͨ�ص��³�
};

/**
*@brief ����״̬
*/
enum AwakenStatus
{
	AWAKEN_NOTHING = 0,		//û�о���
	AWAKEN_ONE = 1,			//һ��
	AWAKEN_TWO = 2,			//����
};

struct NpcPos
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & x & y;
	}

	bool operator<(const NpcPos& other) const
	{
		if (x != other.x)
		{
			return x < other.x;
		}
		if (y != other.y)
		{
			return y < other.y;
		}

		return false;
	}

	Int32	x;
	Int32	y;
};

// ����npc��Ŀǰ��֧�ֳ�����
struct SceneNpc
{
	SceneNpc()
	: guid(0), type(SOT_INVALID), funcType(0), id(0)
	{
		inBattle = 0;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & guid & type & funcType & id & pos & remainTimes & totalTimes & dungeonId & inBattle;
	}

	ObjID_t				guid;
	// npc���ͣ���Ӧö��SceneObjectType
	UInt8				type;
	// �������ͣ���ͬSceneObjectType�в�ͬ�ĺ��壩
	// SOT_CITYMONSTER -> CityMonsterType
	UInt8				funcType;
	UInt32				id;
	NpcPos				pos;
	// ʣ�����
	UInt32				remainTimes;
	// �ܴ���
	UInt32				totalTimes;
	// ��Ӧ�ĵ��³�
	UInt32				dungeonId;
	// �Ƿ�ս����
	UInt8				inBattle;

};

// ����npc��Ϣ
struct SceneNpcInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & sceneId & npcs;
	}

	UInt32					sceneId;
	std::vector<SceneNpc>	npcs;
};

//������Ʒ����
struct SceneItemPos
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & x & y;
	}

	bool operator<(const SceneItemPos& other) const
	{
		if (x != other.x)
		{
			return x < other.x;
		}
		if (y != other.y)
		{
			return y < other.y;
		}

		return false;
	}

	UInt32	x;
	UInt32	y;
};

// ������Ʒ
struct SceneItem
{
	SceneItem()
	: guid(0), dataId(0)
	{
		ownerId = 0;
		disapperTime = 0;
		openerId = 0;
		openTime = 0;
		waveId = 0;
		resourceId = 0;
		refreshTime = 0;
		refreshed = false;
		resPosGroupId = 0;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & guid & dataId & pos & ownerId;
	}

	ObjID_t				guid;
	UInt32				dataId;
	SceneItemPos		pos;
	ObjID_t				ownerId;
	UInt32				disapperTime;
	ObjID_t				openerId;
	UInt32				openTime;
	//������Ϣ
	UInt32				waveId;
	SceneItemPos		blueCenter;
	UInt32				blueRadius;

	//������Դ����
	//��Դ��id
	UInt32				resourceId;
	//��Դ�������id
	UInt32				resPosGroupId;
	//ˢ��ʱ��
	UInt32				refreshTime;
	//�Ƿ��Ѿ�ˢ��
	bool				refreshed;
};

// ������Ʒ��Ϣ
struct SceneItemInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & sceneId & items;
	}

	UInt32					sceneId;
	std::vector<SceneItem>	items;
};

//�Լ�����ԭ��
enum ChiJiDeadReason
{
	CHIJI_DEAD_REASON_OFFLINE = 0,	//����
	CHIJI_DEAD_REASON_BE_KILLED = 1,	//��ɱ
	CHIJI_DEAD_REASON_POISIONED = 2,	//����
	CHIJI_DEAD_REASON_BALANCE = 3,	//����
	CHIJI_DEAD_REASON_TIMEOUT = 4,	//��ʱ
};

// ��þ���ԭ��
enum PlayerIncExpReason
{
	PIER_INVALID,
	// ʹ�þ�����(�����Ǿ���ֵ)
	PIER_EXP_PILL_VALUE,
	// ʹ�þ�����(�����ǰٷֱ�)
	PIER_EXP_PILL_PERCENT,
};

#endif
