#ifndef _GAS_CONSOLE_CMD_HANDLER_H_
#define _GAS_CONSOLE_CMD_HANDLER_H_

#include <ConsoleAgent.h>

/**
*@brief 活动配置查询
*/
class QueryActConfigHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "activeControl"; }
	std::string GetSubCmd() const{ return "queryActive"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 活动配置修改
*/
class SetActConfigHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "activeControl"; }
	std::string GetSubCmd() const{ return "setActive"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief 获得配置删除
*/
class DelActConfigHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "activeControl"; }
	std::string GetSubCmd() const{ return "delActive"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

#endif