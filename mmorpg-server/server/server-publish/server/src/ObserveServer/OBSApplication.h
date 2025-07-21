#ifndef _OBS_APPLICATION_H_
#define _OBS_APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>

/**
*@brief 管理服应用程序类
*/
class OBSApplication : public CLApplication, public Avalon::Singleton<OBSApplication>
{
public:
	OBSApplication();
	~OBSApplication();

public:
	bool OnInit();

	void OnTick(const Avalon::Time& now);

	void OnQuit();

private:
    /**
    *@brief 加载所有数据表
    */
    bool LoadDataTables();
};

#endif
