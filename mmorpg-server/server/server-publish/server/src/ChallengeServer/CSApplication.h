#ifndef _CS_APPLICATION_H_
#define _CS_APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>
#include <AvalonRetryDBConnection.h>

/**
 *@brief ��ս������Ӧ�ó�����
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
	
	//tickʱ��
	UInt64	m_TickTime;
	//���ݿ�����
	Avalon::RetryDBConnection* m_DBConn;
};

#endif
