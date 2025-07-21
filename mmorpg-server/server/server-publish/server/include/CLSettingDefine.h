#ifndef _CL_SETTING_DEFINE_H_
#define _CL_SETTING_DEFINE_H_

#include <CLDefine.h>

/**
 *@brief 快捷栏格子数
 */
#define QUICKBAR_GRID_NUM 20
#define SKILLBAR_GRID_NUM 10
#define ITEMBAR_GRID_NUM 1
#define WEAPON_GRID_NUM 1	//副武器
#define SKILLBAR_NUM	4	//方案数量  adder by huchenhui 2016.07.22
#define SETTING_MIX		1	//最小值


 /**
 *@brief 无效格子位
 */
#define QUICKBAR_GRID_INVALID 0XFF

/**
 *@brief 快捷栏类型
 */
enum QuickBarType
{
	QUICKBAR_INVALID = 0,	//无效
	QUICKBAR_SKILL = 1,		//技能
	QUICKBAR_ITEM = 2,		//道具
	
	QUICKBAR_MAX,
};


/**
*@brief 公平竞技查询配置类型
*/
enum EqualPvpSkillConfigReqType
{
	EQUAL_PVP_SKILL_CONFIG_QUERY = 0,	//查询
	EQUAL_PVP_SKIL_LCONFIG_SET = 1,		//设置
};

/**
 *@brief 快捷栏格子
 */
struct QuickBarGrid
{
	QuickBarGrid():type(0), pos(0), id(0){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & type & pos & id;
	}
	//类型
	UInt8 type;
	//位置
	UInt8 pos;
	//id
	UInt32 id;
};

/**
*@brief 技能栏格子
*/
struct SkillBarGrid
{
	SkillBarGrid() :slot(0), id(0) {}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & slot & id;
	}

	//位置
	UInt8	slot;
	//id
	UInt16	id;
};

typedef std::vector<SkillBarGrid> SkillGridVec;

/**
*@brief 技能栏
*/
struct SkillBarInfo
{
	SkillBarInfo() :index(0) {}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & index & grids;
	}

	UInt8	index;
	SkillGridVec grids;

	UInt32 GetSkillSlotUseNum()
	{
		UInt32 k = 0;
		for (auto& it : grids)
		{
			if (it.id != 0)
				++k;
		}

		return k;
	}
};

/**
*@brief 技能栏方案
*/
struct SkillBarInfos
{
	SkillBarInfos(): index(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & index & skillBars;
	}

	UInt8 index;
	std::vector<SkillBarInfo> skillBars;
};

struct ItemBarGrid
{
	ItemBarGrid() :slot(0), id(0) {}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & slot & id;
	}

	//位置
	UInt8	slot;
	//id
	UInt32	id;
};

/**
 *@brief 游戏选项，位
 */
enum GameOption
{
	GAME_OPTION_REFUSE_TRIBE_REQUEST = 1 << 0,	//拒绝部落邀请
	GAME_OPTION_REFUSE_TEAM_REQUEST =  1 << 1,	//拒绝组队邀请
	GAME_OPTION_REFUSE_TRADE_REQUEST = 1 << 2,	//拒绝交易邀请
	GAME_OPTION_REFUSE_FRIEND_REQUEST = 1 << 3,	//拒绝好友邀请
	GAME_OPTION_REFUSE_PRIVATECHAT =	1 << 4,	//拒绝私聊
	GAME_OPTION_ELITE_DUNGEON_NOTCOUME_ENERGY = 1 << 5, //精英地下城不消耗精力
};

enum GameSetType
{
	// 好友邀请
	GST_FRIEND_INVATE = 1,
	// 隐私设置
	GST_SECRET = 2,

	GST_MAX
};

enum SecretSetType
{
	// 公会邀请
	SST_GUILD_INVATE = 1 << 0,
	// 组队邀请
	SST_TEAM_INVATE =  1 << 1,
	// 决斗邀请
	SST_DUEL_INVATE = 1 << 2,
};

//
enum SaveOptionMask
{
	SOM_NOT_COUSUME_EBERGY = 1 << 0,	// 是否不消耗精力(精英地下城)
};

//
#define MAX_GAMEFUNCTION_OPEN_NUM 120

#endif
