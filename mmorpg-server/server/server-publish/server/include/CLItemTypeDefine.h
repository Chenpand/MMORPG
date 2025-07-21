#ifndef _CL_ITEM_TYPE_DEFINE_H_
#define _CL_ITEM_TYPE_DEFINE_H_

#include "CLDefine.h"

/**
 *@brief 道具类型
 */
enum ItemType
{
	ITEM_INVALID =			0,	//无效道具
	ITEM_EQUIP =			1,	//装备（拥有子类型）
	ITEM_EXPENDABLE,			//消耗品
	ITEM_MATERIAL,				//材料
	ITEM_TASK,					//任务
	ITEM_FASHION,				//装扮
	ITEM_INCOME,				//收益，进包裹自动获得货币
	ITEM_ENERGY_STONE,			//能量石 added by huchenhui 2016.07.21
	ITEM_TITLE,					//称号
	ITEM_PET = 10,				//宠物
	ITEM_GUILD_BUFF = 11,		//公会地下城buff
	ITEM_HEAD_FRAME = 12,		//头像框
	ITEM_SKILL = 13,			//技能
	ITEM_NEWTITLE = 14,			//头衔
	ITEM_INSCRIPTION = 15,		//铭文
	ITEM_LOSTDUNGEON = 16, //迷失地牢物品
	ITEM_MONOPOLY_CARD = 17,	//大富翁卡牌
	ITEM_MAX,					//最大类型数

	//------------废弃---------------------------------
	ITEM_MEDICINE,				//药剂（拥有子类型）
	ITME_GEMSTONE,				//宝石
	ITEM_PACK_ACTIVE,			//包裹扩充			灵活字段1  扩充格子数
	ITEM_RUNE,					//符文
	ITEM_COPYSCENE,				//副本开启类道具	灵活字段1为生成传送门陷阱id  字段2为传送门持续时间(秒) 	字段3为副本地图id
	ITEM_VIPCARD,				//vip卡				灵活字段1为vip等级  字段2为时长(天） 字段3为1表示为体验卡
	//TODO
	ITEM_TELEPORT,				//瞬移类道具		灵活字段1为类型  0随机 1回城 2定点  灵活字段2为读条时间(秒)
	ITEM_TREASUREBOX,			//宝箱
	ITEM_MESS,					//杂物	不可使用
	ITME_IMPEACHBRAND,			//部落首领弹劾令
	//ITEM_PET,					//宠物道具
	ITEM_GIFTBAG,				//礼包类道具		使用走脚本
	ITEM_TREASUREMAP,			//藏宝图
	ITEM_FIREWORKS,				//烟花				灵活字段1为持续时间（秒)
	ITEM_MONEY_USE,				//游戏币，通过使用获得货币，子类型同ITEM_MONEY，灵活字段1为货币值
	ITEM_ADD_BUFF,				//自身加buff,		灵活字段1为buff id，灵活字段2为持续时间，灵活字段3为buff的值
	ITEM_SCRIPT,				//脚本道具
	ITEM_LOGINLIMIT,			//受登录限制的道具类型
	ITEM_GIVE,					//赠送类型
	ITEM_WING,					//翅膀相关道具
	ITEM_MOUNT,					//坐骑道具
	ITEM_RIDE_BOOK,				//骑乘技能书
};

//道具子类别
enum ItemSubType
{
	ST_ITEM_INVALID = 0,

	ST_EQUIP_WEAPON,			//武器
	ST_EQUIP_HEAD,			//头部
	ST_EQUIP_CHEST,			//胸
	ST_EQUIP_BELT,			//腰带
	ST_EQUIP_LEG,			//护腿
	ST_EQUIP_BOOT,			//鞋子
	ST_EQUIP_RING,			//戒指
	ST_EQUIP_NECKLASE,		//项链
	ST_EQUIP_BRACELET,		//手镯
	ST_EQUIP_TITLE,			//称号

	ST_FASHION_HALO,		//时装翅膀
	ST_FASHION_HEAD,			//时装头部
	ST_FASHION_SASH,		//时装腰
	ST_FASHION_CHEST,		//时装上衣
	ST_FASHION_LEG,			//时装下衣
	ST_FASHION_EPAULET,			//时装胸饰

