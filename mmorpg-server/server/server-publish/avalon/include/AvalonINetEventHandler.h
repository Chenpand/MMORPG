/**
 *@author	 
 *@email	 
 *@date		2011-03-03	
 *@brief	网络事件处理器
 */
#ifndef _AVALON_INETEVENTHANDLER_H_
#define _AVALON_INETEVENTHANDLER_H_

#include "AvalonNetworkAPI.h"
#include "AvalonTime.h"

namespace Avalon
{
	enum
	{
		NETEVENT_IN			= 1 << 0,		//可读事件
		NETEVENT_OUT		= 1 << 1,		//可写事件
		NETEVENT_EXCEPTION	= 1 << 2,		//异常，带外数据
		NETEVENT_IO			= NETEVENT_IN | NETEVENT_OUT | NETEVENT_EXCEPTION,	//IO事件
		NETEVENT_TICK		= 1 << 3,		//tick事件
		NETEVENT_ALL		= NETEVENT_IO | NETEVENT_TICK,
	};

	class INetReactor;

	/**
	 *@brief 网络事件处理器接口
	 */
	class INetEventHandler
	{
	public:
		INetEventHandler():m_pReactor(NULL),m_Timeout(0),m_Events(0){}
		virtual ~INetEventHandler(){}

		/**
		 *@brief 获取套接字句柄
		 */
		virtual sock_t GetHandle() const= 0;

		/**
		 *@brief 处理输入事件
		 */
		virtual void HandleInput() = 0;

		/**
		 *@brief 处理输出事件
		 */
		virtual void HandleOutput() = 0;

		/**
		 *@brief 处理异常
		 */
		virtual void HandleException() = 0;

		/**
		 *@brief 从反应堆移除时回调
		 */
		virtual void HandleClose() = 0;

		/**
		 *@brief 定时调用
		 */
		virtual void HandleTick(const Time& now) = 0;
	
		/**
		 *@brief 获取反应堆指针
		 */
		INetReactor* GetReactor() const { return m_pReactor; }

	public:
		/**
		 *@brief 设置获取超时时间
		 */
		void SetTimeout(UInt64 timeout){ m_Timeout = timeout; }
		UInt64 GetTimeout() const { return m_Timeout; }

		/**
		 *@brief 设置获取事件
		 */
		void SetEvents(UInt32 events){ m_Events = events; }
		UInt32 GetEvents() const{ return m_Events; }

		/**
		 *@brief 设置反应堆
		 */
		void SetReactor(INetReactor* reactor){ m_pReactor = reactor; }

	private:
		//反应堆指针
		INetReactor* m_pReactor;
		//计时超时时间
		UInt64	m_Timeout;
		//事件
		UInt32	m_Events;
	};
}

#endif
