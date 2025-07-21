#ifndef _WS_APPLICATION_H_
#define _WS_APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>
#include <CLConfigLoadMgr.h>

// ���׳ͷ�����
enum CheatPunnishType
{
	// ֻ��¼��־
	CHEAT_PUNISH_LOG,	
	// ������
	CHEAT_PUNISH_KICK,
	// �����߲����
	CHECK_PUNISH_FORBID_LOGIN
};

/**
 *@brief �����Ӧ�ó�����
 */
class WSApplication : public CLApplication, public CLConfigLoadMgr, public Avalon::Singleton<WSApplication>
{
public:
	WSApplication();
	~WSApplication();

	/**
	 *@brief ��ȡ֧��ǩ����Կ
	 */
	const std::string& GetBillingSigKey() const{ return m_BillingSigKey; }

	/**
	*@brief ��ȡ�������ɳ����������
	*/
	UInt32 GetMaxPlayerNum() const { return m_MaxOnlinePlayer; }

	/**
	*@brief ��ȡ���������ͬʱ��������
	*/
	UInt32 GetMaxLoadingPlayerNum() const { return m_MaxLoadingPlayer; }

	/**
	*@brief �Ƿ����ظ�������³�
	*/
	bool IsCheckRepeatStartDungeon() const { return m_CheckRepeatStartDungeon; }

	/**
	*@brief �Ƿ�رշ�ֹ4�˶��鹦��
	*/
	bool IsCloseFixTeamMatch() const { return m_CloseFixTeamMatch; }	
	
	/**
	*@brief �Ƿ�����Զ��߶ӳ�����
	*/
	bool IsTeamAutoKick() const { return m_TeamAutoKick; }	

	/**
	*@brief �Ƿ����ļ����
	*/
	bool IsHireTurn() const { return m_HireTurn; }

	/**
	*@brief �Ƿ����ظ�������³�
	*/
	bool IsOpenVerifyDungeon() const { return m_OpenVerifyDungeon; }

	/**
	*@brief �Ƿ��ⶩ����ȫ��
	*/
	bool IsCheckChargeOrder() const { return m_CheckChargeOrder; }

	/**
	*@brief �Ƿ�����¼�Ŷӹ���
	*/
	bool IsOpenLoginQueue() const { return m_OpenLoginQueue; }

	/**
	*@brief ���׳ͷ�
	*/
	CheatPunnishType GetCheatPunish() const { return (CheatPunnishType)m_CheatPunish; }

	/**
	*@brief ��֤���׳ͷ�
	*/
	CheatPunnishType GetVerifyCheatPunish() const { return (CheatPunnishType)m_VerifyCheatPunishType; }

	/**
	*@brief ��̬��ֵ����ʱ�Ƿ�����
	*/
	bool KickWhenStaticValueCheckError() const { return m_KickWhenStaticValueCheckError; }

	/**
	*@brief ����֮������ʱ����T��
	*/
	UInt32 TowerKickWhenCheat() const { return m_TowerKickWhenCheat; }

	/**
	*@brief ���ÿ�տ����״���
	*/
	UInt32 DevilddomDailyCheatNum() const { return m_DevilddomDailyCheatNum; }

	/**
	*@brief ������³�ÿ�տ����״���
	*/
	UInt32 GuildDungeonDailyCheatNum() const { return m_GuildDungeonDailyCheatNum; }

	/**
	*@brief �ŶӸ���ÿ�տ����״���
	*/
	UInt32 TeamCopyDailyCheatNum() const { return m_TeamCopyDailyCheatNum; }

	/**
	*@brief �Ŷ����׷��ʱ��
	*/
	UInt32 GetTeamCopyCheatForbidTime(UInt32 num);

	/**
	*@brief ��ͬ�����ٴ�֮���T��
	*/
	UInt32 GetKickCheatNum() const { return m_KickCheatNum; }

