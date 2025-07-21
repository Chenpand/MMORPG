#ifndef _ACTIVE_TASK_MGR_H_
#define _ACTIVE_TASK_MGR_H_

#include <map>
#include <set>
#include <functional>

#include <CLObjectProperty.h>
#include <CLTaskDefine.h>
#include <CLDungeonDefine.h>
#include <CLMatchDefine.h>
#include <CLEventMgr.h>
#include <CLActivityDefine.h>
#include <CLGameSessionDefine.h>
#include <CLRelationDefine.h>
#include "ActiveRecordMgr.h"

class ActiveTaskMgr;

#define SIGH_IN_DAYNUM	30
#define MAKEUP_USELESSID 100000000

enum GiftPackType
{
	GPT_ONLINE_TIME = 1,	//累计在线时间
	GPT_REACH_LEVEL,		//达到等级

	GPT_MAX,
};

struct FatigueTaskValue
{
	FatigueTaskValue()
	{
		lastFatigue = 0;
		maxFatigue = 0;
		lowExp = 0;
		lowNeedMoneyID = 0;
		hiExp = 0;
		hiNeedMoneyID = 0;
		vipMaxs.clear();
	}

	UInt32 lastFatigue;
	UInt32 maxFatigue;
	UInt64 lowExp;
	UInt32 lowNeedMoneyID;
	UInt64 hiExp;
	UInt32 hiNeedMoneyID;
	std::vector<UInt32> vipMaxs;

};

struct DungenTaskValue
{
	DungenTaskValue()
	{
		id = 0;
		normalCost = 0;
		perfectCost = 0;
		count = 0;
		max = 0;
	}

	UInt32 id;
	UInt32 normalCost;
	UInt32 perfectCost;
	std::vector<UInt32> normalItems;
	std::vector<UInt64> itemNormalNum;
	std::vector<UInt32> perfectItems;
	std::vector<UInt64> itemPerfectNum;
	UInt32 count;
	UInt32 max;
};

//已读通知属性
class ReadNotifyProperty : public CLComplexProperty
{
public:
	ReadNotifyProperty();
	~ReadNotifyProperty();

public:

	void AddProperty(UInt32 type, UInt32 param);
	bool IsReaded(UInt32 type, UInt32 param);

public://编解码相关

	void DecodeString(std::istringstream& is);
	void EncodeString(std::ostringstream& os);

private:
	//属性列表
	std::vector<NotifyInfo>	m_Properties;
};

struct OpActTask;
struct OpActTaskData;
struct WeekSignData;

class ActiveTaskDataEntry;
class ActiveTask;
class CLRecordCallback;
class DungeonDataEntry;
class CLSaveCommand;
class Player;
class GiftPhaseData;

/**
*@brief 活动任务管理器
*/
class ActiveTaskMgr : public CLComplexProperty
{
	typedef std::map<UInt32, ActiveTask*> ActivityMap;

public:
	ActiveTaskMgr();
	~ActiveTaskMgr();

public:
	void OnLoadTask(UInt64 guid, UInt32 dataId, TaskStatus status, std::string parameter, UInt32 acceptTime);

	void SetOwner(Player* owner){ m_pOwner = owner; }
	Player* GetOwner() const { return m_pOwner; }

	//接受任务,活动开始接受任务
	bool AcceptTask(UInt32 activeId, bool online = false);

	//清除任务,活动结束清除任务
	bool ClearTask(UInt32 activeId);

	//提交任务
	bool SubmitTask(UInt32 taskId, bool bRp = false);

	//提交补偿任务
	bool SubmitMakeUpTask(UInt32 taskId, UInt32 param1);

	//补签任务
	bool SubmitTaskRp(std::vector<UInt32>& taskId);

	bool TakePhaseGift(UInt32 id);

	ActiveTask* FindTask(UInt32 id) const;

	// 每周更新活动任务
	void OnLogicWeekChangeForActivities();

	//同步任务
	void SyncAllTask();

	//更新活动任务
	void UpdateActiveTasks(UInt32 offlineDay);

	//签到相关
	void SighInOnline();												//签到上线更新
	void SignInTotalOnline();											//累计签到上线更新
	void CreateSignInTask(ActiveTaskDataEntry* data, bool pre, UInt32 loopNum, UInt32 day);			//生成签到任务
	void UpdateSignInTask(ActiveTask* task, bool pre, UInt32 loopNum, UInt32 day);					//更新签到任务
	bool SubmitSignInTask(ActiveTask* task, bool bRp = false);

