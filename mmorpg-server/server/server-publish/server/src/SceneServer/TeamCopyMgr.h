#ifndef _TEAM_COPY_MGR_H_
#define _TEAM_COPY_MGR_H_

#include <unordered_map>
#include <CLDefine.h>
#include <CLSyncObject.h>
#include <CLTeamCopyDefine.h>
#include <CLObjectProperty.h>

class Player;
class CLRecordCallback;

namespace CLProtocol{
	class TeamCopyPlayerInfoSync;
};

struct TeamCopySelfData : public CLSyncObject
{
	TeamCopySelfData() {}

	ObjUInt64 owner;
	// ����
	ObjUInt32 teamType;
	// ����ս����
	ObjString dayNumStr;
	// ���˶Ӵ���
	ObjUInt32 dayQuitTeamNum;
	// ��ʱ��
	ObjUInt64 dayTime;
	// ����ս����
	ObjString weekNumStr;
	// ���˶Ӵ���
	ObjUInt32 weekQuitTeamNum;
	// ��ʱ��
	ObjUInt64 weekTime;
	// ģʽͨ�ش���
	ObjString modelPassStr;
	// �շ��Ƽ�¼
	ObjString dayFlopStr;
	// �ܷ��Ƽ�¼
	ObjString weekFlopStr;
	// ʹ���ű����ߴ���
	ObjString extraNumStr;
	// roll��������
	ObjString rollNumStr;

	void ChangeDay(const Avalon::Time& now);
	void ChangeWeek(const Avalon::Time& now);
	std::string LogStr();

	//���ݿ����
	void UpdateToDB(bool bFlush = true);
	void InsertToDB();

	CL_DBPROPERTY_BEGIN(CLSyncObject)
	CL_DBPROPERTY("owner", owner)
	CL_DBPROPERTY("team_type", teamType)
	CL_DBPROPERTY("day_num", dayNumStr)
	CL_DBPROPERTY("day_quit_team_num", dayQuitTeamNum)
	CL_DBPROPERTY("day_time", dayTime)
	CL_DBPROPERTY("week_num", weekNumStr)
	CL_DBPROPERTY("week_quit_team_num", weekQuitTeamNum)
	CL_DBPROPERTY("week_time", weekTime)
	CL_DBPROPERTY("model_pass", modelPassStr)
	CL_DBPROPERTY("day_flop", dayFlopStr)
	CL_DBPROPERTY("week_flop", weekFlopStr)
	CL_DBPROPERTY("extra_num", extraNumStr)
	CL_DBPROPERTY("roll_num", rollNumStr)
	CL_DBPROPERTY_END()
};

/**
*brief �ű�����
*/
class TeamCopyMgr
{
public:
	TeamCopyMgr();
	~TeamCopyMgr();

	void SetOwner(Player* player) { m_owner = player; }
	Player* GetOwner() { return m_owner; }

	/**
	*brief �Ƿ����ű�
	*/
	inline bool IsTeamCopy() { return m_IsTeamCopyFlag; }
	inline void SetTeamCopy(bool flag) { m_IsTeamCopyFlag = flag; }

	/**
	*brief init
	*/
	void OnInit(const Avalon::Time& now);

	/**
	*brief tick
	*/
	void OnTick(const Avalon::Time& now);

	/**
	*brief ����
	*/
	void LoadData(CLRecordCallback* callback);

	/**
	*brief ����ս����
	*/
	UInt32 GetWeekNum(UInt32 teamType, UInt32 grade);

	/**
	*brief �Ѷ�ͨ�ش���
	*/
	UInt32 PassGradeNum(UInt32 grade, const std::string& numStr);

	/**
	*brief ʹ���ű�����
	*/
	void UseTeamCopyItem(UInt32 teamType, UInt32 grade, UInt32 num);

	/**
	*brief �ű����ݷ���
	*/
	void HandlerTeamCopyPlayerInfo(CLProtocol::TeamCopyPlayerInfoSync& sync);

	/**
	*brief �л��ű�����
	*/
	bool ChangeTeamCopyScene(UInt32 destSceneId);

	/**
	*brief �˳��ű�����
	*/
	void QuitTeamCopyScene();

	/**
	*brief �Ƿ����ű�����
	*/
	bool IsTeamCopyScene();

	/**
	*brief �����ű�����
	*/
	void ReconnectTeamCopy();

	/**
	*brief �ű��������롢�뿪֪ͨ
	*/
	void NotifyTeamCopySceneEnter();
	void NotifyTeamCopySceneLeave();

	/**
	*brief �ű����³Ǹ���
	*/
	void TeamCopyDungeonRevive(UInt64 targetId, UInt32 reviveId, UInt32 reviveCoinNum);

	/**
	*brief �ű���Ϣͬ��
	*/
	void PlayerInfoToTeamCopy(UInt32 teamType);

	/**
	*brief ��Ʊͬ��
	*/
	void TicketAlterToTeamCopy(UInt32 itemId);

	/**
	*brief װ������ͬ��
	*/
	void EquipScoreToTeamCopy();

	/**
	*brief Loading
	*/
	void NotifyTeamCopyPlayerLoading(UInt32 isLoading);

	/**
	*brief ͨ�ص��³��¼�
	*return param1:�ű��Ѷȣ�param2:ͨ������ʱ�䣬param3:�ű�����
	*/
	void ClearDungeonEvent(ObjID_t param1, ObjID_t param2, ObjID_t param3);

private:

	/**
	*brief ����ʧ��
	*/
	void _SceneFailed();

	/**
	*brief ����
	*/
	bool _IsOpen();
	bool _IsOpen(UInt32 teamType);

	/**
	*brief ��Ʊͬ��
	*/
	void _TicketToTeamCopy();

	/**
	*brief ��Ʊ�Ƿ��㹻
	*/
	bool _TicketIsEnough(UInt32 ticket1, UInt32 ticketNum1, UInt32 ticket2, UInt32 ticketNum2);

	/**
	*brief ˢ���ű�����ʹ�ô���
	*/
	void _RefreshTeamCopyItem(UInt32 teamType);

private:

	Player* m_owner;
	// �Ƿ����ű�
	bool m_IsTeamCopyFlag;
	// �ű�����
	std::unordered_map<UInt32, TeamCopySelfData> m_SelfDataMap;
};

#endif // !_TEAM_COPY_MGR_H_
