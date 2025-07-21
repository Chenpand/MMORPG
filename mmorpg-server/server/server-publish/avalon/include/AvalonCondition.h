/**
 *@author	 
 *@email	 
 *@date		2011-02-25	
 *@brief	条件变量
 */
#ifndef _AVALON_CONDITION_H_
#define _AVALON_CONDITION_H_

#include "AvalonThreadAPI.h"
#include "AvalonMutex.h"

namespace Avalon
{
	/**
	 *@brief 条件变量
	 */
	class Condition : private Nocopyable
	{
	public:
		Condition();
		~Condition();

		/**
		 *@brief 等待
		 */
		void Wait(Mutex& mutex);

		/**
		 *@brief 释放一个线程
		 */
		void Signal();

		/**
		 *@brief 释放所有线程
		 */
		void Broadcast();

	private:
		cond_t m_Cond;
	};
}

#endif
