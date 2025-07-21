#ifndef _DP_APPLICATION_H_
#define _DP_APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>

/**
 *@brief dbproxy服务器应用程序类
 */
class Application : public CLApplication, public Avalon::Singleton<Application>
{
public:
	Application();
	~Application();

public:
	bool OnInit();

	void OnTick(const Avalon::Time& now);

	void OnQuit();

private:
	
	//tick时间
	UInt64	m_TickTime;
};

#endif
