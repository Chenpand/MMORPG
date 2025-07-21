#ifndef _EXPEDITION_MAP_H_
#define _EXPEDITION_MAP_H_

#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <CLAdventureTeamDefine.h>

class ExpeditionMember;


class ExpeditionMap : public CLSyncObject
{
public:
	ExpeditionMap();
	~ExpeditionMap();

	static void DeleteExpeditionMap(ExpeditionMap*& p);

public:
	void SetAccGUID(ObjID_t guid) { m_AccGUID = guid; }

	UInt32 GetAccId() const { return m_AccId; }
	void SetAccId(UInt32 accid) { m_AccId = accid; }

	void SetExpeditionMapId(UInt8 mapId) { m_ExpeditionMapId = mapId; }
	UInt8 GetExpeditionMapId() const { return m_ExpeditionMapId; }

	void SetExpeditionStatus(ExpeditionStatus status) { m_ExpeditionStatus = (UInt8)status; }
	ExpeditionStatus GetExpeditionStatus() const { return (ExpeditionStatus)m_ExpeditionStatus.GetDBValue(); }

	void SetExpeditionStartTime(UInt32 time) { m_ExpeditionStartTime = time; }

	void SetExpeditionDuration(UInt32 duration) { m_ExpeditionDuration = duration; }
	UInt32 GetExpeditionDuration() const { return m_ExpeditionDuration; }

	UInt32 GetExpeditionEndTime() const { return m_ExpeditionStartTime.GetDBValue() + m_ExpeditionDuration.GetDBValue() * HOUR_TO_SEC; }

public:
	/*
	*@brief ���Զ����Ա����
	*/
	bool CheckExpeditionMemberExpire(UInt32 checkTime);

	/*
	*@brief ���Զ��
	*/
	void FinishExpedition();

public:
	/*
	*@brief ��־
	*/
	const std::string LogStr();

	/*
	*@brief ���浽���ݿ�
	*/
	void SaveToDB();

	/*
	*@brief �����ݿ�ɾ��
	*/
	void DeleteFromDB();

public:
	CL_DBPROPERTY_BEGIN(CLSyncObject)
		CL_DBPROPERTY("acc_guid", m_AccGUID);
		CL_DBPROPERTY("accid", m_AccId)
		CL_DBPROPERTY("mapid", m_ExpeditionMapId)
		CL_DBPROPERTY("start_time", m_ExpeditionStartTime)
		CL_DBPROPERTY("duration", m_ExpeditionDuration)
		CL_DBPROPERTY("status", m_ExpeditionStatus)

		CL_DBPROPERTY_END()

private:
	// �˺�guid
	ObjUInt64 m_AccGUID;
	// �˺�id
	ObjUInt32 m_AccId;
	// Զ����ͼid
	ObjUInt8 m_ExpeditionMapId;
	// Զ����ʼʱ��
	ObjUInt32 m_ExpeditionStartTime;
	// Զ������ʱ��(Сʱ)
	ObjUInt32 m_ExpeditionDuration;
	// Զ��״̬(ExpeditionStatus)
	ObjUInt8 m_ExpeditionStatus;
};

#endif