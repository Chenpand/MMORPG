#ifndef _VC_APPLICATION_H_
#define _VC_APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>

/**
*@brief 验证服务器
*/
class VCApplication : public CLApplication, public Avalon::Singleton<VCApplication>
{
public:
	VCApplication();
	~VCApplication();

public:
	bool OnInit();

	void OnTick(const Avalon::Time& now);

	virtual void OnSignal(int signal);

	void OnQuit();

private:
	Avalon::SimpleTimer				m_ShowAbilityTimer;
};

#endif
