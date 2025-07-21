#ifndef _CL_TIME_UTIL_H_
#define _CL_TIME_UTIL_H_

#include "CLDefine.h"


namespace TimeUtil
{
	// 刷新类型
	enum RefreshType
	{
		// 不刷新
		REFRESH_TYPE_NONE = 0,
		// 每月刷新
		REFRESH_TYPE_PER_MONTH,
		// 每周刷新
		REFRESH_TYPE_PER_WEEK,
		// 每日刷新
		REFRESH_TYPE_PER_DAY,
		// 赛季刷新
		REFRESH_TYPE_SEASON,
	};

	// 刷新日期
	struct RefreshDate
	{
		RefreshDate()
		: day(0), weekDay(0), hour(0) {}

		bool CheckValid(RefreshType refreshType) const
		{
			switch (refreshType)
			{
			case REFRESH_TYPE_NONE:
				break;
			case REFRESH_TYPE_PER_MONTH:
				if (day == 0 || day > 28) return false;
				if (hour > 23) return false;
				break;
			case REFRESH_TYPE_PER_WEEK:
				if (weekDay == 0 || weekDay > 7) return false;
				if (hour > 23) return false;
				break;
			case REFRESH_TYPE_PER_DAY:
				if (hour > 23) return false;
				break;
			case REFRESH_TYPE_SEASON:
				break;
			default:
				return false;
			}
			return true;
		}

		const std::string LogStr() const
		{
			std::ostringstream oss;
			oss << "refresh date(day:" << day << ",weekDay:" << weekDay << ",hour:" << hour << ")";
			return oss.str();
		}

		// 刷新日(1-28)
		UInt32 day;
		// 刷新星期(1-7)
		UInt32 weekDay;
		// 刷新时(0-23)
		UInt32 hour;
	};

	// 时间戳计算类型
	enum TimestampCalcType
	{
		// 通用类型(2018-4-8 21:42:39 => 2018-04-08 21:42:39)
		TIMESTAMP_CALC_TYPE_COMMON = 0,
		// 根据开服时间计算
		TIMESTAMP_CALC_TYPE_SERVICE_TIME,
		// 玩家升级开始计时(小时)
		TIMESTAMP_CALC_TYPE_PLAYER_LEVEL_UP,
	};

	/**
	*@brief 时间转换,标准格式-->时间戳(2018-4-8 21:42:39 => 1523194959)
	*/
	UInt32 StandardFormatConvertToTimestamp(const std::string& timeStr);

	/**
	*@brief 时间格式拼接(2018-4-8 21:42:39 => 2018-04-08 21:42:39)
	*/
	const std::string TimeFormatJoint(UInt32 threshold, UInt32 param);

	/**
	*@brief 时间转换,标准格式-->时间戳(1523194959 => 2018-04-08 21:42:39))
	*/
	const std::string TimestampToStandardFormatCustomer(UInt32 timestamp);

	/**
	*@brief 时间转换,标准格式-->时间戳(1523194959 => 2018-4-8 21:42:39)
	*/
	const std::string TimestampToStandardFormat(UInt32 timestamp);

	/**
	*@brief 时间转换,标准格式-->时间戳(1523194959 => 2018-4-8)
	*/
	const std::string TimestampToStandardFormatNoHour(UInt32 timestamp);

	/**
	*@brief 时间转换,标准格式-->时间戳(1523194959 => 2018-04-08<21:42:39>)
	*/
	const std::string TimestampToStandarFormatNoSpace(UInt32 timestamp);

	/**
	*@brief 时间转换,适配表格新旧时间格式
	*/
	bool TimeStrConvertToTimestamp(UInt32& timestamp, const std::string& timeStr);

	/**
	*@brief 时间计算,根据起点和间隔计算终点
	*/
	Int32 CalculateEndPointByStartPointAndInterval(UInt32 satrtPoint, const std::string& interval, UInt32& endPoint);

	/**
	*@brief 获取当前时间微秒级
	*/
	UInt64 GetMicroSecondNow();

	/**
	*@brief 获取当前的自定义时间(毫秒级)
	*/
	UInt64 GetCurrentCustomeTimeStamp(UInt32 hour, UInt32 min = 0, UInt32 sec = 0, UInt32 msec = 0);

	/**
	*@brief 刷新日期转换,字符串-->结构
	*/
	bool StringConvertToRefreshDate(RefreshType refreshType, const std::string& refreshTimePoint, RefreshDate& refreshDate);

	/**
	*@brief 计算下一次刷新时间戳(毫秒级)
	*/
	bool CalculateNextRefreshTimestamp(RefreshType refreshType, RefreshDate refreshDate, UInt64& nextRefreshTimestamp);

	/**
	*@brief 根据指定时间计算下一次刷新时间戳(毫秒级)
	*/
	bool CalculateNextRefreshTimestampByAppointTime(RefreshType refreshType, RefreshDate refreshDate, UInt64 appointTimeMsec, UInt64& nextRefreshTimestamp);

	/**
	*@brief 计算下一次周一六点刷新时间间隔(毫秒级)
	*/
	bool CalculateNextLogicMondayRefreshInterval(UInt64& nextRefreshInterval);

}

#endif