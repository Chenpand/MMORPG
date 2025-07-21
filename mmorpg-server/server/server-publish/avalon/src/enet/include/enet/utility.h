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

void log2file(const char* format, ...);

extern FILE* s_logFile;

#endif /* __ENET_UTILITY_H__ */

