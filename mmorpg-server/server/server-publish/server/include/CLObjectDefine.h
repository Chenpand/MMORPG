#ifndef _CL_OBJECT_DEFINE_H_
#define _CL_OBJECT_DEFINE_H_

#include "CLDefine.h"

/**
 *@brief npc和玩家的状态
 */
enum CreatureStatus
{
	CREATURE_STATUS_NORMAL = 0,		//正常状态
	CREATURE_STATUS_DEAD =	 1,		//死亡状态
    CREATURE_STATUS_WALK =   2,     //走路
    CREATURE_STATUS_RUN  =   3,     //跑步
	CREATURE_STATUS_PK   =   4,     //PK中
};

/**
 *@brief 场景obj类型
 */
enum SceneObjectType
{
	SOT_INVALID,	//无效
	SOT_NPC,		//npc
	SOT_PLAYER,		//玩家
	SOT_ITEM,		//道具
	SOT_TRAP,		//陷阱
	SOT_PET,		//宠物
	SOT_FIREWORKS,	//烟花
	SOT_SUMMONNPC,	//玩家的召唤npc
	SOT_VEHICLE,	//交通工具,
	SOT_CITYMONSTER,	//怪物
};

/**
 *@brief 判断是不是生物
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
 *@brief 判断是不是召唤物
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
 *@brief 场景object的属性枚举
 */
