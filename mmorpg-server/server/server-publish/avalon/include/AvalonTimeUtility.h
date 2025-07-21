#ifndef __AVALON_TIME_UTILITY_H__
#define __AVALON_TIME_UTILITY_H__

#include "AvalonTime.h"
#include "AvalonDate.h"

namespace Avalon
{
#define AVALON_STRINGVAL_LENGTH 64

	/**
	*@brief ʱ����ذ�������
	*/
	class TimeUtil
	{
	public:

		/**
		*@brief ��������ʱ��֮�������������ͬһ���ڲ���һ��
				startTime,endTimeΪʱ���
				dayStartHour����һ���Ǽ��㿪ʼ�ģ�����˵ÿ��6��Ϊһ�죬�ʹ�6
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
		*@brief �ж��Ƿ�ͬһ��
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
		 *@brief ��ȡ���쿪ʼ��ʱ���
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
		*@brief ��ȡ���쿪ʼ��ʱ���
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
		*@brief �Ƚ�����ʱ����Ƿ���ͬһ�߼���
		*/
		static bool CheckInSameLogicDay(const UInt32& time1, const UInt32& time2)
		{
			Avalon::Time t1((UInt64)time1 * Avalon::Time::MSECS_OF_SEC);
			Avalon::Time t2((UInt64)time2 * Avalon::Time::MSECS_OF_SEC);

			return GetDayBeginTime2(t1) == GetDayBeginTime2(t2);
		}

		/*
		*	��ø���ʱ�������ܵĿ�ʼʱ��
		* \param	time		ʱ��
		* \param	beginWDay	ÿ�ܵ���ʾʱ����ܼ���ʼ 0:���� 1:��һ .... 6:����
		* \return	UInt64		�����ܿ�ʼ��ʱ��
		* ������Ϊ���,����ֵ�� ���յ� 0��0��0��
		* ����һΪ���,����ֵ�� ��һ�� 0��0��0��
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
		*	��ø���ʱ�������ܵĿ�ʼʱ��
		* \param	time		ʱ���(����)
		* \param	beginWDay	ÿ�ܵ���ʾʱ����ܼ���ʼ 0:���� 1:��һ .... 6:����
		* \return	UInt64		�����ܿ�ʼ��ʱ��
		* ������Ϊ���,����ֵ�� ���յ� 0��0��0��
		* ����һΪ���,����ֵ�� ��һ�� 0��0��0��
		*/
		static UInt64 getWeekBeginTime(UInt64 time, int beginWDay = 1, int beginDayHour = 0)
		{
			Avalon::Time curTime(time);
			return getWeekBeginTime(curTime, beginWDay, beginDayHour);
		}

		/**
		*@brief �Ƿ���ͬһ��
		*/
		static bool isSameWeek(const Avalon::Time& time1, const Avalon::Time& time2, int beginWDay = 1, int beginDayHour = 0)
		{
			return getWeekBeginTime(time1, beginWDay) == getWeekBeginTime(time2, beginWDay);
		}

		/**
		*@brief �Ƿ���ͬһ��
		*/
		static bool isSameWeek(UInt64 time1, UInt64 time2, int beginWDay = 1, int beginDayHour = 0)
		{
			return getWeekBeginTime(time1, beginWDay, beginDayHour) == getWeekBeginTime(time2, beginWDay, beginDayHour);
		}

		/*
		*	��ø���ʱ�������µĿ�ʼʱ��
		* \param	time		ʱ��
		* \return	UInt64		�����¿�ʼ��ʱ��
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
		*	��ø���ʱ�������µĿ�ʼʱ��
		* \param	time		ʱ��
		* \return	UInt64		�����¿�ʼ��ʱ��
		*/
		static UInt64 getMonthBeginTime(UInt64 time)
		{
			Avalon::Time curTime(time);
			return getMonthBeginTime(curTime);
		}

		/**
		*@brief �Ƿ���ͬһ��
		*/
		static bool isSameMonth(const Avalon::Time& time1, const Avalon::Time& time2)
		{
			return getMonthBeginTime(time1) == getMonthBeginTime(time2);
		}

		/**
		*@brief �Ƿ���ͬһ��
		*/
		static bool isSameMonth(UInt64 time1, UInt64 time2)
		{
			return getMonthBeginTime(time1) == getMonthBeginTime(time2);
		}

		/**
		*@brief ��ȡ�߼��죬��ʱ���ȥ6Сʱ�����㣬������λΪ����
		*/
		static Avalon::Date GetLogicDate(UInt64 time)
		{
			Avalon::Time tmpTime(time - (6 * Avalon::Time::SECS_OF_HOUR * Avalon::Time::MSECS_OF_SEC));
			return Avalon::Date(tmpTime);
		}
	};
};


#endif