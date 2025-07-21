#ifndef _AVALON_EPOLL_REACTOR_H_
#define _AVALON_EPOLL_REACTOR_H_

#include "AvalonINetReactor.h"
#include <map>
#include <vector>

namespace Avalon
{
	/**
	 *@brief epoll��Ӧ��
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
		 *@brief ӳ�䵽epoll�¼�
		 */
		UInt32 MaskToEpollEvents(UInt32 events);

	private:
		//epoll���
		int m_epollFd;
		//���ͬʱ��ȡ���¼���
#if !defined(AVALON_OS_WINDOWS)
		struct epoll_event	m_Events[MAX_WAIT_EVENTS_NUM];
#endif
		//�������б�
		HANDLER_MAP	m_Handlers; //noted by aprilliu, ��socket fdΪkey�� value�� NetConnection*
		//��ʱ��
		HANDLER_VEC	m_TimeoutList;

		//���������
		UInt32 m_MaxNumOfHandlers;
		//tick���
		UInt32 m_TickInter;
		//�Ƿ��Ѵ�
		bool m_bOpened;
		//ɾ��handler��ʶ
		bool m_bRemoveFlag;

		//�ϴ�����ʱ��ʱ��
		UInt64	m_InputTime;
	};
}

#endif