	ST_ITEM_GOLD,			//金币
	ST_ITEM_POINT,			//点券
	ST_ITEM_EXP,				//经验

	ST_NOT_USE_20,

	ST_ENERGY,              //能量石
	ST_WARRIOR_SOUL,		//勇者之魂
	ST_PK_COIN,				//决斗币
	ST_JINGPO,				//精魄
	ST_MAGICCARD,			//附魔卡
	ST_REVIVE_COIN,			//复活币

	ST_BIND_GOLD,			//绑金
	ST_BIND_POINT,			//绑定点券

	ST_GIFT_PACKAGE,		//礼包

	ST_GUILD_CONTRI,		//公会贡献

	ST_SP,					//SP点
	ST_ENERY_STONE,			//能量石
	ST_COUPON,				//强化卷
	ST_MONTH_CARD,			//月卡
	ST_JAR,					//随机罐子
	ST_GIFT_BOX,			//礼盒（带预览）
	ST_FATIGUE_DRUG,		//疲劳药
	ST_DRAWING,				//图纸残卷
	ST_FRAGMENT,			//碎片
	ST_VIP_EXP,				//vip经验
	ST_EXP_PILL,			//经验丸
	ST_GOLDJAR_POINT,		//金罐积分
	ST_MAGJAR_POINT,		//魔罐积分
	ST_PET_EGG,				//宠物蛋
	ST_FASHION_COMPOSER,	//时装合成器
	ST_MONEY_MANAGE_CARD,	//理财卡

	ST_HP_POTION = 50,		//HP药剂
	ST_MP_POTION,			//MP药剂
	ST_HPMP_POTION,			//HP+MP药剂
	ST_CHANGE_NAME,			//改名卡
	ST_BEAD,				//宝珠
	ST_MAGIC_BOX,			//魔盒
	ST_MAGIC_HAMMER,		//魔锤
	ST_COLORLESS_CRYSTAL,	//无色晶体
	ST_JAR_GIFT,			//罐子规则的礼包
	ST_ADD_CHARGE_POINT = 60,	//补发充值积分
	ST_ADD_VIP_POINT = 61,		//补发充值vip积分
	ST_ATTRIBUTE_DRUG = 62,     //属性药水


	// 活动货币
	ST_APPOINTMENT_COIN = 70,	// 预约币
	ST_CONSUME_JAR_GIFT = 73,   //需要消耗道具的魔罐规则的礼包
	ST_PRIMARY_RAFFLE_TICKETS = 74, //普通抽奖券
	ST_MIDDLE_RAFFLE_TICKETS = 75,	//中级抽奖券
	ST_SENIOR_RAFFLE_TICKETS = 76,	//高级抽奖券

	//师们系统值
	ST_MASTER_ACADEMIC_VALUE = 78, //师门学业成长值
	ST_MASTER_GOODTEACH_VALUE = 79, //良师值
	
	//回归硬币
	ST_RETURN_TOKEN = 80,	//回归硬币

	//时装武器
	ST_FASHION_WEAPON = 81,  //时装武器

	// 百变换装节换装卷
	ST_CHANGE_FASHION_ACTIVE_TICKET = 82, // 百变换装节换装卷

	// 换装积分
	ST_DRESS_INTEGRAL_VALUE = 83, // 百变时装活动换装积分
	// 武器租赁好运符 
	ST_WEAPON_LEASE_TICKET = 84,
	// 赐福水晶(佣兵勋章)
	ST_BLESS_CRYSTAL = 87,
	// 传承祝福(成长药剂)
	ST_INHERIT_BLESS = 88,
	// 赏金
	ST_BOUNTY = 91,

	// 升级药水(升1级)
	ST_UPLEVEL = 86,

	//时装光环
	ST_FASHION_GUANGHUAN = 92,		

	//头衔道具
	ST_NEW_TITLE = 94,
	
	// 吃鸡HP药水
	ST_CHIJI_HP = 95,
	// 吃鸡移速药水
	ST_CHIJI_SPEED = 96,
	// 吃鸡投掷物
	ST_CHIJI_THROW = 97,
	// 吃鸡buff道具
	ST_CHIJI_BUFF = 98,