enum SceneObjectAttr
{
	SOA_INVALID,	//无效
	SOA_NAME,		//名字	string
	SOA_LEVEL,		//等级	UInt16
	SOA_EXP,		//经验	UInt64
	SOA_BLESSEXP,	//祝福经验	UInt32
	SOA_SEX,		//性别	UInt8
	SOA_OCCU,		//职业	UInt8
	SOA_PKMODE,		//pk模式	UInt8
	SOA_PKCOLOR,	//pk颜色	UInt8
	SOA_VIPLVL,		//vip等级   UInt8
	SOA_QQVIPINFO,	//qq平台特权信息 QQVipInfo
	SOA_EVIL,		//罪恶	UInt16;
	SOA_ATTACK,		//攻击	UInt32
	SOA_DEFENSE,	//防御	UInt32
	SOA_HIT,		//命中	UInt8
	SOA_DODGE,		//闪避	UInt8
	SOA_THUMP,		//暴击	UInt32
	SOA_HOLDOFF,	//抵挡	UInt32
	SOA_THUMPEFFECT,	//暴击效果 UInt32
	SOA_CHAOSATTACK,	//混沌攻击 UInt32
	SOA_CHAOSDEFENSE,	//混沌防御 UInt32
	SOA_SWIM,			//眩晕	   UInt32
	SOA_SPEEDDOWN,		//减速     UInt32
	SOA_IMMOBILE,		//定身	   UInt32
	SOA_SILENT,			//沉默	   UInt32
	SOA_SWIM_DEF,		//眩晕抗性	UInt32
	SOA_SPEEDDOWN_DEF,	//减速抗性	UInt32
	SOA_IMMOBILE_DEF,	//定身抗性	UInt32
	SOA_SILENT_DEF,		//沉默抗性	UInt32
	SOA_HP,				//生命值	UInt32
	SOA_MAXHP,			//最大生命值	UInt32
	SOA_POWER,			//战斗力	UInt32
	SOA_MOVESPEED,		//移动速度	UInt16
	SOA_POSITION,		//当前位置	CLPosition, UInt16+UInt16
	SOA_DIR,			//方向		UInt8
	SOA_VEHICLEID,		//车id		ObjID_t
	SOA_SKILLS,			//技能列表  只发送给自身 格式:id(UInt16)  + level(UInt8) ... 0(UInt16)
	SOA_BUFFS,			//buff列表 同步给自身和可见玩家 格式:id(UInt16) + value(Int32) + time(UInt32) ... 0(UInt16)
	SOA_ITEMCD,			//道具cd	只同步给自身 格式:cd组(UInt8) + cd时间(UInt32) ... 0(UInt8)
	SOA_GOLD,			//银币	UInt32
	SOA_BINDGOLD,		//绑定金币 UInt32
	SOA_POINT,			//金币 UInt32
	SOA_BINDPOINT,		//绑定点券 UInt32
	SOA_SPIRITSTONE,	//灵石 UInt32
	SOA_HONOUR,			//荣誉 UInt32
	SOA_PACKSIZE,		//随身普通包裹大小 UInt8
	SOA_STORAGESIZE,	//仓库大小		   UInt8
	SOA_EQUIPS,			//周围可见的装备  衣服(UInt32) + 装备(UInt32)
	SOA_SUITEFFECT,		//套装效果 UInt32 |全套强化等级UInt4|全套6级宝石数UInt12(宝石等级UInt4 + 宝石个数UInt8)|全套品质UInt8|全套随机属性星级UInt8|
	SOA_BUFFCONTROL,	//buff控制标识	UInt16(BuffControlFlag组合)
	SOA_DATAID,			//数据项id UInt32
	SOA_STATUS,			//状态     UInt8   
	SOA_NPCTASKSTATUS,	//npc任务状态 UInt8 NpcTaskStatus
	SOA_SKILLBAR,		//技能栏  只发送自身 格式: pos(UInt8 0开始) + id(UInt16) ... 0xFF(UInt8)
	SOA_GAMEOPTIONS,	//游戏选项
	SOA_SUBNAME,		//子名字 string
	SOA_TEAMMASTER,		//是否队长	UInt8 0表示不是 1表示是队长
	SOA_TEAMSIZE,		//队伍大小  UInt8 0表示没队伍
	SOA_TRIBEID,		//所属部落id	UInt64
	SOA_TRIBEPOST,		//部落职务	UInt8
	SOA_ITEMBIND,		//道具绑定  UInt8
	SOA_ITEMQUALITY,	//道具品质	UInt8
	SOA_ITEMNUM,		//道具数量	UInt16
	SOA_TRAPID,			//陷阱id UInt32
	SOA_MOUNTID,		//当前骑乘的坐骑 id(UInt32)
	SOA_BATTLEGROUP,	//战场阵营	UInt8
	SOA_OWNERID,		//道具、召唤物等所有者ID UInt64
	SOA_SUMMONAIMODE,	//召唤物ai模式	UInt8	
	SOA_TEAMID,			//道具所属队伍ID UInt32
	SOA_AUTOHOOK,		//自动挂机信息，只在场景服务器同步
	SOA_ESCORTERID,		//护送者id ObjID_t
	SOA_TITLEMASK,		//称号掩码 16长度的字节数组
	SOA_TITLE,			//当前使用称号  UInt32
	SOA_COMPLETETIME,	//建筑物NPC的建成时间 UInt32
	SOA_DEATHOWNER,		//死亡归属者 ObjID_t
	SOA_POWERDETAIL,	//战斗力详细信息 只发送给自身，格式：id(UInt8) + 值(UInt32) ... 0(UInt8)
	SOA_UPDATEBULLETIN,	//更新版本号 UInt32
	SOA_FUNCNOTIFY,		//预告功能 UInt8
	SOA_STORY,			//当前剧情 UInt16
	SOA_ACTIONFLAG,		//行为标记 UInt32
	SOA_FUNCFLAG,		//功能开启标志 char[8]
	SOA_LOVERNAME,		//伴侣名字 string
	SOA_SRCZONEID,		//原区号	 UInt16
	SOA_PETDATAID,		//宠物数据id UInt32
	S0A_PETTRANSLIST,	//宠物幻化列表 char[25]
	SOA_TRANSID,		//幻化id UInt32
	SOA_DAYCHARGENUM,      //日充值累计   UInt32
	SOA_GATHERSOULVAL,	//聚魂值 UInt32
	SOA_SOULPACK,		//兽魂背包 PetSoul[12]
	SOA_ZONEID,			//区号	UInt16
	SOA_TRANSFIGUREID,	//当前变身ID		UInt32
    SOA_SCENE_POS,      //场景中的位置 ScenePos, UInt32+UInt32
    SOA_SCENE_DIR,      //场景中的方向 UInt16(360度)
	SOA_SP,				//SP	UInt32
	SOA_AWAKEN,			//是否觉醒  UInt8
	SOA_ITEMBAR,		//物品栏  只发送自身 格式: pos(UInt8 0开始) + id(UInt32) ... 0(UInt8)
    SOA_PKVALUE,		//决斗场积分  UInt32
	SOA_FATIGUE,		//疲劳值	UInt16
    SOA_WARRIOR_SOUL,   //勇者之魂   UInt32
    SOA_MATCH_SCORE,    //匹配积分 UInt32
    SOA_WARP_STONE,		//次元石	格式:  id(UInt32) + level(UInt8) + energy(UInt32) ... 0(UInt32)
    SOA_COUNTER,        //各种计数 CounterMgr
	SOA_RETINUE,		//随从 UInt32
	SOA_PK_COIN,		//决斗币 UInt32
	SOA_AVATAR,			//外观 PlayerAvatar
	SOA_REVIVE_COIN,	//复活币 UInt32
	SOA_VIP,			//vip
	SOA_CREATE_TIME,	//创建时间
	SOA_NEW_BOOT,		//新手引导
	SOA_TOWER_WIPEOUT_END_TIME,	//死亡之塔扫荡结束时间 UInt32
	SOA_GUILD_POST,		//公会职务 UInt8
	SOA_GUILD_CONTRI,	//帮贡 UInt32
	SOA_GUILD_SHOP,		//公会商店ID UInt32
	SOA_RED_POINT,		//红点
	SOA_GUILD_NAME,		//公会名 string
	SOA_GUILD_BATTLE_NUMBER,	//公会战次数	UInt32
	SOA_GUILD_BATTLE_SCORE,		//公会战积分	UInt32
	SOA_GUILD_BATTLE_REWARD,	//公会战奖励	
	SOA_DAILY_TASK_SCORE,		//每日任务积分	UInt32
	SOA_DAILY_TASK_REWARD_MASK,	//每日任务积分奖励
	SOA_WUDAO_STATUS,	// 武道大会状态 UInt8
	SOA_MONTH_CARD_EXPIRE_TIME,	//月卡到期时间 UInt32
	SOA_CUSTOM_FIELD,			//自定义字段 UInt32
	SOA_SEASON_LEVEL,			//赛季等级	UInt32
	SOA_SEASON_STAR,			//赛季星星	UInt32
	SOA_SEASON_UPLEVEL_RECORD,	//赛季晋级赛记录	PkRaceResult + ... + 0(UInt8)
	SOA_SEASON_ATTR,			//赛季属性id		UInt8
	SOA_SEASON_KING_MARK,		//王者印记			UInt16
	SOA_AUCTION_REFRESH_TIME,	//上一次刷新拍卖行时间
	SOA_AUCTION_ADDITION_BOOTH,	//拍卖行额外栏位
	SOA_GOLDJAR_POINT,			//金罐积分
	SOA_MAGJAR_POINT,			//魔罐积分
	SOA_BOOT_FLAG,				//弱引导
	SOA_SEASON_EXP,				//赛季经验	UInt32
	SOA_PET_FOLLOW,				//跟随的宠物 UInt64
	SOA_POTION_SET,				//药水设置
	SOA_PRE_OCCU,				//预转职职业 UInt8
	SOA_GUILD_BATTLE_LOTTERY_STATUS,	//公会战抽奖状态 UInt8
	SOA_PVP_SKILLS,						//PVP技能列表  只发送给自身 格式:id(UInt16)  + level(UInt8) ... 0(UInt16)
	SOA_PVP_SKILLBAR,					//技能栏  只发送自身 格式: pos(UInt8 0开始) + id(UInt16) ... 0xFF(UInt8)
	SOA_PVP_SP,					//SP	UInt32
	SOA_ACHIEVEMENT_SCORE,		//成就积分 UInt32
	SOA_ACHIEVEMENT_TASK_REWARD_MASK, //成就积分奖励
	SOA_WEAPON_BAR,						//武器栏
	SOA_APPOINTMENT_OCCU,				//是否是预约角色
	SOA_PACKAGE_SIZE_ADDITION,	//包裹容量加成
	SOA_MONEY_MANAGE_STATUS,			//理财管理状态
	SOA_MONEY_MANAGE_REWARD_MASK,		//理财管理奖励
	SOA_SCORE_WAR_SCORE,				//积分赛积分
	SOA_SCORE_WAR_BATTLE_COUNT,			//积分赛战斗次数
	SOA_SCORE_WAR_REWARD_MASK,			//积分赛参与奖
	SOA_SCORE_WAR_WIN_BATTLE_COUNT,		//积分赛战斗胜利次数
	SOA_ACADEMIC_TOTAL_GROWTH_VALUE,	//徒弟学业成长值
	SOA_MASTER_DAILYTASK_GROWTH,		//师门日常任务成长总值
	SOA_MASTER_ACADEMICTASK_GROWTH,	    //师门学业成长总值
	SOA_MASTER_UPLEVEL_GROWTH,			//师门升级成长总值
	SOA_MASTER_GIVEEQUIP_GROWTH,		//师门赠送装备成长总值
	SOA_MASTER_GIVEGIFT_GROWTH,			//师门赠送礼包成长总值
	SOA_MASTER_TEAMCLEARDUNGON_GROWTH,	//师门师徒组队通过地下城成长总值
	SOA_MASTER_GOODTEACHER_VALUE,		//良师值
	SOA_ROOM_ID,						//房间ID
	SOA_SHOW_FASHION_WEAPON,			//显示时装武器
	SOA_WEAPON_LEASE_TICKETS,			//武器租赁好运符
	SOA_GAME_SET,						//游戏设置
	SOA_NOVICE_GUIDE_CHOOSE_FLAG,		//新手引导选择标志
	SOA_ADVENTURE_TEAM_NAME,            //冒险队(佣兵团)名
	SOA_HEAD_FRAME,					    //头像框
	SOA_NEW_TITLE_NAME,					//穿戴头衔名
	SOA_NEW_TITLE_GUID,					//穿戴头衔GUID
	SOA_CHIJI_HP		= 169,			//吃鸡HP
	SOA_CHIJI_MP		= 170,			//吃鸡MP
	SOA_PACKAGE_TYPE	= 171,			//包裹类型的大小
	SOA_GUILD_EMBLEM	= 172,			//公会徽记
	SOA_OPPO_VIP_LEVEL	= 173,			//oppo vip等级
	SOA_CHIJI_SCORE		= 174,			//吃鸡积分
	SOA_EQUAL_PVP_SKILLS,				//公平竞技场PVP技能列表  只发送给自身 格式:id(UInt16)  + level(UInt8) ... 0(UInt16)
	SOA_EQUAL_PVP_SKILLBAR,				//公平竞技场技能栏  只发送自身 格式: pos(UInt8 0开始) + id(UInt16) ... 0xFF(UInt8)
	SOA_EQUAL_PVP_SP,					//公平竞技场SP	UInt32
	SOA_ACCOUNT_ACHIEVEMENT_SCORE,	   //账号成就积分
	SOA_MALL_POINT = 179,			    //商城积分 UInt32
	SOA_TOTAL_EQUIP_SCORE = 180,		//全身装备评分 UInt32
	SOA_ADVENTURE_COIN = 181,			//冒险币
	SOA_LOSTDUNGEON_HP,					//地牢HP
	SOA_LOSTDUNGEON_MP,					//地牢MP
	SOA_LOSTDUNGEON_SCORE,				//地牢积分
	SOA_LOSTDUNGEON_BATTLE_ST,			//地牢战斗状态
	SOA_CREDIT_POINT,					//信用点券
	SOA_CREDIT_POINT_MONTH_GET,			//信用点券月获得
}; 


