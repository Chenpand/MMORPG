/**
 *@author	 
 *@email	 
 *@date		2011-03-03	
 *@brief	工作者线程池
 */
#ifndef _AVALON_THREADPOOL_H_
#define _AVALON_THREADPOOL_H_

#include "AvalonWorkerThread.h"
#include "AvalonCondition.h"
#include "AvalonTask.h"

#include <vector>
#include <queue>

namespace Avalon
{
	/**
	 *@brief 工作者线程池
	 */
	class ThreadPool : private Nocopyable
	{
		typedef std::vector<WorkerThread*> ThreadList;
		typedef std::queue<Task*> TaskQueue;

	public:
		ThreadPool();
		~ThreadPool();

		/**
		 *@brief 开启线程池
		 *@param threadNum 线程数
		 */
		bool Open(UInt32 threadNum);

		/**
		 *@brief 关闭线程池
		 */
		void Shutdown();

		/**
		 *@brief 添加一个任务
		 */
		bool PostTask(Task* task);

		/**
		 *@brief 获取一个线程，或引起阻塞
		 *@return 需要执行的任务，如果线程池关闭或错误返回NULL
		 */
		Task* GetTask();

	protected:
		/**
		 *@brief 创建工作线程
		 */
		virtual WorkerThread* CreateWorkerThread();

	private:
		//工作线程列表
		ThreadList	m_ThreadList;
		//任务队列
		TaskQueue	m_TaskQueue;
		//条件变量
		Condition	m_Cond;
		//互斥锁
		Mutex		m_Mutex;
		//是否在运行
		bool		m_bRun;
	};
}

#endif