	// 辅助装备
	ST_ASSIST_EQUIP = 99,
	// 魔法石
	ST_MAGICSTONEE = 100,
	// 耳环
	ST_EARRINGS = 101,

	ST_ZENGFU_MATERIAL = 102, //增幅消耗材料
	ST_ZENGFU_ACTIVATE = 103, //虚空物质激活装置
	ST_ZENGFU_CLEANUP = 104, //虚空物质清除装置
	ST_ZENGFU_CHANGE = 105, //虚空物质转换器
	ST_ZENGFU_CREATE = 106, //虚空物质生成装置
	ST_ZENGFU_OVERLOAD = 107, //虚空物质过载装置
	ST_ZENGFU_PROTECT = 108, //增幅保护券
	ST_ZENGFU_AMPLIFICATION = 109, //激化券
		
	ST_ACHIEVEMENT_POINT = 111, //成就点
	ST_CHI_JI_COIN = 112, //吃鸡荣耀点
	ST_ZJSL_WZHJJJ_COIN = 114, //终极试炼王者华戒
	ST_ZJSL_WZHGGG_COIN = 115, //终极试炼王者华冠
	ST_MALL_POINT = 116,			//商城积分
	ST_INSCRIPTION = 117, //铭文

	ST_FLYUPITEM = 118,            //飞升道具

	ST_ADVENTURE_COIN = 119,       //冒险币

	ST_CHALLENGE_SCORE = 122,      //挑战者活动积分

	ST_ADVENTURE_PASS_EXP = 123,   //冒险通行证经验
	ST_ADVENTURE_PASS_ACTIVATE_ITEM = 124, //王者版通行证激活券
	
	ST_FRIEND_PRESENT = 125,	//好友赠送道具

	ST_HONOR_SCORE = 126,	//荣誉

	ST_SPRING_SCORE = 127,  //春季挑战积分

	ST_CHIJI_SKILL = 128,	//吃鸡技能

	ST_CHIJI_SHOP_COIN = 129,	//吃鸡商店货币

	ST_HIRE_COIN = 130,     //招募硬币

	ST_TEAM_COPY_CNT = 131,     //普通团本通行证

	ST_HONOR_GUARD_CARD = 132,     //荣誉保障卡

	ST_ADVENTURE_KING = 133,	//冒险通行证王者版

	ST_USE_DEATH_RESURRECTION = 135, //新爬塔神器道具，死亡复活
	
	ST_LOST_DUNGEON_CURRENCY = 137,	//迷失地宫代币
	
	ST_LOST_DUNGEON_INTEGRAL = 138, //新爬塔积分道具
	
	ST_LOST_DUNGEON_INCREASE_TIMES = 139, //新爬塔增加次数道具

	ST_TEAM_COPY_DIFF_CNT = 140,	//噩梦团本通行证

	ST_MAGIC_DISCOUNT = 142,	//魔罐折扣道具

	ST_LOST_DUNGEON_ADD_SPEED = 143, //新爬塔加速神器

	ST_LOST_DUNGEON_RECOVERY = 144, //新爬塔恢复道具

	ST_CHAMPION_COIN = 145,			//冠军赛竞猜币
	// 虚拟道具
	ST_VIRTUAL		= 120,		// 虚拟道具(实际没有用)
	
	ST_ZENGFU_RAND_TICKET = 146, //激化随机券

	ST_STRENTH_LUCK = 147,		 //幸运强化券

	ST_MAGICCARD_MATERIAL = 148,		 //材料附魔卡道具

	ST_GIFT_RIGHT_CARD = 149,		 //礼遇特权卡

	ST_CREDIT_POINT = 150,		 //信用点券

	ST_FANTASY_COIN = 154,		 //大富翁硬币

	ST_MONOPOLY_CARD = 155,      //大富翁祝福卡

	ST_MONOPOLY_ROLL = 156,      //大富翁骰子

	ST_SECRET_SELL_COIN = 157,      //神秘卖场周年点券

	ST_DUNGEON_TICKET_RIGHT_CARD = 161,//深渊票减免特权卡
};

