#ifndef _ACTIVITY_ATTRIBUTE_H_
#define _ACTIVITY_ATTRIBUTE_H_

#include <CLObjectProperty.h>
#include <CLSyncObject.h>
#include <TimeUtil.h>

class Player;


class ActivityVarMap : public CLComplexProperty
{
public:
	ActivityVarMap() {}
	~ActivityVarMap() {}

public:
	Int64 GetVar(const std::string& key) const;

	void SetVar(const std::string& key, const std::string& value);

public:
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_StringVars;
	}

	virtual void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty);
	virtual void Decode(Avalon::NetInputStream& stream);

	virtual void DecodeString(std::istringstream& is);
	virtual void EncodeString(std::ostringstream& os);

public:
	const std::string LogStr();

private:
	std::map<std::string, std::string> m_StringVars;
};

class OpActivityAttribute : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(OpActivityAttribute, Avalon::NullMutex)

public:
	OpActivityAttribute();
	~OpActivityAttribute() {}

public:
	void SetOpActId(UInt32 id) { m_OpActId = id; }
	UInt32 GetOpActId() const { return m_OpActId; }

	void SetOwner(ObjID_t roleid) { m_Owner = roleid; }
	ObjID_t GetOwner() const { return m_Owner; }

	void SetVar(Player* player, const std::string& key, Int64 value);
	void SetVarNoSync(const std::string& key, Int64 value);

	Int64 GetVar(const std::string& key) const;

	void IncVar(Player* player, const std::string& key, Int64 incValue);
	void IncVarNoSync(const std::string& key, Int64 incValue);

	void SyncVar(Player* player, const std::string key, const std::string& value);

	void SetCreateTime(UInt64 time) { m_CreateTime = time; }

	void SetRefreshTime(UInt64 time) { m_RefreshTime = time; }
	UInt64 GetRefreshTime() const { return m_RefreshTime; }

public:
	/*
	*@brief 自定义刷新
	*/
	bool TryRefresh(TimeUtil::RefreshType refreshType, TimeUtil::RefreshDate refreshDate);

public:
	/*
	*@brief 日志
	*/
	const std::string LogStr();

	/*
	*@brief 保存到数据库
	*/
	void SaveToDB();

public:
	CL_DBPROPERTY_BEGIN(CLSyncObject)
		CL_DBPROPERTY("activity_id", m_OpActId)
		CL_DBPROPERTY("owner", m_Owner)
		CL_DBPROPERTY("parameter", m_VarMap)
		CL_DBPROPERTY("create_time", m_CreateTime)
		CL_DBPROPERTY("refresh_time", m_RefreshTime)

		CL_DBPROPERTY_END()

private:
	// 运营活动id
	ObjUInt32 m_OpActId;
	// 拥有者(角色id)
	ObjUInt64 m_Owner;
	// 变量集
	ActivityVarMap m_VarMap;
	// 创建时间
	ObjUInt64 m_CreateTime;
	// 刷新时间
	ObjUInt64 m_RefreshTime;
};




#endif