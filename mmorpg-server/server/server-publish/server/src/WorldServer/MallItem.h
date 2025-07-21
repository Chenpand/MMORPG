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
	*@brief �Ƿ��ϼ�
	*/
	bool IsOnSale(UInt32 limitTime);

	/**
	*@brief �������ʱ��
	*/
	void CalcEndTime(const std::string& interval);

	/**
	*@brief ת����Ʒ
	*/
	void TransformMallItem(MallItemInfo& mallItem);

	/*
	*@brief ��־
	*/
	const std::string LogStr();

	/*
	*@brief ���浽���ݿ�
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
	// ��Ʒid
	ObjUInt32 m_MallItemId;
	// ��ɫid
	ObjUInt64 m_RoleId;
	// ����ʱ��(����)
	ObjUInt64 m_CreateTime;
	// ��ʼʱ��(����)
	ObjUInt64 m_StartTime;
	// ����ʱ��(����)
	ObjUInt64 m_EndTime;
};

#endif