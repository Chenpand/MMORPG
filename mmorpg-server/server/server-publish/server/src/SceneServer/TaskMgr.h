#ifndef _TASK_MGR_H_
#define _TASK_MGR_H_

#include <map>
#include <set>
#include <CLObjectProperty.h>
#include <CLTaskDefine.h>
#include <CLDungeonDefine.h>
#include <CLWarpStoneDefine.h>
#include <CLMatchDefine.h>
#include "Task.h"
#include "BaseTaskMgr.h"


class Player;
class Npc;
class PlayerShareData;

/**
 *@brief ���������
 */
class TaskMgr : public BaseTaskMgr
{
	//������ɱ�ʶ��С
#define TASK_FINISHEDFLAG_SIZE ((MAX_MAINSIDE_TASKID - 1) / 8 + 1)

	//typedef std::map<UInt32, Task*> TaskMap;
	//typedef std::set<UInt32>		TaskIdSet;

public:
	TaskMgr();
	~TaskMgr();

public:

	/**
	 *@brief ���ܣ��ύ��ɾ��һ������
	 */
	virtual bool AcceptTask(UInt32 id, UInt8 quality, bool bForce, bool bNotifyErr = true);
	virtual bool SubmitTask(BaseTask* task, bool force = false);
	virtual bool AbandonTask(BaseTask* task);
	virtual bool IsTaskAcceptable(UInt32 id, bool bNotifyErr = false, bool skipPreTask = false);

	/**
	 *@brief ������Ҫ��idΪǰ�����������
	 */
	void AcceptWithOutPreTask(UInt32 id);

	/**
	*@brief ��ȡ���������
	*/
	virtual UInt32 GetMaxTaskNum();

	/**
	*@brief ��ȡ��ǰ��������
	*/
	virtual UInt32 GetTaskNum() const;

	/**
	*@brief ��ȡЭ��ʹ�õ������б�
	*/
	void GetTaskBriefList(std::vector<TaskBriefInfo>& taskList);

	/**
	*@brief ǿ�������������
	*/
	UInt32 FinishAllTask(TaskType taskType = TaskType::TASK_MAX , UInt32 taskId = 0);

	/**
	*@brief תְ
	*/
	virtual void OnChangeOccu();

public://�¼�
	/**
	 *@brief tick�¼�
	 */
	void OnHeartbeat(const Avalon::Time& now, int tickType);

	/**
	 *@brief �������
	 */
	void OnOnline(bool bDayChanged);

	/**
	 *@brief �������
	 */
	void OnLevelup();

private:
	//�ɽ�����
	TaskIdSet	m_AcceptableTasks;
	//ӵ����
	Player*		m_pOwner;
};


class DailyTaskMgr : public BaseTaskMgr
{
public:
	DailyTaskMgr();
	virtual ~DailyTaskMgr() {};

public:
	/**
	*@brief ����һ������
	*/
	virtual bool AcceptTask(UInt32 id, UInt8 quality, bool bForce);

	/**
	*@brief �ύһ������
	*/
	virtual bool SubmitTask(BaseTask* task, bool force = false);

	/**
	*@brief �ύ�����ճ�
	*/
	bool SubmitAllTask();

	/**
	*@brief �޸������ճ�����״̬
	*/
	void SetAllTaskStatus();

	/**
	*@brief �Ƿ��ܹ���ȡһ������
	*/
	virtual bool IsTaskAcceptable(UInt32 id, bool bNotifyErr = false);

	/**
	*@brief ��ȡ���������
	*/
	virtual UInt32 GetMaxTaskNum();

public:
	/**
	*@brief �������
	*/
	void OnOnline(bool bDayChanged);

	/**
	*@brief �������
	*/
	void OnLevelup();

public:

	/**
	*@brief ˢ���ճ����� Add by huchenhui
	*/
	void RefreshDailyTask();

private:

