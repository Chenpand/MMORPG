/**
 *@author	 
 *@email	 
 *@date		2011-04-19		
 *@brief	id生成器
 */
#ifndef _AVALON_IDGENERATOR_H_
#define _AVALON_IDGENERATOR_H_

#include "AvalonDefine.h"
#include "AvalonCircularQueue.h"

namespace Avalon
{
	/**
	 *@brief id生成器
	 */
	class IDGenerator 
	{
		typedef CircularQueue<UInt32,256> Queue;

	public:
		IDGenerator()
		{
			m_Min = m_Max = 0;
			m_Seed = m_Min;
		}

		~IDGenerator()
		{
		}

		void Init(UInt32 min,UInt32 max)
		{
			m_Min = min;
			m_Max = max;
			m_Seed = m_Min;
		}

		/**
		 *@brief 生成一个id
		 */
		UInt32 Generate()
		{
			if(!m_FreeList.empty())
			{
				return m_FreeList.pop();
			}

			UInt32 ret = m_Seed;
			if(++m_Seed > m_Max)
			{
				m_Seed = m_Min;
			}
			return ret;
		}

		/**
		 *@brief 释放一个id
		 */
		void Release(UInt32 id)
		{
			m_FreeList.push(id);
		}

	private:
		//最小值
		UInt32	m_Min;
		//最大值
		UInt32	m_Max;
		//种子
		UInt32	m_Seed;
		//空闲列表
		Queue	m_FreeList;
	};
}

#endif
