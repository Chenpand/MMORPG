/**
 *@author	 
 *@email	 
 *@date		2011-03-08	
 *@brief	select��Ӧ��
 */
#ifndef _AVALON_SELECTREACTOR_H_
#define _AVALON_SELECTREACTOR_H_

#include "AvalonINetReactor.h"
#include <map>

namespace Avalon
{
	/**
	 *@brief select��Ӧ��
	 */
	class SelectReactor : public INetReactor
	{
		typedef std::map<sock_t,INetEventHandler*> HANDLER_MAP;

	public:
		SelectReactor();
		~SelectReactor();

		bool Open(UInt32 maxNumOfHandles,UInt32 tickInter);

		void Close();

		bool RegisterHandler(INetEventHandler* handler,UInt32 events);

		void RemoveHandler(INetEventHandler* handler,UInt32 events);

		UInt32 GetHandlerNum() const;

		bool HandleEvents(const Time& now);

		UInt64 GetInputTime() const{ return m_InputTime; }

	private:
		//������
		fd_set m_ReadSet;
		//д����
		fd_set m_WriteSet;
		//�쳣����
		fd_set m_ExceptSet;

		//�������б�
		HANDLER_MAP	m_Handlers;
		//���������
		UInt32 m_MaxNumOfHandlers;
		//tick���
		UInt32 m_TickInter;
		//�Ƿ��Ѵ�
		bool m_bOpened;
		//ɾ��handler��ʶ
		bool m_bRemoveFlag;

		//�ϴ������¼�ʱ��
		UInt64	m_InputTime;
	};
}

#endif
