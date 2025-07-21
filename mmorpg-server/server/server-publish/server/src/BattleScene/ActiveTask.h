#ifndef _ACTIVE_TASK_H_
#define _ACTIVE_TASK_H_

#include <CLActiveEventDataEntry.h>
#include <CLTaskDefine.h>
#include <CLTaskProtocol.h>
#include <CLObject.h>
#include <map>
#include <sstream>

class Player;
class RewardGroup;

enum DBFLAG
{
	df_none = 0,
	df_insert,
	df_update,
	df_delete,
};

/**
 *@brief 任务
 */
class ActiveTask
{
	AVALON_DEFINE_OBJECTPOOL(ActiveTask, Avalon::NullMutex)

	typedef std::map<std::string, std::string> QuestVarMap;
	typedef std::vector<ItemReward>	ItemRewardVec;

public: 
	ActiveTask(Player* owner, UInt32 id);
	~ActiveTask();

public:
	/**
	 *@brief 设置获取id
	 */
	UInt32 GetID() const { return m_Id; }
	UInt64 GetUID()	const { return m_Uid; }
	void SetUID(UInt64 uid) { m_Uid = uid; }
	/**
	 *@brief 获取拥有者
	 */
	Player* GetOwner() const { return m_pOwner; }

	/**
	 *@brief 设置数据项
	 */
	void SetData(ActiveTaskDataEntry* data);
	void SetData();
	/**
	 *@brief 获取数据项                                                                     
	 */
	ActiveTaskDataEntry* GetDataEntry() const{ return m_pDataEntry; }

	/**
	 *@brief 设置获取状态
	 */
	void SetStatus(UInt8 status);
	void SetStatusNoSync(UInt8 status);
	UInt8 GetStatus() const { return m_Status; }
	void SyncStatus();

	/**
	 *@brief 获取设置任务变量
	 */
	Int64 GetVar(const char* name) const;
	void SetVar(const char* name, Int64 val);
	void SetVarNoSync(const char* name, Int64 val);

	/**
	 *@brief 获取设置字符串任务变量
	 */
	const char* GetNameVar(const char* name) const;
	void SetNameVar(const char* name, const char* val);

	/**
	*@brief 获取所有任务变量
	*/
	QuestVarMap GetQuestVar() { return m_QuestVars; }
	
	/**
	 *@brief 获取任务信息
	 */
	//void GetInfo(TaskInfo& info);
	void GetBriefInfo(TaskBriefInfo& info);

	/**
	 *@brief 奖励相关
	 */
	bool CheckGiveRewards();
	void GiveRewards();

	/**
	 *@brief 获取任务奖励
	 */
	static RewardGroup* GetRewards(Player* player, ActiveTaskDataEntry* dataEntry);

	/**
	 *@brief 获得奖励
	 */
	RewardGroup* GetRewards() const;

	/**
	*@brief 添加任务道具
	*/
	void AddTaskItem(UInt32 itemId, UInt32 prob);

	//接收时间
	void SetAcceptTime(UInt32 time);
	UInt32 GetAcceptTime();

public://编解码
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_Status & m_QuestVars;
	}

	void DecodeString(std::istringstream& is);
	void EncodeString(std::ostringstream& os);

	void DecodeQuestVar(std::string str);
	std::string EncodeQuestVar();

	void UpdateTask();
	void InsertTask();
	void DeleteTask();

	void SaveToDB();

	inline void SetDBFlag(DBFLAG flag)	{ m_DbFlag = flag; };
private:
	//uid
	UInt64		m_Uid;
	//id
	UInt32		m_Id;
	//数据项
	ActiveTaskDataEntry*	m_pDataEntry;
	//拥有者
	Player*		m_pOwner;
	//状态(TaskStatus)
	UInt8		m_Status;
	//任务变量
	QuestVarMap m_QuestVars;
	//接受时间
	UInt32		m_AcceptTime;
	//db操作flag
	DBFLAG		m_DbFlag;
};

//任务访问器
typedef CLVisitor<ActiveTask>	AcitiveTaskVisitor;

#endif

