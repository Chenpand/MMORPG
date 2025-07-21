#ifndef _BS_APPLICATION_H_
#define _BS_APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>
#include <AvalonRetryDBConnection.h>

/**
 *@brief ս��������Ӧ�ó�����
 */
class BSApplication : public CLApplication, public Avalon::Singleton<BSApplication>
{
public:
	BSApplication();
	~BSApplication();

public:
	bool OnInit();

	void OnTick(const Avalon::Time& now);

	void OnQuit();

	virtual void OnSignal(int signal);

public:
	Avalon::RetryDBConnection* GetDBConnection() const{ return m_DBConn; }

	UInt32 GetRobotFlag(void) { return m_hasRobot; }

	UInt32 GetNoWarCD() { return m_NoWarCD; }

	/**
	*@brief ս�����
	*/
	UInt32 GetBattleFlag() const { return m_BattleFlag; }

private:
	/**
	*@brief ���ظ��ֶ�̬����
	*/
	void LoadDynamicConfig();

	/**
	*@brief �����������ݱ�
	*/
	bool LoadDataTables();

	/**
	*@brief ���¼������ݱ�
	*/
	bool ReloadDataTables();

private:
	//���¼�������
	bool	m_ReloadConfig;
	
	//tickʱ��
	UInt64	m_TickTime;
	//���ݿ�����
	Avalon::RetryDBConnection* m_DBConn;

	//���������
	UInt32 m_hasRobot;

	// ��սCD
	UInt32 m_NoWarCD;

	// ս�����
	UInt32			m_BattleFlag;
};

#endif
