#ifndef _CL_GUILD_DATAENTRY_H_
#define _CL_GUILD_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <CLGuildDefine.h>

/**
*@brief ����������
*/
struct GuildDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return level; }

	bool Read(Avalon::DataRow& row);

public:
	//����ȼ�
	UInt8	level;
	//��Ա����
	UInt32	memberNum;
	//��������ߵȼ�
	UInt8	buildingMaxLevel[GUILD_BUILDING_NUM];
};

typedef Avalon::DataEntryMgr<GuildDataEntry> GuildDataEntryMgr;

#endif
