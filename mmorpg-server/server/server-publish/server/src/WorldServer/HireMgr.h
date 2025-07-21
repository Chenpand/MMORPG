#ifndef __HIRE_MGR_H__
#define __HIRE_MGR_H__

#include <AvalonSingleton.h>
#include <string.h>
#include <AvalonDefine.h>
#include <map>
#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <AvalonSimpleTimer.h>
#include <CLRecordClient.h>
#include <CLObjectProperty.h>
#include <CLRelationDefine.h>

class WSPlayer;

class HireTaskDataEntry;

enum HireIdentify
{
	//无身份
	HI_INVAILD = 1 << 0,
	//新人
	HI_NEWBIE = 1 << 1,
	//老人
	HI_OLDMAN = 1 << 2,
	//回归
	HI_RETURNMAN = 1 << 3,
};

/**
 *@brief 任务状态
 */
enum HireTaskStatus
{
	HIRE_TASK_INIT = 0,		//初始状态
	HIRE_TASK_UNFINISH = 1,	//已经接任务
	HIRE_TASK_FINISHED = 2,  //已完成，未提交
	HIRE_TASK_FAILED = 3,	//失败
	HIRE_TASK_OVER = 5,		//已完成,已提交
};

enum HireRefreshType
{
	HRT_NULL = 0,
	HRT_WEEK = 1,
	HRT_MONTH = 2,
};

struct CompleteInfo
{
	CompleteInfo(UInt32 accid, UInt64 roleId, const std::string name) : accid(accid), roleId(roleId), name(name) {}
	UInt32 accid;
	UInt64 roleId;
	std::string name;
};

struct HireTask : public CLComplexProperty
{
	HireTask() = default;
	~HireTask() = default;

	void Input(Avalon::NetInputStream& stream);
	void Output(Avalon::NetOutputStream& stream);

	void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty);
	void Decode(Avalon::NetInputStream& stream);

	void DecodeString(std::istringstream& is);
	void EncodeString(std::ostringstream& os);

	void CopyFromDataEntry(HireTaskDataEntry* dataEntry);
	
	UInt32 cnt = 0;
	UInt32 status = HIRE_TASK_INIT;
	std::vector<CompleteInfo> completeInfoVec;

	//配置字段
	UInt32 taskID = 0;  //任务id
	UInt32 type;		//任务类型
	UInt32 fullCnt;		//完成所需计数
	ItemRewardVec rewards; //任务奖励
	UInt32 refreshType;		//刷新周期
	UInt32 identity;        //适用身份
	std::vector<UInt32> param; //参数
};

class HireInfo : public CLSyncObject
{
public:
	HireInfo();
	~HireInfo();

public:
	inline void SetAccid(UInt32 accid) { m_Accid = accid; };
	inline UInt32 GetAccid() { return m_Accid; };
	inline void SetCode(std::string code) { m_Code = code; };
	inline std::string GetCode() { return m_Code; };
	inline void SetBindAccid(UInt32 accid) { m_BindAccid = accid; };
	inline UInt32 GetBindAccid() { return m_BindAccid; };
	inline void SetIdentify(HireIdentify identify) { m_Identify = m_Identify|identify; };
	inline UInt32 GetIdentify() { return static_cast<UInt32>(m_Identify); };
	inline bool IsIdentify(HireIdentify identify) { return m_Identify & identify; };
	inline void SetTaskString(std::string taskStr) { m_TaskString = taskStr; };
	inline void SetBindTime(UInt32 time) { m_BindTime = time; };
	inline UInt32 GetBindTime() { return m_BindTime; };

	void AddBindMeAccid(UInt32 accid) { m_BindMeSet.insert(accid); };

	void AddNewTask(HireTaskDataEntry* data);

	std::vector<HireTask*> GetHireTaskList();

	HireTask* GetHireTask(UInt32 taskId);

	HashSet<UInt32>& GetBindMeSet() { return m_BindMeSet; };

	HirePlayerData GetOnlinePlayerData() { return m_OnlinePlayerData; };

	void OnPlayerOnline(WSPlayer* player);
	void OnPlayerOffline(WSPlayer* player);

	void OnPlayerTrigeTask(WSPlayer* player, HireTaskType type, UInt32 value);
	UInt32 OnPlayerSubmitTask(WSPlayer* player, UInt32 taskId);

	void OnPlayerTrigeBindTask(UInt32 value, UInt32 accid, UInt64 roleId, const std::string& name);

	void OnPlayerChangeName(UInt64 roleId, const std::string& name);

	void OnWeekChange();
	void OnMonthChange();

	void CheckTask();

public:
	void SaveToDB();
	void InsertToDB();
	void DeleteFromDB();
	void Init();
	void ComboHireInfo(HireInfo &abandon);

	CL_DBPROPERTY_BEGIN(CLSyncObject)
		CL_DBPROPERTY("accid", m_Accid)
		CL_DBPROPERTY("code", m_Code)
		CL_DBPROPERTY("bindaccid", m_BindAccid)
		CL_DBPROPERTY("identify", m_Identify)
		CL_DBPROPERTY("tasks", m_TaskString)
		CL_DBPROPERTY("bindtime", m_BindTime)
		CL_DBPROPERTY_END()

private:
	//账号id
	ObjUInt32 m_Accid;
	//邀请码
	ObjString m_Code;
	//我绑定的账号id
	ObjUInt32 m_BindAccid;
	//身份 HireIdentify
	ObjUInt32 m_Identify;
	//任务信息
	ObjString m_TaskString;
	// 绑定别人的时间
	ObjUInt32 m_BindTime;
	//在线玩家信息
	HirePlayerData m_OnlinePlayerData;


private:
	//绑定了我的账号id
	HashSet<UInt32> m_BindMeSet;
	//任务id->任务信息
	std::map<UInt32, HireTask*> m_HireTaskMap;

private:
	void SaveTaskToString();
	void LoadTaskFromString();
};


class HireMgr : public Avalon::Singleton<HireMgr>
{
public:
	HireMgr();
	~HireMgr();
	HireInfo* AddHireInfo(UInt32 accid, UInt32 zoneId);
	HireInfo* GetHireInfo(UInt32 accid);
	HireInfo* GetHireInfo(std::string code);
	void OnPlayerOnline(WSPlayer* player);
	void OnPlayerOffline(WSPlayer* player);
	UInt32 OnPlayerUseCode(WSPlayer* player, std::string code);
	UInt32 OnPlayerUseCodeSuccess(WSPlayer* player, std::string code);
	std::vector<HirePlayerData> OnPlayerQueryHireList(WSPlayer* player);
	void OnPlayerTrigeTask(WSPlayer* player, HireTaskType type, UInt32 value, std::vector<UInt32> teammateIds);
	UInt32 OnPlayerSubmitTask(WSPlayer* player, UInt32 taskId);
	void OnPlayerNameChange(ObjID_t roleId, const std::string& name);
public:
	bool Init();
	void OnTick(const Avalon::Time &now);
	void OnWeekChange();
	void OnMonthChange();
	bool LoadDBData(CLRecordCallback* callback);
	void OnLoadDBEnd();

private:
	//邀请码->账号
	HashMap<std::string, UInt32> m_CodeAccidMap;
	//账号->招募信息
	HashMap<UInt32, HireInfo> m_AccidHireInfoMap;
	//合服导致的重复帐号
};


#endif // __HIRE_MGR_H__





