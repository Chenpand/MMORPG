#include "AvalonEpollReactor.h"
#include <algorithm>
#include <assert.h>
#include "AvalonSocket.h"

namespace Avalon
{
	EpollReactor::EpollReactor()
	{
		m_epollFd = -1;
		m_MaxNumOfHandlers = 0;
		m_TickInter = 0;
		m_bOpened = false;
		m_bRemoveFlag = false;

		m_InputTime = 0;
	}

	EpollReactor::~EpollReactor()
	{
		Close();
	}

	bool EpollReactor::Open(UInt32 maxNumOfHandles, UInt32 tickInter)
	{
		if(m_bOpened) return true;

#if !defined(AVALON_OS_WINDOWS)
		m_epollFd = epoll_create(maxNumOfHandles + 50);
		if(m_epollFd < 0) return false;
#endif

		m_MaxNumOfHandlers = maxNumOfHandles;
		m_TickInter = tickInter;

		m_TimeoutList.clear();
		m_Handlers.clear();

		m_bRemoveFlag = false;
		m_bOpened = true;

		m_InputTime = 0;
		return true;
	}

	void EpollReactor::Close()
	{
		if(!m_bOpened) return;
		m_bOpened = false;

		m_MaxNumOfHandlers = 0;
		m_TickInter = 0;

		m_TimeoutList.clear();
		for(HANDLER_MAP::iterator iter = m_Handlers.begin();
			iter != m_Handlers.end();++iter)
		{
			iter->second->HandleClose();
		}
		m_Handlers.clear();

#if !defined(AVALON_OS_WINDOWS)
		close(m_epollFd);
#endif
		m_epollFd = -1;
	}

	bool EpollReactor::RegisterHandler(INetEventHandler* handler,UInt32 events)
	{
		events &= NETEVENT_ALL;
		if(NULL == handler || 0 == events || !m_bOpened)
		{
			return false;
		}

		sock_t sock = handler->GetHandle();
		if(AVALON_INVALID_SOCK == sock)
		{
			return false;
		}

		HANDLER_MAP::iterator iter = m_Handlers.find(sock);
		if(iter == m_Handlers.end())
		{
			if(m_Handlers.size() >= m_MaxNumOfHandlers || handler->GetReactor() != NULL) //noted by aprilliu, ���������������������ƣ���رո�����
			{
				return false;
			}

			handler->SetTimeout(0);
			handler->SetEvents(0);
			handler->SetReactor(this);
			m_Handlers.insert(std::make_pair(sock,handler));
		}

		UInt32 oldevents = handler->GetEvents();
		events &= ~oldevents;

		handler->SetEvents(events | oldevents);

		if(events & NETEVENT_IO) //���������¼�
		{
#if !defined(AVALON_OS_WINDOWS)
			struct epoll_event epollEvent;
			epollEvent.data.ptr = handler;
			epollEvent.events = MaskToEpollEvents(handler->GetEvents());
			if(oldevents & NETEVENT_IO) //ԭ���������¼����޸�
			{
				assert(epoll_ctl(m_epollFd, EPOLL_CTL_MOD, sock, &epollEvent) == 0);
			}
			else
			{
				assert(epoll_ctl(m_epollFd, EPOLL_CTL_ADD, sock, &epollEvent) == 0);
			}
#endif
		}
		if(events & NETEVENT_TICK)
		{
			m_TimeoutList.push_back(handler);    //noted by aprilliu, ��push��vector�У�Ȼ����ʹ��push_heap���뵽����
			HandlerTimeoutCmp cmp;
			std::push_heap(m_TimeoutList.begin(), m_TimeoutList.end(), cmp);  //notedy by aprilliu. ��Ϊm_TimeoutList�Ǵӿ����鿪ʼpush_backԪ�صģ�ÿ��push�󶼻����push_heap����֤�˶ѵ����ԣ� 
																			  //�Ӷ��������ȵ���make_heap������һ���ѣ����vector��ԭ�������ݣ������ȵ���make_heap ����һ���ѣ�Ȼ����ܼ���������push���ݣ�
		}

		return true;
	}

