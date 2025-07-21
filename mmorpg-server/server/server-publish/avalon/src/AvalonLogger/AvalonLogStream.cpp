#include "AvalonLogStream.h"
#include "AvalonLogger.h"

namespace Avalon
{
	LogStream::LogStream(Logger* logger, LogPriority priority)
		:m_pLogger(logger)
	{
		m_Event.priority = priority;
		m_Offset = 0;
	}

	LogStream::~LogStream()
	{
	}

	void LogStream::Flush()
	{
		m_Event.time = Avalon::Time::CurrentTime();
		m_Event.content[m_Offset] = 0;
		m_pLogger->Log(m_Event.priority, m_Event);
	}
}
