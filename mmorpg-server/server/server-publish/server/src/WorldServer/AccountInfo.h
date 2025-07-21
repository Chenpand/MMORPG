#ifndef __ACCOUNT_INFO_H__
#define __ACCOUNT_INFO_H__

#include <CLMaskProperty.h>
#include <CLDefine.h>
#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <AvalonSimpleTimer.h>
#include <AvalonObjectPool.h>
#include <CLMoneyManageDefine.h>
#include <AvalonNetConnection.h>
#include <CLChatProtocol.h>
#include <CLSysNotify.h>

#include "AccountChargeRecord.h"
#include "DungeonHardInfo.h"
#include "ExpeditionMgr.h"
#include "AccountAchievementTaskMgr.h"
#include "CLChatDefine.h"
#include "WSNetwork.h"
#include "NewTitle.h"
#include "WSPlayer.h"
#include "Monopoly.h"


#define ACCOUNTINFO(T) \
	"Account(" << T->GetID() << "," << T->GetAccID() << ")"


struct RoleValueScoreInfo
{
	AVALON_DEFINE_OBJECTPOOL(RoleValueScoreInfo, Avalon::NullMutex)

	RoleValueScoreInfo() :roleid(0), roleValueScore(0) {}

	// ��ɫid
	ObjID_t roleid;
	// ��ɫ��ֵ����
	UInt32	roleValueScore;
};

class AccountCounter;
class CLRecordCallback;
class AccountCallback;
struct AccountInfo : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(AccountInfo, Avalon::NullMutex)
	typedef HashMap<UInt32, DungeonHardInfo*> Map2DungeonHardInfo;
public:
	AccountInfo();
	~AccountInfo();

public:
	/**
	*@brief tick
	*/
	void OnTick(const Avalon::Time& now);

	/**
	*@brief �߼�����
	*/
	void OnLogicWeekChange(const Avalon::Time& now);

