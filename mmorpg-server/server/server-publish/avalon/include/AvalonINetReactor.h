/**
 *@author	 
 *@email	 
 *@date		2011-03-03	
 *@brief	���練Ӧ��
 */
#ifndef _AVALON_INETREACTOR_H_
#define _AVALON_INETREACTOR_H_

#include "AvalonINetEventHandler.h"

namespace Avalon
{

	/**
	 *@brief ���練Ӧ��
	 */
	class INetReactor
	{
	public:
		virtual ~INetReactor(){}

		/**
		 *@brief �򿪷�Ӧ��
		 *@param maxNumOfHandlers �������
		 *@param tickInter tick���
		 *@return �ɹ�����true��ʧ�ܷ���false
		 */
		virtual bool Open(UInt32 maxNumOfHandles,UInt32 tickInter) = 0;

		/**
		 *@brief �ر�
		 */
		virtual void Close() = 0;

		/**
		 *@brief ע���¼�������
		 */
		virtual bool RegisterHandler(INetEventHandler* handler,UInt32 events) = 0;

		/**
		 *@brief �Ƴ��¼�������
		 */
		virtual void RemoveHandler(INetEventHandler* handler,UInt32 events = NETEVENT_ALL) = 0;

		/**
		 *@brief ��ȡ��������
		 */
		virtual UInt32 GetHandlerNum() const = 0;

		/**
		 *@brief �����¼�
		 */
		virtual bool HandleEvents(const Time& now) = 0;

		/**
		 *@brief ��ȡ�ϴ�����ʱ��ʱ��
		 */
		virtual UInt64 GetInputTime() const = 0;

	protected:
	};
}

#endif
