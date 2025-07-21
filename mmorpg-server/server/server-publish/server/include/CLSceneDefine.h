#ifndef _CL_SCENE_DEFINE_H_
#define _CL_SCENE_DEFINE_H_

#include <CLDefine.h>
#include <AvalonRandom.h>

//主城
#define MAJORCITY_MAPID 5000
#define MAPID_CHIJI_SINGLE	10001
#define MAPID_CHIJI_BATTLE	10100
#define MAPID_CHIJI_PREPARE	10101
#define	MAPID_LOST_DUNGEON	11000
#define MAPID_LOST_DUNGEON_ENTRY	11001

//获取中立区主城
inline UInt32 GetNeutralRandMajorCity()
{
	const static UInt32 maps[] = {713, 714, 715, 716, 717};
	return maps[Avalon::Random::RandBetween(0, sizeof(maps) / sizeof(UInt32) - 1)];
}

/**
 *@brief 阻挡位
 */
enum SceneBlockFlag
{
	SCENE_BLOCK_NONE =			0,		//无阻挡
	SCENE_BLOCK_PHYSICS =	1 << 0,		//物理阻挡
	SCENE_BLOCK_MAGIC =		1 << 1,		//魔法阻挡
	SCENE_BLOCK_ALPHA =		1 << 2,		//透明处理，客户端用
	SCENE_BLOCK_SAFEAREA =	1 << 3,		//安全区

	SCENE_BLOCK_TRAP =		1 << 4,		//陷阱
	SCENE_BLOCK_TRANSPOINT =	1 << 5,	//跳转点
	SCENE_BLOCK_NPC =		1 << 6,		//npc阻挡
	SCENE_BLOCK_ITEM =		1 << 7,		//道具阻挡
};

/**
 *@brief 地图类型
 */
enum SceneType
{
	SCENE_TYPE_INVALID =	0,		// 无效类型
	SCENE_TYPE_NORMAL =		1,		// 普通地图
	SCENE_TYPE_PRIVATESCENE =	2,	// 单人场景
	SCENE_TYPE_TEAMCOPY =	3,		// 队伍副本
	SCENE_TYPE_BATTLE =		4,		// 战场类型
	SCENE_TYPE_PRIVATE_TEAMCOPY = 5,// 个人拥有的队伍副本
	SCENE_TYPE_PUBLICCOPY = 6,		// 公共副本
	SCENE_TYPE_TRIBE = 7,			// 部落副本
	SCENE_TYPE_GHOST = 8,			// 镜像场景
    SCENE_TYPE_SINGLE = 9,          // 个人场景  
    SCENE_TYPE_DUNGEON_ENTRY = 10,  // 选择地下城的场景
	SCENE_TYPE_PK_PREPARE = 11,     // 战斗准备区的场景
	SCENE_TYPE_PK = 12,				// pk场景
	SCENE_TYPE_ACTIVITY = 13,		// 活动场景
	SCENE_TYPE_CHIJI_PREPARE = 14,	// 吃鸡准备区的场景
	SCENE_TYPE_TEAM_COPY = 15,		// 团本
	SCENE_TYPE_LOST_DUNGEON = 16,	// 迷失地牢场景

	SCENE_TYPE_CHAMPION = 17,		//冠军赛
	SCENE_TYPE_DUNGEON_ENTRY_DYNAMIC = 18,  // 动态选择地下城的场景
	SCENE_TYPE_MAX,
};

/**
 *@brief 判断是不是静态地图
 */
inline bool IsStaticMap(UInt8 type)
{
	switch(type)
	{
	case SCENE_TYPE_NORMAL:
	case SCENE_TYPE_GHOST:
    case SCENE_TYPE_SINGLE:
    case SCENE_TYPE_DUNGEON_ENTRY:
	case SCENE_TYPE_PK_PREPARE:
	case SCENE_TYPE_PK:
	case SCENE_TYPE_ACTIVITY:
	case SCENE_TYPE_CHIJI_PREPARE:
	case SCENE_TYPE_TEAM_COPY:
	case SCENE_TYPE_CHAMPION:
		return true;
	}
	return false;
}

