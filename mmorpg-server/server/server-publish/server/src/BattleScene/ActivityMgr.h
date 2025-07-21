#ifndef _ACTIVITY_MGR_H_
#define _ACTIVITY_MGR_H_

#include <functional>
#include <AvalonSingleton.h>
#include <CLGameDefine.h>
#include <map>
#include <vector>
#include <string>
#include "CLActivityDefine.h"

class Player;
class DungeonDataEntry;
struct Buff;

#define SIGN_IN_REFRESH_NUM		30			//ǩ��ˢ������


/**
 *@brief �
 */
class ActivityMgr : public Avalon::Singleton<ActivityMgr>
{
	typedef std::vector<ActivityInfo>	ActivityVec;
	typedef std::map<ObjID_t, ActivityVec>	TribeActivityMap;

public:
	ActivityMgr();
	~ActivityMgr();

public:
	/**
	 *@brief �����رջ
	 */
	void OpenActivity(UInt32 id, const std::string& name, UInt16 level, UInt32 preTime, UInt32 startTime, UInt32 dueTime, UInt8 state);
	void CloseActivity(UInt32 id);
	
	/**
	 *@brief �����رղ���
	 */
	void OpenTribeActivity( ObjID_t uTribeId, const std::string& name, UInt16 level, UInt32 dueTime);
	void CloseTribeActivity(ObjID_t uTribeId, const std::string& name);

	/**
	 *@brief ���û
	 */
	void SetActivities(const ActivityVec& activities, const TribeActivityMap &tribeactivities);

	/**
	 *@brief �Ƿ��ڻ��
	 */
	bool IsInActivity(const std::string& name) const;
	bool IsInActivity(Player* player, const std::string& name) const;
	bool IsInActivity(UInt32 activeId) const;
	bool IsInActivity(UInt32 activeId, UInt32 time) const;		//����ʱ���ж��Ƿ��ڻ�ڼ�
	bool IsInActivity(UInt32 activeId, Player* player) const;

	// �Ƿ����ϻ��ʼ
	bool IsJustInActivity(UInt32 activeId, Player* player) const;

	//��û��Ϣ
	bool GetActivityInfo(UInt32 activeId, ActivityInfo& info);

	/**
	*@brief ���ʿ��ŵĻ
	*/
	void VisitOpenActivities(Player* player, const std::function<bool(const ActivityInfo&)>& F);

	/**
	*@brief ��ȡ�ʱ��
	*/
	bool GetActivityTime(UInt32 activeId, UInt32& startTime, UInt32& endTime);

	/**
	 *@brief �Ƿ��ڲ�����
	 */
	bool IsInTribeActivity(ObjID_t uTribeId, const std::string& name) const;

	/**
	 *@brief ��ȡ���ʼʱ��                                                                     
	 */
	UInt32 GetActivityStartTime(const std::string& name) const;
	
	//��ȡ�����еĻ
	void GetOpenActivity(Player* player, std::vector<ActivityInfo>& activities);

	//֪ͨ��ҽ��ܻ����
	void NotifyPlayerAcceptActiveTask(UInt32 id);

	//֪ͨ���ɾ�������
	void NotifyPlayerRemoveActiveTask(UInt32 id);

	//֪ͨ��һ����
	void NotifyPlayerActiveClosed(UInt32 id);

	void OnTick(const Avalon::Time &now);
	
	/**
	*@brief ��Ӫ�
	*/

	//������Ӫ�����
	void OnLoadOpActData(OpActRegMap& activeData);

	//���ص�ǰ��Ӫ���Ϣ
	//void OnLoadOpActInfo(OpActivityVec& activeInfo);

	//ͬ����ǰ��Ӫ�״̬
	void OnOpActStateChange(OpActivityRegInfo& info);

	//�����
	void OnOpActOpen(OpActivityRegInfo* info);

	//�����(�ӽ����е�����)
	void OnOpActClosed(OpActivityRegInfo* info);

