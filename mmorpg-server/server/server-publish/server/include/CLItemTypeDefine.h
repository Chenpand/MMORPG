#ifndef _CL_ITEM_TYPE_DEFINE_H_
#define _CL_ITEM_TYPE_DEFINE_H_

#include "CLDefine.h"

/**
 *@brief ��������
 */
enum ItemType
{
	ITEM_INVALID =			0,	//��Ч����
	ITEM_EQUIP =			1,	//װ����ӵ�������ͣ�
	ITEM_EXPENDABLE,			//����Ʒ
	ITEM_MATERIAL,				//����
	ITEM_TASK,					//����
	ITEM_FASHION,				//װ��
	ITEM_INCOME,				//���棬�������Զ���û���
	ITEM_ENERGY_STONE,			//����ʯ added by huchenhui 2016.07.21
	ITEM_TITLE,					//�ƺ�
	ITEM_PET = 10,				//����
	ITEM_GUILD_BUFF = 11,		//������³�buff
	ITEM_HEAD_FRAME = 12,		//ͷ���
	ITEM_SKILL = 13,			//����
	ITEM_NEWTITLE = 14,			//ͷ��
	ITEM_INSCRIPTION = 15,		//����
	ITEM_LOSTDUNGEON = 16, //��ʧ������Ʒ
	ITEM_MONOPOLY_CARD = 17,	//���̿���
	ITEM_MAX,					//���������

	//------------����---------------------------------
	ITEM_MEDICINE,				//ҩ����ӵ�������ͣ�
	ITME_GEMSTONE,				//��ʯ
	ITEM_PACK_ACTIVE,			//��������			����ֶ�1  ���������
	ITEM_RUNE,					//����
	ITEM_COPYSCENE,				//�������������	����ֶ�1Ϊ���ɴ���������id  �ֶ�2Ϊ�����ų���ʱ��(��) 	�ֶ�3Ϊ������ͼid
	ITEM_VIPCARD,				//vip��				����ֶ�1Ϊvip�ȼ�  �ֶ�2Ϊʱ��(�죩 �ֶ�3Ϊ1��ʾΪ���鿨
	//TODO
	ITEM_TELEPORT,				//˲�������		����ֶ�1Ϊ����  0��� 1�س� 2����  ����ֶ�2Ϊ����ʱ��(��)
	ITEM_TREASUREBOX,			//����
	ITEM_MESS,					//����	����ʹ��
	ITME_IMPEACHBRAND,			//�������쵯����
	//ITEM_PET,					//�������
	ITEM_GIFTBAG,				//��������		ʹ���߽ű�
	ITEM_TREASUREMAP,			//�ر�ͼ
	ITEM_FIREWORKS,				//�̻�				����ֶ�1Ϊ����ʱ�䣨��)
	ITEM_MONEY_USE,				//��Ϸ�ң�ͨ��ʹ�û�û��ң�������ͬITEM_MONEY������ֶ�1Ϊ����ֵ
	ITEM_ADD_BUFF,				//�����buff,		����ֶ�1Ϊbuff id������ֶ�2Ϊ����ʱ�䣬����ֶ�3Ϊbuff��ֵ
	ITEM_SCRIPT,				//�ű�����
	ITEM_LOGINLIMIT,			//�ܵ�¼���Ƶĵ�������
	ITEM_GIVE,					//��������
	ITEM_WING,					//�����ص���
	ITEM_MOUNT,					//�������
	ITEM_RIDE_BOOK,				//��˼�����
};

//���������
enum ItemSubType
{
	ST_ITEM_INVALID = 0,

	ST_EQUIP_WEAPON,			//����
	ST_EQUIP_HEAD,			//ͷ��
	ST_EQUIP_CHEST,			//��
	ST_EQUIP_BELT,			//����
	ST_EQUIP_LEG,			//����
	ST_EQUIP_BOOT,			//Ь��
	ST_EQUIP_RING,			//��ָ
	ST_EQUIP_NECKLASE,		//����
	ST_EQUIP_BRACELET,		//����
	ST_EQUIP_TITLE,			//�ƺ�

