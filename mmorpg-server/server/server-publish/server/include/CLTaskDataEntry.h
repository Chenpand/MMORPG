#ifndef _CL_TASK_DATAENTRY_H_
#define _CL_TASK_DATAENTRY_H_

#include "CLDefine.h"
#include "CLItemDefine.h"
#include "CLTaskDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <map>

/**
 *@brief �������ñ�
 * ID	��������	�������	��������	ǰ������ID	��С��ҵȼ�  ְҵҪ��	������(��ƷID)	��ȡ����	�ύ����:	��������NPC	�ύnpc	 �������	���񿪹�	
 * �����������ø�ʽ��ItemID1_Counter1,ItemID2_Counter2,ItemID3_Counter3
 */
struct TaskDataEntry : public Avalon::DataEntry
{
public:
	TaskDataEntry();
	~TaskDataEntry();

public:
	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);
	UInt8 GetTargetOccupation();

	UInt32 GetDailyTaskScore();

	std::vector<ItemReward> GetOccuItemRewards(Occupation occu);

	bool IsLegendOccuItemCheck(Occupation occu);

	bool CheckOccu(Occupation occu);

public:

	//����id
	UInt32	id;
	//������
	std::string	name;

	//�������0
	UInt32 param0;

	//��������
	TaskType	type;
	//����������
	TaskSubType subType;
	//ѭ����������
	CycleTaskType cycleType;
	//ѭ������Ȩ��
	UInt32		cycleWeight;

	//���������
	DailyTaskPoolType poolType;

	// ����ر��
	UInt32		poolId;

	//��������
	UInt32 dungeon;
	//ǰ������
	UInt32	preTask;

	//added by aprilliu,2016.05.17  ��ʶ�������Ƿ��Զ����ܸ�����
	UInt8 isServerTake;

	//���ȼ�
	UInt16	maxLevel;
	//�����ȼ�
	UInt16	triggerLevel;
	//ְҵҪ��
	//UInt8	occupation;
	std::vector<Occupation> occus;

	//ְҵ����
	std::vector<OccuItemReward> occuRewards;

	//����
	std::vector<ItemReward>	rewards;

	//��������ID
	UInt32 rewardAdapterId;

	//��ȡ����
	UInt8 publishType;
	//�ύ����
	UInt8 submitType;
	//��������NPC��Ҳ���ǽ�ȡNPC
	UInt32 acceptNpc;
	//�����ύNPC
	UInt32 submitNPC;

	//�Ƿ񹫸�
	bool isAnnouncement;

	//�����ύ����
	UInt32 submitCount;

	//����ֵ
	UInt32 vitality;

	//�������   added by aprilliu, 2016.05.25
	std::string params;

	//ģ��id
	UInt32 templateId;

	//�ű�����
	std::string scriptParam;

	//���񿪹�
	UInt8 open;

	//�����������ȼ�
	UInt16 FinishRightNowLevel;

	//������������������
	UInt32 FinishRightNowItemType;

	//������������������
	UInt32 FinishRightNowItemNum;


	//noted by aprilliu, 2016.04.12 , not used 
	//����
	std::string	describe;
	//δ������Ŀ��
	std::string	initTarget;
	//δ�������Ŀ��
	std::string	unfinishedTarget;
	//�������Ŀ��
	std::string	finishedTarget;
	//��ʱ
	UInt32	timeLimit;
	//������ʱ
	UInt32	abandomTimeLimit;
};

/**
 *@brief ���������������
 */
