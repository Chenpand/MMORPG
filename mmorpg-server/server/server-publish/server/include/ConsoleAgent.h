#ifndef _CONSOLE_AGENT_H_
#define _CONSOLE_AGENT_H_

#include <string>
#include <map>
#include <CLDefine.h>
#include "json/reader.h"

/**
 *@brief �ͷ���̨�������
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
 *@brief �ͷ���̨����
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
	 *@brief ע�ᴦ����
	 */
	void RegisterHandler(ConsoleCmdHandler* handler);

	/**
	 *@brief ���ͻظ�
	 */
	void SendResponse(UInt32 connid, const std::string& cmd, const std::string& subcmd, const json::Object& jsonobj);

	/**
	 *@brief ִ������
	 */
	void HandleRequest(UInt32 connid, const std::string& cmd, const std::string& subcmd, const std::string& json);

protected:

	virtual void DoSendResponse(UInt32 connid, const std::string& cmd, const std::string& subcmd, const std::string& json) = 0;

private:
	//�������б�
	CmdHandlerMap	m_Handlers;
};

#endif
