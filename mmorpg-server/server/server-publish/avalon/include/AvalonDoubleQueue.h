/**
 *@author	 
 *@email	 
 *@date		2011-02-27	
 *@brief	双队列，用于线程间通信
 */
#ifndef _AVALON_DOUBLEQUEUE_H_
#define _AVALON_DOUBLEQUEUE_H_

#include <AvalonCircularQueue.h>
#include <AvalonMutex.h>

namespace Avalon
{
	/**
	 *@brief 双队列
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
		 *@brief 判断是否为空
		 */
		bool empty()
		{
			if(m_pOutputQueue->empty())
			{
				AVALON_AUTOMUTEX(m_Mutex);
				std::swap(m_pInputQueue, m_pOutputQueue); //noted by aprilliu, 需要交互输入和输出队列时 则必须加锁
			}
			return m_pOutputQueue->empty();
		}

		/**
		 *@brief 存入一个元素   noted by aprilliu, 如果往队列中进行push的线程只有一个，则可以不加锁
		 */
		void push(T value)
		{
			m_Mutex.Acquire();
			m_pInputQueue->push(value);
			m_Mutex.Release();
		}

		/**
		 *@brief 取出一个元素
		 */
		T pop()
		{
			if(m_pOutputQueue->empty())
			{
				AVALON_AUTOMUTEX(m_Mutex);
				std::swap(m_pInputQueue, m_pOutputQueue); //noted by aprilliu, 需要交互输入和输出队列时 则必须加锁
			}

			return m_pOutputQueue->pop();
		}

	private:
		//输入队列
		Queue		*m_pInputQueue;
		//输出队列
		Queue		*m_pOutputQueue;
		//互斥锁
		Avalon::Mutex	m_Mutex; 
	};

}

#endif
