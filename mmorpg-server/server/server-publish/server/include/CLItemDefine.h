#ifndef _CL_ITEM_DEFINE_H_
#define _CL_ITEM_DEFINE_H_

#include <AvalonNetStream.h>
#include <json/writer.h>
#include "CLItemTypeDefine.h"
#include "CLObjectProperty.h"

/**
 *@brief 道具属性
 */
enum ItemAttr
{
	IA_INVALID,			//无效属性
	IA_NUM,				//叠加数	UInt16
	IA_BIND,			//绑定		UInt8
	IA_PACK,			//包裹		UInt8
	IA_GRID,			//包裹格子	UInt8
	IA_PHY_ATK,			//物理攻击		UInt32
	IA_MAG_ATK,			//法术攻击
	IA_PHY_DEF,			//物理防御		UInt32
	IA_MAG_DEF,			//法术防御		UInt32
	IA_STR,				//力量		UInt32
	IA_STAMINA,			//体力		UInt32
	IA_INTELLECT,		//智力		UInt32
	IA_SPIRIT,			//精力		UInt32
	IA_QUALITYLV,		//品级
	IA_QUALITY,			//装备品质	UInt8 EquipQuality
	IA_STRENGTHEN,		//强化等级	UInt8
	IA_RANDATTR,		//随机属性	格式：[PkValueID(UInt8) + ModPkValueType(UInt8) + Value(Int32)]...[0(UInt8)]
	IA_DAYUSENUM,			//日使用次数		UInt32
	//IA_GEMSTONE,		//宝石      格式: [pos(UInt8 1-5)][ItemID(UInt32)][Level(UInt8)]...[0(UInt8)]
	IA_ADDMAGIC,		//附魔		
	IA_PARAM1,			//灵活字段1 UInt32
	IA_PARAM2,			//灵活字段2	UInt32
	IA_POWER,			//战斗力	UInt32
	IA_DEADLINE,		//失效时间	UInt32	永久有效的道具值为0
	IA_PRICE,			//价格		UInt32	玩家设置的价格
	IA_STRFAILED,		//强化失败次数	UInt16
	IA_SEAL_STATE,		//是否封装 UInt8
	IA_SEAL_COUNT,		//封装次数 UInt32
	IA_DIS_PHYATK,		//无视物理攻击
	IA_DIS_MAGATK,		//无视法术攻击
	IA_DIS_PHYDEF,			//无视物理防御
	IA_DIS_MAGDEF,			//无视法术防御
	IA_PVP_DIS_PHYATK,		//PVP无视物理攻击
	IA_PVP_DIS_MAGATK,		//PVP无视法术攻击
	IA_PVP_DIS_PHYDEF,			//PVP无视物理防御
	IA_PVP_DIS_MAGDEF,			//PVP无视法术防御
	IA_VALUE_SCORE,			//装备评分
	IA_SET_QL_NUM,			//品级调整次数
	IA_FASHION_ATTRID,		//时装属性id
	IA_FASHION_ATTR_SELNUM,	//时装属性更改次数		
	IA_PHYDEF_PERCENT,		//强化物理防御百分比
	IA_MAGDEF_PERCENT,		//强化魔法防御百分比
	IA_ADD_PRECIOUSBEAD,	//添加宝珠
	IA_STRPROP_LIGHT,		//光属强
	IA_STRPROP_FIRE,		//火属强
	IA_STRPROP_ICE,			//冰属强
	IA_STRPROP_DARK,		//暗属强
	IA_DEFPROP_LIGHT,		//光属抗
	IA_DEFPROP_FIRE,		//火属抗
	IA_DEFPROP_ICE,			//冰属抗
	IA_DEFPROP_DARK,		//暗属抗
	IA_ABNORMAL_RESISTS_TOTAL,	//异抗总
	IA_EAR_FLASH,		//异抗感电
	IA_EAR_BLEEDING,		//异抗出血
	IA_EAR_BURN,		//异抗灼烧
	IA_EAR_POISON,		//异抗中毒
	IA_EAR_BLIND,		//异抗失明
	IA_EAR_STUN,		//异抗晕眩
	IA_EAR_STONE,		//异抗石化
	IA_EAR_FROZEN,		//异抗冰冻
	IA_EAR_SLEEP,		//异抗睡眠
	IA_EAR_CONFUNSE,		//异抗混乱
	IA_EAR_STRAIN,		//异抗束缚
	IA_EAR_SPEED_DOWN,		//异抗减速
	IA_EAR_CURSE,		//异抗诅咒
	IA_TRANSFER_STONE,	//转移石itemid
	IA_RECO_SCORE,		//装备回收所得积分
	IA_IS_LOCK,			//道具锁
	IA_PRECIOUSBEAD_HOLES, //宝珠镶嵌孔
	IA_AUCTION_COOL_TIMESTAMP, //拍卖行交易冷却时间
	IA_IS_TREAS,				//是否珍品
	IA_BERD_EXTRIPE_CNT,		//宝珠的摘除次数
	IA_BERD_REPLACE_CNT,		//宝珠的替换次数
	IA_DATA_ID,					//物品ID
	IA_EQUIP_TYPE,       //装备类型:普通0/强化1/带气息2/红字3
	IA_ENHANCE_TYPE,     //红字装备增幅类型:力量0/智力1/体力2/精神3
	IA_ENHANCE_NUM,      //红字装备增幅数值
	IA_ENHANCE_FAILED,   //红字装备增幅失败次数
	IA_AUCTION_TRANS_NUM,   //拍卖行交易次数
	IA_INSCRIPTION_HOLES,	//铭文镶嵌孔
	IA_INDEPENDATK,			//独立攻击力
	IA_INDEPENDATK_STRENG,	//独立攻击力强化
	IA_SUBTYPE,	//道具子类型
	IA_KEYVALUES, //key-value字段
};

