#ifndef _CL_EQUIPTRANSMAP_DATAENTRY_H_
#define _CL_EQUIPTRANSMAP_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>


/**
 *@brief EQUIPTRANSMAP data item
 */
class EquipTransMapDataEntryBase : public Avalon::DataEntry
{
public:
	EquipTransMapDataEntryBase();
	~EquipTransMapDataEntryBase();

public:
	virtual UInt32 GetKey() const { return iD; };

	bool Read(Avalon::DataRow& row);

	virtual bool InternalRead() { return true; };

	static UInt32 GetColNum() { return 4; };
public:
	//唯一ID
	UInt32	iD;
	//物品ID
	UInt32	itemId;
	//最小等级
	UInt32  minLev;
	//最大等级
	UInt32  maxLev;
	//作用部位
	std::vector<UInt32>	subTypes;
};

typedef Avalon::DataEntryVisitor<EquipTransMapDataEntryBase> EqTransMapDataEntryVisitor;

class EquipTransMapDataEntryMgr : public Avalon::DataEntryMgrBase<EquipTransMapDataEntryBase>
	, public Avalon::Singleton<EquipTransMapDataEntryMgr>
{

};

#endif