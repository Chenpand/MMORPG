#ifndef _CL_EQUIPRECOSCUPCONSRTI_DATAENTRYBASE_H_
#define _CL_EQUIPRECOSCUPCONSRTI_DATAENTRYBASE_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>


/**
 *@brief EQUIPRECOSCUPCONSRTI data item
 */
class EquipRecoScUpConsRtiDataEntryBase : public Avalon::DataEntry
{
public:
	EquipRecoScUpConsRtiDataEntryBase();
	~EquipRecoScUpConsRtiDataEntryBase();

public:
	virtual UInt32 GetKey() const { return iD; };

	bool Read(Avalon::DataRow& row);

	virtual bool InternalRead() { return true; };

	static UInt32 GetColNum() { return 3; };
public:
	//唯一id
	UInt32	iD;
	//次数区间
	std::vector<std::string>	timesSection;
	//系数
	UInt32	ratio;
};

class EquipRecoScUpConsRtiDataEntry : public EquipRecoScUpConsRtiDataEntryBase
{
public:
	EquipRecoScUpConsRtiDataEntry() {};
	~EquipRecoScUpConsRtiDataEntry() {};

public:
	virtual bool InternalRead();

public:
	//次数区间
	std::vector<Int32>  timesSpans;
};

typedef Avalon::DataEntryVisitor<EquipRecoScUpConsRtiDataEntry> EqRecoScUpConsRtiDataEntryVisitor;

class EquipRecoScUpConsRtiDataEntryMgr : public Avalon::DataEntryMgrBase<EquipRecoScUpConsRtiDataEntry>
	, public Avalon::Singleton<EquipRecoScUpConsRtiDataEntryMgr>
{
public:
	//UInt32 getRatio(UInt32 times);
};

#endif