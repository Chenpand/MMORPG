#ifndef __ACCOUNT_CHARGE_ELEM_H__
#define __ACCOUNT_CHARGE_ELEM_H__

#include <CLDefine.h>
#include <sstream>
#include <CLSceneObject.h>
#include <CLObjectProperty.h>

// 充值记录
class AccountChargeElem : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(AccountChargeElem, Avalon::NullMutex)
public:
	AccountChargeElem();
	
public:
	/**
	*@brief 商城类型(对面枚举ChargeMallType)
	*/
	UInt8 GetMallType() const { return m_MallType; }
	void SetMallType(UInt8 type) { m_MallType = type; }

	/**
	*@brief 商品ID
	*/
	UInt32 GetGoodsID() const { return m_GoodsId; }
	void SetGoodsID(UInt32 id) { m_GoodsId = id; }

	/**
	*@brief 账号ID
	*/
	UInt32 GetAccID() const { return m_Accid; }
	void SetAccID(UInt32 id) { m_Accid = id; }

	/**
	*@brief 角色ID
	*/
	ObjID_t GetRoleID() const { return m_RoleId; }
	void SetRoleID(ObjID_t id) { m_RoleId = id; }

	/**
	*@brief 今日次数
	*/
	UInt32 GetTodayTimes();
	void SetTodayTimes(UInt32 times) { m_TodayTimes = times; }

	/**
	*@brief 总次数
	*/
	UInt32 GetTotalTimes() const { return m_TotalTimes; }
	void SetTotalTimes(UInt32 times) { m_TotalTimes = times; }

	/**
	*@brief 上一次购买时间
	*/
	UInt32 GetUpdateTime() const { return m_UpdateTime; }
	void SetUpdateTime(UInt32 time) { m_UpdateTime = time; }

public:
	/* 数据库相关 */
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
	// 商城类型
	ObjUInt8	m_MallType;
	// 商品ID
	ObjUInt32	m_GoodsId;
	// 账号ID
	ObjUInt32	m_Accid;
	// 角色ID
	ObjUInt64	m_RoleId;
	// 今日次数
	ObjUInt32	m_TodayTimes;
	// 总次数
	ObjUInt32	m_TotalTimes;
	// 更新时间
	ObjUInt32	m_UpdateTime;
};

#endif