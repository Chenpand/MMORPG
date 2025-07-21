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
	GPT_ONLINE_TIME = 1,	//�ۼ�����ʱ��
	GPT_REACH_LEVEL,		//�ﵽ�ȼ�

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

//�Ѷ�֪ͨ����
class ReadNotifyProperty : public CLComplexProperty
{
public:
	ReadNotifyProperty();
	~ReadNotifyProperty();

public:

	void AddProperty(UInt32 type, UInt32 param);
	bool IsReaded(UInt32 type, UInt32 param);

public://��������

	void DecodeString(std::istringstream& is);
	void EncodeString(std::ostringstream& os);

private:
	//�����б�
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
*@brief ����������
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

	//��������,���ʼ��������
	bool AcceptTask(UInt32 activeId, bool online = false);

	//�������,������������
	bool ClearTask(UInt32 activeId);

	//�ύ����
	bool SubmitTask(UInt32 taskId, bool bRp = false);

	//�ύ��������
	bool SubmitMakeUpTask(UInt32 taskId, UInt32 param1);

	//��ǩ����
	bool SubmitTaskRp(std::vector<UInt32>& taskId);

	bool TakePhaseGift(UInt32 id);

	ActiveTask* FindTask(UInt32 id) const;

	// ÿ�ܸ��»����
	void OnLogicWeekChangeForActivities();

	//ͬ������
	void SyncAllTask();

	//���»����
	void UpdateActiveTasks(UInt32 offlineDay);

	//ǩ�����
	void SighInOnline();												//ǩ�����߸���
	void SignInTotalOnline();											//�ۼ�ǩ�����߸���
	void CreateSignInTask(ActiveTaskDataEntry* data, bool pre, UInt32 loopNum, UInt32 day);			//����ǩ������
	void UpdateSignInTask(ActiveTask* task, bool pre, UInt32 loopNum, UInt32 day);					//����ǩ������
	bool SubmitSignInTask(ActiveTask* task, bool bRp = false);

	//�¿��
	void OnMonthCardWelfare(UInt32 remainDays, bool isRenew, bool bDayChange, bool isOnline);
	void CreateMonthCardTask(UInt32 remainDays, ActiveTaskDataEntry* data);
	void UpdateMonthCardTask(UInt32 remainDays, ActiveTask* task, bool isRenew, bool bDayChange, bool isOnline);

	//�ж��Ƿ��ڻ��
	bool IsInActive(UInt32 activeId, ActiveTask* task);

	//�ж����ʱ���Ƿ��ж�Ӧ���͵Ļ
	bool IsTimeInAcitveType(UInt32 time, UInt32	tmpType);

	//��ȡʣ��ʱ��
	void GetRestTime(UInt32& time1, UInt32& time2);

	//��ȡ��ȡ������ʱ�������Ӧ��ʱ��
	inline ObjUInt32& GetOnlineTimeBonus()	{ return m_onlineTimeBonusId; }
	//��ȡ��ȡ�ĵȼ������Ӧ�ȼ�
	inline ObjUInt32& GetReachLvBonus()	{ return m_reachLvBonusId; }

	//���߶һ�
	void AcceptItemChangeTask(const std::vector<ActiveTaskDataEntry*>& taskDatas);
	//�������߶һ�����
	void CreateItemChangeTask(ActiveTaskDataEntry* data);
	//���µ��߶һ�����
	void UpdateItemChangeTask(ActiveTask* itemChangeTask);
	//�ύ���߶һ�����
	bool SubmitItemChangeTask(ActiveTask* itemChangeTask);
	//��ˢ��
	void OnLogicWeekChangeForItemChangeTask();

	void DayChargeOnline(UInt32 offlineDay);

	/* ÿ�ճ�ֵ(��)*/

	// ����
	void DayChargeNewOnline(bool bDayChanged);
	// ��Ȼ�տ���
	void DayChargeNewOnDayChange();

	//�ܷ���������Ȩ��
	bool CanBuyGiftRightCard();

	//�ܷ�����Ԩ��Ȩ��
	bool CanBuyDungeonRightCard();

	// ��û����Ԩ��Ȩ��
	bool HaveDungeonRightCard();

	// ������Ԩ��Ȩ��
	void SetDungeonRightCard();

public://�¼�

