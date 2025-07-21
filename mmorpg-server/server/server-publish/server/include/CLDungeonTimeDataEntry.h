#ifndef _CL_DUNGEON_TIME_DATAENTRY_H_
#define _CL_DUNGEON_TIME_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

//�data
class DungeonFinishTimeDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const{ return MakeKey(dungeonId, floor); }

	static UInt32 MakeKey(UInt32 dungeonId, UInt32 floor) { return dungeonId * 100 + floor; }

	bool Read(Avalon::DataRow& row);

public:
	// ���³�ID,���������֮���������
	UInt32		dungeonId;
	// ����
	UInt16		floor;
	// 29����Сʱ��
	UInt32		minTime_29;
	// 39����Сʱ��
	UInt32		minTime_39;
	// 49����Сʱ��
	UInt32		minTime_49;
	// 59����Сʱ��
	UInt32		minTime_59;
	// 69����Сʱ��
	UInt32		minTime_69;
};

typedef Avalon::DataEntryMgr<DungeonFinishTimeDataEntry> DungeonFinishTimeDataEntryMgr;

#endif
