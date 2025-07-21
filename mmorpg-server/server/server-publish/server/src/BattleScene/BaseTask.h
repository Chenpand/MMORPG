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
	*@brief ���û�ȡid
	*/
	UInt32 GetDataId() const { return m_DataId; }

	/**
	*@brief ��ȡӵ����
	*/
	Player* GetOwner() const { return m_pOwner; }

	/**
	*@brief ���û�ȡƷ��
	*/
	void SetQuality(UInt8 quality){ m_Quality = quality; }
	UInt8 GetQuality() const{ return m_Quality; }

	/**
	*@brief ���������ȡ����
	*/
	void ResetSubmitCount() { m_SubmitCount = 0; }
	void IncSubmitCount() { m_SubmitCount = m_SubmitCount + 1; }
	UInt32 GetSubmitCount() { return m_SubmitCount; }

	/**
	*@brief ����״̬
	*/
	void SetStatus(TaskStatus status);

	/**
	*@brief ��ȡ״̬
	*/
	TaskStatus GetStatus() const { return (TaskStatus)m_Status.GetDBValue(); }

	/**
	*@brief ͬ��״̬
	*/
	virtual void SyncStatus();

	/**
	*@brief ��ȡ�������
	*/
	Int64 GetVar(const char* name) const;

	/**
	*@brief �����������
	*/
	void SetVar(const char* name, Int64 val);

	/**
	*@brief �����������
	*/
	void ResetVar();

	/**
	*@brief ��ȡ�����ַ����������
	*/
	const char* GetNameVar(const char* name) const;
	void SetNameVar(const char* name, const char* val);

	virtual void SyncVar(std::string key, std::string value);

	/**
	*@brief ��ȡ�����������
	*/
	QuestVarMap GetQuestVar() { return m_QuestVars.GetQuestVarMap(); }

	/**
	*@brief ��ȡ������Ϣ
	*/
	void GetBriefInfo(TaskBriefInfo& info);

	/**
	*@brief ����������
	*/
	void AddTaskItem(UInt32 itemId, UInt32 prob);

public:

	/**
	*@brief ����������
	*/
	virtual bool SetupBase() = 0;

	/**
	*@brief �������������
	*/
	virtual bool CheckFinish();

	/**
	*@brief ��ȡ��������
	*/
	virtual TaskType GetType() const = 0;

	/**
	*@brief ��ȡ����������
	*/
	virtual TaskSubType GetSubType() const = 0;

	/**
	*@brief ��ȡ��������
	*/
	virtual const std::string& GetName() const = 0;

	/**
	*@brief ��ȡ������ʱ
	*/
	virtual UInt32 GetLimitTime() const = 0;

	/**
	*@brief ��ȡ���������ʱ
	*/
	virtual UInt32 GetAbandonLimitTime() const = 0;

	/**
	*@brief ��ȡ������
	*/
	virtual RewardGroup* GetRewards() const = 0;

	/**
	*@brief ��ȡ�����ȡ����
	*/
	virtual UInt32 GetMaxSubmitCount() const = 0;

	/**
	*@brief ���������
	*/
	virtual bool CheckGiveRewards();

	/**
	*@brief ��ȡ������
	*/
	virtual void GiveRewards();

	/**
	*@brief ��ȡ����ԭ��
	*/
	virtual const char* GetRewardsReason();

	/**
	*@brief �Ƿ���Ҫ����
	*/
	virtual bool IsAnnouncement() const = 0;

	/**
	*@brief ������������
	*/
	virtual UInt32 LostDungeonTaskType() = 0;

	/**
	*@brief ͬ������
	*/
	virtual void SyncData() = 0;

	/**
	*@brief ����������
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

	//Ʒ��
	ObjUInt8	m_Quality;

	//ӵ����
	Player*		m_pOwner;

	//״̬
	ObjUInt32	m_Status;

	//�����ȡ����
	ObjUInt8	m_SubmitCount;

	//�������
	TaskQuestVarMap m_QuestVars;

	//���ʱ��
	ObjUInt32	m_FinishedTime;

	//��־�ַ���
	std::string m_Reason;

};

//���������
typedef CLVisitor<BaseTask>	BaseTaskVisitor;

#endif