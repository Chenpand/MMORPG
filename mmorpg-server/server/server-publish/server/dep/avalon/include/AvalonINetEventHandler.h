/**
 *@author	 
 *@email	 
 *@date		2011-03-03	
 *@brief	�����¼�������
 */
#ifndef _AVALON_INETEVENTHANDLER_H_
#define _AVALON_INETEVENTHANDLER_H_

#include "AvalonNetworkAPI.h"
#include "AvalonTime.h"

namespace Avalon
{
	enum
	{
		NETEVENT_IN			= 1 << 0,		//�ɶ��¼�
		NETEVENT_OUT		= 1 << 1,		//��д�¼�
		NETEVENT_EXCEPTION	= 1 << 2,		//�쳣����������
		NETEVENT_IO			= NETEVENT_IN | NETEVENT_OUT | NETEVENT_EXCEPTION,	//IO�¼�
		NETEVENT_TICK		= 1 << 3,		//tick�¼�
		NETEVENT_ALL		= NETEVENT_IO | NETEVENT_TICK,
	};

	class INetReactor;

	/**
	 *@brief �����¼��������ӿ�
	 */
	class INetEventHandler
	{
	public:
		INetEventHandler():m_pReactor(NULL),m_Timeout(0),m_Events(0){}
		virtual ~INetEventHandler(){}

		/**
		 *@brief ��ȡ�׽��־��
		 */
		virtual sock_t GetHandle() const= 0;

		/**
		 *@brief ���������¼�
		 */
		virtual void HandleInput() = 0;

		/**
		 *@brief ��������¼�
		 */
		virtual void HandleOutput() = 0;

		/**
		 *@brief �����쳣
		 */
		virtual void HandleException() = 0;

		/**
		 *@brief �ӷ�Ӧ���Ƴ�ʱ�ص�
		 */
		virtual void HandleClose() = 0;

		/**
		 *@brief ��ʱ����
		 */
		virtual void HandleTick(const Time& now) = 0;
	
		/**
		 *@brief ��ȡ��Ӧ��ָ��
		 */
		INetReactor* GetReactor() const { return m_pReactor; }

	public:
		/**
		 *@brief ���û�ȡ��ʱʱ��
		 */
		void SetTimeout(UInt64 timeout){ m_Timeout = timeout; }
		UInt64 GetTimeout() const { return m_Timeout; }

		/**
		 *@brief ���û�ȡ�¼�
		 */
		void SetEvents(UInt32 events){ m_Events = events; }
		UInt32 GetEvents() const{ return m_Events; }

		/**
		 *@brief ���÷�Ӧ��
		 */
		void SetReactor(INetReactor* reactor){ m_pReactor = reactor; }

	private:
		//��Ӧ��ָ��
		INetReactor* m_pReactor;
		//��ʱ��ʱʱ��
		UInt64	m_Timeout;
		//�¼�
		UInt32	m_Events;
	};
}

#endif
