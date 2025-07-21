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
	*@brief ����ÿ�չ̶�ʱ��ĵ��³ǿ���ʱ��
	*/
	void SetDailyConfig(UInt32 dungeonId, const VDungeonOpenTime& openTimes, UInt8 maxTimes);

	/**
	*@brief ����ÿ�̶ܹ�����ĵ��³ǿ���ʱ��
	*/
	void SetWeeklyConfig(UInt32 dungeonId, std::vector<UInt8> dayOfWeeks, const VDungeonOpenTime& openTimes, UInt8 maxTimes);

	/**
	*@brief �����ַ�����ȡ����ʱ�䣬��ʽ���£�10:00-12:00|14:00-16:00
	*/
	bool GetDungeonOpenTimeByStr(const std::string openTimeStr, VDungeonOpenTime& openTimes);

public:
	/**
	*@brief ���³ǿ����¼�
	*/
	virtual void OnDungeonOpened(UInt32 dungeonId, UInt8 maxTimes);

	/**
	*@brief ���³ǹر��¼�
	*/
	virtual void OnDungeonClosed(UInt32 dungeonId);

protected:
	bool _CheckDungeonOpenByTime(const DungeonDailyConfig& config, const Avalon::Date& date);

private:
	Avalon::SimpleTimer m_tickTimer;
};

#endif