	//tick�¼�
	void OnHeartbeat(const Avalon::Time& now, int tickType);

	//�������
	void OnOnline(bool bDayChanged);
	void OnOffline();

	//��ҳ���
	void OnBirth();

	//������� 
	void OnLevelup();
	
	//һ�ո���
	void OnDayChange();
	void OnLogicDayChanged();

	//ÿ�ܸ���
	void OnLogicWeekChanged();
	
	//����ǩ������
	void UpdateSignInCount(UInt32 day = 0, UInt32 loop = 0);
	//�����ո�
	void OnActiveDayChange();

	//�ո��ű�����
	bool OnDayChangeScript(ActiveTask* task, bool online, UInt32 offlineDay = 1);

	//��õ���	
	void OnGetItem(UInt32 id, UInt32 num);

	//ɾ������
	void OnRmItem(UInt32 id, UInt32 num);

	//��ʼ���³�
	void OnStartDungeon(const DungeonDataEntry* dungeonData,bool hasFriend, bool hasSameGuild);

	//ͨ�ص��³�
	void OnClearDungeon(UInt32 dungeonId, UInt16 beated, UInt16 areas, LevelScore score, UInt32 usedTime);

	//gm����ͨ�ص��³�
	void GMCommandClearDungeon(UInt32 dungeonId, bool isTeam);

	//ͨ����Ԩ
	void OnClearHell(UInt32 dungeonId, UInt32 usedTime);

	//��λ����
	void OnPkLvUp(UInt32 type, UInt8 lv);

	//ͨ������֮��
	void OnDeathTower(UInt32 towerId, UInt32 layer, UInt32 usedTime);

	//��Ԫʯ����
	void OnWarpStoneUpLevel(UInt32 type, UInt32 level);

	//���й������
	void OnBuyBlackShop();

	//�̵깺����Ʒ
	void OnShopBuy(UInt32 shopId, UInt32 shopItemId, UInt32 num);

	//�����̳ǵ���
	void OnBuyMallItem(UInt32 mallItemId, UInt32 mallItemNum);

	//����
	void OnPVP(PkType type, PkRaceResult status, std::vector<Pk3V3PlayerRaceEndInfo> results = {});

	//ǿ��
	void OnStrengthenEquip();

	//���ϴ��
	void OnRetinueChangeSkill(std::vector<RetinueSkill> newSkills);

	//��������¼�
	void OnRetinueUpStar(UInt32 retinueId, UInt8 retinueStar);

	//�������
	void OnRetinueLvUp(UInt32 retinueId, UInt8 lv);

	//����ʱװ
	void OnWearFashion(UInt32 num);

	//����װ��
	void OnWearEquip();

	//������
	void OnOpenMagicJar();

	//��ħ
	void OnAddMagic();
	
	//��ֵ
	void OnCharge(const char* reason, UInt32 value);
	void OnEventChargeMoney(IEventParam* param);
	void OnBuyDayChargeWelfare(UInt32 chargeGoodsId, UInt32 chargeNum);

	//vip����
	void OnVipLvUp();

	//Ʒ������
	void OnRandQL(IEventParam* param);

	//�������
	void OnEventLevelUp(IEventParam* param);

	//����װ��
	void OnChangeEquip(UInt32 id);

	//�����¿�
	void OnBuyMonthCard();

	//��ȯ����
	void OnMoneyConsume(UInt32 value, const char* reason);

	//�¼��ص�
	void OnEventCallback(IEventParam* param);

public://����

	void DecodeString(std::istringstream& is);
	void EncodeString(std::ostringstream& os);

	//��Ӫ���������
	void DecodeOpString(std::istringstream& is);
	void EncodeOpString(std::ostringstream& os);

	void SaveToDB();

	//��Ӫ������¼�����
public:

	//�ֻ���
	void OnEventBindPhone(IEventParam* param);

	// ԤԼ��ɫ
	void OnAppointmentActivityClose();

	//������ԨƱ
	void OnUsedHellTicket(UInt32 usedNum);

	//����Զ��Ʊ
	void OnUsedYuanGuTicket(UInt32 usedNum);

	//����ƣ��
	void OnRemoveFatigue(UInt16 usedFatigue);

