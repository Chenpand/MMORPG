#ifndef _CL_BET_HORSE_DEFINE_H_
#define _CL_BET_HORSE_DEFINE_H_

// 概率放大1000倍
#define RATE_EXPAND_NUM		(10000)

enum WeatherType
{
	WEATHER_RAIN = 1,	// 雨天
	WEATHER_SUNNY = 2,	// 晴天
	WEATHER_FOGGY = 3,	// 雾天
};

enum TerrainType
{
	TERRAIN_MOUNTAIN = 1,	// 山地
	TERRAIN_TOWN = 2,	// 城镇
	TERRAIN_PLAIN = 3,	// 平原
};

enum ShooterStatusType
{
	SHOOTER_STATUS_EXCELLENT = 1, // 优秀(红色)
	SHOOTER_STATUS_GOOD = 2, // 良好(黄色)
	SHOOTER_STATUS_INSTABLE = 3, // 不稳定(蓝色)
	SHOOTER_STATUS_COMMONLY = 4, // 较差(绿色)
};

enum BetHorsePhaseType
{
	PHASE_TYPE_READY = 1,	// 服务器准备
	PHASE_TYPE_STAKE = 2,	// 押注阶段(1-90分钟)
	PHASE_TYPE_ADJUST = 3,	// 调整阶段(91-100分钟)
	PHASE_TYPE_RESULT = 4,	// 结果阶段(101-120分钟)
	PHASE_TYPE_DAY_END = 5,	// 今日比赛结束
};

enum BetHorseOccupationType
{
	OCCUPATION_TYPE_SNIPER	= 1,	// 狙击枪
	OCCUPATION_TYPE_RIFLE = 2,	// 冲锋枪
	OCCUPATION_TYPE_ASSAULT = 3,	// 突击步枪
};

enum MapPhaseType
{
	MAP_PHASE_FIRST	= 1,	// 首轮地图
	MAP_PHASE_SECOND = 2,	// 次轮地图
	MAP_PHASE_LAST = 3,		// 最后一轮
};


#endif