public:
	// Get/Set

	void SetGateConn(UInt32 nodeId);

	/**
	*@brief ��ǰ��ɫID
	*/
	ObjID_t GetRoleID() const { return m_RoleId; }
	void SetRoleID(ObjID_t id) { m_RoleId = id; }

	/**
	*@brief ��ǰ��ɫ
	*/
	WSPlayer* GetCurrentRole();

	/**
	*@brief �˺�ID
	*/
	UInt32 GetAccID() const { return m_Accid; }
	void SetAccID(UInt32 id) { m_Accid.SetDBValue(id); }

	/**
	*@brief �ㄻ
	*/
	UInt32 GetPoint() const { return m_Point; }
	void SetPoint(UInt32 num) { m_Point = num; }

	/**
	 *@brief ���õㄻ
	 */
	UInt32 GetCreditPoint() const { return m_CreditPoint; }
	void SetCreditPoint(UInt32 num) { m_CreditPoint = num; }
	void AddCreditPoint(UInt32 num);
	void RemoveCreditPoint(UInt32 num);

	/**
	*@brief �̳ǻ���
	*/
	UInt32 GetMallPoint() const { return m_MallPoint; }
	void SetMallPoint(UInt32 num) { m_MallPoint = num; }

	/**
	*@brief ð�ձ�
	*/
	UInt32 GetAdventureCoin() const { return m_AdventureCoin; }
	void SetAdventureCoin(UInt32 num) { m_AdventureCoin = num; }

	/**
	*@brief gmȨ��
	*/
	void SetGMAuthority(UInt8 auth) { m_GMAuthority = auth; }
	bool HasGmAuthority(UInt8 auth) { return m_GMAuthority.GetDBValue() >= auth; }

	/**
	*@brief ��ֵ����
	*/
	UInt32 GetChargeTimes(ChargeMallType mallType, UInt32 goodsId, ObjID_t roleId) { return m_ChargeRecord.GetChargeTimes(mallType, goodsId, roleId); }

	/**
	*@brief ��ȡ���մ���
	*/
	UInt32 GetTodayChargeTimes(ChargeMallType mallType, UInt32 goodsId, ObjID_t roleId) { return m_ChargeRecord.GetTodayChargeTimes(mallType, goodsId, roleId); }

	/**
	*@brief ���ӳ�ֵ����
	*/
	void IncChargeTimes(UInt32 accid, ObjID_t roleId, ChargeMallType mallType, UInt32 goodsId, UInt32 times) { m_ChargeRecord.IncChargeTimes(accid, roleId, mallType, goodsId, times); }

	AcccountChargeRecord& GetChargeRecord() { return m_ChargeRecord; }

	/**
	*@brief VIP�ȼ�
	*/
	UInt8 GetVipLevel() const { return m_VipLevel; }
	void SetVipLevel(UInt8 lv) { m_VipLevel = lv; }

	/**
	*@brief VIP����
	*/
	UInt32 GetVipExp() const { return m_VipExp; }
	void SetVipExp(UInt32 num) { m_VipExp = num; }

	/**
	*@brief �ܳ�ֵ���
	*/
	UInt32 GetTotalChargeNum() const { return m_TotalChargeNum; }
	void SetTotalChargeNum(UInt32 num) { m_TotalChargeNum = num; }

	/**
	*@brief �¿�����ʱ��
	*/
	UInt32 GetMonthCardExpireTime() const { return m_MonthCardExpireTime; }
	void SetMonthCardExpireTime(UInt32 time) { m_MonthCardExpireTime = time; }

	/**
	*@brief ���ü��ػص�
	*/
	void SetLoadFinishCallback(AccountCallback* cb) { m_LoadFinishCallback = cb; }
	AccountCallback* GetLoadFinishCallback() { return m_LoadFinishCallback; }

	/**
	*@brief �ܷ����¿�
	*/
	bool CanBuyMonthCard();

	/**
	*@brief �ֿ������
	*/
	void SetStorageSize(UInt32 size)	{ m_StorageSize = size; }
	UInt32 GetStoragetSize()			{ return m_StorageSize; }

	/**
	*@brief �ܷ�����ƿ�
	*/
	bool CanBuyMoneyManageCard();

	/**
	*@brief ��ƹ�����Ϣ
	*/
	inline UInt8 GetMoneyManageStatus() { return m_MoneyManageStatus; }
	void SetMoneyManageStatus(UInt8 status);

	/**
	*@brief ��ȫ������
	*/
	void SetSecurityPasswd(const std::string& passwd){ m_SecurityPasswd = passwd; }
	const std::string& GetSecurityPasswd(){ return m_SecurityPasswd; }

	/**
	*@brief ��ȫ��״̬
	*/
	void SetSecurityState(UInt32 _state){ m_SecurityState = _state; }
	UInt32 GetSecurityState(){ return m_SecurityState; }

	/**
	*@brief ��ȫ������ʱ��
	*/
	void SetSecurityFreezeTime(UInt32 _time) { m_SecurityFreezeTime = _time; }
	UInt32 GetSecurityFreezeTime() { return m_SecurityFreezeTime; }

	/**
	*@brief ��ȫ���ⶳʱ��
	*/
	void SetSecurityUnFreezeTime(UInt32 _time) { m_SecurityUnFreezeTime = _time; }
	UInt32 GetSecurityUnFreezeTime() { return m_SecurityUnFreezeTime; }

	/**
	*@brief ��ȫ������������
	*/
	void ClearSecurityPasswdErrorNum() { m_PasswdErrorNum = 0; }
	void AddSecurityPasswdErrorNum() { m_PasswdErrorNum = m_PasswdErrorNum + 1; }
	UInt32 GetSecurityPasswdErrorNum() { return m_PasswdErrorNum; }

	/**
	*@brief ��ȫ����������������ʱ��
	*/
	void SetPasswdErrorNumResetTime(UInt64 _time) { m_PasswdErrorNumResetTime = _time; }
	UInt64 GetPasswdErrorNumResetTime() { return m_PasswdErrorNumResetTime; }

