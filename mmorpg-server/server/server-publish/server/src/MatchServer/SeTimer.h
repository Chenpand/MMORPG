//////////////////////////////////////////////////////////////////////////
//  2012-5-17
//  ¶¨Ê±Æ÷
//  
//
/////////////////////////////////////////////////////////////////////////

#ifndef __TIMER_H__
#define __TIMER_H__

#include <CLDefine.h>

namespace Match {

    class SeTimer
    {
    public:
        SeTimer()
        {
            m_dwInterValue = 0;
            m_dwLastTime = 0;
            m_dwNowTime = 0;
            m_bIsVaild = true;
        }
        ~SeTimer() {}

        void SetInterValue(UInt32 dwTime, UInt64 dwCurTime = 0)
        {
            m_dwInterValue = dwTime;
            m_dwNowTime = m_dwLastTime = dwCurTime;
        }
        int GetInterValue()
        {
            return m_dwInterValue;
        }
        bool IsPassed()
        {
            if (m_dwNowTime - m_dwLastTime >= m_dwInterValue)
            {
                m_dwLastTime = m_dwNowTime;
                return true;
            }

            return false;
        }
        void SetVaild(bool bVaild)
        {
            m_bIsVaild = bVaild;
        }
        bool IsVaild()
        {
            return m_bIsVaild;
        }
        void Reset()
        {
            m_dwLastTime = 0;
        }
        void Update(UInt64 dwTime)
        {
            m_dwNowTime = dwTime;
        }

        UInt64 GetRemainTime()
        {
            if (m_dwNowTime - m_dwLastTime >= m_dwInterValue)
            {
                return 0;
            }

            return m_dwInterValue - (m_dwNowTime - m_dwLastTime);
        }

    public:
        static const int ONE_SECOND = 1000;
        static const int ONE_MINUTE = 60 * ONE_SECOND;
        static const int ONE_HOUR = 60 * ONE_MINUTE;
        static const int ONE_DAY = 24 * ONE_HOUR;

    private:
        UInt32  m_dwInterValue;
        UInt64  m_dwLastTime;
        UInt64  m_dwNowTime;
        bool    m_bIsVaild;
    };
};

#endif
