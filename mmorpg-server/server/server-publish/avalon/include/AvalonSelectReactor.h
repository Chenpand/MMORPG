/**
 *@author	 
 *@email	 
 *@date		2011-03-08	
 *@brief	select反应堆
 */
#ifndef _AVALON_SELECTREACTOR_H_
#define _AVALON_SELECTREACTOR_H_

#include "AvalonINetReactor.h"
#include <map>

namespace Avalon
{
	/**
	 *@brief select反应堆
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
		//读集合
		fd_set m_ReadSet;
		//写集合
		fd_set m_WriteSet;
		//异常集合
		fd_set m_ExceptSet;

		//处理器列表
		HANDLER_MAP	m_Handlers;
		//最大处理器数
		UInt32 m_MaxNumOfHandlers;
		//tick间隔
		UInt32 m_TickInter;
		//是否已打开
		bool m_bOpened;
		//删除handler标识
		bool m_bRemoveFlag;

		//上次输入事件时间
		UInt64	m_InputTime;
	};
}

#endif
