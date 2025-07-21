#ifndef __ACCOUNT_CHARGE_ELEM_H__
#define __ACCOUNT_CHARGE_ELEM_H__

#include <CLDefine.h>
#include <sstream>
#include <CLSceneObject.h>
#include <CLObjectProperty.h>

// ��ֵ��¼
class AccountChargeElem : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(AccountChargeElem, Avalon::NullMutex)
public:
	AccountChargeElem();
	
public:
	/**
	*@brief �̳�����(����ö��ChargeMallType)
	*/
	UInt8 GetMallType() const { return m_MallType; }
	void SetMallType(UInt8 type) { m_MallType = type; }

	/**
	*@brief ��ƷID
	*/
	UInt32 GetGoodsID() const { return m_GoodsId; }
	void SetGoodsID(UInt32 id) { m_GoodsId = id; }

	/**
	*@brief �˺�ID
	*/
	UInt32 GetAccID() const { return m_Accid; }
	void SetAccID(UInt32 id) { m_Accid = id; }

	/**
	*@brief ��ɫID
	*/
	ObjID_t GetRoleID() const { return m_RoleId; }
	void SetRoleID(ObjID_t id) { m_RoleId = id; }

	/**
	*@brief ���մ���
	*/
	UInt32 GetTodayTimes();
	void SetTodayTimes(UInt32 times) { m_TodayTimes = times; }

	/**
	*@brief �ܴ���
	*/
	UInt32 GetTotalTimes() const { return m_TotalTimes; }
	void SetTotalTimes(UInt32 times) { m_TotalTimes = times; }

	/**
	*@brief ��һ�ι���ʱ��
	*/
	UInt32 GetUpdateTime() const { return m_UpdateTime; }
	void SetUpdateTime(UInt32 time) { m_UpdateTime = time; }

public:
	/* ���ݿ���� */
	void SaveToDB();
	void InsertToDB();

	CL_DBPROPERTY_BEGIN(CLSyncObject)
		CL_DBPROPERTY("mall_type", m_MallType)
		CL_DBPROPERTY("goodsid", m_GoodsId)
		CL_DBPROPERTY("accid", m_Accid)
		CL_DBPROPERTY("roleid", m_RoleId)
		CL_DBPROPERTY("today_times", m_TodayTimes)
		CL_DBPROPERTY("total_times", m_TotalTimes)
		CL_DBPROPERTY("update_time", m_UpdateTime)
	CL_DBPROPERTY_END()

private:
	// �̳�����
	ObjUInt8	m_MallType;
	// ��ƷID
	ObjUInt32	m_GoodsId;
	// �˺�ID
	ObjUInt32	m_Accid;
	// ��ɫID
	ObjUInt64	m_RoleId;
	// ���մ���
	ObjUInt32	m_TodayTimes;
	// �ܴ���
	ObjUInt32	m_TotalTimes;
	// ����ʱ��
	ObjUInt32	m_UpdateTime;
};

#endif