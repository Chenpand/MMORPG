#ifndef _CL_ITEM_DEFINE_H_
#define _CL_ITEM_DEFINE_H_

#include <AvalonNetStream.h>
#include <json/writer.h>
#include "CLItemTypeDefine.h"
#include "CLObjectProperty.h"

/**
 *@brief ��������
 */
enum ItemAttr
{
	IA_INVALID,			//��Ч����
	IA_NUM,				//������	UInt16
	IA_BIND,			//��		UInt8
	IA_PACK,			//����		UInt8
	IA_GRID,			//��������	UInt8
	IA_PHY_ATK,			//������		UInt32
	IA_MAG_ATK,			//��������
	IA_PHY_DEF,			//�������		UInt32
	IA_MAG_DEF,			//��������		UInt32
	IA_STR,				//����		UInt32
	IA_STAMINA,			//����		UInt32
	IA_INTELLECT,		//����		UInt32
	IA_SPIRIT,			//����		UInt32
	IA_QUALITYLV,		//Ʒ��
	IA_QUALITY,			//װ��Ʒ��	UInt8 EquipQuality
	IA_STRENGTHEN,		//ǿ���ȼ�	UInt8
	IA_RANDATTR,		//�������	��ʽ��[PkValueID(UInt8) + ModPkValueType(UInt8) + Value(Int32)]...[0(UInt8)]
	IA_DAYUSENUM,			//��ʹ�ô���		UInt32
	//IA_GEMSTONE,		//��ʯ      ��ʽ: [pos(UInt8 1-5)][ItemID(UInt32)][Level(UInt8)]...[0(UInt8)]
	IA_ADDMAGIC,		//��ħ		
	IA_PARAM1,			//����ֶ�1 UInt32
	IA_PARAM2,			//����ֶ�2	UInt32
	IA_POWER,			//ս����	UInt32
	IA_DEADLINE,		//ʧЧʱ��	UInt32	������Ч�ĵ���ֵΪ0
	IA_PRICE,			//�۸�		UInt32	������õļ۸�
	IA_STRFAILED,		//ǿ��ʧ�ܴ���	UInt16
	IA_SEAL_STATE,		//�Ƿ��װ UInt8
	IA_SEAL_COUNT,		//��װ���� UInt32
	IA_DIS_PHYATK,		//����������
	IA_DIS_MAGATK,		//���ӷ�������
	IA_DIS_PHYDEF,			//�����������
	IA_DIS_MAGDEF,			//���ӷ�������
	IA_PVP_DIS_PHYATK,		//PVP����������
	IA_PVP_DIS_MAGATK,		//PVP���ӷ�������
	IA_PVP_DIS_PHYDEF,			//PVP�����������
	IA_PVP_DIS_MAGDEF,			//PVP���ӷ�������
	IA_VALUE_SCORE,			//װ������
	IA_SET_QL_NUM,			//Ʒ����������
	IA_FASHION_ATTRID,		//ʱװ����id
	IA_FASHION_ATTR_SELNUM,	//ʱװ���Ը��Ĵ���		
	IA_PHYDEF_PERCENT,		//ǿ����������ٷֱ�
	IA_MAGDEF_PERCENT,		//ǿ��ħ�������ٷֱ�
	IA_ADD_PRECIOUSBEAD,	//��ӱ���
	IA_STRPROP_LIGHT,		//����ǿ
	IA_STRPROP_FIRE,		//����ǿ
	IA_STRPROP_ICE,			//����ǿ
	IA_STRPROP_DARK,		//����ǿ
	IA_DEFPROP_LIGHT,		//������
	IA_DEFPROP_FIRE,		//������
	IA_DEFPROP_ICE,			//������
	IA_DEFPROP_DARK,		//������
	IA_ABNORMAL_RESISTS_TOTAL,	//�쿹��
	IA_EAR_FLASH,		//�쿹�е�
	IA_EAR_BLEEDING,		//�쿹��Ѫ
	IA_EAR_BURN,		//�쿹����
	IA_EAR_POISON,		//�쿹�ж�
	IA_EAR_BLIND,		//�쿹ʧ��
	IA_EAR_STUN,		//�쿹��ѣ
	IA_EAR_STONE,		//�쿹ʯ��
	IA_EAR_FROZEN,		//�쿹����
	IA_EAR_SLEEP,		//�쿹˯��
	IA_EAR_CONFUNSE,		//�쿹����
	IA_EAR_STRAIN,		//�쿹����
	IA_EAR_SPEED_DOWN,		//�쿹����
	IA_EAR_CURSE,		//�쿹����
	IA_TRANSFER_STONE,	//ת��ʯitemid
	IA_RECO_SCORE,		//װ���������û���
	IA_IS_LOCK,			//������
	IA_PRECIOUSBEAD_HOLES, //������Ƕ��
	IA_AUCTION_COOL_TIMESTAMP, //�����н�����ȴʱ��
	IA_IS_TREAS,				//�Ƿ���Ʒ
	IA_BERD_EXTRIPE_CNT,		//�����ժ������
	IA_BERD_REPLACE_CNT,		//������滻����
	IA_DATA_ID,					//��ƷID
	IA_EQUIP_TYPE,       //װ������:��ͨ0/ǿ��1/����Ϣ2/����3
	IA_ENHANCE_TYPE,     //����װ����������:����0/����1/����2/����3
	IA_ENHANCE_NUM,      //����װ��������ֵ
	IA_ENHANCE_FAILED,   //����װ������ʧ�ܴ���
	IA_AUCTION_TRANS_NUM,   //�����н��״���
	IA_INSCRIPTION_HOLES,	//������Ƕ��
	IA_INDEPENDATK,			//����������
	IA_INDEPENDATK_STRENG,	//����������ǿ��
	IA_SUBTYPE,	//����������
	IA_KEYVALUES, //key-value�ֶ�
};

enum ZengfuOverload
{
	ZENGFU_OVERLOAD_MIN = 8,
	ZENGFU_OVERLOAD_MAX = 10,
};

enum WeaponBuildSpecial
{
	WBS_PINZHI = 1, //��ʥƷ��
	WBS_QIYUE = 2,  //ս����Լ
	WBS_DOUBLE = 3, // ˫��
	WBS_FORVER = 4, // ����
};

// �������
enum ItemRandProp
{
	IRP_NONE = 0,
	IRP_STR = 1,		//����
	IRP_STA = 2,		//����
	IRP_INT = 3,		//����
	IRP_SPR = 4,		//����
	IRP_HPMAX = 5,		//hp���ֵ
	IRP_MPMAX = 6,		//mp���ֵ
	IRP_HPREC = 7,		//hp�ָ�
	IRP_MPREC = 8,		//mp�ָ�
	IRP_HIT = 9,		//������
	IRP_DEX = 10,		//�ر���
	IRP_PHYCRT = 11,		//������
	IRP_MGCCRT = 12,		//ħ������
	IRP_ATKSPD = 13,		//����
	IRP_RDYSPD = 14,		//ʩ���ٶ�
	IRP_MOVSPD = 15,		//�ƶ��ٶ�
	IRP_JUMP = 16,		//��Ծ��
	IRP_HITREC = 17,		//Ӳֱ
	IRP_PHYATK = 18,		//�﹥
	IRP_MAGATK = 19,		//����
	IRP_PHYDEF = 20,		//���
	IRP_MAGDEF = 21,		//����	

