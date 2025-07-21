/**
 *@author	 
 *@email	 
 *@date		2011-02-25	
 *@brief	日历时间
 */
#ifndef _AVALON_DATE_H_
#define _AVALON_DATE_H_

#include "AvalonTime.h"
#include <ctime>

namespace Avalon
{
	/**
	 *@brief 日期类，定义了获取年、月、日等接口
	 */
	class Date
	{
	public:
		Date();
		explicit Date(const Time& time);
		~Date(){}

		Time ToTime() const;
		
		/**
		 *@brief 设置获取年份
		 */
		Int32 Year() const{ return m_Tm.tm_year + 1900;}
		void Year(Int32 year){ m_Tm.tm_year = year - 1900; }

		/**
		 *@brief 设置获取月份
		 */
		Int32 Month() const{ return m_Tm.tm_mon + 1; }
		void Month(Int32 month){ m_Tm.tm_mon = month - 1; }

		/**
		 *@brief 设置获取日期
		 */
		Int32 Day() const{ return m_Tm.tm_mday; }
		void Day(Int32 day){ m_Tm.tm_mday = day; }

		/**
		 *@brief 设置获取时
		 */
		Int32 Hour() const{ return m_Tm.tm_hour; }
		void Hour(Int32 hour){ m_Tm.tm_hour = hour; }

		/**
		 *@brief 设置获取分
		 */
		Int32 Min() const{ return m_Tm.tm_min; }
		void Min(Int32 min){ m_Tm.tm_min = min; }

		/**
		 *@brief 设置获取秒
		 */
		Int32 Sec() const{ return m_Tm.tm_sec; }
		void Sec(Int32 sec){ m_Tm.tm_sec = sec; }

		/**
		 *@brief 设置获取毫秒
		 */
		Int32 MSec() const{ return m_Msec; }
		void MSec(Int32 msec){ m_Msec = msec; }

		/**
		 *@brief 设置获取星期
		 */
		Int32 WDay() const { return m_Tm.tm_wday == 0 ? 7:m_Tm.tm_wday; }

		/**
		*@brief 获取本月的天数
		*/
		Int32 GetDaysInMonth()
		{
			Int32 d;
			Int32 day[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
			if (2 == Month())
			{
				d = ((((0 == Year() % 4) && (0 != Year() % 100)) || (0 == Year() % 400)) ? 29 : 28);
			}
			else
			{
				d = day[Month() - 1];
			}
			return d;
		}

	private:
		struct tm	m_Tm;
		//毫秒
		Int32		m_Msec;
	};
}

#endif