//道具三级类别
enum ItemThirdType
{
	TT_ITEM_INVALID = 0,
	TT_ITEM_HUGESWORD = 1,		//巨剑
	TT_ITEM_KATANA = 2,			//太刀
	TT_ITEM_SHORTSWORD = 3,		//短剑
	TT_ITEM_BEAMSWORD = 4,		//光剑
	TT_ITEM_BLUNT = 5,			//钝器
	TT_ITEM_REVOLVER = 6,		//左轮
	TT_ITEM_CROSSBOW = 7,		//弩
	TT_ITEM_HANDCANNON = 8,		//手炮
	TT_ITEM_RIFLE = 9,			//步枪
	TT_ITEM_PISTOL = 10,			//手枪
	TT_ITEM_STAFF = 11,			//法杖
	TT_ITEM_WAND = 12,			//魔杖
	TT_ITEM_SPEAR = 13,			//矛
	TT_ITEM_STICK = 14,			//棍棒
	TT_ITEM_BESOM = 15,			//扫把
	TT_ITEM_GLOVE = 16,		//手套
	TT_ITEM_BIKAI = 17,		//臂铠
	TT_ITEM_CLAW = 18,		//爪
	TT_ITEM_OFG = 19,		//拳套
	TT_ITEM_EAST_STICK = 20,	//东方棍
	TT_ITEM_SICKLE	= 21,	//镰刀
	TT_ITEM_TOTEM	= 22,	//图腾
	TT_ITEM_AXE		= 23,	//战斧
	TT_ITEM_BEADS	= 24,	//念珠
	TT_ITEM_CROSS	= 25,	//十字架
	TT_WP_TYPE_MAX = 26,	//武器类型最大值

	TT_ITEM_CLOTH = 51,		//布甲
	TT_ITEM_SKIN = 52,			//皮夹
	TT_ITEM_LIGHT = 53,			//轻甲
	TT_ITEM_HEAVY = 54,			//重甲
	TT_ITEM_PLATE = 55,			//板甲

	TT_FASHION_JUNIOR = 100,	//初级时装
	TT_FASHION_SENIOR = 101,	//高级时装
	TT_FASHION_FESTIVAL = 102,	//节庆时装
	TT_COMPOSER_JUNIOR = 103,	//初级时装合成器
	TT_COMPOSER_SENIOR = 104,	//高级时装合成器

	TT_ZENGFU_COLOR_PURPLE = 200,   //增幅道具针对紫色装备使用
	TT_ZENGFU_COLOR_PINKUP = 201,   //增幅道具对紫色及以上装备使用
	TT_ZENGFU_COLOR_GREEN = 202,    //增幅道具对绿色及以上装备使用

	TT_DISPOSABLE_STRENG_ITEM = 203,   //一次性强化器
	TT_DISPOSABLE_INCREASE_ITEM = 204, //一次性激化器


	TT_SMALL_FATIGUE_POTION = 300,	//小疲劳药
	TT_MID_FATIGUE_POTION = 301,			//中疲劳药
	TT_BIG_FATIGUE_POTION = 302,			//大疲劳药

	TT_DUNGEON_POTION = 401,		//单局内的药水

	TT_ITEM_CHANGE_PLAYER_NAME = 500,	// 个人改名卡
	TT_ITEM_CHANGE_GUILD_NAME = 501,			// 公会改名卡

	IT_ITEM_GOLD_TITLE = 502,			// 金币称号
	IT_ITEM_RENAME_ADVENTURE_TEAM = 503,// 冒险队改名卡
	IT_ITEM_ROLE_EXTEND = 504,			// 角色栏位扩展卡

	IT_ITEM_CONVERTOR_SAME = 506,		//同套装转换器
	IT_ITEM_CONVERTOR_DIFF = 507,		//跨套装转换器

	IT_ITEM_INSCRIPTION_RED		= 800,	// 红色铭文
	IT_ITEM_INSCRIPTION_YELLOW	= 801,	// 黄色铭文
	IT_ITEM_INSCRIPTION_BLUE	= 802,	// 蓝色铭文
	IT_ITEM_INSCRIPTION_DGLOD	= 803,	// 暗金铭文
	IT_ITEM_INSCRIPTION_BGLOD	= 804,	// 耀金铭文
	IT_ITEM_INSCRIPTION_ORANGE	= 805,	// 橙色铭文
	IT_ITEM_INSCRIPTION_GREEN	= 806,	// 绿色铭文
	IT_ITEM_INSCRIPTION_PURPLE	= 807,	// 紫色铭文
};