	//���������仯
	void OnOpItemSumChange(UInt32 itemId);
	void OnOpItemChangeFashionExchange(UInt32 itemId);
	void OnOpItemChangeFashionWelfare(UInt32 itemId);
	void OnOpChangeFashion(UInt32 itemId);

	// ���³�ͨ�ظ�����Ӫ�����
	void OnPlayerClearDungeonForOpActTasks(IEventParam* param);

public:	//��Ӫ�

	//DB���������Ӫ��������
	void OnLoadOpActTasks(const std::string& taskInfoStr);

	//DB�����������Ӫ��������
	void OnLoadNewOpActTasks(CLRecordCallback* callback);

	/**
	*@brief �����¾���Ӫ����
	*/
	void MixOpActTasks();

	/**
	*@brief ��������
	*/
	OpActTask* FindNewOpActTask(UInt32 taskId, UInt32 isAccountTask = 0);

	/**
	*@brief �������
	*/
	bool AddNewOpActTask(OpActTask* task, UInt32 accountTask = 0);

	/**
	*@brief ɾ������
	*/
	void DelOpActTask(UInt32 taskDataId);

	/**
	*@brief ���ʿ��Ż
	*/
	void VisitOpenOpActData(const std::function<bool(const OpActivityRegInfo*)>& F);

	/**
	*@brief ���ʿ��Ż+��������
	*/
	void VisitOpenOpActAndTaskData(const std::function<bool(const OpActivityRegInfo*, const OpActTaskData&)>& F);

	/**
	*@brief ���ʿ��Ż+��������+��������
	*/
	void VisitOpenOpActAndTask(const std::function<bool(const OpActivityRegInfo*, const OpActTaskData&, OpActTask*)>& F);

	/**
	*@brief ͨ��ָ���ö�ٷ��ʿ��Ż
	*/
	void VisitOpenOpActDataByOpType(OpActivityType opType, const std::function<bool(const OpActivityRegInfo*)>& F);

	/**
	*@brief ͨ��ָ���ö�ٷ��ʿ��Ż+����
	*/
	void VisitOpenOpActAndTaskDataByOpType(OpActivityType opType, const std::function<bool(const OpActivityRegInfo*, const OpActTaskData&)>& F);

	/**
	*@brief ͨ��ָ���ö�ٷ��ʿ��Ż+����
	*/
	void VisitOpenOpActAndTaskByOpType(OpActivityType opType, const std::function<bool(const OpActivityRegInfo*, const OpActTaskData&, OpActTask*)>& F);

	/**
	*@brief �߼�����
	*/
	void OnLogicDayChangeForOpAct();

	/**
	*@brief �߼�����
	*/
	void OnLogicWeekChangeForOpAct();

	/**
	*@brief ������world�ϵ���ļ����
	*/
	void OnHireTask(HireTaskType type, UInt32 value, std::vector<UInt32> teammateIds = {});

private:
	//��������
	void _CheckAndRestTaskByRestType(UInt32 restType);

	bool _CheckAndRestTask(const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task);

	bool _CheckAndUpdateTask(const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task, IEventParam* param);

	//������Ӫ���������¼��������������������߼�
	void _SendOpActTaskFinishedEvent(UInt32 taskId);

	//�������ߵ�½�¼�
	void _SendOpActOnlineEvent(bool dayChange = false);

	//���¼������ͷֱ���¸�����Ӫ�����
	bool _UpdateTaskBySubEventType(const OpActTaskData& taskData, OpActTask* task);

	//��ɵ��³�
	bool _CheckAndUpdateTaskOnClearDungeon(const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task, IEventParam* param);

	//���PK
	bool _CheckAndUpdateTaskOnCompletePk(const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task, IEventParam* param);

	//������³�
	bool _CheckAndUpdateTaskOnEnterDungeon(const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task, IEventParam* param);

	//�������
	bool _CheckAndUpdateTaskOnTaskFinished(const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task, IEventParam* param);

	//���ߡ���½
	bool _CheckAndUpdateTaskOnOnline(const OpActivityRegInfo* opAct, const OpActTaskData& taskData, OpActTask* task, IEventParam* param);

public:
	//��ȡ��Ӫ�����
	void OnTakeOpActTaskReward(UInt32 opActDataId, UInt32 dataId, UInt64 param);