	ST_FASHION_HALO,		//ʱװ���
	ST_FASHION_HEAD,			//ʱװͷ��
	ST_FASHION_SASH,		//ʱװ��
	ST_FASHION_CHEST,		//ʱװ����
	ST_FASHION_LEG,			//ʱװ����
	ST_FASHION_EPAULET,			//ʱװ����

	ST_ITEM_GOLD,			//���
	ST_ITEM_POINT,			//��ȯ
	ST_ITEM_EXP,				//����

	ST_NOT_USE_20,

	ST_ENERGY,              //����ʯ
	ST_WARRIOR_SOUL,		//����֮��
	ST_PK_COIN,				//������
	ST_JINGPO,				//����
	ST_MAGICCARD,			//��ħ��
	ST_REVIVE_COIN,			//�����

	ST_BIND_GOLD,			//���
	ST_BIND_POINT,			//�󶨵�ȯ

	ST_GIFT_PACKAGE,		//���

	ST_GUILD_CONTRI,		//���ṱ��

	ST_SP,					//SP��
	ST_ENERY_STONE,			//����ʯ
	ST_COUPON,				//ǿ����
	ST_MONTH_CARD,			//�¿�
	ST_JAR,					//�������
	ST_GIFT_BOX,			//��У���Ԥ����
	ST_FATIGUE_DRUG,		//ƣ��ҩ
	ST_DRAWING,				//ͼֽ�о�
	ST_FRAGMENT,			//��Ƭ
	ST_VIP_EXP,				//vip����
	ST_EXP_PILL,			//������
	ST_GOLDJAR_POINT,		//��޻���
	ST_MAGJAR_POINT,		//ħ�޻���
	ST_PET_EGG,				//���ﵰ
	ST_FASHION_COMPOSER,	//ʱװ�ϳ���
	ST_MONEY_MANAGE_CARD,	//��ƿ�

	ST_HP_POTION = 50,		//HPҩ��
	ST_MP_POTION,			//MPҩ��
	ST_HPMP_POTION,			//HP+MPҩ��
	ST_CHANGE_NAME,			//������
	ST_BEAD,				//����
	ST_MAGIC_BOX,			//ħ��
	ST_MAGIC_HAMMER,		//ħ��
	ST_COLORLESS_CRYSTAL,	//��ɫ����
	ST_JAR_GIFT,			//���ӹ�������
	ST_ADD_CHARGE_POINT = 60,	//������ֵ����
	ST_ADD_VIP_POINT = 61,		//������ֵvip����
	ST_ATTRIBUTE_DRUG = 62,     //����ҩˮ


	// �����
	ST_APPOINTMENT_COIN = 70,	// ԤԼ��
	ST_CONSUME_JAR_GIFT = 73,   //��Ҫ���ĵ��ߵ�ħ�޹�������
	ST_PRIMARY_RAFFLE_TICKETS = 74, //��ͨ�齱ȯ
	ST_MIDDLE_RAFFLE_TICKETS = 75,	//�м��齱ȯ
	ST_SENIOR_RAFFLE_TICKETS = 76,	//�߼��齱ȯ

	//ʦ��ϵͳֵ
	ST_MASTER_ACADEMIC_VALUE = 78, //ʦ��ѧҵ�ɳ�ֵ
	ST_MASTER_GOODTEACH_VALUE = 79, //��ʦֵ
	
	//�ع�Ӳ��
	ST_RETURN_TOKEN = 80,	//�ع�Ӳ��

	//ʱװ����
	ST_FASHION_WEAPON = 81,  //ʱװ����

	// �ٱ任װ�ڻ�װ��
	ST_CHANGE_FASHION_ACTIVE_TICKET = 82, // �ٱ任װ�ڻ�װ��

	// ��װ����
	ST_DRESS_INTEGRAL_VALUE = 83, // �ٱ�ʱװ���װ����
	// �������޺��˷� 
	ST_WEAPON_LEASE_TICKET = 84,
	// �͸�ˮ��(Ӷ��ѫ��)
	ST_BLESS_CRYSTAL = 87,
	// ����ף��(�ɳ�ҩ��)
	ST_INHERIT_BLESS = 88,
	// �ͽ�
	ST_BOUNTY = 91,

	// ����ҩˮ(��1��)
	ST_UPLEVEL = 86,

