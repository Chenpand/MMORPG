#ifndef _GAS_ITEM_TRANSACTION_H_
#define _GAS_ITEM_TRANSACTION_H_

#include <CLDefine.h>
#include <CLItemDefine.h>

class GASPlayer;
namespace CLProtocol{
	struct TransItemData;
}

/**
 *@brief 道具事务
 */
class GASItemTransaction
{
	friend class GASPlayer;

	enum Status
	{
		STATUS_INIT =	0,
		STATUS_CHECK =	1,
		STATUS_CHECKED =	2,
		STATUS_FINAL =	3,
	};
public:
	GASItemTransaction(GASPlayer* player);
	virtual ~GASItemTransaction();

public:
	void SetErrorCode(UInt32 code) { m_ErrorCode = code; }
	UInt32 GetErrorCode() const { return m_ErrorCode; }

	GASPlayer* GetPlayer() const{ return m_pPlayer; }

	void SetBindSilver(UInt32 bindSilver) { m_BindSilver = bindSilver; }
	UInt32 GetBindSilver() { return m_BindSilver; }
	void SetSilver(UInt32 silver){ m_Silver = silver; }
	UInt32 GetSilver() const{ return m_Silver; }
	void SetGold(UInt32 gold){ m_Gold = gold; }
	UInt32 GetGold() const{ return m_Gold; }
	void SetBindGold(UInt32 gold){ m_BindGold = gold; }
	UInt32 GetBindGold() const{ return m_BindGold; }
	void SetReviveCoin(UInt32 value) { m_ReviveCoin = value; }
	UInt32 GetReviveCoin() const { return m_ReviveCoin; }
	void SetGuildContri(UInt32 value) { m_GuildContri = value; }
	UInt32 GetGuildContri() const { return m_GuildContri; }

	void SetItem(UInt32 id, UInt32 num, UInt32 isEqualItem = 0);
	UInt32 GetItemNum(UInt32 id);
	
	void SetNoBindItem(UInt32 id, UInt32 num){ m_NoBindItems[id] = num; }
	UInt32 GetNoBindItemNum(UInt32 id) const
	{
		std::map<UInt32, UInt32>::const_iterator iter = m_NoBindItems.find(id);
		if(iter != m_NoBindItems.end()) return iter->second;
		return 0;
	}
	void SetCounter(std::string name, UInt32 maxCount, UInt32 needCount = 1)
	{
		m_CounterName = name;
		m_MaxCount = maxCount;
		m_NeedCount = needCount;
	}

	void SetCounterTimes(UInt32 times) { m_CounterTimes = times; }
	UInt32 GetCounterTimes() { return m_CounterTimes; }

	void AddDetailItem(ObjID_t guid, UInt32 itemTypeId, UInt32 num)
	{
		DetailItem detailItem;
		detailItem.guid = guid;
		detailItem.itemTypeId = itemTypeId;
		detailItem.num = num;
		m_detailItems.push_back(detailItem);
	}

public:
	/**
	 *@brief 获取序列
	 */
	UInt16 GetSerial() const{ return m_Serial; }

	/**
	 *@brief 发出检查
	 */
	void RemoveItem(const std::string& reason);

	/**
	 *@brief 解锁
	 */
	void UnLock();

public:
	/**
	 *@brief 检查完
	 */
	virtual void OnCheckSucced(GASPlayer* player) = 0;
	virtual void OnCheckFailed(GASPlayer* player) = 0;

private:
	void CheckItem();

private:
	UInt8		m_Status;
	UInt16		m_Serial;
	UInt32		m_ErrorCode;
	GASPlayer*	m_pPlayer;
	UInt32		m_BindSilver;
	UInt32		m_Silver;
	UInt32		m_BindGold;
	UInt32		m_Gold;
	UInt32		m_ReviveCoin;
	// 公会贡献
	UInt32		m_GuildContri;
	std::map<UInt32, CLProtocol::TransItemData>	m_Items;
	std::map<UInt32, UInt32>	m_NoBindItems;

	/*
		查询次数是否够用，m_MaxCount代表最大次数，m_NeedCount代表需要的次数
		需要maxcount - count > needcount
	*/
	std::string	m_CounterName;
	UInt32		m_MaxCount;
	UInt32		m_NeedCount;
	// 这个是查询出来的当前次数
	UInt32		m_CounterTimes;
	std::vector<DetailItem> m_detailItems;
};

#endif
