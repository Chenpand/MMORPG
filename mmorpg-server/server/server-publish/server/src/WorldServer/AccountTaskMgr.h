#ifndef _ACCOUNT_TASK_MGR_H_
#define _ACCOUNT_TASK_MGR_H_

#include <map>
#include <set>
#include <CLTaskDefine.h>
#include <CLDungeonDefine.h>

#include "AccountTask.h"

class WSPlayer;
class DungeonDataEntry;

class AccountTaskMgr
{
public:
	typedef std::map<UInt32, AccountTask*>	TaskMap;
	typedef std::set<UInt32>			TaskIdSet;

public:
	AccountTaskMgr();
	virtual ~AccountTaskMgr();

public:
	AccountInfo* GetOwner() const { return m_pOwner; }
	void SetOwner(AccountInfo* owner) { m_pOwner = owner; }

public:
	virtual bool AddTask(AccountTask* task);

	/**
	*@brief ����һ������
	*/
	bool LoadTask(AccountTask* task);

	/**
	*@brief ����һ������
	*/
	AccountTask* FindTask(UInt32 id) const;

	/**
	*@brief ɾ��һ������
	*/
	void RemoveTask(AccountTask* task);

	/**
	*@brief �����������
	*/
	void ClearTask();

	/**
	*@brief ��ȡ������
	*/
	virtual UInt32 GetTaskNum() const;

	/**
	*@brief ������������
	*/
	void VisitTasks(AccountTaskVisitor& visitor);

	/**
	*@brief ��ȡ�����б�
	*/
	void GetTaskList(std::vector<TaskBriefInfo>& task);

	virtual bool GetInitTaskBriefInfo(UInt32 id, TaskBriefInfo& info);

	virtual void CheckFinish();

	virtual UInt32 SetTaskItem(UInt32 taskId, std::vector<WorldSetTaskItemStruct>& items);
public:
	/**
	*@brief �����������������
	*/
	virtual void OnCheckPackageEnoughForRewardsRes(UInt32 taskId, const ItemRewardVec& rewards);

public:
	/**
	*@brief ���й���Ȩ��npc����
	*/
	void OnOwnedNpcDied(UInt32 npcid, UInt8 type, UInt32 dungeon);

public:
	void SaveToDB();

protected:
	void OnAcceptOneTask(AccountTask* pTask);

	void OnSubmitOneTask(AccountTask* pTask);
protected:

	//����map
	TaskMap		m_Tasks;

	AccountInfo* m_pOwner;

};

class GetAccountTaskInfoVisitor : public AccountTaskVisitor
{
public:
	GetAccountTaskInfoVisitor(std::vector<TaskBriefInfo>& taskList)
		:m_TaskList(taskList) {}

	bool operator()(AccountTask* task)
	{
		m_TaskList.resize(m_TaskList.size() + 1);
		task->GetBriefInfo(*(m_TaskList.end() - 1));
		return true;
	}
private:
	std::vector<TaskBriefInfo>& m_TaskList;
};



class WeeklyTaskMgr : public AccountTaskMgr
{
public:
	WeeklyTaskMgr();
	virtual ~WeeklyTaskMgr() {};

public:
	/**
	*@brief ����һ������
	*/
	bool AcceptTask(const TaskDataEntry* taskDataEntry, bool isSync = true);

	/**
	*@brief �ύһ������
	*/
	bool SubmitTask(AccountTask* task, bool force = false);

	/**
	*@brief �Ƿ��ܹ���ȡһ������
	*/
	bool IsTaskAcceptable(const TaskDataEntry* dataEntry, bool bNotifyErr = false);

	/**
	*@brief ˢ������
	*/
	void RefreshTasks();

	/**
	*@brief ��ȡ���ύ��ð�ն�������
	*/
	UInt32 GetOverAdventureTeamTasks();

	/**
	*@brief ���һ������
	*/
	bool FinishTask(UInt32 id, bool force = false);

	UInt32 SetTaskItem(UInt32 taskId, std::vector<WorldSetTaskItemStruct>& items);
public:
	/**
	*@brief �������
	*/
	void OnPlayerOnline(WSPlayer* player);

	/**
	*@brief �߼�����
	*/
	void OnLogicWeekChange();

	/**
	*@brief �����������������
	*/
	virtual void OnCheckPackageEnoughForRewardsRes(UInt32 taskId, const ItemRewardVec& rewards);

	/**
	*@brief ���ͨ��
	*/
	void OnPlayerClearDungeon(const DungeonDataEntry* dungeonData, LevelScore score, UInt32 usedTime);
	void OnPlayerClearDungeonForTask(const DungeonDataEntry* dungeonData, LevelScore score, UInt32 usedTime, TaskSubType taskSubType);

	/**
	*@brief �������ƣ��
	*/
	void OnPlayerRemoveFatigue(UInt32 removeFatigue);

	bool OnCheckItemForSubmitItemTaskRet(UInt32 taskId);
private:
	/**
	*@brief ��������
	*/
	void _AcceptTasks();

	/**
	*@brief ��������ɾ������
	*/
	void _RemoveTasksByType(TaskType taskType);

	/**
	*@brief �Ƿ��ѽ���ð�ն�����
	*/
	bool _HasAcceptedAdventureTeamTasks();

	/**
	*@brief ����ð�ն�����
	*/
	void _AcceptAdventureTeamTasks();

};


#endif //_ACCOUNT_TASK_MGR_H_