	IRP_LIGHT = 22,       //�����Թ���
	IRP_FIRE = 23,		  //�����Թ���
	IRP_ICE = 24,		  //�����Թ���
	IRP_DARK = 25,			//�����Թ���
	IRP_LIGHT_ATK = 26, //������ǿ��
	IRP_FIRE_ATK = 27,	//������ǿ��
	IRP_ICE_ATK = 28,	//������ǿ��
	IRP_DARK_ATK = 29,	//������ǿ��
	IRP_LIGHT_DEF = 30, //�����Կ���
	IRP_FIRE_DEF = 31,	//�����Կ���
	IRP_ICE_DEF = 32,	//�����Կ���
	IRP_DARK_DEF = 33,	//�����Կ���
	IRP_GDKX = 34,		//�е翹��
	IRP_CXKX = 35,		//��Ѫ����
	IRP_ZSKX = 36,		//���տ���
	IRP_ZDKX = 37,		//�ж�����
	IRP_SMKX = 38,		//ʧ������
	IRP_XYKX = 39,		//ѣ�ο���
	IRP_SHKX = 40,		//ʯ������
	IRP_BDKX = 41,		//��������
	IRP_SLKX = 42,		//˯�߿���
	IRP_HLKX = 43,		//���ҿ���
	IRP_SFKX = 44,		//��������
	IRP_JSKX = 45,		//���ٿ���
	IRP_ZZKX = 46,		//���俹��
	IRP_YKXZ = 47,		//�����쳣����
	IRP_INDEPEND_ATK = 48, //����������
	IRP_MAX,
};

enum EquipExType
{
	EET_ATK = 0,	//���Թ���
	EET_STR = 1,	//����ǿ��
	EET_DEF = 2,	//���Կ���
	EET_AR = 3,		//�쳣����
	EET_MAX = 4,
};

//װ����������
enum EquipExProp
{
	EEP_LIGHT = 0,	//������
	EEP_FIRE = 1,	//������
	EEP_ICE = 2,	//������
	EEP_DARK = 3,	//������

	EEP_MAX = 4,
};

//װ���쳣����
enum EquipAbnormalResist
{
	EAR_FLASH = 0,		//�е�
	EAR_BLEEDING = 1,		//��Ѫ
	EAR_BURN = 2,		//����
	EAR_POISON = 3,		//�ж�
	EAR_BLIND = 4,		//ʧ��
	EAR_STUN = 5,		//��ѣ
	EAR_STONE = 6,		//ʯ��
	EAR_FROZEN = 7,		//����
	EAR_SLEEP = 8,		//˯��
	EAR_CONFUNSE = 9,		//����
	EAR_STRAIN = 10,		//����
	EAR_SPEED_DOWN = 11,		//����
	EAR_CURSE = 12,		//����
	

	EAR_MAX = 13,
};

//��������
enum ItemScoreProp
{
	ISP_NONE = 0,
	ISP_STR = 1,			//����
	ISP_INT = 2,			//����
	ISP_STAM = 3,			//����
	ISP_SPR = 4,			//����
	ISP_HP = 5,				//HP
	ISP_MP = 6,				//MP
	ISP_HPRECVR = 7,		//HP�ظ�
	ISP_MPRECVR = 8,		//MP�ظ�
	ISP_PHYATK = 9,			//�﹥
	ISP_MAGATK = 10,			//����
	ISP_PHYDEF = 11,			//���
	ISP_MAGDEF = 12,			//����
	ISP_DISPHYATK = 13,		//�����﹥
	ISP_DISMAGATK = 14,		//���ӷ���
	ISP_PHYDMGRDC = 15,		//�����˺�׷�Ӽ���
	ISP_MAGDMGRDC = 16,		//ħ���˺�׷�Ӽ���
	ISP_ATKSPD = 17,			//����
	ISP_MAGSPD = 18,			//ʩ���ٶ�
	ISP_MVSPD = 19,			//�ƶ��ٶ�
	ISP_PHYCRT = 20,			//������
	ISP_MAGCRT = 21,			//ħ������
	ISP_CRTDMG = 22,			//�����˺�
	ISP_HIT = 23,			//������
	ISP_MISS = 24,			//�ر���
	ISP_JZ = 25,				//��ֱ
	ISP_YZ = 26,				//Ӳֱ

	ISP_LIGHT = 27,       //�����Թ���
	ISP_FIRE = 28,		  //�����Թ���
	ISP_ICE = 29,		  //�����Թ���
	ISP_DARK = 30,			//�����Թ���
	ISP_LIGHT_ATK = 31, //������ǿ��
	ISP_FIRE_ATK = 32,	//������ǿ��
	ISP_ICE_ATK = 33,	//������ǿ��
	ISP_DARK_ATK = 34,	//������ǿ��
	ISP_LIGHT_DEF = 35, //�����Կ���
	ISP_FIRE_DEF = 36,	//�����Կ���
	ISP_ICE_DEF = 37,	//�����Կ���
	ISP_DARK_DEF = 38,	//�����Կ���
	ISP_GDKX = 39,		//�е翹��
	ISP_CXKX = 40,		//��Ѫ����
	ISP_ZSKX = 41,		//���տ���
	ISP_ZDKX = 42,		//�ж�����
	ISP_SMKX = 43,		//ʧ������
	ISP_XYKX = 44,		//ѣ�ο���
	ISP_SHKX = 45,		//ʯ������
	ISP_BDKX = 46,		//��������
	ISP_SLKX = 47,		//˯�߿���
	ISP_HLKX = 48,		//���ҿ���
	ISP_SFKX = 49,		//��������
	ISP_JSKX = 50,		//���ٿ���
	ISP_ZZKX = 51,		//���俹��

	ISP_HSWORD = 100, 			//�޽�
	ISP_TD = 101,				//̫��
	ISP_ZL = 102,				//����
	ISP_SP = 103,				//����
	ISP_FZ = 104,				//����
	ISP_MZ = 105,				//ħ��
	ISP_BJ = 106,				//����
	ISP_PJ = 107,				//Ƥ��
	ISP_DUANJIAN = 108,		//�̽�
	ISP_QINJIA = 109,			//���
	ISP_ZHONGJIA = 110,		//�ؼ�
	ISP_BANJIA = 111,			//���
	ISP_OFG = 112,			//����
	ISP_EAST_STICK = 113,		//������
	ISP_GLOVE = 114,		//����
	ISP_BIKAI = 115,		//����
	ISP_CLAW = 116,			//צ
	ISP_NUJIAN = 117,		//���
	ISP_BUQIANG = 118,		//��ǹ
	ISP_GUANGJIAN = 119,	//����
	ISP_SICKLE = 120,		//����
	ISP_TOTEM = 121,		//ͼ��
	ISP_AXE = 122,			//ս��
	ISP_BEADS = 123,		//����
	ISP_CROSS = 124,		//ʮ�ּ�
	ISP_SPEAR = 125,		//ì
	ISP_STICK = 126,		//����

	ISP_ATTACK = 200,		//����������
	ISP_POWER = 201,		//��ά�������
	ISP_REDUCE = 202,		//��������
	ISP_DISPHYATTACK = 203, //���ӷ�������
	ISP_DISPHYREDUCE = 204,	//���ӹ�������
	ISP_FUJIA = 205,		//��������ϵ��
	ISP_INDEPENDATK = 206,	//��������������
	ISP_MAX,
};

/**
 *@brief ������
 */
enum BindType
{
	BIND_TYPE_NONE = 0,		//����
	BIND_TYPE_ONGET = 1,	//��ð�
	BIND_TYPE_ONUSE = 2,	//ʹ�ð�
};

/**
 *@brief ����ѡ��
 */
