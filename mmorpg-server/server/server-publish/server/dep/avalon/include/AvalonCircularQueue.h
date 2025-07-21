/**
 *@author	 
 *@email	 
 *@date		2011-02-25	
 *@brief	环形队列
 */
#ifndef _AVALON_CIRCULARQUEUE_H_
#define _AVALON_CIRCULARQUEUE_H_

#include <AvalonDefine.h>

namespace Avalon
{
	/**
	 *@brief 环形队列模板类
	 */
	template<typename T,size_t InitSize = 32>
	class CircularQueue
	{
		const static size_t INIT_SIZE = (InitSize / 8 + 1) * 8;

	public:
		CircularQueue()
		{
			m_pBuffer = new T[INIT_SIZE];
			m_Size = INIT_SIZE;
			m_Front = m_Tail = 0;
			m_Num = 0;
		}
		~CircularQueue()
		{
			delete[] m_pBuffer;
		}

	public:
		/**
		 *@brief 获取元素个数
		 */
		size_t size() const{ return m_Num; }

		/**
		 *@brief 判断是否为空
		 */
		bool empty() const { return 0 == m_Num; }

		/**
		 *@brief 放个一个元素
		 */
		void push(T value)
		{
			reserve();
			m_pBuffer[m_Front] = value;
			++m_Front;
			m_Front = m_Front % m_Size;
			++m_Num;
		}

		/**
		 *@brief 获取一个元素
		 */
		T pop()
		{
			if(empty()){ return T(); }

			T* ptr = &m_pBuffer[m_Tail];
			++m_Tail;
			m_Tail = m_Tail % m_Size;
			--m_Num;
			return *ptr;
		}

	private:
		/**
		 *@brief 确保足够空间
		 */
		void reserve()
		{
			if(m_Num == m_Size)
			{
				size_t newsize = m_Size + m_Size / 2;
				T*	newbuffer = new T[newsize];

				if(m_Front > m_Tail)
				{
					memmove(newbuffer,m_pBuffer + m_Tail,m_Num * sizeof(T));
				}
				else
				{
					memmove(newbuffer,m_pBuffer + m_Tail,(m_Size - m_Tail) * sizeof(T));
					memmove(newbuffer + m_Size - m_Tail,m_pBuffer,m_Front * sizeof(T));
				}

				delete[] m_pBuffer;
				m_pBuffer = newbuffer;
				m_Size = newsize;
				m_Tail = 0;
				m_Front = m_Num;
			}
		}

	private:
		//数组
		T*		m_pBuffer;
		//队列大小
		size_t	m_Size;
		//队列头
		size_t	m_Front;
		//队列尾
		size_t	m_Tail;
		//元素个数
		size_t	m_Num;
	};
}

#endif
