#include "ConsoleAgent.h"

#include <sstream>
#include "CLDefine.h"
#include "json/writer.h"

ConsoleAgent::ConsoleAgent()
{
}

ConsoleAgent::~ConsoleAgent()
{
	for(CmdHandlerMap::iterator iter = m_Handlers.begin();
		iter != m_Handlers.end(); ++iter)
	{
		SubCmdHandlerMap& handlers = iter->second;
		for(SubCmdHandlerMap::iterator subiter = handlers.begin();
			subiter != handlers.end(); ++subiter)
		{
			delete subiter->second;
		}
	}
	m_Handlers.clear();
}

void ConsoleAgent::RegisterHandler(ConsoleCmdHandler* handler)
{
	if(handler == NULL) return;

	CmdHandlerMap::iterator iter = m_Handlers.find(handler->GetCmd());
	if(iter != m_Handlers.end())
	{
		SubCmdHandlerMap::iterator subiter = iter->second.find(handler->GetSubCmd());
		if(subiter != iter->second.end())
		{
			delete subiter->second;
			subiter->second = handler;
			return;
		}
	}

	if(!m_Handlers[handler->GetCmd()].insert(std::make_pair(handler->GetSubCmd(), handler)).second)
	{
		delete handler;
		return;
	}
}

void ConsoleAgent::SendResponse(UInt32 connid, const std::string& cmd, const std::string& subcmd, const json::Object& jsonobj)
{
	std::ostringstream oss;
	try
	{
		json::Writer::Write(jsonobj, oss);
	}
	catch(const json::Exception& e)
	{
		GameErrorLogStream("Console") << "ConsoleAgent::SendResponse failed: " << e.what() << LogStream::eos;
		return;
	}

	DoSendResponse(connid, cmd, subcmd, oss.str());
	GameInfoLogStream("Console") << "send console response:" << cmd << "|" << subcmd << "|" << oss.str() << "!" << LogStream::eos;
}

void ConsoleAgent::HandleRequest(UInt32 connid, const std::string& cmd, const std::string& subcmd, const std::string& json)
{
	json::Object body;

	if (!json.empty())
	{
		std::istringstream iss(json);
		try
		{
			json::Reader::Read(body, iss);
		}
		catch (json::Exception& e)
		{
			GameErrorLogStream("Console") << "ConsoleAgent::HandleRequest failed:" << e.what() << LogStream::eos;
			return;
		}
	}

	CmdHandlerMap::iterator iter = m_Handlers.find(cmd);
	if(iter != m_Handlers.end())
	{
		SubCmdHandlerMap::iterator subiter = iter->second.find(subcmd);
		if(subiter != iter->second.end())
		{
			GameInfoLogStream("Console") << "handler console request:" << cmd << "|" << subcmd << "|" << json << "!" << LogStream::eos;
			(*subiter->second)(connid, body);
			return;
		}
	}

	GameErrorLogStream("Console") << "unknow cmd: " << cmd << "," << subcmd << "!" << LogStream::eos;
}
