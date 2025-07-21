#include "AvalonWorkerThread.h"
#include "AvalonThreadPool.h"

namespace Avalon
{
	WorkerThread::WorkerThread(ThreadPool *pool):m_pPool(pool)
	{
	}

	WorkerThread::~WorkerThread()
	{
	}

	bool WorkerThread::Init()
	{
		return true;
	}

	void WorkerThread::Run()
	{
		Task* task = m_pPool->GetTask();
		while(task != NULL)
		{
			task->Run(this);
			task->Release();
			task = m_pPool->GetTask();
		}
	}
}
