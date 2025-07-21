#ifndef _WS_ACTIVITY_MGR_H_
#define _WS_ACTIVITY_MGR_H_

#include <functional>
#include <AvalonSingleton.h>
#include <AvalonNetConnection.h>
#include <AvalonSimpleTimer.h>
#include <CLGameDefine.h>
#include <CLGlobalActivityServerDefine.h>
#include "CLItemDefine.h"
#include "CLActivityDefine.h"

class WSPlayer;
class CLRecordCallback;

/**
*@brief �ҳǩ��Ϣ
*/
struct ActivityTabInfo
{
	ActivityTabInfo() :id(0){}

	// ҳǩid
	UInt32 id;
	// ҳǩ��
	std::string name;
	// ҳǩ�µĻid
	std::vector<UInt32> actIds;
};

class WSActivityMgr : public Avalon::Singleton<WSActivityMgr>
{
	/**
	 *@brief �ע����Ϣ
	 */
	struct ActivityRegInfo
	{
		ActivityRegInfo(): type(0), level(0), startTime(0), preTime(0), lastTime(0), gamestartTime(0), gamestartFlag(0), cricleDays(0){}
		ActivityRegInfo(UInt8 _type, UInt16 _level, UInt32 _startTime, UInt32 _preTime, UInt32 _lastTime, UInt32 _cricleDays)
			: type(_type), level(_level), startTime(_startTime), preTime(_preTime), lastTime(_lastTime), gamestartTime(0), gamestartFlag(0), cricleDays(_cricleDays){}

		UInt32		id;
		//���ͣ�0 ÿ�ջ��1 һ���Ի
		UInt8		type;
		// ���
		std::string	name;
		//��Ҫ�ȼ�
		UInt16		level;
		//��ʼʱ��
		UInt32		startTime;
		//׼��ʱ��
		UInt32		preTime;
		//����ʱ��
		UInt32		lastTime;
		//����ʱ��Ҫ��
		UInt32		gamestartTime;
		//����ʱ��Ҫ���ǣ�0 �涨ʱ��֮ǰ�������л��1 �涨ʱ��֮�󿪷����л
		UInt8		gamestartFlag;
		//ѭ����������
		UInt32		cricleDays;
	};
	
	typedef std::vector<ActivityRegInfo>	ActivityRegVec;
	typedef std::map<UInt32, ActivityRegInfo>		ActivityRegMap;
	typedef std::set<ObjID_t>  RightPlayersSet;

	

public:
	WSActivityMgr();
	~WSActivityMgr();

	bool Init();

public:

	/**
	*@brief ����ʱ��Ļ
	*/
	void OpenServerTimeActivity(UInt32 firstOpenServerTime);

	/**
	*@brief ����ҳǩ��
	*/
	void LoadActivityTabTable();

	/**
	*@brief ��ӻҳǩ��Ϣ
	*/
	void AddActivityTabInfo(ActivityTabInfo* tabInfo);

	/**
	*@brief ���ʻҳǩ��Ϣ
	*/
	void VisitActivityTabInfo(const std::function<bool(ActivityTabInfo*)>& F);

	/**
	*@brief ���ҳǩ����
	*/
	void ClearActivityTabInfo();

	/**
	*@brief ��ӻҳǩ
	*/
	bool AddActivityToTab(UInt32 actId, UInt32 tabId);

	/**
	*@brief ���»ҳǩ
	*/
	bool UpdateActivityTab();

	//-------------------------��Ӫ�start------------------------------------------
public:
	//������Ӫ���������
	bool LoadOpActivityData();

	//���ر�����õ���Ӫ�
	bool LoadOpActivityFromDataTable(bool isReload = false);

	//���ر��
	void ReloadOpActivityFromDataTable();

	//������Ӫ�����
	void CreateOpActivity(CLRecordCallback* cb);

	// �����Ӫ�
	void AddOpActivity(OpActivityRegInfo& opAct);

