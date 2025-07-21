#ifndef _ACCOUNT_ACHIEVEMENT_TASK_MGR_H_
#define _ACCOUNT_ACHIEVEMENT_TASK_MGR_H_

#include "AccountTaskMgr.h"

struct AccountInfo;

class AccountAchievementTask : public AccountTask
{
	AVALON_DEFINE_OBJECTPOOL(AccountAchievementTask, Avalon::NullMutex)

public:
	AccountAchievementTask(AccountInfo* owner, UInt32 id);
	AccountAchievementTask(AccountInfo* owner, UInt32 id, QuestVarMap questVar);
	AccountAchievementTask(AccountInfo* owner, UInt32 id, UInt64 guid, TaskStatus status, std::string questVarStr, UInt32 finTime);

};

class AccountAchievementTaskMgr : public AccountTaskMgr
{
public:
	AccountAchievementTaskMgr() {};
	virtual ~AccountAchievementTaskMgr() {};

public:
	/**
	*@brief ����һ������
	*/
	bool AcceptTask(TaskDataEntry* taskDataEntry, AccountTask::QuestVarMap questVar, bool isSync = true);

	//virtual bool AcceptTask(UInt32 id, UInt8 quality, bool bForce);

	/**
	*@brief �ύһ������
	*/
	virtual bool SubmitTask(AccountTask* task, bool force = false);

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
	//virtual bool AddTask(AccountTask* task);

	/**
	*@brief ����Ƿ��������
	*/
	bool CheckFinishTask(UInt32 taskId);

	/**
	*@brief �����������������
	*/
	void OnCheckPackageEnoughForRewardsRes(UInt32 taskId, const ItemRewardVec& rewards);
public:
	/**
	*@brief ��ʼ���˺ųɾ�
	*/
	void InitAccountAchievementTask(bool isSync = true);

public:
	void OnPlayerOnline(WSPlayer* player);

	void OnCreateAdventTeam();

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
	*@brief ���n��ָ���ȼ����ܳ�����
	*/
	void OnCompleteAdventureWeeklyTask(UInt32 taskLevel);
private:

	/**
	*@brief ͬ���ɾ��б�
	*/
	void SyncAccountAchievementTask();

	/**
	*@brief ��ʼ��ѯ��ɫ�ȼ���ְҵ��Ϣ
	*/
	void _BeginQueryRoleLevelAndOccuInfos();

	/**
	*@brief ���������ɫ��������
	*/
	void _OnCheckCondRoleNumTasks(WSPlayer* player, UInt8 reason, UInt32 oldLvl = 0);

};

#endif //_ACCOUNT_ACHIEVEMENT_TASK_MGR_H_