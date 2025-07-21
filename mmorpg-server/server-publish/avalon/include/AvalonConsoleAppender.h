/**
 *@author	 
 *@email	 
 *@date		2011-04-19		
 *@brief	控制台日志输出器
 */
#ifndef _AVALON_CONSOLEAPPENDER_H_
#define _AVALON_CONSOLEAPPENDER_H_

#include "AvalonAppender.h"

namespace Avalon
{
	struct LogEvent;
	/**
	 *@brief 控制台输出器
	 */
	class ConsoleAppender : public Appender
	{
	public:
		ConsoleAppender();
		~ConsoleAppender();

		bool DoOpen();

		bool DoAppend(const LogEvent& logEvent);

		void DoClose();

	private:

	};
}

#endif
