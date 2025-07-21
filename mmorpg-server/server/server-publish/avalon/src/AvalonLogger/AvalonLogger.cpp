#include "AvalonLogger.h"
#include "AvalonTime.h"
#include <cassert>

namespace Avalon
{
	Logger::LoggerMap Logger::m_Loggers;
	Mutex* Logger::m_pLoogersMutex = new Mutex();

	Logger::Logger()
	{
		m_Priority = LOG_DEBUG;  //Ĭ����־����
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

	//noted by aprilliu,�������Ƶõ��������־ϵͳ,���ܴ��ڵ���־ϵͳ(��ͬҵ��ϵͳ)�� Player, Billing, Task���Լ�syslog(ϵͳ��־,�Խ�����Ϊǰ׺)
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

	void Logger::Log(LogPriority priority, const LogEvent& logevent) //�����������־appender����磺��־�ļ�������̨������������־����)
	{
		if (priority < m_Priority) return; //noted by aprilliu, �ж���־��Ϣ����С�����õ���־�����ż� �򲻽���log

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

