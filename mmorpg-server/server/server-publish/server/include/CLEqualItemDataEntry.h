#ifndef _CL_EQUAL_ITEM_DATAENTRY_H_
#define _CL_EQUAL_ITEM_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include<unordered_map>
#include<unordered_set>
/**
*@brief ���ߵȼ۹�ϵ��������
*/
struct EqualItemDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return (UInt32)id; }

	bool Read(Avalon::DataRow& row);

public:
	// ����ID
	UInt32	id;
	//���ĵ���ID
	std::vector<UInt32>	equalItemIds;
};
class EqualItemDataEntryMgr : public Avalon::DataEntryMgrBase<EqualItemDataEntry>
	, public Avalon::Singleton<EqualItemDataEntryMgr>
{
public:
	virtual bool AddEntry(EqualItemDataEntry* entry);
	//ͨ���ǰ󶨻�ð�
	UInt32  GetItemIdByEqualItemID(UInt32 id);
private:
	std::unordered_map<UInt32, UInt32> itemIdByEqualItemID;
};

#endif
