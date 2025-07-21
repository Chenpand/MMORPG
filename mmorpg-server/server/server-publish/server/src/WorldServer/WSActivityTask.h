#ifndef _ACCOUNT_ACTIVITY_TASK_H_
#define _ACCOUNT_ACTIVITY_TASK_H_


#include <CLTaskDefine.h>

enum DBFLAG
{
	df_none = 0,
	df_insert,
	df_update,
	df_delete,
};

class WSPlayer;

class WSActivityTask
{
	AVALON_DEFINE_OBJECTPOOL(WSActivityTask, Avalon::NullMutex)

	typedef std::map<std::string, std::string> VarsMap;

public:
	WSActivityTask();
	~WSActivityTask();

public:
	/**
	*@brief ���û�ȡ�˺�id
	*/
	void SetUID(UInt64 uid) { m_Uid = uid; }
	UInt64 GetUID() const { return m_Uid; }

	/**
	*@brief ���û�ȡ�˺�id
	*/
	void SetAccId(UInt32 accid) { m_AccId = accid; }
	UInt32 GetAccId() const { return m_AccId; }

	/**
	*@brief ���û�ȡ�id
	*/
	void SetActivityId(UInt32 activityId) { m_ActivityId = activityId; }
	UInt32 GetActivityId() const { return m_ActivityId; }

	/**
	*@brief ���û�ȡ����id
	*/
	void SetTaskId(UInt32 taskId) { m_TaskId = taskId; }
	UInt32 GetTaskId() const { return m_TaskId; }

	/**
	*@brief ���û�ȡ����״̬
	*/
	void SetStatus(WSPlayer* player, UInt8 status);
	void SetStatusNoSync(UInt8 status);
	UInt8 GetStatus() const { return m_Status; }
	void SyncStatus(WSPlayer* player);

	/**
	*@brief ���û�ȡ�������
	*/
	void SetVar(WSPlayer* player, const std::string& key, Int64 val);
	void SetVarNoSync(const std::string& key, Int64 val);
	Int64 GetVar(const std::string& key) const;
	void SyncVar(WSPlayer* player, const std::string& key, Int64 val);
	const VarsMap& GetVars() const { return m_Vars; }

	/**
	*@brief ���û�ȡ�������ʱ��
	*/
	void SetAcceptTime(UInt32 time) { m_AcceptTime = time; }
	UInt32 GetAcceptTime() const { return m_AcceptTime; }

	/**
	*@brief ���û�ȡ����ʱ��
	*/
	void SetUpdateTime(UInt32 time) { m_UpdateTime = time; }
	UInt32 GetUpdateTime() const { return m_UpdateTime; }

	/**
	*@brief ���û�ȡ�ո�ʱ��
	*/
	void SetDayChangeTime(UInt32 time);
	UInt32 GetDayChangeTime() const { return m_DayChangeTime; }

public:
	const std::string LogStr() const;

public:
	/**
	*@brief ˢ������(ֻ��״̬��ʼΪ���ܺ����������ʼΪ0�ſ���ʹ��,��������ʵ��)
	*/
	void Refresh(WSPlayer* player);
	void RefreshNoSync();

public:/* ����� */
	void DecodeVar(const std::string& str);
	std::string EncodeVar();

public:/* ���ݿ� */
	void InsertToDB();
	void UpdateToDB();
	void SaveToDB(bool canUpdateTime = true);
	void SetDBFlag(DBFLAG flag)	{ m_DBFlag = flag; };

private:
	// uid
	UInt64	m_Uid;
	// �˺�id
	UInt32	m_AccId;
	// �id
	UInt32	m_ActivityId;
	// ����id
	UInt32	m_TaskId;
	// ����״̬
	UInt8	m_Status;
	// �������
	VarsMap m_Vars;
	// ����ʱ��
	UInt32	m_AcceptTime;
	// ����ʱ��
	UInt32	m_UpdateTime;
	// �ո�ʱ��
	UInt32  m_DayChangeTime;
	// ���ݿ����
	DBFLAG	m_DBFlag;
};

typedef std::vector<WSActivityTask*> WSActivityTaskVec;

#endif