	/**
	*@brief �Ƿ��¼���³���־
	*/
	bool IsRecordDungeonLog() const { return m_IsRecordDungeonLog; }

	/**
	*@brief �Ƿ��¼���³�¼��
	*/
	bool IsRecordDungeonReplay() const { return m_IsRecordDungeonReplay; }

	/**
	*@brief �Ƿ��¼pk¼��
	*/
	bool IsRecordPkReplay() const { return m_IsRecordPkReplay; }

	/**
	*@brief ʹ�ü��ٹ�T��
	*/
	bool KickWhenSpeedCheat() const { return m_KickWhenSpeedCheat; }

	/**
	*@brief ���ż���ϱ���֡
	*/
	bool CheckEnterAreaFrame() const { return m_CheckEnterAreaFrame; }

	/**
	*@brief �ڼ������俪ʼ����ϱ�֡
	*/
	UInt32 GetCheckAreaStartIndex() const { return m_CheckAreaStartIndex; }

	/**
	*@brief ʰȡ����ʱ����ϱ���֡
	*/
	bool CheckPickDropFrame() const { return m_CheckPickDropFrame; }

	/**
	*@brief �Ƿ���֤md5
	*/
	bool IsCheckMD5() const { return m_CheckMd5; }

	/**
	*@brief �Ƿ��뿪ս�������Ŷ�����
	*/
	bool StayTeamWhenLeaveRace() const { return m_StayTeamWhenLeaveRace; }

	/**
	*@brief �Ƿ����������
	*/
	bool IsOpenRedPacket() const { return m_OpenRedPacket; }

	/**
	*@brief �Ƿ���Ҫת���˺�
	*/
	bool IsNeedConvertAccount(const std::string& pf) const { return std::find(m_ConvertPfs.begin(), m_ConvertPfs.end(), pf) != m_ConvertPfs.end(); }

	/**
	*@brief �°����ص�ַ
	*/
	const std::string& GetDownloadUrl() const { return m_DownloadUrl; }

	/**
	*@brief ��Ҫ��¼�ĵ������Ʒ��
	*/
	UInt8 GetNeedRecordDropQuality() const { return m_NeedRecordDropQuality; }

	/**
	*@brief ��Ҫ��¼�ĵ������Ʒ��
	*/
	UInt32 GetGMQueryPlayerMaxNum() const { return m_GMQueryPlayerMaxNum; }
	
	/**
	*@brief �Ƿ񳬼��·�
	*/
	bool IsSuperNewService() const { return m_SuperNewServiceFlag; }

	/**
	*@brief ��ȡ�����·���б�
	*/
	std::vector<UInt32>& GetSuperNewServiceOpActs() { return m_SuperNewServiceOpActs; }

	/**
	*@brief �Ƿ���gm���ݼ������
	*/
	bool IsCheckGmDataLoadSituation() { return m_CheckGmDataLoadSituation; }

	/*
	*@brief �Ƿ������UpdateRelation��������
	*/
	bool IsCheckUpDateRelation() { return m_CheckUpdateRelation; }

	/*
	*@brief �Ƿ��Ż�����UpdateRelation��������
	*/
	bool IsOptimUpdateRelation() { return m_OptimUpdateRelation; }

	/*
	*@brief ����ͳ���ļ�·��
	*/
	std::string GetDropCountPath() const { return m_DropCountPath; }

	/*
	*@brief ����ͳ���ļ�·��
	*/
	std::string GetDigCountPath() const { return m_DigCountPath; }

	UInt32	GetAuctionQueryPriceListInterval() { return m_AuctionQueryPriceListInterval; }

	bool GetAuctionSqlJointModify() { return m_AuctionSqlJointModify; }

	bool GetAuctionSqlInModify() { return m_AuctionSqlInModify; }

	UInt32 GetAuctionPriceListCacheTime() { return m_AuctionPriceListCacheTime; }

	bool IsAccountDataFlush() { return m_AccountDataFlush; }

