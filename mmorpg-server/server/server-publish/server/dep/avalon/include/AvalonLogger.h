/**
 *@author	 
 *@email	 
 *@date		2011-04-19		
 *@brief	��־
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
	 *@brief ��־��
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
		 *@brief ��ȡһ��ʵ��
		 */
		static Logger* getInstance(const char* name);

		/**
         *@brief �ر���־ϵͳ
		 */
		static void Shutdown();

	public:
		/**
		 *@brief ��
		 */
		bool Open();

		/**
		 *@brief �ر�
		 */
		void Close();

		/**
		 *@brief ���һ��׷����
		 */
		bool AddAppender(Appender* appender);

		/**
		 *@brief �������ȼ�
		 */
		void SetPriority(LogPriority priority);

		/**
		 *@brief ������־
		 */
		void Debug(const char* format,...);

		/**
		 *@brief trace��־
		 */
		void Trace(const char* format,...);

		/**
		 *@brief info��־
		 */
		void Info(const char* format,...);

		/**
		 *@brief warn��־
		 */
		void Warn(const char* format,...);

		/**
		 *@brief error��־
		 */
		void Error(const char* format,...);

		/**
		 *@brief fatal��־
		 */
		void Fatal(const char* format,...);

		/**
		 *@brief ���һ��ָ�����ȼ�����־
		 */
		void Log(LogPriority priority, const char* format,va_list args);

		/**
		 *@brief ���һ���ƶ����ȼ�����־
		 */
		void Log(LogPriority priority, const LogEvent& logevent);

		/**
		 *@brief ��ȡ��־��
		 */
		LogStream GetStream(LogPriority priority){ return LogStream(this, priority); }

		/**
		*@brief ��ʼ��׷������
		*/
		void InitAppenders(void);

	private:
		//��־���ȼ�
		LogPriority		m_Priority;
		//appender�б�
		AppenderList	m_Appenders;
		//��־��
		static LoggerMap	m_Loggers;
		//��־�б���
		static Mutex*	m_pLoogersMutex;
		//appender��
		Mutex	m_Mutex;
	};
}

#endif