enum ZengfuOverload
{
	ZENGFU_OVERLOAD_MIN = 8,
	ZENGFU_OVERLOAD_MAX = 10,
};

enum WeaponBuildSpecial
{
	WBS_PINZHI = 1, //神圣品质
	WBS_QIYUE = 2,  //战斗契约
	WBS_DOUBLE = 3, // 双孔
	WBS_FORVER = 4, // 永恒
};

// 随机属性
enum ItemRandProp
{
	IRP_NONE = 0,
	IRP_STR = 1,		//力量
	IRP_STA = 2,		//体力
	IRP_INT = 3,		//智力
	IRP_SPR = 4,		//精神
	IRP_HPMAX = 5,		//hp最大值
	IRP_MPMAX = 6,		//mp最大值
	IRP_HPREC = 7,		//hp恢复
	IRP_MPREC = 8,		//mp恢复
	IRP_HIT = 9,		//命中率
	IRP_DEX = 10,		//回避率
	IRP_PHYCRT = 11,		//物理暴击
	IRP_MGCCRT = 12,		//魔法暴击
	IRP_ATKSPD = 13,		//攻速
	IRP_RDYSPD = 14,		//施放速度
	IRP_MOVSPD = 15,		//移动速度
	IRP_JUMP = 16,		//跳跃力
	IRP_HITREC = 17,		//硬直
	IRP_PHYATK = 18,		//物攻
	IRP_MAGATK = 19,		//法攻
	IRP_PHYDEF = 20,		//物防
	IRP_MAGDEF = 21,		//法防	

	IRP_LIGHT = 22,       //光属性攻击
	IRP_FIRE = 23,		  //火属性攻击
	IRP_ICE = 24,		  //冰属性攻击
	IRP_DARK = 25,			//暗属性攻击
	IRP_LIGHT_ATK = 26, //光属性强化
	IRP_FIRE_ATK = 27,	//火属性强化
	IRP_ICE_ATK = 28,	//冰属性强化
	IRP_DARK_ATK = 29,	//暗属性强化
	IRP_LIGHT_DEF = 30, //光属性抗性
	IRP_FIRE_DEF = 31,	//火属性抗性
	IRP_ICE_DEF = 32,	//冰属性抗性
	IRP_DARK_DEF = 33,	//暗属性抗性
	IRP_GDKX = 34,		//感电抗性
	IRP_CXKX = 35,		//出血抗性
	IRP_ZSKX = 36,		//灼烧抗性
	IRP_ZDKX = 37,		//中毒抗性
	IRP_SMKX = 38,		//失明抗性
	IRP_XYKX = 39,		//眩晕抗性
	IRP_SHKX = 40,		//石化抗性
	IRP_BDKX = 41,		//冰冻抗性
	IRP_SLKX = 42,		//睡眠抗性
	IRP_HLKX = 43,		//混乱抗性
	IRP_SFKX = 44,		//束缚抗性
	IRP_JSKX = 45,		//减速抗性
	IRP_ZZKX = 46,		//诅咒抗性
	IRP_YKXZ = 47,		//所有异常抗性
	IRP_INDEPEND_ATK = 48, //独立攻击力
	IRP_MAX,
};

enum EquipExType
{
	EET_ATK = 0,	//属性攻击
	EET_STR = 1,	//属性强化
	EET_DEF = 2,	//属性抗性
	EET_AR = 3,		//异常抗性
	EET_MAX = 4,
};

//装备附带属性
enum EquipExProp
{
	EEP_LIGHT = 0,	//光属性
	EEP_FIRE = 1,	//火属性
	EEP_ICE = 2,	//冰属性
	EEP_DARK = 3,	//暗属性

	EEP_MAX = 4,
};

//装备异常抗性
enum EquipAbnormalResist
{
	EAR_FLASH = 0,		//感电
	EAR_BLEEDING = 1,		//出血
	EAR_BURN = 2,		//灼烧
	EAR_POISON = 3,		//中毒
	EAR_BLIND = 4,		//失明
	EAR_STUN = 5,		//晕眩
	EAR_STONE = 6,		//石化
	EAR_FROZEN = 7,		//冰冻
	EAR_SLEEP = 8,		//睡眠
	EAR_CONFUNSE = 9,		//混乱
	EAR_STRAIN = 10,		//束缚
	EAR_SPEED_DOWN = 11,		//减速
	EAR_CURSE = 12,		//诅咒
	

	EAR_MAX = 13,
};

//评分属性
enum ItemScoreProp
{
	ISP_NONE = 0,
	ISP_STR = 1,			//力量
	ISP_INT = 2,			//智力
	ISP_STAM = 3,			//体力
	ISP_SPR = 4,			//精神
	ISP_HP = 5,				//HP
	ISP_MP = 6,				//MP
	ISP_HPRECVR = 7,		//HP回复
	ISP_MPRECVR = 8,		//MP回复
	ISP_PHYATK = 9,			//物攻
	ISP_MAGATK = 10,			//法攻
	ISP_PHYDEF = 11,			//物防
	ISP_MAGDEF = 12,			//法防
	ISP_DISPHYATK = 13,		//无视物攻
	ISP_DISMAGATK = 14,		//无视法攻
	ISP_PHYDMGRDC = 15,		//物理伤害追加减少
	ISP_MAGDMGRDC = 16,		//魔法伤害追加减少
	ISP_ATKSPD = 17,			//攻速
	ISP_MAGSPD = 18,			//施法速度
	ISP_MVSPD = 19,			//移动速度
	ISP_PHYCRT = 20,			//物理暴击
	ISP_MAGCRT = 21,			//魔法暴击
	ISP_CRTDMG = 22,			//暴击伤害
	ISP_HIT = 23,			//命中率
	ISP_MISS = 24,			//回避率
	ISP_JZ = 25,				//僵直
	ISP_YZ = 26,				//硬直