	//������Ӫ�����
	void OnOpActTasksReq(UInt32 dataId, std::vector<OpActTask>& tasks);

	//���ʼ
	void OnOpActOpen(const OpActivityRegInfo* info);

	//�����
	void OnOpActClosed(const OpActivityRegInfo* info);

	// ��������Ƿ�����Ҫ��
	bool IsMeetOpActRequireByPlayer(const OpActivityRegInfo* info);

	// ��Ӫ�������³Ǽӳ�
	void GetOpActivityTaskAddition(OpActivityType type, DungeonAddition& addition, const DungeonDataEntry* dungeonData);
	void GetOpActivityTaskAddition(const DungeonDataEntry* dungeonData, DungeonAddition& addition);
	void GetOpActivityTaskBuff(const DungeonDataEntry* dungeonData, std::vector<RaceBuffInfo>& buffs);

	// ��Ӫ�����ƣ�Ϳ۳��ӳ�
	UInt32 GetOpActTaskFatigueRmAddition(OpActivityType type, const DungeonDataEntry* dungeonData);

	// ��ӻع�Ӳ��
	void OnIncReturnToken(UInt32 opActId);

	// ������Ӫ������������
	void OnUpdateOpTaskParamVar(OpActTask* task);

public:

	/********* ����װ��ǿ���ɹ����� *********/

	// �Ƿ�װ��ǿ��ʧ�ܲ����ȼ�
	bool IsNotLoseStrenLvAfterEquipStrengthenFaile(UInt8 oldStrenLv, UInt8 upperLv);

	// ����ǿ���ɹ�����
	void EnhanceEquipStrengthenRate(UInt8 oldStrenLv, UInt8 upperLv, float& strenRate);

public:

	// �����ع��ֵ�
	void ReturnChargeActivity(UInt32 chargeNum);
	// �����ع�������ԨƱ
	void ReturnUseHellTicket(UInt32 useNum);
	// ���������̵�����
	void ReturnEnergyShop();
	// ÿ��ˢ����ս��Ԩ�
	void RefreshDailyChallengeHell(UInt32 isAcc = 0);
	// ����ع�����
	void HandleReturnActExpire();

	// ��ǩ����������
	void LoadWeekSign(CLRecordCallback* callback);
	// ��ǩ���ۼƽ�����ȡ
	void HandleWeekSignSum(UInt32 opActType, UInt32 weekId);
	// ������ǩ��
	void WeekSignTrigger(UInt32 weekSignActType);
	// ��global������ǩ���ĳ齱����
	void HandleWeekSignLottery(UInt32 opActType, UInt32 actId, const ItemRewardVec& rewardVec);

	// GM�޸���ǩ������
	void GMWeekSignNum(UInt32 num);
	// GM������ǩ������
	void GMResetWeekSignTask();
	// GM�����ǩ������
	void GMFinishWeekSignTask();

	// GM������ʱ��Ԩ����
	void GMLimitTimeHellNum(UInt32 num);

	// �ֶ�������Ӫ�����
	void AcceptOpActivityTask(UInt32 opActId, UInt32 taskId);

	//��ս�߻����
	void OnChallengeActivityTask(OpActivityChallengeType type, bool haveExScore, UInt32 param = 0);

	//������ս�����
	void OnSpringActivityTask(OpActivitySpringType type, UInt32 score);

	//������ս������Ӵ���
	void OnSpringScoreAdd(UInt32 num);

	//��ս�߻�����Ӵ���
	void OnChallengeScoreAdd(UInt32 num);

	//2020����ÿ��ǩ��
	void OnWeekSignSpring(UInt32 dungeonTag);

	//�ؾ��һ������񴥷�
	void OnGnomeCoinAdd(UInt32 num);

	// ��ս�߻
	void OnChallengeActivity(const DungeonDataEntry* dungeonData, const std::vector<ObjID_t>& teammateIds);

public:
	// �����ű�ͨ�ػ
	void TeamCopyClearDungeon(UInt32 grade, UInt32 type);
	// �����ű�buff
	void AddTeamCopyBuff(const OpActivityRegInfo* opAct);

	// ����Ԫ��2020�
	void NewYear2020(UInt32 type, UInt32 param1 = 1, UInt32 param2 = 0);

