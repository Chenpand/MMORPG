#ifndef _TIMER_SCRIPT_H_
#define _TIMER_SCRIPT_H_

#include <CLDefine.h>
#include <AvalonScriptFile.h>
#include <set>

class TimerScript;
class WSPlayer;

class ScriptTimerHandler
{
	friend class TimerScript;
public:
	ScriptTimerHandler(UInt64 timeout, UInt64 interval, const std::string& func):m_Timeout(timeout), m_Interval(interval), m_Func(func){}
	~ScriptTimerHandler(){}

	void OnTimeout(TimerScript* script, const Avalon::Time& now);

	UInt64 GetTimeout() const { return m_Timeout; }

private:
	//超时时间
	UInt64		m_Timeout;
	//超时间隔
	UInt64		m_Interval;
	//回调函数
	std::string	m_Func;
};

class ScriptTimerLess : public std::less<ScriptTimerHandler*>
{
public:
	bool operator()(const ScriptTimerHandler& left,const ScriptTimerHandler& right) const
	{
		return left.GetTimeout() < right.GetTimeout();
	}
};

/**
 *@brief 定时器脚本
 */
class TimerScript : public Avalon::ScriptFile
{
	typedef std::multiset<ScriptTimerHandler,ScriptTimerLess> HANDLER_SET;
public:
	TimerScript(Avalon::ScriptState* state);
	~TimerScript();

public:
	/**
	 *@brief 初始化
	 */
	void Init();

	/**
	 *@brief 注册定时事件
	 */
	void RegisterTimerEvent(UInt32 time, UInt32 interval, const char* func);
	void RegisterHourEvent(UInt32 hourtime, const char* func);
	void RegisterDailyEvent(UInt32 daytime, const char* func);
	void RegisterWeeklyEvent(UInt32 weekday, UInt32 daytime, const char* func);

	/**
	*@brief 设置、获得新版本开服排行榜活动的时间
	*/
	void SetGSNewVerTime(Int32 year, Int32 month, Int32 day);
	UInt32 GetGSNewVerTime() const { return m_uGSNewVerTime;}

public:
	/**
	 *@brief 定时处理
	 */
	void OnTick(const Avalon::Time& now);

public://事件
	/**
	 *@brief 服务器查询计数数据返回
	 */
	void OnSelectCounterReturn(const char* key, ObjID_t playerId, const char* counter, UInt32 value);

	/**
	*@brief 活动开始
	*/
	void OnActivityStart(const char *name);

	/**
	 *@brief 活动结束
	 */
	void OnActivityEnd(const char* name);

	/**
	*@brief 活动准备
	*/
	void OnActivityPrepare(const char* name);

	/**
	*@brief 访问排行榜
	*/
	void OnVisitSortlist(const char *func, ObjID_t uId, ObjID_t uOwner, const char *szOwnerName, UInt32 ranking);

	/**
	 *@brief 访问全服排行榜
	 */
	void OnVisitWorldSortlist(const char* func, ObjID_t id, UInt32 ranking, UInt32 value1, UInt32 value2, UInt32 value3);

	/**
	*@brief 注册老兵回归活动
	*/
	void RegVerteransReturnActivity(WSPlayer* player);

private:
	//定时事件列表
	HANDLER_SET		m_Handlers;
	//当前时间
	Avalon::Time	m_Now;
	// 使用新版本排行榜活动的
	UInt32				m_uGSNewVerTime;
};

#endif
