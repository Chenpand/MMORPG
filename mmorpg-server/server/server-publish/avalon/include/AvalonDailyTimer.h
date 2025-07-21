/**
*@author	zhengjiyue
*@email		
*@date		2016-08-08
*@brief		ÿ�չ̶�ʱ��Ķ�ʱ��
*/
#ifndef _AVALON_DAILY_TIMER_H_
#define _AVALON_DAILY_TIMER_H_

#include "AvalonTime.h"
#include "AvalonSimpleTimer.h"
#include "AvalonDate.h"
#include <iostream>

namespace Avalon
{
	/**
	*@brief ��ʱ����ÿ��̶�һ��ʱ�䣬��ÿ�������6��
	*/
	class DailyTimer
	{
	public:
		DailyTimer(UInt8 hour = 0, UInt8 minute = 0, UInt8 second = 0)
		{
			SetTime(hour, minute, second);
		}

		void SetTime(UInt8 hour, UInt8 minute = 0, UInt8 second = 0)
		{
			m_Hour = hour;
			m_Min = minute;
			m_Sec = second;

			UInt64 curTime = Avalon::Time::CurrentTime().MSec();
			Date curDate(Avalon::Time::CurrentTime());
			Date timeoutDate(curDate.ToTime());
			timeoutDate.Hour(m_Hour);
			timeoutDate.Min(m_Min);
			timeoutDate.Sec(m_Sec);
			Time timeoutTime = timeoutDate.ToTime();
			if (curTime >= timeoutTime.MSec())
			{
				timeoutTime += Avalon::Time::SECS_OF_DAY * Avalon::Time::MSECS_OF_SEC;
			}

			timeoutDate = Date(timeoutTime);

			m_Timer.Reset();
			m_Timer.SetLastTime(curTime);
			m_Timer.SetInterval(timeoutTime.MSec() - curTime);
		}

		/**
		*@brief �ж�ʱ���Ƿ�
		*/
		bool IsTimeout(const Avalon::Time& now)
		{
			if (m_Timer.IsTimeout(now))
			{
				SetTime(m_Hour, m_Min, m_Sec);
				return true;
			}
			return false;
		}

	private:
		/*
			��ʱʱ��
		*/
		UInt8			m_Hour;
		UInt8			m_Min;
		UInt8			m_Sec;

		// �����Ķ�ʱ��
		SimpleTimer		m_Timer;
	};
};

#endif