	/**
	*@brief ͬ���ճ����� added by huchenhui 2016.06.23
	*/
	void SyncDailyTask();


	void RandomPoolIndex(UInt32 maxIndex, UInt32 size, std::vector<UInt32>& indexs);

	/**
	*@brief ����ճ����� Add by huchenhui
	*/
	void RandomDailyTask(TaskDataEntryMgr::TaskDataVec& tasks);

	/**
	*@brief ��ȡδ����ճ�  Add by huchenhui
	*/
	std::vector<DailyTask*> GetUnfinishedTasks();

	/**
	*@brief ��ȡ�����Ժ��ܹ���ȡ���ճ�
	*/
	void UplevelAcceptTask(UInt16 level);

private:
	UInt32 m_TaskPoint;

};

class LegendTaskMgr : public BaseTaskMgr
{
public:
	LegendTaskMgr() {};
	virtual ~LegendTaskMgr() {};

	static bool Init();

public:
	virtual bool AddTask(BaseTask* task);

public:
	/**
	*@brief ����һ������
	*/
	bool AcceptTask(TaskDataEntry* taskDataEntry, bool isSync = true);

	/**
	*@brief �ύһ������
	*/
	virtual bool SubmitTask(BaseTask* task, bool force = false);

	/**
	*@brief ����һ������
	*/
	void ResetTask(BaseTask* task);

	/**
	*@brief �Ƿ��ܹ���ȡһ������
	*/
	bool IsTaskAcceptable(TaskDataEntry* taskData);

	/**
	*@brief ��ȡ����֮·�����б�
	*/
	void GetLegendTasks(std::vector<TaskBriefInfo>& tasks);

	/**
	*@brief ��ȡ����֮·�����������
	*/
	virtual UInt32 GetMaxTaskNum() { return MAX_ACCEPT_LEGEND_NUM; }

	/**
	*@brief ����Ƿ��������
	*/
	bool CheckFinishTask(UInt32 taskId);

public:
	/**
	*@brief �������
	*/
	void OnOnline(bool bDayChanged);

	/**
	*@brief �߼�����
	*/
	void OnLogicDayChanged();

	/**
	*@brief �������
	*/
	void OnLevelup();

private:
	void InitLegendTasks(bool isSync = true);
};


class AchievementTaskMgr : public BaseTaskMgr
{
public:
	AchievementTaskMgr() {};
	virtual ~AchievementTaskMgr() {};

public:
	/**
	*@brief ����һ������
	*/
	bool AcceptTask(TaskDataEntry* taskDataEntry, Task::QuestVarMap questVar, bool isSync = true);

	bool AcceptTask(TaskDataEntry* taskDataEntry, bool isSync = true);

	//virtual bool AcceptTask(UInt32 id, UInt8 quality, bool bForce);

	/**
	*@brief �ύһ������
	*/
	virtual bool SubmitTask(BaseTask* task, bool force = false);

	/**
	*@brief �Ƿ��ܹ���ȡһ������
	*/
	virtual bool IsTaskAcceptable(UInt32 id);

	/**
	*@brief ��ȡ���������
	*/
	virtual UInt32 GetMaxTaskNum();

	/**
	*@brief ��ӳɾ�
	*/
	virtual bool AddTask(BaseTask* task);

	/**
	*@brief ����Ƿ��������
	*/
	bool CheckFinishTask(UInt32 taskId);

public:

	/**
	*@brief ��ʼ���ɾ�  added by huchenhui 2016.06.27
	*/
	void InitAchievementTask(bool isSync = true);

	/**
	*@brief ��ȡ����SP�������  added by huchenhui 2017.10.23
	*/
	UInt32 GetRewardSPNum();

public:
	/**
	*@brief �������
	*/
	void OnOnline(bool bDayChanged);

	void OnLevelup();

	void OnTaskSucced(BaseTask* task);

private:

	/**
	*@brief ͬ���ɾ��б� added by huchenhui 2016.06.25
	*/
	void SyncAchievementTask();

};

