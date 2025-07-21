/**
 *@author	 
 *@email	 
 *@date		2011-02-27	
 *@brief	线程API相关定义
 */
#ifndef _AVALON_THREADAPI_H_
#define _AVALON_THREADAPI_H_

#include "AvalonDefine.h"
#if defined(AVALON_OS_WINDOWS)
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#	include <process.h>
#else 
#	include <pthread.h>
#	include <semaphore.h>
#endif

namespace Avalon
{
#if defined(AVALON_OS_WINDOWS)
	typedef CRITICAL_SECTION mutex_t;
	typedef struct
	{
		HANDLE events[2];
	} cond_t;
	typedef HANDLE thread_t;
	typedef HANDLE sem_t;
	typedef  void (* thread_function)(void*);
#	define AVALON_THREADFUNC_RET void
#	define AVALON_THREADFUNC_VAL 
#else
	typedef pthread_mutex_t mutex_t;
	typedef pthread_cond_t cond_t;
	typedef pthread_t thread_t;
	typedef void* (*thread_function)(void*);
#	define AVALON_THREADFUNC_RET void*
#	define AVALON_THREADFUNC_VAL NULL
#endif

#define AVALON_INVALID_THREAD 0

	/**
	 *@brief 初始化互斥体
	 */
	void mutex_init(mutex_t& mutex);

	/**
	 *@brief 上锁
	 */
	void mutex_lock(mutex_t& mutex);

	/**
	 *@brief 尝试上锁
	 */
	bool mutex_trylock(mutex_t& mutex);

	/**
	 *@brief 解锁
	 */
	void mutex_unlock(mutex_t& mutex);

	/**
	 *@brief 销毁锁
	 */
	void mutex_destroy(mutex_t& mutex);



	/**
	 *@brief 初始化条件变量
	 */
	void cond_init(cond_t& cond);

	/**
	 *@brief 等待条件变量
	 */
	void cond_wait(cond_t& cond,mutex_t& mutex);

	/**
	 *@brief 唤醒一个等待的线程
	 */
	void cond_signal(cond_t& cond);

	/**
	 *@brief 释放所有等待线程
	 */
	void cond_broadcast(cond_t& cond);

	/**
	 *@brief 销毁条件变量
	 */
	void cond_destroy(cond_t& cond);



	/**
	 *@brief 初始化信号量
	 */
	void semaphore_init(sem_t& sem,UInt32 value);

	/**
	 *@brief 增加信号量值
	 */
	void semaphore_post(sem_t& sem);

	/**
	 *@brief 等待信号量
	 */
	void semaphore_wait(sem_t& sem);

	/**
	 *@brief 销毁信号量
	 */
	void semaphore_destroy(sem_t& sem);


	/**
	 *@brief 启动一个线程
	 */
	bool thread_start(thread_t& thread,thread_function func,void* arg);

	/**
	 *@brief 退出线程
	 */
	void thread_exit();

	/**
	 *@brief 强制终止线程
	 */
	void thread_terminate(thread_t thread);

	/**
	 *@brief 等待线程终止并回收资源
	 */
	void thread_join(thread_t thread);


	/**
	 *@brief 等待一定时间
	 */
	void thread_sleep(UInt32 msec);
}

#endif
