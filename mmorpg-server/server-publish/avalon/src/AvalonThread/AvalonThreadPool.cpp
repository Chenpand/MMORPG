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

    // �����̳߳�
	bool ThreadPool::Open(UInt32 threadNum)
	{
		m_bRun = true;
		for(UInt32 i = 0;i < threadNum;++i)
		{
			WorkerThread* thread = CreateWorkerThread();  // ����work�߳�
			if (!thread->Init())
			{
				return false;
			}

			if(!thread->Activate())             // ����work�߳�
			{
				delete thread;
				continue;
			}

			m_ThreadList.push_back(thread);     // �����̶߳�����
		}
		m_bRun = (m_ThreadList.size() > 0);
		return m_bRun;
	}

    // ֹͣ�̳߳أ��ȴ��߳��˳�
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

    // ����������ύ����
	bool ThreadPool::PostTask(Task* task)
	{
		if(NULL == task || !m_bRun) return false;

		m_Mutex.Acquire();
		m_TaskQueue.push(task);     // ���� ����������������
		m_Mutex.Release();
		m_Cond.Signal();
		return true;
	}

    // ȡ����
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

    // �������������� WorkerThread ʵ��
	WorkerThread* ThreadPool::CreateWorkerThread()
	{
		return new WorkerThread(this);
	}
}
