#ifndef _US_CONSOLE_CMD_HANDLER_H_
#define _US_CONSOLE_CMD_HANDLER_H_

#include <ConsoleAgent.h>

/**
*@brief …œº‹∂©µ•
*/
class SubmitOrderHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "goldConsignment"; }
	std::string GetSubCmd() const{ return "submitOrder"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};



#endif