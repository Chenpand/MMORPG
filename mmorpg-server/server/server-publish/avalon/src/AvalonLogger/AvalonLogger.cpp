#include "AvalonLogger.h"
#include "AvalonTime.h"
#include <cassert>

namespace Avalon
{
	Logger::LoggerMap Logger::m_Loggers;
	Mutex* Logger::m_pLoogersMutex = new Mutex();

	Logger::Logger()
	{
		m_Priority = LOG_DEBUG;  //默认日志级别
	}

	Logger::~Logger()
	{
		m_Mutex.Acquire();
		for(AppenderList::iterator iter = m_Appenders.begin();iter != m_Appenders.end();++iter)
		{
			(*iter)->Close();
			(*iter)->Finish();
			delete (*iter);
		}
		m_Appenders.clear();
		m_Mutex.Release();
	}

	//noted by aprilliu,根据名称得到具体的日志系统,可能存在的日志系统(不同业务系统)： Player, Billing, Task，以及syslog(系统日志,以进程名为前缀)
	Logger* Logger::getInstance(const char* name)
	{
		if(name == NULL) return NULL;

		Avalon::ScopeMutex scopeMutex(m_pLoogersMutex);
		LoggerMap::iterator iter = m_Loggers.find(name);
		if(iter != m_Loggers.end()) return iter->second;
		
		Logger* logger = new Logger();
		m_Loggers.insert(std::make_pair(name, logger)); 
		return logger;
	}

	void Logger::Shutdown()
	{
		Avalon::ScopeMutex scopeMutex(m_pLoogersMutex);
		for(LoggerMap::iterator iter = m_Loggers.begin();
			iter != m_Loggers.end(); ++iter)
		{
			delete iter->second;
		}
		m_Loggers.clear();
	}

	bool Logger::AddAppender(Avalon::Appender *appender)
	{
		if(appender == NULL || !appender->Open())
		{
			delete appender;
			return false;
		}
		m_Mutex.Acquire();
		m_Appenders.push_back(appender);
		m_Mutex.Release();
		return true;
	}

	void Logger::SetPriority(LogPriority priority)
	{
		m_Priority = priority;
	}

	void Logger::Log(LogPriority priority,const char* format,va_list args)
	{
		LogEvent logEvent;
		logEvent.time = Avalon::Time::CurrentTime();
		logEvent.priority = priority;
		int ret = avalon_vsnprintf(logEvent.content,LogEvent::MAX_LOGGER_LEN - 1, format, args);
		if(ret < 0) return;

		Log(priority, logEvent);
	}

	void Logger::Debug(const char* format,...)
	{
		va_list args;
		va_start(args,format);
		Log(LOG_DEBUG,format,args);
		va_end(args);
	}

	void Logger::Trace(const char* format,...)
	{
		va_list args;
		va_start(args,format);
		Log(LOG_TRACE,format,args);
		va_end(args);
	}

	void Logger::Info(const char* format,...)
	{
		va_list args;
		va_start(args,format);
		Log(LOG_INFO,format,args);
		va_end(args);
	}

	void Logger::Warn(const char* format,...)
	{
		va_list args;
		va_start(args,format);
		Log(LOG_WARN,format,args);
		va_end(args);;
	}

	void Logger::Error(const char* format,...)
	{
		va_list args;
		va_start(args,format);
		Log(LOG_ERROR,format,args);
		va_end(args);
	}

	void Logger::Fatal(const char* format,...)
	{
		va_list args;
		va_start(args,format);
		Log(LOG_FATAL,format,args);
		va_end(args);
	}

	void Logger::Log(LogPriority priority, const LogEvent& logevent) //输出到所有日志appender里（比如：日志文件，控制台，或者网络日志流中)
	{
		if (priority < m_Priority) return; //noted by aprilliu, 判断日志信息级别，小于设置的日志级别门槛 则不进行log

		m_Mutex.Acquire();
		for(AppenderList::iterator iter = m_Appenders.begin();iter != m_Appenders.end();++iter)
		{
			(*iter)->Append(logevent);
		}
		m_Mutex.Release();
	}

	void Logger::InitAppenders(void)
	{
		m_Mutex.Acquire();
		for (AppenderList::iterator iter = m_Appenders.begin(); iter != m_Appenders.end(); ++iter)
		{
			(*iter)->Init();
		}
		m_Mutex.Release();
	}
}