	//ʱװ�⻷
	ST_FASHION_GUANGHUAN = 92,		

	//ͷ�ε���
	ST_NEW_TITLE = 94,
	
	// �Լ�HPҩˮ
	ST_CHIJI_HP = 95,
	// �Լ�����ҩˮ
	ST_CHIJI_SPEED = 96,
	// �Լ�Ͷ����
	ST_CHIJI_THROW = 97,
	// �Լ�buff����
	ST_CHIJI_BUFF = 98,

	// ����װ��
	ST_ASSIST_EQUIP = 99,
	// ħ��ʯ
	ST_MAGICSTONEE = 100,
	// ����
	ST_EARRINGS = 101,

	ST_ZENGFU_MATERIAL = 102, //�������Ĳ���
	ST_ZENGFU_ACTIVATE = 103, //������ʼ���װ��
	ST_ZENGFU_CLEANUP = 104, //����������װ��
	ST_ZENGFU_CHANGE = 105, //�������ת����
	ST_ZENGFU_CREATE = 106, //�����������װ��
	ST_ZENGFU_OVERLOAD = 107, //������ʹ���װ��
	ST_ZENGFU_PROTECT = 108, //��������ȯ
	ST_ZENGFU_AMPLIFICATION = 109, //����ȯ
		
	ST_ACHIEVEMENT_POINT = 111, //�ɾ͵�
	ST_CHI_JI_COIN = 112, //�Լ���ҫ��
	ST_ZJSL_WZHJJJ_COIN = 114, //�ռ��������߻���
	ST_ZJSL_WZHGGG_COIN = 115, //�ռ��������߻���
	ST_MALL_POINT = 116,			//�̳ǻ���
	ST_INSCRIPTION = 117, //����

	ST_FLYUPITEM = 118,            //��������

	ST_ADVENTURE_COIN = 119,       //ð�ձ�

	ST_CHALLENGE_SCORE = 122,      //��ս�߻����

	ST_ADVENTURE_PASS_EXP = 123,   //ð��ͨ��֤����
	ST_ADVENTURE_PASS_ACTIVATE_ITEM = 124, //���߰�ͨ��֤����ȯ
	
	ST_FRIEND_PRESENT = 125,	//�������͵���

	ST_HONOR_SCORE = 126,	//����

	ST_SPRING_SCORE = 127,  //������ս����

	ST_CHIJI_SKILL = 128,	//�Լ�����

	ST_CHIJI_SHOP_COIN = 129,	//�Լ��̵����

	ST_HIRE_COIN = 130,     //��ļӲ��

	ST_TEAM_COPY_CNT = 131,     //��ͨ�ű�ͨ��֤

	ST_HONOR_GUARD_CARD = 132,     //�������Ͽ�

	ST_ADVENTURE_KING = 133,	//ð��ͨ��֤���߰�

	ST_USE_DEATH_RESURRECTION = 135, //�������������ߣ���������
	
	ST_LOST_DUNGEON_CURRENCY = 137,	//��ʧ�ع�����
	
	ST_LOST_DUNGEON_INTEGRAL = 138, //���������ֵ���
	
	ST_LOST_DUNGEON_INCREASE_TIMES = 139, //���������Ӵ�������

	ST_TEAM_COPY_DIFF_CNT = 140,	//ج���ű�ͨ��֤

	ST_MAGIC_DISCOUNT = 142,	//ħ���ۿ۵���

	ST_LOST_DUNGEON_ADD_SPEED = 143, //��������������

	ST_LOST_DUNGEON_RECOVERY = 144, //�������ָ�����

	ST_CHAMPION_COIN = 145,			//�ھ������±�
	// �������
	ST_VIRTUAL		= 120,		// �������(ʵ��û����)
	
	ST_ZENGFU_RAND_TICKET = 146, //�������ȯ

	ST_STRENTH_LUCK = 147,		 //����ǿ��ȯ

	ST_MAGICCARD_MATERIAL = 148,		 //���ϸ�ħ������

	ST_GIFT_RIGHT_CARD = 149,		 //������Ȩ��

	ST_CREDIT_POINT = 150,		 //���õ�ȯ

