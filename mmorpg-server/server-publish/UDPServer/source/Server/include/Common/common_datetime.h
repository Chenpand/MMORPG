#ifndef __COMMON_DATETIME_H__
#define __COMMON_DATETIME_H__

#include <time.h>
#include <sys/time.h>
#include <utime.h>

#include "gameplatform_types.h"
#include "common_namespace_definition.h"
SERVER_BEGIN

#ifdef __cplusplus
extern "C"
{
#endif

	enum
	{
		usec_of_one_second	= 1000000,	//
	};

	struct stDateTime
	{
		int32_t m_iYear;
		int32_t m_iMon;
		int32_t m_iDay;
		int32_t m_iHour;
		int32_t m_iMin;
		int32_t m_iSec;
		int32_t m_iMSec;
	};

	int32_t get_string_datetime(const time_t time, char* pszBuffer);
	int32_t get_struct_datetime(const time_t time, stDateTime& out);
	
	int32_t get_current_string_datetime(char* pszBuffer);
	int32_t get_current_struct_datetime(stDateTime& out);
	
	int32_t get_current_string_date(char* strDate);
	int32_t get_current_string_time(char* strTime);
	int32_t get_current_string_bill(char* strBill);

	int32_t get_struct_datetime_from_string(const char* strDateTime, stDateTime& out);
	int32_t get_string_from_struct_datetime(const stDateTime& stDate,char* pszBuffer);

	void plus_timevalue(struct timeval& src, const struct timeval& plus);
	void minus_timevalue(struct timeval& src, const struct timeval& minus);

	int32_t get_struct_tm_from_datetime_string(struct tm& struct_Tm, const char* pszDateTime);
	int32_t get_time_t_from_datetime_string(time_t& tmTime, const char* pszDateTime);

	//由调用者保证传入的date缓冲区的合法性
	int32_t get_string_date_from_time_t(const time_t time, char* date);
	int32_t get_string_now_date(char* date);

#ifdef __cplusplus
}
#endif

SERVER_END

#endif //__COMMON_DATETIME_H__
