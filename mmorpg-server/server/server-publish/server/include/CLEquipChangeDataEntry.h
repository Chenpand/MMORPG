#ifndef _CL_EQUIP_CHAGNGE_DATAENTRY_H_
#define _CL_EQUIP_CHAGNGE_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief 装备转换表
*/
struct EquipChangeDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	// 唯一id
	UInt32	id;
	// 小职业id
	std::vector<UInt32> jobIds;
	// 转换类型
	UInt8	convertType;
	// 套装装备id
	std::vector<UInt32> suitEquips;
	// 套装等级
	UInt32	suitLevel;
};

class EquipChangeDataEntryMgr : public Avalon::DataEntryMgrBase<EquipChangeDataEntry>
	, public Avalon::Singleton<EquipChangeDataEntryMgr>
{
public:
	virtual bool AddEntry(EquipChangeDataEntry* entry);
	UInt32  GetSuitIdByEqId(UInt32 eqId);
private:
	std::map<UInt32, UInt32> equipMapSuit;
};


#endif
