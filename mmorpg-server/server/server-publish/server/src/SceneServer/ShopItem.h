#ifndef _SHOP_ITEM_H_
#define _SHOP_ITEM_H_

#include <CLItemDefine.h>
#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <CLShopItemDataEntry.h>
#include <CLSceneObject.h>
#include <CLItemProperty.h>

class Player;

/**
 *@brief �̵���Ʒ
 */
class ShopItem : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(ShopItem, Avalon::NullMutex)

public:
	ShopItem();
	~ShopItem();

public:	//set/get
	
	void SetDataEntry(ShopItemDataEntry* dataEntry) {
		m_pDataEntry = dataEntry; 
		m_shopItemId = dataEntry->shopItemId;
		m_restNum = dataEntry->numLimite;
	}
	ShopItemDataEntry* GetDataEntry() const { return m_pDataEntry; }
	
	//��ȡ��Ʒid
	UInt32 GetShopItemId() { return m_shopItemId; }

	inline void SetGrid(UInt8 val) { m_grid = val; }
	inline UInt8 GetGrid() { return m_grid; }
	void ReduceRestNum(UInt32 num);

	//���û�ȡ��Ʒʣ������
	inline Int16 GetRestNum() { return m_restNum; }
	void SetRestNum(Int16 restNum) { m_restNum = restNum; }

	inline void SetVipLv(UInt8 vipLv) { m_vipLv = vipLv; }
	inline UInt8 GetVipLv() { return m_vipLv; }
	inline void SetVipDisCount(UInt32 dis) { m_vipDiscount = dis; }
	inline UInt32 GetVipDisCount() { return m_vipDiscount; }
	inline ObjID_t GetShopId() { return m_shopId; }
	inline void SetOwner(ObjID_t owner) { m_owner = owner; }
	inline void SetShopId(ObjID_t shopId) { m_shopId = shopId; }

	//�����ȡ��Ʒ������
	void CalculateDiscountRate();
	UInt32 GetDiscountRate() { return m_DiscountRate; }

	//����
	UInt32 GetLeaseEndTimeStamp() { return m_LeaseEndTimeStamp; }
	void SetLeaseEndTimeStamp(UInt32 timeStamp) { m_LeaseEndTimeStamp = timeStamp; }
public: //���ߴ������
	/**
	 *@brief �������ٵ��߶���
	 */
	static ShopItem* Create(UInt32 id);
	static void Destroy(ShopItem*& item);

public://��������
	void SyncProperty(Player* player, bool bFlush = false);

	CL_DBPROPERTY_BEGIN(CLSyncObject)
	CL_DBPROPERTY("shopid", m_shopId)
	CL_DBPROPERTY("owner", m_owner)
	CL_DBPROPERTY("shopitemid", m_shopItemId)
	CL_DBPROPERTY("grid", m_grid)
	CL_DBPROPERTY("restnum", m_restNum)
	CL_DBPROPERTY("viplv", m_vipLv)
	CL_DBPROPERTY("vipdiscount", m_vipDiscount)
	CL_DBPROPERTY("lease_end_timestamp", m_LeaseEndTimeStamp)

	CL_DBPROPERTY_END()

private:
	//�̵�id
	ObjUInt64	m_shopId;
	//ӵ����
	ObjUInt64	m_owner;
	//��Ʒid
	ObjUInt32	m_shopItemId;
	//����
	ObjUInt8	m_grid;
	//ʣ������
	ObjInt16	m_restNum;
	//vip�ȼ�
	ObjUInt8	m_vipLv;
	//vip�ۿ�
	ObjUInt32	m_vipDiscount;
	//�����
	ShopItemDataEntry*	m_pDataEntry;
	//�ۿ���(�������ݿ�)
	UInt32		m_DiscountRate;
	//���޽���ʱ���(��)
	ObjUInt32	m_LeaseEndTimeStamp;
};

#endif
