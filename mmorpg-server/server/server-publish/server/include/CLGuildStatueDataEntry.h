#ifndef _CL_GUILD_STATUE_DATAENTRY_H_
#define _CL_GUILD_STATUE_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief �������������
*/
struct GuildStatueDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return (UInt32)type; }

	bool Read(Avalon::DataRow& row);

public:
	// ����
	UInt8	type;
	//���ĵ���ID
	UInt32	costItemId;
	//���ĵ�������
	UInt32	costItemNum;
	//��������
	UInt32	awardContri;
	//���BUFFER
	UInt32	bufferId;
};

typedef Avalon::DataEntryMgr<GuildStatueDataEntry> GuildStatueEntryMgr;

#endif
