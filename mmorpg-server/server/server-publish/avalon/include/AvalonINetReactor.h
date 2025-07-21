/**
 *@author	 
 *@email	 
 *@date		2011-03-03	
 *@brief	网络反应堆
 */
#ifndef _AVALON_INETREACTOR_H_
#define _AVALON_INETREACTOR_H_

#include "AvalonINetEventHandler.h"

namespace Avalon
{

	/**
	 *@brief 网络反应堆
	 */
	class INetReactor
	{
	public:
		virtual ~INetReactor(){}

		/**
		 *@brief 打开反应堆
		 *@param maxNumOfHandlers 最大句柄数
		 *@param tickInter tick间隔
		 *@return 成功返回true，失败返回false
		 */
		virtual bool Open(UInt32 maxNumOfHandles,UInt32 tickInter) = 0;

		/**
		 *@brief 关闭
		 */
		virtual void Close() = 0;

		/**
		 *@brief 注册事件处理器
		 */
		virtual bool RegisterHandler(INetEventHandler* handler,UInt32 events) = 0;

		/**
		 *@brief 移除事件处理器
		 */
		virtual void RemoveHandler(INetEventHandler* handler,UInt32 events = NETEVENT_ALL) = 0;

		/**
		 *@brief 获取处理器数
		 */
		virtual UInt32 GetHandlerNum() const = 0;

		/**
		 *@brief 处理事件
		 */
		virtual bool HandleEvents(const Time& now) = 0;

		/**
		 *@brief 获取上次输入时间时间
		 */
		virtual UInt64 GetInputTime() const = 0;

	protected:
	};
}

#endif
