/**
 *@author	 
 *@email	 
 *@date		2011-02-25	
 *@brief	��������
 */
#ifndef _AVALON_CONDITION_H_
#define _AVALON_CONDITION_H_

#include "AvalonThreadAPI.h"
#include "AvalonMutex.h"

namespace Avalon
{
	/**
	 *@brief ��������
	 */
	class Condition : private Nocopyable
	{
	public:
		Condition();
		~Condition();

		/**
		 *@brief �ȴ�
		 */
		void Wait(Mutex& mutex);

		/**
		 *@brief �ͷ�һ���߳�
		 */
		void Signal();

		/**
		 *@brief �ͷ������߳�
		 */
		void Broadcast();

	private:
		cond_t m_Cond;
	};
}

#endif
