#include "TimeUtil.h"
#if defined(AVALON_OS_WINDOWS)
#	include<windows.h>
#else
#	include<sys/time.h>
#endif

namespace TimeUtil
{
	/**
	*@brief 时间转换,标准格式-->时间戳(2018-4-8 21:42:39 => 1523194959)
	*/
	UInt32 StandardFormatConvertToTimestamp(const std::string& timeStr)
	{
		if (timeStr.empty()) return 0;

		Avalon::Date date;
		Int32 value = 0;
		char split = 0;

		std::istringstream iss(timeStr);
		iss >> value >> split;
		date.Year(value);

		iss >> value >> split;
		date.Month(value);

		iss >> value;
		date.Day(value);

		iss >> value >> split;
		date.Hour(value);

		iss >> value >> split;
		date.Min(value);

		iss >> value;
		date.Sec(value);

		return (UInt32)(date.ToTime().Sec());
	}

	/**
	*@brief 时间格式拼接(2018-4-8 21:42:39 => 2018-04-08 21:42:39)
	*/
	const std::string TimeFormatJoint(UInt32 threshold, UInt32 param)
	{
		std::string str = Avalon::StringUtil::ConvertToString<UInt32>(param);
		if (threshold < str.size()) return "";

		std::string result;
		for (UInt32 idx = 0; idx != threshold - str.size(); ++idx)
		{
			result.append("0");
		}
		result.append(str);

		return result;
	}

	/**
	*@brief 时间转换,标准格式-->时间戳(1523194959 => 2018-04-08 21:42:39))
	*/
	const std::string TimestampToStandardFormatCustomer(UInt32 timestamp)
	{
		if (timestamp == 0) return "";

		Avalon::Date date(Avalon::Time((UInt64)timestamp * SEC_TO_MSEC));

		std::ostringstream oss;
		oss << TimeFormatJoint(4, date.Year()) << "-" << TimeFormatJoint(2, date.Month()) << "-" << TimeFormatJoint(2, date.Day()) << " "
			<< TimeFormatJoint(2, date.Hour()) << ":" << TimeFormatJoint(2, date.Min()) << ":" << TimeFormatJoint(2, date.Sec());

		return oss.str();
	}

	/**
	*@brief 时间转换,标准格式-->时间戳(1523194959 => 2018-4-8 21:42:39)
	*/
	const std::string TimestampToStandardFormat(UInt32 timestamp)
	{
		if (timestamp == 0) return "";

		Avalon::Date date(Avalon::Time((UInt64)timestamp * SEC_TO_MSEC));

		std::ostringstream oss;
		oss << date.Year() << "-" << date.Month() << "-" << date.Day()
			<< " " << date.Hour() << ":" << date.Min() << ":" << date.Sec();

		return oss.str();
	}

	/**
	*@brief 时间转换,标准格式-->时间戳(1523194959 => 2018-4-8)
	*/
	const std::string TimestampToStandardFormatNoHour(UInt32 timestamp)
	{
		if (timestamp == 0) return "";

		Avalon::Date date(Avalon::Time((UInt64)timestamp * SEC_TO_MSEC));

		std::ostringstream oss;
		oss << date.Year() << "-" << date.Month() << "-" << date.Day();

		return oss.str();
	}

	/**
	*@brief 时间转换,标准格式-->时间戳(1523194959 => 2018-04-08<21:42:39>)
	*/
	const std::string TimestampToStandarFormatNoSpace(UInt32 timestamp)
	{
		if (timestamp == 0) return "";

		Avalon::Date date(Avalon::Time((UInt64)timestamp * SEC_TO_MSEC));

		std::ostringstream oss;
		oss << TimeFormatJoint(4, date.Year()) << "-" << TimeFormatJoint(2, date.Month()) << "-" << TimeFormatJoint(2, date.Day()) << "<"
			<< TimeFormatJoint(2, date.Hour()) << ":" << TimeFormatJoint(2, date.Min()) << ":" << TimeFormatJoint(2, date.Sec()) << ">";

		return oss.str();
	}

	/**
	*@brief 时间转换,适配表格新旧时间格式
	*/
	bool TimeStrConvertToTimestamp(UInt32& timestamp, const std::string& timeStr)
	{
		if (timeStr.empty())
		{
			timestamp = 0;
			return true;
		}

		std::vector<std::string> strVec;
		Avalon::StringUtil::Split(timeStr, strVec, "-");
		if (strVec.size() == 1)
		{
			timestamp = Avalon::StringUtil::ConvertToValue<UInt32>(strVec.front());

			if (timestamp != Avalon::StringUtil::ConvertToValue<UInt32>(timeStr))
			{
				return false;
			}
		}
		else if (strVec.size() == 3)
		{
			timestamp = StandardFormatConvertToTimestamp(timeStr);

			if (TimestampToStandardFormat(timestamp) != timeStr &&
				TimestampToStandardFormatCustomer(timestamp) != timeStr)
			{
				return false;
			}
		}
		else
		{
			return false;
		}

		return true;
	}