/**
 *@brief 切场景数据
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
		
	//唯一id
	ObjID_t	id;
	//账号id
	UInt32	accid;
	//网关id
	UInt32	gateid;
	//源节点
	UInt32	srcnode;
	//源场景id
	UInt32	srcscene;
	//目标服务器
	UInt32	destnode;
	//目标场景id
	UInt32	destscene;
};


/** 
 *@brief 陷阱形状
 */
enum TrapShape
{
	TRAP_SHAPE_CIRCLE = 0,	//圆形
	TRAP_SHAPE_RECT = 1,	//矩形
};

/**
 *@brief 陷阱类型
 */
enum TrapType
{
	TRAP_TYPE_NORMAL =	0,	//普通陷阱
	TRAP_TYPE_BLOCK =	1,	//阻挡陷阱
	TRAP_TYPE_TRANS =	2,	//传送陷阱
	TRAP_TYPE_EVENT =	3,	//触发脚本事件的陷阱
	TRAP_TYPE_EFFECT =	4,	//特效陷阱
	TRAP_TYPE_LAST	= 5,    //持续陷阱
};

/**
 *@brief 是否需要设置陷阱阻挡
 */
inline bool IsTrapSetBlock(int type)
{
	return type != TRAP_TYPE_EFFECT;
}

