
#include "Common/common_base.h"
#include "Common/common_datetime.h"

#ifdef WIN32
//伪函数
int gettimeofday(struct timeval *a_pstTv, struct timezone *a_pstTz)
{
	return 0;
}

struct tm* localtime_r(const time_t *timep, struct tm *result)
{
	return NULL;
}

char* asctime_r(const struct tm *tm, char *buf)
{
	return NULL;
}

char* ctime_r(const time_t *timep, char *buf)
{
	return NULL;
}

struct tm* gmtime_r(const time_t *timep, struct tm *result)
{
	return NULL;
}

#endif

SERVER_BEGIN

int32_t get_string_datetime(const time_t time, char* pszBuffer)
{
	if (NULL == pszBuffer)
	{
		return fail;
	}

	struct tm stTime;
	struct tm* pstTime = NULL;
	pstTime = localtime_r(&time, &stTime);
	if (NULL == pstTime)
	{
		return fail;
	}

	sprintf(pszBuffer, "%04d-%02d-%02d %02d:%02d:%02d",
		stTime.tm_year + 1900, stTime.tm_mon + 1, stTime.tm_mday,
		stTime.tm_hour, stTime.tm_min, stTime.tm_sec);

	return success;
}

int32_t get_struct_datetime(const time_t time, stDateTime& out)
{
	struct tm *pstTime = NULL;
	struct tm stTime;
	time_t tTmp = time;

	
	pstTime = localtime_r(&tTmp, &stTime);
	if(NULL == pstTime)
	{
		return fail;
	}

	out.m_iYear = stTime.tm_year + 1900;
	out.m_iMon = stTime.tm_mon + 1;
	out.m_iDay = stTime.tm_mday;
	out.m_iHour = stTime.tm_hour;
	out.m_iMin = stTime.tm_min;
	out.m_iSec = stTime.tm_sec;
	out.m_iMSec = 0;

	return success;
}

int32_t get_current_string_datetime(char* pszBuffer)
{
	time_t now;
	time(&now);

	return get_string_datetime((const time_t) now, pszBuffer);
}

int32_t get_current_struct_datetime(stDateTime& out)
{
	time_t now;
	time(&now);
	return get_struct_datetime((const time_t)now, out);
}

int32_t get_current_string_date(char* strDate)
{
	struct tm *pstTm = NULL;
	time_t now  =0;

	if(NULL == strDate)
	{
		return fail;
	}

	time(&now);
	strDate[0] = '\0';

	pstTm = localtime(&now);
	if(NULL == pstTm)
	{
		return fail;
	}

	sprintf(strDate, "%04d-%02d-%02d", pstTm->tm_year + 1900, pstTm->tm_mon + 1, pstTm->tm_mday);
	return success;
}

int32_t get_current_string_time(char* strTime)
{
	struct tm *pstTm = NULL;
	time_t now = 0;

	if(NULL == strTime)
	{
		return fail;
	}

	time(&now);
	strTime[0] = '\0';

	pstTm = localtime(&now);
	if(NULL == pstTm)
	{
		return fail;
	}

	sprintf(strTime, "%02d:%02d:%02d", pstTm->tm_hour, pstTm->tm_min, pstTm->tm_sec);
	return success;
}

int32_t get_current_string_bill(char* strBill)
{
	struct tm *pstTm = NULL;
	time_t now  =0;

	if(NULL == strBill)
	{
		return fail;
	}

	time(&now);
	strBill[0] = '\0';

	pstTm = localtime(&now);
	if(NULL == pstTm)
	{
		return fail;
	}

	sprintf(strBill, "%04d-%02d-%02d-%02d", pstTm->tm_year + 1900, pstTm->tm_mon + 1, pstTm->tm_mday, pstTm->tm_hour);
	return success;
}

int32_t get_struct_datetime_from_string(const char* strDateTime, stDateTime& out)
{
	char *pTime = NULL;

	if (NULL == strDateTime) 
	{
		return fail;
	}

	pTime = (char*)&strDateTime[0];

	//年
	pTime[4] = '\0';
	out.m_iYear = atoi(pTime);
	pTime += 5;
	
	//月
	pTime[2] = '\0';
	out.m_iMon= atoi(pTime);
	pTime += 3;

	//日
	pTime[2] = '\0';
	out.m_iDay= atoi(pTime);
	pTime += 3;

	//小时
	pTime[2] = '\0';
	out.m_iHour= atoi(pTime);
	pTime += 3;

	//分钟
	pTime[2] = '\0';
	out.m_iMin= atoi(pTime);
	pTime += 3;

	//秒
	pTime[2] = '\0';
	out.m_iSec= atoi(pTime);
	pTime += 3;	

	return success;
}