	//月卡活动
	void OnMonthCardWelfare(UInt32 remainDays, bool isRenew, bool bDayChange, bool isOnline);
	void CreateMonthCardTask(UInt32 remainDays, ActiveTaskDataEntry* data);
	void UpdateMonthCardTask(UInt32 remainDays, ActiveTask* task, bool isRenew, bool bDayChange, bool isOnline);

	//判断是否在活动中
	bool IsInActive(UInt32 activeId, ActiveTask* task);

	//判断这个时间是否有对应类型的活动
	bool IsTimeInAcitveType(UInt32 time, UInt32	tmpType);

	//获取剩余时间
	void GetRestTime(UInt32& time1, UInt32& time2);

	//获取领取的在线时间礼包对应的时间
	inline ObjUInt32& GetOnlineTimeBonus()	{ return m_onlineTimeBonusId; }
	//获取领取的等级礼包对应等级
	inline ObjUInt32& GetReachLvBonus()	{ return m_reachLvBonusId; }

	//道具兑换
	void AcceptItemChangeTask(const std::vector<ActiveTaskDataEntry*>& taskDatas);
	//创建道具兑换任务
	void CreateItemChangeTask(ActiveTaskDataEntry* data);
	//更新道具兑换任务
	void UpdateItemChangeTask(ActiveTask* itemChangeTask);
	//提交道具兑换任务
	bool SubmitItemChangeTask(ActiveTask* itemChangeTask);
	//周刷新
	void OnLogicWeekChangeForItemChangeTask();

	void DayChargeOnline(UInt32 offlineDay);

	/* 每日充值(新)*/

	// 上线
	void DayChargeNewOnline(bool bDayChanged);
	// 自然日跨天
	void DayChargeNewOnDayChange();

	//能否买礼遇特权卡
	bool CanBuyGiftRightCard();

	//能否买深渊特权卡
	bool CanBuyDungeonRightCard();

	// 有没有深渊特权卡
	bool HaveDungeonRightCard();

	// 设置深渊特权卡
	void SetDungeonRightCard();

public://事件

	//tick事件
	void OnHeartbeat(const Avalon::Time& now, int tickType);

	//玩家上线
	void OnOnline(bool bDayChanged);
	void OnOffline();

	//玩家出生
	void OnBirth();

	//玩家升级 
	void OnLevelup();
	
	//一日更新
	void OnDayChange();
	void OnLogicDayChanged();

	//每周更新
	void OnLogicWeekChanged();
	
	//更新签到计数
	void UpdateSignInCount(UInt32 day = 0, UInt32 loop = 0);
	//七日日更
	void OnActiveDayChange();

	//日更脚本处理
	bool OnDayChangeScript(ActiveTask* task, bool online, UInt32 offlineDay = 1);

	//获得道具	
	void OnGetItem(UInt32 id, UInt32 num);

	//删除道具
	void OnRmItem(UInt32 id, UInt32 num);

	//开始地下城
	void OnStartDungeon(const DungeonDataEntry* dungeonData,bool hasFriend, bool hasSameGuild);

	//通关地下城
	void OnClearDungeon(UInt32 dungeonId, UInt16 beated, UInt16 areas, LevelScore score, UInt32 usedTime);

	//gm命令通关地下城
	void GMCommandClearDungeon(UInt32 dungeonId, bool isTeam);

	//通关深渊
	void OnClearHell(UInt32 dungeonId, UInt32 usedTime);

	//段位提升
	void OnPkLvUp(UInt32 type, UInt8 lv);

	//通关死亡之塔
	void OnDeathTower(UInt32 towerId, UInt32 layer, UInt32 usedTime);

	//次元石升级
	void OnWarpStoneUpLevel(UInt32 type, UInt32 level);

	//黑市购买道具
	void OnBuyBlackShop();

	//商店购买物品
	void OnShopBuy(UInt32 shopId, UInt32 shopItemId, UInt32 num);

	//购买商城道具
	void OnBuyMallItem(UInt32 mallItemId, UInt32 mallItemNum);

	//决斗
	void OnPVP(PkType type, PkRaceResult status, std::vector<Pk3V3PlayerRaceEndInfo> results = {});

	//强化
	void OnStrengthenEquip();

	//随从洗炼
	void OnRetinueChangeSkill(std::vector<RetinueSkill> newSkills);

	//随从升星事件
	void OnRetinueUpStar(UInt32 retinueId, UInt8 retinueStar);

	//随从升级
	void OnRetinueLvUp(UInt32 retinueId, UInt8 lv);