	/**
	*@brief 时间计算,根据起点和间隔计算终点
	*/
	Int32 CalculateEndPointByStartPointAndInterval(UInt32 satrtPoint, const std::string& interval, UInt32& endPoint)
	{
		if (interval.empty()) return -1;

		char split = 0;

		UInt32 day = 0;
		UInt32 hour = 0;
		UInt32 min = 0;
		UInt32 sec = 0;
		std::istringstream iss(interval);
		iss >> day >> split >> hour >> split >> min >> split >> sec;

		if (day == 0 || hour >= 24 || min >= 60 || sec >= 60)
		{
			return -1;
		}

		endPoint = satrtPoint + (day - 1) * DAY_TO_SEC + hour * HOUR_TO_SEC + min * MIN_TO_SEC + sec;

		return 0;
	}

	UInt64 GetMicroSecondNow()
	{
#if defined(AVALON_OS_WINDOWS)
		FILETIME fileTime;
		ULARGE_INTEGER li;
		const static UInt64 TIME_DIFF = 116444736000000000;
		;
		::GetSystemTimeAsFileTime(&fileTime);
		li.LowPart = fileTime.dwLowDateTime;
		li.HighPart = fileTime.dwHighDateTime;
		return ((UInt64)li.QuadPart - TIME_DIFF) / 10;
#else
		struct timeval tv;
		gettimeofday(&tv, NULL);
		return (UInt64)tv.tv_sec * 1000000 + (UInt64)tv.tv_usec;
#endif
	}

	UInt64 GetCurrentCustomeTimeStamp(UInt32 hour, UInt32 min, UInt32 sec, UInt32 msec)
	{
		Avalon::Date currentDate = CURRENT_DATE;
		currentDate.Hour(hour);
		currentDate.Min(min);
		currentDate.Sec(sec);
		currentDate.MSec(msec);

		return currentDate.ToTime().MSec();
	}

	bool StringConvertToRefreshDate(RefreshType refreshType, const std::string& refreshTimePoint, RefreshDate& refreshDate)
	{
		switch (refreshType)
		{
		case TimeUtil::REFRESH_TYPE_NONE:
			if (!refreshTimePoint.empty())
			{
				return false;
			}
			break;
		case TimeUtil::REFRESH_TYPE_PER_MONTH:
		{
			if (refreshTimePoint.empty())
			{
				return false;
			}

			std::vector<std::string> tmpVec;
			Avalon::StringUtil::Split(refreshTimePoint, tmpVec, "|");
			if (tmpVec.size() != 2)
			{
				return false;
			}

			refreshDate.day = Avalon::StringUtil::ConvertToValue<UInt32>(tmpVec[0]);
			refreshDate.hour = Avalon::StringUtil::ConvertToValue<UInt32>(tmpVec[1]);
			break;
		}
		case TimeUtil::REFRESH_TYPE_PER_WEEK:
		{
			if (refreshTimePoint.empty())
			{
				return false;
			}

			std::vector<std::string> tmpVec;
			Avalon::StringUtil::Split(refreshTimePoint, tmpVec, "|");
			if (tmpVec.size() != 2)
			{
				return false;
			}

			refreshDate.weekDay = Avalon::StringUtil::ConvertToValue<UInt32>(tmpVec[0]);
			refreshDate.hour = Avalon::StringUtil::ConvertToValue<UInt32>(tmpVec[1]);
			break;
		}
		case TimeUtil::REFRESH_TYPE_PER_DAY:
		{
			if (refreshTimePoint.empty())
			{
				return false;
			}

			std::vector<std::string> tmpVec;
			Avalon::StringUtil::Split(refreshTimePoint, tmpVec, "|");
			if (tmpVec.size() != 1)
			{
				return false;
			}

			refreshDate.hour = Avalon::StringUtil::ConvertToValue<UInt32>(tmpVec[0]);
			break;
		}
		case TimeUtil::REFRESH_TYPE_SEASON:
		{
			break;
		}
		default: return false;
		}

		if (!refreshDate.CheckValid(refreshType))
		{
			return false;
		}
		return true;
	}

