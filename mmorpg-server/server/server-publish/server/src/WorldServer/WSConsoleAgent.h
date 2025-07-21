#ifndef _WS_CONSOLE_AGENT_H_
#define _WS_CONSOLE_AGENT_H_

#include <AvalonSingleton.h>
#include <ConsoleAgent.h>

/**
 *@brief 控制台代理
 */
class WSConsoleAgent : public ConsoleAgent, public Avalon::Singleton<WSConsoleAgent>
{
public:
	WSConsoleAgent();
	~WSConsoleAgent();

public:
	void Init();
	void Final();

public:
	void DoSendResponse(UInt32 connid, const std::string& cmd, const std::string& subcmd, const std::string& json);
};

#endif
