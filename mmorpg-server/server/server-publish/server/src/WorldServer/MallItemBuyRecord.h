#ifndef _MALL_ITEM_BUY_RECORD_H_
#define _MALL_ITEM_BUY_RECORD_H_

#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <TimeUtil.h>

struct MallItemInfo;


struct MallItemAccountBuyIndex
{
	MallItemAccountBuyIndex(UInt32 _accId, UInt32 _mallItemId)
	:accId(_accId), mallItemId(_mallItemId) {}

	bool operator<(const MallItemAccountBuyIndex& rhs) const
	{
		if (accId != rhs.accId)
			return accId < rhs.accId;

		if (mallItemId != rhs.mallItemId)
			return mallItemId < rhs.mallItemId;

		return false;
	}

	UInt32 accId;
	UInt32 mallItemId;
};

class MallItemAccountBuyRecord : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(MallItemAccountBuyRecord, Avalon::NullMutex)

public:
	MallItemAccountBuyRecord();
	~MallItemAccountBuyRecord() {}

public:
	void SetMallItemId(UInt32 id) { m_MallItemId = id; }
	UInt32 GetMallItemId() const { return m_MallItemId; }

	void SetOwner(UInt32 accid) { m_Owner = accid; }
	UInt32 GetOwner() const { return m_Owner; }

	void SetBuyedNum(UInt32 num) { m_BuyedNum = num; }
	UInt32 GetBuyedNum() const { return m_BuyedNum; }
	void IncBuyedNum(UInt32 incNum);

	void SetRefreshTime(UInt64 time) { m_RefreshTime = time; }
	UInt64 GetRefreshTime() const { return m_RefreshTime; }

	void SetCreateTime(UInt64 time) { m_CreateTime = time; }
	UInt64 GetCreateTime() const { return m_CreateTime; }

public:
	bool TryRefresh(TimeUtil::RefreshType refreshType, TimeUtil::RefreshDate refreshDate);

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
	CL_DBPROPERTY_BEGIN(CLSyncObject)
		CL_DBPROPERTY("mall_item_id", m_MallItemId)
		CL_DBPROPERTY("owner", m_Owner)
		CL_DBPROPERTY("buyed_num", m_BuyedNum)
		CL_DBPROPERTY("refresh_time", m_RefreshTime)
		CL_DBPROPERTY("create_time", m_CreateTime)

		CL_DBPROPERTY_END()
	
private:
	// 商品id
	ObjUInt32 m_MallItemId;
	// 拥有者(账号id)
	ObjUInt32 m_Owner;
	// 已购买数量
	ObjUInt32 m_BuyedNum;
	// 刷新时间(毫秒)
	ObjUInt64 m_RefreshTime;
	// 创建时间(毫秒)
	ObjUInt64 m_CreateTime;
};


#endif