	ST_FANTASY_COIN = 154,		 //����Ӳ��

	ST_MONOPOLY_CARD = 155,      //����ף����

	ST_MONOPOLY_ROLL = 156,      //��������

	ST_SECRET_SELL_COIN = 157,      //�������������ȯ

	ST_DUNGEON_TICKET_RIGHT_CARD = 161,//��ԨƱ������Ȩ��
};

//�����������
enum ItemThirdType
{
	TT_ITEM_INVALID = 0,
	TT_ITEM_HUGESWORD = 1,		//�޽�
	TT_ITEM_KATANA = 2,			//̫��
	TT_ITEM_SHORTSWORD = 3,		//�̽�
	TT_ITEM_BEAMSWORD = 4,		//�⽣
	TT_ITEM_BLUNT = 5,			//����
	TT_ITEM_REVOLVER = 6,		//����
	TT_ITEM_CROSSBOW = 7,		//��
	TT_ITEM_HANDCANNON = 8,		//����
	TT_ITEM_RIFLE = 9,			//��ǹ
	TT_ITEM_PISTOL = 10,			//��ǹ
	TT_ITEM_STAFF = 11,			//����
	TT_ITEM_WAND = 12,			//ħ��
	TT_ITEM_SPEAR = 13,			//ì
	TT_ITEM_STICK = 14,			//����
	TT_ITEM_BESOM = 15,			//ɨ��
	TT_ITEM_GLOVE = 16,		//����
	TT_ITEM_BIKAI = 17,		//����
	TT_ITEM_CLAW = 18,		//צ
	TT_ITEM_OFG = 19,		//ȭ��
	TT_ITEM_EAST_STICK = 20,	//������
	TT_ITEM_SICKLE	= 21,	//����
	TT_ITEM_TOTEM	= 22,	//ͼ��
	TT_ITEM_AXE		= 23,	//ս��
	TT_ITEM_BEADS	= 24,	//����
	TT_ITEM_CROSS	= 25,	//ʮ�ּ�
	TT_WP_TYPE_MAX = 26,	//�����������ֵ

	TT_ITEM_CLOTH = 51,		//����
	TT_ITEM_SKIN = 52,			//Ƥ��
	TT_ITEM_LIGHT = 53,			//���
	TT_ITEM_HEAVY = 54,			//�ؼ�
	TT_ITEM_PLATE = 55,			//���

	TT_FASHION_JUNIOR = 100,	//����ʱװ
	TT_FASHION_SENIOR = 101,	//�߼�ʱװ
	TT_FASHION_FESTIVAL = 102,	//����ʱװ
	TT_COMPOSER_JUNIOR = 103,	//����ʱװ�ϳ���
	TT_COMPOSER_SENIOR = 104,	//�߼�ʱװ�ϳ���

	TT_ZENGFU_COLOR_PURPLE = 200,   //�������������ɫװ��ʹ��
	TT_ZENGFU_COLOR_PINKUP = 201,   //�������߶���ɫ������װ��ʹ��
	TT_ZENGFU_COLOR_GREEN = 202,    //�������߶���ɫ������װ��ʹ��

	TT_DISPOSABLE_STRENG_ITEM = 203,   //һ����ǿ����
	TT_DISPOSABLE_INCREASE_ITEM = 204, //һ���Լ�����


	TT_SMALL_FATIGUE_POTION = 300,	//Сƣ��ҩ
	TT_MID_FATIGUE_POTION = 301,			//��ƣ��ҩ
	TT_BIG_FATIGUE_POTION = 302,			//��ƣ��ҩ

	TT_DUNGEON_POTION = 401,		//�����ڵ�ҩˮ

	TT_ITEM_CHANGE_PLAYER_NAME = 500,	// ���˸�����
	TT_ITEM_CHANGE_GUILD_NAME = 501,			// ���������

	IT_ITEM_GOLD_TITLE = 502,			// ��ҳƺ�
	IT_ITEM_RENAME_ADVENTURE_TEAM = 503,// ð�նӸ�����
	IT_ITEM_ROLE_EXTEND = 504,			// ��ɫ��λ��չ��

	IT_ITEM_CONVERTOR_SAME = 506,		//ͬ��װת����
	IT_ITEM_CONVERTOR_DIFF = 507,		//����װת����

