#ifndef __MALL_ITEM_H__
#define __MALL_ITEM_H__

#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <CLMallDefine.h>


class WSPlayer;

class MallItem : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(MallItem, Avalon::NullMutex)

public:
	MallItem();
	~MallItem();

public:
	void SetMallItemId(UInt32 id) { m_MallItemId = id; }
	UInt32 GetMallItemId() const { return m_MallItemId; }

	void SetRoleId(UInt64 accid) { m_RoleId = accid; }
	UInt64 GetRoleId() const { return m_RoleId; }

	void SetCreateTime(UInt64 time) { m_CreateTime = time; }
	UInt64 GetCreateTime() const { return m_CreateTime; }

	void SetStartTime(UInt64 time) { m_StartTime = time; }
	UInt64 GetStartTime() const { return m_StartTime; }

	void SetEndTime(UInt64 time) { m_EndTime = time; }
	UInt64 GetEndTime() const { return m_EndTime; }

public:
	/**
	*@brief 是否上架
	*/
	bool IsOnSale(UInt32 limitTime);

	/**
	*@brief 计算结束时间
	*/
	void CalcEndTime(const std::string& interval);

	/**
	*@brief 转换商品
	*/
	void TransformMallItem(MallItemInfo& mallItem);

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
		CL_DBPROPERTY("mall_itemid", m_MallItemId)
		CL_DBPROPERTY("roleid", m_RoleId)
		CL_DBPROPERTY("create_time", m_CreateTime)
		CL_DBPROPERTY("start_time", m_StartTime)
		CL_DBPROPERTY("end_time", m_EndTime)

		CL_DBPROPERTY_END()

private:
	// 商品id
	ObjUInt32 m_MallItemId;
	// 角色id
	ObjUInt64 m_RoleId;
	// 创建时间(毫秒)
	ObjUInt64 m_CreateTime;
	// 开始时间(毫秒)
	ObjUInt64 m_StartTime;
	// 结束时间(毫秒)
	ObjUInt64 m_EndTime;
};

#endif