#ifndef _CL_GUILD_TABLE_DATAENTRY_H_
#define _CL_GUILD_TABLE_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include "CLItemDefine.h"

/**
*@brief ����Բ������
*/
struct GuildTableDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return (UInt32)type; }

	bool Read(Avalon::DataRow& row);

public:
	// ����(�Ƿ�Ԯ��)
	UInt8	type;
	// ÿ�մ���
	UInt32	maxTimes;
	// ����
	std::vector<ItemReward> rewards;
	// �ʼ�����ID
	UInt32	mailContentId;
};

typedef Avalon::DataEntryMgr<GuildTableDataEntry> GuildTableDataEntryMgr;

#endif
