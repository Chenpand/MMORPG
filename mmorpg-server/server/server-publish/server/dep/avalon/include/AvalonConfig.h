/**
 *@author	 
 *@email	 
 *@date		2011-02-25	
 *@brief	定义了一些跨平台相关的宏定义
 *			avalon支持windows和linux平台，在两个平台下平别使用vc和g++编译
 */
#ifndef _AVALON_CONFIG_H_
#define _AVALON_CONFIG_H_

#if defined(WIN32) || defined(_WIN32)
#	define AVALON_OS_WIN32		1
#	define AVALON_OS_WINDOWS	1
#elif defined(WIN64) || defined(_WIN64)
#	define AVALON_OS_WIN64		1
#	define AVALON_OS_WINDOWS	1
#else
#	define AVALON_OS_LINUX      1
#endif

#if defined(_MSC_VER)
#	define AVALON_COMPILER_MSVC	1
#	define AVALON_COMP_VER		_MSC_VER
#elif defined(__GNUC__)
#   define AVALON_COMPILER_GNUC	1
#	define AVALON_COMP_VER (((__GNUC__)*100) + \
        (__GNUC_MINOR__*10) + \
        __GNUC_PATCHLEVEL__)
#endif

#if defined(_DEBUG) || defined(DEBUG)
#	define AVALON_DEBUG_MODE
#endif

#if defined(AVALON_OS_WINDOWS)
#	define	AVALON_EXPORT __declspec( dllexport)
#else
#	define	AVALON_EXPORT
#endif

#if defined(__ALPHA) || defined(__alpha) || defined(__alpha__) || defined(_M_ALPHA)
	#define _ARCH _ARCH_ALPHA
	#define _ARCH_LITTLE_ENDIAN 1
#elif defined(i386) || defined(__i386) || defined(__i386__) || defined(_M_IX86)
	#define _ARCH _ARCH_IA32
	#define _ARCH_LITTLE_ENDIAN 1
#elif defined(_IA64) || defined(__IA64__) || defined(__ia64__) || defined(__ia64) || defined(_M_IA64)
	#define _ARCH _ARCH_IA64
	#if defined(hpux) || defined(_hpux)
		#define _ARCH_BIG_ENDIAN 1
	#else
		#define _ARCH_LITTLE_ENDIAN 1
	#endif
#elif defined(__x86_64__) || defined(_M_X64)
	#define _ARCH _ARCH_AMD64
	#define _ARCH_LITTLE_ENDIAN 1
#elif defined(__mips__) || defined(__mips) || defined(__MIPS__) || defined(_M_MRX000)
	#define _ARCH _ARCH_MIPS
	#define _ARCH_BIG_ENDIAN 1
#elif defined(__hppa) || defined(__hppa__)
	#define _ARCH _ARCH_HPPA
	#define _ARCH_BIG_ENDIAN 1
#elif defined(__PPC) || defined(__POWERPC__) || defined(__powerpc) || defined(__PPC__) || \
      defined(__powerpc__) || defined(__ppc__) || defined(__ppc) || defined(_ARCH_PPC) || defined(_M_PPC)
	#define _ARCH _ARCH_PPC
	#define _ARCH_BIG_ENDIAN 1
#elif defined(_POWER) || defined(_ARCH_PWR) || defined(_ARCH_PWR2) || defined(_ARCH_PWR3) || \
      defined(_ARCH_PWR4) || defined(__THW_RS6000)
	#define _ARCH _ARCH_POWER
	#define _ARCH_BIG_ENDIAN 1
#elif defined(__sparc__) || defined(__sparc) || defined(sparc)
	#define _ARCH _ARCH_SPARC
	#define _ARCH_BIG_ENDIAN 1
#elif defined(__arm__) || defined(__arm) || defined(ARM) || defined(_ARM_) || defined(__ARM__) || defined(_M_ARM)
	#define _ARCH _ARCH_ARM
	#if defined(__ARMEB__)
		#define _ARCH_BIG_ENDIAN 1
	#else
		#define _ARCH_LITTLE_ENDIAN 1
	#endif
#elif defined(__m68k__)
	#define _ARCH _ARCH_M68K
	#define _ARCH_BIG_ENDIAN 1
#elif defined(__s390__)
	#define _ARCH _ARCH_S390
	#define _ARCH_BIG_ENDIAN 1
#elif defined(__sh__) || defined(__sh)
	#define _ARCH _ARCH_SH
	#if defined(__LITTLE_ENDIAN__)
		#define _ARCH_LITTLE_ENDIAN 1
	#else
		#define _ARCH_BIG_ENDIAN 1
	#endif
#endif

#endif