/**
 *@brief 判断是不是动态地图
 */
inline bool IsDynMap(UInt8 type)
{
	if(type == SCENE_TYPE_INVALID || type >= SCENE_TYPE_MAX) return false;
	return !IsStaticMap(type);
}

/**
 *@brief 判断是不是副本
 */
inline bool IsCopyScene(UInt8 type)
{
	switch(type)
	{
	case SCENE_TYPE_PRIVATESCENE:
	case SCENE_TYPE_TEAMCOPY:
	case SCENE_TYPE_PRIVATE_TEAMCOPY:
	case SCENE_TYPE_PUBLICCOPY:
		return true;
	}
	return false;
}

/**
 *@brief 判断能否允许跨服进副本                                                                     
 */
inline bool IsTourCopyScene(UInt8 type)
{
	switch(type)
	{
	case SCENE_TYPE_PUBLICCOPY:
		return true;
	}
	return false;
}

/**
 *@brief 动态地图开始
 */
#define MIN_DYNSCENE_ID 1000000

/**
 *@brief 获取镜像场景id
 */
inline UInt32 MakeGhostSceneId(UInt32 mapId, UInt32 serialId)
{
	return serialId * MIN_DYNSCENE_ID / 10 + mapId;
}

/**
 *@brief 获取真实场景id
 */
inline UInt32 GetRealSceneId(UInt32 sceneId)
{
	if(sceneId >= MIN_DYNSCENE_ID) return sceneId;
	return sceneId % (MIN_DYNSCENE_ID / 10);
}

/**
 *@brief 判断是否能用瞬移道具
 */
inline bool IsCanUseTeleportItem(UInt8 type)
{
	if(type == SCENE_TYPE_INVALID || type >= SCENE_TYPE_MAX) return false;
	switch(type)
	{
	case SCENE_TYPE_BATTLE:
		return false;
	}
	return true;
}

/**
 *@brief 判断是否全奖励
 */
inline bool IsFullExpItemReward(UInt8 type)
{
	switch(type)
	{
	case SCENE_TYPE_TEAMCOPY:
	case SCENE_TYPE_PRIVATE_TEAMCOPY:
		return true;
	}
	return false;
}

/**
 *@brief 根据场景id是否静态地图
 */
inline bool IsStaticMapId(UInt32 id)
{
	return id < MIN_DYNSCENE_ID;
}

/**
 *@brief 地图选项
 */
enum SceneOption
{
	SCENE_OPTION_RIDE =	1 << 0,				//是否可骑乘
	SCENE_OPTION_FLY =	1 << 1,				//是否可飞行    ----废弃
	SCENE_OPTION_PK =	1 << 2,				//是否可pk
	SCENE_OPTION_DAILY = 1 << 3,			//是否显示在日常活动界面
	SCENE_OPTION_PKFREE = 1 << 4,			//是否自由pk场景
	SCENE_OPTION_FORBIDFLY = 1 << 5,		//禁止使用小飞鞋，卷轴等瞬移道具
	SCENE_OPTION_NO_NPCBLOCK = 1 << 6,		//无npc阻挡
	SCENE_OPTION_PICKUPBIND = 1 << 7,		//拾取绑定
    SCENE_OPTION_NO_ENTER_SCENE = 1 << 8,   //不需要发进入场景的协议
};

/**
 *@brief 副本最大难度
 */
#define COPYSCENE_DIFFICULTY_NORMAL 0
#define MAX_DIFFICULTY_LEVEL 2
/**
 *@brief 最长副本时间
 */
#define MAX_COOPYSCENE_TIME (3 * HOUR_TO_SEC)
/**
 *@brief 副本死亡复活时间
 */
#define COPYSCENE_REVIVE_TIME 20
/**
 *@brief 副本进入拉人超时时间
 */
#define COPYSCENE_PULL_TIMEOUT 60

