/**
 *@author	 
 *@email	 
 *@date		2018-09-30		
 *@brief	�ļ���־��������̰߳棩
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
	 *@brief �ļ������  noted by aprilliu, ���ļ���ʽ�����־��Ϣ
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
		*@brief �����߳�
		*/
		void Run();
		/**
		*@brief ֹͣ�߳�
		*/
		void Stop();

	private:
		//�ļ���
		std::string		m_Prefix;
		//����ļ���
		std::ofstream	m_Stream;
		//��������
		FileRollType	m_RollType;
		//���ļ���־
		Date			m_OpenDate;
		//��־�¼�����
		LogEventQueue	m_LogEvents;
		//���б�־
		bool			m_bRun;
	};
}

#endif
