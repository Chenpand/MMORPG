#ifndef _ACCOUNT_TASK_H_
#define _ACCOUNT_TASK_H_

#include <CLTaskDefine.h>
#include <CLTaskDataEntry.h>
#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <TimeUtil.h>

struct AccountInfo;
class RewardGroup;

class AccountTaskQuestVarMap : public CLComplexProperty
{
public:
	typedef std::map<std::string, std::string> QuestVarMap;
public:
	AccountTaskQuestVarMap();
	~AccountTaskQuestVarMap();

public:
	void SetQuestVarMap(const QuestVarMap& var);
	QuestVarMap& GetQuestVarMap() { return m_QuestVars; }

	Int64 GetVar(const char* name) const;

	void SetVar(const std::string& key, const std::string& value);

public:
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_QuestVars;
	}

	void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty);
	void Decode(Avalon::NetInputStream& stream);

	void DecodeString(std::istringstream& is);
	void EncodeString(std::ostringstream& os);

private:
	QuestVarMap	m_QuestVars;
};


class AccountTask : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(AccountTask, Avalon::NullMutex)

public:
	typedef std::map<std::string, std::string> QuestVarMap;
	typedef std::vector<ItemReward>	ItemRewardVec;

public:
	AccountTask(AccountInfo* owner, UInt32 id);
	AccountTask(AccountInfo* owner, UInt32 id, QuestVarMap questVar);
	AccountTask(AccountInfo* owner, UInt32 id, UInt64 guid, TaskStatus status, std::string questVarStr);
	virtual ~AccountTask(){};

public:

	/**
	*@brief 拥有者
	*/
	AccountInfo* GetOwner() { return m_pOwner; }

	/**
	*@brief 设置获取id
	*/
	UInt32 GetDataId() const { return m_DataId; }

	/**
	*@brief 设置获取品质
	*/
	void SetQuality(UInt8 quality){ m_Quality = quality; }
	UInt8 GetQuality() const{ return m_Quality; }

	/**
	*@brief 设置状态
	*/
	void SetStatus(TaskStatus status);

	/**
	*@brief 获取状态
	*/
	TaskStatus GetStatus() const { return (TaskStatus)m_Status.GetDBValue(); }

	/**
	*@brief 同步状态
	*/
	virtual void SyncStatus();

	/**
	*@brief 获取任务变量
	*/
	Int64 GetVar(const char* name) const;

	/**
	*@brief 设置任务变量
	*/
	void SetVar(const char* name, Int64 val);

	/**
	*@brief 重置任务变量
	*/
	void ResetVar();

	/**
	*@brief 获取设置字符串任务变量
	*/
	void SetNameVar(const char* name, const char* val);

	virtual void SyncVar(std::string key, std::string value);

	/**
	*@brief 获取所有任务变量
	*/
	QuestVarMap GetQuestVar() { return m_QuestVars.GetQuestVarMap(); }

	/**
	*@brief 获取任务信息
	*/
	void GetBriefInfo(TaskBriefInfo& info);

public:
	/**
	*@brief 设置数据项
	*/
	virtual bool SetupBase();

	/**
	*@brief 检查任务完成情况
	*/
	virtual bool CheckFinish();

	/**
	*@brief 获取数据项
	*/
	TaskDataEntry* GetDataEntry() const{ return m_pDataEntry; }

	/**
	*@brief 获取基本属性
	*/
	virtual TaskType GetType() const{ return m_pDataEntry->type; }
	virtual TaskSubType GetSubType() const{ return m_pDataEntry->subType; }
	virtual const std::string& GetName() const { return m_pDataEntry->name; }
	virtual UInt32 GetLimitTime() const{ return m_pDataEntry->timeLimit; }
	virtual UInt32 GetAbandonLimitTime() const{ return m_pDataEntry->abandomTimeLimit; }
	virtual bool IsAnnouncement() const { return m_pDataEntry->isAnnouncement; }

	/**
	*@brief 获取任务接取次数
	*/
	virtual UInt32 GetMaxSubmitCount() const { return m_pDataEntry->submitCount; }

	/**
	*@brief 检查任务奖励
	*/
	virtual void CheckGiveRewards();

	/**
	*@brief 获取任务奖励
	*/
	virtual ItemRewardVec GetRewards();

	/**
	*@brief 给予任务奖励
	*/
	virtual void GiveRewards();

	/**
	*@brief 获取任务原因
	*/
	virtual const char* GetRewardsReason();

	virtual void OnSetStatus();

	virtual void OnSetVar();

	/**
	*@brief 刷新
	*/
	virtual void OnRefresh();

	/**
	*@brief 获取脚本参数
	*/
	Int32 GetScriptParm();

	/**
	*@brief 日志串
	*/
	std::string LogStr();

	/**
	*@brief 处理提交任务
	*/
	virtual void OnSubmited();

	virtual void OnSubmitedAdventWeeklyTask();

public:
	AVALON_DEFINE_NETSERIALIZATION()
	{
		UInt8 status = m_Status;
		stream & m_Quality & status & m_QuestVars & m_FinishedTime & m_SubmitCount;
	}

	//void DecodeString(std::istringstream& is);
	//void EncodeString(std::ostringstream& os);

	void SaveDBTask();
	void DeleteDBTask();

public:
	CL_DBPROPERTY_BEGIN(CLSyncObject)
		CL_DBPROPERTY("dataid", m_DataId)
		CL_DBPROPERTY("status", m_Status)
		CL_DBPROPERTY("parameter", m_QuestVars)
		CL_DBPROPERTY("finished_time", m_FinishedTime)
		CL_DBPROPERTY("submit_count", m_SubmitCount)
		CL_DBPROPERTY("type", m_TaskType)
		CL_DBPROPERTY("create_time", m_CreateTime)
		CL_DBPROPERTY_END()

protected:

	//拥有者
	AccountInfo* m_pOwner;

	//DataID
	ObjUInt32	m_DataId;

	//品质
	ObjUInt8	m_Quality;

	//状态
	ObjUInt32	m_Status;

	//任务接取次数
	ObjUInt8	m_SubmitCount;

	//任务变量
	AccountTaskQuestVarMap m_QuestVars;

	//完成时间
	ObjUInt32	m_FinishedTime;

	//数据项
	TaskDataEntry*	m_pDataEntry;

	//日志字符串
	std::string m_Reason;

	//任务类型
	ObjUInt8	m_TaskType;

	//创建时间
	ObjUInt32	m_CreateTime;
};

//任务访问器
typedef CLVisitor<AccountTask>	AccountTaskVisitor;

class AdventureWeeklyTask : public AccountTask
{
	AVALON_DEFINE_OBJECTPOOL(AdventureWeeklyTask, Avalon::NullMutex)
public:
	AdventureWeeklyTask(AccountInfo* owner, UInt32 id);
	AdventureWeeklyTask(AccountInfo* owner, UInt32 id, UInt64 guid, TaskStatus status, std::string questVarStr);

public:
	/**
	*@brief 获得奖励
	*/
	virtual ItemRewardVec GetRewards();

private:
};

#endif //_ACCOUNT_TASK_H_