	//穿戴时装
	void OnWearFashion(UInt32 num);

	//穿戴装备
	void OnWearEquip();

	//开罐子
	void OnOpenMagicJar();

	//附魔
	void OnAddMagic();
	
	//充值
	void OnCharge(const char* reason, UInt32 value);
	void OnEventChargeMoney(IEventParam* param);
	void OnBuyDayChargeWelfare(UInt32 chargeGoodsId, UInt32 chargeNum);

	//vip升级
	void OnVipLvUp();

	//品级调整
	void OnRandQL(IEventParam* param);

	//玩家升级
	void OnEventLevelUp(IEventParam* param);

	//穿脱装备
	void OnChangeEquip(UInt32 id);

	//购买月卡
	void OnBuyMonthCard();

	//点券消费
	void OnMoneyConsume(UInt32 value, const char* reason);

	//事件回调
	void OnEventCallback(IEventParam* param);

public://编码

	void DecodeString(std::istringstream& is);
	void EncodeString(std::ostringstream& os);

	//运营活动任务编解码
	void DecodeOpString(std::istringstream& is);
	void EncodeOpString(std::ostringstream& os);

	void SaveToDB();

	//运营活动任务事件处理
public:

	//手机绑定
	void OnEventBindPhone(IEventParam* param);

	// 预约角色
	void OnAppointmentActivityClose();

	//消耗深渊票
	void OnUsedHellTicket(UInt32 usedNum);

	//消耗远古票
	void OnUsedYuanGuTicket(UInt32 usedNum);

	//消耗疲劳
	void OnRemoveFatigue(UInt16 usedFatigue);

	//道具数量变化
	void OnOpItemSumChange(UInt32 itemId);
	void OnOpItemChangeFashionExchange(UInt32 itemId);
	void OnOpItemChangeFashionWelfare(UInt32 itemId);
	void OnOpChangeFashion(UInt32 itemId);

	// 地下城通关更新运营活动任务
	void OnPlayerClearDungeonForOpActTasks(IEventParam* param);

public:	//运营活动

	//DB加载玩家运营任务数据
	void OnLoadOpActTasks(const std::string& taskInfoStr);

	//DB加载新玩家运营任务数据
	void OnLoadNewOpActTasks(CLRecordCallback* callback);

	/**
	*@brief 整合新旧运营任务
	*/
	void MixOpActTasks();

	/**
	*@brief 查找任务
	*/
	OpActTask* FindNewOpActTask(UInt32 taskId, UInt32 isAccountTask = 0);

	/**
	*@brief 添加任务
	*/
	bool AddNewOpActTask(OpActTask* task, UInt32 accountTask = 0);

	/**
	*@brief 删除任务
	*/
	void DelOpActTask(UInt32 taskDataId);

	/**
	*@brief 访问开放活动
	*/
	void VisitOpenOpActData(const std::function<bool(const OpActivityRegInfo*)>& F);

	/**
	*@brief 访问开放活动+任务配置
	*/
	void VisitOpenOpActAndTaskData(const std::function<bool(const OpActivityRegInfo*, const OpActTaskData&)>& F);

	/**
	*@brief 访问开放活动+任务配置+任务数据
	*/
	void VisitOpenOpActAndTask(const std::function<bool(const OpActivityRegInfo*, const OpActTaskData&, OpActTask*)>& F);

	/**
	*@brief 通过指定活动枚举访问开放活动
	*/
	void VisitOpenOpActDataByOpType(OpActivityType opType, const std::function<bool(const OpActivityRegInfo*)>& F);

	/**
	*@brief 通过指定活动枚举访问开放活动+任务
	*/
	void VisitOpenOpActAndTaskDataByOpType(OpActivityType opType, const std::function<bool(const OpActivityRegInfo*, const OpActTaskData&)>& F);

	/**
	*@brief 通过指定活动枚举访问开放活动+任务
	*/
	void VisitOpenOpActAndTaskByOpType(OpActivityType opType, const std::function<bool(const OpActivityRegInfo*, const OpActTaskData&, OpActTask*)>& F);

	/**
	*@brief 逻辑跨天
	*/
	void OnLogicDayChangeForOpAct();

	/**
	*@brief 逻辑跨周
	*/
	void OnLogicWeekChangeForOpAct();

	/**
	*@brief 触发了world上的招募任务
	*/
	void OnHireTask(HireTaskType type, UInt32 value, std::vector<UInt32> teammateIds = {});

private:
	//任务重置
	void _CheckAndRestTaskByRestType(UInt32 restType);

