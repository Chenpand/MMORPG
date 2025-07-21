#ifndef _CS_APPLICATION_H_
#define _CS_APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>
#include <AvalonRetryDBConnection.h>

/**
 *@brief 挑战服务器应用程序类
 */
class CSApplication : public CLApplication, public Avalon::Singleton<CSApplication>
{
public:
	CSApplication();
	~CSApplication();

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
