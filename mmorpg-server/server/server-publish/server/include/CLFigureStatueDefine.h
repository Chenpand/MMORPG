#ifndef _CL_FIGURE_STATUE_H_
#define _CL_FIGURE_STATUE_H_


#include <CLGameDefine.h>

// 人物雕像类型
enum FigureStatueType
{
	FST_INVALID = 0,
	FST_GUILD,					// 公会雕像
	FST_GUILD_ASSISTANT,		// 公会副会长雕像
	FST_GUILD_ASSISTANT_TWO,	// 公会副会长雕像2

	FST_GUILD_DUNGEON_FIRST,	// 公会地下城伤害第一名
	FST_GUILD_DUNGEON_SECOND,	// 公会地下城伤害第二名
	FST_GUILD_DUNGEON_THIRD,	// 公会地下城伤害第三名
};



// 人物雕像
struct FigureStatueInfo
{
	FigureStatueInfo()
	{
		accId = 0;
		roleId = 0;
		occu = 0;
		statueType = 0;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & accId & roleId & name & occu & avatar & statueType & guildName;
	}

	// 账号id
	UInt32			accId;
	// 角色id
	UInt64			roleId;
	// 玩家名字
	std::string		name;
	// 玩家职业
	UInt8			occu;
	// 玩家外观
	PlayerAvatar	avatar;
	// 人物雕像类型
	UInt8			statueType;
	// 公会名字
	std::string		guildName;
};

typedef std::vector<FigureStatueInfo> FigureStatueVec;

#endif //_CL_FIGURE_STATUE_H_

