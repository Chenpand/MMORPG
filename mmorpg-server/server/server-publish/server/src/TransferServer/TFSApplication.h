#ifndef _TFS_APPLICATION_H_
#define _TFS_APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>

/**
 *@brief 中转服务器应用程序类
 */
class TFSApplication : public CLApplication, public Avalon::Singleton<TFSApplication>
{
public:
	TFSApplication();
	~TFSApplication();

public:
	bool OnInit();

	void OnTick(const Avalon::Time& now);

	void OnQuit();

	virtual void OnSignal(int signal);

private:
	/**
	*@brief 加载各种动态配置
	*/
	void LoadDynamicConfig();

private:

	//tick时间
	UInt64	m_TickTime;
	// 重载配置
	bool m_ReloadConfig;
};

#endif
