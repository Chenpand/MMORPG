#include "AvalonThreadPool.h"
#include <cassert>

namespace Avalon
{
	ThreadPool::ThreadPool()
	{
		m_bRun = false;
	}

	ThreadPool::~ThreadPool()
	{
	}

    // 启动线程池
	bool ThreadPool::Open(UInt32 threadNum)
	{
		m_bRun = true;
		for(UInt32 i = 0;i < threadNum;++i)
		{
			WorkerThread* thread = CreateWorkerThread();  // 创建work线程
			if (!thread->Init())
			{
				return false;
			}

			if(!thread->Activate())             // 启动work线程
			{
				delete thread;
				continue;
			}

			m_ThreadList.push_back(thread);     // 放入线程队列中
		}
		m_bRun = (m_ThreadList.size() > 0);
		return m_bRun;
	}

    // 停止线程池，等待线程退出
	void ThreadPool::Shutdown()
	{
		m_bRun = false;
		for(ThreadList::iterator iter = m_ThreadList.begin();
			iter != m_ThreadList.end(); ++iter)
		{
			(*iter)->Join();
			delete (*iter);
		}
		m_ThreadList.clear();

		assert(m_TaskQueue.size() == 0);
	}

    // 向任务队列提交任务
	bool ThreadPool::PostTask(Task* task)
	{
		if(NULL == task || !m_bRun) return false;

		m_Mutex.Acquire();
		m_TaskQueue.push(task);     // 加锁 向任务队列添加任务
		m_Mutex.Release();
		m_Cond.Signal();
		return true;
	}

    // 取任务
	Task* ThreadPool::GetTask()
	{
		Task* task = NULL;
		m_Mutex.Acquire();
		while(m_TaskQueue.empty())
		{
			if(!m_bRun)
			{
				m_Mutex.Release();
				return NULL;
			}
			m_Cond.Wait(m_Mutex);
		}
		task = m_TaskQueue.front();
		m_TaskQueue.pop();
		m_Mutex.Release();
		return task;
	}

    // 工厂函数，创建 WorkerThread 实例
	WorkerThread* ThreadPool::CreateWorkerThread()
	{
		return new WorkerThread(this);
	}
}