	bool _CheckAndRestTask(const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task);

	bool _CheckAndUpdateTask(const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task, IEventParam* param);

	//发送运营活动任务完成事件，触发其他关联任务逻辑
	void _SendOpActTaskFinishedEvent(UInt32 taskId);

	//发送上线登陆事件
	void _SendOpActOnlineEvent(bool dayChange = false);

	//按事件子类型分别更新各种运营活动任务
	bool _UpdateTaskBySubEventType(const OpActTaskData& taskData, OpActTask* task);

	//完成地下城
	bool _CheckAndUpdateTaskOnClearDungeon(const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task, IEventParam* param);

	//完成PK
	bool _CheckAndUpdateTaskOnCompletePk(const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task, IEventParam* param);

	//参与地下城
	bool _CheckAndUpdateTaskOnEnterDungeon(const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task, IEventParam* param);

	//完成任务
	bool _CheckAndUpdateTaskOnTaskFinished(const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task, IEventParam* param);

	//上线、登陆
	bool _CheckAndUpdateTaskOnOnline(const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task, IEventParam* param);

public:
	//领取运营活动奖励
	void OnTakeOpActTaskReward(UInt32 opActDataId, UInt32 dataId, UInt64 param);

	//请求运营活动任务
	void OnOpActTasksReq(UInt32 dataId, std::vector<OpActTask>& tasks);

	//活动开始
	void OnOpActOpen(const OpActivityRegInfo* info);

	//活动结束
	void OnOpActClosed(const OpActivityRegInfo* info);

	// 玩家条件是否满足活动要求
	bool IsMeetOpActRequireByPlayer(const OpActivityRegInfo* info);

	// 运营活动任务地下城加成
	void GetOpActivityTaskAddition(OpActivityType type, DungeonAddition& addition, const DungeonDataEntry* dungeonData);
	void GetOpActivityTaskAddition(const DungeonDataEntry* dungeonData, DungeonAddition& addition);
	void GetOpActivityTaskBuff(const DungeonDataEntry* dungeonData, std::vector<RaceBuffInfo>& buffs);

	// 运营活动任务疲劳扣除加成
	UInt32 GetOpActTaskFatigueRmAddition(OpActivityType type, const DungeonDataEntry* dungeonData);

	// 添加回归硬币
	void OnIncReturnToken(UInt32 opActId);

	// 更新运营活动任务参数变量
	void OnUpdateOpTaskParamVar(OpActTask* task);

public:

	/********* 提升装备强化成功概率 *********/

	// 是否装备强化失败不掉等级
	bool IsNotLoseStrenLvAfterEquipStrengthenFaile(UInt8 oldStrenLv, UInt8 upperLv);

	// 提升强化成功概率
	void EnhanceEquipStrengthenRate(UInt8 oldStrenLv, UInt8 upperLv, float& strenRate);

public:

	// 触发回归充值活动
	void ReturnChargeActivity(UInt32 chargeNum);
	// 触发回归消耗深渊票
	void ReturnUseHellTicket(UInt32 useNum);
	// 触发精力商店任务
	void ReturnEnergyShop();
	// 每日刷新挑战深渊活动
	void RefreshDailyChallengeHell(UInt32 isAcc = 0);
	// 处理回归活动过期
	void HandleReturnActExpire();

	// 周签到加载数据
	void LoadWeekSign(CLRecordCallback* callback);
	// 周签到累计奖励领取
	void HandleWeekSignSum(UInt32 opActType, UInt32 weekId);
	// 处理周签到
	void WeekSignTrigger(UInt32 weekSignActType);
	// 从global返回周签到的抽奖奖励
	void HandleWeekSignLottery(UInt32 opActType, UInt32 actId, const ItemRewardVec& rewardVec);

	// GM修改周签到周数
	void GMWeekSignNum(UInt32 num);
	// GM重置周签到任务
	void GMResetWeekSignTask();
	// GM完成周签到任务
	void GMFinishWeekSignTask();

	// GM调整限时深渊次数
	void GMLimitTimeHellNum(UInt32 num);

	// 手动接受运营活动任务
	void AcceptOpActivityTask(UInt32 opActId, UInt32 taskId);

	//挑战者活动触发
	void OnChallengeActivityTask(OpActivityChallengeType type, bool haveExScore, UInt32 param = 0);

	//春季挑战活动触发
	void OnSpringActivityTask(OpActivitySpringType type, UInt32 score);

