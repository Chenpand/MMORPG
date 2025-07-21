#ifndef _CL_DUNGEON_TIMES_DATAENTRY_H_
#define _CL_DUNGEON_TIMES_DATAENTRY_H_

#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include "CLDefine.h"
#include "CLVipDefine.h"
#include "CLDungeonDataEntry.h"

// ���³Ǵ���
class DungeonTimesDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const{ return (UInt32)type; }

	bool Read(Avalon::DataRow& row);

public:
	// ���³�������
	DungeonSubType		type;
	// ��������
	UInt8				baseTimes;
	// �����ɹ������
	UInt8				baseBuyTimes;
	// ���⹺�������VIP��Ȩ
	VipPrivilegeType	buyTimesVipPrivilege;
	// ����������ĵĻ���
	UInt32				buyTimesCostItemId;
	// ����������Ļ�������
	std::vector<UInt32> buyTimesCosts;
	// �Ѿ�����Ĵ���counter
	std::string			usedTimesCounter;
	// ���⹺��Ĵ���counter
	std::string			buyTimesCounter;
	// ÿ�ܴ���Ĵ���counter
	std::string			weekUsedTimesCounter;
	// ÿ�ܿɴ����
	UInt32				weekTimesLimit;
	// �˺�ÿ�տɴ����
	UInt32				accountDailyTimesLimit;
};

typedef Avalon::DataEntryMgr<DungeonTimesDataEntry> DungeonTimesDataEntryMgr;

#endif
