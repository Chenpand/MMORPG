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
 *@brief 商店物品
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
	
	//获取商品id
	UInt32 GetShopItemId() { return m_shopItemId; }

	inline void SetGrid(UInt8 val) { m_grid = val; }
	inline UInt8 GetGrid() { return m_grid; }
	void ReduceRestNum(UInt32 num);

	//设置获取商品剩余数量
	inline Int16 GetRestNum() { return m_restNum; }
	void SetRestNum(Int16 restNum) { m_restNum = restNum; }

	inline void SetVipLv(UInt8 vipLv) { m_vipLv = vipLv; }
	inline UInt8 GetVipLv() { return m_vipLv; }
	inline void SetVipDisCount(UInt32 dis) { m_vipDiscount = dis; }
	inline UInt32 GetVipDisCount() { return m_vipDiscount; }
	inline ObjID_t GetShopId() { return m_shopId; }
	inline void SetOwner(ObjID_t owner) { m_owner = owner; }
	inline void SetShopId(ObjID_t shopId) { m_shopId = shopId; }

	//计算获取商品总数量
	void CalculateDiscountRate();
	UInt32 GetDiscountRate() { return m_DiscountRate; }

	//租赁
	UInt32 GetLeaseEndTimeStamp() { return m_LeaseEndTimeStamp; }
	void SetLeaseEndTimeStamp(UInt32 timeStamp) { m_LeaseEndTimeStamp = timeStamp; }
public: //道具创建相关
	/**
	 *@brief 创建销毁道具对象
	 */
	static ShopItem* Create(UInt32 id);
	static void Destroy(ShopItem*& item);

public://编解码相关
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
	//商店id
	ObjUInt64	m_shopId;
	//拥有者
	ObjUInt64	m_owner;
	//商品id
	ObjUInt32	m_shopItemId;
	//格子
	ObjUInt8	m_grid;
	//剩余数量
	ObjInt16	m_restNum;
	//vip等级
	ObjUInt8	m_vipLv;
	//vip折扣
	ObjUInt32	m_vipDiscount;
	//表格项
	ShopItemDataEntry*	m_pDataEntry;
	//折扣率(不存数据库)
	UInt32		m_DiscountRate;
	//租赁结束时间戳(秒)
	ObjUInt32	m_LeaseEndTimeStamp;
};

#endif