	//春季挑战活动分增加触发
	void OnSpringScoreAdd(UInt32 num);

	//挑战者活动分增加触发
	void OnChallengeScoreAdd(UInt32 num);

	//2020春季每周签到
	void OnWeekSignSpring(UInt32 dungeonTag);

	//地精币回馈大礼触发
	void OnGnomeCoinAdd(UInt32 num);

	// 挑战者活动
	void OnChallengeActivity(const DungeonDataEntry* dungeonData, const std::vector<ObjID_t>& teammateIds);

public:
	// 触发团本通关活动
	void TeamCopyClearDungeon(UInt32 grade, UInt32 type);
	// 增加团本buff
	void AddTeamCopyBuff(const OpActivityRegInfo* opAct);

	// 触发元旦2020活动
	void NewYear2020(UInt32 type, UInt32 param1 = 1, UInt32 param2 = 0);

	//礼遇特权登陆领奖
	void UpdateGiftRightOnlineTask();

	//礼遇特权团本结算
	void OnGiftRightTeamDungeonTask(UInt32 grade,UInt32 type);

public:
	// 兑换折扣活动
	// 获取兑换任务对应兑换折扣活动任务状态
	UInt8 GetDiscoutOpTaskStByTmpTypeAndExchTaskId(UInt32 tmpType, UInt32 exchTaskId);

	// 设置兑换折扣活动任务状态
	void SetDiscoutOpTaskStByTmpTypeAndExchTaskId(UInt32 tmpType, UInt32 exchTaskId, UInt8 st);

	/****帐号累计在线****/
	void _UpdateAccountCumulateOnlineOpActTask();
public:

	//DB
	void SaveOpTaskDB(OpActTask* task, UInt32 accountTask = 0);
	void UpdateOpTaskDB(OpActTask* task, UInt32 accountTask = 0);
	void InsertOpTaskDB(OpActTask* task, UInt32 accountTask = 0);

private:
	//	接受运营活动
	void _AcceptOpActivities();
	void _AcceptOpActivity(const OpActivityRegInfo* opAct);

	// 接受运营活动任务
	void _AcceptOpActivityTasks(const OpActivityRegInfo* opAct);

	//	更新活动玩家记录数据
	void _UpdateOpActivityPlayerRecordData(const OpActivityRegInfo* opAct);

	//	检查刷新活动count数据
	void _CheckRefreshOpActCounter(const std::string& countName, const OpActivityRegInfo* opAct);

	/********* 每日登录 *********/

	// 接受每日登录任务
	void _AcceptDailyLoginOpTasks(OpActTask* task, const OpActTaskData& taskData);

	// 更新每日登录任务
	void _UpdateDailyLoginOp();

	/********* 玩家升级得奖励 *********/

	// 更新升级任务
	void _UpdatePlayerLevelUpOpActTasks(OpActTask* task, const OpActTaskData& taskData);

	/********* 手机绑定 *********/

	// 更新手机绑定任务
	void _UpdateBindPhoneOpActTasks();

	/********* 决斗币奖励 *********/

	// 决斗结束更新任务
	void _OnPlayerPVPEndForPVPPKCoinOpAct(PkType type, PkRaceResult status);

	/********* 预约职业活动*********/

	// 更新预约职业活动任务
	void _UpdateAppointmentOpActTasks(OpActTask* task, const OpActTaskData& taskData);

	/********* 深渊票抽奖 *********/

	// 更新深渊票抽奖活动任务
	void _UpdateHellTicketOpActTasks(UInt32 usedTickets);

	/********* 疲劳代币 *********/

	// 接受疲劳代币任务
	void _AcceptFatigueTokenOpActTasks(OpActTask* task, const OpActTaskData& taskData);

	// 消耗疲劳
	void _OnRemoveFatigueForFatigueTokenOpAct(UInt32 fatigueNum);

	// 更新疲劳代币活动
	void _UpdateFatigueTokenOpActTasks(const OpActivityRegInfo* opAct);

	// 提交疲劳代币任务
	bool _OnSubmitFatigueToken(const OpActivityRegInfo* opAct, OpActTask* task, OpActTaskData* taskData);

	/********* 疲劳buff *********/

	// 消耗疲劳
	void _OnRemoveFatigueForFatigueBuffOpAct(UInt32 fatigueNum);

	// 添加疲劳buff
	void _AddFatigueBuff(const OpActivityRegInfo* opAct);

	/********* 疲劳燃烧 *********/

	// tick疲劳燃烧活动
	void _OnTickFatigueBurningOpAct(const Avalon::Time& now);

