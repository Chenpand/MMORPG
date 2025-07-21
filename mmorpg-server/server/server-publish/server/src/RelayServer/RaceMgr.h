#ifndef __RACE_MGR_H__
#define __RACE_MGR_H__

#include <AvalonSingleton.h>
#include <AvalonPropertySet.h>
#include <AvalonSimpleTimer.h>
#include <CLDefine.h>
#include <CLMatchDefine.h>
#include "RaceDefine.h"

struct ReconnectInfo
{
	ReconnectInfo()
	{
		memset(this, 0, sizeof(*this));
	}

	ObjID_t		roldId;
	ObjID_t		raceId;
	UInt32		timeoutTime;
};

// 战斗统计
struct RaceStatistic
{
	RaceStatistic()
	{
		memset(this, 0, sizeof(*this));
	}

	// 创建的比赛数量
	UInt32		createNum;
	// 开始的比赛数量
	UInt32		startNum;
	// 不同步的比赛数量
	UInt32		unsyncNum;
};

class Race;
class RaceMgr : public Avalon::Singleton<RaceMgr>
{
public:
    RaceMgr();
    ~RaceMgr();

	bool OnInit(Avalon::PropertySet* prop);

    void OnTick(const Avalon::Time& now);

    Race* CreateRace(RaceType type);

    void AddRace(Race* race);
    Race* FindRace(ObjID_t id);
    
    template<class T>
    T* FindRaceWithType(ObjID_t id)
    {
        auto race = FindRace(id);
        if (!race)
        {
            return NULL;
        }

        return dynamic_cast<T*>(race);
    }

    void DelRace(ObjID_t id);

public:
	// 重连相关

	/**
	*@brief 能否
	*/
	const ReconnectInfo* GetReconnectInfo(ObjID_t roleId);

	/**
	*@brief 删除重连信息
	*/
	void DelReconnectInfo(ObjID_t roleId);

	/**
	*@brief 添加重连信息
	*/
	void AddReconnectInfo(ObjID_t roleId, ObjID_t raceId, UInt32 timeoutTime);

	/**
	*@brief 获得观战负载
	*/
	UInt32 GetObserveLoad();

public:
	/**
	*@brief 物理发包间隔
	*/
	UInt32 GetPhysicalFPS() const { return m_physicalFPS; }

	/**
	*@brief 逻辑发包间隔
	*/
	UInt32 GetLogicalFPS() const { return m_logicalFPS; }

public:
	// 比赛统计
	void IncCreateRaceNum(RaceInstanceType type);
	void IncStartRaceNum(RaceInstanceType type);
	void IncUnsyncRaceNum(RaceInstanceType type);

private:
    HashMap<ObjID_t, Race*>         m_races;
	HashMap<ObjID_t, ReconnectInfo>	m_reconnInfo;

	// 系统参数
	UInt32							m_physicalFPS;
	UInt32							m_logicalFPS;

	// 汇报战斗信息定时器
	Avalon::SimpleTimer				m_reportTimer;

	// 比赛统计
	RaceStatistic					m_raceStatistic[RACE_NUM];
};

#endif