class TaskDataEntryMgr : public Avalon::DataEntryMgrBase<TaskDataEntry>
	, public Avalon::Singleton<TaskDataEntryMgr >
{
public:
	typedef std::vector<UInt32> TaskIdVec;
	typedef std::map<UInt32, TaskIdVec> TaskIdVecMap;

	typedef std::vector<TaskDataEntry*>	TaskDataVec;
	typedef std::map<UInt32, TaskDataVec> TaskDataVecMap;
	typedef std::map<UInt8, TaskDataVec> DailyTaskDataMap;

	typedef std::map<UInt16, TaskDataVec> LevelTaskDataMap;

	struct DailyTaskSubPool
	{
		DailyTaskSubPool() : pooId(0) { }
		UInt32 pooId;
		std::vector<TaskDataEntry*> dailyTasks;
	};

	struct CycleTypeTasks
	{
		CycleTypeTasks() : sumWeight(0) {}

		TaskDataVec cycleTask;
		UInt32 sumWeight;
	};

	typedef std::map<UInt16, std::map<CycleTaskType, CycleTypeTasks>> CycleTaskDataMap;

	typedef std::vector<TaskDataEntry*> DailyTaskVec;
	typedef std::vector<DailyTaskSubPool> DailyTaskPool;
	typedef std::map<DailyTaskPoolType, DailyTaskPool> DailyTaskPoolMap;
	typedef std::map<UInt16, DailyTaskPoolMap> LevelPoolMap;
	typedef std::map<UInt16, DailyTaskVec> SureDailyTaskMap;


	TaskDataEntryMgr();
	~TaskDataEntryMgr();

public:
	bool AddEntry(TaskDataEntry* entry);

	/**
	 *@brief ���ݵȼ���ȡ�ɽ������б�
	 */
	const TaskDataVec& GetLvlTasks(UInt16 level) const;
	/**
	 *@brief ���������ȡ���������б�
	 */
	const TaskDataVec& GetPostTasks(UInt32 taskid) const;
	/**
	 *@brief ����npc��ȡ�ɽ������б�
	 */
	void GetAcceptTasks(UInt32 npcid, TaskIdVec& tasks) const;
	/**
	 *@brief ����npc��ȡ���ύ�����б�
	 */
	void GetSubmitTasks(UInt32 npcid, TaskIdVec& tasks) const;

	//added by aprilliu, 2016.04.20 ����dungeonId�õ���Ӧ�������б�
	void GetDungeonTasks(UInt32 dungeonId, TaskIdVec& tasks) const;

	/**
	 *@brief ���ݵȼ�����ճ������б�
	 */
	void GetDailyTasks(TaskType types, UInt16 level, TaskDataVec& tasks);

	/**
	*@brief ��óɾ��б�   add by huchenhui 2016.06.25
	*/
	void GetAchievementTasks(UInt16 level, TaskDataVec& tasks);

	/**
	*@brief ����˺ųɾ��б�   add by huchenhui 2019.02.15
	*/
	void GetAccountAchievementTasks(TaskDataVec& tasks);

	/**
	*@brief ���Ӷ��������
	*/
	void GetAllAdventureTeamTasks(TaskDataVec& tasks);
	bool RandAdventureTeamTasks(TaskDataVec& tasks, UInt32 taskGrade, UInt32 taskNum);
	bool RandAdventureTeamTasks(TaskDataVec& tasks, const std::map<UInt32, UInt32>& taskGrades);

	/**
	*@brief ���ݳɾͻ�ȡ���������б�   add by huchenhui 2016.07.14
	*/
	const TaskDataVec& GetPostAchievementTasks(UInt32 taskid) const;

	/**
	*@brief ��ú������   add by huchenhui 2016.12.16
	*/
	void GetRedPackTasks(TaskDataVec& tasks);

	/**
	*@brief ���ݺ�������ȡ���������б�   add by huchenhui 2016.12.16
	*/
	const TaskDataVec& GetPostRedPackTasks(UInt32 taskId) const;

	const DailyTaskPool& GetDailyTaskPool(UInt16 level, DailyTaskPoolType type);

	const DailyTaskVec& GetSureDailyTasks(UInt16 level);

	/**
	*@brief ���ݵȼ������ͻ�ȡѭ�������б�   add by huchenhui 2016.10.28
	*/
	const CycleTypeTasks& GetCycleTasks(UInt16 level, CycleTaskType type);

	/**
	*@brief ���ݵȼ������ͻ�ȡѭ�������б�   add by huchenhui 2016.10.28
	*/
	TaskDataEntry* RandomCycleTask(UInt16 level, CycleTaskType type);

	/**
	*@brief ���ݵȼ���ȡ����֮·�����б�   add by huchenhui 2017.08.22
	*/
	const TaskDataVec& GetLegendTasks(UInt16 level);

	/**
	*@brief ��ȡʦ���ճ������б�
	*/
	const TaskDataVec& GetMasterDailyTasks();

	/**
	*@brief ��ȡʦ���ճ�����
	*/
	const  TaskDataEntry* GetMasterDailyTask(UInt32 key);

	/**
	*@brief ��ȡʦ��ѧҵ�ɳ������б�
	*/
	const TaskDataVec& GetMasterAcademicGrowthTasks();
	
	/**
	*@brief ��ȡʦ��ѧҵ�ɳ�����
	*/
	const TaskDataEntry* GetMasterAcademicGrowthTask(UInt32 key);
private:
	void OnAddEntry(TaskDataEntry* entry);

private:
	//�ȼ�����map ��ʾҪ��ĳ�ȼ��������б�
	TaskDataVecMap	m_LvlTasks;
	//��������map  ��ʾĳ������ĺ��������б�Ҳ���Ǹ�������ĳЩ�����ǰ������
	TaskDataVecMap	m_PostTasks;
	//npc-�ɽ�����map  ��ʾ������ĳ��NPC���� �ɽ��ܵ������б�
	TaskIdVecMap	m_AcceptTasks;
	//npc-�ɽ�����map  ����ĳ��npc�Ͻ��ӵ������б�
	TaskIdVecMap	m_SubmitTasks;

	//����ӵ�е������б� added by aprilliu, 2016.04.12
	TaskIdVecMap m_DungeonTasks;

	//�ճ�����map   ĳ���͵��ճ������б�
	DailyTaskDataMap	m_DailyTasks;

	//�ɾ�map		��ʾ�ɾ��б�
	TaskDataVec		m_AchievementTasks;

	//�ɾͺ�������map	��ʾĳ���ɾ͵ĺ��������б�
	TaskDataVecMap	m_PostAchievementTask;

	//�����
	LevelPoolMap m_PoolMap;

	//�س����ճ�
	SureDailyTaskMap m_SureDailyTasks;

	//���ݵȼ���ѭ���б�
	CycleTaskDataMap	m_LevelCycleTasks;

	//�������
	TaskDataVec			m_RedPacketTasks;
	//��������������map
	TaskDataVecMap		m_PostRedPacketTasks;

	//���ݵȼ��Ĵ���֮·�б�
	LevelTaskDataMap	m_LevelLegendTasks;

	//ʦ���ճ�����
	TaskDataVec			m_MasterDailyTasks;
	//ʦ��ѧҵ�ɳ�����
	TaskDataVec			m_MasterAcademicGrowthTasks;

	//�˺ųɾ�
	TaskDataVec			m_AccountAchievementTasks;

	//����ð�ն�(Ӷ����)����
	TaskDataVec			m_AllAdventureTeamTasks;
	//ð�ն�(Ӷ����)C������
	TaskDataVec			m_AdventureTeamCGTasks;
	//ð�ն�(Ӷ����)B������
	TaskDataVec			m_AdventureTeamBGTasks;
	//ð�ն�(Ӷ����)A������
	TaskDataVec			m_AdventureTeamAGTasks;
	//ð�ն�(Ӷ����)S������
	TaskDataVec			m_AdventureTeamSGTasks;
	//ð�ն�(Ӷ����)SS������
	TaskDataVec			m_AdventureTeamSSGTasks;
	//ð�ն�(Ӷ����)SSS������
	TaskDataVec			m_AdventureTeamSSSGTasks;
};