	UInt64 GetBattleFlag() const { return m_BattleFlag; }

	UInt32 GetAuctionDueOffSaleTime() { return m_AuctionDueOffSaleTime; }

	bool NotifyGiveupVerifyWhenRaceUnexist() { return m_NotifyGiveupVerifyWhenRaceUnexist; }

	bool CheckDevilDropBug() const { return m_DevilDropBug; }
	
	/**
	*@brief �Ƿ��ӳ�ˢ���˺��ܳ����ݵȵ�
	*/
	bool IsAccountDelayedRefresh() const { return m_AccountDelayedRefreshFlag; }

	bool IsNewDailyDrop() const { return m_NewDailyDrop; }

	bool IsCheckChargeAccount() const { return m_CheckChargeAccount; }

	bool IsAdjustOnOnline() const { return m_AdjustOnOnline; }
	
	/*
	*@brief ͳ�Ƶ����ļ�·��
	*/
	std::string GetDropItemCountPath() const { return m_DropItemCountPath; }

	/**
	*@brief �Ƿ��¼�ű���־
	*/
	bool IsLogTeamCopyProtocol() { return m_LogTeamCopyProtocol; }

	/**
	*@brief �Ƿ����������ɫ��Ϣ
	*/
	bool IsLoadOtherRoleInfo() { return m_LoadOtherRoleInfo; }

	/**
	*@brief ��ȡ�������ʱ�Ƿ����ɫ�ȼ�
	*/
	bool IsCheckLevelWhenFlyupGift() { return m_CheckLevelWhenFlyupGift; }

	/**
	*@brief ����ս������������
	*/
	UInt32 GetGuildBattleScoreRank() { return m_GuildBattleScoreRank; }

	UInt32 GetTableCount() { return m_databaseTableCount; }

	/**
	*@brief �Ƿ�ʹ��������������Ʒ��������
	*/
	bool IsUseAuctionNumCache() const { return m_IsUseAuctionNumCache; }

	/**
	*@brief ������������Ʒ��������ʱ��
	*/
	UInt32	GetAuctionNumCacheTime() const{ return m_AuctionNumCacheTime; }
	void SetAuctionNumCacheTime(UInt32 time) { m_AuctionNumCacheTime = time; }

	/**
	*@brief �Ƿ���Ѽ�����
	*/
	bool GetPrintRelationsUid() { return m_PrintRelationsUid; }

	/**
	*@brief �����в������ڴ�
	*/
	bool IsAuctionOpUseMem() { return m_AuctionOpUseMem; }

	/**
	*@brief �Ƿ�ر�ʦͽ���߷���ʦ������
	*/
	bool IsCloseMasterOffPubTask() const { return m_CloseMasterOffPubTask; }

	/**
	*@brief �Ƿ�ر����ߺ�������
	*/
	bool IsCloseOffPresentGive() const { return m_CloseOffPresentGive; }
public:
	bool OnInit();

	void OnTick(const Avalon::Time& now);

	void OnQuit();

	/**
	*@brief �źŴ���
	*/
	virtual void OnSignal(int signal);

	/**
	 *@brief ���ܣ�����ʱҲ���ж��Ƿ����
	 */
	void OnWeekChanged();

	/**
	 *@brief ����6�㣬����ʱҲ���ж��Ƿ����
	 */
	void OnMonthChanged();
	

	/**
	*@brief ִ�п����������Ƿ�����
	*/
	bool IsOnChangedConditionReady();

	/**
	 *@brief ���죬����ʱҲ���ж��Ƿ����
	 */
	void OnDayChanged();

	/**
	*@brief ���¼������ݱ�
	*/
	void ReloadDataTables();

	std::string GetHellMonsterCountPath() const { return m_HellMonsterCountPath; }

