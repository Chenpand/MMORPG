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
	};
};


#endif