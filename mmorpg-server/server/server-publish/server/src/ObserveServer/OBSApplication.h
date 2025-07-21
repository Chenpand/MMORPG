#ifndef _OBS_APPLICATION_H_
#define _OBS_APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>

/**
*@brief �����Ӧ�ó�����
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
    *@brief �����������ݱ�
    */
    bool LoadDataTables();
};

#endif