	IT_ITEM_INSCRIPTION_RED		= 800,	// ��ɫ����
	IT_ITEM_INSCRIPTION_YELLOW	= 801,	// ��ɫ����
	IT_ITEM_INSCRIPTION_BLUE	= 802,	// ��ɫ����
	IT_ITEM_INSCRIPTION_DGLOD	= 803,	// ��������
	IT_ITEM_INSCRIPTION_BGLOD	= 804,	// ҫ������
	IT_ITEM_INSCRIPTION_ORANGE	= 805,	// ��ɫ����
	IT_ITEM_INSCRIPTION_GREEN	= 806,	// ��ɫ����
	IT_ITEM_INSCRIPTION_PURPLE	= 807,	// ��ɫ����
};

/**
*@brief װ������
*/
enum EquipType
{
	EQUIP_NORMAL = 0,               //��ͨװ��
	EQUIP_HAVE_SMELL = 1,           //����Ϣ��װ��
	EQUIP_RED = 2,                  //����װ��
};

/**
*@brief ����װ����������
*/
enum EnhanceType
{
	ENHANCE_INVALID = 0,              //��Ч
	ENHANCE_STRENTH = 1,              //����
	ENHANCE_INTELLECT = 2,            //����
	ENHANCE_STAMINA = 3,              //����
	ENHANCE_SPIRIT = 4,               //����
};


//��Ʒ���
enum ShopItemType
{
	SIT_NONE = 0,
	SIT_MATERIAL,		//�ز�
	SIT_WEAPON,			//����
	SIT_ARMOR,			//����
	SIT_JEWELRY,		//����
	SIT_COST,			//����Ʒ
	SIT_VALUABLE,		//��Ʒ
	SIT_RETINUE,		//���
};

//����ʹ�����
enum ItemUseType
{
	IUT_NONE = 0,		//����ʹ��
	IUT_USESINGLE,		//�ɵ���ʹ��
	IUT_USEALL,			//������ʹ��
};


const static Int32 ITEMTYPE_SORT_ORDER[] = {
	ITEM_EQUIP, 
	ITEM_LOGINLIMIT,
	ITEM_VIPCARD,
	ITEM_MEDICINE,
	ITEM_TREASUREMAP,
	ITEM_PACK_ACTIVE,
	ITEM_GIFTBAG,
	ITEM_TREASUREBOX,
	ITEM_TELEPORT,
	ITME_GEMSTONE
};

//�̵�ҳǩˢ������
enum RefreshCycleType
{
	//��ˢ������
	REFRESH_CYCLE_NONE,
	//ÿ��ˢ��
	REFRESH_CYCLE_DAILY,
	//ÿ��ˢ��
	REFRESH_CYCLE_WEEK,
	//ÿ��ˢ��
	REFRESH_CYCLE_MONTH,
};

//�̵�ҳǩˢ�����ڼ���״̬
enum RefreshCycleState
{
	//�Ƿ�
	REFRESH_CYCLE_INVAILD,
	//��������ˢ��
	REFRESH_CYCLE_ACTIVE,
	//ˢ�����ڼ���
	REFRESH_CYCLE_ACTIVATION,
	//ˢ������δ����
	REFRESH_CYCLE_QUIET,
};

//װ����λ���
enum EquipPartType
{
	EPT_NONE = 0,
	EPT_WEAPON,		//����
	EPT_CLOTH,		//����
	EPT_LEATHER,	//Ƥ��
	EPT_HEAVY,		//�ؼ�
	EPT_PLATE,		//���
	EPT_LIGHT,		//���
	EPT_JEWELRY,	//����
	EPT_ASSIST,		//����װ��
	EPT_MAGICSTONE,		//ħ��ʯ
};