	bool CalculateNextRefreshTimestamp(RefreshType refreshType, RefreshDate refreshDate, UInt64& nextRefreshTimestamp)
	{
		if (refreshType == TimeUtil::REFRESH_TYPE_NONE)
		{
			return false;
		}

		if (!refreshDate.CheckValid(refreshType))
		{
			return false;
		}

		Int32 nextRefreshRemainDays = 0;

		switch (refreshType)
		{
		case TimeUtil::REFRESH_TYPE_PER_MONTH:
			if (CURRENT_DATE.Day() == (Int32)refreshDate.day)
			{
				if (CURRENT_DATE.Hour() < (Int32)refreshDate.hour)
				{
					break;
				}
			}
			nextRefreshRemainDays = CURRENT_DATE.GetDaysInMonth() - CURRENT_DATE.Day() + (Int32)refreshDate.day;
			break;
		case TimeUtil::REFRESH_TYPE_PER_WEEK:
			if (CURRENT_DATE.WDay() == (Int32)refreshDate.weekDay)
			{
				if (CURRENT_DATE.Hour() < (Int32)refreshDate.hour)
				{
					break;
				}
			}
			nextRefreshRemainDays = 7 - CURRENT_DATE.WDay() + (Int32)refreshDate.weekDay;
			break;
		case TimeUtil::REFRESH_TYPE_PER_DAY:
			if (CURRENT_DATE.Hour() < (Int32)refreshDate.hour)
			{
				break;
			}
			nextRefreshRemainDays = 1;
			break;
		default:
			return false;
		}

		nextRefreshTimestamp = GetCurrentCustomeTimeStamp(refreshDate.hour) + (UInt64)nextRefreshRemainDays * DAY_TO_MSEC;
		
		return true;
	}

	bool CalculateNextRefreshTimestampByAppointTime(RefreshType refreshType, RefreshDate refreshDate, UInt64 appointTimeMsec, UInt64& nextRefreshTimestamp)
	{
		if (refreshType == TimeUtil::REFRESH_TYPE_NONE)
		{
			return false;
		}

		if (!refreshDate.CheckValid(refreshType))
		{
			return false;
		}

		if (appointTimeMsec == 0)
		{
			return false;
		}

		Avalon::Time appintTime(appointTimeMsec);
		Avalon::Date appointDate(appintTime);
// 		appointDate.Hour((Int32)refreshDate.hour);
// 		appointDate.Min(0);
// 		appointDate.Sec(0);

		Int32 nextRefreshRemainDays = 0;

		switch (refreshType)
		{
		case TimeUtil::REFRESH_TYPE_PER_MONTH:
		{
			if (appointDate.Day() == (Int32)refreshDate.day)
			{
				if (appointDate.Hour() < (Int32)refreshDate.hour)
				{
					break;
				}
			}
			Int32 daysInMonth = appointDate.GetDaysInMonth();
			Int32 curDay = appointDate.Day();
			if (daysInMonth < curDay)
			{
				return false;
			}
			nextRefreshRemainDays = daysInMonth - curDay + (Int32)refreshDate.day;
			break;
		}
		case TimeUtil::REFRESH_TYPE_PER_WEEK:
		{
			if (appointDate.WDay() == (Int32)refreshDate.weekDay)
			{
				if (appointDate.Hour() < (Int32)refreshDate.hour)
				{
					appointDate.Hour((Int32)refreshDate.hour);
					appointDate.Min(0);
					appointDate.Sec(0);
					break;
				}
			}
			Int32 curWDay = appointDate.WDay();
			if (7 < curWDay)
			{
				return false;
			}
			nextRefreshRemainDays = 7 - curWDay + (Int32)refreshDate.weekDay;
			break;
		}
		case TimeUtil::REFRESH_TYPE_PER_DAY:
			if (appointDate.Hour() < (Int32)refreshDate.hour)
			{
				break;
			}
			nextRefreshRemainDays = 1;
			break;
		default:
			return false;
		}

		nextRefreshTimestamp = appointDate.ToTime().MSec() + (UInt64)nextRefreshRemainDays * DAY_TO_MSEC;

		return true;
	}

	bool CalculateNextLogicMondayRefreshInterval(UInt64& nextRefreshInterval)
	{
		TimeUtil::RefreshDate date;
		date.weekDay = 1;
		date.hour = 6;

		UInt64 nextMondayRefreshTimestamp = 0;

		if (!TimeUtil::CalculateNextRefreshTimestamp(TimeUtil::REFRESH_TYPE_PER_WEEK, date, nextMondayRefreshTimestamp))
		{
			return false;
		}

		UInt64 curTime = CURRENT_TIME.MSec();
		if (nextMondayRefreshTimestamp < curTime)
		{
			return false;
		}

		nextRefreshInterval = nextMondayRefreshTimestamp - curTime;

		return true;
	}
}