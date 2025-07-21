#ifndef _CL_FASHION_MERGE_CHANGE_ACTIVE_DATAENTRY_H_ 
#define _CL_FASHION_MERGE_CHANGE_ACTIVE_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include "CLItemDefine.h"

/**
*@brief 百变时装节时装合成模板表
*/
class FashionMergeChangeActiveDataEntry : public Avalon::DataEntry
{
public:
	FashionMergeChangeActiveDataEntry(){};
	~FashionMergeChangeActiveDataEntry(){};

public:
	virtual UInt32 GetKey() const { return id; };

	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 5; };
public:
	//时装id
	UInt32 id;
	//高级时装id
	UInt32 advanceId;
	//高级时装合成概率
	UInt32 pro;
	//货币消耗
	ItemReward goldConsume;
	//换装卷消耗
	ItemReward ticketConsume;
};

typedef Avalon::DataEntryVisitor<FashionMergeChangeActiveDataEntry> FashionMergeChangeActiveDataEntryVisitor;

class FashionMergeChangeActiveDataEntryMgr : public Avalon::DataEntryMgrBase<FashionMergeChangeActiveDataEntry>
	, public Avalon::Singleton<FashionMergeChangeActiveDataEntryMgr>
{

};

#endif