/**
 *@author	 
 *@email	 
 *@date		2011-03-03	
 *@brief	线程任务任务
 */
#ifndef _AVALON_TASK_H_
#define _AVALON_TASK_H_


namespace Avalon
{
	/**
	 *@brief 多线程调度的任务
	 */
	class WorkerThread;
	class Task
	{
	public:
		/**
		 *@brief 任务执行接口，子类实现
		 */
		virtual void Run(WorkerThread* thread) = 0;

		/**
		 *@brief 释放这个任务
		 */
		virtual void Release() = 0;
	};
}

#endif
