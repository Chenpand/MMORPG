/**
 *@author	 
 *@email	 
 *@date		2018-09-30		
 *@brief	文件日志输出器（线程版）
 */
#ifndef _AVALON_FILEAPPENDER_THREAD_H_
#define _AVALON_FILEAPPENDER_THREAD_H_

#include <string>
#include <fstream>
#include "AvalonAppender.h"
#include "AvalonDate.h"

#include "AvalonThread.h"
#include <AvalonDoubleQueue.h>

namespace Avalon
{
	/**
	 *@brief 文件输出器  noted by aprilliu, 以文件形式输出日志信息
	 */
	class FileAppenderThread : public Appender, public Thread
	{
		typedef Avalon::DoubleQueue<LogEvent, 256> LogEventQueue;

	public:
		explicit FileAppenderThread(const std::string& prefix, FileRollType rollType = FILEROLL_NONE);
		~FileAppenderThread();

		bool DoOpen();

		bool DoAppend(const LogEvent& logEvent);

		void DoClose();

	public:
		virtual bool Init();
		virtual bool Finish();

	private:
		std::string GetFileName() const;
		bool CheckReopen(const Date& now) const;
		bool FlushLogEvent(const LogEvent& logEvent);

	public:
		/**
		*@brief 启动线程
		*/
		void Run();
		/**
		*@brief 停止线程
		*/
		void Stop();

	private:
		//文件名
		std::string		m_Prefix;
		//输出文件流
		std::ofstream	m_Stream;
		//滚动类型
		FileRollType	m_RollType;
		//打开文件日志
		Date			m_OpenDate;
		//日志事件队列
		LogEventQueue	m_LogEvents;
		//运行标志
		bool			m_bRun;
	};
}

#endif