	ISP_LIGHT = 27,       //光属性攻击
	ISP_FIRE = 28,		  //火属性攻击
	ISP_ICE = 29,		  //冰属性攻击
	ISP_DARK = 30,			//暗属性攻击
	ISP_LIGHT_ATK = 31, //光属性强化
	ISP_FIRE_ATK = 32,	//火属性强化
	ISP_ICE_ATK = 33,	//冰属性强化
	ISP_DARK_ATK = 34,	//暗属性强化
	ISP_LIGHT_DEF = 35, //光属性抗性
	ISP_FIRE_DEF = 36,	//火属性抗性
	ISP_ICE_DEF = 37,	//冰属性抗性
	ISP_DARK_DEF = 38,	//暗属性抗性
	ISP_GDKX = 39,		//感电抗性
	ISP_CXKX = 40,		//出血抗性
	ISP_ZSKX = 41,		//灼烧抗性
	ISP_ZDKX = 42,		//中毒抗性
	ISP_SMKX = 43,		//失明抗性
	ISP_XYKX = 44,		//眩晕抗性
	ISP_SHKX = 45,		//石化抗性
	ISP_BDKX = 46,		//冰冻抗性
	ISP_SLKX = 47,		//睡眠抗性
	ISP_HLKX = 48,		//混乱抗性
	ISP_SFKX = 49,		//束缚抗性
	ISP_JSKX = 50,		//减速抗性
	ISP_ZZKX = 51,		//诅咒抗性

	ISP_HSWORD = 100, 			//巨剑
	ISP_TD = 101,				//太刀
	ISP_ZL = 102,				//左轮
	ISP_SP = 103,				//手炮
	ISP_FZ = 104,				//法杖
	ISP_MZ = 105,				//魔杖
	ISP_BJ = 106,				//布甲
	ISP_PJ = 107,				//皮夹
	ISP_DUANJIAN = 108,		//短剑
	ISP_QINJIA = 109,			//轻甲
	ISP_ZHONGJIA = 110,		//重甲
	ISP_BANJIA = 111,			//板甲
	ISP_OFG = 112,			//手套
	ISP_EAST_STICK = 113,		//东方棍
	ISP_GLOVE = 114,		//手套
	ISP_BIKAI = 115,		//臂铠
	ISP_CLAW = 116,			//爪
	ISP_NUJIAN = 117,		//弩箭
	ISP_BUQIANG = 118,		//步枪
	ISP_GUANGJIAN = 119,	//光刃
	ISP_SICKLE = 120,		//镰刀
	ISP_TOTEM = 121,		//图腾
	ISP_AXE = 122,			//战斧
	ISP_BEADS = 123,		//念珠
	ISP_CROSS = 124,		//十字架
	ISP_SPEAR = 125,		//矛
	ISP_STICK = 126,		//棍棒

	ISP_ATTACK = 200,		//攻击力评分
	ISP_POWER = 201,		//四维相关评分
	ISP_REDUCE = 202,		//减伤评分
	ISP_DISPHYATTACK = 203, //无视防御评分
	ISP_DISPHYREDUCE = 204,	//无视攻击评分
	ISP_FUJIA = 205,		//附加评分系数
	ISP_INDEPENDATK = 206,	//独立攻击力评分
	ISP_MAX,
};

/**
 *@brief 绑定类型
 */
enum BindType
{
	BIND_TYPE_NONE = 0,		//不绑定
	BIND_TYPE_ONGET = 1,	//获得绑定
	BIND_TYPE_ONUSE = 2,	//使用绑定
};

/**
 *@brief 道具选项
 */
enum ItemOption
{
	ITEM_OPTION_SALE	=			1 << 0,	//是否可出售给npc
	ITEM_OPTION_TRADE	=			1 << 1,	//是否可交易
	ITEM_OPTION_MAIL	=			1 << 2,	//是否可邮寄
	ITEM_OPTION_DISCARD =			1 << 3,	//是否可丢弃
	ITEM_OPTION_DESTROY =			1 << 4,	//是否可销毁
	ITEM_OPTION_DROP_ONOFFLINE	=	1 << 5,	//下线是否掉落
	ITEM_OPTION_DROP_ONCHSCENE	=	1 << 6,	//跳转地图是否掉落
	ITEM_OPTION_STORAGE	=			1 << 7,	//存仓库
	ITEM_OPTION_DTOPONDIE =			1 << 8, //是否死亡掉落
	ITEM_OPTION_USE		=			1 << 9,	//是否可使用
	ITEM_OPTION_BUY		=			1 << 10,//是否可向npc购买
	ITEM_OPTION_MUSE	=			1 << 11,//是否可批量使用
	ITEM_OPTION_PICKUPBIND =		1 << 12,//拾取绑定
	ITEM_OPTION_QUICKBAR =			1 << 13,//是否可放入快捷栏
	ITEM_OPTION_FORBIDDEN_STR =		1 << 14,//是否禁止强化
};


enum PotionConfigType
{
	POTION_CONFIG_TYPE_MAIN = 0,
	POTION_CONFIG_TYPE_HP   = 1,
	POTION_CONFIG_TYPE_MP   = 2,
};


/**
 *@brief 道具效果参数个数
 */
#define ITEM_EFFECT_PARAM_NUM	4

/**
*@brief 装备强化最大等级
*/
#define ITEM_STRENTH_MAX	20
/**
 *@brief 包裹类型(顺序不可改)
 */
