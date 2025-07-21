#include "AvalonConsoleAppender.h"
#include <iostream>
#include <iomanip>
#include <AvalonDate.h>

namespace Avalon
{

	ConsoleAppender::ConsoleAppender()
	{
	}

	ConsoleAppender::~ConsoleAppender()
	{
	}

	bool ConsoleAppender::DoOpen()
	{
		return true;
	}

	void ConsoleAppender::DoClose()
	{
	}

	bool ConsoleAppender::DoAppend(const Avalon::LogEvent& logEvent)
	{
		Date date(logEvent.time);
		std::cout << "[" << std::setfill('0') << std::setw(4) << date.Year() 
			<< "-" << std::setw(2) << date.Month() 
			<< "-" << std::setw(2) << date.Day()
			<< " " << std::setw(2) << date.Hour()
			<< ":" << std::setw(2) << date.Min()
			<< ":" << std::setw(2) << date.Sec() << "]";
		std::cout << "[" << PRIORITY_NAME[logEvent.priority] << "] ";
		std::cout << logEvent.content;
		std::cout << std::endl;
		return true;
	}
}