/**
*@brief 装备类型
*/
enum EquipType
{
	EQUIP_NORMAL = 0,               //普通装备
	EQUIP_HAVE_SMELL = 1,           //带气息的装备
	EQUIP_RED = 2,                  //红字装备
};

/**
*@brief 红字装备增幅类型
*/
enum EnhanceType
{
	ENHANCE_INVALID = 0,              //无效
	ENHANCE_STRENTH = 1,              //力量
	ENHANCE_INTELLECT = 2,            //智力
	ENHANCE_STAMINA = 3,              //体力
	ENHANCE_SPIRIT = 4,               //精神
};


//商品类别
enum ShopItemType
{
	SIT_NONE = 0,
	SIT_MATERIAL,		//素材
	SIT_WEAPON,			//武器
	SIT_ARMOR,			//防具
	SIT_JEWELRY,		//首饰
	SIT_COST,			//消耗品
	SIT_VALUABLE,		//珍品
	SIT_RETINUE,		//随从
};

//道具使用类别
enum ItemUseType
{
	IUT_NONE = 0,		//不可使用
	IUT_USESINGLE,		//可单个使用
	IUT_USEALL,			//可批量使用
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

//商店页签刷新周期
enum RefreshCycleType
{
	//无刷新周期
	REFRESH_CYCLE_NONE,
	//每日刷新
	REFRESH_CYCLE_DAILY,
	//每周刷新
	REFRESH_CYCLE_WEEK,
	//每月刷新
	REFRESH_CYCLE_MONTH,
};

//商店页签刷新周期激活状态
enum RefreshCycleState
{
	//非法
	REFRESH_CYCLE_INVAILD,
	//黑市主动刷新
	REFRESH_CYCLE_ACTIVE,
	//刷新周期激活
	REFRESH_CYCLE_ACTIVATION,
	//刷新周期未激活
	REFRESH_CYCLE_QUIET,
};

//装备部位类别
enum EquipPartType
{
	EPT_NONE = 0,
	EPT_WEAPON,		//武器
	EPT_CLOTH,		//布甲
	EPT_LEATHER,	//皮甲
	EPT_HEAVY,		//重甲
	EPT_PLATE,		//板甲
	EPT_LIGHT,		//轻甲
	EPT_JEWELRY,	//首饰
	EPT_ASSIST,		//辅助装备
	EPT_MAGICSTONE,		//魔法石
};

//装备品级属性类别
enum EquipQLAttrType
{
	EQAT_PHYATK = 0,		//物理攻击
	EQAT_MAGATK,			//魔法攻击
	EQAT_PHYDEF,			//物理防御
	EQAT_MAGDEF,			//魔法防御
	EQAT_STRENGTH,			//力量
	EQAT_STAMINA,			//耐力
	EQAT_SPIRIT,			//精神
	EQAT_INTELLECT,			//智力
	EQAT_LIGHT_STRPROP,		//光属强
	EQAT_FIRE_STRPROP,		//火属强
	EQAT_ICE_STRPROP,		//冰属强
	EQAT_DARK_STRPROP,		//暗属强
	EQAT_LIGHT_DEFPROP,		//光属抗
	EQAT_FIRE_DEFPROP,		//火属抗
	EQAT_ICE_DEFPROP,		//冰属抗
	EQAT_DARK_DEFPROP,		//暗属抗
	EQAT_GDKX,		//感电抗性
	EQAT_CXKX,		//出血抗性
	EQAT_ZSKX,		//灼烧抗性
	EQAT_ZDKX,		//中毒抗性
	EQAT_SMKX,		//失明抗性
	EQAT_YXKX,		//晕眩抗性
	EQAT_SHKX,		//石化抗性
	EQAT_BDKX,		//冰冻抗性
	EQAT_SLEEPKX,		//睡眠抗性
	EQAT_HLKX,		//混乱抗性
	EQAT_SFKX,		//束缚抗性
	EQAT_JSKX,		//减速抗性
	EQAT_ZZKX,		//诅咒抗性
	EQAT_TOTALKX,	//抗性总
	EQAT_INDEPENDATK,	//独立攻击力
	EQAT_MAX,
};

//装备品级调整模式
enum EquipQLMode
{
	EQM_ALL = 0,
	EQM_SINGLE,