enum ItemOption
{
	ITEM_OPTION_SALE	=			1 << 0,	//�Ƿ�ɳ��۸�npc
	ITEM_OPTION_TRADE	=			1 << 1,	//�Ƿ�ɽ���
	ITEM_OPTION_MAIL	=			1 << 2,	//�Ƿ���ʼ�
	ITEM_OPTION_DISCARD =			1 << 3,	//�Ƿ�ɶ���
	ITEM_OPTION_DESTROY =			1 << 4,	//�Ƿ������
	ITEM_OPTION_DROP_ONOFFLINE	=	1 << 5,	//�����Ƿ����
	ITEM_OPTION_DROP_ONCHSCENE	=	1 << 6,	//��ת��ͼ�Ƿ����
	ITEM_OPTION_STORAGE	=			1 << 7,	//��ֿ�
	ITEM_OPTION_DTOPONDIE =			1 << 8, //�Ƿ���������
	ITEM_OPTION_USE		=			1 << 9,	//�Ƿ��ʹ��
	ITEM_OPTION_BUY		=			1 << 10,//�Ƿ����npc����
	ITEM_OPTION_MUSE	=			1 << 11,//�Ƿ������ʹ��
	ITEM_OPTION_PICKUPBIND =		1 << 12,//ʰȡ��
	ITEM_OPTION_QUICKBAR =			1 << 13,//�Ƿ�ɷ�������
	ITEM_OPTION_FORBIDDEN_STR =		1 << 14,//�Ƿ��ֹǿ��
};


enum PotionConfigType
{
	POTION_CONFIG_TYPE_MAIN = 0,
	POTION_CONFIG_TYPE_HP   = 1,
	POTION_CONFIG_TYPE_MP   = 2,
};


/**
 *@brief ����Ч����������
 */
#define ITEM_EFFECT_PARAM_NUM	4

/**
*@brief װ��ǿ�����ȼ�
*/
#define ITEM_STRENTH_MAX	20
/**
 *@brief ��������(˳�򲻿ɸ�)
 */
enum PackType
{
	PACK_INVALID			= 0,	//��Ч
	PACK_EQUIP,						//װ������
	PACK_MATERIAL,					//���ϰ���
	PACK_EXPENDABLE,				//����Ʒ����
	PACK_TASK,						//������߰���
	PACK_FASHION,					//ʱװ����
	PACK_WEAR,						//��װ���İ���
	PACK_FASHION_WEAR,				//��װ����ʱװ����
	PACK_STORAGE,					//�ֿ�
	PACK_ENERGY_STONE,				//����ʯ	adder by huchenhui	2016.07.21
	PACK_TITLE,						//�ƺ�		adder by huchenhui	2016.07.21
	PACK_PET,						//�������	adder by huchenhui	2017.08.04
	PACK_EQRCOV,					//װ�����հ���
	PACK_ROLE_STORAGE,				//��ɫ�ֿ⣬��Ѳֿ�
	PACK_INSCRIPTION,				//���İ���
	PACK_LOSTDUNGEON,				//��ʧ���ΰ���
	PACK_MAX,

	PACK_NORMAL,					//������ͨ����(����)
	PACK_BUYBACK,					//�ع�����(����)
	PACK_LOTTERY,					//���齱�ֿ�(����)
	PACK_WING,						//���(����)
};

/**
*@brief �����������
*/
enum TribePackType
{
	TRIBE_PACK_INVALID	= 0,		// ��Ч
	TRIBE_PACK_STORAGE	= 1,		// ����ֿ�
	TRIBE_PACK_SHOP		= 2,		// �����̵�
	TRIBE_PACK_MAX,
};

/**
 *@brief �Ƿ�����Я���İ���
 */
inline bool IsCarryOnPackage(int packType)
{
	switch(packType)
	{
	case PACK_EQUIP:					
	case PACK_MATERIAL:					
	case PACK_EXPENDABLE:				
	case PACK_TASK:						
	case PACK_FASHION:					
	case PACK_ENERGY_STONE:
	case PACK_TITLE:
	case PACK_WEAR:
	case PACK_FASHION_WEAR:
	case PACK_PET:
	case PACK_INSCRIPTION:
		return true;
	}
	return false;
}

/**
 *@brief ������С
 */
const static UInt32 PackSize[PACK_MAX] = { 0 , 1000, 1000, 1000, 1000, 1000, 14, 8, 100, 20, 1000, 80, 20, 270, 1000, 10 };
const static UInt32 MAX_GRID_NUM_PER_ROLE_STORAGE_PAGE = 30;

/**
*@brief �����������
*/
const static UInt8 TribePackSize[TRIBE_PACK_MAX] = {0, 168, 50};

const static Int32 WEAPON_GRID = 9;
#define EQUIP_GRID_NUM 10
/**
 *@brief ����װ�����ͻ�ȡװ��λ��
 */
inline UInt8 GetGridByEquipType(int type)
{
// 	if(type <= EQUIP_TYPE_SHOES) return UInt8(type - 1);
// 	if(type > EQUIP_TYPE_SHOES && type <= EQUIP_TYPE_WEAPON_MAX) return WEAPON_GRID;
// 	return UInt8(WEAPON_GRID + type - EQUIP_TYPE_WEAPON_MAX - 1);
	return 255;
}

/**
*@brief ����װ�����ͻ�ó��װ��λ��
*/
inline UInt8 GetWingGridByEquipType(int type)
{
// 	switch(type)
// 	{
// 	case EQUIP_TYPE_AMULET:		//�����
// 		return 9;
// 	case EQUIP_TYPE_CLOTHES:	//�·�
// 		return 0;
// 	case EQUIP_TYPE_HANDGUARD:	//����
// 		return 2;
// 	case EQUIP_TYPE_BELT:		//����
// 		return 4;
// 	case EQUIP_TYPE_LEGGUARD:	//����
// 		return 6;
// 	case EQUIP_TYPE_TORQUE:		//����
// 		return 3;
// 	case EQUIP_TYPE_RING:		//��ָ
// 		return 5;
// 	case EQUIP_TYPE_BANGLE:		//����
// 		return 7;
// 	case EQUIP_TYPE_SHOES:		//Ь��
// 		return 8;
// 	}
// 	
// 	if(type > EQUIP_TYPE_SHOES && type <= EQUIP_TYPE_WEAPON_MAX) return 1;
	return 255;
}

/**
 *@brief ����λ��
 */
struct ItemPos
{
	ItemPos():pack(0),gridIndex(0){}
	ItemPos(UInt8 _pack, UInt32 _grid) :pack(_pack), gridIndex(_grid){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & pack & gridIndex;
	}

	bool operator==(const ItemPos& pos) const
	{
		return pack == pos.pack && gridIndex == pos.gridIndex;
	}

	//����
	UInt8	pack;
	//��������
	UInt32	gridIndex;
};


/**
 *@brief ��Ч�ĵ���λ��
 */
#define INVALID_ITEMGRID UInt32(-1)
const static ItemPos INVALID_ITEMPOS;

//��ȡ��ɫ����ҳ
inline UInt32 GetRoleStoragePageByGrid(UInt32 gridIndex)
{
	if (gridIndex == INVALID_ITEMGRID)
	{
		gridIndex = 0;
	}

	if (gridIndex >= PackSize[PACK_ROLE_STORAGE])
	{
		return UInt32(-1);
	}

	return gridIndex / MAX_GRID_NUM_PER_ROLE_STORAGE_PAGE;
}

/**
 *@brief ����Ʒ��
 */
enum ItemQuality
{
	ITEM_QUALITY_INVLID = 0,
	ITEM_QUALITY_WHITE,			//��
	ITEM_QUALITY_BLUE,			//�� �߼�
	ITEM_QUALITY_PURPLE,		//�� ϡ��
	ITEM_QUALITY_GREEN,			//�� ����
	ITEM_QUALITY_PINK,			//�� ����
	ITEM_QUALITY_YELLOW,		//�� ��˵
	ITEM_QUALITY_SILVERY = 7,   //�� ��ʥ

	ITEM_QUALITY_MAX,
};

