#ifndef _LS_APPLICATION_H_
#define _LS_APPLICATION_H_

#include <AvalonServerApplication.h>
#include <AvalonSingleton.h>

/**
 *@brief 日志服务器应用程序类  noted by aprilliu  也就是DataLogServer服务器
 */
class LSApplication : public Avalon::ServerApplication, public Avalon::Singleton<LSApplication>
{
public:
	LSApplication();
	~LSApplication();

public:
	bool OnInit();

	void OnTick(const Avalon::Time& now);

	void OnQuit();

private:
};

#endif
