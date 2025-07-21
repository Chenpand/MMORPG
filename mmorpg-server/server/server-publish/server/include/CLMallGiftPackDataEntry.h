#ifndef _CL_MALL_GIFT_PACK_DATA_ENTRY_H_
#define _CL_MALL_GIFT_PACK_DATA_ENTRY_H_

#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <CLMallDefine.h>

struct MallGiftPackDataEntry : public Avalon::DataEntry
{
public:
	MallGiftPackDataEntry();
	~MallGiftPackDataEntry();

public:
	UInt32 GetKey() const { return giftPackId; }

	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 4; };

public:
	//限时礼包id
	UInt32 giftPackId;

	//限时礼包触发条件
	UInt8 activateCond;

	//限时时长(小时)
	UInt32 limitInterval;

	//扩展参数
	std::vector<UInt32> params;
};


typedef HashMap<UInt8, std::set<UInt32>> ActivateCondMap;

class MallGiftPackDataEntryMgr : public Avalon::DataEntryMgrBase<MallGiftPackDataEntry>
	, public Avalon::Singleton<MallGiftPackDataEntryMgr>
{
public:
	MallGiftPackDataEntryMgr();
	~MallGiftPackDataEntryMgr();

	bool AddEntry(MallGiftPackDataEntry* dataEntry);

	void GetGiftsByCondition(MallGiftPackActivateCond cond, std::vector<UInt32>& gifts);

private:
	//限时礼包触发条件-->限时礼包Id
	HashMap<UInt8, std::vector<UInt32>> m_ActivateCondGiftsMap;
};

#endif