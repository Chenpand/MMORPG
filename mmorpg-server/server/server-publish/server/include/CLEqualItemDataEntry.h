#ifndef _CL_EQUAL_ITEM_DATAENTRY_H_
#define _CL_EQUAL_ITEM_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include<unordered_map>
#include<unordered_set>
/**
*@brief 道具等价关系表数据项
*/
struct EqualItemDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return (UInt32)id; }

	bool Read(Avalon::DataRow& row);

public:
	// 道具ID
	UInt32	id;
	//消耗道具ID
	std::vector<UInt32>	equalItemIds;
};
class EqualItemDataEntryMgr : public Avalon::DataEntryMgrBase<EqualItemDataEntry>
	, public Avalon::Singleton<EqualItemDataEntryMgr>
{
public:
	virtual bool AddEntry(EqualItemDataEntry* entry);
	//通过非绑定获得绑定
	UInt32  GetItemIdByEqualItemID(UInt32 id);
private:
	std::unordered_map<UInt32, UInt32> itemIdByEqualItemID;
};

#endif
