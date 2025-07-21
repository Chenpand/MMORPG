#ifndef _CL_DUNGEON_DAILY_DROP_DATAENTRY_H_
#define _CL_DUNGEON_DAILY_DROP_DATAENTRY_H_

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <CLItemDefine.h>
#include <DropItemMgr.h>

/**
*@brief ���³�ÿ�յ����
*/
class DungeonDailyDropDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow &row);

public:
	// ID
	UInt32	id;
	// ÿ��ǰ���ε���
	UInt32	dailyNum;
	// ������Ʒ
	VDropItemElem	items;
};

typedef Avalon::DataEntryMgr<DungeonDailyDropDataEntry> DungeonDailyDropDataEntryMgr;

#endif