	//��ѯ��Ӫ�����
	void QueryOpActData(std::map<UInt32,OpActivityRegInfo>& datas);
	//������Ӫ�����
	void UpdateOpActData(OpActivityRegInfo& info);
	//ɾ����Ӫ�����
	bool DeleteOpActData(UInt32 dataId);

	// ���ʻ
	void VisitOpActivity(const std::function<bool(const OpActivityRegInfo&)>& F);

	// �����Ӫ��������
	void OnCheckGmOpActLoadOver(UInt32 totalNum);

	// �Ƿ�������gm��Ӫ�
	bool IsGmDataLoadOver() { return m_GmDataLoadOver; }

	// �Զ������û����
	void CustomSetOpActData(UInt32 id, UInt32 startTime, UInt32 endTime);

	//GM�ӿڱ����
	bool GMDecodeRewardItems(std::vector<ItemReward>& items, std::string strItems);
	std::string GMEncodRewardItems(std::vector<ItemReward>& items);

	//�Ƿ��ڻ��
	bool IsInOpActivity(UInt32 activityId);

	//��ȡ��Ӫ�by activityId
	bool GetOpActivityById(UInt32 id, OpActivityRegInfo& opAct);

	//��ȡ�����ӳ�
	void GetActivityDropAddition(UInt32 dungeonId, DungeonAddition& addition);

	//������Ӫ�״̬
	void SyncOpActStateChange(OpActivityRegInfo& info);

	// ���ͬ���
	void GlobalActivitySync(const OpActivityInfoObjectVec& opActs);
	
	
private:

	//������Ӫ�����
	void TraversalOpRegActs(const Avalon::Time& now);

	//��Ӫ�״̬�ı��¼�
	void OnOpActStateChange(OpActivityRegInfo& info, ActivityState state);

	void OnTickOpAct(const Avalon::Time& now);

	//��Ӫ���������
	void OnOpActivityChangeOpen(const OpActivityRegInfo& info);
	void OnChangeOpenNewServerFighting(const OpActivityRegInfo& info);

	//��Ӫ��رմ���
	void OnOpActivityChangeClose(const OpActivityRegInfo& info);

	//-------------------------��Ӫ�end------------------------------------------

public:
	/**
	*@brief ���ע����Ϣ
	*/
	void ClearRegInfos();

	/**
	*@brief ע��ÿ�ջ
	*/
	void RegisterDailyActivity(UInt32 id, const std::string &name, UInt16 level, const std::string &startTime, UInt32 preTime, UInt32 lastTime);

	/**
	*@brief ����ÿ�ջ
	*/
	void UpdateDailyActivity(UInt32 id, UInt16 level, const std::string &startTime, UInt32 preTime, UInt32 lastTime);

	/**
	*@brief ע��һ���Ի
	*/
	void RegisterOnceActivity(UInt32 id, const std::string &name, UInt16 level, const std::string &startTime, UInt32 preTime, UInt32 lastTime);
	void RegisterOnceActivity(const std::string &name, UInt16 level, UInt32 startTime, UInt32 preTime, UInt32 lastTime);

	/**
	*@brief ע�Ὺ��һ���Ի(�����ÿ���ʱ��)
	*/
	void RegisterOpenServerOnceActivity(UInt32 id, const std::string &name, UInt16 level, const std::string &startTime, UInt32 lastTime, UInt32 openServerTime);

	/**
	*@biref ע��ÿ�ܻ
	*/
	void RegisterWeekActivity(UInt32 id, const std::string &name, UInt16 level, const std::string &startTime, UInt32 preTime, UInt32 lastTime);
	/**
	*@brief ����ÿ�ܻ
	*/
	void UpdateWeekActivity(UInt32 id, UInt16 level, const std::string &startTime, UInt32 preTime, UInt32 lastTime);

	/**
	*@biref ע��ÿ�»(ÿ��1��0�㿪ʼ������������)
	*/
	void RegisterMonthActivity(UInt32 id, const std::string &name, UInt16 level);

