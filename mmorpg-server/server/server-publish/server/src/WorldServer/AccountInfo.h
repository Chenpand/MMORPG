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

	// 角色id
	ObjID_t roleid;
	// 角色价值评分
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
	*@brief 逻辑跨周
	*/
	void OnLogicWeekChange(const Avalon::Time& now);

public:
	// Get/Set

	void SetGateConn(UInt32 nodeId);

	/**
	*@brief 当前角色ID
	*/
	ObjID_t GetRoleID() const { return m_RoleId; }
	void SetRoleID(ObjID_t id) { m_RoleId = id; }

	/**
	*@brief 当前角色
	*/
	WSPlayer* GetCurrentRole();

	/**
	*@brief 账号ID
	*/
	UInt32 GetAccID() const { return m_Accid; }
	void SetAccID(UInt32 id) { m_Accid.SetDBValue(id); }

	/**
	*@brief 点劵
	*/
	UInt32 GetPoint() const { return m_Point; }
	void SetPoint(UInt32 num) { m_Point = num; }

	/**
	 *@brief 信用点劵
	 */
	UInt32 GetCreditPoint() const { return m_CreditPoint; }
	void SetCreditPoint(UInt32 num) { m_CreditPoint = num; }
	void AddCreditPoint(UInt32 num);
	void RemoveCreditPoint(UInt32 num);

	/**
	*@brief 商城积分
	*/
	UInt32 GetMallPoint() const { return m_MallPoint; }
	void SetMallPoint(UInt32 num) { m_MallPoint = num; }

	/**
	*@brief 冒险币
	*/
	UInt32 GetAdventureCoin() const { return m_AdventureCoin; }
	void SetAdventureCoin(UInt32 num) { m_AdventureCoin = num; }

	/**
	*@brief gm权限
	*/
	void SetGMAuthority(UInt8 auth) { m_GMAuthority = auth; }
	bool HasGmAuthority(UInt8 auth) { return m_GMAuthority.GetDBValue() >= auth; }

	/**
	*@brief 充值次数
	*/
	UInt32 GetChargeTimes(ChargeMallType mallType, UInt32 goodsId, ObjID_t roleId) { return m_ChargeRecord.GetChargeTimes(mallType, goodsId, roleId); }

	/**
	*@brief 获取今日次数
	*/
	UInt32 GetTodayChargeTimes(ChargeMallType mallType, UInt32 goodsId, ObjID_t roleId) { return m_ChargeRecord.GetTodayChargeTimes(mallType, goodsId, roleId); }

	/**
	*@brief 增加充值次数
	*/
	void IncChargeTimes(UInt32 accid, ObjID_t roleId, ChargeMallType mallType, UInt32 goodsId, UInt32 times) { m_ChargeRecord.IncChargeTimes(accid, roleId, mallType, goodsId, times); }

	AcccountChargeRecord& GetChargeRecord() { return m_ChargeRecord; }

	/**
	*@brief VIP等级
	*/
	UInt8 GetVipLevel() const { return m_VipLevel; }
	void SetVipLevel(UInt8 lv) { m_VipLevel = lv; }

	/**
	*@brief VIP经验
	*/
	UInt32 GetVipExp() const { return m_VipExp; }
	void SetVipExp(UInt32 num) { m_VipExp = num; }

	/**
	*@brief 总充值金额
	*/
	UInt32 GetTotalChargeNum() const { return m_TotalChargeNum; }
	void SetTotalChargeNum(UInt32 num) { m_TotalChargeNum = num; }

	/**
	*@brief 月卡到期时间
	*/
	UInt32 GetMonthCardExpireTime() const { return m_MonthCardExpireTime; }
	void SetMonthCardExpireTime(UInt32 time) { m_MonthCardExpireTime = time; }

	/**
	*@brief 设置加载回调
	*/
	void SetLoadFinishCallback(AccountCallback* cb) { m_LoadFinishCallback = cb; }
	AccountCallback* GetLoadFinishCallback() { return m_LoadFinishCallback; }

	/**
	*@brief 能否购买月卡
	*/
	bool CanBuyMonthCard();

	/**
	*@brief 仓库格子数
	*/
	void SetStorageSize(UInt32 size)	{ m_StorageSize = size; }
	UInt32 GetStoragetSize()			{ return m_StorageSize; }

	/**
	*@brief 能否购买理财卡
	*/
	bool CanBuyMoneyManageCard();

	/**
	*@brief 理财管理信息
	*/
	inline UInt8 GetMoneyManageStatus() { return m_MoneyManageStatus; }
	void SetMoneyManageStatus(UInt8 status);

	/**
	*@brief 安全锁密码
	*/
	void SetSecurityPasswd(const std::string& passwd){ m_SecurityPasswd = passwd; }
	const std::string& GetSecurityPasswd(){ return m_SecurityPasswd; }

	/**
	*@brief 安全锁状态
	*/
	void SetSecurityState(UInt32 _state){ m_SecurityState = _state; }
	UInt32 GetSecurityState(){ return m_SecurityState; }

	/**
	*@brief 安全锁冻结时间
	*/
	void SetSecurityFreezeTime(UInt32 _time) { m_SecurityFreezeTime = _time; }
	UInt32 GetSecurityFreezeTime() { return m_SecurityFreezeTime; }

	/**
	*@brief 安全锁解冻时间
	*/
	void SetSecurityUnFreezeTime(UInt32 _time) { m_SecurityUnFreezeTime = _time; }
	UInt32 GetSecurityUnFreezeTime() { return m_SecurityUnFreezeTime; }

	/**
	*@brief 安全锁密码错误次数
	*/
	void ClearSecurityPasswdErrorNum() { m_PasswdErrorNum = 0; }
	void AddSecurityPasswdErrorNum() { m_PasswdErrorNum = m_PasswdErrorNum + 1; }
	UInt32 GetSecurityPasswdErrorNum() { return m_PasswdErrorNum; }

	/**
	*@brief 安全锁密码错误次数重置时间
	*/
	void SetPasswdErrorNumResetTime(UInt64 _time) { m_PasswdErrorNumResetTime = _time; }
	UInt64 GetPasswdErrorNumResetTime() { return m_PasswdErrorNumResetTime; }