	// 提交疲劳燃烧任务
	bool _OnSubmitFatigueBurning(OpActTask* task, OpActTaskData* taskData);

	// 添加疲劳燃烧buff
	void _AddFatigueBurningBuff(OpActTask* task, const OpActTaskData& taskData);

	/********* 每日奖励 *********/

	// 接受每日奖励任务
	void _AcceptDailyRewardOpActTasks(OpActTask* task);

	/********* 七夕鹊桥 *********/

	// 接受七夕鹊桥任务
	void _AcceptMagpieBridgeOpActTasks(const OpActivityRegInfo* opAct);

	// 七夕鹊桥任务消耗疲劳
	void _OnMagpieBridgeOpTaskRmFatigue(UInt32 fatigueNum);

	// 七夕鹊桥任务跨逻辑天
	void _OnMagpieBridgeOpTaskLogicDayChange();

	/********* 月卡活动 *********/

	// 购买月卡
	void _OnOpActBuyMonthCard();

	/********* buff加成活动 *********/

	// 添加buff
	void _AddBuffAddition(const OpActivityRegInfo* opAct);

	/********* 地下城随机buff活动 *********/

	// 玩家开始地下城
	void _OnPlayerStartDungeonForDungeonRandomBuffOpAct(const DungeonDataEntry* dungeonData);

	/********* 百变换装福利 *********/
	// 接受任务
	void _AcceptChangeFashionOpWelfareActTask(OpActTask* task, const OpActTaskData& taskData);

	// 更新任务
	void _UpdateChangeFashionWelfareOpActTasks(const OpActivityRegInfo* opAct);

	// 提交任务
	bool _OnSubmitChangeFashionWelfareOpActTask(const OpActivityRegInfo* opAct, OpActTask* task, OpActTaskData* taskData);

	/********* 百变时装绝版兑换 *********/
	// 接受任务
	void _AcceptChangeFashionExchangeOpActTask(OpActTask* task, const OpActTaskData& taskData);

	// 更新任务
	void _UpdateChangeFashionExchangeOpActTasks(const OpActivityRegInfo* opAct);

	// 提交任务
	bool _OnSubmitChangeFashionExchangeOpActTask(const OpActivityRegInfo* opAct, OpActTask* task, OpActTaskData* taskData, ObjID_t fashionId);

	/********* 每日buff活动 *********/

	// 自然日更新活动处理
	void _OnDayChangeForDailyBuffOpAct();

	// 更新每日buff活动任务
	void _UpdateDailyBuffOpActTask();

	/********* 神器罐子活动 *********/

	// 自然日更新神器罐子活动处理
	void _OnDayChangeForArtifactJarOpAct();


	/**********新回归活动***********/

	// 检查回归活动数据是否过期
	void _CheckReturnExpire();
	// 回归任务是否过期
	void _ReturnTaskExpire(UInt32 type);
	// 更新账号活动记录
	void _RefreshActivityRecordByAcc(UInt32 accid, UInt32 actId, OpActTask* task);

	// 接任务回归赠礼，按玩家回归时的等级
	void _AcceptReturnPresent(const OpActivityRegInfo* opAct);
	// 接任务精力兑换
	void _AcceptReturnEnergyExchange(const OpActivityRegInfo* opAct);
	// 接取回归任务
	void _AcceptReturnActivityTask(const OpActivityRegInfo* opAct);

	// 回归赠礼
	bool _OnSubmitReturnPresent(const OpActivityRegInfo* opAct, OpActTask* task, OpActTaskData* taskData);
	// 精力兑换活动物品
	bool _OnSubmitEnergyShopExchangeItem(const OpActivityRegInfo* opAct, OpActTask* task, OpActTaskData* taskData);
	// 领取回归充值返利
	bool _OnSubmitReturnChargeRebate(const OpActivityRegInfo* opAct, OpActTask* task, OpActTaskData* taskData);
	// 领取挑战深渊_AcceptOpActivityTasks
	bool _OnSubmitChallengeHell(const OpActivityRegInfo* opAct, OpActTask* task, OpActTaskData* taskData);

	/***********购买赠送活动*************/
	//刷新
	bool _UpdateBuyPresentOpActTask(OpActTask& task, const OpActTaskData& taskData, const OpActivityRegInfo& opAct, UInt32 mallItemId, UInt32 mallItemNum);

	/***********累计通过地下城天数活动****/
	// 地下城通关更新任务
	void _OnPlayerClearDungeonForDungeonPassedCumulateDayOpAct(const DungeonDataEntry* dungeonData);