	/**
	*@brief ע�Ὺ���
	*/
	void RegisterGSActivity(UInt32 id, const std::string &name, UInt16 level, UInt32 lastTime);

	/**
	*@brief ע�Ὺ���
	*@param name:����� level:��ҵȼ� endTime:����ʱ�䣨��ʽ������|Сʱ:����:�룩
	*/
	void RegisterGSActivity(UInt32 id, const std::string &name, UInt16 level, const std::string& endTime);

	/**
	*@brief ע�Ὺ���
	*@param name:����� level:��ҵȼ� fromTime:�����ڼ���ĳ��ʱ��㿪ʼ toTime:�����ڼ���ĳ��ʱ������ ����ʽ������|Сʱ:����:�룩
	*/
	void RegisterGSActivity(UInt32 id, const std::string &name, UInt16 level, const std::string& fromTime, const std::string& toTime);

	/**
	*@brief ע�Ὺ���
	*@param name:����� level:��ҵȼ� fromDay:�����ڼ��쿪��ʱ��㿪ʼ toDay:�����ڼ��쿪��ʱ������
	*/
	void RegisterGSActivity(UInt32 id, const std::string &name, UInt16 level, UInt16 fromDay, UInt16 toDay);

	/**
	*@brief ע�����û
	*/
	void RegisterConsistActivity(const std::string &name, UInt16 level);
	void RegisterConsistActivity(UInt32 id, const std::string &name, UInt16 level);

	/**
	*@brief ע���Զ��嶨ʱ�
	*/
	void RegisterCustomCircleActivity(UInt32 id, const std::string &name, UInt16 level, const std::string startTime, UInt32 preTime, const UInt32 lastTime);

	/**
	*@brief ���û�Ŀ���ʱ��Ҫ��
	*/
	void SetGameStartLimit(UInt32 id, const std::string &gamestartTime, UInt8 gamestartFlag);

	/**
	*@brief ��ӿ�����Ϣ
	*/
	void AddGlobalActivity(const char *szName);

	/**
	*@brief ���ʻ
	*/
	void VisitActivity(const std::function<bool(const ActivityInfo&)>& F);
	

	/**
	*@brief ���һ
	*/
	bool FindActivity(UInt32 actId, ActivityInfo& actInfo);

	/**
	 *@brief ����һ����㲥ȫ�����
	 */
	bool OpenActivity(UInt32 id, const std::string& name, UInt16 level, UInt8 state, UInt32 preTime, UInt32 startTime, UInt32 dueTime);

	// �޸Ļ��һ�����
	void NotifyActivityStateChange(WSPlayer* player, Int32 id, const std::string& name, UInt16 level, UInt8 state, UInt32 preTime, UInt32 startTime, UInt32 dueTime);
	/**
	 *@brief �ر�һ���
	 */
	void CloseActivity(const std::string& name);

	/**
	 *@brief �Ƿ��ڻ��
	 */
	bool IsInActivity(const std::string& name) const;
	bool IsInActivity(WSPlayer* player, const std::string& name) const;
	bool IsPreActivity(const std::string& name) const;
	bool IsPreActivity(WSPlayer* player, const std::string& name) const;

	/**
	*@brief �Ƿ��ڻ��
	*/
	bool IsEndActivity(const std::string& name) const;

	/**
	*@brief ��ȡ���ʼ����ʱ��
	*/
	UInt32 GetActivityPreTime(const std::string& name) const;
	UInt32 GetActivityStartTime(const std::string& name) const;
	UInt32 GetActivityDueTime(const std::string& name) const;

	//ע��ϵͳ���������¼�
	void RegSysNotifyCount();

	//��ȡ��ǰ���ŵĻ
	void GetOpenActivity(WSPlayer* player, std::vector<ActivityInfo>& infos);

	//������鿪�ŵû�Ƿ���������
	void OnPlayerLevelUp(WSPlayer* player, UInt16 oldLv);

	//�̳�ϵͳ�ӿ�
	UInt32 GetMallSale(OpActivityType type);

