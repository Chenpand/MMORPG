/**
 *@author	 
 *@email	 
 *@date		2011-03-01	
 *@brief	线程类定义
 */
#ifndef _AVALON_THREAD_H_
#define _AVALON_THREAD_H_

#include <AvalonThreadAPI.h>
#include <AvalonSemaphore.h>

namespace Avalon
{
	class INetEventHandler;
	class INetReactor;

	/**
	 *@brief 线程类
	 */
	class Thread : private Nocopyable
	{
	public:
		Thread();
		virtual ~Thread();

	protected:
		/**
		 *@brief 线程执行体，子类实现!! 子类 比如：NetThread
		 */
		virtual void Run() = 0;

	public:
		/**
		 *@brief 激活这个线程
		 */
		bool Activate();

		/**
		 *@brief 强制终止线程
		 */
		virtual void Stop();

		/**
		 *@brief 回收线程资源
		 */
		void Join();

		static void Sleep(UInt32 msecs);

	private:
		static AVALON_THREADFUNC_RET Function(void*);

	private:
		//线程
		thread_t	m_Thread;
		//条信号量
		Semaphore	m_Sem;
	};
}

#endif
