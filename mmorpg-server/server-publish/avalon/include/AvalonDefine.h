/**
 *@author	 
 *@email	 
 *@date		2011-02-25	
 *@brief	一些跨平台的定义
 */
#ifndef _AVALON_DEFINE_H_
#define _AVALON_DEFINE_H_

#include "AvalonConfig.h"
#include <cstdio>
#include <cstring>
#include <stdint.h>

#ifdef __GNUC__ //gnu gcc/g++

#include <inttypes.h>
#endif // __GNUC__

#if defined(AVALON_COMPILER_GNUC) && !defined(STLPORT)
#   if AVALON_COMP_VER >= 430
#       include <tr1/unordered_map>
#       include <tr1/unordered_set> 
#   else
#       include <ext/hash_map>
#       include <ext/hash_set>
#   endif
#else
#   if defined(AVALON_COMPILER_MSVC) && !defined(STLPORT) && AVALON_COMP_VER >= 1600 // VC++ 10.0
#    	include <unordered_map>
#    	include <unordered_set>
#	else
#   	include <hash_set>
#   	include <hash_map>
#	endif
#endif 


//版本号定义
#define AVALON_VERSION_MAJOR 1
#define AVALON_VERSION_MINOR 0
#define AVALON_VERSION_PATCH 0

#define AVALON_VERSION    ((AVALON_VERSION_MAJOR << 16) | (AVALON_VERSION_MINOR << 8) | AVALON_VERSION_PATCH)

namespace Avalon
{
	//基本类型跨平台定义
	typedef char			Int8;
	typedef unsigned char	UInt8;
	typedef short			Int16;
	typedef unsigned short	UInt16;
	typedef int				Int32;
	typedef unsigned int	UInt32;


#if defined(AVALON_COMPILER_MSVC)
	typedef __int64			Int64;
	typedef unsigned __int64	UInt64;
	typedef	int				key_t;			//added by aprilliu
#else
	typedef long long		Int64;
	typedef unsigned long long	UInt64;
#endif

	typedef float			Real32;
	typedef	double			Real64;

	typedef enum 
	{
		success = 0,
		fail = -1,
	}avalon_result;

	//hash_map,hash_set跨平台定义
#if defined(AVALON_COMPILER_GNUC) && AVALON_COMP_VER >= 310 && !defined(STLPORT)
#   if AVALON_COMP_VER >= 430
#       define HashMap ::std::tr1::unordered_map
#		define HashSet ::std::tr1::unordered_set
#   else
#       define HashMap ::__gnu_cxx::hash_map
#       define HashSet ::__gnu_cxx::hash_set
#   endif
#else
#   if defined(AVALON_COMPILER_MSVC)
#       if AVALON_COMP_VER >= 1600 // VC++ 10.0
#			define HashMap ::std::tr1::unordered_map
#           define HashSet ::std::tr1::unordered_set
#		elif AVALON_COMP_VER > 1300 && !defined(_STLP_MSVC)
#           define HashMap ::stdext::hash_map
#           define HashSet ::stdext::hash_set
#       else
#           define HashMap ::std::hash_map
#           define HashSet ::std::hash_set
#       endif
#   else
#       define HashMap ::std::hash_map
#       define HashSet ::std::hash_set
#   endif
#endif


#if defined(AVALON_COMPILER_MSVC)
#	define AVALON_INT64_FORMAT	"%I64d"
#	define AVALON_UINT64_FORMAT "%I64u"

#	define avalon_snprintf sprintf_s
#	define avalon_sscanf sscanf_s
#	define avalon_strncpy(d,s,l) strncpy_s(d,l+1,s,l)
#	define avalon_memmove(d,s,l) memmove_s(d,l,s,l)
#	define avalon_vsnprintf(d,l,f,a) vsnprintf_s(d,l,_TRUNCATE,f,a)
#	define avalon_atoll(s) _atoi64(s)
#	define avalon_strdup(s) _strdup(s)

#else
#	define AVALON_INT64_FORMAT "%lld"
#	define AVALON_UINT64_FORMAT "%llu"

#	define avalon_snprintf snprintf
#	define avalon_sscanf sscanf
#	define avalon_strncpy strncpy
#	define avalon_memmove memmove
#	define avalon_vsnprintf vsnprintf
#	define avalon_atoll(s) atoll(s)
#	define avalon_strdup(s) strdup(s)
#endif


#define AVALON_DELETE(Ptr) \
	do{ \
		delete Ptr; \
		Ptr = NULL; \
	}while(false)

#define AVALON_DELETE_ARRAY(Ptr) \
	do { \
		delete[] Ptr; \
		Ptr = NULL; \
	}while(false)
}

#if defined(AVALON_COMPILER_GNUC) && AVALON_COMP_VER >= 310 && AVALON_COMP_VER < 430 && !defined(STLPORT)
#	include	<string>
namespace __gnu_cxx
{
	template<> struct hash<std::string>
	{
		size_t operator()(const std::string& s) const
		{
			return __stl_hash_string(s.c_str());
		}
	};

	template<> struct hash<Avalon::UInt64>
	{	// hash functor
	public:
	typedef Avalon::UInt64 _Kty;
	typedef Avalon::UInt32 _Inttype;	// use first 2*32 bits

	size_t operator()(const _Kty& _Keyval) const
		{	// hash _Keyval to size_t value by pseudorandomizing transform
		return (hash<_Inttype>()((_Inttype)(_Keyval & 0xffffffffUL))
			^ hash<_Inttype>()((_Inttype)(_Keyval >> 32)));
		}
	};
}
#endif

/**
 *@brief 一些常量定义
 */
#define AVALON_PI 3.141592653589793f

#include "AvalonNocopyable.h"

#endif