	/**
	*@brief ��ȡ��ǰ���ŵĻ
	*/
	ActivityVec& GetOpenActivitys() { return m_Activities; }

	/**
	*@brief ��Ƿ񿪷�
	*/
	bool IsInActivity(UInt16 playerLevel, UInt32 activityId);

	/**
	*@brief ��ȡ����ŵȼ�
	*/
	UInt16 GetActivityNeedLevel(UInt32 activityId);

	/**
	*@brief �Ƿ����˺ŵĻ
	*/
	bool IsAccountActivity(UInt32 activityId);

	/**
	*@brief ��ȡ��ɫ�󶨵Ļ
	*/
	void GetRoleActivity(std::vector<ActivityInfo>& activities);

	/**
	*@brief ��ȡ�˺Ű󶨵Ļ
	*/
	void GetAccountActivity(std::vector<ActivityInfo>& activities, WSPlayer* player);

	ActivityRegInfo*  FindActivityRegInfo(UInt32 actId);
	ActivityInfo* FindActivityInfo(std::string& actName);
	ActivityRegInfo*  FindNearestActivityRegInfo(std::string& actName);
public:

	/**
	 *@brief �¼�
	 */
	void OnTick(const Avalon::Time& now);

	/**
	 *@brief �������
	 */
	void OnPlayerOnline(WSPlayer* player);

	/**
	 *@brief ����������
	 */
	void OnSceneConnected(UInt32 nodeId);

	/**
	*@brief Gate������
	*/
	void OnGateConnected(UInt32 nodeId);

	/**
	*@brief ���ʼ
	*/
	void OnActivityStart(const std::string &name);

	/**
	 *@brief �����
	 */
	void OnActivityEnd(const std::string& name);
	void OnActivityEnd(ActivityInfo &activity);
	/**
	*@brief �����
	*/
	void OnActivityPrepare(const std::string& name);

	//����
	void OnDayChanged(WSPlayer* player);

public: //gm
	void GmOpenActive(UInt32 id, UInt8 state);

private:
	/**
	 *@brief ֪ͨ�״̬�ı�
	 */
	void NotifyActivityStateChange(ActivityInfo &activity, UInt8 state);

	//��Ӫ���������
	bool OpTaskEncode(const std::vector<OpActTaskData>& tasks, std::string& taskStr, std::string& rewardStr);
	bool OpTaskDecode(std::vector<OpActTaskData>& tasks, const std::string& taskStr, const std::string& rewardStr);

	void _SendOpActivitiesToServer(Avalon::NetConnectionPtr conn);
	void _BrocastOpActivitiesToServers();

	/**
	*@brief �㲥�״̬�ı䵽���
	*/
	void _BrocastActivityStateChangeToPlayers(Int32 id, const std::string& name, UInt8 state, UInt16 level, UInt32 preTime, UInt32 startTime, UInt32 dueTime);

	//֪ͨBattleServer�ʱ��
	void _NotifyBattleActivityTime(const std::string& actName);

	//֪ͨUnionServer�ʱ��
	void _NotifyUnionActivityTime(UInt32 unionFunType, const std::string& actName);
public:
	//�ע����Ϣ
	ActivityRegMap	m_mapRegInfos;
	//��б�
	ActivityVec		m_Activities;

	//����ʱ��Ļע����Ϣ
	ActivityRegMap	m_OpenServerTimeRegInfos;

	// �������Ʊ�
	std::map<std::string, std::string>	m_mapGlobalActivity;

	//��Ӫע��
	OpActRegMap	m_opRegInfos;
	//����ʱ��ʱ�
	OpActRegMap m_TmpOpRegInfos;
	
	// gm��Ӫ��������
	bool m_GmDataLoadOver;

	// �ҳǩ
	std::vector<ActivityTabInfo*> m_ActivityTabsInfo;

	// Tick��ʱ����10s
	Avalon::SimpleTimer				m_TickTimer10s;
};

#endif