public:// 账号计数
	/**
	*@brief 加载账号计数
	*/
	void LoadAccountCounter(CLRecordCallback* callback);

	/*
	*@brief 查找账号计数
	*/
	AccountCounter* FindAccountCounter(AccountCounterType type);

	/*
	*@brief 添加账号计数
	*/
	void AddAccountCounter(AccountCounter* counter);

	/*
	*@brief 设置获取账号计数
	*/
	void SetAccountCounterNum(WSPlayer* player, AccountCounterType type, UInt64 num);
	void SetAccountCounterNumNoSync(AccountCounterType type, UInt64 num);
	UInt64 GetAccountCounterNum(AccountCounterType type);

	/*
	*@brief 判断能否扣钱
	*/
	bool CheckRmAccountMoney(ItemSubType moneyType, UInt32 cost);
	bool RemoveAccountMoney(WSPlayer* player, const std::string& reason, ItemSubType moneyType, UInt32 num);

	/*
	*@brief 增加账号金钱
	*/
	bool IncAccountMoney(WSPlayer* player, const std::string& reason, ItemSubType moneyType, UInt32 incNum);

	/*
	*@brief 判断能否扣账号计数
	*/
	bool CheckRmAccountCounterNum(AccountCounterType type, UInt64 cost);
	bool RemoveAccountCounterNum(WSPlayer* player, const std::string& reason, AccountCounterType type, UInt64 num);

