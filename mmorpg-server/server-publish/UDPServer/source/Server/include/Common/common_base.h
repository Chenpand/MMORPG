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

	queue_buffer_is_not_enough = 2,   /**< ���пռ䲻��*/
	queue_is_empty			   = 3,   /**< ����Ϊ��*/


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
	max_datetime_string_length = 32,  /**<����ʱ���ַ�������󳤶� <*/
	max_file_name_length = 255,		  /**<�ļ�����󳤶�<*/
	max_path_length = 255,			  /**<·������󳤶�*/ 
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

//�޳��ַ���ǰ��Ŀ��ַ�������: '', '\t', '\n'��
void TrimString(char *pszString );

//�޳��ַ���ǰ����ַ�������: '', '\t', '\n'�ȣ�����ת��ASCII�ַ�ΪСд
void StringTrimAndToLowerCase(char *pszString );

//
/*
* @method:    get_next_token
* @fullname:  Game51::Server::get_next_token
* @access:    public 
* @param: const char * src
* @param: char separator �ָ��
* @param: char * out ��src�е�һ�γ��ַָ����ǰ������
* @return:   char* ���طָ���Ժ������
* @qualifier:
* @note	
* @par ʾ��:
* @code

* @endcode
 
* @see
* @deprecated 
*/
char* get_next_token(const char* src, char separator, char* out);


/*
 * nochdir ��0����daemon���̽��ı䵱ǰ����Ŀ¼��"/"
 * noclodes ��0�� ��daemon���̽��ض����׼��������Լ�����"/dev/null"
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
