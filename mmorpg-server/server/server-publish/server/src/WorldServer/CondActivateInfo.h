#ifndef _COND_ACTIVATE_INFO_
#define _COND_ACTIVATE_INFO_


#include <CLSyncObject.h>
#include <CLObjectProperty.h>

class WSPlayer;


/**
*@brief 条件触发信息
*/
class CondActivateInfo : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(CondActivateInfo, Avalon::NullMutex);

public:
	CondActivateInfo();
	~CondActivateInfo();

	static CondActivateInfo* Create();
	static void Destroy(CondActivateInfo*& item);

public:
	/**
	*@brief 设置获取账号id
	*/
	void SetAccID(UInt32 accId) { m_AccId = accId; }
	UInt32 GetAccID() const { return m_AccId; }

	/**
	*@brie 设置获取角色id
	*/
	void SetRoleID(UInt64 roleId) { m_RoleId = roleId; }
	UInt64 GetRoleID() const { return m_RoleId; }

	/**
	*@brief 设置获取商城商品id
	*/
	void SetMallGoodsID(UInt32 goodsId) { m_MallGoodsID = goodsId; }
	UInt32 GetMallGoodsID() const { return m_MallGoodsID; }

	/**
	*@brief 设置获取触发开始时间
	*/
	void SetActivateStartTime(UInt32 startTime) { m_ActivateStartTime = startTime; }
	UInt32 GetActivateStartTime() const { return m_ActivateStartTime; }

	/**
	*@brief 设置获取触发结束时间
	*/
	void SetActivateEndTime(UInt32 endTime) { m_ActivateEndTime = endTime; }
	UInt32 GetActivateEndTime() const { return m_ActivateEndTime; }

	/**
	*@brief 设置获取剩余数量
	*/
	void SetRestNum(UInt16 restNum) { m_RestNum = restNum; }
	UInt16 GetRestNum() const { return m_RestNum; }

	/**
	*@brief 设置获取触发状态
	*/
	void SetActivateState(UInt8 state) { m_ActivateState = state; }
	UInt8 GetActivateState() const { return m_ActivateState; }

	/**
	*@brief 设置获取触发条件
	*/
	void SetActivateCond(UInt8 cond) { m_ActivateCond = cond; }
	UInt8 GetActivateCond() const { return m_ActivateCond; }

	/**
	*@brief 增加获取触发计数
	*/
	void AddActivateCount();
	UInt16 GetActivateCount() const { return m_ActivateCount; }

	/**
	*@brief 设置获取触发子条件（用于共用触发一个商品）
	*/
	void SetActivateSubCond(UInt8 subCond) { m_ActivateSubCond = subCond; }
	UInt8 GetActivateSubCond() const { return m_ActivateSubCond; }

	/**
	*@brief 设置获取触发类型
	*/
	void SetActivateType(UInt8 type) { m_ActivateType = type; }
	UInt8 GetActivateType() const { return m_ActivateType; }

public:
	//数据库相关
	void UpdateToDB(WSPlayer* player, bool bFlush = true);
	void InsertToDB(WSPlayer* player);

	CL_DBPROPERTY_BEGIN(CLSyncObject)
		CL_DBPROPERTY("accid", m_AccId)
		CL_DBPROPERTY("roleid", m_RoleId)
		CL_DBPROPERTY("gift_pack_id", m_MallGoodsID)
		CL_DBPROPERTY("activate_start_time", m_ActivateStartTime)
		CL_DBPROPERTY("activate_end_time", m_ActivateEndTime)
		CL_DBPROPERTY("rest_num", m_RestNum)
		CL_DBPROPERTY("activate_state", m_ActivateState)
		CL_DBPROPERTY("activate_cond", m_ActivateCond)
		CL_DBPROPERTY("activate_counter", m_ActivateCount)
		CL_DBPROPERTY("activate_sub_cond", m_ActivateSubCond)
		CL_DBPROPERTY("activate_type", m_ActivateType)

	CL_DBPROPERTY_END()

private:
	// 账号id
	ObjUInt32		m_AccId;

	// 角色id
	ObjUInt64		m_RoleId;

	// 商城商品id
	ObjUInt32		m_MallGoodsID;

	// 触发开始时间
	ObjUInt32		m_ActivateStartTime;

	// 触发结束时间
	ObjUInt32		m_ActivateEndTime;

	// 剩余数量
	ObjUInt16		m_RestNum;

	// 触发状态
	ObjUInt8		m_ActivateState;

	// 触发条件
	ObjUInt8		m_ActivateCond;

	// 触发计数
	ObjUInt16		m_ActivateCount;

	// 触发子条件
	ObjUInt8		m_ActivateSubCond;

	// 触发类型
	ObjUInt8		m_ActivateType;
};

#endif