	//������Ȩ��½�콱
	void UpdateGiftRightOnlineTask();

	//������Ȩ�ű�����
	void OnGiftRightTeamDungeonTask(UInt32 grade,UInt32 type);

public:
	// �һ��ۿۻ
	// ��ȡ�һ������Ӧ�һ��ۿۻ����״̬
	UInt8 GetDiscoutOpTaskStByTmpTypeAndExchTaskId(UInt32 tmpType, UInt32 exchTaskId);

	// ���öһ��ۿۻ����״̬
	void SetDiscoutOpTaskStByTmpTypeAndExchTaskId(UInt32 tmpType, UInt32 exchTaskId, UInt8 st);

	/****�ʺ��ۼ�����****/
	void _UpdateAccountCumulateOnlineOpActTask();
public:

	//DB
	void SaveOpTaskDB(OpActTask* task, UInt32 accountTask = 0);
	void UpdateOpTaskDB(OpActTask* task, UInt32 accountTask = 0);
	void InsertOpTaskDB(OpActTask* task, UInt32 accountTask = 0);

private:
	//	������Ӫ�
	void _AcceptOpActivities();
	void _AcceptOpActivity(const OpActivityRegInfo* opAct);

	// ������Ӫ�����
	void _AcceptOpActivityTasks(const OpActivityRegInfo* opAct);

	//	���»��Ҽ�¼����
	void _UpdateOpActivityPlayerRecordData(const OpActivityRegInfo* opAct);

	//	���ˢ�»count����
	void _CheckRefreshOpActCounter(const std::string& countName, const OpActivityRegInfo* opAct);

	/********* ÿ�յ�¼ *********/

	// ����ÿ�յ�¼����
	void _AcceptDailyLoginOpTasks(OpActTask* task, const OpActTaskData& taskData);

	// ����ÿ�յ�¼����
	void _UpdateDailyLoginOp();

	/********* ��������ý��� *********/

	// ������������
	void _UpdatePlayerLevelUpOpActTasks(OpActTask* task, const OpActTaskData& taskData);

	/********* �ֻ��� *********/

	// �����ֻ�������
	void _UpdateBindPhoneOpActTasks();

	/********* �����ҽ��� *********/

	// ����������������
	void _OnPlayerPVPEndForPVPPKCoinOpAct(PkType type, PkRaceResult status);

	/********* ԤԼְҵ�*********/

	// ����ԤԼְҵ�����
	void _UpdateAppointmentOpActTasks(OpActTask* task, const OpActTaskData& taskData);

	/********* ��ԨƱ�齱 *********/

	// ������ԨƱ�齱�����
	void _UpdateHellTicketOpActTasks(UInt32 usedTickets);

	/********* ƣ�ʹ��� *********/

	// ����ƣ�ʹ�������
	void _AcceptFatigueTokenOpActTasks(OpActTask* task, const OpActTaskData& taskData);

	// ����ƣ��
	void _OnRemoveFatigueForFatigueTokenOpAct(UInt32 fatigueNum);

	// ����ƣ�ʹ��һ
	void _UpdateFatigueTokenOpActTasks(const OpActivityRegInfo* opAct);

	// �ύƣ�ʹ�������
	bool _OnSubmitFatigueToken(const OpActivityRegInfo* opAct, OpActTask* task, OpActTaskData* taskData);

	/********* ƣ��buff *********/

	// ����ƣ��
	void _OnRemoveFatigueForFatigueBuffOpAct(UInt32 fatigueNum);

	// ���ƣ��buff
	void _AddFatigueBuff(const OpActivityRegInfo* opAct);

	/********* ƣ��ȼ�� *********/

	// tickƣ��ȼ�ջ
	void _OnTickFatigueBurningOpAct(const Avalon::Time& now);

	// �ύƣ��ȼ������
	bool _OnSubmitFatigueBurning(OpActTask* task, OpActTaskData* taskData);

	// ���ƣ��ȼ��buff
	void _AddFatigueBurningBuff(OpActTask* task, const OpActTaskData& taskData);

	/********* ÿ�ս��� *********/

	// ����ÿ�ս�������
	void _AcceptDailyRewardOpActTasks(OpActTask* task);

	/********* ��Ϧȵ�� *********/