	//�����Ӫ�״̬BY��Ӫ�id
	//ActivityState	GetOpActState(UInt32 id);

	//������п��ŵ���Ӫ���Ϣ
	void GetOpenOpActs(std::vector<OpActivityRegInfo*>& opActs);

	//���������Ӫ���Ϣ
	void GetOpActs(std::vector<OpActivityRegInfo*>& opActs);

	//�����Ӫ���������BY����dataid
	OpActTaskData* GetOpActTaskData(UInt32 dataId);

	//���������Ӫ���������BYģ�����
	bool GetOpActTaskDataVec(UInt32 tmpType, std::vector<OpActTaskData*>& datas);

	//�����Ӫ�����BY dataId
	OpActivityRegInfo* GetOpActData(UInt32 dataId);

	//�����Ӫ�����BY tmpId
	void GetOpActDataByTmpType(OpActivityType type, std::vector<OpActivityRegInfo*>& activities);
	//�����Ӫ�����BY tmpId
	std::vector<OpActivityRegInfo*> GetOpActDataByTmpTypes(UInt32 tmpType, bool mustInActivity = true);

	//֪ͨ�����Ӫ�״̬�ı�
	void NotifyPlayerOpActChange(OpActivityRegInfo& info);

	//�Ƿ�����Ӫ�
	bool IsInOpActivity(UInt32 activityId);
	bool IsInOpActivity(UInt32 activityId, Player* player);
	bool IsInOpActivity(const OpActivityRegInfo* opAct, Player* player);

	//�Ƿ�����Ӫ�
	bool IsInOpActivityByTmpType(UInt32 tmpId, UInt16 playerLevel);

	//��ȡ��Ӫ��ӳ�ϵ��
	UInt32 GetOpActivityAddition(UInt32 tmpId, Player* player);

	void GetVeteranReturnOpActBuffs(Player* player, std::vector<Buff*>& veteranBuffs, const DungeonDataEntry* dungeonData);

	//��ȡ�����еĻid
	UInt32 GetOpenActivityIdByTmplateType(UInt32 actTmpType);
	OpActivityRegInfo* GetOpenActivityRegInfoByTmplateType(Player* player, OpActivityType actTmpType);

	// �����Ӫ����ݺϷ�
	bool CheckOpActivityDataValid(Player* player, UInt32 opActId, OpActivityType opActType);

	// ���ػ�˺�����
	void LoadActivityAccRecord(UInt32 accid, ActivityAccRecord* record);
	// �����˺Ż����
	ActivityAccRecord* FindAccActivityRecord(UInt32 accid, UInt32 actId);
	// �����˺Ż����
	void AddAccActivityRecord(UInt32 accid, ActivityAccRecord* record);
	// �����˺Ż����
	void UpdateAccActivityRecord(UInt32 accid, const ActivityAccRecord* record, bool isInsert = false);
	// ɾ�����ڵ��˺Ż��¼
	void DelExpireAccActivityRecord(UInt32 accid);

private:
	void _AddOpActData(OpActivityRegInfo& info);

public:
	//�
	ActivityVec m_Activities;

	//����
	TribeActivityMap	m_TribeActivities;

	//��Ӫ�����,tmptypeΪkey	
	std::map<UInt32, std::vector<OpActivityRegInfo*>>	m_tmpTypeToOpActDatas;
	//��Ӫ�����,dataIdΪkey	
	std::map<UInt32, OpActivityRegInfo*>				m_dataIdToOpActData;
	//��Ӫ�����,dataIdΪkey
	std::map<UInt32, OpActTaskData*>					m_dataIdToTaskDatas;
	//��Ӫ�����,tmptypeΪkey
	std::map<UInt32, std::vector<OpActTaskData*>>		m_tmpTypeToTaskDatas;

	// �˺���Ӫ���¼��key->�˺�(key->�id)
	std::map<UInt32, std::map<UInt32, ActivityAccRecord*>>	m_accActivityRecordMap;
};

#endif
