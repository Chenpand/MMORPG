#ifndef _CL_EQUIPRECOVERYPRICE_DATAENTRYBASE_H_
#define _CL_EQUIPRECOVERYPRICE_DATAENTRYBASE_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>


/**
 *@brief EQUIPRECOVERYPRICE data item
 */
class EquipRecoveryPriceDataEntryBase : public Avalon::DataEntry
{
public:
	EquipRecoveryPriceDataEntryBase();
	~EquipRecoveryPriceDataEntryBase();

public:
	virtual UInt32 GetKey() const { return iD; };

	bool Read(Avalon::DataRow& row);

	virtual bool InternalRead() { return true; };

	static UInt32 GetColNum() { return 4; };
public:
	//装备等级当id
	UInt32	iD;
	//蓝色品质
	std::string	blue;
	//紫色品质
	std::string	purple;
	//绿色品质
	std::string	pink;
};

class EquipRecoveryPriceDataEntry : public EquipRecoveryPriceDataEntryBase
{
public:
	EquipRecoveryPriceDataEntry() {};
	~EquipRecoveryPriceDataEntry() {};

public:
	virtual bool InternalRead();

public:
	std::map<UInt8, std::vector<UInt32>> prices;
};

typedef Avalon::DataEntryVisitor<EquipRecoveryPriceDataEntry> EqRecoveryPriceDataEntryVisitor;

class EqRecoveryPriceDataEntryMgr : public Avalon::DataEntryMgrBase<EquipRecoveryPriceDataEntry>
	, public Avalon::Singleton<EqRecoveryPriceDataEntryMgr>
{
public:
	UInt32 getEquipMaxPrice(UInt32 equipLv, UInt32 color);
};

#endif