/**
 *@brief 陷阱操控buff
 */
#define TRAP_HANDLE_BUFF 2102

/**
*@brief 同步事件类型
*/
enum SyncEventType
{
	SET_INVALID,
	SET_FRIEND_GIVE,	//好友赠送
	SET_REVIVE,			// 复活
	SET_ADD_AUCTION,	//拍卖行上架
	SET_SELL_AUCTION,	//拍卖行售出
	SET_ADD_REDPOINT,	//添加红点
	SET_CLR_REDPOINT,	//清除红点
	SET_OPEN_FREE_GOLD_CHEST, //打开免费的黄金宝箱
	SET_BUY_DUNGEON_TIMES,	// 购买地下城次数
	SET_AUCTION_REFRESH_TIME,// 拍卖行刷新时间
	SET_NEW_YEAR_RED_PACKET, //新年红包
	SET_PREMIUM_LEAGUE_ENROLL,// 赏金联赛报名
	SET_BUY_AUCTION,		//拍卖行购买
	SET_JOIN_GUILD,			//加入工会
	SET_GUILD_TERR_CHEST_DOUBLE,	// 工会领地翻牌加倍
	SET_GUILD_BATTLE_SCORE,			//工会战积分
	SET_GUILD_OCCUPY_TERRITORY,		//占领领地
	SET_DUNGEON_USE_POTION,		// 地下城中吃药
	SET_ENTER_CROSS_SCENE,		// 进入跨服场景
	SET_EXIT_CROSS_SCENE,		// 退出跨服场景
	SET_RELATION_RELIEVE_MASTER, //解除师徒关系
	SET_RELATION_DISCIPLE_FINISHSCHOOL, //徒弟出师
	SET_RELATION_DISCIPLE_APPRENTICE,  //徒弟拜师成功
	SET_ADD_GOODTEACH_VALUE,	//添加良师值
	SET_REMOVE_FATIGUE,			//消耗疲劳
	SET_OPPO_VIP_LEVEL,			//oppo vip等级
	SET_ACCOUT_ACHIVE_SCORE,	//设置账号成就积分
	SET_LOST_DUNG_HP_MP,		//设置地牢血量，蓝量
	SET_LOST_DUNG_BATTLE_ST,	//设置玩家地牢战场状态
	SET_LOST_DUNG_PROT_TIME,	//设置玩家地牢战场保护时间戳
	SET_TEAM_COPY_CLEAR_DUNGEON,	//团本通关地下城
};

