#ifndef _US_CONSOLE_AGENT_H_
#define _US_CONSOLE_AGENT_H_

#include <AvalonSingleton.h>
#include <ConsoleAgent.h>

/**
*@brief 控制台代理
*/
class USConsoleAgent : public ConsoleAgent, public Avalon::Singleton<USConsoleAgent>
{
public:
	USConsoleAgent();
	~USConsoleAgent();

public:
	void Init();
	void Final();

public:
	void DoSendResponse(UInt32 connid, const std::string& cmd, const std::string& subcmd, const std::string& json);
};

#endif
