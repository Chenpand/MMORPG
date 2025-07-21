#ifndef _CL_EQUIP_CHG_CONS_DATAENTRY_H_
#define _CL_EQUIP_CHG_CONS_DATAENTRY_H_

#include "CLDefine.h"
#include "CLItemDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

struct ConvertorSt 
{
	ConvertorSt()
		: threType(0), suitLev(0), coumseNum(0)
	{}
	UInt32	threType;	//需要道具三类型
	UInt32   suitLev;	//试用套装等级
	UInt32	 coumseNum;	//消耗数量
};

/**
*@brief 装备转换表
*/
struct EquipChgConsDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	// 唯一id
	UInt32	id;
	// 转换类型
	UInt8	convertType;
	// 装备子类型
	UInt8	eqSubType;
	// 套装等级
	UInt32	suitLevel;
	// 品质1
	UInt8	color1;
	// 品质2
	UInt8	color2;
	// 消耗材料
	std::vector<ItemReward> consumeItems;
	// 消耗转换器
	ConvertorSt	convertor;
};

struct EquipChgConsDataKey
{
	EquipChgConsDataKey(UInt8	convertType_, UInt8	subtype_, UInt32 level_, UInt8 color1_, UInt8 color2_)
		: convertType(convertType_), subtype(subtype_), level(level_), color1(color1_), color2(color2_){}

	bool operator<(const EquipChgConsDataKey& other) const
	{
		if (other.convertType != convertType)
		{
			return convertType < other.convertType;
		}
		if (other.subtype != subtype)
		{
			return subtype < other.subtype;
		}
		if (other.level != level)
		{
			return level < other.level;
		}
		if (other.color1 != color1)
		{
			return color1 < other.color1;
		}

		return color2 < other.color2;
	}

	UInt8	convertType;
	UInt8	subtype;
	UInt32	level;
	UInt8	color1; 
	UInt8	color2;
};

class EquipChgConsDataEntryMgr : public Avalon::DataEntryMgrBase<EquipChgConsDataEntry>
	, public Avalon::Singleton<EquipChgConsDataEntryMgr>
{
public:
	virtual bool AddEntry(EquipChgConsDataEntry* entry);
	EquipChgConsDataEntry*  GetDataEntry(UInt8	convertType, UInt8	subtype, UInt32 level, UInt8 color1, UInt8 color2);
private:
	std::map<EquipChgConsDataKey, EquipChgConsDataEntry*> equipequipMapSuitMap;
};
#endif
