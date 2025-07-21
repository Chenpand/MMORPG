#ifndef _CL_DEATH_TOWER_AWARD_DATAENTRY_H_
#define _CL_DEATH_TOWER_AWARD_DATAENTRY_H_

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>


class DeathTowerAwardDataEntry : public Avalon::DataEntry
{
public:
	DeathTowerAwardDataEntry();
	~DeathTowerAwardDataEntry();

	UInt32 GetKey() const { return floor; }

	bool Read(Avalon::DataRow& row);

public:

public:
	UInt32			floor;
	UInt32			exp;
	UInt32			warriorSoul;
	UInt32			dropSet;
	UInt32			awardItemId;
	UInt32			awardItemNum;
	UInt16			levelLimit;
};

typedef Avalon::DataEntryMgr<DeathTowerAwardDataEntry> DeathTowerAwardDataEntryMgr;

#endif

