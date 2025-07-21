#ifndef _SS_APPLICATION_H_
#define _SS_APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>
#include <CLConfigLoadMgr.h>

/**
 *@brief ������Ӧ�ó�����
 */
class SSApplication : public CLApplication, public CLConfigLoadMgr, public Avalon::Singleton<SSApplication>
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

	UInt32 GetMaxPlayerBuffCacheNum() const { return m_MaxPlayerBuffCacheNum; }

public:
	bool IsCounterLog() { return m_IsCounterLog; };

	bool IsCheckAntiAddict() { return m_CheckAntiAddict; }
	UInt32 GetTouLoginInterval() { return m_TourLoginInterval; }
	UInt32 GetTourOnlineTime() { return m_TourOnlineTime; }
	UInt32 GetAntiAddictTipTime() { return m_AntiAddictTipTime; }
	void LoadAntiAddictNotifyTime(const std::string& tourTime, const std::string& noAdultTime);
	UInt32 IsNotifyTourAntiAddict(UInt32 oddTime);
	UInt32 IsNotifyNoAdultAntiAddict(UInt32 oddTime);

	bool UseNewOfflineCharge() const { return m_NewOfflineCharge; }

	bool IsCloseAbnormalTransHandle() const { return !m_IsHandleAbnormalTrans; }

	bool IsCloseDevil() const { return m_IsCloseDevil; }

	const std::string& GetStrengthenTicketPlanRecordPath() const { return m_StrenTicketPlanRecordFilePath; }
	const std::string& GetArtifactJarRecordPath() const { return m_ArtifactJarRecordFilePath; }

	UInt64 GetStrengthenTicketFuseUseMaxTime() const { return m_StrenTicketFuseUseMaxTime; }

	bool OpenSaveDataAfterCharge() const { return m_IsOpenChargeSaveData; }

	bool IsUseFreezeAbnormalNew() const { return m_IsUseFreezeAbnormalNew; }
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
	*@brief ����ͳ���ļ�·��
	*/
	std::string GetEnhanceCountPath() const { return m_EnhanceCountPath; }

	/*
	*@brief ����ת��ͳ���ļ�·��
	*/
	std::string GetBeadConvertCountPath() const { return m_BeadConvertCountPath; }

	/*
	*@brief ��ֲͳ���ļ�·��
	*/
	std::string GetPlantCountPath() const { return m_PlantPath; }

	std::string GetComposeCardPath() const { return m_ComposeCardPath; }
	
	std::string GetGiftCountPath() const { return m_GiftCountPath; }

	/*
	*@brief �Ƿ񿪷����¿�ʼ���³�
	*/
	bool IsOpenRestartDungeon() const { return m_OpenRestartDungeon; }

	/*
	*@brief �Ƿ��鿪ʼ���³�Ƶ��
	*/
	bool IsCheckStartDungeonFrequence() const { return m_IsCheckStartDungeonFrequence; }

	/*
	*@brief ͬһ���ڿ�ʼ���³���Ϣ����
	*/
	UInt32 GetMaxStartDungeonNumInSecond() const { return m_MaxStartDungeonNumInSecond; }

	/*
	*@brief �Ƿ����������
	*/
	bool IsCheckJarType() const { return m_CheckJarType; }

	/*
	*@brief ǿ���ܿ��Ĺ���
	*/
	bool IsSpecialJar(UInt32 id) const { return std::find(m_CanOpenJarIds.begin(), m_CanOpenJarIds.end(), id) != m_CanOpenJarIds.end(); }

	/*
	*@brief �Ƿ����µ��̵�ˢ������
	*/
	bool IsUseNewShopRefresh() const { return m_NewShopRefesh; }

	/*
	*@brief �Ƿ����µ�������������
	*/
	bool IsNewChatCheck() const { return m_NewChatCheck; }

	/*
	*@brief �Ƿ����µ�������������
	*/
	bool IsOpenMoveCheck() const { return m_OpenMoveCheck; }

	/*
	*@brief �Ƿ�����ϸ��־
	*/
	bool IsOpenDetailLog() const { return m_OpenDetailLog; }
	
	/**
	*@brief ��ȡ����������pvp�ӳ�ϵ��
	*/
	UInt32	GetIndpentPvpRatio() const{ return m_IndpentAtlPvpRatio; }
	void	SetIndpentPvpRatio(UInt32 value) { m_IndpentAtlPvpRatio = value; }

	/**
	*@brief �ͻ�����־��󳤶�
	*/
	UInt32 GetClientLogMaxLength() const { return m_ClientLogMaxLength; }

	/**
	*@brief ���ǿ�����
	*/
	bool CheckStrengthenResult() const { return m_CheckStrengthenResult; }

	/**
	*@brief �Ƿ������ܷ���
	*/
	bool OpenPerf() const { return m_OpenPerf; }

	/**
	*@brief �Ƿ���������������
	*/
	bool OpenFastSellBat() const { return m_OpenFastSellBat; }

	/**
	*@brief �Ƿ���PlayerOnOnline�Ż�
	*/
	bool OpenPlayerOnOnline() const { return m_OpenPlayerOnOnline; }

	/**
	*@brief �µ��������
	*/
	bool NewHeartbeat() const { return m_NewHeartbeat; }


	/**
	*@brief ������ͬ������
	*/
	bool OpenNewSync() { return m_IsOpenNewSync; }

