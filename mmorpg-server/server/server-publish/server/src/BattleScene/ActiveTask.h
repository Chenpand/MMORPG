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
 *@brief ����
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
	 *@brief ���û�ȡid
	 */
	UInt32 GetID() const { return m_Id; }
	UInt64 GetUID()	const { return m_Uid; }
	void SetUID(UInt64 uid) { m_Uid = uid; }
	/**
	 *@brief ��ȡӵ����
	 */
	Player* GetOwner() const { return m_pOwner; }

	/**
	 *@brief ����������
	 */
	void SetData(ActiveTaskDataEntry* data);
	void SetData();
	/**
	 *@brief ��ȡ������                                                                     
	 */
	ActiveTaskDataEntry* GetDataEntry() const{ return m_pDataEntry; }

	/**
	 *@brief ���û�ȡ״̬
	 */
	void SetStatus(UInt8 status);
	void SetStatusNoSync(UInt8 status);
	UInt8 GetStatus() const { return m_Status; }
	void SyncStatus();

	/**
	 *@brief ��ȡ�����������
	 */
	Int64 GetVar(const char* name) const;
	void SetVar(const char* name, Int64 val);
	void SetVarNoSync(const char* name, Int64 val);

	/**
	 *@brief ��ȡ�����ַ����������
	 */
	const char* GetNameVar(const char* name) const;
	void SetNameVar(const char* name, const char* val);

	/**
	*@brief ��ȡ�����������
	*/
	QuestVarMap GetQuestVar() { return m_QuestVars; }
	
	/**
	 *@brief ��ȡ������Ϣ
	 */
	//void GetInfo(TaskInfo& info);
	void GetBriefInfo(TaskBriefInfo& info);

	/**
	 *@brief �������
	 */
	bool CheckGiveRewards();
	void GiveRewards();

	/**
	 *@brief ��ȡ������
	 */
	static RewardGroup* GetRewards(Player* player, ActiveTaskDataEntry* dataEntry);

	/**
	 *@brief ��ý���
	 */
	RewardGroup* GetRewards() const;

	/**
	*@brief ����������
	*/
	void AddTaskItem(UInt32 itemId, UInt32 prob);

	//����ʱ��
	void SetAcceptTime(UInt32 time);
	UInt32 GetAcceptTime();

public://�����
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
	//������
	ActiveTaskDataEntry*	m_pDataEntry;
	//ӵ����
	Player*		m_pOwner;
	//״̬(TaskStatus)
	UInt8		m_Status;
	//�������
	QuestVarMap m_QuestVars;
	//����ʱ��
	UInt32		m_AcceptTime;
	//db����flag
	DBFLAG		m_DbFlag;
};

//���������
typedef CLVisitor<ActiveTask>	AcitiveTaskVisitor;

#endif

