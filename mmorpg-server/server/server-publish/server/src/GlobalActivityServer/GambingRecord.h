#ifndef _GAMBING_RECORD_H_
#define _GAMBING_RECORD_H_


#include <AvalonObjectPool.h>
#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <CLItemDefine.h>
#include <CLObjectProperty.h>


/**
*@brief �ᱦ��¼
*/
class GambingRecord : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(GambingRecord, Avalon::NullMutex)

public:
	GambingRecord();
	~GambingRecord();

public:
	/**
	*@brief ���û�ȡ���Ψһid
	*/
	void SetOwnerUId(const std::string& uid) { m_OwnerUId = uid; }
	const std::string GetOwnerUId() const { return m_OwnerUId; }

	/**
	*@brief ���û�ȡ��ɫ��
	*/
	void SetOwnerName(const std::string& name) { m_OwnerName = name; }
	const std::string& GetOwnerName() const { return m_OwnerName; }

	/**
	*@brief ���û�ȡ������ڷ�����
	*/
	void SetOwnerServerName(const std::string& name) { m_OwnerServerName = name; }
	const std::string GetOwnerServerName() const { return m_OwnerServerName; }

	/**
	*@brief ���û�ȡ����������ķ��ڵ�
	*/
	void SetOwnerSourceCenterNode(UInt32 node) { m_OwnerSourceCenterNode = node; }
	UInt32 GetOwnerSourceCenterNode() const { return m_OwnerSourceCenterNode; }

	/**
	*@brief ���û�ȡ������ڹ�����ڵ�
	*/
	void SetOwnerSourceAdminNode(UInt32 node) { m_OwnerSourceAdminNode = node; }
	UInt32 GetOwnerSourceAdminNode() const { return m_OwnerSourceAdminNode; }

	/**
	*@brief ���û�ȡ�ᱦ��Ʒid
	*/
	void SetGambingItemId(UInt32 itemid) { m_GambingItemId = itemid; }
	UInt32 GetGambingItemId() const { return m_GambingItemId; }

	/**
	*@brief ���û�ȡ�ᱦ��Ʒ����
	*/
	void SetGambingItemNum(UInt32 num) { m_GambingItemNum = num; }
	UInt32 GetGambingItemNum() const { return m_GambingItemNum; }

	/**
	*@brief ���û�ȡ���߱�id
	*/
	void SetItemDataId(UInt32 id) { m_ItemDataId = id; }
	UInt32 GetItemDataId() const { return m_ItemDataId; }

	/**
	*@brief ���û�ȡ�ᱦ��id
	*/
	void SetGambingGroupId(UInt16 groupid) { m_GroupId = groupid; }
	UInt16 GetGambingGroupId() const { return m_GroupId; }

	/**
	*@brief ���ӻ�ȡͶ�����
	*/
	void AddInvestCopies(UInt32 copies) { m_InvestCopies = m_InvestCopies + copies; }
	UInt32 GetInvestCopies() const { return m_InvestCopies; }

	/**
	*@brief ���û�ȡͶ�����id
	*/
	void SetInvestCurrencyId(UInt32 id) { m_InvestCurrencyId = id; }
	UInt32 GetInvestCurrencyId() const { return m_InvestCurrencyId; }

	/**
	*@brief ���ӻ�ȡͶ���������
	*/
	void AddInvestCurrencyNum(UInt32 num) { m_InvestCurrencyNum = m_InvestCurrencyNum + num; }
	UInt32 GetInvestCurrencyNum() const { return m_InvestCurrencyNum; }

	/**
	*@brief ���û�ȡ�ᱦȨ��
	*/
	void SetGambingWeight(UInt32 weight) { m_GambingWeight = weight; }
	UInt32 GetGambingWeight() const { return m_GambingWeight; }

	/**
	*@brief ���û�ȡ����ʱ��
	*/
	void SetCreateTimeSec(UInt32 time) { m_CreateTimestamp = time; }
	UInt32 GetCreateTimeSec() const { return m_CreateTimestamp; }

	/**
	*@brief ������ᱦ����(�ͻ�����ʾ)
	*/
	std::string CalculateGambingRate(UInt32 baseValue);

public:
	//���ݿ����
	void UpdateToDB();
	void InsertToDB();

	CL_DBPROPERTY_BEGIN(CLSyncObject)
		CL_DBPROPERTY("owner_uid", m_OwnerUId)
		CL_DBPROPERTY("owner_name", m_OwnerName)
		CL_DBPROPERTY("owner_server_name", m_OwnerServerName)
		CL_DBPROPERTY("owner_source_center_node", m_OwnerSourceCenterNode)
		CL_DBPROPERTY("owner_source_admin_node", m_OwnerSourceAdminNode)
		CL_DBPROPERTY("gambing_item_id", m_GambingItemId)
		CL_DBPROPERTY("gambing_item_num", m_GambingItemNum)
		CL_DBPROPERTY("item_data_id", m_ItemDataId)
		CL_DBPROPERTY("group_id", m_GroupId)
		CL_DBPROPERTY("invest_currency_id", m_InvestCurrencyId)
		CL_DBPROPERTY("invest_currency_num", m_InvestCurrencyNum)
		CL_DBPROPERTY("invest_copies", m_InvestCopies)
		CL_DBPROPERTY("gambing_weight", m_GambingWeight)
		CL_DBPROPERTY("create_time_sec", m_CreateTimestamp)

		CL_DBPROPERTY_END()

