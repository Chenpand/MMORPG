#ifndef _WS_CONSOLE_AGENT_H_
#define _WS_CONSOLE_AGENT_H_

#include <AvalonSingleton.h>
#include <ConsoleAgent.h>

/**
*@brief ����̨����
*/
class GASConsoleAgent : public ConsoleAgent, public Avalon::Singleton<GASConsoleAgent>
{
public:
	GASConsoleAgent();
	~GASConsoleAgent();

public:
	void Init();
	void Final();

public:
	void DoSendResponse(UInt32 connid, const std::string& cmd, const std::string& subcmd, const std::string& json);
};

#endif
