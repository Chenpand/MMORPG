#ifndef _ACCOUNT_COUNTER_H_
#define _ACCOUNT_COUNTER_H_

#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <TimeUtil.h>


class AccountCounter : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(AccountCounter, Avalon::NullMutex)

public:
	AccountCounter();
	~AccountCounter() {}

public:
	void SetAccGUID(ObjID_t guid) { m_AccGUID = guid; }

	void SetOwner(UInt32 accid) { m_Owner = accid; }
	UInt32 GetOwner() const { return m_Owner; }

	void SetCounterType(AccountCounterType type) { m_CounterType = (UInt32)type; }
	AccountCounterType GetCounterType() const { return (AccountCounterType)m_CounterType.GetDBValue(); }

	void SetCounterNum(UInt64 num);
	UInt64 GetCounterNum();
	UInt64 IncCounterNum(UInt64 incNum);

	UInt64 GetRefreshTime() const { return m_RefreshTime; }

public:
	bool TryRefresh();
	bool TryRefresh(TimeUtil::RefreshType refreshType, TimeUtil::RefreshDate refreshDate);

public:
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
		CL_DBPROPERTY("acc_guid", m_AccGUID);
		CL_DBPROPERTY("owner", m_Owner)
		CL_DBPROPERTY("counter_type", m_CounterType)
		CL_DBPROPERTY("counter_num", m_CounterNum)
		CL_DBPROPERTY("create_time", m_CreateTime)
		CL_DBPROPERTY("refresh_time", m_RefreshTime)

		CL_DBPROPERTY_END()

private:
	// �˺�guid
	ObjUInt64 m_AccGUID;
	// �˺�id
	ObjUInt32 m_Owner;
	// ��������
	ObjUInt8 m_CounterType;
	// ����
	ObjUInt64 m_CounterNum;
	// ����ʱ��
	ObjUInt32 m_CreateTime;
	// ˢ��ʱ��
	ObjUInt64 m_RefreshTime;
};

#endif