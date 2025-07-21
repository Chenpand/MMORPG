#ifndef _WS_SHOP_ITEM_BUY_RECORD_H_
#define _WS_SHOP_ITEM_BUY_RECORD_H_

#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <TimeUtil.h>


struct WSShopItemAccountBuyIndex
{
	WSShopItemAccountBuyIndex(UInt32 _accId, UInt32 _shopItemId)
	:accId(_accId), shopItemId(_shopItemId) {}

	bool operator<(const WSShopItemAccountBuyIndex& rhs) const
	{
		if (accId != rhs.accId)
			return accId < rhs.accId;

		if (shopItemId != rhs.shopItemId)
			return shopItemId < rhs.shopItemId;

		return false;
	}

	UInt32 accId;
	UInt32 shopItemId;
};

struct WSShopItemRoleBuyIndex
{
	WSShopItemRoleBuyIndex(ObjID_t _roleId, UInt32 _shopItemId)
	:roleId(_roleId), shopItemId(_shopItemId) {}

	bool operator<(const WSShopItemRoleBuyIndex& rhs) const
	{
		if (roleId != rhs.roleId)
			return roleId < rhs.roleId;

		if (shopItemId != rhs.shopItemId)
			return shopItemId < rhs.shopItemId;

		return false;
	}

	ObjID_t roleId;
	UInt32 shopItemId;
};

class WSShopItemBuyBaseRecord : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(WSShopItemBuyBaseRecord, Avalon::NullMutex)

public:
	WSShopItemBuyBaseRecord();
	virtual ~WSShopItemBuyBaseRecord() {}

public:
	void SetShopItemId(UInt32 id) { m_ShopItemId = id; }
	UInt32 GetShopItemId() const { return m_ShopItemId; }

	void SetBuyedNum(UInt32 num) { m_BuyedNum = num; }
	UInt32 GetBuyedNum() const { return m_BuyedNum; }
	void IncBuyedNum(UInt32 incNum) { m_BuyedNum = m_BuyedNum + incNum; }

	void SetRefreshTime(UInt64 time) { m_RefreshTime = time; }
	UInt64 GetRefreshTime() const { return m_RefreshTime; }

	void SetCreateTime(UInt64 time) { m_CreateTime = time; }
	UInt64 GetCreateTime() const { return m_CreateTime; }

	void SetUpdateTime(UInt64 time) { m_CreateTime = time; }
	UInt64 GetUpdateTime() const { return m_CreateTime; }

public:
	bool TryRefresh(TimeUtil::RefreshType refreshType, TimeUtil::RefreshDate refreshDate);

public:
	CL_DBPROPERTY_BEGIN(CLSyncObject)
		CL_DBPROPERTY("shop_item_id", m_ShopItemId)
		CL_DBPROPERTY("buyed_num", m_BuyedNum)
		CL_DBPROPERTY("refresh_time", m_RefreshTime)
		CL_DBPROPERTY("create_time", m_CreateTime)
		CL_DBPROPERTY("update_time", m_UpdateTime)

		CL_DBPROPERTY_END()

protected:
	// 商品id
	ObjUInt32 m_ShopItemId;
	// 已购买数量
	ObjUInt32 m_BuyedNum;
	// 刷新时间(毫秒)
	ObjUInt64 m_RefreshTime;
	// 创建时间(毫秒)
	ObjUInt64 m_CreateTime;
	// 更新时间(毫秒)
	ObjUInt64 m_UpdateTime;
};

class WSShopItemAccountBuyRecord : public WSShopItemBuyBaseRecord
{
	AVALON_DEFINE_OBJECTPOOL(WSShopItemAccountBuyRecord, Avalon::NullMutex)

public:
	WSShopItemAccountBuyRecord(UInt32 accid);
	~WSShopItemAccountBuyRecord() {}

public:
	void SetAccGUID(ObjID_t guid) { m_AccGUID = guid; }

	UInt32 GetOwner() const { return m_Owner; }

	void SetShopId(UInt32 shopid) { m_ShopId = shopid; }

public:
	/*
	*@brief 日志
	*/
	const std::string LogStr();

	/*
	*@brief 保存到数据库
	*/
	void SaveToDB();

public:
	CL_DBPROPERTY_BEGIN(WSShopItemBuyBaseRecord)
		CL_DBPROPERTY("acc_guid", m_AccGUID);
		CL_DBPROPERTY("owner", m_Owner)
		CL_DBPROPERTY("shop_id", m_ShopId)

		CL_DBPROPERTY_END()

private:
	// 账号guid
	ObjUInt64 m_AccGUID;
	// 拥有者(账号id)
	ObjUInt32 m_Owner;
	// 商店id
	ObjUInt32 m_ShopId;
};

class WSShopItemRoleBuyRecord : public WSShopItemBuyBaseRecord
{
	AVALON_DEFINE_OBJECTPOOL(WSShopItemRoleBuyRecord, Avalon::NullMutex)

public:
	WSShopItemRoleBuyRecord(ObjID_t roleid);
	~WSShopItemRoleBuyRecord() {}

public:
	ObjID_t GetOwner() const { return m_Owner; }

public:
	/*
	*@brief 日志
	*/
	const std::string LogStr();

	/*
	*@brief 保存到数据库
	*/
	void SaveToDB();

public:
	CL_DBPROPERTY_BEGIN(WSShopItemBuyBaseRecord)
		CL_DBPROPERTY("owner", m_Owner)

		CL_DBPROPERTY_END()

private:
	// 拥有者(角色id)
	ObjUInt64 m_Owner;
};

#endif