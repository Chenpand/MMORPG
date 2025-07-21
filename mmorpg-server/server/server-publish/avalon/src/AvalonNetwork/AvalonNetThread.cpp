#include "AvalonNetThread.h"
#include "AvalonINetEventHandler.h"
#include "AvalonSelectReactor.h"
#include "AvalonEpollReactor.h"

namespace Avalon
{
	NetThread::NetThread(UInt32 maxHandler)
	{
#if defined(AVALON_OS_WINDOWS)
		m_pReactor = new SelectReactor();
#else
		m_pReactor = new EpollReactor();
#endif
		m_MaxHandlerNum = maxHandler;
	}

	NetThread::~NetThread()
	{
		m_Mutex.Acquire();
		for(HANDLER_MAP::iterator iter = m_WaitQueue.begin();
			iter != m_WaitQueue.end();++iter)
		{
			iter->first->HandleClose();
		}
		m_WaitQueue.clear();
		m_Mutex.Release();

		delete m_pReactor;
	}

	void NetThread::RegisterHandler(INetEventHandler* handler,UInt32 events)
	{
		if(NULL == handler) return;

		m_Mutex.Acquire();
		m_WaitQueue.insert(std::make_pair(handler,events));   //m_WaitQueue 可以被监听线程以及IO线程所修改 所以要进行互斥操作。具体当监听线程接受到一个连接后，会把新连接注册到一个IO线程中
		m_Mutex.Release();
	}

	UInt32 NetThread::GetHandlerNum() const
	{
		return m_pReactor->GetHandlerNum();
	}

	void NetThread::Stop(UInt32 inputTimeout)
	{
		int	waitTimes = 12000; //最长等待1分钟
		while(Avalon::Time::CurrentTime().MSec() < m_pReactor->GetInputTime() + inputTimeout
			&& waitTimes-- > 0)
		{
			Thread::Sleep(5);
		}

		EndProcess();
	}

	bool NetThread::OnInit()
	{
		if(!m_pReactor->Open(m_MaxHandlerNum,1000))
		{
			return false;
		}
		return true;
	}

	void NetThread::OnTick(const Time& now)
	{
		HANDLER_MAP tempList;
		m_Mutex.Acquire();
		tempList = m_WaitQueue;
		m_WaitQueue.clear();
		m_Mutex.Release();

		for(HANDLER_MAP::iterator iter = tempList.begin();
			iter != tempList.end();++iter)
		{
			if(!m_pReactor->RegisterHandler(iter->first,iter->second))
			{
				iter->first->HandleClose();
			}
		}
		tempList.clear();

		int times = 200;
		while(times -- > 0 && m_pReactor->HandleEvents(now)) ;
	}

	void NetThread::OnQuit()
	{
		m_pReactor->Close();
	}

	void NetThread::Run()
	{
		BeginProcess();
	}



	NetThreadGroup::NetThreadGroup(UInt32 threadNum,UInt32 handlerPerThread)
	{
		for(UInt32 i = 0;i < threadNum;++i)
		{
			NetThread* thread = new NetThread(handlerPerThread);
			thread->SetInterval(Avalon::Time(2));
			m_Threads.push_back(thread);
		}
	}

	NetThreadGroup::~NetThreadGroup()
	{
		for(NETTHREAD_VEC::iterator iter = m_Threads.begin();iter != m_Threads.end();++iter)
		{
			delete *iter;
		}
		m_Threads.clear();
	}

	bool NetThreadGroup::Activate()
	{
		for(NETTHREAD_VEC::iterator iter = m_Threads.begin();iter != m_Threads.end();++iter)
		{
			if(!(*iter)->Activate())
			{
				Stop(0);
				return false;
			}
		}
		return true;
	}

	void NetThreadGroup::Stop(UInt32 inputTimeout)
	{
		for(NETTHREAD_VEC::iterator iter = m_Threads.begin();iter != m_Threads.end();++iter)
		{
			(*iter)->Stop(inputTimeout);
		}
	}

	void NetThreadGroup::Join()
	{
		for(NETTHREAD_VEC::iterator iter = m_Threads.begin();iter != m_Threads.end();++iter)
		{
			(*iter)->Join();
		}
	}

	NetThread* NetThreadGroup::GetMinHandlerThread()
	{
		NetThread* thread = NULL;
		UInt32 handlerNum = UInt32(-1);

		for(NETTHREAD_VEC::iterator iter = m_Threads.begin();iter != m_Threads.end();++iter)
		{
			UInt32 num = (*iter)->GetHandlerNum();
			if(num < handlerNum)
			{
				handlerNum = num;
				thread = *iter;
			}
		}
		return thread;
	}
}