enum PackType
{
	PACK_INVALID			= 0,	//无效
	PACK_EQUIP,						//装备包裹
	PACK_MATERIAL,					//材料包裹
	PACK_EXPENDABLE,				//消耗品包裹
	PACK_TASK,						//任务道具包裹
	PACK_FASHION,					//时装包裹
	PACK_WEAR,						//已装备的包裹
	PACK_FASHION_WEAR,				//已装备的时装包裹
	PACK_STORAGE,					//仓库
	PACK_ENERGY_STONE,				//能量石	adder by huchenhui	2016.07.21
	PACK_TITLE,						//称号		adder by huchenhui	2016.07.21
	PACK_PET,						//宠物包裹	adder by huchenhui	2017.08.04
	PACK_EQRCOV,					//装备回收包裹
	PACK_ROLE_STORAGE,				//角色仓库，免费仓库
	PACK_INSCRIPTION,				//铭文包裹
	PACK_LOSTDUNGEON,				//迷失地牢包裹
	PACK_MAX,

	PACK_NORMAL,					//随身普通包裹(废弃)
	PACK_BUYBACK,					//回购包裹(废弃)
	PACK_LOTTERY,					//祈福抽奖仓库(废弃)
	PACK_WING,						//翅膀(废弃)
};

/**
*@brief 部落包裹类型
*/
enum TribePackType
{
	TRIBE_PACK_INVALID	= 0,		// 无效
	TRIBE_PACK_STORAGE	= 1,		// 部落仓库
	TRIBE_PACK_SHOP		= 2,		// 部落商店
	TRIBE_PACK_MAX,
};

/**
 *@brief 是否随身携带的包裹
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
 *@brief 包裹大小
 */
const static UInt32 PackSize[PACK_MAX] = { 0 , 1000, 1000, 1000, 1000, 1000, 14, 8, 100, 20, 1000, 80, 20, 270, 1000, 10 };
const static UInt32 MAX_GRID_NUM_PER_ROLE_STORAGE_PAGE = 30;

/**
*@brief 部落包裹类型
*/
const static UInt8 TribePackSize[TRIBE_PACK_MAX] = {0, 168, 50};

const static Int32 WEAPON_GRID = 9;
#define EQUIP_GRID_NUM 10
/**
 *@brief 根据装备类型获取装备位置
 */
inline UInt8 GetGridByEquipType(int type)
{
// 	if(type <= EQUIP_TYPE_SHOES) return UInt8(type - 1);
// 	if(type > EQUIP_TYPE_SHOES && type <= EQUIP_TYPE_WEAPON_MAX) return WEAPON_GRID;
// 	return UInt8(WEAPON_GRID + type - EQUIP_TYPE_WEAPON_MAX - 1);
	return 255;
}

/**
*@brief 根据装备类型获得翅膀装备位置
*/
inline UInt8 GetWingGridByEquipType(int type)
{
// 	switch(type)
// 	{
// 	case EQUIP_TYPE_AMULET:		//护身符
// 		return 9;
// 	case EQUIP_TYPE_CLOTHES:	//衣服
// 		return 0;
// 	case EQUIP_TYPE_HANDGUARD:	//护手
// 		return 2;
// 	case EQUIP_TYPE_BELT:		//腰带
// 		return 4;
// 	case EQUIP_TYPE_LEGGUARD:	//护腿
// 		return 6;
// 	case EQUIP_TYPE_TORQUE:		//项链
// 		return 3;
// 	case EQUIP_TYPE_RING:		//戒指
// 		return 5;
// 	case EQUIP_TYPE_BANGLE:		//手镯
// 		return 7;
// 	case EQUIP_TYPE_SHOES:		//鞋子
// 		return 8;
// 	}
// 	
// 	if(type > EQUIP_TYPE_SHOES && type <= EQUIP_TYPE_WEAPON_MAX) return 1;
	return 255;
}

/**
 *@brief 道具位置
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

	//包裹
	UInt8	pack;
	//格子索引
	UInt32	gridIndex;
};


/**
 *@brief 无效的道具位置
 */
#define INVALID_ITEMGRID UInt32(-1)
const static ItemPos INVALID_ITEMPOS;

//获取角色背包页
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
 *@brief 道具品质
 */
enum ItemQuality
{
	ITEM_QUALITY_INVLID = 0,
	ITEM_QUALITY_WHITE,			//白
	ITEM_QUALITY_BLUE,			//蓝 高级
	ITEM_QUALITY_PURPLE,		//紫 稀有
	ITEM_QUALITY_GREEN,			//绿 传承
	ITEM_QUALITY_PINK,			//粉 神器
	ITEM_QUALITY_YELLOW,		//黄 传说
	ITEM_QUALITY_SILVERY = 7,   //银 神圣

	ITEM_QUALITY_MAX,
};

/**
*@brief 道具品质2
*/
enum ItemQuality2
{
	ITEM_QUALITY2_INVLID = 0,
	ITEM_QUALITY2_GRAY,			//灰色
	ITEM_QUALITY2_WHITE,		//白 
	ITEM_QUALITY2_ROSERED,		//玫红

	ITEM_QUALITY2_MAX,
};

/**
*@brief 道具品质位枚举（废弃）
*/
enum ItemBitQuality
{
	IBQ_WHITE	= 1 << 0,			//白
	IBQ_BLUE	= 1 << 1,			//蓝
	IBQ_PURPLE	= 1 << 2,			//紫
	IBQ_GREEN	= 1 << 3,			//绿
	IBQ_PINK	= 1 << 4,			//粉
	IBQ_YELLOW	= 1 << 5,			//黄
};

/**
*@brief 装备品级
*/
enum EquipQualityLv
{
	EQL_INVALID = 0,			
	EQL_LOWEST,					//最下级
	EQL_LOW,					//下级
	EQL_MID,					//中级
	EQL_HIGH,					//上级
	EQL_HIGHEST,				//最上级

	EQL_MAX,
};



/**
 *@brief 装备品质数
 */
#define EQUIP_QUALITY_MAX 6



/**
 *@brief 根据道具类型规范品质
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
 *@brief 装备品质配置
 */
