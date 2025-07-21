#ifndef _CL_DUNGEON_VERIFY_PROB_DATAENTRY_H_
#define _CL_DUNGEON_VERIFY_PROB_DATAENTRY_H_

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief �ؿ���֤���ʱ�
*/
class DungeonVerifyProbDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow &row);

public:
	//ID
	UInt32	id;
	//����
	UInt32	prob;
};

typedef Avalon::DataEntryVisitor<DungeonVerifyProbDataEntry> DungeonVerifyProbDataEntryVisitor;
typedef Avalon::DataEntryMgr<DungeonVerifyProbDataEntry> DungeonVerifyProbDataEntryMgr;

#endif