public:// �˺ż���
	/**
	*@brief �����˺ż���
	*/
	void LoadAccountCounter(CLRecordCallback* callback);

	/*
	*@brief �����˺ż���
	*/
	AccountCounter* FindAccountCounter(AccountCounterType type);

	/*
	*@brief ����˺ż���
	*/
	void AddAccountCounter(AccountCounter* counter);

	/*
	*@brief ���û�ȡ�˺ż���
	*/
	void SetAccountCounterNum(WSPlayer* player, AccountCounterType type, UInt64 num);
	void SetAccountCounterNumNoSync(AccountCounterType type, UInt64 num);
	UInt64 GetAccountCounterNum(AccountCounterType type);

	/*
	*@brief �ж��ܷ��Ǯ
	*/
	bool CheckRmAccountMoney(ItemSubType moneyType, UInt32 cost);
	bool RemoveAccountMoney(WSPlayer* player, const std::string& reason, ItemSubType moneyType, UInt32 num);

	/*
	*@brief �����˺Ž�Ǯ
	*/
	bool IncAccountMoney(WSPlayer* player, const std::string& reason, ItemSubType moneyType, UInt32 incNum);

	/*
	*@brief �ж��ܷ���˺ż���
	*/
	bool CheckRmAccountCounterNum(AccountCounterType type, UInt64 cost);
	bool RemoveAccountCounterNum(WSPlayer* player, const std::string& reason, AccountCounterType type, UInt64 num);

public:// ð�ն�
	/**
	*@brief ð�նӶ���
	*/
	void SetAdventureTeamName(const std::string& name) { m_AdventureTeamName = name; }
	const std::string& GetAdventureTeamName() const { return m_AdventureTeamName; }
	bool HasAdventureTeam();

	/**
	*@brief ð�նӵȼ�
	*/
	void SetAdventureTeamLevel(UInt16 level);
	UInt16 GetAdventureTeamLevel() const { return m_AdventureTeamLevel; }

	/**
	*@brief ð�նӾ���
	*/
	void SetAdventureTeamExp(UInt64 exp) { m_AdventureTeamExp = exp; }
	UInt64 GetAdventureTeamExp() const { return m_AdventureTeamExp; }
	void TryIncAdventureTeamExp(WSPlayer* player, const std::string& reason, UInt64 incExp);

	/**
	*@brief �͸�ˮ��������ף����������
	*/
	void TryIncBlessExp(WSPlayer* player, const std::string& reason, UInt64 incAdventureTeamExp, AccountCounterType counterType);

	/**
	*@brief ð�ն�����
	*/
	void SetAdventureTeamGrade(UInt8 grade) { m_AdventureTeamGrade = grade; }
	UInt8 GetAdventureTeamGrade() const { return m_AdventureTeamGrade; }
	const std::string& GetAdventureTeamGradeStr() const;

	/**
	*@brief ��ȡԶ������z
	*/
	ExpeditionMgr& GetExpeditionMgr() { return m_ExpeditionMgr; }

	/**
	*@brief ���û�ȡ��ҽ�������ְҵ
	*/
	void SetUnlockedNewOccus(const std::map<UInt8, UInt8>& occus);
	void GetUnlockedNewOccus(std::map<UInt8, UInt8>& newOccus);

	/**
	*@brief ��ӽ�������ְҵ
	*/
	bool AddUnlockedNewOccu(UInt8 newOccu);

	/**
	*@brief �����������ְҵ
	*/
	bool RemoveUnlockedNewOccus(const std::vector<UInt8>& occus);

	/**
	*@brief ���û�ȡ��������ְҵ��ʱ��
	*/
	void SetQueryUnlockedNewOccusTime(UInt32 time) { m_QueryUnlockedNewOccusTime = time; }
	UInt32 GetQueryUnlockedNewOccusTime() { return m_QueryUnlockedNewOccusTime; }

public:// ��ɫ����
	/**
	*@brief ���û�ȡ���н�ɫ������
	*/
	void SetTotalScoreOfAllRoleValue(UInt32 score) { m_TotalScoreOfAllRoleValue = score; }
	UInt32 GetTotalScoreOfAllRoleValue() const { return m_TotalScoreOfAllRoleValue; }

	/**
	*@brief �����˺��½�ɫ����
	*/
	void LoadRoleValueScoreCallback(CLRecordCallback* callback);

	/*
	*@brief ���ҽ�ɫ����
	*/
	RoleValueScoreInfo* FindRoleValueScoreInfo(ObjID_t roleid);

	/*
	*@brief ��ӽ�ɫ����
	*/
	void AddRoleValueScoreInfo(RoleValueScoreInfo* scoreInfo);

	/**
	*@brief �����ɫ������
	*/
	UInt32 CalcTotalScoreOfAllRoleValue();

	/*
	*@brief ��ɫ�����б仯
	*/
	bool HasChangedOfRoleValueScore(ObjID_t roleid, UInt32 roleValueScore);

	/*
	*@brief ��ɫ���ֱ仯
	*/
	void OnRoleValueScoreChange(WSPlayer* player, const std::string& reason, UInt32 roleValueScore);

	/**
	*@brief ���н�ɫ�����ֱ仯
	*/
	void OnTotalScoreOfAllRoleValueChange(WSPlayer* player, const std::string& reason);

