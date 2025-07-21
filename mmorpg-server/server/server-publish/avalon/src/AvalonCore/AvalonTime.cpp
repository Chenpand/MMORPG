#include "AvalonTime.h"
#if defined(AVALON_OS_WINDOWS)
#	include<windows.h>
#else
#	include<sys/time.h>
#endif

namespace Avalon
{

	Time Time::CurrentTime()
	{
#if defined(AVALON_OS_WINDOWS)
		FILETIME fileTime;
		ULARGE_INTEGER li;
		const static UInt64 TIME_DIFF = 116444736000000000; 
;
		::GetSystemTimeAsFileTime(&fileTime);
		li.LowPart = fileTime.dwLowDateTime;
		li.HighPart = fileTime.dwHighDateTime;
		return ((UInt64)li.QuadPart - TIME_DIFF)/10000;
#else
		struct timeval tv;
		gettimeofday(&tv,NULL);
		return (UInt64)tv.tv_sec * 1000 + (UInt64)tv.tv_usec/1000;
#endif
	}
}

