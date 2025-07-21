#ifndef _EXPEDITION_MEMBER_H_
#define _EXPEDITION_MEMBER_H_

#include <CLSyncObject.h>
#include <CLObjectProperty.h>


class ExpeditionMember : public CLSyncObject
{
public:
	ExpeditionMember();
	~ExpeditionMember() {}

	static void DeleteExpeditionMember(ExpeditionMember*& p);

public:
	void SetAccGUID(ObjID_t guid) { m_AccGUID = guid; }

	void SetAccId(UInt32 accid) { m_AccId = accid; }
	UInt32 GetAccId() const { return m_AccId; }

	void SetRoleId(UInt64 roleid) { m_RoleId = roleid; }
	UInt64 GetRoleId() const { return m_RoleId; }

	void SetExpeditionMapId(UInt8 mapid) { m_ExpeditionMapId = mapid; }
	UInt8 GetExpeditionMapId() const { return m_ExpeditionMapId; }

	void SetRoleName(const std::string& name) { m_RoleName = name; }
	const std::string GetRoleName() const { return m_RoleName; }

	void SetRoleLevel(UInt16 level) { m_RoleLevel = level; }
	UInt16 GetRoleLevel() const { return m_RoleLevel; }

	void SetOccu(UInt8 occu) { m_Occu = occu; }
	UInt8 GetOccu() const { return m_Occu; }

	void SetAvatar(const PlayerAvatar& avatar) { m_Avatar = avatar; }
	const PlayerAvatar& GetAvatar() const { return m_Avatar; }

	UInt32 GetCreateTime() const { return m_CreateTime; }

public:
	/*
	*@brief ���Զ����Ա����
	*/
	bool CheckExpeditionMemberExpire();

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
		CL_DBPROPERTY("roleid", m_RoleId)
		CL_DBPROPERTY("mapid", m_ExpeditionMapId)
		CL_DBPROPERTY("role_name", m_RoleName)
		CL_DBPROPERTY("role_level", m_RoleLevel)
		CL_DBPROPERTY("occu", m_Occu)
		CL_DBPROPERTY("avatar", m_Avatar)
		CL_DBPROPERTY("create_time", m_CreateTime)

		CL_DBPROPERTY_END()

private:
	// �˺�guid
	ObjUInt64 m_AccGUID;
	// �˺�id
	ObjUInt32 m_AccId;
	// ��ɫid
	ObjUInt64 m_RoleId;
	// Զ����ͼid
	ObjUInt8 m_ExpeditionMapId;
	// ��ɫ��
	ObjString m_RoleName;
	// ��ɫ�ȼ�
	ObjUInt16 m_RoleLevel;
	// ְҵ
	ObjUInt8 m_Occu;
	// ���
	ObjPlayerAvatar	m_Avatar;
	// ����ʱ��
	ObjUInt32 m_CreateTime;
};

#endif