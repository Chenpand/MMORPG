#ifndef _AVALON_EPOLL_REACTOR_H_
#define _AVALON_EPOLL_REACTOR_H_

#include "AvalonINetReactor.h"
#include <map>
#include <vector>

namespace Avalon
{
	/**
	 *@brief epoll反应堆
	 */
	class EpollReactor : public INetReactor
	{
		const static size_t MAX_WAIT_EVENTS_NUM = 512;

		typedef std::map<sock_t,INetEventHandler*> HANDLER_MAP;
		typedef std::vector<INetEventHandler*> HANDLER_VEC;

		struct HandlerTimeoutCmp
		{
			bool operator()(const INetEventHandler* handler1, const INetEventHandler* handler2) const
			{
				return handler1->GetTimeout() > handler2->GetTimeout();
			}
		};

	public:
		EpollReactor();
		~EpollReactor();

		bool Open(UInt32 maxNumOfHandles, UInt32 tickInter);

		void Close();

		bool RegisterHandler(INetEventHandler* handler,UInt32 events);

		void RemoveHandler(INetEventHandler* handler,UInt32 events);

		UInt32 GetHandlerNum() const;

		bool HandleEvents(const Time& now);

		UInt64 GetInputTime() const{ return m_InputTime; }

	private:
		/**
		 *@brief 映射到epoll事件
		 */
		UInt32 MaskToEpollEvents(UInt32 events);

	private:
		//epoll句柄
		int m_epollFd;
		//最大同时获取的事件数
#if !defined(AVALON_OS_WINDOWS)
		struct epoll_event	m_Events[MAX_WAIT_EVENTS_NUM];
#endif
		//处理器列表
		HANDLER_MAP	m_Handlers; //noted by aprilliu, 以socket fd为key， value是 NetConnection*
		//超时堆
		HANDLER_VEC	m_TimeoutList;

		//最大处理器数
		UInt32 m_MaxNumOfHandlers;
		//tick间隔
		UInt32 m_TickInter;
		//是否已打开
		bool m_bOpened;
		//删除handler标识
		bool m_bRemoveFlag;

		//上次输入时间时间
		UInt64	m_InputTime;
	};
}

#endif
