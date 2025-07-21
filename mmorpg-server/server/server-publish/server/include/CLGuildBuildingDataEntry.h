#ifndef _CL_GUILD_BUILDING_COST_DATAENTRY_H_
#define _CL_GUILD_BUILDING_COST_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <CLGuildDefine.h>

/**
*@brief ���Ὠ��������
*/
struct GuildBuildingDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return level; }

	bool Read(Avalon::DataRow& row);

public:
	// �ȼ�
	UInt8	level;
	// �����������⼶����
	UInt32	buildingCost[GUILD_BUILDING_NUM];
	// �̵�ID
	UInt8	shopId;
	// ���������ID
	UInt32	welfareGiftId;
};

typedef Avalon::DataEntryMgr<GuildBuildingDataEntry> GuildBuildingDataMgr;

#endif
