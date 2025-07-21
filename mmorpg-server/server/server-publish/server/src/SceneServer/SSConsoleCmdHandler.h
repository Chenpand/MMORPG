#ifndef _SS_CONSOLE_CMD_HANDLER_H_
#define _SS_CONSOLE_CMD_HANDLER_H_

#include <ConsoleAgent.h>


/**
*@brief �쳣���ײ�ѯ
*/
class AbnormalTransQueryHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const { return "abnormalTrans"; }
	std::string GetSubCmd() const { return "query"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief �쳣���׽ⶳ
*/
class AbnormalTransUnfreezeHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const { return "abnormalTrans"; }
	std::string GetSubCmd() const { return "unfreeze"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

#endif