//װ��Ʒ���������
enum EquipQLAttrType
{
	EQAT_PHYATK = 0,		//������
	EQAT_MAGATK,			//ħ������
	EQAT_PHYDEF,			//�������
	EQAT_MAGDEF,			//ħ������
	EQAT_STRENGTH,			//����
	EQAT_STAMINA,			//����
	EQAT_SPIRIT,			//����
	EQAT_INTELLECT,			//����
	EQAT_LIGHT_STRPROP,		//����ǿ
	EQAT_FIRE_STRPROP,		//����ǿ
	EQAT_ICE_STRPROP,		//����ǿ
	EQAT_DARK_STRPROP,		//����ǿ
	EQAT_LIGHT_DEFPROP,		//������
	EQAT_FIRE_DEFPROP,		//������
	EQAT_ICE_DEFPROP,		//������
	EQAT_DARK_DEFPROP,		//������
	EQAT_GDKX,		//�е翹��
	EQAT_CXKX,		//��Ѫ����
	EQAT_ZSKX,		//���տ���
	EQAT_ZDKX,		//�ж�����
	EQAT_SMKX,		//ʧ������
	EQAT_YXKX,		//��ѣ����
	EQAT_SHKX,		//ʯ������
	EQAT_BDKX,		//��������
	EQAT_SLEEPKX,		//˯�߿���
	EQAT_HLKX,		//���ҿ���
	EQAT_SFKX,		//��������
	EQAT_JSKX,		//���ٿ���
	EQAT_ZZKX,		//���俹��
	EQAT_TOTALKX,	//������
	EQAT_INDEPENDATK,	//����������
	EQAT_MAX,
};

//װ��Ʒ������ģʽ
enum EquipQLMode
{
	EQM_ALL = 0,
	EQM_SINGLE,

	EQM_MAX
};

/**
*@brief �Ƿ��Ǹ���װ��
*/
inline bool IsAssistEquip(int type)
{	
	return type == ST_ASSIST_EQUIP;
}

/**
*@brief �Ƿ���ħ��ʯ
*/
inline bool IsMagicStoneEquip(int type)
{
	return type == ST_MAGICSTONEE;
}

/**
 *@brief �ж��Ƿ�Ϊ����
 */
inline bool IsWeapon(int type)
{
	switch(type)
	{
	case ST_EQUIP_WEAPON:
		return true;
	default:
		return false;
	}
}

/**
 *@brief �ж��Ƿ�Ϊ����
 */
inline bool IsDefend(int type)
{
	switch(type)
	{
	case ST_EQUIP_HEAD:
	case ST_EQUIP_CHEST:
	case ST_EQUIP_BELT:
	case ST_EQUIP_LEG:
	case ST_EQUIP_BOOT:
		return true;
	default:
		return false;
	}
}

/**
 *@brief �ж��Ƿ�Ϊ����
 */
inline bool IsOrnaments(int type)
{
	switch(type)
	{
	case ST_EQUIP_RING:
	case ST_EQUIP_NECKLASE:
	case ST_EQUIP_BRACELET:
		return true;
	default:
		return false;
	}
}


inline bool IsFashion(ItemSubType type)
{
	switch (type)
	{
	case ST_FASHION_HALO:
	case ST_FASHION_HEAD:
	case ST_FASHION_SASH:		
	case ST_FASHION_CHEST:
	case ST_FASHION_LEG:
	case ST_FASHION_EPAULET:
		return true;
	default:
		return false;
	}
}

/**
 *@brief �ж��Ƿ񹥻�װ��
 */
inline bool IsAttackEquip(int type)
{
	if(IsWeapon(type)) return true;

// 	switch(type)
// 	{
// 	case EQUIP_TYPE_AMULET:
// 	case EQUIP_TYPE_TORQUE:
// 	case EQUIP_TYPE_RING:
// 	case EQUIP_TYPE_BANGLE:
// 		return true;
// 	}
	return false;
}

/**
 *@brief �ж��Ƿ����װ��
 */
inline bool IsDefenseEquip(int type)
{
	return IsDefend(type);
}

/**
 *@brief �ж��Ƿ���Ҫ����ս����
 */
inline bool IsEquipNeedCalPower(int type)
{
// 	return (type != EQUIP_TYPE_INVALID && type < EQUIP_TYPE_WEAPON_MAX) 
// 		|| type == EQUIP_TYPE_LOCK || type == EQUIP_TYPE_WING;
	return false;
}

/**
 *@brief �ж��Ƿ���Ը���
 */