public:// 冒险队
	/**
	*@brief 冒险队队名
	*/
	void SetAdventureTeamName(const std::string& name) { m_AdventureTeamName = name; }
	const std::string& GetAdventureTeamName() const { return m_AdventureTeamName; }
	bool HasAdventureTeam();

	/**
	*@brief 冒险队等级
	*/
	void SetAdventureTeamLevel(UInt16 level);
	UInt16 GetAdventureTeamLevel() const { return m_AdventureTeamLevel; }

	/**
	*@brief 冒险队经验
	*/
	void SetAdventureTeamExp(UInt64 exp) { m_AdventureTeamExp = exp; }
	UInt64 GetAdventureTeamExp() const { return m_AdventureTeamExp; }
	void TryIncAdventureTeamExp(WSPlayer* player, const std::string& reason, UInt64 incExp);

	/**
	*@brief 赐福水晶、传承祝福经验增加
	*/
	void TryIncBlessExp(WSPlayer* player, const std::string& reason, UInt64 incAdventureTeamExp, AccountCounterType counterType);

	/**
	*@brief 冒险队评级
	*/
	void SetAdventureTeamGrade(UInt8 grade) { m_AdventureTeamGrade = grade; }
	UInt8 GetAdventureTeamGrade() const { return m_AdventureTeamGrade; }
	const std::string& GetAdventureTeamGradeStr() const;

	/**
	*@brief 获取远征管理z
	*/
	ExpeditionMgr& GetExpeditionMgr() { return m_ExpeditionMgr; }

	/**
	*@brief 设置获取玩家解锁的新职业
	*/
	void SetUnlockedNewOccus(const std::map<UInt8, UInt8>& occus);
	void GetUnlockedNewOccus(std::map<UInt8, UInt8>& newOccus);

	/**
	*@brief 添加解锁的新职业
	*/
	bool AddUnlockedNewOccu(UInt8 newOccu);

	/**
	*@brief 清除解锁的新职业
	*/
	bool RemoveUnlockedNewOccus(const std::vector<UInt8>& occus);

	/**
	*@brief 设置获取解锁的新职业的时间
	*/
	void SetQueryUnlockedNewOccusTime(UInt32 time) { m_QueryUnlockedNewOccusTime = time; }
	UInt32 GetQueryUnlockedNewOccusTime() { return m_QueryUnlockedNewOccusTime; }

public:// 角色评分
	/**
	*@brief 设置获取所有角色总评分
	*/
	void SetTotalScoreOfAllRoleValue(UInt32 score) { m_TotalScoreOfAllRoleValue = score; }
	UInt32 GetTotalScoreOfAllRoleValue() const { return m_TotalScoreOfAllRoleValue; }

	/**
	*@brief 加载账号下角色评分
	*/
	void LoadRoleValueScoreCallback(CLRecordCallback* callback);

	/*
	*@brief 查找角色评分
	*/
	RoleValueScoreInfo* FindRoleValueScoreInfo(ObjID_t roleid);

	/*
	*@brief 添加角色评分
	*/
	void AddRoleValueScoreInfo(RoleValueScoreInfo* scoreInfo);

	/**
	*@brief 计算角色总评分
	*/
	UInt32 CalcTotalScoreOfAllRoleValue();

	/*
	*@brief 角色评分有变化
	*/
	bool HasChangedOfRoleValueScore(ObjID_t roleid, UInt32 roleValueScore);

	/*
	*@brief 角色评分变化
	*/
	void OnRoleValueScoreChange(WSPlayer* player, const std::string& reason, UInt32 roleValueScore);

	/**
	*@brief 所有角色总评分变化
	*/
	void OnTotalScoreOfAllRoleValueChange(WSPlayer* player, const std::string& reason);

public:// 角色栏位
	/**
	*@brief 可扩展角色解锁数量
	*/
	void SetUnlockedExtensibleRoleField(UInt32 num) { m_UnlockedExtensibleRoleField = num; }
	UInt32 GetUnlockedExtensibleRoleField() const { return m_UnlockedExtensibleRoleField; }
	bool UnlockExtensibleRoleField(UInt32 incNum);

