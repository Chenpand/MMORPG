#ifndef _AVALON_LOGSTREAM_H_
#define _AVALON_LOGSTREAM_H_

#include <AvalonLogEvent.h>

namespace Avalon
{
	class Logger;

	/**
	 *@brief 日志流  noted by aprilliu, 具体用于对一条日志信息进行格式化输出的
	 */
	class LogStream
	{
	public:
		LogStream(Logger* logger, LogPriority priority);
		~LogStream();

		typedef  LogStream& Operator(LogStream& stream);

	public:
		static LogStream& eos(LogStream& stream)
		{
			stream.Flush();
			return stream;
		}

#define AVALON_LOG_BASETYPE_INPUT(Type, Fmt) \
		LogStream& operator << (Type val){ \
			if(m_Offset + 64 > LogEvent::MAX_LOGGER_LEN) return *this; \
			int ret = avalon_snprintf(m_Event.content + m_Offset, \
				LogEvent::MAX_LOGGER_LEN - m_Offset, Fmt, Int32(val)); \
			m_Offset += size_t(ret); \
			return *this; \
		}

		AVALON_LOG_BASETYPE_INPUT(Int8,"%d")
		AVALON_LOG_BASETYPE_INPUT(UInt8,"%d")
		AVALON_LOG_BASETYPE_INPUT(Int16,"%d")
		AVALON_LOG_BASETYPE_INPUT(UInt16,"%d")
		AVALON_LOG_BASETYPE_INPUT(Int32,"%d")
		AVALON_LOG_BASETYPE_INPUT(UInt32,"%u")

		LogStream& operator << (Int64 val){
			if(m_Offset + 64 > LogEvent::MAX_LOGGER_LEN) return *this;
			int ret = avalon_snprintf(m_Event.content + m_Offset,
				LogEvent::MAX_LOGGER_LEN - m_Offset, AVALON_INT64_FORMAT, val);
			m_Offset += size_t(ret);
			return *this;
		}

		LogStream& operator << (UInt64 val){
			if(m_Offset + 64 > LogEvent::MAX_LOGGER_LEN) return *this;
			int ret = avalon_snprintf(m_Event.content + m_Offset,
				LogEvent::MAX_LOGGER_LEN - m_Offset, AVALON_UINT64_FORMAT, val);
			m_Offset += size_t(ret);
			return *this;
		}

		LogStream& operator << (Real32 val){
			if(m_Offset + 64 > LogEvent::MAX_LOGGER_LEN) return *this;
			int ret = avalon_snprintf(m_Event.content + m_Offset,
				LogEvent::MAX_LOGGER_LEN - m_Offset, "%f", val);
			m_Offset += size_t(ret);
			return *this;
		}

		LogStream& operator << (Real64 val){
			if(m_Offset + 64 > LogEvent::MAX_LOGGER_LEN) return *this;
			int ret = avalon_snprintf(m_Event.content + m_Offset,
				LogEvent::MAX_LOGGER_LEN - m_Offset, "%lf", val);
			m_Offset += size_t(ret);
			return *this;
		}

		LogStream& operator << (const char* str){
			if(str == NULL) return *this;

			while((m_Offset < LogEvent::MAX_LOGGER_LEN - 1)
				&& *str != 0){
				m_Event.content[m_Offset++] = *str++;
			}
			m_Event.content[m_Offset] = 0;
			return *this;
		}

		LogStream& operator<< (const std::string& str){
			size_t len = str.length();
			len = LogEvent::MAX_LOGGER_LEN - m_Offset - 1 > len ? len : LogEvent::MAX_LOGGER_LEN - m_Offset - 1;
			avalon_strncpy(m_Event.content + m_Offset, str.c_str(), len);
			m_Offset += len;
			return *this;
		}

		LogStream& operator<<( Operator ope){
			return ope(*this);
		}

		void Flush();

	private:
		//日志
		Logger*		m_pLogger;
		//日志事件
		LogEvent	m_Event;
		//偏移量
		size_t		m_Offset;
	};
}

#endif
