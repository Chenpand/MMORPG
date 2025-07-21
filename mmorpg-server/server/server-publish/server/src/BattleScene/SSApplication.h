#ifndef _SS_APPLICATION_H_
#define _SS_APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>

/**
 *@brief ������Ӧ�ó�����
 */
class SSApplication : public CLApplication, public Avalon::Singleton<SSApplication>
{
public:
	SSApplication();
	~SSApplication();

public:
	bool OnInit();

	void OnTick(const Avalon::Time& now);

	void OnQuit();

	virtual void OnSignal(int signal);

	UInt32 GetSceneMgrTickIntervalMs() const { return m_SceneMgrTickIntervalMSec; }

	UInt32 GetPlayerBuffCacheTime() const { return m_PlayerBuffCacheTime; }

public:
	bool IsCounterLog() { return m_IsCounterLog; };

	bool IsCheckAntiAddict() { return m_CheckAntiAddict; }

	bool UseNewOfflineCharge() const { return m_NewOfflineCharge; }

	bool IsCloseAbnormalTransHandle() const { return !m_IsHandleAbnormalTrans; }

	bool IsCloseDevil() const { return m_IsCloseDevil; }

	const std::string& GetStrengthenTicketPlanRecordPath() const { return m_StrenTicketPlanRecordFilePath; }
	const std::string& GetArtifactJarRecordPath() const { return m_ArtifactJarRecordFilePath; }

	UInt64 GetStrengthenTicketFuseUseMaxTime() const { return m_StrenTicketFuseUseMaxTime; }

	bool OpenSaveDataAfterCharge() const { return m_IsOpenChargeSaveData; }


	/**
	*@brief �ƶ���ͬ��Ƶ��(ms)
	*/
	UInt32 GetMovePacketIntervalMSec() const { return m_MovePacketIntervalMSec; }

	/**
	*@brief ������ע�б����Ƶ��(ms)
	*/
	UInt32 GeWatchListUpdateIntervalMSec() const { return m_WatchListUpdateIntervalMSec; }

	/*
	*@brief ͳ���ļ�·��
	*/
	std::string GetJarCountPath() const { return m_JarCountPath; }

	/*
	*@brief �Լ��ɼ�������
	*/
	UInt32 GetChiJiChoiceSkillNum() { return m_ChiJiChoiceSkillNum; }

	/*
	*@brief �Լ��ͻ����ƶ������ͼ��(ms)
	*/
	UInt32 GetChiJiMovePacketIntervalMs() { return m_ChiJiMovePacketIntervalMs; }

	/*
	*@brief �ǲ��ǵ����˺�
	*/
	bool IsDebugAccount(UInt32 accid) { return std::find(m_DebugAccounts.begin(), m_DebugAccounts.end(), accid) != m_DebugAccounts.end(); }

private:
	/**
	 *@brief �����������ݱ�
	 */
	bool LoadDataTables();

	/**
	*@brief ���¼������ݱ�
	*/
	void ReloadDataTables();

	// ���ظ��ֶ�̬����
	void LoadDynamicConfig();

private:
	//tickʱ��
	UInt64	m_TickTime;

	//���¼�������
	bool	m_ReloadConfig;

	// �Ƿ�Counter��־
	bool	m_IsCounterLog;

	// SceneMgr��Tick���ʱ��(ms)
	UInt32	m_SceneMgrTickIntervalMSec;

	// �Ƿ��������
	bool	m_CheckAntiAddict;

	// ��Ҷ��󻺴�ʱ��(Sec)
	UInt32	m_PlayerBuffCacheTime;

	// �µ����߲���
	bool	m_NewOfflineCharge;

	// �Ƿ����쳣����
	bool	m_IsHandleAbnormalTrans;

	//���¼��ر��
	bool	m_ReloadTable;

	// �ر����ؿ�
	bool	m_IsCloseDevil;

	// ǿ��ȯ������¼�ļ�·��
	std::string m_StrenTicketPlanRecordFilePath;
	// ������¼�ļ�·��
	std::string m_ArtifactJarRecordFilePath;

	// ǿ��ȯ�ںϺ�ʱ���ʱ��
	UInt64 m_StrenTicketFuseUseMaxTime;

	// �Ƿ�����ֵ�󱣴�����
	bool	m_IsOpenChargeSaveData;

	// �ƶ���ͬ��Ƶ��(ms)
	UInt32	m_MovePacketIntervalMSec;

	// ������ע�б����Ƶ��(ms)
	UInt32	m_WatchListUpdateIntervalMSec;

	// ����ͳ���ļ�·��
	std::string		m_JarCountPath;

	// �Լ��ɼ�������
	UInt32	m_ChiJiChoiceSkillNum;

	// �Լ��ͻ����ƶ������ͼ��(ms)
	UInt32	m_ChiJiMovePacketIntervalMs;

	// Tick��ʱ����200ms
	Avalon::SimpleTimer				m_TickTimer200ms;
	// Tick��ʱ����500ms
	Avalon::SimpleTimer				m_TickTimer500ms;
	// Tick��ʱ����1000ms
	Avalon::SimpleTimer				m_TickTimer1000ms;

	// �������
	std::vector<UInt32>				m_DebugAccounts;
};


#endif