int32_t get_string_from_struct_datetime(const stDateTime& stDate,char* pszBuffer)
{
	if (NULL == pszBuffer)
	{
		return  fail;
	}
	sprintf(pszBuffer, "%04d-%02d-%02d %02d:%02d:%02d",
		stDate.m_iYear , stDate.m_iMon, stDate.m_iDay,
		stDate.m_iHour, stDate.m_iMin, stDate.m_iSec);
	return  success;
}
void plus_timevalue(struct timeval& src, const struct timeval& plus)
{
	timeval tvTemp;
	tvTemp.tv_sec = src.tv_sec + plus.tv_sec;
	tvTemp.tv_sec += ((src.tv_usec+plus.tv_usec)/usec_of_one_second);
	tvTemp.tv_usec = ((src.tv_usec+plus.tv_usec)%usec_of_one_second);

	src.tv_sec = tvTemp.tv_sec;
	src.tv_usec = tvTemp.tv_usec;
}

void minus_timevalue(struct timeval& src, const struct timeval& minus)
{
	timeval tvTemp;

	if( src.tv_usec < minus.tv_usec )
	{
		tvTemp.tv_usec = (usec_of_one_second + src.tv_usec) - minus.tv_usec;
		tvTemp.tv_sec = src.tv_sec - minus.tv_sec - 1;
	}
	else
	{
		tvTemp.tv_usec = src.tv_usec - minus.tv_usec;
		tvTemp.tv_sec  = src.tv_sec - minus.tv_sec;
	}

	src.tv_sec = tvTemp.tv_sec;
	src.tv_usec = tvTemp.tv_usec;
}

//把类型为"YYYY-MM-DD HH:MM:SS"格式的datetime转换为结构时间struct tm
//成功返回 success, 否则其它
int32_t get_struct_tm_from_datetime_string( struct tm& struct_Tm, const char* pszDateTime )
{
	if (NULL == pszDateTime)
	{
		return fail;
	}

	char* pszTmp = NULL;
	char szTimeBuf[32];

	szTimeBuf[0] = '\0';
	memset(&struct_Tm, 0, sizeof(struct_Tm));

	strncpy(szTimeBuf, pszDateTime, sizeof(szTimeBuf)-1);

	if (strlen(pszDateTime) < 19) //"YYYY-MM-DD HH:MM:SS"格式至少有19个字符
	{
		return fail;
	}

	pszTmp = &szTimeBuf[0];
	//取出年
	pszTmp[4] = '\0';
	struct_Tm.tm_year = atoi(pszTmp) - 1900;
	pszTmp += 5;

	pszTmp[2] = '\0';
	struct_Tm.tm_mon = atoi(pszTmp) -1;
	pszTmp += 3;

	pszTmp[2] = '\0';
	struct_Tm.tm_mday = atoi(pszTmp);
	pszTmp += 3;

	pszTmp[2] = '\0';
	struct_Tm.tm_hour = atoi(pszTmp);
	pszTmp += 3;

	pszTmp[2] = '\0';
	struct_Tm.tm_min = atoi(pszTmp);
	pszTmp += 3;

	pszTmp[2] = '\0';
	struct_Tm.tm_sec = atoi(pszTmp);
	pszTmp += 3;	


	return success;

}

int32_t get_time_t_from_datetime_string( time_t& tmTime, const char* pszDateTime )
{
	if (NULL == pszDateTime)
	{
		return fail;
	}

	struct tm stTime;
	
	if (success != get_struct_tm_from_datetime_string(stTime, pszDateTime))
	{
		return fail;
	}

	time_t tTime = 0;
	tTime = mktime(&stTime);
	if ((time_t)(-1) == tTime)
	{
		return fail;
	}

	tmTime = tTime;

	return success;
}

int32_t get_string_date_from_time_t( const time_t time, char* date )
{
	struct stDateTime tmpDateTime;
	if (success != get_struct_datetime(time, tmpDateTime))
	{
		return fail;
	}

	sprintf(date, "%04d-%02d-%02d", tmpDateTime.m_iYear, tmpDateTime.m_iMon, tmpDateTime.m_iDay);
	
	return success;
}

int32_t get_string_now_date( char* date )
{
	time_t now = time(NULL);

	return get_string_date_from_time_t((const time_t)now, date);
}

SERVER_END