public:// ��ɫ��λ
	/**
	*@brief ����չ��ɫ��������
	*/
	void SetUnlockedExtensibleRoleField(UInt32 num) { m_UnlockedExtensibleRoleField = num; }
	UInt32 GetUnlockedExtensibleRoleField() const { return m_UnlockedExtensibleRoleField; }
	bool UnlockExtensibleRoleField(UInt32 incNum);

public:// �ؾ��̵�
	/**
	*@brief �ؾ����������
	*/
	void SetGnomeCoinNum(UInt32 num) { m_GnomeCoinNum = num; }
	UInt32 GetGnomeCoinNum();

	/**
	*@brief �ؾ������ˢ��ʱ��
	*/
	void SetGnomeCoinRefreshTime(UInt32 time) { m_GnomeCoinRefreshTime = time; }
	UInt32 GetGnomeCoinRefreshTime() const { return m_GnomeCoinRefreshTime; }

	/**
	*@brief �����豸
	*/
	void InitSecurityState();
	void ParseCommonDevMap();
	void UpdateCommonDevStr();
	void UpdateCommonDevLoginTime(const std::string& deviceID);

	bool IsCommonDev(const std::string& deviceID);
	void ClearCommonDev();
	void AddCommonDev(const std::string& deviceID);
	void DropCommonDev(const std::string& deviceID);
	void GetDeviceCode(std::string& _devCode);
	bool CheckPasswdLength(const std::string& passwd);
	bool CheckSecurityLockCrossDay();

	// ���³��Ѷ�

	void GetAllDungeonHardInfo(std::vector<SceneDungeonHardInfo>& infoes);

	/**
	*@brief ���µ��³��Ѷ���Ϣ
	*/
	void UpdateDungeonHardInfo(UInt32 dungeonId, DungeonHardType type);

	/**
	*@brief ���ص��³��Ѷ���Ϣ
	*/
	bool LoadDungeonHardInfo(DungeonHardInfo* hard);

	/**
	*@brief ͨ�����³�ID��ȡ���³��Ѷ���Ϣ
	*/
	DungeonHardInfo* FindDungeonHardInfoById(UInt32 dungeonId);

	/**
	*@brief װ�����޺��˷�
	*/
	UInt32 GetWeaponLeaseTickets() const { return m_weaponLeaseTickets; }
	void SetWeaponLeaseTickets(UInt32 num) { m_weaponLeaseTickets = num; }

	/**
	*@brief �˺�����ʱ��
	*/
	UInt32 GetOfflineTime() const { return m_OfflineTime; }
	void SetOfflineTime(UInt32 time) { m_OfflineTime = time; }

public: // �˺�����
	void LoadAccountTask(CLRecordCallback* callback);
	
	/**
	*@brief ��鱳����������
	*/
	void OnCheckPackageEnoughForTaskRewardsRes(UInt32 taskId, const ItemRewardVec& rewards);

	/**
	*@brief �ύ����
	*/
	void SubmitAccounTask(UInt32 taskId);

	/**
	*@brief ��ȡ�ܳ��������
	*/
	WeeklyTaskMgr& GetWeeklyTaskMgr() { return m_WeeklyTaskMgr; }

	/**
	*@brief ��ȡ���ύ��ð�ն�������
	*/
	UInt32 GetOverAdventureTeamTasks();

	/**
	*@brief �Ͻ�������Ʒ
	*/
	UInt32 SetTaskItem(UInt32 taskId, std::vector<WorldSetTaskItemStruct>& items);