inline bool IsEquipCanRemake(int type)
{
	//return type != EQUIP_TYPE_INVALID && type < EQUIP_TYPE_WEAPON_MAX;
	return false;
}

/**
*@brief �ж��Ƿ�Լ�����
*/
inline bool IsChiJiType(int type)
{
	switch (type)
	{
	case ST_CHIJI_HP:break;
	case ST_CHIJI_SPEED:break;
	case ST_CHIJI_THROW:break;
	case ST_CHIJI_BUFF:break;
	case ST_CHIJI_SKILL:break;
	default:
		return false;
	}

	return true;
}

/**
 *@brief ��������ʯ������
 */
enum PackActiveItemType
{
	ITEM_NORMALPACK_ACTIVE = 0,		//��ͨ��������
	ITEM_STORAGE_ACTIVE =	 1,		//�ֿ�����
};

/**
 *@brief ҩ��������
 */
enum MedicineType
{
	MEDICINE_TYPE_INVALID = 0,		//��Ч����
	MEDICINE_TYPE_HP = 1,			//��Ѫ ����ֶ�1��ֵ�����ֶ�2�ٷֱ���
	MEDICINE_TYPE_HPCONTINUED = 2,	//������Ѫ �ֶ�1buff��id���ֶ�2����ʱ��(����)
};

/**
 *@brief �������������
 */
enum TaskItemType
{
	TASKITEM_NOUSE = 0,			//����ʹ�õ���
	TASKITEM_USE_TARGET = 1,	//��ҪĿ��ʹ��		����ֶ�1Ϊ����ʱ�䣨�룩,0��ʾ������  
	TASKITEM_USE_NOTARGET = 2,	//����ҪĿ��ʹ��	����ֶ�1Ϊ����ʱ�䣨�룩,0��ʾ������
};

/**
 *@brief �ű�����������
 */
enum ScriptItemType
{
	SCRIPTITEM_USE_NOTARGET = 0,	//����ҪĿ��ʹ�� ����ֶ�1Ϊ����ʱ�䣨�룩,0��ʾ������
	SCRIPTITEM_USE_TARGET = 1,		//��ҪĿ��ʹ��   ����ֶ�1Ϊ����ʱ�䣨�룩,0��ʾ������
	SCRIPTITEM_USE_OPTION = 2,		//������ѡ��Ľű�����
	SCRIPTITEM_USE_TARGET_PLAYER = 3,	//��Ҫ���Ŀ��ʹ��   ����ֶ�1Ϊ����ʱ�䣨�룩,0��ʾ������
};

/**
*@brief �������������
*/
enum PetItemType
{
	PETITEM_INVALID = 0,		// ��Ч����
	PETITEM_EGG = 1,			// ���ﵰ	����ֶ�1Ϊ����id
	PETITEM_SKILLBOOK = 2,		// ������	����ֶ�1Ϊ����id���ֶ�2Ϊ���ܵȼ�
	PETITEM_EMPTYBOOK = 3,		// ��������
	PETITEM_APTITUDE = 4,		// ���ʵ�
	PETITEM_FOOD = 5,			// ��������
	PETITEM_GATHERSOUL = 6,		// �ۻ굤   ����ֶ�1Ϊ���Ӿۻ�ֵ
};

/**
*@brief �ر�ͼ������
*/
enum TreasureMapType
{
	TREASUREMAP_INVALID = 0,
	TREASUREMAP_UNUSED = 1,		// δʹ��
	TREASUREMAP_USED =2,		// ��ʹ��
};

/**
*@brief ��Ϸ��������(����)
*/
enum MoneyType
{
	MONEY_INVALID = 0,
	MONEY_BINDSILVER = 20,	//		 =====����
	MONEY_GOLD = 1,			// ���
	MONEY_POINT = 2,		// ��ȯ
	MONEY_EXP = 3,			// ����
	MONEY_JADESTONE = 4,	// ��ʯ(����)
	MONEY_HONOUR = 5,		// ����(����)
	MONEY_BINDGOLD = 6,		// �󶨽�(����)
	MONEY_POINT_DEPRACTED = 7,		// ����(����)
	MONEY_IRONSTONE = 8,	// ��ʯ(����)
	MONEY_SPIRITSTONE = 9,	// ��ʯ(����)
    MONEY_COUPON = 10,      // �ㄻ(����)
    MONEY_WARRIOR_SOUL = 11,  // ����֮��
	MONEY_PK_COIN = 12,		// ������
	MONEY_REVIVE_COIN = 13,	// �����
	MONEY_GUILD_CONTRI = 14,// ���ṱ��
	MONEY_CREDIT_POINT = 15,// ���õ�ȯ

