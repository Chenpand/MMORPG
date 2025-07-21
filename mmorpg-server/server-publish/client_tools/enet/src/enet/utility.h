/** 
 @file  utility.h
 @brief ENet utility header
*/
#ifndef __ENET_UTILITY_H__
#define __ENET_UTILITY_H__

#include <stdlib.h>
#include <stdio.h>

#define ENET_MAX(x, y) ((x) > (y) ? (x) : (y))
#define ENET_MIN(x, y) ((x) < (y) ? (x) : (y))

int http_req(const char* url, const char* content);

void log2file(const char* format, ...);

void log2buf(const char* format, ...);

extern FILE* s_logFile;
extern char logs[64 * 1024];

#endif /* __ENET_UTILITY_H__ */