/**
*@brief ����Ʒ��2
*/
enum ItemQuality2
{
	ITEM_QUALITY2_INVLID = 0,
	ITEM_QUALITY2_GRAY,			//��ɫ
	ITEM_QUALITY2_WHITE,		//�� 
	ITEM_QUALITY2_ROSERED,		//õ��

	ITEM_QUALITY2_MAX,
};

/**
*@brief ����Ʒ��λö�٣�������
*/
enum ItemBitQuality
{
	IBQ_WHITE	= 1 << 0,			//��
	IBQ_BLUE	= 1 << 1,			//��
	IBQ_PURPLE	= 1 << 2,			//��
	IBQ_GREEN	= 1 << 3,			//��
	IBQ_PINK	= 1 << 4,			//��
	IBQ_YELLOW	= 1 << 5,			//��
};

/**
*@brief װ��Ʒ��
*/
enum EquipQualityLv
{
	EQL_INVALID = 0,			
	EQL_LOWEST,					//���¼�
	EQL_LOW,					//�¼�
	EQL_MID,					//�м�
	EQL_HIGH,					//�ϼ�
	EQL_HIGHEST,				//���ϼ�

	EQL_MAX,
};



/**
 *@brief װ��Ʒ����
 */
#define EQUIP_QUALITY_MAX 6



/**
 *@brief ���ݵ������͹淶Ʒ��
 */
inline UInt8 ValidateItemQuality(int type, int subtype, UInt8 quality)
{
	switch(type)
	{
	case ITEM_EQUIP:
		if(quality >= EQUIP_QUALITY_MAX) return EQUIP_QUALITY_MAX - 1;
		return quality;
	case ITEM_SCRIPT:
		if(quality >= EQUIP_QUALITY_MAX) return EQUIP_QUALITY_MAX - 1;
		return quality;
	case ITME_GEMSTONE:
		if(quality == 0) return 1;
		if(quality > 15) return 15;
		return quality;
	case ITEM_RUNE:
		if(quality == 0) return 1;
		if(quality > 5) return 5;
		return quality;
	case ITEM_WING:
		if(WING_ITEM_FEATHER == subtype)
		{
			if(quality >= EQUIP_QUALITY_MAX) return EQUIP_QUALITY_MAX - 1;
			return quality;
		}
		return 0;
	};
	return 0;
}


/**
 *@brief װ��Ʒ������
 */
struct EquipQualityCfg
{
	UInt32			baseAttrRate;				//�������Լӳ�
	UInt32			randAttrRate;				//������Լӳ�
};

const static EquipQualityCfg  EQUIP_QUALITY_CFG[] = {
	{10000, 5000},
	{20000, 6000},
	{25000, 7000},
	{30000,	8500},
	{35000,	10000},
	{40000, 15000},
};

/**
*@brief װ�����ȼ�
*/
#define MAX_EQUIP_LEVEL 100
/**
*@brief װ���ȼ�������
*/
#define EQUIP_SECTION_NUM (MAX_EQUIP_LEVEL / 10 + 1)
/**
*@brief ���װ���ȼ���
*/
inline UInt16 GetEquipLevelSection(UInt16 level)
{
	return level / 10;
}

/**
*@brief ��������������
*/
#define MAX_EQUIPATTR_STAR_NUM 25


/**
*@brief װ�������ǿ���ȼ�
*/
const UInt8	EQUIP_MAX_STR_LEVEL[EQUIP_SECTION_NUM][EQUIP_QUALITY_MAX] = {{0, 9, 9, 0, 0, 0},
																		 {0, 9, 9, 0, 0, 0},
																		 {0, 9, 9, 12, 0, 0},
																		 {0, 9, 9, 12, 18, 21},
																		 {0, 9, 9, 12, 18, 21},
																		 {0, 9, 9, 12, 18, 21},
																		 {0, 15, 15, 18, 24, 27},
																		 {0, 15, 15, 18, 24, 27},
																		 {0, 18, 18, 21, 27, 30},
																		 {0, 18, 18, 21, 27, 30},
																		 {0, 0, 0, 0, 0, 30},
																		};

//���ʯ����
#define MAX_EQUIP_GEM_HOLE 4
/**
*@brief ��ʯ��������
*/
const UInt8 EQUIP_HOLE_CFG[EQUIP_SECTION_NUM][MAX_EQUIP_GEM_HOLE] = {
	{255, 255, 255, 255},
	{255, 255, 255, 255},
	{255, 255, 255, 255},
	{3, 6, 9, 12},
	{6, 9, 12, 15},
	{6, 9, 12, 15},
	{9, 12, 15, 18},
	{9, 12, 15, 18},
	{12, 15, 18, 21},
	{12, 15, 18, 21},
	{12, 15, 18, 21},
};
/**
 *@brief ����ǿ���ȼ���ȡ����
 */
inline UInt32 GetHoleNumByStrengthenLvl(UInt16 equiplvl, UInt8 level)
{
	UInt32	uSection = GetEquipLevelSection(equiplvl);

	UInt32	uHoleNum = MAX_EQUIP_GEM_HOLE;
	for(; uHoleNum > 0; uHoleNum--)
	{
		if(level >= EQUIP_HOLE_CFG[uSection][uHoleNum - 1])
			return uHoleNum;
	}

	return 0;
}


/**
*@brief ÿ��ǿ��ʧ�����ӵĳɹ���
*/
#define EQUIP_STR_RATE_PER_FAILED	500
/**
*@brief vip�ȼ����ӵ�ǿ���ɹ���
*/
#define EQUIP_STR_RATE_VIP_ADD	200

/**
 *@brief ��ʯ���������
 */
#define MAX_GEMSTONE_ATTRNUM 3
/**
 *@brief ������������
 */
#define MAX_EQUIPATTR_NUM 3
/**
*@brief ���ħ��
*/
#define MAX_MAGIC_NUM 5
/**
 *@brief ���ǿ���ȼ�
 */
#define MAX_STRENGTHEN_LEVEL 15

/**
 *@brief �������˥��ϵ��
 */
#define RANDATTR_RATE_FACTOR 0.4f
/**
 *@brief �ϳ�һ����ʯ��Ҫ�ı�ʯ��
 */
#define GEMSTONE_COMPOSE_SRC_NUM 4
/**
 *@brief ����ʹ��һ�����ʹ����
 */
#define MAX_USE_ITEMNUM_ONCE 30

/**
 *@brief װ����������
 */
enum EquipRemakeType
{
	EQUIP_REMAKE_INLAY	=			1,	//��Ƕ
	EQUIP_REMAKE_STRENGTHEN =		2,	//ǿ����param1 Ŀ��ȼ���0 ǿ��1������0 �Զ�ǿ����Ŀ��ȼ�
	EQUIP_REMAKE_XIYUAN =			3,	//ϴԪ��������id ��װ����param1 �����������(��0��ʼ)
	//TODO
	EQUIP_REMAKE_TAKEAPART =		5,	//���
	EQUIP_REMAKE_TAKEAPART_STONE =	6,	//��ж��ʯ
	EQUIP_REMAKE_COMPOSE_STONE =	7,	//��ʯ�ϳ�
	EQUIP_REMAKE_INHERIT_ATTRS =	8,	//�̳�����
	EQUIP_REMAKE_UPGRADE =			10,	//װ������
	EQUIP_REMAKE_UPGRADEQUALITY =	11,	//װ��Ʒ��������param1 Ŀ��Ʒ�ʣ�4 ��װ��5 ����װ
	EQUIP_REMAKE_INLAY_RUNE =		19,	//������Ƕ��������id ����
	EQUIP_REMAKE_TAKEAPART_RUNE =	20,	//���Ĳ�ж
};

