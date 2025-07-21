#ifndef _OS_APPLICATION_H_
#define _OS_APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>

/**
 *@brief ����Ӧ�ó�����
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

	//�ϴ�tickʱ��
	UInt64		m_TickTime;
};

#endif
