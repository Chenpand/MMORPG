#ifndef __CL_DUNGEON_CONFIG_MGR_H__
#define __CL_DUNGEON_CONFIG_MGR_H__

#include <set>
#include <CLDefine.h>

// 地下城开放时间
struct DungeonOpenTime
{
	DungeonOpenTime() : beginHour(0), beginMinute(0), endHour(0), endMinute(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & beginHour & beginMinute & endHour & endMinute;
	}

	UInt8	beginHour;
	UInt8	beginMinute;
	UInt8	endHour;
	UInt8	endMinute;
};
typedef std::vector<DungeonOpenTime> VDungeonOpenTime;

// 地下城每日配置
struct DungeonDailyConfig
{
	DungeonDailyConfig() : maxTimes(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & dayOfWeek & openTimes & maxTimes;
	}

	// 周几
	UInt8							dayOfWeek;
	// 当天的开放时间
	VDungeonOpenTime				openTimes;
	// 当天的最多次数
	UInt8							maxTimes;
};

// 地下城每周配置
struct DungeonWeeklyConfig
{
	DungeonWeeklyConfig() : newConfig(true) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream.SeriaArray(dailyConfigs, sizeof(dailyConfigs) / sizeof(dailyConfigs[0]));
	}

	// 是不是新增的配置
	bool					newConfig;
	// 每天的配置
	DungeonDailyConfig		dailyConfigs[7];
};

// 配置地下城的开放时间和开放次数，以周为单位
class CLDungeonConfigMgr
{
public:
	CLDungeonConfigMgr();
	~CLDungeonConfigMgr();

public:
	/**
	*@brief 设置地下城开放情况
	*/
	void SetDungeonOpen(UInt32 dungeonId, UInt8 maxTimes);
	void SetDungeonClose(UInt32 dungeonId);

	/**
	*@brief 地下城是否开放（如果不在地下城配置中，就认为这个图是开放的）
	*/
	bool IsDungeonOpened(UInt32 dungeonId);

	/**
	*@brief 当天地下城最多次数
	*/
	UInt8 GetDungeonMaxTimes(UInt32 dungeonId);

	/**
	*@brief 添加每周的配置
	*/
	void AddWeeklyConfig(UInt32 dungeonId, const DungeonWeeklyConfig& config);

	/**
	*@brief 添加具体时间的地下城开放时间，如：星期天10:00-12:00
	*/
	void AddConfig(UInt32 dungeonId, UInt8 dayOfWeek, const VDungeonOpenTime& openTimes, UInt8 maxTimes);

	/**
	*@brief 清除地下城配置
	*/
	void ClearConfig(UInt32 dungeonId);

public:
	/**
	*@brief 地下城开放事件
	*/
	virtual void OnDungeonOpened(UInt32 dungeonId, UInt8 maxTimes) {}

	/**
	*@brief 地下城关闭事件
	*/
	virtual void OnDungeonClosed(UInt32 dungeonId) {}

protected:
	// 所有的关卡配置
	std::map<UInt32, DungeonWeeklyConfig>	m_dungeonConfigs;

	// 目前开放的地下城，key是地下城ID，value是最多次数
	std::map<UInt32, UInt8>					m_openedDungeon;
};

#endif