/**
*@brief �ʼ�����
*/
struct MailItemReward
{
public:
	MailItemReward() :id(0), num(0), strenth(0), auctionCoolTimeStamp(0), auctionTransTimes(0){}
	MailItemReward(UInt32 _id, UInt32 _num, UInt8 _strenth, 
		std::string _reason, UInt32 _auctionCoolTimeStamp, UInt32 _auctionTransTimes)
		:id(_id), num(_num), strenth(_strenth), reason(_reason), 
		auctionCoolTimeStamp(_auctionCoolTimeStamp), auctionTransTimes(_auctionTransTimes){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & num & strenth & reason & auctionCoolTimeStamp & auctionTransTimes;
	}

public:
	//����id
	UInt32 id;
	//��������
	UInt32 num;
	//ǿ���ȼ�
	UInt8 strenth;
	//��Դ
	std::string reason;
	//�����н�����ȴʱ��
	UInt32 auctionCoolTimeStamp;
	//�����н��״���
	UInt32 auctionTransTimes;
};

/**
 *@brief ����
 */
struct ItemRewardBase
{
	ItemRewardBase() : itemId(0), itemNum(0) {}
	ItemRewardBase(UInt32 id, UInt32 num) : itemId(id), itemNum(num) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & itemId & itemNum;
	}

	UInt32 itemId;
	UInt32 itemNum;
}; 

/**
 *@brief ���߽���, modified by aprilliu, 2016.04.12
 */
struct ItemReward
{
public:
	ItemReward() :id(0), num(0), qualityLv(0), strenth(0), auctionCoolTimeStamp(0), equipType(0), auctionTransTimes(0){}
	ItemReward(UInt32 _id, UInt32 _num, UInt8 _qualityLv, UInt8 _strenth)
		:id(_id), num(_num), qualityLv(_qualityLv), strenth(_strenth), auctionCoolTimeStamp(0), equipType(0), auctionTransTimes(0) {}
	ItemReward(UInt32 _id, UInt32 _num, UInt8 _qualityLv, UInt8 _strenth, UInt32 _auctionCoolTimeStamp)
		:id(_id), num(_num), qualityLv(_qualityLv), strenth(_strenth), auctionCoolTimeStamp(_auctionCoolTimeStamp), equipType(0), auctionTransTimes(0) {}
	ItemReward(UInt32 _id, UInt32 _num, UInt8 _qualityLv, UInt8 _strenth, UInt32 _auctionCoolTimeStamp, UInt8 _equipType)
		:id(_id), num(_num), qualityLv(_qualityLv), strenth(_strenth), auctionCoolTimeStamp(_auctionCoolTimeStamp), equipType(_equipType), auctionTransTimes(0) {}
	ItemReward(UInt32 _id, UInt32 _num, UInt8 _qualityLv, UInt8 _strenth, UInt32 _auctionCoolTimeStamp, UInt8 _equipType, UInt32 _auctionTransTimes)
		:id(_id), num(_num), qualityLv(_qualityLv), strenth(_strenth), auctionCoolTimeStamp(_auctionCoolTimeStamp), equipType(_equipType), auctionTransTimes(_auctionTransTimes){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		//stream & id & num & bind & quality;
		stream & id & num & qualityLv & strenth & auctionCoolTimeStamp & equipType & auctionTransTimes;
	}

	
	void DecodeString(std::istringstream& is)
	{
		/*
		char split = 0;
		int val = 0;
		is >> id >> split >> val;
		bind = val;
		is >> split >> val >> split >> num;
		quality = val;
		*/
		char split = 0;
		is >> id >> split >> num;
	}

	void DecodeStringForMail(std::istringstream& is)
	{
		/*
		char split = 0;
		int val = 0;
		is >> id >> split >> val;
		bind = val;
		is >> split >> val >> split >> num;
		quality = val;
		*/
		std::string str;
		is >> str;
		std::vector<std::string> ss;

		Avalon::StringUtil::Split(str, ss, "_");

		if (ss.size() >= 2)
		{
			id = Avalon::StringUtil::ConvertToValue<UInt32>(ss[0]);
			num = Avalon::StringUtil::ConvertToValue<UInt32>(ss[1]);
		}

		if (ss.size() >= 3)
		{
			strenth = Avalon::StringUtil::ConvertToValue<UInt8>(ss[2]);
		}

		if (ss.size() >= 4)
		{
			equipType = Avalon::StringUtil::ConvertToValue<UInt8>(ss[3]);
		}

		if (ss.size() >= 5)
		{
			auctionTransTimes = Avalon::StringUtil::ConvertToValue<UInt32>(ss[4]);
		}
		//char split = 0;
		//is >> id >> split >> num;
	}

	void EncodeString(std::ostringstream& os, char split = '_') const
	{
		//os << id << '_' << int(bind) << '_' << int(quality) << '_' << num;
		os << id << split << num << split << (UInt32)strenth << split << (UInt32)equipType 
			<< split << auctionTransTimes;
	}

	static void DecodeStringToItemRewardsForMail(std::string& ss, std::vector<ItemReward>& itemRewards)
	{
		if (ss.size() == 0)
		{
			return;
		}
		std::vector<std::string> ssv;
		Avalon::StringUtil::Split(ss, ssv, ";");

		for (auto s : ssv)
		{
			ItemReward item;
			std::istringstream stream(s);
			item.DecodeStringForMail(stream);
			if (item.id == 0)
			{
				ErrorLogStream << "err item.id = 0" << LogStream::eos;
				continue;
			}
			if (item.num == 0)
			{
				ErrorLogStream << "err item.num = 0" << LogStream::eos;
				continue;
			}
			itemRewards.push_back(item);
		}
		return;
	}

public:
	//����id
	UInt32 id;
	//��������
	UInt32 num;
 	//����Ʒ��
 	UInt8  qualityLv;
 	//ǿ���ȼ�
 	UInt8 strenth;
	//�����н�����ȴʱ��
	UInt32 auctionCoolTimeStamp;
	//װ������(��Ӧö��EquipType)
	UInt8  equipType;
	//�����н��״���
	UInt32 auctionTransTimes;
};
typedef std::vector<ItemReward> ItemRewardVec;

//���޽��
struct OpenJarResult
{
	OpenJarResult() :_JarItemId(0), _ItemUId(0) {}
	
	AVALON_DEFINE_NETSERIALIZATION()
	{
		//stream & id & num & bind & quality;
		stream & _JarItemId & _ItemUId;
	}

public:
	UInt32	_JarItemId;
	UInt64	_ItemUId;
};

//ְҵ��Ʒ����
struct OccuItemReward
{
	Occupation occu;
	ItemReward itemReward;
};


/**
*@brief �������Ϻϳ�Ŀ��������Χ
*/
enum EnhanceComboMaterial
{
	ENHANCE_COMBO_MATERIAL_NUM_MIN = 1,
	ENHANCE_COMBO_MATERIAL_NUM_MAX = 99,
};

/**
 *@brief �ϳ����ԭ����
 */
#define MAX_ITEM_COMPOSE_MATERIAL_NUM 5

/**
 *@brief ǿ����װЧ���ӳ�
 */
struct StrengthenSuitValue
{
	Int32	attack;
	Int32	defense;
	Int32	thump;
	Int32	holdoff;
	Int32	hp;
};

const StrengthenSuitValue STRENGTHEN_SUIT_VALUES[] = 
{
	{ 200, 200, 200, 200, 2000 },
	{ 300, 300, 300, 300, 3000 },
	{ 400, 400, 400, 400, 4000 },
	{ 600, 600, 600, 600, 6000 },
	{ 700, 700, 700, 700, 7000 },
	{ 800, 800, 800, 800, 8000 },
	{ 1200,	1200, 1200, 1200, 12000 },
	{ 1400, 1400, 1400, 1400, 14000 },
	{ 1800, 1800, 1800, 1800, 18000 }
};


