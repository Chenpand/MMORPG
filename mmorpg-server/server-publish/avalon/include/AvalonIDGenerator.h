/**
 *@author	 
 *@email	 
 *@date		2011-04-19		
 *@brief	id������
 */
#ifndef _AVALON_IDGENERATOR_H_
#define _AVALON_IDGENERATOR_H_

#include "AvalonDefine.h"
#include "AvalonCircularQueue.h"

namespace Avalon
{
	/**
	 *@brief id������
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
		 *@brief ����һ��id
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
		 *@brief �ͷ�һ��id
		 */
		void Release(UInt32 id)
		{
			m_FreeList.push(id);
		}

	private:
		//��Сֵ
		UInt32	m_Min;
		//���ֵ
		UInt32	m_Max;
		//����
		UInt32	m_Seed;
		//�����б�
		Queue	m_FreeList;
	};
}

#endif
