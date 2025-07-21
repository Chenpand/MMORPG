#ifndef _CL_EQUIPRECOVERYREWARD_DATAENTRYBASE_H_
#define _CL_EQUIPRECOVERYREWARD_DATAENTRYBASE_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>


/**
 *@brief EQUIPRECOVERYREWARD data item
 */
class EquipRecoveryRewardDataEntryBase : public Avalon::DataEntry
{
public:
	EquipRecoveryRewardDataEntryBase();
	~EquipRecoveryRewardDataEntryBase();

public:
	virtual UInt32 GetKey() const { return iD; };

	bool Read(Avalon::DataRow& row);

	virtual bool InternalRead() { return true; };

	static UInt32 GetColNum() { return 4; };
public:
	//奖励ID
	UInt32	iD;
	//积分参数
	UInt32	integral;
	//对应打开的罐子id
	UInt32	jarID;
};

typedef Avalon::DataEntryVisitor<EquipRecoveryRewardDataEntryBase> EqRecoRewardDataEntryVisitor;

class EquipRecoveryRewardDataEntryMgr : public Avalon::DataEntryMgrBase<EquipRecoveryRewardDataEntryBase>
	, public Avalon::Singleton<EquipRecoveryRewardDataEntryMgr >
{

};

#endif