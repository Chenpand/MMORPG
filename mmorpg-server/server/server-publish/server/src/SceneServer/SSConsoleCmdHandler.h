#ifndef _SS_CONSOLE_CMD_HANDLER_H_
#define _SS_CONSOLE_CMD_HANDLER_H_

#include <ConsoleAgent.h>


/**
*@brief 异常交易查询
*/
class AbnormalTransQueryHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const { return "abnormalTrans"; }
	std::string GetSubCmd() const { return "query"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 异常交易解冻
*/
class AbnormalTransUnfreezeHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const { return "abnormalTrans"; }
	std::string GetSubCmd() const { return "unfreeze"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

#endif