#ifndef _GAS_CONSOLE_CMD_HANDLER_H_
#define _GAS_CONSOLE_CMD_HANDLER_H_

#include <ConsoleAgent.h>

/**
*@brief ����ò�ѯ
*/
class QueryActConfigHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "activeControl"; }
	std::string GetSubCmd() const{ return "queryActive"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief ������޸�
*/
class SetActConfigHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "activeControl"; }
	std::string GetSubCmd() const{ return "setActive"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

/**
*@brief �������ɾ��
*/
class DelActConfigHandler : public ConsoleCmdHandler
{
public:
	std::string GetCmd() const{ return "activeControl"; }
	std::string GetSubCmd() const{ return "delActive"; }

	void operator()(UInt32 connid, json::Object& jsonobj);
};

#endif