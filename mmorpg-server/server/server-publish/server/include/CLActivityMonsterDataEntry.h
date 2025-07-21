#ifndef __ACTIVITY_MONSTER_DATA_ENTRY_H__
#define __ACTIVITY_MONSTER_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include "CLDropItemDefine.h"
#include "CLDungeonAreaDataEntry.h"

struct ActivityOpenTime
{
	UInt32 startHour;
	UInt32 startMin;
	UInt32 endHour;
	UInt32 endMin;
};

struct ActivityMonsterDropInfo
{
	UInt16 minLevel;
	UInt16 maxLevel;
	UInt32 dropSetId;
};

/**
*@brief ������
*/
class ActivityMonsterDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow &row);

public:
	// ID
	UInt32					id;
	// ����
	std::string				name;
	// ���������
	UInt32					startDate;
	// ���������
	UInt32					endDate;
	// ÿ�տ���ʱ��
	ActivityOpenTime		openTime;
	// ÿ�ֹ�������
	UInt32					perRollMonsterNum;
	// ÿ�ֳ���ʱ��(s)
	UInt32					perRollDurningSec;
	// ˢ�ֵ�����
	DungeonAreaPointType	pointType;
	// ˢ�ֱ�ID
	UInt32					groupId;
	// ����
	UInt32					prob;
	// ����
	std::vector<ActivityMonsterDropInfo>	drops;
	// ���ܵ���
	std::vector<DropItem>	mayDropItems;
	// ���ʼ��ʾ
	UInt32					startNotice;
	// ɱ����ʾ
	UInt32					killNotice;
	// ��ˢ����ʾ
	UInt32					clearNotice;
	// �������ʾ
	UInt32					overNotice;
	// ���³ǵȼ�Ҫ��(��С�ȼ�)
	UInt32					needDungeonLevel;
};

typedef Avalon::DataEntryVisitor<ActivityMonsterDataEntry> ActivityMonsterDataEntryVisitor;
typedef Avalon::DataEntryMgr<ActivityMonsterDataEntry> ActivityMonsterDataEntryMgr;

#endif