	// ������Ϧȵ������
	void _AcceptMagpieBridgeOpActTasks(const OpActivityRegInfo* opAct);

	// ��Ϧȵ����������ƣ��
	void _OnMagpieBridgeOpTaskRmFatigue(UInt32 fatigueNum);

	// ��Ϧȵ��������߼���
	void _OnMagpieBridgeOpTaskLogicDayChange();

	/********* �¿�� *********/

	// �����¿�
	void _OnOpActBuyMonthCard();

	/********* buff�ӳɻ *********/

	// ���buff
	void _AddBuffAddition(const OpActivityRegInfo* opAct);

	/********* ���³����buff� *********/

	// ��ҿ�ʼ���³�
	void _OnPlayerStartDungeonForDungeonRandomBuffOpAct(const DungeonDataEntry* dungeonData);

	/********* �ٱ任װ���� *********/
	// ��������
	void _AcceptChangeFashionOpWelfareActTask(OpActTask* task, const OpActTaskData& taskData);

	// ��������
	void _UpdateChangeFashionWelfareOpActTasks(const OpActivityRegInfo* opAct);

	// �ύ����
	bool _OnSubmitChangeFashionWelfareOpActTask(const OpActivityRegInfo* opAct, OpActTask* task, OpActTaskData* taskData);

	/********* �ٱ�ʱװ����һ� *********/
	// ��������
	void _AcceptChangeFashionExchangeOpActTask(OpActTask* task, const OpActTaskData& taskData);

	// ��������
	void _UpdateChangeFashionExchangeOpActTasks(const OpActivityRegInfo* opAct);

	// �ύ����
	bool _OnSubmitChangeFashionExchangeOpActTask(const OpActivityRegInfo* opAct, OpActTask* task, OpActTaskData* taskData, ObjID_t fashionId);

	/********* ÿ��buff� *********/

	// ��Ȼ�ո��»����
	void _OnDayChangeForDailyBuffOpAct();

	// ����ÿ��buff�����
	void _UpdateDailyBuffOpActTask();

	/********* �������ӻ *********/

	// ��Ȼ�ո����������ӻ����
	void _OnDayChangeForArtifactJarOpAct();


	/**********�»ع�***********/

	// ���ع������Ƿ����
	void _CheckReturnExpire();
	// �ع������Ƿ����
	void _ReturnTaskExpire(UInt32 type);
	// �����˺Ż��¼
	void _RefreshActivityRecordByAcc(UInt32 accid, UInt32 actId, OpActTask* task);

	// ������ع����񣬰���һع�ʱ�ĵȼ�
	void _AcceptReturnPresent(const OpActivityRegInfo* opAct);
	// ���������һ�
	void _AcceptReturnEnergyExchange(const OpActivityRegInfo* opAct);
	// ��ȡ�ع�����
	void _AcceptReturnActivityTask(const OpActivityRegInfo* opAct);

	// �ع�����
	bool _OnSubmitReturnPresent(const OpActivityRegInfo* opAct, OpActTask* task, OpActTaskData* taskData);
	// �����һ����Ʒ
	bool _OnSubmitEnergyShopExchangeItem(const OpActivityRegInfo* opAct, OpActTask* task, OpActTaskData* taskData);
	// ��ȡ�ع��ֵ����
	bool _OnSubmitReturnChargeRebate(const OpActivityRegInfo* opAct, OpActTask* task, OpActTaskData* taskData);
	// ��ȡ��ս��Ԩ_AcceptOpActivityTasks
	bool _OnSubmitChallengeHell(const OpActivityRegInfo* opAct, OpActTask* task, OpActTaskData* taskData);

	/***********�������ͻ*************/
	//ˢ��
	bool _UpdateBuyPresentOpActTask(OpActTask& task, const OpActTaskData& taskData, const OpActivityRegInfo& opAct, UInt32 mallItemId, UInt32 mallItemNum);

	/***********�ۼ�ͨ�����³������****/
	// ���³�ͨ�ظ�������
	void _OnPlayerClearDungeonForDungeonPassedCumulateDayOpAct(const DungeonDataEntry* dungeonData);

