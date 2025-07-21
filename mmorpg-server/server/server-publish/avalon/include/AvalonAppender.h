/**
 *@author	 
 *@email	 
 *@date		2011-04-19		
 *@brief	日志输出器
 */
#ifndef _AVALON_APPENDER_H_
#define _AVALON_APPENDER_H_

#include "AvalonLogEvent.h"
#include "AvalonMutex.h"

namespace Avalon
{
	/**
	 *@brief 日志输出器
	 */
	class Appender
	{
	public:
		Appender(){}
		virtual ~Appender(){}

		/**
		 *@brief 打开追加器
		 */
		bool Open();

		/**
		 *@brief 关闭
		 */
		void Close();

		/**
		 *处理一个日志事件
		 */
		bool Append(const LogEvent& logEvent);

	public:
		virtual bool Init() { return true; }
		virtual bool Finish() { return true; }

	protected:
		virtual bool DoOpen() = 0;
		virtual bool DoAppend(const LogEvent& logEvent) = 0;
		virtual void DoClose() = 0;

	private:
		//互斥锁
		Avalon::Mutex m_Mutex;
	};
}

#endif
