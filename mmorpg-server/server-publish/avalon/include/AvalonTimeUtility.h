#ifndef __AVALON_TIME_UTILITY_H__
#define __AVALON_TIME_UTILITY_H__

#include "AvalonTime.h"
#include "AvalonDate.h"

namespace Avalon
{
#define AVALON_STRINGVAL_LENGTH 64

	/**
	*@brief 时间相关帮助函数
	*/
	class TimeUtil
	{
	public:

		/**
		*@brief 计算两个时间之间相隔的天数，同一天内不算一天
				startTime,endTime为时间戳
				dayStartHour代表一天是几点开始的，比如说每天6点为一天，就传6
		*/
		static UInt32 CountIntervalDays(UInt32 startTime, UInt32 endTime, UInt32 dayStartHour)
		{
			if (startTime >= endTime)
			{
				return 0;
			}

			Avalon::Date startDate(Time((UInt64)startTime * Time::MSECS_OF_SEC));
			if (startDate.Hour() < (Int32)dayStartHour)
			{
				startDate = Avalon::Date(Avalon::Time(((UInt64)startTime - Time::SECS_OF_DAY) * Time::MSECS_OF_SEC));
				startDate.Hour(dayStartHour);
				startDate.Min(0);
				startDate.Sec(0);
				startDate.MSec(0);
			}
			else
			{
				startDate.Hour(dayStartHour);
				startDate.Min(0);
				startDate.Sec(0);
				startDate.MSec(0);
			}

			UInt32 startSec = (UInt32)startDate.ToTime().Sec();
			if (endTime <= startSec)
			{
				return 0;
			}

			UInt32 days = (endTime - startSec) / Time::SECS_OF_DAY;
			return days;
		}

		/**
		*@brief 判断是否同一天
		*/
		static bool IsSameDay(const Avalon::Time& time1, const Avalon::Time& time2)
		{
			Avalon::Date date1(time1);
			Avalon::Date date2(time2);

			if (date1.Day() != date2.Day() || date1.Month() != date2.Month() || date1.Year() != date2.Year())
				return false;

			return true;
		}

		static bool IsSameDay(UInt32 time1, UInt32 time2)
		{
			return IsSameDay(Avalon::Time(UInt64(time1) * 1000), Avalon::Time(UInt64(time2) * 1000));
		}

		/**
		 *@brief 获取今天开始的时间戳
		 */
		static UInt64 GetDayBeginTime(const Avalon::Time& time)
		{
			Avalon::Date date(time);
			date.Hour(0);
			date.Min(0);
			date.Sec(0);
			date.MSec(0);
			return date.ToTime().MSec();
		}

		/**
		*@brief 获取今天开始的时间戳
		*/
		static UInt64 GetDayBeginTime2(const Avalon::Time& time)
		{
			Avalon::Date nowDate(time);
			Avalon::Date dayBeginDate = nowDate;
			dayBeginDate.Hour(6);
			dayBeginDate.Min(0);
			dayBeginDate.Sec(0);
			dayBeginDate.MSec(0);
			auto dayBeginTime = dayBeginDate.ToTime().MSec();

			if (nowDate.Hour() < 6)
			{
				dayBeginTime -= Avalon::Time::SECS_OF_DAY * Avalon::Time::MSECS_OF_SEC;
			}

			return dayBeginTime;
		}

		/**
		*@brief 比较两个时间戳是否在同一逻辑天
		*/
		static bool CheckInSameLogicDay(const UInt32& time1, const UInt32& time2)
		{
			Avalon::Time t1((UInt64)time1 * Avalon::Time::MSECS_OF_SEC);
			Avalon::Time t2((UInt64)time2 * Avalon::Time::MSECS_OF_SEC);

			return GetDayBeginTime2(t1) == GetDayBeginTime2(t2);
		}