/**
 *@brief ��ʯ��װЧ��
 */
struct GemstoneSuitValue
{
	UInt32	stoneNum;
	UInt32	value;
};

const GemstoneSuitValue GEMSTONE_SUIT_VALUES[6][2] = 
{
	{{25, 200},		{50, 200}},
	{{25, 400},		{50, 500}},
	{{25, 700},		{50, 800}},
	{{25, 1000},	{50, 1100}},
	{{25, 1500},	{50, 1700}},
	{{50, 2000},	{50, 2000}},
};

//�����������Ǽ�
#define MAX_RANDATTR_STAR 18

/**
 *@brief ���������װЧ��
 */
const Int32 RANDATTR_SUIT_VALUES[] = 
{
	500,
	600,
	700,
	900,
	1100,
	1300,
	1500,
	1700,
	1900,
	2100
};

/**
 *@brief ���߶Ի���ѡ��
 */
struct ItemDialogOption
{
public:
	ItemDialogOption():id(0),icon(0){}
	~ItemDialogOption(){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & title & icon;
	}

public:
	//ѡ��id
	UInt64	id;
	//ѡ����
	std::string	title;
	//icon
	UInt8	icon;
};


/**
 *@brief ���߶һ���Ϣ                                                                     
 */
struct ItemExchangeInfo
{
	ItemExchangeInfo():itemId(0), quality(0){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & itemId & quality & items;
	}

	UInt32	itemId;
	UInt8	quality;
	std::map<UInt32, UInt16>	items;
};

/**
*@brief ���߼�����������
*/
enum ItemLockType1
{
	ILT_NEW_PASSWD = 0,			//����������
	ILT_VERIFY_PASSWD = 1,		//��֤����
	ILT_FORGET_PASSWD = 2,		//��������
	ILT_RESET_PASSWD = 3,		//��������
	ILT_CACEL_LOCK = 4,			//ȡ������
	ILT_LOCK_RANGE = 5,			//������Χ	
};

/**
*@brief ������Χ
*/
enum ItemLockRangeType
{
	ILRT_EQUIP_PACK = 1 << 0,	//����װ��
	ILRT_NORMAL_PACK = 1 << 1,	//����
	ILRT_STORAGE_PACK = 1 << 2, //�ֿ�
	ILRT_PET = 1 << 3,			//����
	ILRT_MOUNT = 1 << 4,		//����

	//����
	ILRT_ALL = ILRT_EQUIP_PACK | ILRT_NORMAL_PACK | ILRT_STORAGE_PACK 
			   | ILRT_PET | ILRT_MOUNT,
};

/**
*@brief ���߼�����ť״̬
*/
enum ItemLockButtonType
{
	ILBT_INIT = 0,	//δ��������״̬,��ɫ
	ILBT_LOCK = 1,  //�����������״̬ ��ɫ
	ILBT_UNLOCK = 2,//����״̬ ��ɫ��
};

//���ֿ�ID
#define PANIC_BUY_CARD 32173

struct RaceItemRandProperty
{
    AVALON_DEFINE_NETSERIALIZATION()
    {
        stream & type & value;
    }

    // ���ͣ�ItemRandProp��
    UInt8   type;
    // �������ֵ
    UInt32  value;
};

struct RacePrecBead
{
	RacePrecBead()
	{
		preciousBeadId = 0;
		buffId = 0;
	}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & preciousBeadId & buffId;
	}
	//����id
	UInt32	preciousBeadId;
	//����buffid
	UInt32	buffId;
};

struct RaceEquip
{
	RaceEquip()
	{
		uid = 0;
		id = 0;
		pos = 0;
		phyAtk = 0;
		magAtk = 0;
		phyDef = 0;
		magDef = 0;
		strenth = 0;
		stamina = 0;
		intellect = 0;
		spirit = 0;
		cardId = 0;
		disPhyAtk = 0;
		disMagAtk = 0;
		disPhyDef = 0;
		disMagDef = 0;
		strengthen = 0;
		fashionAttrId = 0;
		phyDefPercent = 0;
		magDefPercent = 0;
		preciousBeadId = 0;
		isTimeLimit = 0;
		magicCardLv = 0;
	    equipType = 0;
		enhanceType = 0;
		enhanceNum = 0;
		independAtk = 0;
		indpendAtkStreng = 0;
	}

    AVALON_DEFINE_NETSERIALIZATION()
    {
		stream & uid & id & pos & phyAtk & magAtk & phyDef & magDef & strenth & 
			stamina & intellect & spirit & properties & cardId & disPhyAtk &  
			disMagAtk & disPhyDef & disMagDef & strengthen & fashionAttrId & phyDefPercent &
			magDefPercent & preciousBeadId & isTimeLimit & atkPropEx & strPropEx & defPropEx & abnormalResistsTotal & abnormalResists
			& mountPrecBeads & magicCardLv & equipType & enhanceType & enhanceNum & inscriptionIds
			& independAtk & indpendAtkStreng;
    }

	//Ψһid
	ObjID_t		uid;

    //װ��ID
    UInt32      id;
    //װ��λ��
	UInt32       pos;
    //������
    UInt32      phyAtk;
    //��������
    UInt32      magAtk;
    //�������
    UInt32      phyDef;
	//��������
	UInt32		magDef;
    //����
    UInt32      strenth;
    //����
    UInt32      stamina;
    //����
    UInt32      intellect;
    //����
    UInt32      spirit;
    //�������
    std::vector<RaceItemRandProperty> properties;
	//��ħ��
	UInt32		cardId;
	//����������
	UInt32		disPhyAtk;
	//���ӷ�������
	UInt32		disMagAtk;
	//���ӷ���(����10000��ԭ)
	UInt32		disPhyDef;
	UInt32		disMagDef;
	//����ٷֱ�
	UInt32		phyDefPercent;
	//ħ���ٷֱ�
	UInt32		magDefPercent;
	//ǿ��
	UInt8		strengthen;
	//ʱװ����id
	UInt32		fashionAttrId;
	//����
	UInt32		preciousBeadId;
	//�Ƿ�ʱЧ��
	UInt8		isTimeLimit;
	//������������
	std::vector<Int32>	atkPropEx;
	//ǿ����������
	std::vector<Int32>	strPropEx;
	//���Ը�������
	std::vector<Int32>	defPropEx;
	//�쳣������
	Int32		abnormalResistsTotal;
	//�쳣����
	std::vector<Int32>	abnormalResists;
	//��Ƕ����
	std::vector<RacePrecBead> mountPrecBeads;
	//��ħ���ȼ�
	UInt8 magicCardLv;
	//װ������:��ͨ0/����Ϣ1/����2
	UInt8 equipType;
	//����װ����������:����0/����1/����2/����3
	UInt8    enhanceType;
	//����װ��������ֵ
	UInt32   enhanceNum;
	//����ID����
	std::vector<UInt32> inscriptionIds;
	//����������
	UInt32	 independAtk;
	//����������ǿ���ӳ�
	UInt32	 indpendAtkStreng;
};


struct RaceItem
{
	RaceItem() {
		id = 0;
		num = 0;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & num;
	}

	//װ��ID
	UInt32      id;
	//����
	UInt16		num;
};

// ��Ʒ������Ϣ
struct ItemBaseInfo
{
	ItemBaseInfo()
	{
		id = 0;
		typeId = 0;
		pos = 0;
		strengthen = 0;
		equipType = 0;
		enhanceType = 0;
		equipScore = 0;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & typeId & pos & strengthen & equipType & enhanceType & equipScore;
	}

	// ΨһID
	ObjID_t		id;
	// ����ID
	UInt32		typeId;
	// λ��
	UInt32		pos;
	// ǿ��
	UInt8		strengthen;
	// װ������
	UInt8       equipType;
	//����·��
	UInt8       enhanceType;
	//װ������
	UInt32		equipScore;
};

