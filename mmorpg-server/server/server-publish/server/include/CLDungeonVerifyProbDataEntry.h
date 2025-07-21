#ifndef _CL_DUNGEON_VERIFY_PROB_DATAENTRY_H_
#define _CL_DUNGEON_VERIFY_PROB_DATAENTRY_H_

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief 关卡验证概率表
*/
class DungeonVerifyProbDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow &row);

public:
	//ID
	UInt32	id;
	//概率
	UInt32	prob;
};

typedef Avalon::DataEntryVisitor<DungeonVerifyProbDataEntry> DungeonVerifyProbDataEntryVisitor;
typedef Avalon::DataEntryMgr<DungeonVerifyProbDataEntry> DungeonVerifyProbDataEntryMgr;

#endif
