#ifndef _OS_APPLICATION_H_
#define _OS_APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>

/**
 *@brief 社会服应用程序类
 */
class OSApplication : public CLApplication, public Avalon::Singleton<OSApplication>
{
public:
	OSApplication();
	~OSApplication();

public:
	bool OnInit();

	void OnTick(const Avalon::Time& now);

	void OnQuit();

private:

	//上次tick时间
	UInt64		m_TickTime;
};

#endif