	/**
	*@brief �������³ǽ��㲹�����У��
	*/
	bool IsOpenDungeonRaceEndChecksum() const {	return m_OpenDungeonRaceEndChecksum; }

public:
	void IncInitDataLoadCount(){ ++m_InitDataLoadCount; }
	void DecInitDataLoadCount(){ if(m_InitDataLoadCount > 0) --m_InitDataLoadCount; }

private:
	/**
	*@brief ���ض�̬������
	*/
	void LoadDynamicConfig();

	/**
	 *@brief �ȴ����ݿ�������
	 */
	bool WaitDataLoadFinished();

	/**
	 *@brief �����������ݱ�
	 */
	bool LoadDataTables();

	/**
	*@brief ע����Ҫ���صı�
	*/
	virtual void RegisterReloadDataTables();

	/**
	 *@brief ���ܺͿ���
	 */
	void CheckTime(const Avalon::Time& now);	
	
	/**
	 *@brief ֪ͨUnion��id�ı�
	 */
	void NotifyUnionZoneIDChanged();


private:
	//��ʼ���ݼ�����
	UInt32			m_InitDataLoadCount;
	//֧��ǩ��key
	std::string		m_BillingSigKey;
	//�ϴ�tickʱ��
	UInt64			m_TickTime;
	//�������
	UInt32			m_MaxOnlinePlayer;
	//�������
	UInt32			m_MaxLoadingPlayer;
	//���¼�������
	bool			m_ReloadConfig;
	//�Ƿ�رշ�ֹ4�˶��鹦��
	bool            m_CloseFixTeamMatch;
	//�Ƿ����ظ�������³�
	bool			m_CheckRepeatStartDungeon;
	//�Ƿ��ⶩ����ȫ��
	bool			m_CheckChargeOrder;
	// �Ƿ����Ŷӹ���
	bool			m_OpenLoginQueue;
	// ���׳ͷ�
	UInt8			m_CheatPunish;
	// �Ƿ��md5���
	bool			m_CheckMd5;
	// �Ƿ����±��
	bool			m_ReloabTable;
	// �Ƿ��뿪ս�������Ŷ�����
	bool			m_StayTeamWhenLeaveRace;
	// ��̬��ֵ����ʱ�Ƿ�����
	bool			m_KickWhenStaticValueCheckError;

	// �Ƿ�����֤
	bool			m_OpenVerifyDungeon;
	// ��֤���׺�Ĵ���
	UInt8			m_VerifyCheatPunishType;
	// ����֮������ʱ����T��
	bool			m_TowerKickWhenCheat;
	// ���ÿ������״���
	UInt32			m_DevilddomDailyCheatNum;
	// ������³�ÿ������״���
	UInt32			m_GuildDungeonDailyCheatNum;
	// �ŶӸ���ÿ������״���
	UInt32			m_TeamCopyDailyCheatNum;
	// �ű����׷��ʱ��
	std::vector<UInt32>	m_TeamCopyCheatForbidTime;

	// ��ͬ�����ٴ�֮���T��
	UInt32			m_KickCheatNum;
	// �Ƿ��¼���³���־
	bool			m_IsRecordDungeonLog;
	// �Ƿ��¼���³�¼��
	bool			m_IsRecordDungeonReplay;
	// �Ƿ��¼pk¼��
	bool			m_IsRecordPkReplay;
	// ʹ�ü��ٹ��Ƿ�����
	bool			m_KickWhenSpeedCheat;
	// ���ż���ϱ���֡
	bool			m_CheckEnterAreaFrame;
	// �ڼ������俪ʼ����ϱ�֡
	UInt32			m_CheckAreaStartIndex;
	// ʰȡ����ʱ����ϱ���֡
	bool			m_CheckPickDropFrame;
	// �Ƿ񿪷�������
	bool			m_OpenRedPacket;
	// ��Ҫ�˺�ת�Ƶ�����
	std::vector<std::string>	m_ConvertPfs;
	// �±����ص�ַ
	std::string					m_DownloadUrl;
	// ��Ҫ��¼�ĵ������Ʒ��
	UInt8						m_NeedRecordDropQuality;
	// gm��������ѯ�����
	UInt32			m_GMQueryPlayerMaxNum;
	// �����·����
	bool			m_SuperNewServiceFlag;
	// �����·��
	std::vector<UInt32> m_SuperNewServiceOpActs;
	// �Ƿ���gm���ݼ������
	bool			m_CheckGmDataLoadSituation;
	// �Ƿ������UpDateRelation��������
	bool			m_CheckUpdateRelation;

