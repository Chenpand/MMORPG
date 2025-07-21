#include "enet/utility.h"
#include <stdarg.h>

FILE* s_logFile = NULL;

void log2file(const char* format, ...)
{
	if (!s_logFile)
	{
		return;
	}

	char buf[1024];
	memset(buf, 0, sizeof(buf));

	va_list args;
	va_start(args, format);
	vsprintf(buf, format, args);
	va_end(args);

	fputs(buf, s_logFile);
	fflush(s_logFile);
}