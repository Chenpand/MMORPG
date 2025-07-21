#ifndef __CL_DUNGEON_CONFIG_MGR_H__
#define __CL_DUNGEON_CONFIG_MGR_H__

#include <set>
#include <CLDefine.h>

// ���³ǿ���ʱ��
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

// ���³�ÿ������
struct DungeonDailyConfig
{
	DungeonDailyConfig() : maxTimes(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & dayOfWeek & openTimes & maxTimes;
	}

	// �ܼ�
	UInt8							dayOfWeek;
	// ����Ŀ���ʱ��
	VDungeonOpenTime				openTimes;
	// �����������
	UInt8							maxTimes;
};

// ���³�ÿ������
struct DungeonWeeklyConfig
{
	DungeonWeeklyConfig() : newConfig(true) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream.SeriaArray(dailyConfigs, sizeof(dailyConfigs) / sizeof(dailyConfigs[0]));
	}

	// �ǲ�������������
	bool					newConfig;
	// ÿ�������
	DungeonDailyConfig		dailyConfigs[7];
};

// ���õ��³ǵĿ���ʱ��Ϳ��Ŵ���������Ϊ��λ
class CLDungeonConfigMgr
{
public:
	CLDungeonConfigMgr();
	~CLDungeonConfigMgr();

public:
	/**
	*@brief ���õ��³ǿ������
	*/
	void SetDungeonOpen(UInt32 dungeonId, UInt8 maxTimes);
	void SetDungeonClose(UInt32 dungeonId);

	/**
	*@brief ���³��Ƿ񿪷ţ�������ڵ��³������У�����Ϊ���ͼ�ǿ��ŵģ�
	*/
	bool IsDungeonOpened(UInt32 dungeonId);

	/**
	*@brief ������³�������
	*/
	UInt8 GetDungeonMaxTimes(UInt32 dungeonId);

	/**
	*@brief ���ÿ�ܵ�����
	*/
	void AddWeeklyConfig(UInt32 dungeonId, const DungeonWeeklyConfig& config);

	/**
	*@brief ��Ӿ���ʱ��ĵ��³ǿ���ʱ�䣬�磺������10:00-12:00
	*/
	void AddConfig(UInt32 dungeonId, UInt8 dayOfWeek, const VDungeonOpenTime& openTimes, UInt8 maxTimes);

	/**
	*@brief ������³�����
	*/
	void ClearConfig(UInt32 dungeonId);

public:
	/**
	*@brief ���³ǿ����¼�
	*/
	virtual void OnDungeonOpened(UInt32 dungeonId, UInt8 maxTimes) {}

	/**
	*@brief ���³ǹر��¼�
	*/
	virtual void OnDungeonClosed(UInt32 dungeonId) {}

protected:
	// ���еĹؿ�����
	std::map<UInt32, DungeonWeeklyConfig>	m_dungeonConfigs;

	// Ŀǰ���ŵĵ��³ǣ�key�ǵ��³�ID��value��������
	std::map<UInt32, UInt8>					m_openedDungeon;
};

#endif