	MONEY_ITEM_EX = 21,		// ���ﻻ��
};

/**
 *@brief �ܵ�½���Ƶĵ���������
 */
enum LoginLimitItemType
{
	LOGINLIMIT_ITEM_INVALUD = 0,
	LOGINLIMIT_ITEM_BINDGOLD = 1,	//������  ,����ֶ�1���÷����ܶ�
};

/**
*@brief ������������
*/
enum WingItemType
{
	WING_ITEM_INVALID = 0,
	WING_ITEM_FEATHER = 1,		// ��ë
	WING_ITEM_LEVELUP = 2,		// �����������
	WING_ITEM_LEVELDOWN = 3,	// ��򽵼�����
	WING_ITEM_FEATHERSOUL = 4,	// ������
};

enum ShopItemExlimite
{
	SIE_NONE = 0,
	SIE_DUEL_POINT,
	SIE_TOWER_LEVEL,
	SIE_GUILD_SHOP_LV,
	SIE_HONOR_LEVEL,
};

enum ItemUseLimiteType
{
	IULT_NONE = 0,		
	IULT_DAYLIMITE,		//�����ƴ���
	IULT_VIPLIMITE,		//vip���ƴ���
	IULT_TEAMCOPY,		//�ű�����
	IULT_HONORLV,		//�����ȼ�����
	IULT_SUITLEV,		//ʹ����װ�ȼ�
	IULT_WEEK6LIMITE,	//�����ƴ���
	IULT_MAX,
};

/**
 *@brief �жϵ��������Ƿ����Ʒ�ʣ������������ϸ��Ϣ
 */
inline bool IsDetailInfoItem(int type, int subtype, unsigned int deadline)
{
	if(0 != deadline) return true;

	switch(type)
	{
	case ITEM_EQUIP:
	case ITEM_TELEPORT:
		return true;
	}
	return false;
}


// ����Ĳ���������
#define MESS_TRIBE_TYPE	8
//С��Ь
#define MESS_FLY_SHOES	31815
//������
#define	TRANS_WOOD_PASSPORT 1107
//С����
#define ITEM_HORN 9
//�������
#define ITEM_REVIVE 10
//Ӣ�������
#define ITEM_HERO_TOKEN	126

//������
#define ITEM_CHANGENAME_ID			45
//�ڰ�������
#define ITEM_DT_FORGETSKILL_ID		47
//���Է�
#define ITEM_CHANGESEX_ID			656

// ���
#define ITEM_GOLD					600000001
// ��ȯ
#define ITEM_POINT					600000002
// �������
#define ITEM_EXP					600000003
// �󶨵�ȯ
#define ITEM_BIND_POINT				600000008
// ���õ�ȯ
#define ITEM_CREDIT_POINT			600002551
// �����ҵ���
#define ITEM_WARRIOR_SOUL			300001101
// ����֮�����³�ID
#define DEATH_TOWER_DUNGEON_ID		2003000


// ��һ�λ�õúϳ�ʱװ���
enum LastGetSkyFashionType
{
	LGSFT_NONE = 0,
	LGSFT_NORMAL = 1,
	LGSFT_GOLD = 2,
};

enum ItemOperateEnum
{
	IPOE_INVALE = 0,
	IPOE_STRENG = 1,    //ǿ��
	IPOE_UPGRADE = 2,   //����
	IPOE_ADDMAGIC = 3,  //��ħ
	IPOE_MOUNTBEAD = 4, //����
	IPOE_DECOMPOSE = 5, //�ֽ�
	IPOE_ENHANCE = 6,   //����
	IPOE_CLEARSMELL = 7, //�����Ϣ
	IPOE_REDACTIVE = 8, //����
	IPOE_REDCHAGE = 9, //ת��
};

#endif