	void EpollReactor::RemoveHandler(INetEventHandler* handler,UInt32 events)
	{
		events &= NETEVENT_ALL;
		if(NULL == handler || 0 == events || !m_bOpened)
		{
			return;
		}

		sock_t sock = handler->GetHandle();
		if(AVALON_INVALID_SOCK == sock)
		{
			return;
		}

		HANDLER_MAP::iterator iter = m_Handlers.find(sock);
		if(iter == m_Handlers.end())
		{
			return;
		}

		UInt32 oldevents = handler->GetEvents();
		events &= oldevents;

		handler->SetEvents(oldevents & ~events);

		if(events & NETEVENT_IO)
		{
#if !defined(AVALON_OS_WINDOWS)
			struct epoll_event epollEvent;
			epollEvent.data.ptr = handler;
			epollEvent.events = MaskToEpollEvents(handler->GetEvents());
			if(handler->GetEvents() & NETEVENT_IO) //���������¼����޸�
			{
				assert(epoll_ctl(m_epollFd, EPOLL_CTL_MOD, sock, &epollEvent) == 0);
			}
			else
			{
				assert(epoll_ctl(m_epollFd, EPOLL_CTL_DEL, sock, &epollEvent) == 0);
			}
#endif
		}
		if(events & NETEVENT_TICK) //ɾ��tick�¼�
		{
			for(HANDLER_VEC::iterator iter = m_TimeoutList.begin();
				iter != m_TimeoutList.end(); ++iter)
			{
				if((*iter) == handler)
				{
					m_TimeoutList.erase(iter);
					break;
				}
			}
			HandlerTimeoutCmp cmp;
			std::make_heap(m_TimeoutList.begin(), m_TimeoutList.end(), cmp); //noted by aprilliu, ����m_TimeoutList����ɾ����Ԫ�أ�����ʹ��make_heapʹ�����¹����ɶ�
		}

		if(handler->GetEvents() == 0)
		{
			handler->SetReactor(NULL);
			m_bRemoveFlag = true;
			handler->HandleClose();
			m_Handlers.erase(iter);
		}
	}

	UInt32 EpollReactor::GetHandlerNum() const
	{
		return m_Handlers.size();
	}

	bool EpollReactor::HandleEvents(const Time& now)
	{
		if(!m_bOpened || m_Handlers.empty()) return false;

		while(!m_TimeoutList.empty())
		{
			INetEventHandler* handler = *(m_TimeoutList.begin());

			if(now.MSec() >= handler->GetTimeout())
			{
				HandlerTimeoutCmp cmp;
				std::pop_heap(m_TimeoutList.begin(), m_TimeoutList.end(), cmp);
				UInt64 timeout = handler->GetTimeout() + ((now.MSec() - handler->GetTimeout()) / m_TickInter + 1) * m_TickInter;
				handler->SetTimeout(timeout);
				handler->HandleTick(now);
				std::push_heap(m_TimeoutList.begin(), m_TimeoutList.end(), cmp);
			}
			else break;
		}

#if !defined(AVALON_OS_WINDOWS)
		int eventNum = 0;
		while(true)
		{
			eventNum = epoll_wait(m_epollFd, (struct epoll_event*)&m_Events, MAX_WAIT_EVENTS_NUM, 0);
			if(AVALON_SOCK_ERROR == eventNum)
			{
				if(AVALON_EINTR == Avalon::Socket::GetError()) continue;
				else
				{
					NETLOG_ERROR("error %d happend when epoll!",Avalon::Socket::GetError());
					return false;
				}
			}
			break;
		}
#endif

		bool ret = false;
#if !defined(AVALON_OS_WINDOWS)
		for(int i = 0; i < eventNum; ++i)
		{
			INetEventHandler* handler = (INetEventHandler*)m_Events[i].data.ptr;
			UInt32 events = m_Events[i].events;

			m_bRemoveFlag = false;
			if(!m_bRemoveFlag && ( handler->GetEvents() & NETEVENT_IN ) && ( events & EPOLLIN))
			{
				m_InputTime = now.MSec();
				handler->HandleInput();
				ret = true;
			}
			if(!m_bRemoveFlag && ( handler->GetEvents() & NETEVENT_OUT ) && ( events & EPOLLOUT))
			{
				handler->HandleOutput();
				ret = true;
			}
			if(!m_bRemoveFlag && ( handler->GetEvents() & NETEVENT_EXCEPTION) && ( events & EPOLLERR || events & EPOLLHUP || events & EPOLLPRI) )
			{
				handler->HandleException();
				ret = true;
			}
		}
#endif

		return ret;
	}

	UInt32 EpollReactor::MaskToEpollEvents(UInt32 events)
	{
		UInt32 epollEvents = 0;
#if !defined(AVALON_OS_WINDOWS)
		if(events & NETEVENT_IN) epollEvents |= EPOLLIN;
		if(events & NETEVENT_OUT) epollEvents |= EPOLLOUT;
		if(events & NETEVENT_EXCEPTION) epollEvents |= EPOLLPRI;
#endif
		return epollEvents;
	}
}
