#ifndef _CL_DUNGEON_GUARANTEE_DROP_DATAENTRY_H_
#define _CL_DUNGEON_GUARANTEE_DROP_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

//�data
class DungeonGuaranteeDropDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);

public:
	// id
	UInt32	id;
	// ���³�id
	UInt32	dungeonId;
	// ��С����
	UInt32	minTimes;
	// ������
	UInt32	maxTimes;
	// ���䷽��
	UInt32	dropSetId;
	// ��������
	UInt32	resetType;
	// VIP�ȼ���������
	UInt32	vipLevelLimitMin;
	// VIP�ȼ���������
	UInt32	vipLevelLimitMax;
};

//typedef Avalon::DataEntryMgr<DungeonGuaranteeDropDataEntry> DungeonGuaranteeDropDataEntryMgr;

class DungeonGuaranteeDropDataEntryMgr : public Avalon::DataEntryMgrBase<DungeonGuaranteeDropDataEntry>
	, public Avalon::Singleton<DungeonGuaranteeDropDataEntryMgr>
{
public:
	std::vector<DungeonGuaranteeDropDataEntry*> GetDataEntryByDungeonID(UInt32 dungeonID);
};

#endif
