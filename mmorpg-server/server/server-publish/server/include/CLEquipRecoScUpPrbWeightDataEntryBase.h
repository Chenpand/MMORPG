#ifndef _CL_EQUIPRECOSCUPPRBWEIGHT_DATAENTRYBASE_H_
#define _CL_EQUIPRECOSCUPPRBWEIGHT_DATAENTRYBASE_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>


/**
 *@brief EQUIPRECOSCUPPRBWEIGHT data item
 */
class EquipRecoScUpPrbWeightDataEntryBase : public Avalon::DataEntry
{
public:
	EquipRecoScUpPrbWeightDataEntryBase();
	~EquipRecoScUpPrbWeightDataEntryBase();

public:
	virtual UInt32 GetKey() const { return iD; };

	bool Read(Avalon::DataRow& row);

	virtual bool InternalRead() { return true; };

	static UInt32 GetColNum() { return 3; };
public:
	//唯一id
	UInt32	iD;
	//提升积分
	UInt32	upScore;
	//概率权重
	UInt32	probWeight;
};

typedef Avalon::DataEntryVisitor<EquipRecoScUpPrbWeightDataEntryBase> EqRecoScUpPrbWeightDataEntryVisitor;

class EquipRecoScUpPrbWeightDataEntryMgr : public Avalon::DataEntryMgrBase<EquipRecoScUpPrbWeightDataEntryBase>
	, public Avalon::Singleton<EquipRecoScUpPrbWeightDataEntryMgr>
{

};


#endif