/**
*@author
*@email
*@date		2011-03-03
*@brief	工作者线程
*/
#ifndef _AVALON_WORKERTHREAD_H_
#define _AVALON_WORKERTHREAD_H_

#include "AvalonThread.h"

namespace Avalon
{
	class ThreadPool;

	/**
	*@brief 工作者线程类
	*/
	class WorkerThread : public Thread
	{
	public:
		explicit WorkerThread(ThreadPool* poll);
		virtual ~WorkerThread();

		/**
		*@brief 初始化接口，子类实现
		*/
		virtual bool Init();

		virtual void Run();

	private:
		//线程池
		ThreadPool* m_pPool;
	};
}

#endif