struct EquipQualityCfg
{
	UInt32			baseAttrRate;				//基础属性加成
	UInt32			randAttrRate;				//随机属性加成
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
*@brief 装备最大等级
*/
#define MAX_EQUIP_LEVEL 100
/**
*@brief 装备等级段数量
*/
#define EQUIP_SECTION_NUM (MAX_EQUIP_LEVEL / 10 + 1)
/**
*@brief 获得装备等级段
*/
inline UInt16 GetEquipLevelSection(UInt16 level)
{
	return level / 10;
}

/**
*@brief 随机属性最大星数
*/
#define MAX_EQUIPATTR_STAR_NUM 25


/**
*@brief 装备的最大强化等级
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

//最大宝石孔数
#define MAX_EQUIP_GEM_HOLE 4
/**
*@brief 宝石开孔配置
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
 *@brief 根据强化等级获取孔数
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
*@brief 每次强化失败增加的成功率
*/
#define EQUIP_STR_RATE_PER_FAILED	500
/**
*@brief vip等级增加的强化成功率
*/
#define EQUIP_STR_RATE_VIP_ADD	200

/**
 *@brief 宝石最大属性数
 */
#define MAX_GEMSTONE_ATTRNUM 3
/**
 *@brief 最大随机属性数
 */
#define MAX_EQUIPATTR_NUM 3
/**
*@brief 最大附魔数
*/
#define MAX_MAGIC_NUM 5
/**
 *@brief 最高强化等级
 */
#define MAX_STRENGTHEN_LEVEL 15

/**
 *@brief 随机属性衰减系数
 */
#define RANDATTR_RATE_FACTOR 0.4f
/**
 *@brief 合成一个宝石需要的宝石数
 */
#define GEMSTONE_COMPOSE_SRC_NUM 4
/**
 *@brief 批量使用一次最多使用数
 */
#define MAX_USE_ITEMNUM_ONCE 30

/**
 *@brief 装备改造类型
 */
enum EquipRemakeType
{
	EQUIP_REMAKE_INLAY	=			1,	//镶嵌
	EQUIP_REMAKE_STRENGTHEN =		2,	//强化，param1 目标等级，0 强化1级，非0 自动强化的目标等级
	EQUIP_REMAKE_XIYUAN =			3,	//洗元，主材料id 副装备，param1 随机属性索引(从0开始)
	//TODO
	EQUIP_REMAKE_TAKEAPART =		5,	//拆解
	EQUIP_REMAKE_TAKEAPART_STONE =	6,	//拆卸宝石
	EQUIP_REMAKE_COMPOSE_STONE =	7,	//宝石合成
	EQUIP_REMAKE_INHERIT_ATTRS =	8,	//继承属性
	EQUIP_REMAKE_UPGRADE =			10,	//装备升阶
	EQUIP_REMAKE_UPGRADEQUALITY =	11,	//装备品质提升，param1 目标品质，4 金装，5 暗金装
	EQUIP_REMAKE_INLAY_RUNE =		19,	//符文镶嵌，主材料id 符文
	EQUIP_REMAKE_TAKEAPART_RUNE =	20,	//符文拆卸
};

/**
*@brief 邮件道具
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
	//道具id
	UInt32 id;
	//道具数量
	UInt32 num;
	//强化等级
	UInt8 strenth;
	//来源
	std::string reason;
	//拍卖行交易冷却时间
	UInt32 auctionCoolTimeStamp;
	//拍卖行交易次数
	UInt32 auctionTransTimes;
};

/**
 *@brief 道具
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
 *@brief 道具奖励, modified by aprilliu, 2016.04.12
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
	//道具id
	UInt32 id;
	//道具数量
	UInt32 num;
 	//道具品级
 	UInt8  qualityLv;
 	//强化等级
 	UInt8 strenth;
	//拍卖行交易冷却时间
	UInt32 auctionCoolTimeStamp;
	//装备类型(对应枚举EquipType)
	UInt8  equipType;
	//拍卖行交易次数
	UInt32 auctionTransTimes;
};
typedef std::vector<ItemReward> ItemRewardVec;

//开罐结果
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

//职业物品奖励
struct OccuItemReward
{
	Occupation occu;
	ItemReward itemReward;
};


/**
*@brief 增幅材料合成目标数量范围
*/
enum EnhanceComboMaterial
{
	ENHANCE_COMBO_MATERIAL_NUM_MIN = 1,
	ENHANCE_COMBO_MATERIAL_NUM_MAX = 99,
};

/**
 *@brief 合成最大原料数
 */
#define MAX_ITEM_COMPOSE_MATERIAL_NUM 5

/**
 *@brief 强化套装效果加成
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
 *@brief 宝石套装效果
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

//最大随机属性星级
#define MAX_RANDATTR_STAR 18

/**
 *@brief 随机属性套装效果
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
 *@brief 道具对话框选项
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
	//选项id
	UInt64	id;
	//选项名
	std::string	title;
	//icon
	UInt8	icon;
};


/**
 *@brief 道具兑换信息                                                                     
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
*@brief 道具加锁操作类型
*/
enum ItemLockType1
{
	ILT_NEW_PASSWD = 0,			//设置新密码
	ILT_VERIFY_PASSWD = 1,		//验证密码
	ILT_FORGET_PASSWD = 2,		//忘记密码
	ILT_RESET_PASSWD = 3,		//重设密码
	ILT_CACEL_LOCK = 4,			//取消上锁
	ILT_LOCK_RANGE = 5,			//锁定范围	
};

/**
*@brief 锁定范围
*/
enum ItemLockRangeType
{
	ILRT_EQUIP_PACK = 1 << 0,	//身上装备
	ILRT_NORMAL_PACK = 1 << 1,	//背包
	ILRT_STORAGE_PACK = 1 << 2, //仓库
	ILRT_PET = 1 << 3,			//宠物
	ILRT_MOUNT = 1 << 4,		//坐骑

