#ifndef _ACTIVITY_MGR_H_
#define _ACTIVITY_MGR_H_

#include <functional>
#include <AvalonSingleton.h>
#include <AvalonObjectPool.h>
#include <AvalonSimpleTimer.h>

#include <CLGameDefine.h>
#include <CLGlobalActivityServerDefine.h>


// 需要同步活动的游戏区信息
struct NeedSyncActivityGameZoneInfo
{
	NeedSyncActivityGameZoneInfo()
	{
		zoneId = 0;
		needSync = false;
	}

	UInt32 zoneId;
	bool   needSync;
};

class GASGameZone;

class ActivityMgr : public Avalon::Singleton<ActivityMgr>
{
public:
	ActivityMgr();
	~ActivityMgr();

public:
	bool OnInit();
	void Final();

	/**
	*@brief tick
	*/
	void OnTick(const Avalon::Time& now);

	/**
	*@brief 重载表格
	*/
	void ReloadTable();

	/**
	*@brief 添加一项运营活动
	*/
	void AddOpActivityInfo(OpActivityInfo* opAct);

	/**
	*@brief 查找一项运营活动
	*/
	OpActivityInfo* FindOpActivityInfo(UInt32 actId);

	/**
	*@brief 访问运营活动
	*/
	void VisitOpActivities(const std::function<bool(OpActivityInfo*)>& F);

public:
	/**
	*@brief 向gm请求活动
	*/
	void RequestGmOpActivity();

	/**
	*@brief gm设置运营活动
	*/
	void GmSetOpActivity(const OpActivityInfo& opAct);

	/**
	*@brief gm关闭运营活动
	*/
	bool GmCloseOpActivity(UInt32 id);

public:
	void OnConnected(UInt32 id);

	/**
	*@brief 重置需要同步活动的游戏区信息
	*/
	void _ResetNeedSyncActGameZoneInfo();

	/**
	*@brief 广播单个活动到所有区
	*/
	void _BroadcastActivity(const OpActivityInfo* opAct);

	/**
	*@brief 广播所有活动到所有区
	*/
	void _BroadcastActivities();

	/**
	*@brief 广播所有活动到指定区
	*/
	void _BroadcastActivities(GASGameZone* zone);

private:
	/**
	*@brief 加载表格
	*/
	bool _LoadTable(bool isReload = false);

	/**
	*@brief 加载表格
	*/
	bool _LoadTaskTable();

	/**
	*@brief 活动状态改变
	*/
	void _OnActivityStateChange(OpActivityInfo* opAct, ActivityState status);

	/**
	*@brief 活动准备
	*/
	void _OnActivityPrepare(const OpActivityInfo* opAct);

	/**
	*@brief 活动开启
	*/
	void _OnActivityStart(const OpActivityInfo* opAct);

	/**
	*@brief 活动关闭
	*/
	void _OnActivityEnd(const OpActivityInfo* opAct);

private:	
	// 运营活动
	OpActivityInfoVec m_OpActivityInfoVec;
	// 加载时临时活动
	OpActivityInfoVec m_TempOpActivityInfoVec;

	// 需要同步活动的游戏区
	std::vector<NeedSyncActivityGameZoneInfo*> m_NeedSyncActGameZones;

	// 每秒定时器
	Avalon::SimpleTimer m_PerSecTimer;
};

#endif
