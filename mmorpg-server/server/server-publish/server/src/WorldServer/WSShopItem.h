#ifndef _WS_SHOP_ITEM_H_
#define _WS_SHOP_ITEM_H_

#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <CLShopProtocol.h>


// ��Ʒˢ������
enum ShopItemRefreshType
{
	// ��ˢ��
	SHOP_ITEM_RT_NOT = 0,
	// ÿ��1��6��
	SHOP_ITEM_RT_ONE,
};

// ��Ʒ�޹�����
enum ShopItemLimitBuyType
{
	// ���޹�
	SHOP_ITEM_LBT_NOT = 0,
	// ˢ���޹�
	SHOP_ITEM_LBT_REFRESH,
};

// ��Ʒ����
struct WSShopItemIndex
{
	WSShopItemIndex(UInt8 tabType, UInt8 jobType)
	: m_TabType(tabType), m_JobType(jobType)
	{}

	bool operator<(const WSShopItemIndex& rhs) const
	{
		if (m_TabType != rhs.m_TabType)
			return m_TabType < rhs.m_TabType;

		if (m_JobType != rhs.m_JobType)
			return m_JobType < rhs.m_JobType;

		return false;
	}

	// ҳǩ���
	UInt8		m_TabType;
	// ְҵ���
	UInt8		m_JobType;
};

class AccountShopItemDataEntry;
class WSPlayer;

class WSShopItem : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(WSShopItem, Avalon::NullMutex)

public:
	WSShopItem();
	~WSShopItem();

	bool InitByShopItemData(WSPlayer* player, const AccountShopItemDataEntry* data);

public:
	void SetShopItemId(UInt32 id) { m_ShopItemId = id; }
	UInt32 GetShopItemId() const { return m_ShopItemId; }

	void SetOwner(UInt32 accid) { m_Owner = accid; }
	UInt32 GetOwner() const { return m_Owner; }

	void SetCreateTime(UInt64 time) { m_CreateTime = time; }
	UInt64 GetCreateTime() const { return m_CreateTime; }

	void SetUpdateTime(UInt64 time) { m_UpdateTime = time; }
	UInt64 GetUpdateTime() const { return m_UpdateTime; }

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
		CL_DBPROPERTY("shop_itemid", m_ShopItemId)
		CL_DBPROPERTY("owner", m_Owner)
		CL_DBPROPERTY("create_time", m_CreateTime)
		CL_DBPROPERTY("update_time", m_UpdateTime)

		CL_DBPROPERTY_END()

private:
	// ��Ʒid
	ObjUInt32 m_ShopItemId;
	// ӵ����(�˺�id)
	ObjUInt32 m_Owner;
	// ����ʱ��(����)
	ObjUInt64 m_CreateTime;
	// ����ʱ��(����)
	ObjUInt64 m_UpdateTime;
};

typedef std::vector<WSShopItem*> WSShopItemVec;

#endif