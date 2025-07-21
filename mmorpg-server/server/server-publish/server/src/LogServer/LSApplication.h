#ifndef _LS_APPLICATION_H_
#define _LS_APPLICATION_H_

#include <AvalonServerApplication.h>
#include <AvalonSingleton.h>

/**
 *@brief ��־������Ӧ�ó�����  noted by aprilliu  Ҳ����DataLogServer������
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
