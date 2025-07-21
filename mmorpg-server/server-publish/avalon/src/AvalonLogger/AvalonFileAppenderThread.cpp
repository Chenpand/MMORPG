#include "AvalonFileAppender.h"
#include <iomanip>
#include <sstream>
#include <AvalonFileAppenderThread.h>

namespace Avalon
{
	FileAppenderThread::FileAppenderThread(const std::string& prefix, FileRollType rollType)
		:m_Prefix(prefix)
	{
		m_RollType = rollType;
		m_bRun = false;
	}

	FileAppenderThread::~FileAppenderThread()
	{
		while (!m_LogEvents.empty())
		{
			LogEvent logEvent = m_LogEvents.pop();
			if (logEvent.time != 0)
			{
				FlushLogEvent(logEvent);
			}
		}
	}

	bool FileAppenderThread::DoOpen()
	{
		if(!m_Stream.is_open())
		{
			m_OpenDate = Date(Avalon::Time::CurrentTime());
			m_Stream.open(GetFileName().c_str(),std::ios_base::out | std::ios_base::app );
			return m_Stream.is_open();
		}
		return true;
	}

	bool FileAppenderThread::DoAppend(const Avalon::LogEvent& logEvent)
	{
		m_LogEvents.push(logEvent);
		return true;
	}

	void FileAppenderThread::DoClose()
	{
		m_Stream.close();
	}

	bool FileAppenderThread::Init()
	{
		m_bRun = true;
		Thread::Activate();
		return true;
	}

	bool FileAppenderThread::Finish()
	{
		m_bRun = false;
		Thread::Stop();
		return true;
	}

	std::string FileAppenderThread::GetFileName() const //noted by aprilliu, 输出类似“SceneServer_20130831” 名字的信息
	{
		std::ostringstream stream;

		stream << m_Prefix << "_" << std::setfill('0');
		switch(m_RollType)
		{
		case FILEROLL_HOUR:
			{
				stream << std::setw(4) << m_OpenDate.Year()
					<< std::setw(2) << m_OpenDate.Month()
					<< std::setw(2) << m_OpenDate.Day()
					<< std::setw(2) << m_OpenDate.Hour()
					<< std::setw(2) << m_OpenDate.Min();
			}
			break;
		case FILEROLL_DAY:
		case FILEROLL_MONTH:
		default:
			{
				stream << std::setw(4) << m_OpenDate.Year()
					<< std::setw(2) << m_OpenDate.Month()
					<< std::setw(2) << m_OpenDate.Day();
			}
			break;
		}

		stream << ".log";
		return stream.str();
	}

	bool FileAppenderThread::CheckReopen(const Date& now) const
	{
		switch(m_RollType)
		{
		case FILEROLL_HOUR:
			{
				return m_OpenDate.Year() != now.Year()
					|| m_OpenDate.Month() != now.Month()
					|| m_OpenDate.Day() != now.Day()
					|| m_OpenDate.Hour() != now.Hour();
			}
			break;
		case FILEROLL_DAY:
			{
				return m_OpenDate.Year() != now.Year()
					|| m_OpenDate.Month() != now.Month()
					|| m_OpenDate.Day() != now.Day();
			}
			break;
		case FILEROLL_MONTH:
			{
				return m_OpenDate.Year() != now.Year()
					|| m_OpenDate.Month() != now.Month();
			}
			break;
		default:
			return false;
		}
		return false;
	}

	bool FileAppenderThread::FlushLogEvent(const LogEvent& logEvent)
	{
		Date date(logEvent.time);
		if (CheckReopen(date)) //noted by aprilliu, 根据日志文件滚动类型 来判断是否需要打开新的日志文件
		{
			m_Stream.close();
			m_OpenDate = date;
			m_Stream.open(GetFileName().c_str(), std::ios_base::out | std::ios_base::app);
		}

		//noted by aprilliu, 以“[2013-09-18 07:31:49][ERROR] xxxxx”的形式输出日志信息
		m_Stream << "[" << std::setfill('0') << std::setw(4) << date.Year()
			<< "-" << std::setw(2) << date.Month()
			<< "-" << std::setw(2) << date.Day()
			<< " " << std::setw(2) << date.Hour()
			<< ":" << std::setw(2) << date.Min()
			<< ":" << std::setw(2) << date.Sec() << "]";
		m_Stream << "[" << PRIORITY_NAME[logEvent.priority] << "] ";
		m_Stream << logEvent.content;
		m_Stream << std::endl;
		m_Stream.flush();
		return true;
	}

	void FileAppenderThread::Run()
	{
		while (m_bRun)
		{
			LogEvent logEvent = m_LogEvents.pop();
			if (logEvent.time != 0)
			{
				FlushLogEvent(logEvent);
			}
			else
			{
				Sleep(10);
			}
		}
	}

	void FileAppenderThread::Stop()
	{
		m_bRun = false;
		Thread::Stop();
	}

}

