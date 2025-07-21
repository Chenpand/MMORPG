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
	*@brief ӵ����
	*/
	AccountInfo* GetOwner() { return m_pOwner; }

	/**
	*@brief ���û�ȡid
	*/
	UInt32 GetDataId() const { return m_DataId; }

	/**
	*@brief ���û�ȡƷ��
	*/
	void SetQuality(UInt8 quality){ m_Quality = quality; }
	UInt8 GetQuality() const{ return m_Quality; }

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

public:
	/**
	*@brief ����������
	*/
	virtual bool SetupBase();

	/**
	*@brief �������������
	*/
	virtual bool CheckFinish();

	/**
	*@brief ��ȡ������
	*/
	TaskDataEntry* GetDataEntry() const{ return m_pDataEntry; }

	/**
	*@brief ��ȡ��������
	*/
	virtual TaskType GetType() const{ return m_pDataEntry->type; }
	virtual TaskSubType GetSubType() const{ return m_pDataEntry->subType; }
	virtual const std::string& GetName() const { return m_pDataEntry->name; }
	virtual UInt32 GetLimitTime() const{ return m_pDataEntry->timeLimit; }
	virtual UInt32 GetAbandonLimitTime() const{ return m_pDataEntry->abandomTimeLimit; }
	virtual bool IsAnnouncement() const { return m_pDataEntry->isAnnouncement; }

	/**
	*@brief ��ȡ�����ȡ����
	*/
	virtual UInt32 GetMaxSubmitCount() const { return m_pDataEntry->submitCount; }

	/**
	*@brief ���������
	*/
	virtual void CheckGiveRewards();

	/**
	*@brief ��ȡ������
	*/
	virtual ItemRewardVec GetRewards();

	/**
	*@brief ����������
	*/
	virtual void GiveRewards();

	/**
	*@brief ��ȡ����ԭ��
	*/
	virtual const char* GetRewardsReason();

	virtual void OnSetStatus();

	virtual void OnSetVar();

	/**
	*@brief ˢ��
	*/
	virtual void OnRefresh();

	/**
	*@brief ��ȡ�ű�����
	*/
	Int32 GetScriptParm();

	/**
	*@brief ��־��
	*/
	std::string LogStr();

	/**
	*@brief �����ύ����
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

	//ӵ����
	AccountInfo* m_pOwner;

	//DataID
	ObjUInt32	m_DataId;

	//Ʒ��
	ObjUInt8	m_Quality;

	//״̬
	ObjUInt32	m_Status;

	//�����ȡ����
	ObjUInt8	m_SubmitCount;

	//�������
	AccountTaskQuestVarMap m_QuestVars;

	//���ʱ��
	ObjUInt32	m_FinishedTime;

	//������
	TaskDataEntry*	m_pDataEntry;

	//��־�ַ���
	std::string m_Reason;

	//��������
	ObjUInt8	m_TaskType;

	//����ʱ��
	ObjUInt32	m_CreateTime;
};

//���������
typedef CLVisitor<AccountTask>	AccountTaskVisitor;

class AdventureWeeklyTask : public AccountTask
{
	AVALON_DEFINE_OBJECTPOOL(AdventureWeeklyTask, Avalon::NullMutex)
public:
	AdventureWeeklyTask(AccountInfo* owner, UInt32 id);
	AdventureWeeklyTask(AccountInfo* owner, UInt32 id, UInt64 guid, TaskStatus status, std::string questVarStr);

public:
	/**
	*@brief ��ý���
	*/
	virtual ItemRewardVec GetRewards();

private:
};

#endif //_ACCOUNT_TASK_H_