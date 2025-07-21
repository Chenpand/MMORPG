/**
 *@author	 
 *@email	 
 *@date		2011-02-27	
 *@brief	�߳�API��ض���
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
	 *@brief ��ʼ��������
	 */
	void mutex_init(mutex_t& mutex);

	/**
	 *@brief ����
	 */
	void mutex_lock(mutex_t& mutex);

	/**
	 *@brief ��������
	 */
	bool mutex_trylock(mutex_t& mutex);

	/**
	 *@brief ����
	 */
	void mutex_unlock(mutex_t& mutex);

	/**
	 *@brief ������
	 */
	void mutex_destroy(mutex_t& mutex);



	/**
	 *@brief ��ʼ����������
	 */
	void cond_init(cond_t& cond);

	/**
	 *@brief �ȴ���������
	 */
	void cond_wait(cond_t& cond,mutex_t& mutex);

	/**
	 *@brief ����һ���ȴ����߳�
	 */
	void cond_signal(cond_t& cond);

	/**
	 *@brief �ͷ����еȴ��߳�
	 */
	void cond_broadcast(cond_t& cond);

	/**
	 *@brief ������������
	 */
	void cond_destroy(cond_t& cond);



	/**
	 *@brief ��ʼ���ź���
	 */
	void semaphore_init(sem_t& sem,UInt32 value);

	/**
	 *@brief �����ź���ֵ
	 */
	void semaphore_post(sem_t& sem);

	/**
	 *@brief �ȴ��ź���
	 */
	void semaphore_wait(sem_t& sem);

	/**
	 *@brief �����ź���
	 */
	void semaphore_destroy(sem_t& sem);


	/**
	 *@brief ����һ���߳�
	 */
	bool thread_start(thread_t& thread,thread_function func,void* arg);

	/**
	 *@brief �˳��߳�
	 */
	void thread_exit();

	/**
	 *@brief ǿ����ֹ�߳�
	 */
	void thread_terminate(thread_t thread);

	/**
	 *@brief �ȴ��߳���ֹ��������Դ
	 */
	void thread_join(thread_t thread);


	/**
	 *@brief �ȴ�һ��ʱ��
	 */
	void thread_sleep(UInt32 msec);
}

#endif