/**
 *@brief 副本队伍信息
 */
struct CopySceneTeamInfo
{
	CopySceneTeamInfo():id(0), playerNum(0), avgLevel(0), hasPwd(0){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & title & playerNum & avgLevel & hasPwd;
	}

	//队伍id
	UInt32	id;
	//队伍标题
	std::string	title;
	//人数
	UInt8	playerNum;
	//玩家平均等级
	UInt16	avgLevel;
	//是否有密码
	UInt8	hasPwd;
};

/**
 *@brief 副本进入类型
 */
enum CopySceneEnterType
{
	COPYSCENE_ENTER_NORMAL = 0,		//界面进入

	COPYSCENE_ENTER_PULL = 3,		//拉人进入
	COPYSCENE_ENTER_CHARGE = 4,		//回应扣费进入
};


/**
 *@brief npc位置信息
 */
struct NpcPosInfo
{
	NpcPosInfo():mapId(0){}

	UInt32	mapId;
	CLPosition	pos;
};

/**
 *@brief 封神台
 */
#define SCENE_FENGSHENTAI 402

/**
 *@brief 远古盛宴
 */
#define SCENE_YUANGUSHENGYAN 403

/**
 *@brief 真.狐首丘
 */
#define SCENE_ZHENHUQIU   506

/**
 *@brief 擂台赛地图id
 */
#define SCENE_CHALLENGE_ID	703

/**
 *@brief 渡劫地图id
 */
#define SCENE_DUJIE_ID 900

// 战场类型
enum BATTLESCENE_TYPE
{
	BATTLESCENE_WUDOU_LOW	= 1,	// 低级武斗场
	BATTLESCENE_WUDOU_MIDDLE = 2,	// 中级武斗场
	BATTLESCENE_WUDOU_HIGH  = 3,	// 高级武斗场

	BATTLESCENE_TRIBEWAR	= 5,	// 宝地战
	BATTLESCENE_WORLDCHALLENGE = 6,	// 跨服挑战赛
	BATTLESCENE_TRIBEMELEE	= 7,	// 水晶战
	BATTLESCENE_JIEJIE		= 8,	// 结界
	BATTLESCENE_WORLD_WUDOU_LOW = 9,		//初级跨服武斗场
	BATTLESCENE_WORLD_WUDOU_MIDDLE = 10,	//中级跨服武斗场
	BATTLESCENE_WORLD_WUDOU_HIGH = 11,		//高级跨服武斗场
	BATTLESCENE_WORLD_WUDOU_FINAL = 12,		//终极跨服武斗场
};

enum TOWER_TYPE
{
	TOWER_FENGMO = 1,			//封魔塔
	TOWER_TONGTIAN = 2,			//通天塔
	TOWER_CHENGSHEN = 3			//成神之路
};



const static UInt32 DROP_NOTIFY_MAPS[] = { 112, 126, 127, 128, 402 };

inline bool IsDropNotifyMapId(UInt32 id)
{
	for(UInt32 i = 0; i < sizeof(DROP_NOTIFY_MAPS) / sizeof(DROP_NOTIFY_MAPS[0]); ++i)
	{
		if(DROP_NOTIFY_MAPS[i] == id) return true;
	}
	return false;
}

inline bool IsGrowthCopyScene(UInt32 id)
{
	if (id >= 1000 && id <= 1053) return true;
	return false;
}

inline bool IsWudouBattleScene(int type)
{
	if((type >= BATTLESCENE_WUDOU_LOW && type <= BATTLESCENE_WUDOU_LOW)
		||(type >= BATTLESCENE_WORLD_WUDOU_LOW && type <= BATTLESCENE_WORLD_WUDOU_LOW))
		return true;
	return false;
}

inline bool IsSceneNeedCheckLevel(int type)
{
	if (type >= SCENE_TYPE_SINGLE && type <= SCENE_TYPE_ACTIVITY)
	{
		return false;
	}

	return true;
}

#endif

