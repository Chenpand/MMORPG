/**
 *@author	 
 *@email	 
 *@date		2011-02-27	
 *@brief	˫���У������̼߳�ͨ��
 */
#ifndef _AVALON_DOUBLEQUEUE_H_
#define _AVALON_DOUBLEQUEUE_H_

#include <AvalonCircularQueue.h>
#include <AvalonMutex.h>

namespace Avalon
{
	/**
	 *@brief ˫����
	 */
	template<typename T,size_t InitSize = 32>
	class DoubleQueue
	{
		typedef CircularQueue<T, InitSize> Queue;

	public:
		DoubleQueue()
		{
			m_pInputQueue = new Queue;
			m_pOutputQueue = new Queue;
		}
		~DoubleQueue()
		{
			delete m_pInputQueue;
			delete m_pOutputQueue;
		}

		/**
		 *@brief �ж��Ƿ�Ϊ��
		 */
		bool empty()
		{
			if(m_pOutputQueue->empty())
			{
				AVALON_AUTOMUTEX(m_Mutex);
				std::swap(m_pInputQueue, m_pOutputQueue); //noted by aprilliu, ��Ҫ����������������ʱ ��������
			}
			return m_pOutputQueue->empty();
		}

		/**
		 *@brief ����һ��Ԫ��   noted by aprilliu, ����������н���push���߳�ֻ��һ��������Բ�����
		 */
		void push(T value)
		{
			m_Mutex.Acquire();
			m_pInputQueue->push(value);
			m_Mutex.Release();
		}

		/**
		 *@brief ȡ��һ��Ԫ��
		 */
		T pop()
		{
			if(m_pOutputQueue->empty())
			{
				AVALON_AUTOMUTEX(m_Mutex);
				std::swap(m_pInputQueue, m_pOutputQueue); //noted by aprilliu, ��Ҫ����������������ʱ ��������
			}

			return m_pOutputQueue->pop();
		}

	private:
		//�������
		Queue		*m_pInputQueue;
		//�������
		Queue		*m_pOutputQueue;
		//������
		Avalon::Mutex	m_Mutex; 
	};

}

#endif
