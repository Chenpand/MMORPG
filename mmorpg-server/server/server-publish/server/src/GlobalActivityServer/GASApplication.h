#ifndef _CS_APPLICATION_H_
#define _CS_APPLICATION_H_

#include <AvalonSingleton.h>
#include <AvalonRetryDBConnection.h>
#include <CLApplication.h>

/**
 *@brief ��ս������Ӧ�ó�����
 */
class GASApplication : public CLApplication, public Avalon::Singleton<GASApplication>
{
public:
	GASApplication();
	~GASApplication();

public:
	bool OnInit();

	void OnTick(const Avalon::Time& now);

	void OnQuit();

	void OnSignal(int signal);

public:
	Avalon::RetryDBConnection* GetDBConnection() const{ return m_DBConn; }

	/**
	*@brief ���ض�̬������
	*/
	void LoadDynamicConfig();

	/**
	*@brief �����������ݱ�
	*/
	bool LoadDataTables();

	/**
	*@brief �ȴ����ݿ�������
	*/
	bool WaitDataLoadFinished();

	/**
	*@brief ���¼������ݱ�
	*/
	void ReloadDataTables();

	void IncInitDataLoadCount(){ ++m_InitDataLoadCount; }
	void DecInitDataLoadCount(){ if (m_InitDataLoadCount > 0) --m_InitDataLoadCount; }

public:
	/**
	*@brief �����ܿ���
	*/
	bool IsOpenBetHorse() { return m_BetHorseSwitch; }

	/**
	*@brief ��������������
	*/
	bool IsOpenBetHorseResend() { return m_OpenBetHorseResend; }

	const std::string& GetGmOpActivityLibUrl() const { return m_GmOpActivityLibUrl; }
	const std::string& GetGmOpActivityKey() const { return m_GmOpActivityKey; }

	UInt32 GetServerIdInGM() const { return m_ServerIdInGM; }

private:
	//��ʼ���ݼ�����
	UInt32			m_InitDataLoadCount;

	//���¼�������
	bool	m_ReloadConfig;

	//���ݿ�����
	Avalon::RetryDBConnection* m_DBConn;

	// �����ܿ���
	bool		m_BetHorseSwitch;
	// �����ܿ���
	bool		m_OpenBetHorseResend;
	//gm��Ӫ����ַ
	std::string		m_GmOpActivityLibUrl;
	//gm��Ӫ�key
	std::string		m_GmOpActivityKey;
	//gm���÷�����id
	UInt32			m_ServerIdInGM;
};

#endif
