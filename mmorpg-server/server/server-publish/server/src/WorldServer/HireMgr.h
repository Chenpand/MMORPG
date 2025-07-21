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
	//�����
	HI_INVAILD = 1 << 0,
	//����
	HI_NEWBIE = 1 << 1,
	//����
	HI_OLDMAN = 1 << 2,
	//�ع�
	HI_RETURNMAN = 1 << 3,
};

/**
 *@brief ����״̬
 */
enum HireTaskStatus
{
	HIRE_TASK_INIT = 0,		//��ʼ״̬
	HIRE_TASK_UNFINISH = 1,	//�Ѿ�������
	HIRE_TASK_FINISHED = 2,  //����ɣ�δ�ύ
	HIRE_TASK_FAILED = 3,	//ʧ��
	HIRE_TASK_OVER = 5,		//�����,���ύ
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

	//�����ֶ�
	UInt32 taskID = 0;  //����id
	UInt32 type;		//��������
	UInt32 fullCnt;		//����������
	ItemRewardVec rewards; //������
	UInt32 refreshType;		//ˢ������
	UInt32 identity;        //�������
	std::vector<UInt32> param; //����
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
	//�˺�id
	ObjUInt32 m_Accid;
	//������
	ObjString m_Code;
	//�Ұ󶨵��˺�id
	ObjUInt32 m_BindAccid;
	//��� HireIdentify
	ObjUInt32 m_Identify;
	//������Ϣ
	ObjString m_TaskString;
	// �󶨱��˵�ʱ��
	ObjUInt32 m_BindTime;
	//���������Ϣ
	HirePlayerData m_OnlinePlayerData;


private:
	//�����ҵ��˺�id
	HashSet<UInt32> m_BindMeSet;
	//����id->������Ϣ
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
	//������->�˺�
	HashMap<std::string, UInt32> m_CodeAccidMap;
	//�˺�->��ļ��Ϣ
	HashMap<UInt32, HireInfo> m_AccidHireInfoMap;
	//�Ϸ����µ��ظ��ʺ�
};


#endif // __HIRE_MGR_H__