	//�Ƿ��Ż�UpDateRelation��������
	bool			m_OptimUpdateRelation;

	// ����ͳ���ļ�·��
	std::string		m_DropCountPath;

	// �������ļ�·��
	std::string		m_DigCountPath;

	//�����в�ѯ�۸��б�ʱ����
	UInt32			m_AuctionQueryPriceListInterval;

	//�����в�ѯ����޸�
	bool			m_AuctionSqlJointModify;

	//�����в�ѯ����޸�
	bool			m_AuctionSqlInModify;

	//�����м۸��б���ʱ��
	UInt32			m_AuctionPriceListCacheTime;

	//�����е�����Ʒ�Զ��¼�ʱ��
	UInt32			m_AuctionDueOffSaleTime;

	// Tick��ʱ����200ms
	Avalon::SimpleTimer				m_TickTimer200ms;
	// Tick��ʱ����500ms
	Avalon::SimpleTimer				m_TickTimer500ms;
	// Tick��ʱ����1000ms
	Avalon::SimpleTimer				m_TickTimer1000ms;
	// Tick��ʱ����1����
	Avalon::SimpleTimer				m_TickTimerMin;

	// �˺ű����ݲ�����ˢ��
	bool							m_AccountDataFlush;
	// ս�����
	UInt64							m_BattleFlag;
	// ��֤����ʱ������������ھ�ɾ��
	bool							m_NotifyGiveupVerifyWhenRaceUnexist;
	// ����������BUG
	bool							m_DevilDropBug;
	// �˺��ӳ�ˢ�¿���
	bool							m_AccountDelayedRefreshFlag;
	// ÿ�յ����µļ�¼��ʽ
	bool							m_NewDailyDrop;
	// ��ֵʱ����˺����ɫ�Ƿ�һ��
	bool							m_CheckChargeAccount;
	// ����ononlie˳��
	bool							m_AdjustOnOnline;
	// ����ͳ���ļ�·��
	std::string					    m_DropItemCountPath;
	// �Ƿ��¼�ű���Ϣ
	bool							m_LogTeamCopyProtocol;
	// �Ƿ����������ɫ��Ϣ
	bool							m_LoadOtherRoleInfo;
	// ��ȡ�������ʱ�Ƿ����ɫ�ȼ�
	bool							m_CheckLevelWhenFlyupGift;
	// ����ս������������
	UInt32							m_GuildBattleScoreRank;

	//���ݿ�ֱ�����
	UInt32							m_databaseTableCount;

	//����Զ��߶ӳ�����
	bool							m_TeamAutoKick;

	//��ļ����
	bool							m_HireTurn;

	//������������Ʒ���濪��
	bool							m_IsUseAuctionNumCache;
	UInt32							m_AuctionNumCacheTime;

	// �µ����ر�����
	bool							m_NewReloadTable;

	//��Ҫ��ӡ�������ݵ����id
	bool							m_PrintRelationsUid;

	//�����в������ڴ�
	bool							m_AuctionOpUseMem;

	// ����ͳ���ļ�·��
	std::string						m_HellMonsterCountPath;

	// �������³ǽ��㲹�����У��
	bool							m_OpenDungeonRaceEndChecksum;

	//ʦͽ���߷��������Ƿ�ر�
	bool							m_CloseMasterOffPubTask;

	//������������ʦ���ر�
	bool							m_CloseOffPresentGive;
};

#endif