// ���ٹ���Ŀ������
enum QuickBuyTargetType
{
	// ����
	QUICK_BUY_REVIVE,
	// �������
	QUICK_BUY_ITEM,

	QUICK_BUY_NUM,
};

//ǿ���ͷ�����
enum StrPunishType
{
	SPT_NONE = 0,	
	SPT_MINUSONE,	//��һ
	SPT_ZERO,		//����
	SPT_BROKEN,		//����
	SPT_MINUSMORE,	//�����2-3��
};

//�������
enum JarType
{
	JT_NONE = 0,
	JT_MAGIC,		//ħ��
	JT_GOLD,			//���
	JT_BATTLE,			//�����
	JT_MAGIC_55,		//55��ħ��
	JT_EQRECO = 5,			//װ������
	JT_GIFT,			//�������
	JT_NORMAL_MAX = 100,

	JT_WING,			//��������
	JT_FASHION,			//ʱװ�����
	JT_EQUIP,			//װ�������
	JT_ACT_MAX = 200,

	JT_MAX,
};

/**
*@brief �����Ӽ�¼
*/
struct OpenJarRecord
{
	OpenJarRecord()
	{
		uid = 0;
		jarId = 0;
		name = "";
		itemId = 0;
		num = 0;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & name & itemId & num;
	}

	ObjID_t uid;
	//����id
	UInt32 jarId;
	//����������
	std::string	name;
	//����id
	UInt32 itemId;
	UInt32 num;
};

// �������񣬾���ĵ���
struct DetailItem
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & guid & itemTypeId & num;
	}

	ObjID_t	guid;
	UInt32	itemTypeId;
	UInt32	num;
};


// ������
enum GiftPackFilterType
{
	GPFT_NONE = 0,
	GPFT_OCCU,				//ְҵɾѡ
	GPFT_RANDOM,			//���ɾѡ
	GPFT_CUSTOM,			//�ֶ�ѡ��
	GPFT_CUSTOM_OCCU,		//�ֶ�ѡ������ְҵ��
	GPFT_CHIJI_EQUIP,		//�Լ�װ��
};

//����������Ϣ
struct ItemRenewInfo
{
	UInt32	durationDay;		//��������
	UInt32	moneyId;			//����id
	UInt32	moneyNum;			//��������
};

// ������ʱ�ж�
enum ItemCheckType
{
	ICT_VALID = 0,				// ��Ч
	ICT_ABOUT_TO_EXPIRE,		// ��Ҫ����
	ICT_EXPIRE,					// ���ڿ�����
	ICT_NEED_DELETE,			// ���ڱ�ɾ��
};

// ʱ�޵���
struct TimeItem
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & itemUid & state;
	}

	UInt64 itemUid;
	UInt32 state;		// ItemCheckType
};

//�̵�����
enum ShopType
{
	ST_ANCIENT = 0,			//Զ���̵�
	ST_MYSTERY = 1,			//�����̵�
	ST_FORGE = 2,			//�����̵�
	ST_BRAVE = 3,			//�����̵�
	ST_FIGHT = 4,			//�����̵�
	ST_GUILD = 5,			//�����̵�
	ST_MAGIC = 6,			//ħ���̵�
	ST_GOLD = 7,			//����̵�
	ST_ABYSS = 8,			//��Ԩ�̵�
	ST_GUILD_STORAGE = 9,	//����ֿ��̵�
	ST_ACTIVITY = 10,		//��̵�
	ST_DUNGEON = 11,		//���³��̵�
	ST_MASTER = 12,			//��ʦ�̵�
	ST_LEASE = 13,			//�����̵�
	ST_ADVENTURE_TEAM = 14,	//ð�ն�(Ӷ����)�̵�
	ST_OP_ACTIVITY = 15,	//��Ӫ��̵�
	ST_ACCOUNT_BIND_GOLD = 17, //�˺Ű���̵�
	ST_ADVENTURE_PASS = 18, //ð�ձ��̵�
	ST_HIRE = 19,           //��ļ�̵�
	ST_CHAMPION = 20,		//�ھ����̵�
	ST_MONOPOLY = 21,		//�����̵�
	ST_SECRET_SELL = 22,	//��������
};

//�̵�ˢ������
enum ShopRefreshType
{
	SRT_NO_REFRESH = 0,		//����ˢ��
	SRT_BLACK = 1,			//����ˢ��(���ֶ�ˢ���޴�)
	SRT_NORMAL = 2,			//��ͨˢ��(�����ֶ�ˢ�²��޴�)
	SRT_NORMAL_MANUAL = 3,	//���ֶ�ˢ�²��޴���
};

/**
*@brief װ������ScoreItem
*/
struct EqRecScoreItem
{
public:
	EqRecScoreItem() :uid(0), score(0){}
	EqRecScoreItem(UInt64 uid_, UInt32 score_) :uid(uid_), score(score_){}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & uid & score;
	}
	// װ��uid
	UInt64	uid;
	// ����
	UInt32	score;
};

//װ�����չ���״̬
enum EqRecoJarState
{
	EQRECO_JARST_CANNOTOPEN = 0, //���ܴ�
	EQRECO_JARST_CANOPEN = 1, //�ɴ�
	EQRECO_JARST_OPEND = 2, //�Ѵ򿪹�
};

#define FASHION_CMPS_NEED_CAP	1	//ʱװ�ϳ���Ҫ��ʱװ������λ��
#define FASHION_CMPS_T0_M		12	//ʱװ�ϳ�T0������ֵ
#define L_SKY_RESET_NUM			5	//L�������������ͨ�߼��������ô���
#define L_FIX_ADVANCE_WT		200	//ÿ�ϳ�һ���߼������������20
#define L_FIX_COMMON_WT			100	//ÿ�ϳ�һ���߼������������10
#define L_FIX_MAX_WT			1000
#define L_COMMON_COMBO_NUM		3	//L����������N����ͨ��������
#define L_ADVANCE_COMBO_NUM		1	//L����������N�θ߼���������

//˫��ְҵ ����������װ����������������id
#define ACCESSWEAPON_EQUIPED_PACKAGE_GRID_ID 10

//�����ʱװ����
enum SkyFashionType
{
	SFT_COMMON = 1, //��ͨ
	SFT_ADVANCE = 2, //�߼�
	SFT_TRANSP = 3, //͸��
};

enum ItemLockType
{
	ILT_ITEM_UNLOCK	= 0,			// ����û��
	ILT_ITEM_LOCK	= 1 << 0,		// ������
	ILT_LEASE_LOCK  = 1 << 1,		// ������
	ILT_AUCTION_RETURN_LOCK  = 1 << 2,     // �����з�����
	ILT_FASHION_LOCK = 1 << 3,		// ʱװ��

	// ������
	ILT_ANY_LOCK = ILT_ITEM_LOCK | ILT_LEASE_LOCK | ILT_AUCTION_RETURN_LOCK,
};

enum PreciousBeadOpType
{
	PRECIOUS_BEAD_MOUNT		= 1,	// ������Ƕ
	PRECIOUS_BEAD_UPGRADE	= 2,	// ��������
	PRECIOUS_BEAD_EXTRIPE	= 3,	// ����ժȡ
	PRECIOUS_BEAD_REPLACE	= 4,	// �����û�
	PRECIOUS_BEAD_CONVERT   = 5,	// ����ת��
};

struct ProWeightItem
{
	UInt32 value;
	UInt32 weight;
};

struct CounterItem
{
	CounterItem() : currencyId(0), name(""), value(0) {}
	CounterItem(UInt32 _currencyId, std::string& _name, UInt32 _value) :name(_name), value(_value){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream  & currencyId & name & value;
	}

