/**
 *@author	 
 *@email	 
 *@date		2011-02-26		
 *@brief	时间
 */
#ifndef _AVALON_TIME_H_
#define _AVALON_TIME_H_

#include "AvalonDefine.h"
#include <string>

namespace Avalon
{
	/**
	 *@brief 时间类，精确到毫秒
	 */
	class Time
	{
	public:
        //每秒毫秒数
        const static UInt32 MSECS_OF_SEC = 1000;
		//每分秒数
		const static UInt32 SECS_OF_MIN = 60;
		//每小时秒数
		const static UInt32 SECS_OF_HOUR = (60 * SECS_OF_MIN);
		//每日秒数
		const static UInt32 SECS_OF_DAY = (24 * SECS_OF_HOUR);
		//每日毫秒数
		const static UInt32 MSECS_OF_DAY = (SECS_OF_DAY * MSECS_OF_SEC);

	public:
		Time():m_MilliSecond(0){}
		Time(UInt64 msec):m_MilliSecond(msec){}
		Time(const Time& time):m_MilliSecond(time.m_MilliSecond){}
		~Time(){}

	public:
		/**
		 *@brief 获取当前时间
		 */
		static Time CurrentTime();

		/**
		 *@brief 获取毫秒数
		 */
		UInt64 MSec() const{ return m_MilliSecond; }

		/**
		 *@brief 获取秒数
		 */
		UInt64 Sec() const { return m_MilliSecond / 1000; }

		//逻辑操作符重载
		bool operator<(const Time& time) const{ return m_MilliSecond < time.m_MilliSecond; }
		bool operator>(const Time& time) const{ return m_MilliSecond > time.m_MilliSecond; }
		bool operator>=(const Time& time) const{ return !operator<(time); }
		bool operator<=(const Time& time) const{ return !operator>(time); }

		bool operator==(const Time& time) const { return m_MilliSecond == time.m_MilliSecond; }
		bool operator!=(const Time& time) const{ return !operator==(time); }

		//算术操作符重载
		friend const Time operator+ (const Time&,const Time&);
		friend const Time operator- (const Time&,const Time&);
		
		const Time& operator+= (const Time& time)
		{
			m_MilliSecond += time.m_MilliSecond; 
			return *this;
		}

		const Time& operator-= (const Time& time)
		{
			m_MilliSecond -= time.m_MilliSecond;
			return *this;
		}

	private:
		//1970年1月1日0时到现在的毫秒数
		UInt64	m_MilliSecond;
	};


	inline const Time operator+ (const Time& time1,const Time& time2)
	{
		return time1.m_MilliSecond + time2.m_MilliSecond;
	}

	inline const Time operator- (const Time& time1,const Time& time2)
	{
		return time1.m_MilliSecond - time2.m_MilliSecond;
	}
}

#endif