	/***********������ս�****/
	// ���������ս���ÿ����ս�����˺�����
	bool _CheckAcceptTaskAccountLimit(const OpActivityRegInfo* opAct, OpActivityBehavior behavior);
	// ���������ս���ÿ����ս�����ɫ����
	bool _CheckAcceptTaskPlayerLimit(const OpActivityRegInfo* opAct);


	/****������****/
	// ���������������
	void _UpdateSecondAnniversaryPrayOpActTask();

	/***********�������****/
	// �ύʱ��⽱����Ʒð�ջռ�����
	bool _OnSubmitOpActCheckMaoxianheijReward(const OpActivityRegInfo* opAct, OpActTask* task, OpActTaskData* taskData);
	
	/****�ۼ�����****/
	void _UpdateCumulateOnlineOpActTask();

	/****������Ȩ****/
	void _UpdateGiftRightOpActTask(const Avalon::Time& now);

	/****���ں����****/
	void _UpdateSpringFestivalRainOpActTask();

	/****���ں����ȡ�****/
	// ��������
	void _AcceptSpringRedPacketRecvOpActTask(OpActTask* task, const OpActTaskData& taskData);
	// �����ֵ
	void _OnChargeForSpringRedPacketOpActTask(UInt32 chargeNum);
	// ��������
	void _UpdateSpringRedPacketRecvOpActTask(UInt8 type, UInt32 chargeNum);
	
	/****���ߺ���****/
	void _UpdateOnlineGiftOpActTask();

	/****��ֵ����****/
	void _OnMoneyChargeForRechargeAndConsumeOpActTask(const char* reason, UInt32 chargeNum);
	void _OnMoneyConsumeForRechargeAndConsumeOpActTask(UInt32 value, UInt32 reasonId);
	void _OnCheckRechargeAndConsumeOpActTask();


	//��������
	void _OnPointRemoveSecretSell(UInt32 value, UInt32 reasonId, UInt64 p1, UInt32 p2);


public:
	/****ֲ����****/
	void UpdatePlantOpActTask();

	//��ȡ�����ȯ����
	UInt32 GetSecretCoin();
	//���������ȯ����
	void SetSecretCoin(UInt32 num);
	//��Ҳ������ҡ����
	void OnPlayerMonopolyRoll();
private:

	// ����˳�������һ��
	void _AcceptFirstOrderlyTask(const OpActivityRegInfo* opAct);

	// ������ǩ��
	bool _WeekSignTrigger(UInt32 opActType, UInt32 weekSignActType);
	// �����ǩ����������
	void _CheckWeekSignFinish(UInt32 actType);
	// ��ǩ���
	bool _OnSubmitWeekSign(const OpActivityRegInfo* opAct, OpActTask* task, OpActTaskData* taskData);
	// ֪ͨ��ǩ����������
	void _NotifyWeekSignBoxData(UInt32 opActType);
	// ֪ͨ��ǩ���ۼ�����
	void _NotifyWeekSignSumData(UInt32 opActType);
	// ������ǩ������
	void _AddWeekSignData(UInt32 opActType);
	// ˢ����ʱ�䱦������
	void _RefreshWeekTimeBox(UInt32 opActType, WeekSignData& signData);
	// ��������
	void _ModifyWeekSignData(UInt32 opActType, UInt32 option, UInt32 weekId = 0, UInt32 actId = 0, const ItemRewardVec* rewadVec = NULL);
	void _UpdateWeekSignData(UInt32 opActType, UInt32 option, WeekSignData& signData, CLSaveCommand* cmd, UInt32 weekId = 0, UInt32 actId = 0, const ItemRewardVec* rewadVec = NULL);
	// ��ǩ������ˢ��
	void _CrossWeekSignTaskReward(UInt32 actType);

	// ȫ���Ź������
	void _AcceptWholeBargain();

private:
	void _CreateActiveTask(UInt32 actID);

	// �������ջ����
	void _OnSevenDayClose(ActiveTask* task);

	//������������
	void MakeUpFatgue(bool online);
	void CreatFatigueTask(ActiveTaskDataEntry* data, UInt32 lastFatigue);
	void UpdateFatigueTask(ActiveTask* task, UInt32 lastFatigue);
	bool SubmitFatigueTask(ActiveTask* task, UInt16 perfect, UInt16 num);
	void UpdateMaxFatigue();

