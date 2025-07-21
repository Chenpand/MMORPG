#ifndef _SYS_INTERFACE_H_
#define _SYS_INTERFACE_H_

#include <CLDefine.h>
#include <AvalonRandom.h>
#include <CLItemDefine.h>

class WSPlayer;
class WSScene;

class SysInterface
{
public:
	void LogError(const char* msg){ SYSLOG_ERROR(msg); }
	void LogInfo(const char* msg){ SYSLOG_INFO(msg); }
	void LogDebug(const char* msg){ SYSLOG_DEBUG(msg); }
	void LogTrace(const char* msg){ SYSLOG_TRACE(msg); }
	void LogFatal(const char* msg){ SYSLOG_FATAL(msg); }

	UInt64 Now(){ return CURRENT_TIME.MSec(); }

	void RegCounter(const char* name, Int32 cycle, UInt32 param = 0);

	Int32 Year(){ return CURRENT_DATE.Year(); }
	Int32 Month(){ return CURRENT_DATE.Month(); }
	Int32 Day(){ return CURRENT_DATE.Day(); }
	Int32 Hour(){ return CURRENT_DATE.Hour(); }
	Int32 Min(){ return CURRENT_DATE.Min(); }
	Int32 Sec(){ return CURRENT_DATE.Sec(); }
	Int32 WeekDay(){ return CURRENT_DATE.WDay(); }

	Int32 RandBetween(Int32 min, Int32 max){ return Avalon::Random::RandBetween(min, max); }

	UInt32 TimeOfDay();

	UInt32 MakeTime(Int32 year, Int32 month, Int32 day, Int32 hour, Int32 min, Int32 sec);

	/**
	 *@brief 查找一个场景
	 */
	WSScene* FindScene(UInt32 id);
	
	/**
	 *@brief 选择一个创建动态场景的服务器
	 */
	UInt32 SelectDynSceneServer();

	/**
	*@brief 创建公共副本
	*/
	void CreatePublicDynScene(UInt32 uMapId, UInt16 uLevel, UInt64 uParam, UInt32 uLastTime);

	/**
	 *@brief 广播公告
	 */
	void BroadcastNotify(UInt16 type, UInt8 color, const char* word);

	/**
	 *@brief 按等级广播公告
	 */
	void BroadcastNotifyByLvl(UInt16 level, UInt16 type, UInt8 color, const char* word);

	/**
	@brief 系统邮件相关
	*/
	std::vector<ItemReward>* GetRewardGroup();
	void AddReward(std::vector<ItemReward> *pGroup, UInt32 uId, UInt32 uNum, UInt8 qualityLv, UInt8 strength);
	void SendSysMail(ObjID_t tReceiverId, const char* senderName, const char* title, const char* content, const char* reason, std::vector<ItemReward>* rewards);
	void BroadcastSysMailOnline(const char* senderName, const char* title, const char* content, const char* reason, std::vector<ItemReward>* rewards);

	/**
	 *@brief 活动相关
	 */
	void ClearActivityRegInfos();
	void RegisterDailyActivity(UInt32 id, const char *name, UInt16 level, const char *startTime, UInt32 preTime, UInt32 lastTime);
	void RegisterOnceActivity(UInt32 id, const char *name, UInt16 level, const char *startTime, UInt32 preTime, UInt32 lastTime);
	void RegisterOpenServerOnceActivity(UInt32 id, const char *name, UInt16 level, const char *startTime, UInt32 lastTime, UInt32 openServerTime);
	void RegisterWeekActivity(UInt32 id, const char *name, UInt16 level, const char *startTime, UInt32 preTime, UInt32 lastTime);
	void RegisterMonthActivity(UInt32 id, const char *name, UInt16 level);
	void RegisterCustomCircleActivity(UInt32 id, const char *name, UInt16 level, const char *startTime, UInt32 preTime, const UInt32 lastTime);
	void RegisterConsistActivity(UInt32 id, const char *name, UInt16 level);

	void RegisterGSActivity(UInt32 id, const char *name, UInt16 level, UInt32 lastTime);
	void RegisterGSActivityEndTime(UInt32 id, const char *name, UInt16 level, const char* endTime);
	void RegisterGSActivityIntervalTime(UInt32 id, const char *name, UInt16 level, const char* fromTime, const char* toTime);
	void RegisterGSActivityIntervalDay(UInt32 id, const char *name, UInt16 level, UInt16 fromDay, UInt16 toDay);
	
	void SetActGameStartLimit(UInt32 id, const char *gamestartTime, UInt8 gamestartFlag);
	void CloseActivity(const char* name);
	bool IsInActivity(const char *name);
	void AddGlobalActivity(const char *name);

	void SendAnnouncementNow(UInt32 id, UInt32 interval, UInt32 times);
	void SendAnnouncement(UInt32 id, const char* begintime, UInt32 interval, UInt32 times);
	void PublicAnnouncementList(UInt32 id, UInt32 durationTime, UInt32 intervalTime, UInt32 startTimeOffset = 0);
	void RemoveAnnouncement(UInt32 dataId);

	void SetActivityMonsterOpen(UInt8 open);

	/**
	*@brief 清空某个排行榜
	*/
	void ClearSortList(UInt8 type);

	/**
	*@brief 新年活动红包排行榜
	*/
	void OnNewYearRedPacketEnd();

	/**
	 *@brief 查询计数值
	 */
	void SelectCounterValue(const char* key, const char* counter, UInt32 minValue);

	/**
	*@brief 访问排行榜
	*/
	void VisitSortList(UInt8 sortType, UInt32 uNum, const char *func);

	void VisitPlayer(const char* fun);

	/**
	*@brief 注册NPC
	*/
	void RegisterNpc(UInt32 npcId, UInt32 mapId, UInt16 x, UInt16 y);

	/**
	*@brief GameParam参数设置
	*/
	UInt32 GetGameValue(const char* name);
	void SetGameValue(const char* name, UInt32 value);

	/**
	*@brief 同步活动全服计数器
	*/
	void SyncActivityCounter(UInt8 type, UInt32 value);

	void BroadcastEffectEvent(UInt8 type);

	/**
	*@brief 获得区ID
	*/
	UInt32 GetZoneId();
	
	UInt32 GetNeutralZoneId() const;

	/**
	*@brief 获取平台
	*/
	const char* GetPlatform() const;

	/**
	*@brief 功能开放
	*/
	void OpenService(UInt32 type);
	void CloseService(UInt32 type);

	/**
	*@brief 设置自动战斗模式
	*/
	void SetAutoBattleMode(UInt8 mode);

	/**
	*@brief 公会战
	*/
	void ChangeGuildBattleStatus(UInt32 targetStatus);

	//商品上架
	void OnSaleMallWaitGoods(UInt32 goodId);
	
	//商品下架
	void OffSaleMallWaitGoods(UInt32 goodId);

	// 召唤城镇怪物
	void SummonCityMonster();

	// 开启吃鸡
	void ChijiOpen(const char* name);

	// 金币寄售结束
	void GoldConsignmentClose();

private:

};

extern SysInterface sys;

#endif
