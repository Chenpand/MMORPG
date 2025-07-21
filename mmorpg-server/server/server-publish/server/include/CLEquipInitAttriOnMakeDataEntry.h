#ifndef _CL_EQUIP_INITATTRI_ONMAKE_DATAENTRY_H_
#define _CL_EQUIP_INITATTRI_ONMAKE_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>


/**
 *@brief data item
 */
class EquipInitAttriOnMakeDataEntry : public Avalon::DataEntry
{
public:
	EquipInitAttriOnMakeDataEntry() {}
	~EquipInitAttriOnMakeDataEntry() {}

public:
	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 7; };

public:
	//唯一id
	UInt32		id;
	//道具id
	UInt32		itemId;
	//强化等级
	UInt32		strenthLv;
	//品级
	UInt32		qualityLv;
	//是否租赁
	UInt8		isLease;
	//推荐职业
	std::vector<UInt32>	fitOccus;
	//来源
	UInt32		sourceId;
};

typedef Avalon::DataEntryVisitor<EquipInitAttriOnMakeDataEntry> EquipInitAttriOnMakeDataEntryVisitor;

class EquipInitAttriOnMakeDataEntryMgr : public Avalon::DataEntryMgrBase<EquipInitAttriOnMakeDataEntry>
	, public Avalon::Singleton<EquipInitAttriOnMakeDataEntryMgr>
{
public:
	EquipInitAttriOnMakeDataEntryMgr() {}
	~EquipInitAttriOnMakeDataEntryMgr() {}
};

#endif