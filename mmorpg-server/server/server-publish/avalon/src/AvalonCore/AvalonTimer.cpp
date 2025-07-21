#include "AvalonTimer.h"
#include <vector>

namespace Avalon
{

	TimerReactor::TimerReactor()
	{
		m_Now = Avalon::Time::CurrentTime();
	}
	
	TimerReactor::~TimerReactor()
	{
		for(HANDLER_SET::iterator iter = m_Handlers.begin();iter != m_Handlers.end();++iter)
		{
			(*iter)->Release();
		}
		m_Handlers.clear();
	}

	void TimerReactor::Schedule(TimerHandler* handler,UInt64 interval)
	{
		if(handler == NULL) return;

		handler->m_Interval = interval;
		handler->m_Timeout  = m_Now.MSec() + interval;
		handler->m_Type = TIMER_REPEAT;
		m_Handlers.insert(handler);
	}

	void TimerReactor::ScheduleAt(Avalon::TimerHandler *handler, const Avalon::Time &time, Avalon::UInt64 interval)
	{
		if(handler == NULL || interval == 0) return;

		handler->m_Interval = interval;
		if(m_Now.MSec() > time.MSec())
		{
			handler->m_Timeout = time.MSec() + ((m_Now.MSec() - time.MSec() - 1) / interval + 1) * interval;
		}
		else
		{
			handler->m_Timeout = time.MSec();
		}
		handler->m_Type = TIMER_REPEAT;
		m_Handlers.insert(handler);
	}

	void TimerReactor::ScheduleOnce(Avalon::TimerHandler *handler, Avalon::UInt64 interval)
	{
		if(handler == NULL) return;

		handler->m_Interval = interval;
		handler->m_Timeout = m_Now.MSec() + interval;
		handler->m_Type = TIMER_ONCE;
		m_Handlers.insert(handler);
	}

	void TimerReactor::ScheduleAt(Avalon::TimerHandler *handler, const Avalon::Time& time)
	{
		if(handler == NULL) return;

		handler->m_Interval = 0;
		handler->m_Timeout = time.MSec();
		handler->m_Type = TIMER_ONCE;
		m_Handlers.insert(handler);
	}

	void TimerReactor::RemoveHandler(Avalon::TimerHandler *handler)
	{
		if(handler == NULL) return;

		HANDLER_SET::iterator lowerIter = m_Handlers.lower_bound(handler);
		HANDLER_SET::iterator upperIter = m_Handlers.upper_bound(handler);

		for(HANDLER_SET::iterator iter = lowerIter;iter != upperIter;++iter)
		{
			if(*iter == handler)
			{
				m_Handlers.erase(iter);
				handler->Release();
				break;
			}
		}
	}

	void TimerReactor::HandleEvent(const Avalon::Time &now)
	{
		m_Now = now;

		std::vector<TimerHandler*> handlerList;

		for(HANDLER_SET::iterator iter = m_Handlers.begin();iter != m_Handlers.end();)
		{
			if(m_Now.MSec() < (*iter)->GetTimeout()) break;

			TimerHandler* handler = *iter;
			m_Handlers.erase(iter++);

			handler->OnTimeout(m_Now);

			if(handler->m_Type == TIMER_ONCE)
			{
				handler->Release();
			}
			else
			{
				handler->m_Timeout = m_Now.MSec() + handler->m_Interval;
				handlerList.push_back(handler);
			}
		}

		m_Handlers.insert(handlerList.begin(), handlerList.end());
	}
}

