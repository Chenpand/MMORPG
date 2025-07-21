#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>
#include <AvalonRetryDBConnection.h>

/**
 *@brief ���а������Ӧ�ó�����
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
	
	//tickʱ��
	UInt64	m_TickTime;
	//���ݿ�����
	Avalon::RetryDBConnection* m_DBConn;
};

#endif
