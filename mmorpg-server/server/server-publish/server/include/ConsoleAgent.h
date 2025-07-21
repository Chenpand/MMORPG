#ifndef _CONSOLE_AGENT_H_
#define _CONSOLE_AGENT_H_

#include <string>
#include <map>
#include <CLDefine.h>
#include "json/reader.h"

/**
 *@brief 客服后台命令处理器
 */
class ConsoleCmdHandler
{
public:
	virtual ~ConsoleCmdHandler(){}

	virtual std::string GetCmd() const = 0;
	virtual std::string GetSubCmd() const = 0;
	virtual void operator()(UInt32 nodeid, json::Object& jsonobj) = 0;
};

/**
 *@brief 客服后台代理
 */
class ConsoleAgent
{
	typedef std::map<std::string, ConsoleCmdHandler*>	SubCmdHandlerMap;
	typedef std::map<std::string, SubCmdHandlerMap>  CmdHandlerMap;

public:
	ConsoleAgent();
	virtual ~ConsoleAgent();

public:
	/**
	 *@brief 注册处理器
	 */
	void RegisterHandler(ConsoleCmdHandler* handler);

	/**
	 *@brief 发送回复
	 */
	void SendResponse(UInt32 connid, const std::string& cmd, const std::string& subcmd, const json::Object& jsonobj);

	/**
	 *@brief 执行请求
	 */
	void HandleRequest(UInt32 connid, const std::string& cmd, const std::string& subcmd, const std::string& json);

protected:

	virtual void DoSendResponse(UInt32 connid, const std::string& cmd, const std::string& subcmd, const std::string& json) = 0;

private:
	//处理器列表
	CmdHandlerMap	m_Handlers;
};

#endif