public: // �˺ųɾ�

	/**
	*@brief ��ȡ�˺ųɾ͹�����
	*/
	AccountAchievementTaskMgr& GetAccountAchievementTaskMgr() { return m_AchievementTasks; }

public: //�˺�ͷ��
	PlayerTitleAccountMgr& GetTitleMgr() { return m_TitleMgr; }

public: //�˺ųɾ͵�
	UInt32	GetAccountAchievementScore() { return m_AccountAchievementScore; }

	void	AddAccountAchievementScore(WSPlayer* player, UInt32 score);

public:
	/**
	*@brief ����֪ͨ
	*/
	template <typename... Args>
	void SendNotify(UInt32 id, const Args & ... args)
	{
		UInt16 type = 0;
		UInt8 color = 0;
		char buffer[CL_MAX_CHAT_LEN + 1];
		memset(buffer, 0, sizeof(buffer));
		bool ret = SysNotifyMgr::Instance()->MakeNotify(id, type, color, buffer, CL_MAX_CHAT_LEN, args...);

		if (!ret) return;

		Notify(type, color, buffer);
	}

	template <typename... Args>
	void SendNotify(const char* ctx, const Args & ... args)
	{
		char buffer[CL_MAX_CHAT_LEN + 1];
		memset(buffer, 0, sizeof(buffer));
		bool ret = SysNotifyMgr::Instance()->MakeNotify(ctx, buffer, CL_MAX_CHAT_LEN, args...);
		if (!ret) return;
		Notify(20, 0, buffer);
	}

	void Notify(UInt16 type, UInt8 color, const char* word);

public: //�¼�
	void OnPlayerOnline(WSPlayer* player);

	void OnPlayerOffline(WSPlayer* player);

	void OnOwnedNpcDied(UInt32 npcid, UInt8 type, UInt32 dungeon);

	/**
	 *@brief �ȼ��ı�
	 */
	void OnPlayerLevelChanged(WSPlayer* player, UInt32 oldLvl);

	/**
	*@brief ְҵ�ı�
	*/
	void OnPlayerOccuChanged(WSPlayer* player);

	/**
	*@brief ð�ն�(Ӷ����)�ȼ��ı�
	*/
	void OnAdventureTeamLevelChanged();

	/**
	*@brief ð�ն�(Ӷ����)�����ı�
	*/
	void OnAdventureTeamGradeChanged();
	/**
	*@brief ���ͨ��
	*/
	void OnPlayerClearDungeon(const DungeonDataEntry* dungeonData, LevelScore score, UInt32 usedTime);

	/**
	*@brief ƣ������
	*/
	void OnRemoveFatigue(UInt32 removeFatigue);

	/**
	*@brief ����
	*/
	MonopolyInfo& GetMonopolyInfo() { return m_Monopoly; }

public:
	// �����������

	/**
	*@brief ���Ӽ�������������ر��μ������
	*/
	UInt8 IncLoadDataCount();

	/**
	*@brief ��������
	*/
	bool CheckLoadDataSerial(UInt8 serial);
	void SetLoadDataFinished(UInt8 serial);
	bool IsLoadFinished() const { return m_LoadDataSerial == 0; }

	/**
	*@brief ���سɹ�
	*/
	void OnLoadFinished();

	/**
	*@brief ���سɹ�
	*/
	void OnLoadFailure();

