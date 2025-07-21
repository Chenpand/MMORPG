#ifndef _DS_APPLICATION_H_
#define _DS_APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>
#include <AvalonRetryDBConnection.h>

/**
 *@brief ��ս������Ӧ�ó�����
 */
class DSApplication : public CLApplication, public Avalon::Singleton<DSApplication>
{
public:
	DSApplication();
	~DSApplication();

public:
	bool OnInit();

	void OnTick(const Avalon::Time& now);

	void OnQuit();

	/**
	*@brief �źŴ���
	*/
	virtual void OnSignal(int signal);

public:
	/**
	*@brief �Ƿ����ظ�������³�
	*/
	bool IsCheckRepeatStartDungeon() const { return m_CheckRepeatStartDungeon; }

	/**
	*@brief �Ƿ���֤md5
	*/
	bool IsCheckMD5() const { return m_CheckMd5; }

	/**
	*@brief �Ƿ��¼���³���־
	*/
	bool IsRecordDungeonLog() const { return m_IsRecordDungeonLog; }

	/**
	*@brief �Ƿ��¼���³�¼��
	*/
	bool IsRecordDungeonReplay() const { return m_IsRecordDungeonReplay; }

	/**
	*@brief ʰȡ����ʱ����ϱ���֡
	*/
	bool CheckPickDropFrame() const { return m_CheckPickDropFrame; }
private:
	/**
	*@brief ���ض�̬������
	*/
	void LoadDynamicConfig();

	/**
	*@brief �����������ݱ�
	*/
	bool LoadDataTables();

private:
	//tickʱ��
	UInt64			m_TickTime;
	
	//�Ƿ����ظ�������³�
	bool			m_CheckRepeatStartDungeon;

	// �Ƿ��md5���
	bool			m_CheckMd5;
	
	// �Ƿ��¼���³���־
	bool			m_IsRecordDungeonLog;

	// �Ƿ��¼���³�¼��
	bool			m_IsRecordDungeonReplay;

	//���¼�������
	bool			m_ReloadConfig;

	// ʰȡ����ʱ����ϱ���֡
	bool			m_CheckPickDropFrame;
};

#endif