	//�����ؿ�����
	void MakeUpDungen(bool online);
	void GetDungenValue(UInt32 id, DungenTaskValue& value);
	void CreatDungenTask(ActiveTaskDataEntry* data, DungenTaskValue& value);
	void UpdateDungenTask(ActiveTask* task, DungenTaskValue& value);
	bool SubmitDungeonTask(ActiveTask* task, UInt16 perfect, UInt16 num);
	UInt32 GetDungeonMissNum(UInt32 subType);
	void UpdateDungeonLastNum();

	//��ֵ�
	bool IsCharge(UInt32 ActiveId);

	//���ջ
	bool IsSevenDay(UInt32 ActiveId);
	bool CanTakeSevenDayTask(UInt32 ActiveId);

	//�¿��
	bool IsMonthCard(UInt32 ActiveId);

	//ǩ��
	bool IsSignIn(UInt32 ActiveId);

	//�ۼ�ǩ��
	bool IsSignInTotal(UInt32 ActiveId);

	//�ۼ����߽���
	void UpdateOnlineTimeBonus();

	//����ȼ�����
	void UpdateReachLvBonus();

	//��ʼ�����ӻ
	void InitJarAct();

	//���߶һ�	added by yangwenhao
	bool IsItemChange(UInt32 ActiveId);

	// ������δ��ȡ�ý������ʼ�
	void SendUnreciveBonusToMail(ActiveTask* task);

private://֪ͨ

	// ��������֪ͨ��Ϣ
	void SendOnlineNotifyInfo();

	// ���͸���֪ͨ��Ϣ
	void SendUpdateNotifyInfo(UInt32 type, UInt32 param);

private:
	//������Ϣ
	void SendSyncPhaseGift(UInt32 giftId, UInt8 canTake);

private:
	//ע���¼�
	void RegistGameEvent();
	void UnRegistGameEvent();

public:
	//���ӻ������IDӳ��
	UInt32 GetJarIdFromActId(UInt32 actId);
	UInt32 GetActIdFromJarId(UInt32 jarId);

public:
	//DB������Ӫ��˺���������
	void OnLoadOpActAccountTasks(CLRecordCallback* callback);

	// opTask ��Ҫ��ɾ���ģ� dstTask �Ǳ���������
	void OnMergeActAccountTask(OpActivityRegInfo* info, OpActTask* opTask, OpActTask* dstTask);

	/**
	*@brief �����˺�����
	*/
	OpActTask* FindOpActAccountTask(UInt32 taskId);

	/**
	*@brief ����˺�����
	*/
	bool AddOpActAccountTask(OpActTask* task);

	/**
	*@brief ��������˺�����
	*/
	void ClearAllOpActAccountTasks();

public:
	//��ȡ��Ӫ��˺�������
	void OnTakeOpActAccountTaskReward(UInt32 opActDataId, UInt32 dataId, UInt64 param);

private:
	
	//DB �˺�����
	void SaveOpAccountTaskDB(OpActTask* task);
	void UpdateOpAccountTaskDB(OpActTask* task);
	void InsertOpAccountTaskDB(OpActTask* task);
	void DeleteOpAccountTaskDB(OpActTask* task);

private:
	/**
	*@brief �˺��������Ƿ������˺��콱����
	*/
	bool _CheckOpTaskAccountSubmitLimit(const OpActTaskData* taskData);

	/**
	*@brief �����˺������콱����
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
	//ӵ����
	Player*			m_pOwner;
	//��ǩ��ȯ����
	UInt32			m_rpCost;
	//�ۼ����߽���
	ObjUInt32		m_onlineTimeBonusId;
	//��ʱ�¼�����һ�����
	GiftPhaseData*		m_waitGiftData;

	//����ȼ�����
	ObjUInt32			m_reachLvBonusId;

	//��Ӫ�����dataid key
	std::map<UInt32, OpActTask*>		m_idToOpActTask;
	//��Ӫ����������
	std::vector<OpActTask*>				m_OldOpActTaskMap;

	// ��ǩ��(key->opActType)
	std::map<UInt32, WeekSignData>		m_WeekSignMap;

	// ��Ӫ��˺�����dataid key
	std::map<UInt32, OpActTask*>		m_idToOpActAccountTask;
};

#endif
