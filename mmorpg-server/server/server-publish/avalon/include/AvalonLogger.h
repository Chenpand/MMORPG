/**
 *@author	 
 *@email	 
 *@date		2011-04-19		
 *@brief	日志
 */
#ifndef _AVALON_LOGGER_H_
#define _AVALON_LOGGER_H_

#include <list>
#include <map>
#include <cstdarg>
#include "AvalonAppender.h"
#include "AvalonLogStream.h"

namespace Avalon
{
	/**
	 *@brief 日志类
	 */
	class Logger
	{
		const static size_t MAX_LOGGER_LENGTH = 1024;

		typedef std::map<std::string, Logger*>	LoggerMap;
		typedef std::list<Appender*>		AppenderList;

	private:
		Logger();
		~Logger();

	public:
		/**
		 *@brief 获取一个实例
		 */
		static Logger* getInstance(const char* name);

		/**
         *@brief 关闭日志系统
		 */
		static void Shutdown();

	public:
		/**
		 *@brief 打开
		 */
		bool Open();

		/**
		 *@brief 关闭
		 */
		void Close();

		/**
		 *@brief 添加一个追加器
		 */
		bool AddAppender(Appender* appender);

		/**
		 *@brief 设置优先级
		 */
		void SetPriority(LogPriority priority);

		/**
		 *@brief 调试日志
		 */
		void Debug(const char* format,...);

		/**
		 *@brief trace日志
		 */
		void Trace(const char* format,...);

		/**
		 *@brief info日志
		 */
		void Info(const char* format,...);

		/**
		 *@brief warn日志
		 */
		void Warn(const char* format,...);

		/**
		 *@brief error日志
		 */
		void Error(const char* format,...);

		/**
		 *@brief fatal日志
		 */
		void Fatal(const char* format,...);

		/**
		 *@brief 输出一条指定优先级的日志
		 */
		void Log(LogPriority priority, const char* format,va_list args);

		/**
		 *@brief 输出一天制定优先级的日志
		 */
		void Log(LogPriority priority, const LogEvent& logevent);

		/**
		 *@brief 获取日志流
		 */
		LogStream GetStream(LogPriority priority){ return LogStream(this, priority); }

		/**
		*@brief 初始化追加器们
		*/
		void InitAppenders(void);

	private:
		//日志优先级
		LogPriority		m_Priority;
		//appender列表
		AppenderList	m_Appenders;
		//日志集
		static LoggerMap	m_Loggers;
		//日志列表锁
		static Mutex*	m_pLoogersMutex;
		//appender锁
		Mutex	m_Mutex;
	};
}

#endif