public:// 地精商店
	/**
	*@brief 地精纪念币数量
	*/
	void SetGnomeCoinNum(UInt32 num) { m_GnomeCoinNum = num; }
	UInt32 GetGnomeCoinNum();

	/**
	*@brief 地精纪念币刷新时间
	*/
	void SetGnomeCoinRefreshTime(UInt32 time) { m_GnomeCoinRefreshTime = time; }
	UInt32 GetGnomeCoinRefreshTime() const { return m_GnomeCoinRefreshTime; }

	/**
	*@brief 常用设备
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

	// 地下城难度

	void GetAllDungeonHardInfo(std::vector<SceneDungeonHardInfo>& infoes);

	/**
	*@brief 更新地下城难度信息
	*/
	void UpdateDungeonHardInfo(UInt32 dungeonId, DungeonHardType type);

	/**
	*@brief 加载地下城难度信息
	*/
	bool LoadDungeonHardInfo(DungeonHardInfo* hard);

	/**
	*@brief 通过地下城ID获取地下城难度信息
	*/
	DungeonHardInfo* FindDungeonHardInfoById(UInt32 dungeonId);

	/**
	*@brief 装备租赁好运符
	*/
	UInt32 GetWeaponLeaseTickets() const { return m_weaponLeaseTickets; }
	void SetWeaponLeaseTickets(UInt32 num) { m_weaponLeaseTickets = num; }

	/**
	*@brief 账号离线时间
	*/
	UInt32 GetOfflineTime() const { return m_OfflineTime; }
	void SetOfflineTime(UInt32 time) { m_OfflineTime = time; }

public: // 账号任务
	void LoadAccountTask(CLRecordCallback* callback);
	
	/**
	*@brief 检查背包容量返回
	*/
	void OnCheckPackageEnoughForTaskRewardsRes(UInt32 taskId, const ItemRewardVec& rewards);

	/**
	*@brief 提交任务
	*/
	void SubmitAccounTask(UInt32 taskId);

	/**
	*@brief 获取周常任务管理
	*/
	WeeklyTaskMgr& GetWeeklyTaskMgr() { return m_WeeklyTaskMgr; }

	/**
	*@brief 获取已提交的冒险队任务数
	*/
	UInt32 GetOverAdventureTeamTasks();

	/**
	*@brief 上交任务物品
	*/
	UInt32 SetTaskItem(UInt32 taskId, std::vector<WorldSetTaskItemStruct>& items);

public: // 账号成就

	/**
	*@brief 获取账号成就管理器
	*/
	AccountAchievementTaskMgr& GetAccountAchievementTaskMgr() { return m_AchievementTasks; }

public: //账号头衔
	PlayerTitleAccountMgr& GetTitleMgr() { return m_TitleMgr; }

public: //账号成就点
	UInt32	GetAccountAchievementScore() { return m_AccountAchievementScore; }

	void	AddAccountAchievementScore(WSPlayer* player, UInt32 score);

public:
	/**
	*@brief 发送通知
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

public: //事件
	void OnPlayerOnline(WSPlayer* player);

	void OnPlayerOffline(WSPlayer* player);

	void OnOwnedNpcDied(UInt32 npcid, UInt8 type, UInt32 dungeon);

	/**
	 *@brief 等级改变
	 */
	void OnPlayerLevelChanged(WSPlayer* player, UInt32 oldLvl);

	/**
	*@brief 职业改变
	*/
	void OnPlayerOccuChanged(WSPlayer* player);

	/**
	*@brief 冒险队(佣兵团)等级改变
	*/
	void OnAdventureTeamLevelChanged();

	/**
	*@brief 冒险队(佣兵团)评级改变
	*/
	void OnAdventureTeamGradeChanged();
	/**
	*@brief 玩家通关
	*/
	void OnPlayerClearDungeon(const DungeonDataEntry* dungeonData, LevelScore score, UInt32 usedTime);

	/**
	*@brief 疲劳消耗
	*/
	void OnRemoveFatigue(UInt32 removeFatigue);

	/**
	*@brief 大富翁
	*/
	MonopolyInfo& GetMonopolyInfo() { return m_Monopoly; }

public:
	// 加载数据相关

	/**
	*@brief 增加加载项个数，返回本次加载序号
	*/
	UInt8 IncLoadDataCount();

	/**
	*@brief 检查加载项
	*/
	bool CheckLoadDataSerial(UInt8 serial);
	void SetLoadDataFinished(UInt8 serial);
	bool IsLoadFinished() const { return m_LoadDataSerial == 0; }

	/**
	*@brief 加载成功
	*/
	void OnLoadFinished();

	/**
	*@brief 加载成功
	*/
	void OnLoadFailure();

