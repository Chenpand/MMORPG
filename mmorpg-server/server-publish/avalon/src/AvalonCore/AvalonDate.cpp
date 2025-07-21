#include "AvalonDate.h"
#include <cstring>
#include <AvalonObjectPool.h>

#if defined(AVALON_OS_WINDOWS)
#	define avalon_tzset _tzset
#else
#	define avalon_tzset	tzset
#endif

namespace Avalon
{
	Date::Date()
	{
		memset(&m_Tm,0,sizeof(m_Tm));
		m_Msec = 0;
	}

#define AVALON_TZSET() \
	struct TZHelper{ \
	TZHelper(){ avalon_tzset(); }}; \
	static TZHelper _tzhelper;

	Date::Date(const Avalon::Time &time)
	{
		time_t t = (time_t)time.Sec();
		AVALON_TZSET();
#if defined(AVALON_COMPILER_MSVC)
		localtime_s(&m_Tm,&t);
#else
		localtime_r(&t,&m_Tm);
#endif
		m_Msec = (Int32)(time.MSec() % 1000);
	}

	Time Date::ToTime() const
	{
		AVALON_TZSET();
		return (UInt64)mktime((struct tm*)&m_Tm) * 1000 + m_Msec;
	}
}
