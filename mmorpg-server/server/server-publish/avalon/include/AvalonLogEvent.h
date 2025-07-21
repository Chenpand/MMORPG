/**
 *@author	 
 *@email	 
 *@date		2011-04-19		
 *@brief	日志事件
 */
#ifndef _AVALON_LOGEVENT_H_
#define _AVALON_LOGEVENT_H_

#include <AvalonDefine.h>
#include <AvalonTime.h>
#include <string>

namespace Avalon
{
	/**
	 *@brief 日志优先级
	 */
	enum LogPriority
	{
		LOG_DEV,			//开发日志
		LOG_DEBUG,			//调试信息
		LOG_TRACE,			//跟踪
		LOG_INFO,			//记录
		LOG_WARN,			//警告
		LOG_ERROR,			//错误
		LOG_FATAL,			//崩溃
		LOG_MAX,			//最大级别
	};

	const static char* PRIORITY_NAME[LOG_MAX] = {
		"DEV",
		"DEBUG",
		"TRACE",
		"INFO",
		"WARN",
		"ERROR",
		"FATAL",
	};

	inline const char* GetPriorityNameByIndex(int index)
	{
		if(index >= LOG_MAX) return NULL;
		return PRIORITY_NAME[index];
	}

	/**
	 *@brief 日志事件
	 */
	struct LogEvent
	{
		//最大日志长度
		const static size_t MAX_LOGGER_LEN = 2048;

	public:
		LogEvent():priority(LOG_DEBUG)
		{
			memset(content, 0, sizeof(content));
		}
		~LogEvent(){}

	public:
		//时间
		Time			time;
		//优先级
		LogPriority		priority;
		//内容
		char		    content[MAX_LOGGER_LEN];
	};
}

#endif
