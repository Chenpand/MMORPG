#include "AvalonThread.h"

namespace Avalon
{
	Thread::Thread()
	{
		m_Thread = AVALON_INVALID_THREAD;
	}

	Thread::~Thread()
	{
	}

	AVALON_THREADFUNC_RET Thread::Function(void* arg)
	{
		Thread* pThread = (Thread*)arg;
		pThread->m_Sem.Post();
		pThread->Run();
		pThread->m_Thread = AVALON_INVALID_THREAD;
		thread_exit();
		return AVALON_THREADFUNC_VAL;
	}

	bool Thread::Activate()
	{
		if(thread_start(m_Thread,Function,this))
		{
			m_Sem.Wait();
			return true;
		}
		return false;
	}

	void Thread::Stop()
	{
		if(m_Thread != AVALON_INVALID_THREAD)
		{
			thread_terminate(m_Thread);
			m_Thread = AVALON_INVALID_THREAD;
		}
	}

	void Thread::Join()
	{
		if(m_Thread != AVALON_INVALID_THREAD)
		{
			thread_join(m_Thread);
			m_Thread = AVALON_INVALID_THREAD;
		}
	}

	void Thread::Sleep(UInt32 msecs)
	{
		thread_sleep(msecs);
	}
}
