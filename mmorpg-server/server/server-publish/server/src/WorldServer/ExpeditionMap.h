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
	*@brief 检查远征成员到期
	*/
	bool CheckExpeditionMemberExpire(UInt32 checkTime);

	/*
	*@brief 完成远征
	*/
	void FinishExpedition();

public:
	/*
	*@brief 日志
	*/
	const std::string LogStr();

	/*
	*@brief 保存到数据库
	*/
	void SaveToDB();

	/*
	*@brief 从数据库删除
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
	// 账号guid
	ObjUInt64 m_AccGUID;
	// 账号id
	ObjUInt32 m_AccId;
	// 远征地图id
	ObjUInt8 m_ExpeditionMapId;
	// 远征开始时间
	ObjUInt32 m_ExpeditionStartTime;
	// 远征持续时间(小时)
	ObjUInt32 m_ExpeditionDuration;
	// 远征状态(ExpeditionStatus)
	ObjUInt8 m_ExpeditionStatus;
};

#endif