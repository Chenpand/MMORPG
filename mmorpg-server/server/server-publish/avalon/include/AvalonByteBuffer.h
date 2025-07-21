/**
 *@author	黄列
 *@date		2011-03-12	
 *@brief	字节缓冲
 */
#ifndef _AVALON_BYTEBUFFER_H_
#define _AVALON_BYTEBUFFER_H_

#include "AvalonDefine.h"

namespace Avalon
{
	/**
	 *@brief 字节缓冲，是一个固定大小的环形队列
	 */
	template<size_t MaxSize>
	class ByteBuffer
	{
		const static size_t BUF_SIZE = MaxSize + 1;

	public:
		ByteBuffer():m_ReadPos(0),m_WritePos(0){}
		~ByteBuffer(){}

		/**
		 *@brief 写入字节，只能全部写入或不写入
		 *@return 写入成功返回true，缓冲区满返回false
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
		 *@brief 递增读位置
		 */
		void writeforward(size_t size)
		{
			m_WritePos = (m_WritePos + size) % BUF_SIZE;
		}

		/**
		 *@brief 读取字节，只能读取指定数量字节或不读取
		 *@return 读取成功返回true，缓冲中字节不足返回false
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
		 *@brief 清空缓冲数据
		 */
		void clear()
		{
			m_ReadPos = m_WritePos = 0;
		}

		/**
		 *@brief 获取缓冲数据大小
		 */
		size_t size() const
		{
			return ( m_WritePos + BUF_SIZE - m_ReadPos ) % BUF_SIZE ;
		}

		/**
		 *@brief 获取剩余容量
		 */
		size_t capacity() const
		{
			return MaxSize - size();
		}

		/**
		 *@brief 判断是否为空
		 */
		bool empty() const
		{
			return size() == 0;
		}


	private:
		//字节数组
		UInt8 m_Buffer[BUF_SIZE];
		//当前读取位置
		size_t m_ReadPos;
		//当前写入位置
		size_t m_WritePos;
	};
}

#endif