	EQM_MAX
};

/**
*@brief 是否是辅助装备
*/
inline bool IsAssistEquip(int type)
{	
	return type == ST_ASSIST_EQUIP;
}

/**
*@brief 是否是魔法石
*/
inline bool IsMagicStoneEquip(int type)
{
	return type == ST_MAGICSTONEE;
}

/**
 *@brief 判断是否为武器
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
 *@brief 判断是否为防具
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
 *@brief 判断是否为首饰
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
 *@brief 判断是否攻击装备
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
 *@brief 判断是否防御装备
 */
inline bool IsDefenseEquip(int type)
{
	return IsDefend(type);
}

/**
 *@brief 判断是否需要计算战斗力
 */
inline bool IsEquipNeedCalPower(int type)
{
// 	return (type != EQUIP_TYPE_INVALID && type < EQUIP_TYPE_WEAPON_MAX) 
// 		|| type == EQUIP_TYPE_LOCK || type == EQUIP_TYPE_WING;
	return false;
}

/**
 *@brief 判断是否可以改造
 */
inline bool IsEquipCanRemake(int type)
{
	//return type != EQUIP_TYPE_INVALID && type < EQUIP_TYPE_WEAPON_MAX;
	return false;
}

/**
*@brief 判断是否吃鸡类型
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
 *@brief 包裹扩充石子类型
 */
enum PackActiveItemType
{
	ITEM_NORMALPACK_ACTIVE = 0,		//普通包裹扩充
	ITEM_STORAGE_ACTIVE =	 1,		//仓库扩充
};

/**
 *@brief 药剂子类型
 */
enum MedicineType
{
	MEDICINE_TYPE_INVALID = 0,		//无效类型
	MEDICINE_TYPE_HP = 1,			//加血 灵活字段1数值量，字段2百分比量
	MEDICINE_TYPE_HPCONTINUED = 2,	//持续加血 字段1buff的id，字段2持续时间(毫秒)
};

/**
 *@brief 任务道具子类型
 */
enum TaskItemType
{
	TASKITEM_NOUSE = 0,			//不可使用道具
	TASKITEM_USE_TARGET = 1,	//需要目标使用		灵活字段1为读条时间（秒）,0表示不读条  
	TASKITEM_USE_NOTARGET = 2,	//不需要目标使用	灵活字段1为读条时间（秒）,0表示不读条
};

/**
 *@brief 脚本道具子类型
 */
enum ScriptItemType
{
	SCRIPTITEM_USE_NOTARGET = 0,	//不需要目标使用 灵活字段1为读条时间（秒）,0表示不读条
	SCRIPTITEM_USE_TARGET = 1,		//需要目标使用   灵活字段1为读条时间（秒）,0表示不读条
	SCRIPTITEM_USE_OPTION = 2,		//带功能选项的脚本道具
	SCRIPTITEM_USE_TARGET_PLAYER = 3,	//需要玩家目标使用   灵活字段1为读条时间（秒）,0表示不读条
};

/**
*@brief 宠物道具子类型
*/
enum PetItemType
{
	PETITEM_INVALID = 0,		// 无效类型
	PETITEM_EGG = 1,			// 宠物蛋	灵活字段1为宠物id
	PETITEM_SKILLBOOK = 2,		// 技能书	灵活字段1为技能id，字段2为技能等级
	PETITEM_EMPTYBOOK = 3,		// 无字天书
	PETITEM_APTITUDE = 4,		// 资质丹
	PETITEM_FOOD = 5,			// 宠物饲料
	PETITEM_GATHERSOUL = 6,		// 聚魂丹   灵活字段1为增加聚魂值
};

/**
*@brief 藏宝图子类型
*/
enum TreasureMapType
{
	TREASUREMAP_INVALID = 0,
	TREASUREMAP_UNUSED = 1,		// 未使用
	TREASUREMAP_USED =2,		// 已使用
};

/**
*@brief 游戏币子类型(废弃)
*/
enum MoneyType
{
	MONEY_INVALID = 0,
	MONEY_BINDSILVER = 20,	//		 =====废弃
	MONEY_GOLD = 1,			// 金币
	MONEY_POINT = 2,		// 点券
	MONEY_EXP = 3,			// 经验
	MONEY_JADESTONE = 4,	// 玉石(废弃)
	MONEY_HONOUR = 5,		// 荣誉(废弃)
	MONEY_BINDGOLD = 6,		// 绑定金贝(废弃)
	MONEY_POINT_DEPRACTED = 7,		// 点数(废弃)
	MONEY_IRONSTONE = 8,	// 铁石(废弃)
	MONEY_SPIRITSTONE = 9,	// 灵石(废弃)
    MONEY_COUPON = 10,      // 点劵(废弃)
    MONEY_WARRIOR_SOUL = 11,  // 勇者之魂
	MONEY_PK_COIN = 12,		// 决斗币
	MONEY_REVIVE_COIN = 13,	// 复活币
	MONEY_GUILD_CONTRI = 14,// 公会贡献
	MONEY_CREDIT_POINT = 15,// 信用点券