	//所有
	ILRT_ALL = ILRT_EQUIP_PACK | ILRT_NORMAL_PACK | ILRT_STORAGE_PACK 
			   | ILRT_PET | ILRT_MOUNT,
};

/**
*@brief 道具加锁按钮状态
*/
enum ItemLockButtonType
{
	ILBT_INIT = 0,	//未设置密码状态,灰色
	ILBT_LOCK = 1,  //设置密码加锁状态 金色
	ILBT_UNLOCK = 2,//解锁状态 金色打开
};

//积分卡ID
#define PANIC_BUY_CARD 32173

struct RaceItemRandProperty
{
    AVALON_DEFINE_NETSERIALIZATION()
    {
        stream & type & value;
    }

    // 类型（ItemRandProp）
    UInt8   type;
    // 具体的数值
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
	//宝珠id
	UInt32	preciousBeadId;
	//附加buffid
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

	//唯一id
	ObjID_t		uid;

    //装备ID
    UInt32      id;
    //装备位置
	UInt32       pos;
    //物理攻击
    UInt32      phyAtk;
    //法术攻击
    UInt32      magAtk;
    //物理防御
    UInt32      phyDef;
	//法术防御
	UInt32		magDef;
    //力量
    UInt32      strenth;
    //体力
    UInt32      stamina;
    //智力
    UInt32      intellect;
    //精神
    UInt32      spirit;
    //随机属性
    std::vector<RaceItemRandProperty> properties;
	//附魔卡
	UInt32		cardId;
	//无视物理攻击
	UInt32		disPhyAtk;
	//无视法术攻击
	UInt32		disMagAtk;
	//无视防御(除以10000还原)
	UInt32		disPhyDef;
	UInt32		disMagDef;
	//物防百分比
	UInt32		phyDefPercent;
	//魔防百分比
	UInt32		magDefPercent;
	//强化
	UInt8		strengthen;
	//时装属性id
	UInt32		fashionAttrId;
	//宝珠
	UInt32		preciousBeadId;
	//是否时效性
	UInt8		isTimeLimit;
	//攻击附带属性
	std::vector<Int32>	atkPropEx;
	//强化附带属性
	std::vector<Int32>	strPropEx;
	//抗性附带属性
	std::vector<Int32>	defPropEx;
	//异常抗性总
	Int32		abnormalResistsTotal;
	//异常抗性
	std::vector<Int32>	abnormalResists;
	//镶嵌宝珠
	std::vector<RacePrecBead> mountPrecBeads;
	//附魔卡等级
	UInt8 magicCardLv;
	//装备类型:普通0/带气息1/红字2
	UInt8 equipType;
	//红字装备增幅类型:力量0/智力1/体力2/精神3
	UInt8    enhanceType;
	//红字装备增幅数值
	UInt32   enhanceNum;
	//铭文ID数组
	std::vector<UInt32> inscriptionIds;
	//独立攻击力
	UInt32	 independAtk;
	//独立攻击力强化加成
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

	//装备ID
	UInt32      id;
	//数量
	UInt16		num;
};

// 物品基本信息
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

	// 唯一ID
	ObjID_t		id;
	// 类型ID
	UInt32		typeId;
	// 位置
	UInt32		pos;
	// 强化
	UInt8		strengthen;
	// 装备类型
	UInt8       equipType;
	//增幅路线
	UInt8       enhanceType;
	//装备评分
	UInt32		equipScore;
};

// 快速购买目标类型
enum QuickBuyTargetType
{
	// 复活
	QUICK_BUY_REVIVE,
	// 购买道具
	QUICK_BUY_ITEM,

	QUICK_BUY_NUM,
};

//强化惩罚类型
enum StrPunishType
{
	SPT_NONE = 0,	
	SPT_MINUSONE,	//减一
	SPT_ZERO,		//归零
	SPT_BROKEN,		//破碎
	SPT_MINUSMORE,	//随机降2-3级
};

//罐子类别
enum JarType
{
	JT_NONE = 0,
	JT_MAGIC,		//魔罐
	JT_GOLD,			//金罐
	JT_BATTLE,			//武道会
	JT_MAGIC_55,		//55级魔罐
	JT_EQRECO = 5,			//装备回收
	JT_GIFT,			//礼包罐子
	JT_NORMAL_MAX = 100,

	JT_WING,			//翅膀袖珍罐
	JT_FASHION,			//时装袖珍罐
	JT_EQUIP,			//装备袖珍罐
	JT_ACT_MAX = 200,

	JT_MAX,
};

