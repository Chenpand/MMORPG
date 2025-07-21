/**
 *@author	 
 *@email	 
 *@date		2011-02-27	
 *@brief	Avalon互斥锁
 */
#ifndef _AVALON_MUTEX_H_
#define _AVALON_MUTEX_H_

#include "AvalonThreadAPI.h"

namespace Avalon
{
	/**
	 *@brief 互斥锁，windows下用CriticalSection实现
	 */
	class Mutex : private Nocopyable
	{
		friend class Condition;
	public:
		Mutex();
		~Mutex();

	public:
		/**
		 *@brief 获得锁
		 */
		void Acquire();

		/**
		 *@brief 释放锁
		 */
		void Release();

		/**
		 *@brief 尝试获得锁
		 */
		bool TryAcquire();
	private:
		//系统互斥体
		mutex_t	m_Mutex;
	};


	/**
	 *@brief 作用域锁
	 */
	class ScopeMutex : private Nocopyable
	{
	public:
		ScopeMutex(Mutex* pMutex):m_pMutex(pMutex)
		{
			m_pMutex->Acquire();
		}

		~ScopeMutex()
		{
			m_pMutex->Release();
		}
	private:
		//互斥体
		Mutex *m_pMutex;
	};

#define AVALON_AUTOMUTEX(Mutex) \
	ScopeMutex _scopeMutex(&Mutex);
}

#endif