	MONEY_ITEM_EX = 21,		// 以物换物
};

/**
 *@brief 受登陆限制的道具子类型
 */
enum LoginLimitItemType
{
	LOGINLIMIT_ITEM_INVALUD = 0,
	LOGINLIMIT_ITEM_BINDGOLD = 1,	//返利卡  ,灵活字段1配置返利总额
};

/**
*@brief 翅膀道具子类型
*/
enum WingItemType
{
	WING_ITEM_INVALID = 0,
	WING_ITEM_FEATHER = 1,		// 羽毛
	WING_ITEM_LEVELUP = 2,		// 翅膀升级道具
	WING_ITEM_LEVELDOWN = 3,	// 翅膀降级道具
	WING_ITEM_FEATHERSOUL = 4,	// 羽魂道具
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
	IULT_DAYLIMITE,		//日限制次数
	IULT_VIPLIMITE,		//vip限制次数
	IULT_TEAMCOPY,		//团本道具
	IULT_HONORLV,		//荣誉等级道具
	IULT_SUITLEV,		//使用套装等级
	IULT_WEEK6LIMITE,	//周限制次数
	IULT_MAX,
};

/**
 *@brief 判断道具类型是否具有品质，绑定外的其他详细信息
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


// 杂物的部落捐献类的
#define MESS_TRIBE_TYPE	8
//小飞鞋
#define MESS_FLY_SHOES	31815
//运送令
#define	TRANS_WOOD_PASSPORT 1107
//小喇叭
#define ITEM_HORN 9
//复活道具
#define ITEM_REVIVE 10
//英雄令道具
#define ITEM_HERO_TOKEN	126

//改名卡
#define ITEM_CHANGENAME_ID			45
//黑暗塔遗忘
#define ITEM_DT_FORGETSKILL_ID		47
//变性符
#define ITEM_CHANGESEX_ID			656

// 金币
#define ITEM_GOLD					600000001
// 点券
#define ITEM_POINT					600000002
// 经验道具
#define ITEM_EXP					600000003
// 绑定点券
#define ITEM_BIND_POINT				600000008
// 信用点券
#define ITEM_CREDIT_POINT			600002551
// 死亡币道具
#define ITEM_WARRIOR_SOUL			300001101
// 死亡之塔地下城ID
#define DEATH_TOWER_DUNGEON_ID		2003000


// 上一次获得得合成时装结果
enum LastGetSkyFashionType
{
	LGSFT_NONE = 0,
	LGSFT_NORMAL = 1,
	LGSFT_GOLD = 2,
};

enum ItemOperateEnum
{
	IPOE_INVALE = 0,
	IPOE_STRENG = 1,    //强化
	IPOE_UPGRADE = 2,   //升级
	IPOE_ADDMAGIC = 3,  //附魔
	IPOE_MOUNTBEAD = 4, //宝珠
	IPOE_DECOMPOSE = 5, //分解
	IPOE_ENHANCE = 6,   //激化
	IPOE_CLEARSMELL = 7, //清除气息
	IPOE_REDACTIVE = 8, //激活
	IPOE_REDCHAGE = 9, //转化
};

#endif
