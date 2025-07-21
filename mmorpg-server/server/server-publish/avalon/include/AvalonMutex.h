/**
 *@author	 
 *@email	 
 *@date		2011-02-27	
 *@brief	Avalon������
 */
#ifndef _AVALON_MUTEX_H_
#define _AVALON_MUTEX_H_

#include "AvalonThreadAPI.h"

namespace Avalon
{
	/**
	 *@brief ��������windows����CriticalSectionʵ��
	 */
	class Mutex : private Nocopyable
	{
		friend class Condition;
	public:
		Mutex();
		~Mutex();

	public:
		/**
		 *@brief �����
		 */
		void Acquire();

		/**
		 *@brief �ͷ���
		 */
		void Release();

		/**
		 *@brief ���Ի����
		 */
		bool TryAcquire();
	private:
		//ϵͳ������
		mutex_t	m_Mutex;
	};


	/**
	 *@brief ��������
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
		//������
		Mutex *m_pMutex;
	};

#define AVALON_AUTOMUTEX(Mutex) \
	ScopeMutex _scopeMutex(&Mutex);
}

#endif