class CycleTaskMgr : public BaseTaskMgr
{
public:
	CycleTaskMgr();
	virtual ~CycleTaskMgr();

public:

	UInt32 GetMaxTaskNum() { return 1; }

	/**
	*@brief ����һ������
	*/
	bool AcceptTask(TaskDataEntry* taskDataEntry);

	/**
	*@brief �ύһ������
	*/
	virtual bool SubmitTask(BaseTask* task, bool force = false);

	/**
	*@brief �Ƿ��ܹ���ȡһ������
	*/
	bool IsTaskAcceptable(TaskDataEntry* taskDataEntry);

	/**
	*@brief ˢ��ѭ������
	*/
	//UInt32 RefreshCycleTask();

	/**
	*@brief ˢ��ѭ������
	*/
	UInt32 RefreshCycleTask(UInt32 taskId = 0);

	/**
	*@brief ˢ��ѭ������ ѭ������
	*/
	void RefreshCycleTaskDailyCount();

	/**
	*@brief �·������б�
	*/
	//void SyncCycleTask();

	/**
	*@brief ��ȡ�������ɵ���������
	*/
	void GetCanGenTaskType(std::vector<UInt8>& types, UInt8 excludeType = CTT_INVALID);

	/**
	*@brief �������
	*/
	void OnCompleteTaskSucc(BaseTask* task);

	/**
	*@brief �������������ɴ�������
	*/
	void ClearTypeCounter();

public:
	/**
	*@brief �������
	*/
	void OnOnline(bool bDayChanged);

	/**
	*@brief �������
	*/
	void OnLevelup();

private:
	/**
	*@brief ���ѭ����������
	*/
	CycleTaskType RandomCycleType(bool force = false);

	/**
	*@brief ��ʼ������
	*/
	void InitCycle();

	/**
	*@brief ˢ��ѭ������
	*/
	UInt32 RefreshTask(UInt32 taskId = 0, bool force = false);

	/**
	*@brief ����ѭ������
	*/
	void AbandonAllTask();

private:
	UInt32 m_CycleTaskMax;
	UInt32 m_CycleTaskDailyMax;
	UInt32 m_CycleTaskRefreshFreeCount;
	UInt32 m_CycleTaskRefreshConsume;

	UInt32 m_CycleTaskTypeMax[CTT_MAX];
};

class RedPacketTaskMgr : public AchievementTaskMgr
{
public:
	RedPacketTaskMgr() {};
	virtual ~RedPacketTaskMgr() {};

public:
	/**
	*@brief ����һ������
	*/
	bool AcceptTask(TaskDataEntry* taskDataEntry, Task::QuestVarMap questVar);

	//virtual bool AcceptTask(UInt32 id, UInt8 quality, bool bForce);

	/**
	*@brief �ύһ������
	*/
	virtual bool SubmitTask(BaseTask* task, bool force = false);

	/**
	*@brief �Ƿ��ܹ���ȡһ������
	*/
	virtual bool IsTaskAcceptable(UInt32 id, bool bNotifyErr = false);

	/**
	*@brief ��ʼ������  added by huchenhui 2016.12.16
	*/
	void InitRedPacketTask();

public:
	void OnHeartbeat(const Avalon::Time& now, int tickType);

public:
	/**
	*@brief �������
	*/
	void OnOnline(bool bDayChanged);

};

class MasterDailyTaskMgr : public BaseTaskMgr
{
public:
	MasterDailyTaskMgr();
	~MasterDailyTaskMgr() {}
public:
	/**
	*@brief �������
	*/
	void OnOnline(bool bDayChanged);

	/**
	*@brief �������
	*/
	void OnOffline();

	/**
	*@brief ��ȡȫ������
	*/
	void AcceptAllTask();

	/**
	*@brief ����һ������
	*/
	bool AcceptTask(TaskDataEntry* taskDataEntry);

