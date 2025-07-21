#ifndef __PERFORMANCE_MGR_H__
#define __PERFORMANCE_MGR_H__

#include <AvalonDefine.h>
#include <AvalonTime.h>
#include <AvalonSingleton.h>
#include <CLDefine.h>

struct PerformanceRecord
{
    UInt64      session;
    UInt64      usedTime;
};

class PerformanceMgr : public Avalon::Singleton<PerformanceMgr>
{
public:
    PerformanceMgr();
    ~PerformanceMgr();

    void AddPerf(const char* name, UInt64 usedTime);
    void Reset();
    void ShowPerf();

private:
    void GenSession();

private:
    HashMap<const char*, PerformanceRecord> perfRecords_;
    UInt64 session_;
};

#define PLAYERSYNCSELFDATA "SyncSelfData"
#define PLAYERSYNCSELFDATAHASADDR "SyncSelfData/hasaddr"
#define PLAYERNUM "PlayerNum"
#define PLAYER_TICK "PlayerTick"
#define SCENE_TICK "SceneTick"

class PerformanceCountMgr
{
public:
	PerformanceCountMgr() = delete;
	PerformanceCountMgr(std::string moduleName);
	~PerformanceCountMgr();
	static void CleanCount(std::string moduleName);
	static void ShowCount(std::string moduleName);
	static void SetCount(std::string moduleName, UInt64 count);
	static UInt64 GetCount(std::string moduleName);
private:
	static HashMap<std::string, UInt64> m_CountMap;
	std::string m_ModuleName;
};

#endif