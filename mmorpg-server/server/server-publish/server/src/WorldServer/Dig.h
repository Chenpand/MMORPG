#ifndef __DIG_H__
#define __DIG_H__

#include "CLDigDefine.h"
#include "CLDigDataEntry.h"

class OpenDigTransaction;
class WSPlayer;

class DigItem
{
public:
	DigItem(UInt32 index) :m_Index(index), m_ItemId(0), m_Weight(0), m_DigItemType(DigItemType::DIT_INVALID),m_RewardId(0){}
	~DigItem() {}

public:
	UInt32 m_Index;
	UInt32 m_ItemId;
	UInt32 m_Weight;
	DigItemType m_DigItemType;
	UInt32 m_RewardId;
};

class Dig
{
public:
	Dig(UInt32 index);
	~Dig(){}

public:
	UInt32 CheckOpen(WSPlayer* player, UInt32 mapId);

	UInt32 Open(WSPlayer* player, UInt32& index, UInt32& itemId, UInt32& itemNum);

	UInt32 GMOpen(WSPlayer* player, UInt32& index, UInt32& itemId, UInt32& itemNum);

	UInt32 Refresh(UInt32 refreshTime, DigType digType = DigType::DIG_INVALID);

	void GetDigInfo(DigInfo& info);

	void GetDigDetailInfo(DigDetailInfo& info);

public:

	inline UInt32 GetIndex() { return m_Index; }
	inline void SetIndex(UInt32 index) { m_Index = index; }

	inline DigType GetDigType() { return m_DigType; }
	inline void SetDigType(DigType digType) { m_DigType = digType; }

	inline DigStatus GetDigStatus() { return m_DigStatus; }
	inline void SetDigStatus(DigStatus status) { m_DigStatus = status; }

	inline UInt32 GetLastRefreshTime() { return m_LastRefreshTime; }
	inline void SetLastRefreshTime(UInt32 refreshTime) { m_LastRefreshTime = refreshTime; }

	inline UInt32 GetLastStatusTime() { return m_LastStatusTime; }
	inline void SetLastStatusTime(UInt32 lastStatusTime) { m_LastStatusTime = lastStatusTime; }

	inline UInt32 GetOpenItemIndex() { return m_OpenItemIndex; }
	inline void SetOpenItemIndex(UInt32 itemIndex) { m_OpenItemIndex = itemIndex; }

	inline UInt32 GetOpenItemId() { return m_OpenItemId; }
	inline void SetOpenItemId(UInt32 itemId) { m_OpenItemId = itemId; }

	inline UInt32 GetOpenItemNum() { return m_OpenItemNum; }
	inline void SetOpenItemNum(UInt32 itemNum) { m_OpenItemNum = itemNum; }

	inline OpenDigTransaction* GetOpenDigTransaction() { return m_OpenDigTransaction; }
	inline void SetOpenDigTransaction(OpenDigTransaction* tran) { m_OpenDigTransaction = tran; }

private:
	UInt32 RandomSlot(DigItemPoolDataEntryMgr::DigItemPoolGroupMap& digItemPoolGroup, std::map<UInt32, DigItemPoolDataEntry*>& digItemPoolDataEntryMap);

	void PushSlot(DigItemPoolDataEntry* dataEntry);

	bool GetRandomIndex(std::vector<UInt32>& randomVec, DigItemPoolDataEntry* digItemPoolDataEntry);

	void RemoveRandomIndex(std::vector<UInt32>& randomVec, UInt32 index);

	void InitDigItemPools();

public:
	//宝箱物品
	std::vector<DigItem> m_DigItemPools;


	UInt32 m_GridNum;

	//索引
	UInt32 m_Index;

	//挖宝点类型
	DigType m_DigType;

	//挖宝点状态
	DigStatus m_DigStatus;

	//最后刷新时间
	UInt32 m_LastRefreshTime;

	//最后状态变更
	UInt32 m_LastStatusTime;

	//随机的位置
	UInt32 m_OpenItemIndex;

	//随机的道具ID
	UInt32 m_OpenItemId;

	//随机的道具数量
	UInt32 m_OpenItemNum;

	//打开宝箱的事务
	OpenDigTransaction* m_OpenDigTransaction;

};

#endif //__DIG_H__