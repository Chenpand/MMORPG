#ifndef _SYS_INTERFACE_H_
#define _SYS_INTERFACE_H_

#include <CLDefine.h>
#include <AvalonRandom.h>

class Player;
class RewardGroup;
class Scene;
class NpcDialog;
class Npc;
class Creature;

class SysInterface
{
public:
	void LogError(const char* msg){ SYSLOG_ERROR(msg); }
	void LogInfo(const char* msg){ SYSLOG_INFO(msg); }
	void LogDebug(const char* msg){ SYSLOG_DEBUG(msg); }
	void LogTrace(const char* msg){ SYSLOG_TRACE(msg); }
	void LogFatal(const char* msg){ SYSLOG_FATAL(msg); }

	UInt64 Now(){ return CURRENT_TIME.MSec(); }
	bool IsSameDay(UInt32 time1, UInt32 time2);

	void RegCounter(const char* name, Int32 cycle, UInt32 param = 0);

	Int32 Year(){ return CURRENT_DATE.Year(); }
	Int32 Month(){ return CURRENT_DATE.Month(); }
	Int32 Day(){ return CURRENT_DATE.Day(); }
	Int32 Hour(){ return CURRENT_DATE.Hour(); }
	Int32 Min(){ return CURRENT_DATE.Min(); }
	Int32 Sec(){ return CURRENT_DATE.Sec(); }
	Int32 WeekDay(){ return CURRENT_DATE.WDay(); }

	bool SelectInHundred(Int32 value){ return Avalon::Random::SelectInHundred(value); }
	bool SelectInThousand(Int32 value){ return Avalon::Random::SelectInThousand(value); }
	bool SelectInTenThousand(Int32 value){ return Avalon::Random::SelectInTenThousand(value); }
	bool SelectInHundredThousand(Int32 value){ return Avalon::Random::SelectInHundredThousand(value); }

	Int32 RandBetween(Int32 min, Int32 max){ return Avalon::Random::RandBetween(min, max); }

	void Notify(ObjID_t id, UInt16 type, UInt8 color, const char* word);
	void BroadcastNotify(UInt16 type, UInt8 color, const char* word);

	/**
	 *@brief 排行榜值变化
	 */
	void OnSortValueChanged(UInt8 sortType, ObjID_t id, UInt16 level, UInt32 value1, UInt32 value2, UInt32 value3);
	void OnWorldSortValueChanged(Player* player, UInt8 sortType, UInt32 value1, UInt32 value2, UInt32 value3);
	
	RewardGroup* GetRewardGroup();
	void SendSysMail(ObjID_t tReceiverId, const char* senderName, const char* title, const char* content, const char* reason, RewardGroup* rewards);

	/**
	 *@brief 判断是否处于活动中
	 */
	bool IsInActivity(const char* name);
	
	/**
	 *@brief 获取地图名
	 */
	const char* GetMapName(UInt32 mapId);

	/**
	 *@brief 获取随机方向
	 */
	UInt8 GetRandDirection();

	/**
	 *@brief 获取基本经验 
	 */
	UInt32 GetBaseExp(UInt16 level);

	/**
	 *@brief 获取指定等级基础银贝
	 */
	UInt32 GetBaseSilver(UInt16 level);

	/**
	*@brief 获得世界等级
	*/
	UInt32 GetWorldLevel();


	void BroadcastEffectEvent(UInt8 type);


	UInt32 GetZoneId();

	void SendOfflineNotify(const char* type, const char* word);

	/**
	*@brief 同步计数值给玩家
	*/
	void SyncCounter(Player *pPlayer, const char* szCounterName);

	UInt32 GetNeutralZoneId() const;

	/**
	 *@brief 获取中立区随机主城                                                                     
	 */
	UInt32 GetNeutralRandMajorCity() const;

public:
	// 城镇怪物相关

	/**
	*@brief 活动城镇刷怪
	*/
	void GenActivityCityMonster();

	/**
	*@brief 活动城镇刷怪
	*/
	void RemoveTaskCityMonster(ObjID_t owner, const char* idstr);

	//获取精力兑换经验
	UInt32 GetEnergyExchangeExp(UInt32 playerLev);
};

extern SysInterface sys;

#endif
