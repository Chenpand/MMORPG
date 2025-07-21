/**
 *@author	 
 *@email	 
 *@date		2011-04-19		
 *@brief	����̨��־�����
 */
#ifndef _AVALON_CONSOLEAPPENDER_H_
#define _AVALON_CONSOLEAPPENDER_H_

#include "AvalonAppender.h"

namespace Avalon
{
	struct LogEvent;
	/**
	 *@brief ����̨�����
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
