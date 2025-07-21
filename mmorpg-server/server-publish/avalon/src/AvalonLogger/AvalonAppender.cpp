#include "AvalonAppender.h"

namespace Avalon
{
	bool Appender::Open()
	{
		bool ret = false;
		m_Mutex.Acquire();
		ret = DoOpen();
		m_Mutex.Release();
		return ret;
	}

	void Appender::Close()
	{
		m_Mutex.Acquire();
		DoClose();
		m_Mutex.Release();
	}

	bool Appender::Append(const LogEvent& logEvnet)
	{
		bool ret = false;
		m_Mutex.Acquire();
		ret = DoAppend(logEvnet);
		m_Mutex.Release();
		return ret;
	}
}
