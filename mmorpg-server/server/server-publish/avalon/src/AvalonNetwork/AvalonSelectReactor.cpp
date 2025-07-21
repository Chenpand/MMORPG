#include "AvalonSelectReactor.h"
#include "AvalonSocket.h"

namespace Avalon
{
	SelectReactor::SelectReactor()
	{
		m_MaxNumOfHandlers = 0;
		m_TickInter = 0;
		m_bOpened = false;
		m_bRemoveFlag = false;

		m_InputTime = 0;
	}

	SelectReactor::~SelectReactor()
	{
		Close();
	}

	bool SelectReactor::Open(UInt32 maxNumOfHandles, UInt32 tickInter)
	{
		if(m_bOpened) return true;

		m_MaxNumOfHandlers = maxNumOfHandles;
		m_TickInter = tickInter;
		
		FD_ZERO(&m_ReadSet);
		FD_ZERO(&m_WriteSet);
		FD_ZERO(&m_ExceptSet);

		m_Handlers.clear();
		m_bRemoveFlag = false;

		m_bOpened = true;
		m_InputTime = 0;
		return true;
	}

	void SelectReactor::Close()
	{
		if(!m_bOpened) return;
		m_bOpened = false;

		m_MaxNumOfHandlers = 0;
		m_TickInter = 0;

		for(HANDLER_MAP::iterator iter = m_Handlers.begin();
			iter != m_Handlers.end();++iter)
		{
			iter->second->HandleClose();
		}
		m_Handlers.clear();
	}

	bool SelectReactor::RegisterHandler(INetEventHandler *handler, UInt32 events)
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
			 if(m_Handlers.size() >= m_MaxNumOfHandlers || handler->GetReactor() != NULL)
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

		 if(events & NETEVENT_IN)
		 {
			 FD_SET(sock,&m_ReadSet);
		 }
		 if(events & NETEVENT_OUT)
		 {
			 FD_SET(sock,&m_WriteSet);
		 }
		 if(events & NETEVENT_EXCEPTION)
		 {
			 FD_SET(sock,&m_ExceptSet);
		 }
		 
		 handler->SetEvents(oldevents | events);

		 return true;
	}

	void SelectReactor::RemoveHandler(INetEventHandler *handler, UInt32 events)
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
		 
		 if(events & NETEVENT_IN)
		 {
			 FD_CLR(sock,&m_ReadSet);
		 }
		 if(events & NETEVENT_OUT)
		 {
			 FD_CLR(sock,&m_WriteSet);
		 }
		 if(events & NETEVENT_EXCEPTION)
		 {
			 FD_CLR(sock,&m_ExceptSet);
		 }

		 handler->SetEvents(oldevents & ~events);

		 if(handler->GetEvents() == 0)
		 {
			 handler->SetReactor(NULL);
			 m_bRemoveFlag = true;
			 handler->HandleClose();
			 m_Handlers.erase(iter);
		 }
	}

	UInt32 SelectReactor::GetHandlerNum() const
	{
		return m_Handlers.size();
	}

	bool SelectReactor::HandleEvents(const Time& now)
	{
		if(!m_bOpened || m_Handlers.empty()) return false;

		timeval val;
		val.tv_sec = 0;
		val.tv_usec = 0;

		sock_t maxsock = (--m_Handlers.end())->first;

		fd_set readset = m_ReadSet;
		fd_set writeset = m_WriteSet;
		fd_set exceptset = m_ExceptSet;

		while(true)
		{
			int ret = select(maxsock+1,&readset,&writeset,&exceptset,&val);
			if(AVALON_SOCK_ERROR == ret)
			{
				if(AVALON_EINTR == Avalon::Socket::GetError()) continue;
				else
				{
					NETLOG_ERROR("error %d happend when select!",Avalon::Socket::GetError());
					return false;
				}
			}
			break;
		}

		bool ret = false;
		for(HANDLER_MAP::iterator nextIter = m_Handlers.begin(),iter;
			nextIter != m_Handlers.end();)
		{
			iter = nextIter++;

			INetEventHandler* handler = iter->second;

			m_bRemoveFlag = false;
			if(!m_bRemoveFlag && ( handler->GetEvents() & NETEVENT_IN ) && FD_ISSET(iter->first,&readset))
			{
				m_InputTime = now.MSec();
				handler->HandleInput();
				ret = true;
			}
			if(!m_bRemoveFlag && ( handler->GetEvents() & NETEVENT_OUT ) && FD_ISSET(iter->first,&writeset))
			{
				handler->HandleOutput();
				ret = true;
			}
			if(!m_bRemoveFlag && ( handler->GetEvents() & NETEVENT_EXCEPTION) && FD_ISSET(iter->first,&exceptset) )
			{
				handler->HandleException();
				ret = true;
			}
			if(!m_bRemoveFlag && ( handler->GetEvents() & NETEVENT_TICK ))
			{
				if(now.MSec() >= handler->GetTimeout())
				{
					UInt64 timeout = handler->GetTimeout() + ((now.MSec() - handler->GetTimeout()) / m_TickInter + 1) * m_TickInter;
					handler->SetTimeout(timeout);
					handler->HandleTick(now);
				}
			}
		}
		return ret;
	}
}
