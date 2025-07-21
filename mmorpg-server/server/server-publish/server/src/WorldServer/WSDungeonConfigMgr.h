#ifndef __WS_DUNGEON_CONFIG_MGR_H__
#define __WS_DUNGEON_CONFIG_MGR_H__

#include <CLDungeonConfigMgr.h>
#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>

class WSDungeonConfigMgr : public CLDungeonConfigMgr, public Avalon::Singleton<WSDungeonConfigMgr>
{
public:
	WSDungeonConfigMgr();
	~WSDungeonConfigMgr();

	bool Initialize();
		
	void OnTick(const Avalon::Time& now);

	void OnSceneConnected(UInt32 id);

public:
	/**
	*@brief 设置每日固定时间的地下城开放时间
	*/
	void SetDailyConfig(UInt32 dungeonId, const VDungeonOpenTime& openTimes, UInt8 maxTimes);

	/**
	*@brief 设置每周固定几天的地下城开放时间
	*/
	void SetWeeklyConfig(UInt32 dungeonId, std::vector<UInt8> dayOfWeeks, const VDungeonOpenTime& openTimes, UInt8 maxTimes);

	/**
	*@brief 根据字符串获取开放时间，格式如下：10:00-12:00|14:00-16:00
	*/
	bool GetDungeonOpenTimeByStr(const std::string openTimeStr, VDungeonOpenTime& openTimes);

public:
	/**
	*@brief 地下城开放事件
	*/
	virtual void OnDungeonOpened(UInt32 dungeonId, UInt8 maxTimes);

	/**
	*@brief 地下城关闭事件
	*/
	virtual void OnDungeonClosed(UInt32 dungeonId);

protected:
	bool _CheckDungeonOpenByTime(const DungeonDailyConfig& config, const Avalon::Date& date);

private:
	Avalon::SimpleTimer m_tickTimer;
};

#endif