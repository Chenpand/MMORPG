/**
 *@author	 
 *@email	 
 *@date		2011-03-12		
 *@brief	定时器
 */
#ifndef _AVALON_TIMER_H_
#define _AVALON_TIMER_H_

#include "AvalonTime.h"
#include "AvalonSingleton.h"
#include <set>

namespace Avalon
{
	enum TimerType
	{
		TIMER_ONCE,		//一次触发
		TIMER_REPEAT,	//多次
	};

	/**
	 *@brief 定时处理器
	 */
	class TimerHandler
	{
		friend class TimerReactor;
	public:
		TimerHandler():m_Type(TIMER_ONCE),m_Timeout(0),m_Interval(0){}
		virtual ~TimerHandler(){}

		/**
		 *@brief 超时事件
		 */
		virtual void OnTimeout(const Avalon::Time& now) = 0;

		/**
		 *@brief 销毁
		 */
		virtual void Release(){ delete this; }

		/**
		 *@brief 获取超时时间
		 */
		UInt64 GetTimeout() const { return m_Timeout; }

	private:
		//类型
		TimerType	m_Type;
		//超时时间
		UInt64		m_Timeout;
		//超时间隔
		UInt64		m_Interval;
	};

	class TimerLess : public std::less<TimerHandler*>
	{
	public:
		bool operator()(const TimerHandler* left,const TimerHandler* right) const
		{
			return left->GetTimeout() < right->GetTimeout();
		}
	};

	/**
	 *@brief 定时反应堆
	 */
	class TimerReactor : public Avalon::Singleton<TimerReactor>
	{
		typedef std::multiset<TimerHandler*,TimerLess> HANDLER_SET;
	public:
		TimerReactor();
		~TimerReactor();

	public:
		/**
		 *@brief 调度一次
		 */
		void ScheduleOnce(TimerHandler* handler,UInt64 interval);

		/**
		 *@brief 调度一次
		 */
		void ScheduleAt(TimerHandler* handler,const Avalon::Time& time);

		/**
		 *@brief 调度多次
		 */
		void ScheduleAt(TimerHandler* handler,const Avalon::Time& time,UInt64 interval);

		/**
		 *@brief 调度多次
		 */
		void Schedule(TimerHandler* handler,UInt64 interval);

		/**
		 *@brief 移除
		 */
		void RemoveHandler(TimerHandler* handler);

		/**
		 *@brief 处理事件
		 */
		void HandleEvent(const Avalon::Time& now);

	private:
		//定时事件列表
		HANDLER_SET		m_Handlers;
		//当前时间
		Avalon::Time	m_Now;
	};
}

#endif
