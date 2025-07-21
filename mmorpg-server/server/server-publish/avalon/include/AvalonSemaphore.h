/**
 *@author	 
 *@email	 
 *@date		2011-03-01	
 *@brief	�ź���
 */
#ifndef _AVALON_SEMAPHORE_H_
#define _AVALON_SEMAPHORE_H_

#include "AvalonThreadAPI.h"

namespace Avalon
{
	/**
	 *@brief �ź���
	 */
	class Semaphore : private Nocopyable
	{
	public:
		explicit Semaphore(UInt32 value = 0);
		~Semaphore();

		/*
		 *@brief postһ���ź�
		 */
		void Post();

		/**
		 *@brief �ȴ�
		 */
		void Wait();

	private:
		sem_t m_Sem;
	};
}

#endif