		/*
		*	获得给定时间所在周的开始时间
		* \param	time		时间
		* \param	beginWDay	每周的启示时间从周几开始 0:周日 1:周一 .... 6:周六
		* \return	UInt64		返回周开始的时间
		* 以周日为起点,返回值是 周日的 0点0分0秒
		* 以周一为起点,返回值是 周一的 0点0分0秒
		*/
		static UInt64 getWeekBeginTime(const Avalon::Time& time, int beginWDay = 1, int beginDayHour = 0)
		{
			Avalon::Date date(time);
			date.Hour(beginDayHour);
			date.Min(0);
			date.Sec(0);
			date.MSec(0);

			UInt64 t_time = date.ToTime().Sec();

			if (date.WDay() < beginWDay)
				t_time -= (7 - beginWDay + date.WDay()) * Avalon::Time::SECS_OF_DAY;
			else if (date.WDay() > beginWDay)
				t_time -= (date.WDay() - beginWDay) * Avalon::Time::SECS_OF_DAY;
			else
			{
				if (time.Sec() >= t_time)
				{
					return t_time;
				}
				else
				{
					t_time -= (7 - beginWDay + date.WDay()) * Avalon::Time::SECS_OF_DAY;
				}
			}
			return t_time;
		}

		/*
		*	获得给定时间所在周的开始时间
		* \param	time		时间戳(毫秒)
		* \param	beginWDay	每周的启示时间从周几开始 0:周日 1:周一 .... 6:周六
		* \return	UInt64		返回周开始的时间
		* 以周日为起点,返回值是 周日的 0点0分0秒
		* 以周一为起点,返回值是 周一的 0点0分0秒
		*/
		static UInt64 getWeekBeginTime(UInt64 time, int beginWDay = 1, int beginDayHour = 0)
		{
			Avalon::Time curTime(time);
			return getWeekBeginTime(curTime, beginWDay, beginDayHour);
		}

		/**
		*@brief 是否在同一周
		*/
		static bool isSameWeek(const Avalon::Time& time1, const Avalon::Time& time2, int beginWDay = 1, int beginDayHour = 0)
		{
			return getWeekBeginTime(time1, beginWDay) == getWeekBeginTime(time2, beginWDay);
		}

		/**
		*@brief 是否在同一周
		*/
		static bool isSameWeek(UInt64 time1, UInt64 time2, int beginWDay = 1, int beginDayHour = 0)
		{
			return getWeekBeginTime(time1, beginWDay, beginDayHour) == getWeekBeginTime(time2, beginWDay, beginDayHour);
		}

		/*
		*	获得给定时间所在月的开始时间
		* \param	time		时间
		* \return	UInt64		返回月开始的时间
		*/
		static UInt64 getMonthBeginTime(const Avalon::Time& time)
		{
			Avalon::Date date(time);
			date.Day(1);
			date.Hour(0);
			date.Min(0);
			date.Sec(0);
			date.MSec(0);

			return date.ToTime().Sec();
		}

		static UInt64 getMonthLogicBeginTime(const Avalon::Time& time)
		{
			Avalon::Date date(time);
			date.Day(1);
			date.Hour(6);
			date.Min(0);
			date.Sec(0);
			date.MSec(0);

			return date.ToTime().Sec();
		}

		static UInt64 getMonthLogicBeginTime(UInt64 time)
		{
			Avalon::Time curTime(time);
			return getMonthLogicBeginTime(curTime);
		}

		static UInt64 getNextMonthLogicBeginTime(UInt64 curTime)
		{
			Avalon::Date date(curTime);
			date.Day(1);
			date.Hour(6);
			date.Min(0);
			date.Sec(0);
			date.MSec(0);
			if (date.Month() == 12)
			{
				date.Year(date.Year() + 1);
				date.Month(1);
			}
			else
			{
				date.Month(date.Month() + 1);
			}
			return date.ToTime().Sec();
		}

		/*
		*	获得给定时间所在月的开始时间
		* \param	time		时间
		* \return	UInt64		返回月开始的时间
		*/
		static UInt64 getMonthBeginTime(UInt64 time)
		{
			Avalon::Time curTime(time);
			return getMonthBeginTime(curTime);
		}

		/**
		*@brief 是否在同一月
		*/
		static bool isSameMonth(const Avalon::Time& time1, const Avalon::Time& time2)
		{
			return getMonthBeginTime(time1) == getMonthBeginTime(time2);
		}

		/**
		*@brief 是否在同一月
		*/
		static bool isSameMonth(UInt64 time1, UInt64 time2)
		{
			return getMonthBeginTime(time1) == getMonthBeginTime(time2);
		}

		/**
		*@brief 获取逻辑天，用时间减去6小时来计算，参数单位为毫秒
		*/
		static Avalon::Date GetLogicDate(UInt64 time)
		{
			Avalon::Time tmpTime(time - (6 * Avalon::Time::SECS_OF_HOUR * Avalon::Time::MSECS_OF_SEC));
			return Avalon::Date(tmpTime);
		}
	};
};


#endif