/**
 *@brief �ճ������
 */
struct DailyTaskDataEntry : public Avalon::DataEntry
{
public:
	DailyTaskDataEntry();
	~DailyTaskDataEntry();

public:
	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);

public:

	//ID
	UInt32		id;

	//���������
	DailyTaskPoolType type;

	// ����ر��
	UInt32		poolId;

	//taskDataEntry
	TaskDataEntry* taskDataEntry;

};

/**
 *@brief �ճ����������������
 */
class DailyTaskDataEntryMgr : public Avalon::DataEntryMgrBase<DailyTaskDataEntry>, public Avalon::Singleton<DailyTaskDataEntryMgr>
{
public:
	DailyTaskDataEntryMgr();
	~DailyTaskDataEntryMgr();

public:
	bool AddEntry(DailyTaskDataEntry* entry);

private:

};

struct DailyTaskScoreRewardDataEntry : public Avalon::DataEntry
{
public:
	DailyTaskScoreRewardDataEntry();
	~DailyTaskScoreRewardDataEntry();

public:
	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);

public:
	UInt32 id;

	//����
	UInt32 score;

	//������Ʒ
	std::vector<ItemReward> itemReward;

};

typedef Avalon::DataEntryMgr<DailyTaskScoreRewardDataEntry> DailyTaskScoreRewardDataEntryMgr;


class AchievementLevelDataEntry : public Avalon::DataEntry
{
public:
	AchievementLevelDataEntry();
	~AchievementLevelDataEntry();

public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	UInt32 id;

	//��С����
	UInt32 minScore;
	//������
	UInt32 maxScore;

	//����
	std::vector<ItemReward> itemReward;
};

typedef Avalon::DataEntryMgr<AchievementLevelDataEntry> AchievementLevelDataEntryMgr;


class WeekSignSpringDataEntry : public Avalon::DataEntry
{
public:
	WeekSignSpringDataEntry();
	virtual ~WeekSignSpringDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);



public:
	// ID
	UInt32                                          id;
	// ��ʼ�ȼ�
	UInt32                                          startLv;
	// �����ȼ�
	UInt32                                          endLv;
	// ���³�tag
	std::vector<UInt32>                             dungeonTag;


};

class WeekSignSpringDataEntryMgr : public Avalon::DataEntryMgrBase<WeekSignSpringDataEntry>
	, public Avalon::Singleton<WeekSignSpringDataEntryMgr>
{
public:
	virtual bool AddEntry(WeekSignSpringDataEntry* entry);
	bool IsInScope(UInt32 lv, UInt32 tag);
};

#endif