private:
	// ���Ψһid
	ObjString m_OwnerUId;
	// ��ɫ��
	ObjString m_OwnerName;
	// ������ڷ�����
	ObjString m_OwnerServerName;
	// ������ڵ����ķ��ڵ�
	ObjUInt32 m_OwnerSourceCenterNode;
	// ������ڵĹ�����ڵ�
	ObjUInt32 m_OwnerSourceAdminNode;
	// �ᱦ��Ʒid
	ObjUInt32 m_GambingItemId;
	// ��Ʒ����
	ObjUInt32 m_GambingItemNum;
	// ���߱�id
	ObjUInt32 m_ItemDataId;
	// �ᱦ��id
	ObjUInt16 m_GroupId;
	// Ͷ�����id
	ObjUInt32 m_InvestCurrencyId;
	// Ͷ���������
	ObjUInt32 m_InvestCurrencyNum;
	// Ͷ�����
	ObjUInt32 m_InvestCopies;
	// �ᱦȨ��
	ObjUInt32 m_GambingWeight;
	// ����ʱ��
	ObjUInt32 m_CreateTimestamp;
};

/**
*@brief �ᱦ����״̬
*/
enum GambingLotteryStatus
{
	// ��ʼֵ
	GLS_INIT = 0,
	// �ȴ�
	GLS_WAIT,
	// �ѿ���
	GLS_OPENED,
};

/**
*@brief �ᱦ��Ʒ������¼
*/
class GambingLotteryRecord : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(GambingLotteryRecord, Avalon::NullMutex)

public:
	GambingLotteryRecord();
	~GambingLotteryRecord();

public:
	/**
	*@brief ���û�ȡ��Ʒid
	*/
	void SetGambingItemId(UInt32 id) { m_GambingItemId = id; }
	UInt32 GetGambingItemId() const { return m_GambingItemId; }

	/**
	*@brief ���û�ȡ�ᱦ��Ʒid
	*/
	void SetGambingItemNum(UInt32 num) { m_GambingItemNum = num; }
	UInt32 GetGambingItemNum() const { return m_GambingItemNum; }

	/**
	*@brief ���û�ȡ���߱�id
	*/
	void SetItemDataId(UInt32 id) { m_ItemDataId = id; }
	UInt32 GetItemDataId() const { return m_ItemDataId; }

	/**
	*@brief ���û�ȡ��id
	*/
	void SetGroupId(UInt16 id) { m_GroupId = id; }
	UInt16 GetGroupId() const { return m_GroupId; }

	/**
	*@brief ���û�ȡ����ʱ��
	*/
	void SetSoldOutTimeSec(UInt32 time) { m_SoldOutTimeSec = time; }
	UInt32 GetSoldOutTimeSec() const { return m_SoldOutTimeSec; }

	/**
	*@brief ���û�ȡ����ʱ��
	*/
	void SetLotteryTimeSec(UInt32 time) { m_LotteryTimeSec = time; }
	UInt32 GetLotteryTimeSec() const { return m_LotteryTimeSec; }

	/**
	*@brief ���û�ȡӮ��id
	*/
	void SetGainerUId(const std::string& uid) { m_GainerUId = uid; }
	const std::string& GetGainerUId() const { return m_GainerUId; }

	/**
	*@brief ���û�ȡ����ʱ��
	*/
	void SetCreateTimeSec(UInt32 time) { m_CreateTimeSec = time; }
	UInt32 GetCreateTimeSec() const { return m_CreateTimeSec; }

public:
	//���ݿ����
	void UpdateToDB(bool bFlush = true);
	void InsertToDB();

	CL_DBPROPERTY_BEGIN(CLSyncObject)
		CL_DBPROPERTY("gambing_item_id", m_GambingItemId)
		CL_DBPROPERTY("gambing_item_num", m_GambingItemNum)
		CL_DBPROPERTY("item_data_id", m_ItemDataId)
		CL_DBPROPERTY("group_id", m_GroupId)
		CL_DBPROPERTY("soldout_time_sce", m_SoldOutTimeSec)
		CL_DBPROPERTY("lottery_time_sec", m_LotteryTimeSec)
		CL_DBPROPERTY("gainer_uid", m_GainerUId)
		CL_DBPROPERTY("create_time_sec", m_CreateTimeSec)

		CL_DBPROPERTY_END()

private:
	// ��Ʒid
	ObjUInt32 m_GambingItemId;
	// ��Ʒ����
	ObjUInt32 m_GambingItemNum;
	// ���߱�id
	ObjUInt32 m_ItemDataId;
	// ��id
	ObjUInt16 m_GroupId;
	// ����ʱ��
	ObjUInt32 m_SoldOutTimeSec;
	// ����ʱ��
	ObjUInt32 m_LotteryTimeSec;
	// Ӯ��
	ObjString m_GainerUId;
	// ����ʱ��
	ObjUInt32 m_CreateTimeSec;
};

#endif