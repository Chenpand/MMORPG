#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>
#include <AvalonRetryDBConnection.h>

/**
 *@brief 排行榜服务器应用程序类
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

public:
	Avalon::RetryDBConnection* GetDBConnection() const{ return m_DBConn; }

private:
	
	//tick时间
	UInt64	m_TickTime;
	//数据库连接
	Avalon::RetryDBConnection* m_DBConn;
};

#endif
