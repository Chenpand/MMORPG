/**
 *@author	����
 *@date		2011-03-12	
 *@brief	�ֽڻ���
 */
#ifndef _AVALON_BYTEBUFFER_H_
#define _AVALON_BYTEBUFFER_H_

#include "AvalonDefine.h"

namespace Avalon
{
	/**
	 *@brief �ֽڻ��壬��һ���̶���С�Ļ��ζ���
	 */
	template<size_t MaxSize>
	class ByteBuffer
	{
		const static size_t BUF_SIZE = MaxSize + 1;

	public:
		ByteBuffer():m_ReadPos(0),m_WritePos(0){}
		~ByteBuffer(){}

		/**
		 *@brief д���ֽڣ�ֻ��ȫ��д���д��
		 *@return д��ɹ�����true��������������false
		 */
		bool write(const void* buf,size_t size)
		{
			if(capacity() < size) return false;

			size_t len = m_WritePos + size > BUF_SIZE ? ( BUF_SIZE - m_WritePos) : size;
			avalon_memmove(m_Buffer + m_WritePos,buf,len);
			if(len != size)
			{
				avalon_memmove(m_Buffer,(const UInt8*)buf + len,size - len);
			}
			writeforward(size);
			
			return true;
		}

		/**
		 *@brief ������λ��
		 */
		void writeforward(size_t size)
		{
			m_WritePos = (m_WritePos + size) % BUF_SIZE;
		}

		/**
		 *@brief ��ȡ�ֽڣ�ֻ�ܶ�ȡָ�������ֽڻ򲻶�ȡ
		 *@return ��ȡ�ɹ�����true���������ֽڲ��㷵��false
		 */
		bool read(void* buf,size_t size)
		{
			if(size() <  size) return false;

			size_t len = m_ReadPos + size > BUF_SIZE ? (BUF_SIZE - m_ReadPos) : size;
			avalon_memmove(buf,m_ReadPos,len);
			if(len != size)
			{
				avalon_memmove((UInt8*)buf + len,m_Buffer,size - len);
			}
			readforward(size);

			return true;
		}

		void readforward(size_t size)
		{
			m_ReadPos = (m_ReadPos + size) % BUF_SIZE;
		}

		/**
		 *@brief ��ջ�������
		 */
		void clear()
		{
			m_ReadPos = m_WritePos = 0;
		}

		/**
		 *@brief ��ȡ�������ݴ�С
		 */
		size_t size() const
		{
			return ( m_WritePos + BUF_SIZE - m_ReadPos ) % BUF_SIZE ;
		}

		/**
		 *@brief ��ȡʣ������
		 */
		size_t capacity() const
		{
			return MaxSize - size();
		}

		/**
		 *@brief �ж��Ƿ�Ϊ��
		 */
		bool empty() const
		{
			return size() == 0;
		}


	private:
		//�ֽ�����
		UInt8 m_Buffer[BUF_SIZE];
		//��ǰ��ȡλ��
		size_t m_ReadPos;
		//��ǰд��λ��
		size_t m_WritePos;
	};
}

#endif