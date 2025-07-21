#ifndef _COND_ACTIVATE_INFO_
#define _COND_ACTIVATE_INFO_


#include <CLSyncObject.h>
#include <CLObjectProperty.h>

class WSPlayer;


/**
*@brief ����������Ϣ
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
	*@brief ���û�ȡ�˺�id
	*/
	void SetAccID(UInt32 accId) { m_AccId = accId; }
	UInt32 GetAccID() const { return m_AccId; }

	/**
	*@brie ���û�ȡ��ɫid
	*/
	void SetRoleID(UInt64 roleId) { m_RoleId = roleId; }
	UInt64 GetRoleID() const { return m_RoleId; }

	/**
	*@brief ���û�ȡ�̳���Ʒid
	*/
	void SetMallGoodsID(UInt32 goodsId) { m_MallGoodsID = goodsId; }
	UInt32 GetMallGoodsID() const { return m_MallGoodsID; }

	/**
	*@brief ���û�ȡ������ʼʱ��
	*/
	void SetActivateStartTime(UInt32 startTime) { m_ActivateStartTime = startTime; }
	UInt32 GetActivateStartTime() const { return m_ActivateStartTime; }

	/**
	*@brief ���û�ȡ��������ʱ��
	*/
	void SetActivateEndTime(UInt32 endTime) { m_ActivateEndTime = endTime; }
	UInt32 GetActivateEndTime() const { return m_ActivateEndTime; }

	/**
	*@brief ���û�ȡʣ������
	*/
	void SetRestNum(UInt16 restNum) { m_RestNum = restNum; }
	UInt16 GetRestNum() const { return m_RestNum; }

	/**
	*@brief ���û�ȡ����״̬
	*/
	void SetActivateState(UInt8 state) { m_ActivateState = state; }
	UInt8 GetActivateState() const { return m_ActivateState; }

	/**
	*@brief ���û�ȡ��������
	*/
	void SetActivateCond(UInt8 cond) { m_ActivateCond = cond; }
	UInt8 GetActivateCond() const { return m_ActivateCond; }

	/**
	*@brief ���ӻ�ȡ��������
	*/
	void AddActivateCount();
	UInt16 GetActivateCount() const { return m_ActivateCount; }

	/**
	*@brief ���û�ȡ���������������ڹ��ô���һ����Ʒ��
	*/
	void SetActivateSubCond(UInt8 subCond) { m_ActivateSubCond = subCond; }
	UInt8 GetActivateSubCond() const { return m_ActivateSubCond; }

	/**
	*@brief ���û�ȡ��������
	*/
	void SetActivateType(UInt8 type) { m_ActivateType = type; }
	UInt8 GetActivateType() const { return m_ActivateType; }

public:
	//���ݿ����
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
	// �˺�id
	ObjUInt32		m_AccId;

	// ��ɫid
	ObjUInt64		m_RoleId;

	// �̳���Ʒid
	ObjUInt32		m_MallGoodsID;

	// ������ʼʱ��
	ObjUInt32		m_ActivateStartTime;

	// ��������ʱ��
	ObjUInt32		m_ActivateEndTime;

	// ʣ������
	ObjUInt16		m_RestNum;

	// ����״̬
	ObjUInt8		m_ActivateState;

	// ��������
	ObjUInt8		m_ActivateCond;

	// ��������
	ObjUInt16		m_ActivateCount;

	// ����������
	ObjUInt8		m_ActivateSubCond;

	// ��������
	ObjUInt8		m_ActivateType;
};

#endif