#ifndef _FIGURE_STATUE_H_
#define _FIGURE_STATUE_H_

#include <CLSyncObject.h>
#include <CLObjectProperty.h>


//�������
class FigureStatue : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(FigureStatue, Avalon::NullMutex);

public:
	FigureStatue();
	~FigureStatue();

	static FigureStatue* Create();
	static void Destroy(FigureStatue*& ptr);

public:
	/**
	*@brief ���û�ȡ�˺�id
	*/
	void SetAccID(UInt32 accId) { m_AccId = accId; }
	UInt32 GetAccID() { return m_AccId; }

	/**
	*@brief ���û�ȡ��ɫid
	*/
	void SetRoleID(UInt64 roleId) { m_RoleId = roleId; }
	UInt64 GetRoleID() { return m_RoleId; }

	/**
	*@brief ���û�ȡ�������
	*/
	void SetRoleName(const std::string& name) { m_Name = name; }
	const std::string& GetRoleName() { return m_Name; }

	/**
	*@brief ���û�ȡ���ְҵ
	*/
	void SetOccu(UInt8 occu) { m_Occu = occu; }
	UInt8 GetOccu() { return m_Occu; }

	/**
	*@brief ���û�ȡ������
	*/
	void SetAvatar(const PlayerAvatar& avatar) { m_Avatar = avatar; }
	const PlayerAvatar& GetAvatar() { return m_Avatar; }

	/**
	*@brief ���û�ȡ�����������
	*/
	void SetStatueType(UInt8 statueType) { m_StatueType = statueType; }
	UInt8 GetStatueType() { return m_StatueType; }

	/**
	*@brief ���û�ȡ��������
	*/
	void SetGuildName(const std::string& guildName) { m_GuildName = guildName; }
	const std::string& GetGuildName() { return m_GuildName; }
	
	/**
	*@brief ���û�ȡ����id
	*/
	void SetGuildId(UInt64 guildId) { m_GuildId = guildId; }
	UInt64 GetGuildId() { return m_GuildId; }

public:
	//���ݿ����
	void UpdateToDB(bool bFlush = true);
	void InsertToDB();

	CL_DBPROPERTY_BEGIN(CLSyncObject)
		CL_DBPROPERTY("accid", m_AccId)
		CL_DBPROPERTY("roleid", m_RoleId)
		CL_DBPROPERTY("name", m_Name)
		CL_DBPROPERTY("occu", m_Occu)
		CL_DBPROPERTY("avatar", m_Avatar)
		CL_DBPROPERTY("statuetype", m_StatueType)
		CL_DBPROPERTY("guildname", m_GuildName)
		CL_DBPROPERTY("guildid", m_GuildId)

	CL_DBPROPERTY_END()

private:
	// �˺�id
	ObjUInt32		m_AccId;
	// ��ɫid
	ObjUInt64		m_RoleId;
	// �������
	ObjString		m_Name;
	// ���ְҵ
	ObjUInt8		m_Occu;
	// ������
	ObjPlayerAvatar	m_Avatar;
	// �����������
	ObjUInt8		m_StatueType;
	// ��������
	ObjString		m_GuildName;
	// ����ID
	ObjUInt64		m_GuildId;
};

#endif