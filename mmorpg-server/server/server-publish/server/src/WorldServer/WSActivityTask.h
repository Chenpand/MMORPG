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
	*@brief 设置获取账号id
	*/
	void SetUID(UInt64 uid) { m_Uid = uid; }
	UInt64 GetUID() const { return m_Uid; }

	/**
	*@brief 设置获取账号id
	*/
	void SetAccId(UInt32 accid) { m_AccId = accid; }
	UInt32 GetAccId() const { return m_AccId; }

	/**
	*@brief 设置获取活动id
	*/
	void SetActivityId(UInt32 activityId) { m_ActivityId = activityId; }
	UInt32 GetActivityId() const { return m_ActivityId; }

	/**
	*@brief 设置获取任务id
	*/
	void SetTaskId(UInt32 taskId) { m_TaskId = taskId; }
	UInt32 GetTaskId() const { return m_TaskId; }

	/**
	*@brief 设置获取任务状态
	*/
	void SetStatus(WSPlayer* player, UInt8 status);
	void SetStatusNoSync(UInt8 status);
	UInt8 GetStatus() const { return m_Status; }
	void SyncStatus(WSPlayer* player);

	/**
	*@brief 设置获取任务变量
	*/
	void SetVar(WSPlayer* player, const std::string& key, Int64 val);
	void SetVarNoSync(const std::string& key, Int64 val);
	Int64 GetVar(const std::string& key) const;
	void SyncVar(WSPlayer* player, const std::string& key, Int64 val);
	const VarsMap& GetVars() const { return m_Vars; }

	/**
	*@brief 设置获取任务接受时间
	*/
	void SetAcceptTime(UInt32 time) { m_AcceptTime = time; }
	UInt32 GetAcceptTime() const { return m_AcceptTime; }

	/**
	*@brief 设置获取更新时间
	*/
	void SetUpdateTime(UInt32 time) { m_UpdateTime = time; }
	UInt32 GetUpdateTime() const { return m_UpdateTime; }

	/**
	*@brief 设置获取日更时间
	*/
	void SetDayChangeTime(UInt32 time);
	UInt32 GetDayChangeTime() const { return m_DayChangeTime; }

public:
	const std::string LogStr() const;

public:
	/**
	*@brief 刷新任务(只有状态初始为接受和任务变量初始为0才可以使用,否则自行实现)
	*/
	void Refresh(WSPlayer* player);
	void RefreshNoSync();

public:/* 编解码 */
	void DecodeVar(const std::string& str);
	std::string EncodeVar();

public:/* 数据库 */
	void InsertToDB();
	void UpdateToDB();
	void SaveToDB(bool canUpdateTime = true);
	void SetDBFlag(DBFLAG flag)	{ m_DBFlag = flag; };

private:
	// uid
	UInt64	m_Uid;
	// 账号id
	UInt32	m_AccId;
	// 活动id
	UInt32	m_ActivityId;
	// 任务id
	UInt32	m_TaskId;
	// 任务状态
	UInt8	m_Status;
	// 任务变量
	VarsMap m_Vars;
	// 接受时间
	UInt32	m_AcceptTime;
	// 更新时间
	UInt32	m_UpdateTime;
	// 日更时间
	UInt32  m_DayChangeTime;
	// 数据库操作
	DBFLAG	m_DBFlag;
};

typedef std::vector<WSActivityTask*> WSActivityTaskVec;

#endif