public:	// ����������־���
	/**
	*@brief ����udplog
	*/
	template <typename... Args>
	void SendUdpLog(const char* typeInfo, const char* format, const Args & ... args)
	{
		//WSPlayer* player = GetCurrentRole();
		//if (player)
		//{
		//	player->SendUdpLog(typeInfo, format, args...);
		//}
		//else
		{
			if (typeInfo == NULL || format == NULL) return;
			CUserLogger	*pLogger = _analyzer.GetInstance(UDPLOG_SCENE_UID);
			if (NULL == pLogger) return;
			char userLogBuffer[MAX_USERMSG_LENGTH] = {0};
			//memset(userLogBuffer, 0, MAX_USERMSG_LENGTH);
			UInt32 nTime = (UInt32)Avalon::Time::CurrentTime().Sec();
			UInt16 zoneId = 0;
			UInt32 monthCardExpireTime = 0;
			UInt32 bronZoneId = 0;
			UInt32 isReturn = 0;

#ifndef AVALON_OS_WINDOWS
			snprintf(userLogBuffer, sizeof(userLogBuffer), LOG_USERMSG_FORMAT, GAME_VERSION.c_str(), "", zoneId, "", "", "", "", "", (UInt64)0, GetAccID(), 0, (UInt32)GetVipLevel(), 0, monthCardExpireTime, 0, nTime, bronZoneId, isReturn, 0, 0, 0);
#else
			_snprintf_s(userLogBuffer, sizeof(userLogBuffer), LOG_USERMSG_FORMAT, GAME_VERSION.c_str(), "", zoneId, "", "", "", "", "", (UInt64)0, GetAccID(), 0, (UInt32)GetVipLevel(), 0, monthCardExpireTime, 0, nTime, bronZoneId, isReturn, 0, 0, 0);
#endif

			pLogger->SetUserIP("");
			pLogger->SetUserMsg(userLogBuffer);
			pLogger->LogMsg(typeInfo, format, args...);

		}
	}

public:

	/**
	*@brief ���͵��ͻ���
	*/
	void SendProtocol(Avalon::Protocol& protocol);

	/**
	*@brief ������Ϣ�����ͻ���
	*/
	void SendPacket(Avalon::Packet* packet);

public:
	/* ���ݿ���� */
	void SaveToDB();

	CL_DBPROPERTY_BEGIN(CLSyncObject)
		CL_DBPROPERTY("accid", m_Accid)
		CL_DBPROPERTY("point", m_Point)
		CL_DBPROPERTY("credit_point", m_CreditPoint)
		CL_DBPROPERTY("gmauthority", m_GMAuthority);
		CL_DBPROPERTY("viplvl", m_VipLevel)
		CL_DBPROPERTY("vipexp", m_VipExp)
		CL_DBPROPERTY("total_charge_num", m_TotalChargeNum)
		CL_DBPROPERTY("month_card_expire_time", m_MonthCardExpireTime)
		CL_DBPROPERTY("storage_size", m_StorageSize)
		CL_DBPROPERTY("money_manage_status", m_MoneyManageStatus)
		CL_DBPROPERTY("security_passwd", m_SecurityPasswd)
		CL_DBPROPERTY("security_freeze_time", m_SecurityFreezeTime)
		CL_DBPROPERTY("security_unfreeze_time", m_SecurityUnFreezeTime)
		CL_DBPROPERTY("common_device", m_CommonDevice)
		CL_DBPROPERTY("security_passwd_error_num", m_PasswdErrorNum)
		CL_DBPROPERTY("acc_offline_time", m_PasswdErrorNumResetTime)
		CL_DBPROPERTY("weapon_lease_tickets", m_weaponLeaseTickets)
		CL_DBPROPERTY("unlock_extensible_role_num", m_UnlockedExtensibleRoleField)
		CL_DBPROPERTY("gnome_coin_num", m_GnomeCoinNum)
		CL_DBPROPERTY("gnome_coin_refresh_time", m_GnomeCoinRefreshTime)
		CL_DBPROPERTY("adventure_team_name", m_AdventureTeamName)
		CL_DBPROPERTY("adventure_team_level", m_AdventureTeamLevel)
		CL_DBPROPERTY("adventure_team_exp", m_AdventureTeamExp)
		CL_DBPROPERTY("adventure_team_grade_id", m_AdventureTeamGrade)
		CL_DBPROPERTY("all_role_value_score", m_TotalScoreOfAllRoleValue)
		CL_DBPROPERTY("unlocked_new_occus", m_UnlockedNewOccus);
		CL_DBPROPERTY("query_new_occus_time", m_QueryUnlockedNewOccusTime);
		CL_DBPROPERTY("offline_time", m_OfflineTime);
		CL_DBPROPERTY("account_achievement_score", m_AccountAchievementScore);
		CL_DBPROPERTY("mall_point", m_MallPoint);
		CL_DBPROPERTY("adventure_coin", m_AdventureCoin)
	CL_DBPROPERTY_END()


