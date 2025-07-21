/**
 *@author	 
 *@email	 
 *@date		2011-02-26		
 *@brief	�򵥶�ʱ��
 */
#ifndef _AVALON_SIMPLE_TIMER_H_
#define _AVALON_SIMPLE_TIMER_H_

#include "AvalonTime.h"

namespace Avalon
{

	/**
	 *@brief ��ʱ��
	 */
	class SimpleTimer
	{
	public:
		SimpleTimer():m_Interval(0),m_LastTime(0){}
		~SimpleTimer(){}

		/**
		 *@brief ���û�ȡ��ʱ���
		 *@param interval ��ʱ���������
		 */
		void SetInterval(UInt64 interval){ m_Interval = interval; }
		UInt64 GetInterval(){ return m_Interval; }

		/**
		 *@brief �����ϴδ���ʱ��
		 */
		void SetLastTime(UInt64 time){ m_LastTime = time; }
		UInt64 GetLastTime() const{ return m_LastTime; }

		/**
		 *@brief ���ö�ʱ��
		 */
		void Reset()
		{
			m_Interval = 0;
			m_LastTime = 0;
		}

		/**
		 *@brief �ж�ʱ���Ƿ�
		 */
		bool IsTimeout(const Avalon::Time& now)
		{
			if(m_LastTime == 0)
			{
				m_LastTime = now.MSec();
				return false;
			}
			else if(now.MSec() > m_LastTime + m_Interval)
			{
				m_LastTime = now.MSec();
				return true;
			}
			return false;
		}

		/**
		 *@brief ��ȡ��ʧʱ��
		 */
		UInt64 GetEscapeTime(const Avalon::Time& now) const
		{
			if(m_LastTime == 0 || now.MSec() < m_LastTime)
			{
				return 0;
			}
			return now.MSec() - m_LastTime;
		}

	private:
		//��ʱ���
		UInt64 m_Interval;
		//��ʱʱ��
		UInt64 m_LastTime;
	};
};

#endif
