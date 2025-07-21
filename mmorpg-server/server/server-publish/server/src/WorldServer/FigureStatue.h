#ifndef _FIGURE_STATUE_H_
#define _FIGURE_STATUE_H_

#include <CLSyncObject.h>
#include <CLObjectProperty.h>


//人物雕像
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
	*@brief 设置获取账号id
	*/
	void SetAccID(UInt32 accId) { m_AccId = accId; }
	UInt32 GetAccID() { return m_AccId; }

	/**
	*@brief 设置获取角色id
	*/
	void SetRoleID(UInt64 roleId) { m_RoleId = roleId; }
	UInt64 GetRoleID() { return m_RoleId; }

	/**
	*@brief 设置获取玩家名字
	*/
	void SetRoleName(const std::string& name) { m_Name = name; }
	const std::string& GetRoleName() { return m_Name; }

	/**
	*@brief 设置获取玩家职业
	*/
	void SetOccu(UInt8 occu) { m_Occu = occu; }
	UInt8 GetOccu() { return m_Occu; }

	/**
	*@brief 设置获取玩家外观
	*/
	void SetAvatar(const PlayerAvatar& avatar) { m_Avatar = avatar; }
	const PlayerAvatar& GetAvatar() { return m_Avatar; }

	/**
	*@brief 设置获取人物雕像类型
	*/
	void SetStatueType(UInt8 statueType) { m_StatueType = statueType; }
	UInt8 GetStatueType() { return m_StatueType; }

	/**
	*@brief 设置获取公会名字
	*/
	void SetGuildName(const std::string& guildName) { m_GuildName = guildName; }
	const std::string& GetGuildName() { return m_GuildName; }
	
	/**
	*@brief 设置获取公会id
	*/
	void SetGuildId(UInt64 guildId) { m_GuildId = guildId; }
	UInt64 GetGuildId() { return m_GuildId; }

public:
	//数据库相关
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
	// 账号id
	ObjUInt32		m_AccId;
	// 角色id
	ObjUInt64		m_RoleId;
	// 玩家名字
	ObjString		m_Name;
	// 玩家职业
	ObjUInt8		m_Occu;
	// 玩家外观
	ObjPlayerAvatar	m_Avatar;
	// 人物雕像类型
	ObjUInt8		m_StatueType;
	// 公会名字
	ObjString		m_GuildName;
	// 公会ID
	ObjUInt64		m_GuildId;
};

#endif