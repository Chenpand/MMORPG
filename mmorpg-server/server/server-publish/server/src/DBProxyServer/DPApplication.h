#ifndef _DP_APPLICATION_H_
#define _DP_APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>

/**
 *@brief dbproxy������Ӧ�ó�����
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
	
	//tickʱ��
	UInt64	m_TickTime;
};

#endif
