/**
 *@author	 
 *@email	 
 *@date		2011-02-26		
 *@brief	简单定时器
 */
#ifndef _AVALON_SIMPLE_TIMER_H_
#define _AVALON_SIMPLE_TIMER_H_

#include "AvalonTime.h"

namespace Avalon
{

	/**
	 *@brief 定时器
	 */
	class SimpleTimer
	{
	public:
		SimpleTimer():m_Interval(0),m_LastTime(0){}
		~SimpleTimer(){}

		/**
		 *@brief 设置获取定时间隔
		 *@param interval 定时间隔，毫秒
		 */
		void SetInterval(UInt64 interval){ m_Interval = interval; }
		UInt64 GetInterval(){ return m_Interval; }

		/**
		 *@brief 设置上次触发时间
		 */
		void SetLastTime(UInt64 time){ m_LastTime = time; }
		UInt64 GetLastTime() const{ return m_LastTime; }

		/**
		 *@brief 重置定时器
		 */
		void Reset()
		{
			m_Interval = 0;
			m_LastTime = 0;
		}

		/**
		 *@brief 判断时间是否到
		 */
		bool IsTimeout(const Avalon::Time& now)
		{
			if(m_LastTime == 0)
			{
				m_LastTime = now.MSec();
				return false;
			}
			else if(now.MSec() > m_LastTime + m_Interval)
			{
				m_LastTime = now.MSec();
				return true;
			}
			return false;
		}

		/**
		 *@brief 获取流失时间
		 */
		UInt64 GetEscapeTime(const Avalon::Time& now) const
		{
			if(m_LastTime == 0 || now.MSec() < m_LastTime)
			{
				return 0;
			}
			return now.MSec() - m_LastTime;
		}

	private:
		//定时间隔
		UInt64 m_Interval;
		//超时时间
		UInt64 m_LastTime;
	};
};

#endif
