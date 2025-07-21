#ifndef _BASE_TASK_H_
#define _BASE_TASK_H_

#include <CLTaskDefine.h>
#include <CLTaskDataEntry.h>
#include <CLTaskProtocol.h>
#include <CLObject.h>
#include <map>
#include <sstream>
#include <CLRewardAdapter.h>
#include <CLErrorCode.h>
#include <CLSceneObject.h>
#include <CLSyncObject.h>
#include <CLObjectProperty.h>

class Player;
class RewardGroup;

class TaskQuestVarMap : public CLComplexProperty
{
public:
	typedef std::map<std::string, std::string> QuestVarMap;
public:
	TaskQuestVarMap();
	~TaskQuestVarMap();

public:
	QuestVarMap GetQuestVarMap() { return m_QuestVars; }

	void SetQuestVarMap(QuestVarMap var) { m_QuestVars = var; }

	Int64 GetVar(const char* name) const;

	const char* GetNameVar(const char* name) const;

	void SetVar(std::string key, std::string value);

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

class BaseTask : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(BaseTask, Avalon::NullMutex)
public:
	typedef std::map<std::string, std::string> QuestVarMap;
	typedef std::vector<ItemReward>	ItemRewardVec;

public:
	BaseTask(Player* owner, UInt32 id);
	BaseTask(Player* owner, UInt32 id, UInt64 guid);
	virtual ~BaseTask(){};

public:

	/**
	*@brief 设置获取id
	*/
	UInt32 GetDataId() const { return m_DataId; }

	/**
	*@brief 获取拥有者
	*/
	Player* GetOwner() const { return m_pOwner; }

	/**
	*@brief 设置获取品质
	*/
	void SetQuality(UInt8 quality){ m_Quality = quality; }
	UInt8 GetQuality() const{ return m_Quality; }

	/**
	*@brief 设置任务接取次数
	*/
	void ResetSubmitCount() { m_SubmitCount = 0; }
	void IncSubmitCount() { m_SubmitCount = m_SubmitCount + 1; }
	UInt32 GetSubmitCount() { return m_SubmitCount; }

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
	const char* GetNameVar(const char* name) const;
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

	/**
	*@brief 添加任务道具
	*/
	void AddTaskItem(UInt32 itemId, UInt32 prob);

public:

	/**
	*@brief 设置数据项
	*/
	virtual bool SetupBase() = 0;

	/**
	*@brief 检查任务完成情况
	*/
	virtual bool CheckFinish();

	/**
	*@brief 获取任务类型
	*/
	virtual TaskType GetType() const = 0;

	/**
	*@brief 获取任务子类型
	*/
	virtual TaskSubType GetSubType() const = 0;

	/**
	*@brief 获取任务名称
	*/
	virtual const std::string& GetName() const = 0;

	/**
	*@brief 获取任务限时
	*/
	virtual UInt32 GetLimitTime() const = 0;

	/**
	*@brief 获取任务放弃限时
	*/
	virtual UInt32 GetAbandonLimitTime() const = 0;

	/**
	*@brief 获取任务奖励
	*/
	virtual RewardGroup* GetRewards() const = 0;

	/**
	*@brief 获取任务接取次数
	*/
	virtual UInt32 GetMaxSubmitCount() const = 0;

	/**
	*@brief 检查任务奖励
	*/
	virtual bool CheckGiveRewards();

	/**
	*@brief 获取任务奖励
	*/
	virtual void GiveRewards();

	/**
	*@brief 获取任务原因
	*/
	virtual const char* GetRewardsReason();

	/**
	*@brief 是否需要公告
	*/
	virtual bool IsAnnouncement() const = 0;

	/**
	*@brief 爬塔任务类型
	*/
	virtual UInt32 LostDungeonTaskType() = 0;

	/**
	*@brief 同步数据
	*/
	virtual void SyncData() = 0;

	/**
	*@brief 设置脏数据
	*/

	virtual void OnSetStatus();

	virtual void OnSetVar();

public:
	AVALON_DEFINE_NETSERIALIZATION()
	{
		UInt8 status = m_Status;
		stream & m_Quality & status & m_QuestVars & m_FinishedTime & m_SubmitCount;
	}

	//void DecodeString(std::istringstream& is);
	//void EncodeString(std::ostringstream& os);

	void UpdateTask();
	void InsertTask();
	void DeleteTask();

public:
	CL_DBPROPERTY_BEGIN(CLSyncObject)
	CL_DBPROPERTY("dataid", m_DataId)
	CL_DBPROPERTY("status", m_Status)
	CL_DBPROPERTY("parameter", m_QuestVars)
	CL_DBPROPERTY("finished_time", m_FinishedTime)
	CL_DBPROPERTY("submit_count", m_SubmitCount)
	CL_DBPROPERTY_END()

protected:

	//DataID
	ObjUInt32	m_DataId;

	//品质
	ObjUInt8	m_Quality;

	//拥有者
	Player*		m_pOwner;

	//状态
	ObjUInt32	m_Status;

	//任务接取次数
	ObjUInt8	m_SubmitCount;

	//任务变量
	TaskQuestVarMap m_QuestVars;

	//完成时间
	ObjUInt32	m_FinishedTime;

	//日志字符串
	std::string m_Reason;

};

//任务访问器
typedef CLVisitor<BaseTask>	BaseTaskVisitor;

#endif