public:	// 数据中心日志相关
	/**
	*@brief 发送udplog
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
	*@brief 发送到客户端
	*/
	void SendProtocol(Avalon::Protocol& protocol);

	/**
	*@brief 发送消息包到客户端
	*/
	void SendPacket(Avalon::Packet* packet);

public:
	/* 数据库相关 */
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

	// 角色ID队列
	std::set<UInt64>		m_RoleIdSet;

	//当前角色ID
	ObjID_t					m_RoleId;

	// 账号ID
	ObjUInt32				m_Accid;
	// 点劵
	ObjUInt32				m_Point;
	// 信用点券
	ObjUInt32				m_CreditPoint;
	// 商城积分
	ObjUInt32				m_MallPoint;
	// gm权限
	ObjUInt8				m_GMAuthority;
	// 充值记录
	AcccountChargeRecord	m_ChargeRecord;
	// vip等级
	ObjUInt8				m_VipLevel;
	// vip经验
	ObjUInt32				m_VipExp;
	// 总充值数
	ObjUInt32				m_TotalChargeNum;
	// 月卡到期时间
	ObjUInt32				m_MonthCardExpireTime;
	// 地下城难度信息
	Map2DungeonHardInfo     m_DungeonHardInfoes;
	// 保存数据库定时器
	Avalon::SimpleTimer		m_SaveTimer;

	//加载项数
	UInt8					m_LoadDataCount;
	//加载序号
	UInt32					m_LoadDataSerial;
	AccountCallback*		m_LoadFinishCallback;
	//仓库格子数
	ObjUInt32				m_StorageSize;
	//理财角色状态
	ObjUInt8				m_MoneyManageStatus;

	//安全锁密码
	ObjString				m_SecurityPasswd;

	//安全锁状态
	UInt32					m_SecurityState;

	//安全锁冻结时间
	ObjUInt32				m_SecurityFreezeTime;

	//安全锁解冻时间
	ObjUInt32				m_SecurityUnFreezeTime;

	//常用设备
	ObjString				m_CommonDevice;

	//常用设备列表
	std::map<std::string, std::string> m_CommonDevMap;

	//当天输入密码错误次数
	ObjUInt32				m_PasswdErrorNum;
	
	//安全锁密码错误次数重置时间
	ObjUInt64				m_PasswdErrorNumResetTime;

	// 武器租赁好运符
	ObjUInt32				m_weaponLeaseTickets;

	// 解锁的可扩展角色栏位
	ObjUInt32				m_UnlockedExtensibleRoleField;
	// 地精纪念币数量
	ObjUInt32				m_GnomeCoinNum;
	// 地精纪念币刷新时间
	ObjUInt32				m_GnomeCoinRefreshTime;

	// 冒险队队名
	ObjString				m_AdventureTeamName;
	// 冒险队等级
	ObjUInt16				m_AdventureTeamLevel;
	// 冒险队经验
	ObjUInt64				m_AdventureTeamExp;
	// 冒险队评级
	ObjUInt8				m_AdventureTeamGrade;

	// 所有角色价值评分集
	std::vector<RoleValueScoreInfo*> m_AllRoleValueScores;
	// 所有角色价值总评分
	ObjUInt32				m_TotalScoreOfAllRoleValue;

	// 账号计数
	std::vector<AccountCounter*> m_AccountCounter;

	// 远征管理
	ExpeditionMgr			m_ExpeditionMgr;
	//账号成就
	AccountAchievementTaskMgr	m_AchievementTasks;

	// 玩家解锁的新职业
	ObjString				m_UnlockedNewOccus;
	// 查询解锁的新职业的时间
	ObjUInt32				m_QueryUnlockedNewOccusTime;

	// 账号离线时间
	ObjUInt32				m_OfflineTime;

	// 逻辑周刷新时间
	UInt64					m_LogicWeekChangeTime;

	// 账号周常任务
	WeeklyTaskMgr			m_WeeklyTaskMgr;

	//账号头衔
	PlayerTitleAccountMgr	m_TitleMgr;

	//账号成就点
	ObjUInt32				m_AccountAchievementScore;

	//冒险币
	ObjUInt32				m_AdventureCoin;

	//大富翁
	MonopolyInfo			m_Monopoly;
};

#endif