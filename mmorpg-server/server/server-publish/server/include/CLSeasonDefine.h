#ifndef _CL_SEASON_DEFINE_H_
#define _CL_SEASON_DEFINE_H_

#define SEASON_ID_KEY "season_id"
#define SEASON_START_TIME_KEY "season_start_time"
#define SEASON_DURATION_TIME_KEY "season_duration_time"

#define SEASON_CHANGE_ATTR_TIME "season_change_attr_time"

enum SeasonEventType
{
	SEASON_EVENT_TYPE_INVALID = 0,
	SEASON_EVENT_TYPE_SEASON = 1,
	SEASON_EVENT_TYPE_SEASON_ATTR = 2,
};

enum SeasonCyclesType
{
	SCT_INVALID,
	SCT_MONTH,	//月
	SCT_WEEK,	//周
};

// 赛季状态
enum SeasonStatus
{
	GS_INVALID,			//未开始
	GS_PROGRESS,		//进行中
};

enum SeasonPlayStatus
{
	SPS_INVALID,			//
	SPS_NEW = 1,			//新赛季
	SPS_NEW_ATT = 2,		//新属性
};

enum SeasonUplevelStatus
{ 
	SUS_INVALID,			//未开始晋级赛
	SUS_UPLEVEL,			//晋级赛中
};

enum SeasonMainLevel
{
	SML_BRONZE = 1,		//青铜
	SML_SILVER,			//白银
	SML_GOLD,			//黄金
	SML_PLATINUM,		//铂金
	SML_DIAMOND,		//钻石
	SML_KING,			//最强王者
};


struct SeasonExpEntry
{
	SeasonExpEntry(UInt32 _winExp, UInt32 _loseExp)
	{
		winExp = _winExp;
		loseExp = _loseExp;
	}

	UInt32 winExp;
	UInt32 loseExp;
};


#endif	//_CL_SEASON_DEFINE_H_