	/***********暑期挑战活动****/
	// 检查暑期挑战活动的每周挑战任务账号限制
	bool _CheckAcceptTaskAccountLimit(const OpActivityRegInfo* opAct, OpActivityBehavior behavior);
	// 检查暑期挑战活动的每周挑战任务角色限制
	bool _CheckAcceptTaskPlayerLimit(const OpActivityRegInfo* opAct);


	/****周年祈福****/
	// 更新周年祈福活动任务
	void _UpdateSecondAnniversaryPrayOpActTask();

	/***********玩家升级****/
	// 提交时检测奖励物品冒险徽记数量
	bool _OnSubmitOpActCheckMaoxianheijReward(const OpActivityRegInfo* opAct, OpActTask* task, OpActTaskData* taskData);
	
	/****累计在线****/
	void _UpdateCumulateOnlineOpActTask();

	/****礼遇特权****/
	void _UpdateGiftRightOpActTask(const Avalon::Time& now);

	/****春节红包雨****/
	void _UpdateSpringFestivalRainOpActTask();

	/****春节红包领取活动****/
	// 接受任务
	void _AcceptSpringRedPacketRecvOpActTask(OpActTask* task, const OpActTaskData& taskData);
	// 处理充值
	void _OnChargeForSpringRedPacketOpActTask(UInt32 chargeNum);
	// 更新任务
	void _UpdateSpringRedPacketRecvOpActTask(UInt8 type, UInt32 chargeNum);
	
	/****在线好礼****/
	void _UpdateOnlineGiftOpActTask();

	/****充值消费****/
	void _OnMoneyChargeForRechargeAndConsumeOpActTask(const char* reason, UInt32 chargeNum);
	void _OnMoneyConsumeForRechargeAndConsumeOpActTask(UInt32 value, UInt32 reasonId);
	void _OnCheckRechargeAndConsumeOpActTask();


	//神秘卖场
	void _OnPointRemoveSecretSell(UInt32 value, UInt32 reasonId, UInt64 p1, UInt32 p2);


public:
	/****植树节****/
	void UpdatePlantOpActTask();

	//获取周年点券数量
	UInt32 GetSecretCoin();
	//设置周年点券数量
	void SetSecretCoin(UInt32 num);
	//玩家参与大富翁摇骰子
	void OnPlayerMonopolyRoll();
private:

	// 接受顺序任务第一个
	void _AcceptFirstOrderlyTask(const OpActivityRegInfo* opAct);

	// 触发周签到
	bool _WeekSignTrigger(UInt32 opActType, UInt32 weekSignActType);
	// 检查周签到任务的完成
	void _CheckWeekSignFinish(UInt32 actType);
	// 周签到活动
	bool _OnSubmitWeekSign(const OpActivityRegInfo* opAct, OpActTask* task, OpActTaskData* taskData);
	// 通知周签到宝箱数据
	void _NotifyWeekSignBoxData(UInt32 opActType);
	// 通知周签到累计数据
	void _NotifyWeekSignSumData(UInt32 opActType);
	// 增加周签到数据
	void _AddWeekSignData(UInt32 opActType);
	// 刷新周时间宝箱数据
	void _RefreshWeekTimeBox(UInt32 opActType, WeekSignData& signData);
	// 保存数据
	void _ModifyWeekSignData(UInt32 opActType, UInt32 option, UInt32 weekId = 0, UInt32 actId = 0, const ItemRewardVec* rewadVec = NULL);
	void _UpdateWeekSignData(UInt32 opActType, UInt32 option, WeekSignData& signData, CLSaveCommand* cmd, UInt32 weekId = 0, UInt32 actId = 0, const ItemRewardVec* rewadVec = NULL);
	// 周签到跨周刷新
	void _CrossWeekSignTaskReward(UInt32 actType);

	// 全民团购活动打折
	void _AcceptWholeBargain();

private:
	void _CreateActiveTask(UInt32 actID);

	// 处理七日活动结束
	void _OnSevenDayClose(ActiveTask* task);

	//补偿体力任务
	void MakeUpFatgue(bool online);
	void CreatFatigueTask(ActiveTaskDataEntry* data, UInt32 lastFatigue);
	void UpdateFatigueTask(ActiveTask* task, UInt32 lastFatigue);
	bool SubmitFatigueTask(ActiveTask* task, UInt16 perfect, UInt16 num);
	void UpdateMaxFatigue();

