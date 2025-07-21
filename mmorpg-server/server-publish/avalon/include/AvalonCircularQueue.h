/**
 *@author	 
 *@email	 
 *@date		2011-02-25	
 *@brief	���ζ���
 */
#ifndef _AVALON_CIRCULARQUEUE_H_
#define _AVALON_CIRCULARQUEUE_H_

#include <AvalonDefine.h>

namespace Avalon
{
	/**
	 *@brief ���ζ���ģ����
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
		 *@brief ��ȡԪ�ظ���
		 */
		size_t size() const{ return m_Num; }

		/**
		 *@brief �ж��Ƿ�Ϊ��
		 */
		bool empty() const { return 0 == m_Num; }

		/**
		 *@brief �Ÿ�һ��Ԫ��
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
		 *@brief ��ȡһ��Ԫ��
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
		 *@brief ȷ���㹻�ռ�
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
		//����
		T*		m_pBuffer;
		//���д�С
		size_t	m_Size;
		//����ͷ
		size_t	m_Front;
		//����β
		size_t	m_Tail;
		//Ԫ�ظ���
		size_t	m_Num;
	};
}

#endif
