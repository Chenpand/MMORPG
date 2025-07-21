/**
 *@author	 
 *@email	 
 *@date		2011-03-12		
 *@brief	��ʱ��
 */
#ifndef _AVALON_TIMER_H_
#define _AVALON_TIMER_H_

#include "AvalonTime.h"
#include "AvalonSingleton.h"
#include <set>

namespace Avalon
{
	enum TimerType
	{
		TIMER_ONCE,		//һ�δ���
		TIMER_REPEAT,	//���
	};

	/**
	 *@brief ��ʱ������
	 */
	class TimerHandler
	{
		friend class TimerReactor;
	public:
		TimerHandler():m_Type(TIMER_ONCE),m_Timeout(0),m_Interval(0){}
		virtual ~TimerHandler(){}

		/**
		 *@brief ��ʱ�¼�
		 */
		virtual void OnTimeout(const Avalon::Time& now) = 0;

		/**
		 *@brief ����
		 */
		virtual void Release(){ delete this; }

		/**
		 *@brief ��ȡ��ʱʱ��
		 */
		UInt64 GetTimeout() const { return m_Timeout; }

	private:
		//����
		TimerType	m_Type;
		//��ʱʱ��
		UInt64		m_Timeout;
		//��ʱ���
		UInt64		m_Interval;
	};

	class TimerLess : public std::less<TimerHandler*>
	{
	public:
		bool operator()(const TimerHandler* left,const TimerHandler* right) const
		{
			return left->GetTimeout() < right->GetTimeout();
		}
	};

	/**
	 *@brief ��ʱ��Ӧ��
	 */
	class TimerReactor : public Avalon::Singleton<TimerReactor>
	{
		typedef std::multiset<TimerHandler*,TimerLess> HANDLER_SET;
	public:
		TimerReactor();
		~TimerReactor();

	public:
		/**
		 *@brief ����һ��
		 */
		void ScheduleOnce(TimerHandler* handler,UInt64 interval);

		/**
		 *@brief ����һ��
		 */
		void ScheduleAt(TimerHandler* handler,const Avalon::Time& time);

		/**
		 *@brief ���ȶ��
		 */
		void ScheduleAt(TimerHandler* handler,const Avalon::Time& time,UInt64 interval);

		/**
		 *@brief ���ȶ��
		 */
		void Schedule(TimerHandler* handler,UInt64 interval);

		/**
		 *@brief �Ƴ�
		 */
		void RemoveHandler(TimerHandler* handler);

		/**
		 *@brief �����¼�
		 */
		void HandleEvent(const Avalon::Time& now);

	private:
		//��ʱ�¼��б�
		HANDLER_SET		m_Handlers;
		//��ǰʱ��
		Avalon::Time	m_Now;
	};
}

#endif
