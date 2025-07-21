#ifndef _CL_EQUIP_SUIT_DATAENTRY_H_
#define _CL_EQUIP_SUIT_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

//套装属性
class EquipSuitDataEntry : public Avalon::DataEntry
{
public:
	EquipSuitDataEntry();

public:
	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);

public:
	UInt32 id;
	// 套装等级
	UInt32 suitLev;
	// 不同件数对应的套装属性
	UInt32 attrIdByEquipNum[6];
	// 套装评分修正
	UInt32 eqScoreAdjust;
	// 不同件数对应的套装属性
	UInt32 attrIdByEquipNumNew[9];
};
typedef Avalon::DataEntryMgr<EquipSuitDataEntry> EquipSuitDataEntryMgr;
#endif
