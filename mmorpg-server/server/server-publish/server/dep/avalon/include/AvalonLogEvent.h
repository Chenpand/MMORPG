/**
 *@author	 
 *@email	 
 *@date		2011-04-19		
 *@brief	��־�¼�
 */
#ifndef _AVALON_LOGEVENT_H_
#define _AVALON_LOGEVENT_H_

#include <AvalonDefine.h>
#include <AvalonTime.h>
#include <string>

namespace Avalon
{
	/**
	 *@brief ��־���ȼ�
	 */
	enum LogPriority
	{
		LOG_DEV,			//������־
		LOG_DEBUG,			//������Ϣ
		LOG_TRACE,			//����
		LOG_INFO,			//��¼
		LOG_WARN,			//����
		LOG_ERROR,			//����
		LOG_FATAL,			//����
		LOG_MAX,			//��󼶱�
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
	 *@brief ��־�¼�
	 */
	struct LogEvent
	{
		//�����־����
		const static size_t MAX_LOGGER_LEN = 2048;

	public:
		LogEvent():priority(LOG_DEBUG)
		{
			memset(content, 0, sizeof(content));
		}
		~LogEvent(){}

	public:
		//ʱ��
		Time			time;
		//���ȼ�
		LogPriority		priority;
		//����
		char		    content[MAX_LOGGER_LEN];
	};
}

#endif