	/**
	*@brief �ύһ������
	*/
	virtual bool SubmitTask(BaseTask* task, bool force = false);

	/**
	*@brief �Ƿ��ܹ���ȡһ������
	*/
	bool IsTaskAcceptable(TaskDataEntry* taskData);

	/**
	*@brief �߼�����
	*/
	void OnLogicDayChanged();

	/**
	*@brief ������������
	*/
	void AbandonAllTask(bool notify = true, bool checkAcceptTime = false);

	/**
	*@brief ��ȡ���������
	*/
	virtual UInt32 GetMaxTaskNum();

	/**
	*@brief ��ȡ��������
	*/
	void GetShareData(MasterTaskShareData& sharedata);

	/**
	*@brief ���ʦ���ճ������Ƿ����
	*/
	bool CheckAllTasksFinished();

	/**
	*@brief ����˺�ʦ���Ĺ�ϵ
	*/
	void OnRelationRelieveMaster(ObjID_t masterId);

	/**
	*@brief ͽ�ܳ�ʦ
	*/
	void OnRelationDiscipleFinishSchool(ObjID_t masterId);

public:
	/**
	*@brief ֱ�Ӳ������ݿ��������,������ߵ�ʱ�����
	*/
	void static ClearOfflinePlayerAllTask(ObjID_t ownerId);
private:
	/*
	*@brief ͬ�����ͻ���
	*/
	void SyncToClient();

	/**
	*@brief ͬ����world
	*/
	void SyncToWorld();
};

class MasterAcademicGrowthTaskMgr : public BaseTaskMgr
{
public:
	MasterAcademicGrowthTaskMgr();
	~MasterAcademicGrowthTaskMgr() {}
public:
	/**
	*@brief �������
	*/
	void OnOnline(bool bDayChanged);

	/*
	*@brief �������
	*/
	void OnOffline();

	/*
	*@brief ��ȡȫ������
	*/
	void AcceptAllTask(bool force = false);

	/*
	*@brief ������������
	*/
	void AbandonAllTask(bool notify = true);

	/**
	*@brief ����һ������
	*/
	bool AcceptTask(TaskDataEntry* taskDataEntry);

	/**
	*@brief �Ƿ��ܹ���ȡһ������
	*/
	bool IsTaskAcceptable(TaskDataEntry* taskData);

	/**
	*@brief �ύһ������
	*/
	virtual bool SubmitTask(BaseTask* task, bool force = false);

	/**
	*@brief ��ȡ���������
	*/
	virtual UInt32 GetMaxTaskNum();

	/**
	*@brief ��ʦ�ɹ����ȡ����
	*/
	void OnAddMaster();

	/**
	*@brief ��ȡ��������
	*/
	void GetShareData(MasterTaskShareData& sharedata);

	/**
	*@brief ����˺�ʦ���Ĺ�ϵ
	*/
	void OnRelationRelieveMaster(ObjID_t masterId);

	/**
	*@brief ͽ�ܳ�ʦ
	*/
	void OnRelationDiscipleFinishSchool(ObjID_t masterId);

	/**
	*@brief ����
	*/
	void OnLevelup();
	
	/**
	*@brief pvp
	*/
	void OnTeamPVP(PkType type, PkRaceResult status, std::vector<ObjID_t>& teammateIds);

	/**
	*@brief ���﹥��
	*/
	void OnTeamKillCityMonster(CityMonsterType cityMonsterType, UInt32 monsterId, UInt64 monsterGuid, std::vector<ObjID_t>& teammateIds);
public:
	/**
	*@brief ֱ�Ӳ������ݿ��������
	*/
	void static ClearOfflinePlayerAllTask(ObjID_t ownerId);
private:
	/*
	*@brief ͬ�����ͻ���
	*/
	void SyncToClient();

	/**
	*@brief ͬ����world
	*/
	void SyncToWorld();

};

#endif