/**
*@brief 开罐子记录
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
	//罐子id
	UInt32 jarId;
	//开罐者名字
	std::string	name;
	//道具id
	UInt32 itemId;
	UInt32 num;
};

// 道具事务，具体的道具
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


// 礼包类别
enum GiftPackFilterType
{
	GPFT_NONE = 0,
	GPFT_OCCU,				//职业删选
	GPFT_RANDOM,			//随机删选
	GPFT_CUSTOM,			//手动选择
	GPFT_CUSTOM_OCCU,		//手动选择满足职业得
	GPFT_CHIJI_EQUIP,		//吃鸡装备
};

//道具续费信息
struct ItemRenewInfo
{
	UInt32	durationDay;		//持续天数
	UInt32	moneyId;			//货币id
	UInt32	moneyNum;			//货币数量
};

// 道具限时判断
enum ItemCheckType
{
	ICT_VALID = 0,				// 有效
	ICT_ABOUT_TO_EXPIRE,		// 快要到期
	ICT_EXPIRE,					// 到期可续费
	ICT_NEED_DELETE,			// 到期被删除
};

// 时限道具
struct TimeItem
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & itemUid & state;
	}

	UInt64 itemUid;
	UInt32 state;		// ItemCheckType
};

//商店类型
enum ShopType
{
	ST_ANCIENT = 0,			//远古商店
	ST_MYSTERY = 1,			//神秘商店
	ST_FORGE = 2,			//打造商店
	ST_BRAVE = 3,			//勇者商店
	ST_FIGHT = 4,			//决斗商店
	ST_GUILD = 5,			//公会商店
	ST_MAGIC = 6,			//魔罐商店
	ST_GOLD = 7,			//金罐商店
	ST_ABYSS = 8,			//深渊商店
	ST_GUILD_STORAGE = 9,	//公会仓库商店
	ST_ACTIVITY = 10,		//活动商店
	ST_DUNGEON = 11,		//地下城商店
	ST_MASTER = 12,			//良师商店
	ST_LEASE = 13,			//租赁商店
	ST_ADVENTURE_TEAM = 14,	//冒险队(佣兵团)商店
	ST_OP_ACTIVITY = 15,	//运营活动商店
	ST_ACCOUNT_BIND_GOLD = 17, //账号绑金商店
	ST_ADVENTURE_PASS = 18, //冒险币商店
	ST_HIRE = 19,           //招募商店
	ST_CHAMPION = 20,		//冠军赛商店
	ST_MONOPOLY = 21,		//大富翁商店
	ST_SECRET_SELL = 22,	//神秘卖场
};

//商店刷新类型
enum ShopRefreshType
{
	SRT_NO_REFRESH = 0,		//不能刷新
	SRT_BLACK = 1,			//黑市刷新(可手动刷新限次)
	SRT_NORMAL = 2,			//普通刷新(不可手动刷新不限次)
	SRT_NORMAL_MANUAL = 3,	//可手动刷新不限次数
};

/**
*@brief 装备回收ScoreItem
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
	// 装备uid
	UInt64	uid;
	// 积分
	UInt32	score;
};

//装备回收罐子状态
enum EqRecoJarState
{
	EQRECO_JARST_CANNOTOPEN = 0, //不能打开
	EQRECO_JARST_CANOPEN = 1, //可打开
	EQRECO_JARST_OPEND = 2, //已打开过
};

#define FASHION_CMPS_NEED_CAP	1	//时装合成需要的时装包裹空位数
#define FASHION_CMPS_T0_M		12	//时装合成T0的幸运值
#define L_SKY_RESET_NUM			5	//L概率下天空套普通高级概率重置次数
#define L_FIX_ADVANCE_WT		200	//每合出一件高级的天空套修正20
#define L_FIX_COMMON_WT			100	//每合出一件高级的天空套修正10
#define L_FIX_MAX_WT			1000
#define L_COMMON_COMBO_NUM		3	//L概率下连续N次普通修正概率
#define L_ADVANCE_COMBO_NUM		1	//L概率下连续N次高级修正概率

//双剑职业 副武器在已装备武器背包个格子id
#define ACCESSWEAPON_EQUIPED_PACKAGE_GRID_ID 10

//天空套时装类型
enum SkyFashionType
{
	SFT_COMMON = 1, //普通
	SFT_ADVANCE = 2, //高级
	SFT_TRANSP = 3, //透明
};

enum ItemLockType
{
	ILT_ITEM_UNLOCK	= 0,			// 道具没锁
	ILT_ITEM_LOCK	= 1 << 0,		// 道具锁
	ILT_LEASE_LOCK  = 1 << 1,		// 租赁锁
	ILT_AUCTION_RETURN_LOCK  = 1 << 2,     // 拍卖行返还锁
	ILT_FASHION_LOCK = 1 << 3,		// 时装锁

	// 任意锁
	ILT_ANY_LOCK = ILT_ITEM_LOCK | ILT_LEASE_LOCK | ILT_AUCTION_RETURN_LOCK,
};

enum PreciousBeadOpType
{
	PRECIOUS_BEAD_MOUNT		= 1,	// 宝珠镶嵌
	PRECIOUS_BEAD_UPGRADE	= 2,	// 宝珠升级
	PRECIOUS_BEAD_EXTRIPE	= 3,	// 宝珠摘取
	PRECIOUS_BEAD_REPLACE	= 4,	// 宝珠置换
	PRECIOUS_BEAD_CONVERT   = 5,	// 宝珠转换
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

	UInt32 currencyId; //对应货币id
	std::string name;
	UInt32 value;
};

#define EQ_PREADBEAD_MAXNUM		2	//装备宝珠镶嵌孔最大数量

enum UpgradePreciousBeadType
{
	UP_PRECBEAD_UNMOUNTED = 0, //未镶嵌
	UP_PRECBEAD_MOUNTED = 1,   //已镶嵌
};

#define PRECBEAD_UP_NEED_CAP	1	//宝珠升级,摘除需要的包裹空位数

//检查道具请求类型
enum CheckMailReqType
{
	CMRT_NONE = 0, 
	CMRT_AUCTION_PUNISH_CANCEL_TRANS = 1, //拍卖行惩罚取消交易检查道具
	CMRT_BLACKMARKET_TRANSACTION_REQ = 2, //黑市商人申请交易
};

//道具绑定类型
enum ItemBindType
{
	IBTP_NONE = 0,
	IBTP_NOT  = 1,	//非绑定
	IBTP_ROLE = 2,  //角色绑定
	IBTP_ACCT = 3,  //账号绑定
};

//附魔卡一键合成结束协议
enum MagicCardCompOneKeyEndReason
{
	MCCER_NONE = 0,
	MCCER_TIMES_MAX = 1,		//次数达到最大
	MCCER_ITEMPACK_FULL = 2,	//背包满
	MCCER_MONEY_INSUFF = 3,		//货币不足
	MCCER_MONEY_MAX = 4,
};

enum FlyUpStatus
{
	//没飞升
	FLY_UP_STATUS_OUT = 0,
	//飞升中
	FLY_UP_STATUS_IN = 1,
};

enum EquipInscriptionSynthesisNumLimit
{
	EQUIP_INSCRIPTION_LIMIT_MIN = 2,
	EQUIP_INSCRIPTION_LIMIT_MAX = 5,
};

//道具好友赠送信息项
struct FriendPresentInfo
{
	FriendPresentInfo() : guid(0){}
	
	AVALON_DEFINE_NETSERIALIZATION() 
	{
		stream  & guid & friendId & friendOcc & friendLev & friendname & isOnline 
			& beSendedTimes & beSendedLimit & sendTimes & sendLimit & sendedTotalTimes & sendedTotalLimit;
	}

	//唯一id
	ObjID_t		guid;
	//好友id
	ObjID_t		friendId;
	//好友职业
	UInt8		friendOcc;
	//好友等级
	UInt16		friendLev;
	//好友名字
	std::string friendname;
	//在线状态
	UInt8		isOnline;
	//给我赠送数量
	UInt32		beSendedTimes;
	//被赠送上限
	UInt32		beSendedLimit;
	//我赠送数量
	UInt32		sendTimes;
	//赠送上限
	UInt32		sendLimit;
	//被赠送总次数
	UInt32		sendedTotalTimes;
	//被赠送总次数上限
	UInt32      sendedTotalLimit;
};

enum EquipConvertType
{
	EQCTP_SAME = 1,	//同套装
	EQCTP_DIFF = 2,	//跨套装
};


//冒险通行证王者版解锁方式
enum AdventurePassOpenKingType
{
	APOKT_ITEM = 0,
	APOKT_RMB = 1,
};

enum ItemKeyValuesKeyType
{
	IKVKT_NONE = 0,
	IKVKT_WBP_STR = 1,		//力量
	IKVKT_WBP_STA = 2,		//体力
	IKVKT_WBP_INT = 3,		//智力
	IKVKT_WBP_SPR = 4,		//精神
	IKVKT_WBP_HPMAX = 5,		//hp最大值
	IKVKT_WBP_MPMAX = 6,		//mp最大值
	IKVKT_WBP_HPREC = 7,		//hp恢复
	IKVKT_WBP_MPREC = 8,		//mp恢复
	IKVKT_WBP_HIT = 9,		//命中率
	IKVKT_WBP_DEX = 10,		//回避率
	IKVKT_WBP_PHYCRT = 11,		//物理暴击
	IKVKT_WBP_MGCCRT = 12,		//魔法暴击
	IKVKT_WBP_ATKSPD = 13,		//攻速
	IKVKT_WBP_RDYSPD = 14,		//施放速度
	IKVKT_WBP_MOVSPD = 15,		//移动速度
	IKVKT_WBP_JUMP = 16,		//跳跃力
	IKVKT_WBP_HITREC = 17,		//硬直
	IKVKT_WBP_PHYATK = 18,		//物攻
	IKVKT_WBP_MAGATK = 19,		//法攻
	IKVKT_WBP_PHYDEF = 20,		//物防
	IKVKT_WBP_MAGDEF = 21,		//法防	
	IKVKT_WBP_LIGHT = 22,       //光属性攻击
	IKVKT_WBP_FIRE = 23,		  //火属性攻击
	IKVKT_WBP_ICE = 24,		  //冰属性攻击
	IKVKT_WBP_DARK = 25,			//暗属性攻击
	IKVKT_WBP_LIGHT_ATK = 26, //光属性强化
	IKVKT_WBP_FIRE_ATK = 27,	//火属性强化
	IKVKT_WBP_ICE_ATK = 28,	//冰属性强化
	IKVKT_WBP_DARK_ATK = 29,	//暗属性强化
	IKVKT_WBP_LIGHT_DEF = 30, //光属性抗性
	IKVKT_WBP_FIRE_DEF = 31,	//火属性抗性
	IKVKT_WBP_ICE_DEF = 32,	//冰属性抗性
	IKVKT_WBP_DARK_DEF = 33,	//暗属性抗性
	IKVKT_WBP_GDKX = 34,		//感电抗性
	IKVKT_WBP_CXKX = 35,		//出血抗性
	IKVKT_WBP_ZSKX = 36,		//灼烧抗性
	IKVKT_WBP_ZDKX = 37,		//中毒抗性
	IKVKT_WBP_SMKX = 38,		//失明抗性
	IKVKT_WBP_XYKX = 39,		//眩晕抗性
	IKVKT_WBP_SHKX = 40,		//石化抗性
	IKVKT_WBP_BDKX = 41,		//冰冻抗性
	IKVKT_WBP_SLKX = 42,		//睡眠抗性
	IKVKT_WBP_HLKX = 43,		//混乱抗性
	IKVKT_WBP_SFKX = 44,		//束缚抗性
	IKVKT_WBP_JSKX = 45,		//减速抗性
	IKVKT_WBP_ZZKX = 46,		//诅咒抗性
	IKVKT_WBP_YKXZ = 47,		//所有异常抗性
	IKVKT_WBP_INDEPEND_ATK = 48, //独立攻击力
	IKVKT_WBP_PhySkillMp = 49, //物理技能耗蓝变化
	IKVKT_WBP_PhySkillCd = 50 , //物理技能CD变化
	IKVKT_WBP_MagSkillMp = 51 , //魔法技能耗蓝变化
	IKVKT_WBP_MagSkillCd = 52 , //魔法技能CD变化
	IKVKT_WBP_QSKX = 53, //侵蚀抗性


	IKVKT_WBP_BUFF_BEGIN = 100,
	//特殊属性（buff信息表ID）
	IKVKT_WBP_BUFF_END = 200,

	IKVKT_WBP_SPECIAL = 300, //装备特性类型
	IKVKT_WBP_BUILD_WEAPON = 301, //打造出的装备
	IKVKT_WBP_SPECIAL_DETAIL = 302, //装备特性数值
	IKVKT_WBP_ADDITION_SCORE = 303, //打造出武器的附加评分
	
	IKVKT_WBP_MAX,
	
	IKVKT_MAX,
};

//绑定金币id
#define BIND_GOLD_ID 600000007

#endif
