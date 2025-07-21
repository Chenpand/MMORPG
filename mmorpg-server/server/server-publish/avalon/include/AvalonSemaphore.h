/**
 *@author	 
 *@email	 
 *@date		2011-03-01	
 *@brief	信号量
 */
#ifndef _AVALON_SEMAPHORE_H_
#define _AVALON_SEMAPHORE_H_

#include "AvalonThreadAPI.h"

namespace Avalon
{
	/**
	 *@brief 信号量
	 */
	class Semaphore : private Nocopyable
	{
	public:
		explicit Semaphore(UInt32 value = 0);
		~Semaphore();

		/*
		 *@brief post一个信号
		 */
		void Post();

		/**
		 *@brief 等待
		 */
		void Wait();

	private:
		sem_t m_Sem;
	};
}

#endif