private:

	Avalon::NetConnectionPtr m_Conn;

	// ��ɫID����
	std::set<UInt64>		m_RoleIdSet;

	//��ǰ��ɫID
	ObjID_t					m_RoleId;

	// �˺�ID
	ObjUInt32				m_Accid;
	// �ㄻ
	ObjUInt32				m_Point;
	// ���õ�ȯ
	ObjUInt32				m_CreditPoint;
	// �̳ǻ���
	ObjUInt32				m_MallPoint;
	// gmȨ��
	ObjUInt8				m_GMAuthority;
	// ��ֵ��¼
	AcccountChargeRecord	m_ChargeRecord;
	// vip�ȼ�
	ObjUInt8				m_VipLevel;
	// vip����
	ObjUInt32				m_VipExp;
	// �ܳ�ֵ��
	ObjUInt32				m_TotalChargeNum;
	// �¿�����ʱ��
	ObjUInt32				m_MonthCardExpireTime;
	// ���³��Ѷ���Ϣ
	Map2DungeonHardInfo     m_DungeonHardInfoes;
	// �������ݿⶨʱ��
	Avalon::SimpleTimer		m_SaveTimer;

	//��������
	UInt8					m_LoadDataCount;
	//�������
	UInt32					m_LoadDataSerial;
	AccountCallback*		m_LoadFinishCallback;
	//�ֿ������
	ObjUInt32				m_StorageSize;
	//��ƽ�ɫ״̬
	ObjUInt8				m_MoneyManageStatus;

	//��ȫ������
	ObjString				m_SecurityPasswd;

	//��ȫ��״̬
	UInt32					m_SecurityState;

	//��ȫ������ʱ��
	ObjUInt32				m_SecurityFreezeTime;

	//��ȫ���ⶳʱ��
	ObjUInt32				m_SecurityUnFreezeTime;

	//�����豸
	ObjString				m_CommonDevice;

	//�����豸�б�
	std::map<std::string, std::string> m_CommonDevMap;

	//������������������
	ObjUInt32				m_PasswdErrorNum;
	
	//��ȫ����������������ʱ��
	ObjUInt64				m_PasswdErrorNumResetTime;

	// �������޺��˷�
	ObjUInt32				m_weaponLeaseTickets;

	// �����Ŀ���չ��ɫ��λ
	ObjUInt32				m_UnlockedExtensibleRoleField;
	// �ؾ����������
	ObjUInt32				m_GnomeCoinNum;
	// �ؾ������ˢ��ʱ��
	ObjUInt32				m_GnomeCoinRefreshTime;

	// ð�նӶ���
	ObjString				m_AdventureTeamName;
	// ð�նӵȼ�
	ObjUInt16				m_AdventureTeamLevel;
	// ð�նӾ���
	ObjUInt64				m_AdventureTeamExp;
	// ð�ն�����
	ObjUInt8				m_AdventureTeamGrade;

	// ���н�ɫ��ֵ���ּ�
	std::vector<RoleValueScoreInfo*> m_AllRoleValueScores;
	// ���н�ɫ��ֵ������
	ObjUInt32				m_TotalScoreOfAllRoleValue;

	// �˺ż���
	std::vector<AccountCounter*> m_AccountCounter;

	// Զ������
	ExpeditionMgr			m_ExpeditionMgr;
	//�˺ųɾ�
	AccountAchievementTaskMgr	m_AchievementTasks;

	// ��ҽ�������ְҵ
	ObjString				m_UnlockedNewOccus;
	// ��ѯ��������ְҵ��ʱ��
	ObjUInt32				m_QueryUnlockedNewOccusTime;

	// �˺�����ʱ��
	ObjUInt32				m_OfflineTime;

	// �߼���ˢ��ʱ��
	UInt64					m_LogicWeekChangeTime;

	// �˺��ܳ�����
	WeeklyTaskMgr			m_WeeklyTaskMgr;

	//�˺�ͷ��
	PlayerTitleAccountMgr	m_TitleMgr;

	//�˺ųɾ͵�
	ObjUInt32				m_AccountAchievementScore;

	//ð�ձ�
	ObjUInt32				m_AdventureCoin;

	//����
	MonopolyInfo			m_Monopoly;
};

#endif