private:
	/**
	 *@brief �����������ݱ�
	 */
	bool LoadDataTables();

	/**
	*@brief ���¼������ݱ�
	*/
	void ReloadDataTables();

	/**
	*@brief ���ظ��ֶ�̬����
	*/
	void LoadDynamicConfig();

	/**
	*@brief ע����Ҫ���صı�
	*/
	virtual void RegisterReloadDataTables();

private:
	//������ͬ������
	bool  m_IsOpenNewSync = false;

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
	// �������ο͵�¼���
	UInt32	m_TourLoginInterval;
	// �������ο�����ʱ��
	UInt32	m_TourOnlineTime;
	// ��������ǰ��ʾʱ��
	UInt32  m_AntiAddictTipTime;
	// �������ο���ʾʱ��
	std::vector<UInt32> m_AddictTourNotifyTimeVec;
	// ������δ������ʾʱ��
	std::vector<UInt32> m_AddictNoAdultNotifyTimeVec;

	// ��Ҷ��󻺴�ʱ��(Sec)
	UInt32	m_PlayerBuffCacheTime;

	// ��Ҷ�����󻺴����
	UInt32	m_MaxPlayerBuffCacheNum;

	// �µ����߲���
	bool	m_NewOfflineCharge;

	// �Ƿ����쳣����
	bool	m_IsHandleAbnormalTrans;

	// �Ƿ������µĽ�Ҷ��ᶨʱ
	bool	m_IsUseFreezeAbnormalNew;

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

	// ����ͳ���ļ�·��
	std::string		m_EnhanceCountPath;

	// ����ת��ͳ���ļ�·��
	std::string		m_BeadConvertCountPath;

	// ֲ��ͳ���ļ�·��
	std::string     m_PlantPath;

	std::string     m_ComposeCardPath;

	std::string		m_GiftCountPath;
	
	// Tick��ʱ����200ms
	Avalon::SimpleTimer				m_TickTimer200ms;
	// Tick��ʱ����500ms
	Avalon::SimpleTimer				m_TickTimer500ms;
	// Tick��ʱ����1000ms
	Avalon::SimpleTimer				m_TickTimer1000ms;
	// Tick��ʱ����1����
	Avalon::SimpleTimer				m_TickTimerMin;
	// Tick��ʱ����1��
	Avalon::SimpleTimer				m_TickTimerMonth;

	// �Ƿ������¿�ʼ���³�
	bool							m_OpenRestartDungeon;
	// �Ƿ��鿪ʼ���³�Ƶ��
	bool							m_IsCheckStartDungeonFrequence;
	// ͬһ���ڿ�ʼ���³���Ϣ����
	UInt32							m_MaxStartDungeonNumInSecond;

	// ����������
	bool							m_CheckJarType;
	// ǿ���ܿ��Ĺ���ID
	std::vector<UInt32>				m_CanOpenJarIds;

	// �µ��̵�ˢ������
	bool							m_NewShopRefesh;
	// �Ƿ����µ�������������
	bool							m_NewChatCheck;
	// �Ƿ����ƶ����
	bool							m_OpenMoveCheck;
	// �Ƿ�����ϸ��־
	bool							m_OpenDetailLog;
	// ����������PVPϵ��
	UInt32							m_IndpentAtlPvpRatio;

	// �ͻ�����־����
	UInt32							m_ClientLogMaxLength;
	// ���ǿ�����
	bool							m_CheckStrengthenResult;

	// ���ܷ���
	bool							m_OpenPerf;

	//������������
	bool							m_OpenFastSellBat;

	//PlayerOnOnline�Ż�
	bool							m_OpenPlayerOnOnline;

	// �µ��������
	bool							m_NewHeartbeat;

	// �µ����ر�����
	bool							m_NewReloadTable;
};


#endif