	//补偿关卡任务
	void MakeUpDungen(bool online);
	void GetDungenValue(UInt32 id, DungenTaskValue& value);
	void CreatDungenTask(ActiveTaskDataEntry* data, DungenTaskValue& value);
	void UpdateDungenTask(ActiveTask* task, DungenTaskValue& value);
	bool SubmitDungeonTask(ActiveTask* task, UInt16 perfect, UInt16 num);
	UInt32 GetDungeonMissNum(UInt32 subType);
	void UpdateDungeonLastNum();

	//充值活动
	bool IsCharge(UInt32 ActiveId);

	//七日活动
	bool IsSevenDay(UInt32 ActiveId);
	bool CanTakeSevenDayTask(UInt32 ActiveId);

	//月卡活动
	bool IsMonthCard(UInt32 ActiveId);

	//签到
	bool IsSignIn(UInt32 ActiveId);

	//累计签到
	bool IsSignInTotal(UInt32 ActiveId);

	//累计在线奖励
	void UpdateOnlineTimeBonus();

	//到达等级奖励
	void UpdateReachLvBonus();

	//初始化罐子活动
	void InitJarAct();

	//道具兑换	added by yangwenhao
	bool IsItemChange(UInt32 ActiveId);

	// 把任务未领取得奖励发邮件
	void SendUnreciveBonusToMail(ActiveTask* task);

private://通知

	// 发送上线通知信息
	void SendOnlineNotifyInfo();

	// 发送更新通知信息
	void SendUpdateNotifyInfo(UInt32 type, UInt32 param);

private:
	//发送消息
	void SendSyncPhaseGift(UInt32 giftId, UInt8 canTake);

private:
	//注册事件
	void RegistGameEvent();
	void UnRegistGameEvent();

public:
	//罐子活动到罐子ID映射
	UInt32 GetJarIdFromActId(UInt32 actId);
	UInt32 GetActIdFromJarId(UInt32 jarId);

public:
	//DB加载运营活动账号任务数据
	void OnLoadOpActAccountTasks(CLRecordCallback* callback);

	// opTask 是要被删除的， dstTask 是保留下来的
	void OnMergeActAccountTask(OpActivityRegInfo* info, OpActTask* opTask, OpActTask* dstTask);

	/**
	*@brief 查找账号任务
	*/
	OpActTask* FindOpActAccountTask(UInt32 taskId);

	/**
	*@brief 添加账号任务
	*/
	bool AddOpActAccountTask(OpActTask* task);

	/**
	*@brief 清除所有账号任务
	*/
	void ClearAllOpActAccountTasks();

public:
	//领取运营活动账号任务奖励
	void OnTakeOpActAccountTaskReward(UInt32 opActDataId, UInt32 dataId, UInt64 param);

private:
	
	//DB 账号任务
	void SaveOpAccountTaskDB(OpActTask* task);
	void UpdateOpAccountTaskDB(OpActTask* task);
	void InsertOpAccountTaskDB(OpActTask* task);
	void DeleteOpAccountTaskDB(OpActTask* task);

private:
	/**
	*@brief 账号任务检测是否满足账号领奖限制
	*/
	bool _CheckOpTaskAccountSubmitLimit(const OpActTaskData* taskData);

	/**
	*@brief 更新账号任务领奖次数
	*/
	void _UpdateOpTaskAccountSubmitTimes(const OpActTaskData* taskData);

public:
	//
	void OnPlayerLoadDataSuccessForSummerChallengeOpAct();
private:
	// 
	void _UpdateSummerChallengeWeeklyTasksAfterPlayerLoadDataSuccess(const OpActivityRegInfo* opAct);

private:
	ActivityMap		m_ActiveTasks;
	//拥有者
	Player*			m_pOwner;
	//补签点券消耗
	UInt32			m_rpCost;
	//累计在线奖励
	ObjUInt32		m_onlineTimeBonusId;
	//计时事件的下一级礼包
	GiftPhaseData*		m_waitGiftData;

	//到达等级奖励
	ObjUInt32			m_reachLvBonusId;

	//运营活动任务dataid key
	std::map<UInt32, OpActTask*>		m_idToOpActTask;
	//运营活动任务旧数据
	std::vector<OpActTask*>				m_OldOpActTaskMap;

	// 周签到(key->opActType)
	std::map<UInt32, WeekSignData>		m_WeekSignMap;

	// 运营活动账号任务dataid key
	std::map<UInt32, OpActTask*>		m_idToOpActAccountTask;
};

#endif
