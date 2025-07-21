/**
 *@author	 
 *@email	 
 *@date		2011-03-27	
 *@brief	网络线程
 */
#ifndef _AVALON_NETTHREAD_H_
#define _AVALON_NETTHREAD_H_

#include <map>
#include <vector>
#include "AvalonThread.h"
#include "AvalonMutex.h"
#include "AvalonProcessUnit.h"

namespace Avalon
{
	class INetEventHandler;
	class INetReactor;

	/**
	 *@brief 网络线程
	 */
	class NetThread : public Thread, public ProcessUnit
	{
		typedef std::map<INetEventHandler*,UInt32> HANDLER_MAP;

	public:
		explicit NetThread(UInt32 maxHandler);
		~NetThread();

	public:
		/**
		 *@brief 注册一个句柄
		 */
		void RegisterHandler(INetEventHandler* handler,UInt32);

		/**
		 *@brief 获取当前线程数
		 */
		UInt32 GetHandlerNum() const;

		void Stop(UInt32 inputTimeout);
		
	protected:
		bool OnInit();

		void OnTick(const Time& now);

		void OnQuit();

		void Run(); //noted by aprilliu, 在线程函数中被调用

	private:
		//反应堆  noted by aprilliu, 无论是监听线程还是IO线程 都会有自己的Reactor，对于监听线程的reactor一般只有一个Acceptor 的handler
		INetReactor* m_pReactor;
		//待加入反应堆的处理器队列
		HANDLER_MAP m_WaitQueue;
		//锁
		Mutex m_Mutex;  //用于互斥访问 m_WaitQueue
		//最大句柄数
		UInt32 m_MaxHandlerNum;
	};

	/**
	 *@brief 网络线程组  noted by aprilliu 循环间隔被设置为固定的2ms
	 */
	class NetThreadGroup
	{
		typedef std::vector<NetThread*> NETTHREAD_VEC;

	public:
		NetThreadGroup(UInt32 threadNum, UInt32 handlerPerThread);
		~NetThreadGroup();

		/**
		 *@brief 启动所有线程
		 */
		bool Activate();

		/**
		 *@brief 停止所有线程
		 */
		void Stop(UInt32 inputTimeout);

		/**
		 *@brief 等待所有线程终止
		 */
		void Join();

		/**
		 *获取一个拥有最少处理器数的网络线程
		 */
		NetThread* GetMinHandlerThread();

	private:
		//线程数组
		NETTHREAD_VEC m_Threads;

	};
}

#endif
