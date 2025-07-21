#ifndef __COMMON_BASE_H__
#define __COMMON_BASE_H__

#include "common_namespace_definition.h"
#include "gameplatform_types.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>
#include <time.h>
#include <errno.h>
//#include <stdint.h>
#include <stdarg.h>

// #ifdef WIN32
// #include <winsock2.h>
// #include <sys/types.h>
// #include <sys/timeb.h>
// #else
#include <signal.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include <net/if.h>
#include <dirent.h>
#include <dlfcn.h>
#include <sys/select.h>
#include <assert.h>

//#endif

SERVER_BEGIN

enum
{
	success = 0,
	fail = 1,

	error = -1,

	queue_buffer_is_not_enough = 2,   /**< 队列空间不够*/
	queue_is_empty			   = 3,   /**< 队列为空*/


};

enum //seconds in xxx
{
	MINUTE = 60, //one minute
	HOUR   = 60*MINUTE, //one hour
	DAY	   = 24*HOUR,	//one day
	YEAR   = 365*DAY	//one year
};

enum
{
	max_datetime_string_length = 32,  /**<日期时间字符串的最大长度 <*/
	max_file_name_length = 255,		  /**<文件名最大长度<*/
	max_path_length = 255,			  /**<路径的最大长度*/ 
};


#ifndef MAX
#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#define MS_TO_TV(tv, ms)		do									\
{									\
	(tv).tv_sec = (ms)/1000; 		\
	(tv).tv_usec = ((ms) % 1000)*1000;\
	} while(0)

#define TV_TO_MS(ms, tv)		ms = (tv).tv_sec*1000 + (tv).tv_usec/1000

#define TV_DIFF(t, t1, t2)		do									\
{									\
	(t).tv_sec	=	(t1).tv_sec - (t2).tv_sec;\
	(t).tv_usec	=	(t1).tv_usec - (t2).tv_usec;\
	} while(0)

#define TV_CLONE(dst, src)		do									\
{									\
	(dst).tv_sec	=	(src).tv_sec; \
	(dst).tv_usec	=	(src).tv_usec; \
	} while(0)

#define ABS_MINUS(v1, v2)	((v1) > (v2) ? ((v1) - (v2)) : ((v2) - (v1)) )

//剔除字符串前后的空字符，比如: '', '\t', '\n'等
void TrimString(char *pszString );

//剔除字符串前后空字符，比如: '', '\t', '\n'等，并且转换ASCII字符为小写
void StringTrimAndToLowerCase(char *pszString );

//
/*
* @method:    get_next_token
* @fullname:  Game51::Server::get_next_token
* @access:    public 
* @param: const char * src
* @param: char separator 分割符
* @param: char * out 在src中第一次出现分割符以前的内容
* @return:   char* 返回分割符以后的内容
* @qualifier:
* @note	
* @par 示例:
* @code

* @endcode
 
* @see
* @deprecated 
*/
char* get_next_token(const char* src, char separator, char* out);


/*
 * nochdir 非0，则daemon进程将改变当前工作目录到"/"
 * noclodes 非0， 则daemon进程将重定向标准输入输出以及错误到"/dev/null"
*/
void InitDaemon(int8_t nochdir, int8_t noclose);

//
template <class T>
void game51_swap(T& lv, T& rv)
{
	T tmp = lv;
	lv = rv;
	rv = tmp;
}


SERVER_END

#endif /*__COMMON_BASE_H__*/
