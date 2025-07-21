#ifndef _CL_DAILY_TODO_DEFINE_H_
#define _CL_DAILY_TODO_DEFINE_H_
#include <CLDefine.h>

enum DailyTodoSubType
{
	DTSTP_NONE = 0,
	DTSTP_DIALY_TASK,			//日常任务
	DTSTP_MAIN_DUNGEON,			//主线地下城
	DTSTP_SHENYUAN_DUNGEON,		//深渊地下城
	DTSTP_YUANGU_DUNGEON,		//远古地下城
	DTSTP_CITY_MONSTER_DUNGEON,	//怪物攻城
	DTSTP_XUKONG_DUNGEON,		//虚空裂缝
	DTSTP_HUNDUN_DUNGEON,		//混沌地下城
	DTSTP_COMMON_TEAM_COPY_ONE = 8,		//克茜拉普通团本
	DTSTP_DIFF_TEAM_COPY_ONE = 18,		//克茜拉噩梦团本
	DTSTP_COMMON_TEAM_COPY_TWO = 19,	//群风岛普通团本
	DTSTP_DIFF_TEAM_COPY_TWO = 20,		//群风岛噩梦团本

	DTSTP_MAX,
};

//每日必做
struct DailyTodoInfo
{
	DailyTodoInfo()
	{
		id = 0;
		dataId = 0;
		dayProgress = 0;
		dayProgTime = 0;
		dayProgMax = 0;
		weekProgress = 0;
		weekProgTime = 0;
		weekProgMax = 0;
	}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & dataId & dayProgress & dayProgTime & dayProgMax & weekProgress & weekProgTime & weekProgMax;
	}
	UInt64		id;				//唯一id
	UInt32		dataId;			//表id
	UInt32		dayProgress;	//每日进度
	UInt32		dayProgTime;	//每日进度更新时间戳
	UInt32		dayProgMax;		//每日进度最大值
	UInt32		weekProgress;	//每周进度
	UInt32		weekProgTime;	//每周进度更新时间戳
	UInt32		weekProgMax;	//每周进度最大值
};


#endif