/**
*@brief 觉醒状态
*/
enum AwakenStatus
{
	AWAKEN_NOTHING = 0,		//没有觉醒
	AWAKEN_ONE = 1,			//一觉
	AWAKEN_TWO = 2,			//二觉
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

// 城镇npc（目前就支持城镇怪物）
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
	// npc类型，对应枚举SceneObjectType
	UInt8				type;
	// 功能类型（不同SceneObjectType有不同的含义）
	// SOT_CITYMONSTER -> CityMonsterType
	UInt8				funcType;
	UInt32				id;
	NpcPos				pos;
	// 剩余次数
	UInt32				remainTimes;
	// 总次数
	UInt32				totalTimes;
	// 对应的地下城
	UInt32				dungeonId;
	// 是否战斗中
	UInt8				inBattle;

};

// 场景npc信息
struct SceneNpcInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & sceneId & npcs;
	}

	UInt32					sceneId;
	std::vector<SceneNpc>	npcs;
};

//场景物品坐标
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

// 场景物品
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
	//调试信息
	UInt32				waveId;
	SceneItemPos		blueCenter;
	UInt32				blueRadius;

	//地牢资源数据
	//资源表id
	UInt32				resourceId;
	//资源坐标编组id
	UInt32				resPosGroupId;
	//刷新时间
	UInt32				refreshTime;
	//是否已经刷新
	bool				refreshed;
};

// 场景物品信息
struct SceneItemInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & sceneId & items;
	}

	UInt32					sceneId;
	std::vector<SceneItem>	items;
};

//吃鸡死亡原因
enum ChiJiDeadReason
{
	CHIJI_DEAD_REASON_OFFLINE = 0,	//掉线
	CHIJI_DEAD_REASON_BE_KILLED = 1,	//被杀
	CHIJI_DEAD_REASON_POISIONED = 2,	//毒死
	CHIJI_DEAD_REASON_BALANCE = 3,	//结算
	CHIJI_DEAD_REASON_TIMEOUT = 4,	//超时
};

// 获得经验原因
enum PlayerIncExpReason
{
	PIER_INVALID,
	// 使用经验丸(经验是绝对值)
	PIER_EXP_PILL_VALUE,
	// 使用经验丸(经验是百分比)
	PIER_EXP_PILL_PERCENT,
};

#endif
