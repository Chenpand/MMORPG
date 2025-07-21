#ifndef _CL_TIME_UTIL_H_
#define _CL_TIME_UTIL_H_

#include "CLDefine.h"


namespace TimeUtil
{
	// ˢ������
	enum RefreshType
	{
		// ��ˢ��
		REFRESH_TYPE_NONE = 0,
		// ÿ��ˢ��
		REFRESH_TYPE_PER_MONTH,
		// ÿ��ˢ��
		REFRESH_TYPE_PER_WEEK,
		// ÿ��ˢ��
		REFRESH_TYPE_PER_DAY,
		// ����ˢ��
		REFRESH_TYPE_SEASON,
	};

	// ˢ������
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

		// ˢ����(1-28)
		UInt32 day;
		// ˢ������(1-7)
		UInt32 weekDay;
		// ˢ��ʱ(0-23)
		UInt32 hour;
	};

	// ʱ�����������
	enum TimestampCalcType
	{
		// ͨ������(2018-4-8 21:42:39 => 2018-04-08 21:42:39)
		TIMESTAMP_CALC_TYPE_COMMON = 0,
		// ���ݿ���ʱ�����
		TIMESTAMP_CALC_TYPE_SERVICE_TIME,
		// ���������ʼ��ʱ(Сʱ)
		TIMESTAMP_CALC_TYPE_PLAYER_LEVEL_UP,
	};

	/**
	*@brief ʱ��ת��,��׼��ʽ-->ʱ���(2018-4-8 21:42:39 => 1523194959)
	*/
	UInt32 StandardFormatConvertToTimestamp(const std::string& timeStr);

	/**
	*@brief ʱ���ʽƴ��(2018-4-8 21:42:39 => 2018-04-08 21:42:39)
	*/
	const std::string TimeFormatJoint(UInt32 threshold, UInt32 param);

	/**
	*@brief ʱ��ת��,��׼��ʽ-->ʱ���(1523194959 => 2018-04-08 21:42:39))
	*/
	const std::string TimestampToStandardFormatCustomer(UInt32 timestamp);

	/**
	*@brief ʱ��ת��,��׼��ʽ-->ʱ���(1523194959 => 2018-4-8 21:42:39)
	*/
	const std::string TimestampToStandardFormat(UInt32 timestamp);

	/**
	*@brief ʱ��ת��,��׼��ʽ-->ʱ���(1523194959 => 2018-4-8)
	*/
	const std::string TimestampToStandardFormatNoHour(UInt32 timestamp);

	/**
	*@brief ʱ��ת��,��׼��ʽ-->ʱ���(1523194959 => 2018-04-08<21:42:39>)
	*/
	const std::string TimestampToStandarFormatNoSpace(UInt32 timestamp);

	/**
	*@brief ʱ��ת��,�������¾�ʱ���ʽ
	*/
	bool TimeStrConvertToTimestamp(UInt32& timestamp, const std::string& timeStr);

	/**
	*@brief ʱ�����,�������ͼ�������յ�
	*/
	Int32 CalculateEndPointByStartPointAndInterval(UInt32 satrtPoint, const std::string& interval, UInt32& endPoint);

	/**
	*@brief ��ȡ��ǰʱ��΢�뼶
	*/
	UInt64 GetMicroSecondNow();

	/**
	*@brief ��ȡ��ǰ���Զ���ʱ��(���뼶)
	*/
	UInt64 GetCurrentCustomeTimeStamp(UInt32 hour, UInt32 min = 0, UInt32 sec = 0, UInt32 msec = 0);

	/**
	*@brief ˢ������ת��,�ַ���-->�ṹ
	*/
	bool StringConvertToRefreshDate(RefreshType refreshType, const std::string& refreshTimePoint, RefreshDate& refreshDate);

	/**
	*@brief ������һ��ˢ��ʱ���(���뼶)
	*/
	bool CalculateNextRefreshTimestamp(RefreshType refreshType, RefreshDate refreshDate, UInt64& nextRefreshTimestamp);

	/**
	*@brief ����ָ��ʱ�������һ��ˢ��ʱ���(���뼶)
	*/
	bool CalculateNextRefreshTimestampByAppointTime(RefreshType refreshType, RefreshDate refreshDate, UInt64 appointTimeMsec, UInt64& nextRefreshTimestamp);

	/**
	*@brief ������һ����һ����ˢ��ʱ����(���뼶)
	*/
	bool CalculateNextLogicMondayRefreshInterval(UInt64& nextRefreshInterval);

}

#endif