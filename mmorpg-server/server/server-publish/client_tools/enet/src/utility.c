#include "enet/utility.h"
#include <stdarg.h>

//static FILE* s_logFile = NULL;
FILE* s_logFile = NULL;

int http_req(const char* url, const char* content)
{

    return 0;
}

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

void log2buf(const char* format, ...)
{
    static int init = 1;
    if (init)
    {
        memset(logs, 0, sizeof(logs));
        init = 0;
    }

    char buf[1024];
    memset(buf, 0, sizeof(buf));

    va_list args;
    va_start(args, format);
    vsprintf(buf, format, args);
    va_end(args);

    strcat(logs, buf);
}