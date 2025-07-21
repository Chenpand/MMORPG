#include "AvalonThreadAPI.h"
#include <cassert>

#if defined(AVALON_OS_WINDOWS)
#	include<limits.h>
#else
#	include<unistd.h>
#endif

namespace Avalon
{
#if defined(AVALON_OS_WINDOWS)

	void mutex_init(mutex_t& mutex)
	{
		InitializeCriticalSection(&mutex);
	}

	void mutex_lock(mutex_t& mutex)
	{
		EnterCriticalSection(&mutex);
	}

	bool mutex_trylock(mutex_t& mutex)
	{
#if _WIN32_WINNT >= 0x0400
		return TRUE == TryEnterCriticalSection(&mutex);
#else
		return false;
#endif
	}

	void mutex_unlock(mutex_t& mutex)
	{
		LeaveCriticalSection(&mutex);
	}

	void mutex_destroy(mutex_t& mutex)
	{
		DeleteCriticalSection(&mutex);
	}



	void cond_init(cond_t& cond)
	{
		SECURITY_ATTRIBUTES attr;
		attr.nLength = sizeof(SECURITY_ATTRIBUTES);
		attr.lpSecurityDescriptor = NULL;
		attr.bInheritHandle = TRUE;
		cond.events[0] = CreateEvent(&attr,FALSE,FALSE,NULL);
		cond.events[1] = CreateEvent(&attr,TRUE,FALSE,NULL);
		
		assert(NULL != cond.events[0] && NULL != cond.events[1]);
	}

	void cond_wait(cond_t& cond,mutex_t& mutex)
	{
		mutex_unlock(mutex);
		assert(WaitForMultipleObjects(2,cond.events,FALSE,INFINITE) != WAIT_FAILED);
		mutex_lock(mutex);
	}

	void cond_signal(cond_t& cond)
	{
		assert(SetEvent(cond.events[0]));
	}

	void cond_broadcast(cond_t& cond)
	{
		assert(SetEvent(cond.events[1]));
		ResetEvent(cond.events[1]);
	}

	void cond_destroy(cond_t& cond)
	{
		CloseHandle(cond.events[0]);
		CloseHandle(cond.events[1]);
	}


	void semaphore_init(sem_t& sem,UInt32 value)
	{
		SECURITY_ATTRIBUTES attr;
		attr.nLength = sizeof(SECURITY_ATTRIBUTES);
		attr.lpSecurityDescriptor = NULL;
		attr.bInheritHandle = TRUE;
		sem = CreateSemaphore(&attr,value,LONG_MAX,NULL);
		assert(sem != NULL);
	}

	void semaphore_post(sem_t& sem)
	{
		ReleaseSemaphore(sem,1,NULL);
	}

	void semaphore_wait(sem_t& sem)
	{
		WaitForSingleObject(sem,INFINITE);
	}

	void semaphore_destroy(sem_t& sem)
	{
		CloseHandle(sem);
	}


	bool thread_start(thread_t& thread,thread_function func,void* arg)
	{
		uintptr_t id = _beginthread(func,0,arg);
		if(id != -1L)
		{
			thread = (thread_t)id;
			return true;
		}
		return false;
	}

	void thread_exit()
	{
		_endthread();
	}

	void thread_terminate(thread_t thread)
	{
		TerminateThread(thread,0);
	}

	void thread_join(thread_t thread)
	{
		WaitForSingleObject(thread,INFINITE);
	}

	void thread_sleep(UInt32 msec)
	{
		Sleep(msec);
	}

#else

	void mutex_init(mutex_t& mutex)
	{
		pthread_mutexattr_t attr;	//锁属性
	
		pthread_mutexattr_init(&attr);		
		pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);	//设置锁类型
	
		assert(pthread_mutex_init(&mutex,&attr) == 0);
	
		pthread_mutexattr_destroy(&attr);
	}

	void mutex_lock(mutex_t& mutex)
	{
		assert(pthread_mutex_lock(&mutex) == 0);
	}

	bool mutex_trylock(mutex_t& mutex)
	{
		return pthread_mutex_trylock(&mutex) == 0;
	}

	void mutex_unlock(mutex_t& mutex)
	{
		assert(pthread_mutex_unlock(&mutex) == 0);
	}

	void mutex_destroy(mutex_t& mutex)
	{
		pthread_mutex_destroy(&mutex);
	}

	

	void cond_init(cond_t& cond)
	{
		assert(pthread_cond_init(&cond,NULL) == 0);
	}

	void cond_wait(cond_t& cond,mutex_t& mutex)
	{
		assert(pthread_cond_wait(&cond,&mutex) == 0);
	}

	void cond_signal(cond_t& cond)
	{
		assert(pthread_cond_signal(&cond) == 0);
	}

	void cond_broadcast(cond_t& cond)
	{
		assert(pthread_cond_broadcast(&cond) == 0);
	}

	void cond_destroy(cond_t& cond)
	{
		pthread_cond_destroy(&cond);
	}


	void semaphore_init(sem_t& sem,UInt32 value)
	{
		assert(sem_init(&sem,0,value) == 0);
	}

	void semaphore_post(sem_t& sem)
	{
		sem_post(&sem);
	}

	void semaphore_wait(sem_t& sem)
	{
		sem_wait(&sem);
	}

	void semaphore_destroy(sem_t& sem)
	{
		 sem_destroy(&sem);
	}


	bool thread_start(thread_t& thread,thread_function func,void* arg)
	{
		pthread_attr_t attr;
		pthread_attr_init(&attr);

		int ret = pthread_create(&thread, &attr, func,arg);
		pthread_attr_destroy(&attr);
		return ret >= 0;
	}

	void thread_exit()
	{
		pthread_exit(NULL);
	}

	void thread_terminate(thread_t thread)
	{
		pthread_cancel(thread);
	}

	void thread_join(thread_t thread)
	{
		pthread_join(thread,NULL);
	}

	void thread_sleep(UInt32 msec)
	{
		usleep(msec * 1000);
	}

#endif
}