	UInt32 currencyId; //��Ӧ����id
	std::string name;
	UInt32 value;
};

#define EQ_PREADBEAD_MAXNUM		2	//װ��������Ƕ���������

enum UpgradePreciousBeadType
{
	UP_PRECBEAD_UNMOUNTED = 0, //δ��Ƕ
	UP_PRECBEAD_MOUNTED = 1,   //����Ƕ
};

#define PRECBEAD_UP_NEED_CAP	1	//��������,ժ����Ҫ�İ�����λ��

//��������������
enum CheckMailReqType
{
	CMRT_NONE = 0, 
	CMRT_AUCTION_PUNISH_CANCEL_TRANS = 1, //�����гͷ�ȡ�����׼�����
	CMRT_BLACKMARKET_TRANSACTION_REQ = 2, //�����������뽻��
};

//���߰�����
enum ItemBindType
{
	IBTP_NONE = 0,
	IBTP_NOT  = 1,	//�ǰ�
	IBTP_ROLE = 2,  //��ɫ��
	IBTP_ACCT = 3,  //�˺Ű�
};

//��ħ��һ���ϳɽ���Э��
enum MagicCardCompOneKeyEndReason
{
	MCCER_NONE = 0,
	MCCER_TIMES_MAX = 1,		//�����ﵽ���
	MCCER_ITEMPACK_FULL = 2,	//������
	MCCER_MONEY_INSUFF = 3,		//���Ҳ���
	MCCER_MONEY_MAX = 4,
};

enum FlyUpStatus
{
	//û����
	FLY_UP_STATUS_OUT = 0,
	//������
	FLY_UP_STATUS_IN = 1,
};

enum EquipInscriptionSynthesisNumLimit
{
	EQUIP_INSCRIPTION_LIMIT_MIN = 2,
	EQUIP_INSCRIPTION_LIMIT_MAX = 5,
};

//���ߺ���������Ϣ��
struct FriendPresentInfo
{
	FriendPresentInfo() : guid(0){}
	
	AVALON_DEFINE_NETSERIALIZATION() 
	{
		stream  & guid & friendId & friendOcc & friendLev & friendname & isOnline 
			& beSendedTimes & beSendedLimit & sendTimes & sendLimit & sendedTotalTimes & sendedTotalLimit;
	}

	//Ψһid
	ObjID_t		guid;
	//����id
	ObjID_t		friendId;
	//����ְҵ
	UInt8		friendOcc;
	//���ѵȼ�
	UInt16		friendLev;
	//��������
	std::string friendname;
	//����״̬
	UInt8		isOnline;
	//������������
	UInt32		beSendedTimes;
	//����������
	UInt32		beSendedLimit;
	//����������
	UInt32		sendTimes;
	//��������
	UInt32		sendLimit;
	//�������ܴ���
	UInt32		sendedTotalTimes;
	//�������ܴ�������
	UInt32      sendedTotalLimit;
};

enum EquipConvertType
{
	EQCTP_SAME = 1,	//ͬ��װ
	EQCTP_DIFF = 2,	//����װ
};


//ð��ͨ��֤���߰������ʽ
enum AdventurePassOpenKingType
{
	APOKT_ITEM = 0,
	APOKT_RMB = 1,
};

enum ItemKeyValuesKeyType
{
	IKVKT_NONE = 0,
	IKVKT_WBP_STR = 1,		//����
	IKVKT_WBP_STA = 2,		//����
	IKVKT_WBP_INT = 3,		//����
	IKVKT_WBP_SPR = 4,		//����
	IKVKT_WBP_HPMAX = 5,		//hp���ֵ
	IKVKT_WBP_MPMAX = 6,		//mp���ֵ
	IKVKT_WBP_HPREC = 7,		//hp�ָ�
	IKVKT_WBP_MPREC = 8,		//mp�ָ�
	IKVKT_WBP_HIT = 9,		//������
	IKVKT_WBP_DEX = 10,		//�ر���
	IKVKT_WBP_PHYCRT = 11,		//������
	IKVKT_WBP_MGCCRT = 12,		//ħ������
	IKVKT_WBP_ATKSPD = 13,		//����
	IKVKT_WBP_RDYSPD = 14,		//ʩ���ٶ�
	IKVKT_WBP_MOVSPD = 15,		//�ƶ��ٶ�
	IKVKT_WBP_JUMP = 16,		//��Ծ��
	IKVKT_WBP_HITREC = 17,		//Ӳֱ
	IKVKT_WBP_PHYATK = 18,		//�﹥
	IKVKT_WBP_MAGATK = 19,		//����
	IKVKT_WBP_PHYDEF = 20,		//���
	IKVKT_WBP_MAGDEF = 21,		//����	
	IKVKT_WBP_LIGHT = 22,       //�����Թ���
	IKVKT_WBP_FIRE = 23,		  //�����Թ���
	IKVKT_WBP_ICE = 24,		  //�����Թ���
	IKVKT_WBP_DARK = 25,			//�����Թ���
	IKVKT_WBP_LIGHT_ATK = 26, //������ǿ��
	IKVKT_WBP_FIRE_ATK = 27,	//������ǿ��
	IKVKT_WBP_ICE_ATK = 28,	//������ǿ��
	IKVKT_WBP_DARK_ATK = 29,	//������ǿ��
	IKVKT_WBP_LIGHT_DEF = 30, //�����Կ���
	IKVKT_WBP_FIRE_DEF = 31,	//�����Կ���
	IKVKT_WBP_ICE_DEF = 32,	//�����Կ���
	IKVKT_WBP_DARK_DEF = 33,	//�����Կ���
	IKVKT_WBP_GDKX = 34,		//�е翹��
	IKVKT_WBP_CXKX = 35,		//��Ѫ����
	IKVKT_WBP_ZSKX = 36,		//���տ���
	IKVKT_WBP_ZDKX = 37,		//�ж�����
	IKVKT_WBP_SMKX = 38,		//ʧ������
	IKVKT_WBP_XYKX = 39,		//ѣ�ο���
	IKVKT_WBP_SHKX = 40,		//ʯ������
	IKVKT_WBP_BDKX = 41,		//��������
	IKVKT_WBP_SLKX = 42,		//˯�߿���
	IKVKT_WBP_HLKX = 43,		//���ҿ���
	IKVKT_WBP_SFKX = 44,		//��������
	IKVKT_WBP_JSKX = 45,		//���ٿ���
	IKVKT_WBP_ZZKX = 46,		//���俹��
	IKVKT_WBP_YKXZ = 47,		//�����쳣����
	IKVKT_WBP_INDEPEND_ATK = 48, //����������
	IKVKT_WBP_PhySkillMp = 49, //�����ܺ����仯
	IKVKT_WBP_PhySkillCd = 50 , //������CD�仯
	IKVKT_WBP_MagSkillMp = 51 , //ħ�����ܺ����仯
	IKVKT_WBP_MagSkillCd = 52 , //ħ������CD�仯
	IKVKT_WBP_QSKX = 53, //��ʴ����


	IKVKT_WBP_BUFF_BEGIN = 100,
	//�������ԣ�buff��Ϣ��ID��
	IKVKT_WBP_BUFF_END = 200,

	IKVKT_WBP_SPECIAL = 300, //װ����������
	IKVKT_WBP_BUILD_WEAPON = 301, //�������װ��
	IKVKT_WBP_SPECIAL_DETAIL = 302, //װ��������ֵ
	IKVKT_WBP_ADDITION_SCORE = 303, //����������ĸ�������
	
	IKVKT_WBP_MAX,
	
	IKVKT_MAX,
};

//�󶨽��id
#define BIND_GOLD_ID 600000007

#endif
